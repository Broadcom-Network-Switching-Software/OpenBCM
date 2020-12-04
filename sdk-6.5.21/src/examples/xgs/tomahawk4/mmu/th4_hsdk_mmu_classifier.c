/*
 * Feature  : classifier
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_classifier.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_classifier_log.txt
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
 * This script can verify below cosq APIs:
 * ========
 *    bcm_cosq_classifier_create
 *    bcm_cosq_classifier_destroy
 *    bcm_cosq_classifier_get
 *    bcm_cosq_classifier_mapping_clear
 *    bcm_cosq_classifier_mapping_multi_get
 *    bcm_cosq_classifier_mapping_multi_set
 *
 * Summary:
 * ========
 * This cint example demonstrate
 *  1) Creating a classifier profile
 *  2) Modifying the mapping of the created profile
 *  3) Use IFP to assign the profile to matched traffic
 *  4) Clearing the mapping and destroying the profile.
 *
 *  Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select two ports
 *      b) Set up L2 switching path between the ports
 *
 *    ==========================================
 *    2) Step2 - Configuration (Done in test_setup()):
 *    ==========================================
 *      c) Create a classifier profile
 *      d) Set the profile map
 *      e) Configure IFP to assign to the classifier profile
 *      d) Set up flex counter per queue
 *
 *    ==========================================
 *    3) Step3 - Verification (Done in test_verify()):
 *    ==========================================
 *     Expected Result:
 *      Transmit a packet to the ingress port and create VLAN, with priority 0,
 *      expect the packet to egress on the queue mapped by classifier
 *
 *    ==========================================
 *    4) Step4 - Cleanup (Done in test_cleanup())
 *    ==========================================
 *      a) Delete classifier profile
 *      b) Remove the per queue flex counter
 *      c) Delete L2 forwarding path
 *      d) Delete the IFP rule
 */

cint_reset();

int unit = 0;

bcm_cosq_classifier_t classifier, out_classifier;
int classifier_id = -1;
int TC_NUM = 16;
int array_count = TC_NUM*3;
bcm_cos_t priority_array[TC_NUM*3] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
bcm_cos_t cosq_array[TC_NUM*3] = {2,6,5,4,3,2,1,3,4,3,2,1,1,2,3,4,
                                  2,1,2,3,3,1,2,3,3,1,2,3,3,1,2,3,
                                  2,1,2,3,3,1,2,3,3,1,2,3,3,1,2,3};

/* These two variables are acutually not used by APIs, dumy parameters */
bcm_gport_t port = 0;
bcm_gport_t queue_group = 0;

bcm_cos_t out_priority_array[TC_NUM*3] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                          0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                          0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
bcm_cos_t out_cosq_array[TC_NUM*3] = {0};

bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_vlan_t vid = 2;
uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
uint32_t stat_counter_id;
bcm_flexctr_counter_value_t counter_value;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list_ptr, int num_ports)
{
    int i = 0, port = 0, rv = 0;
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

    if ((port == 0 ) || ( port < num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    /* generating port list */
    for (i= 0; i < num_ports; i++) {
        port_list_ptr[i] = tempports[i];
    }

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

int
ifp_setup(int unit)
{
    int port_data, mod_data, port_mask, mod_mask;
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;
    int rv;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_ASET_INIT(group_config.aset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifySrcPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCosMapNew);
    group_config.priority = 1;
    rv = bcm_field_group_config_create(unit, &group_config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_group_config_create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    print group_config.group;
    entry = 1;
    print bcm_field_entry_create(unit, group_config.group, &entry);
    mod_data = 0;
    mod_mask = -1;
    port_data = ingress_port;
    port_mask = -1;

    bcm_field_qualify_SrcPort(unit, entry, mod_data, mod_mask, port_data, port_mask);
    bcm_field_action_add(unit, entry, bcmFieldActionCosMapNew, classifier_id, 0);
    rv = bcm_field_entry_install(unit, entry);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_install: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

int
ifp_cleanup(int unit)
{
    return (bcm_field_init(unit));
}

int classifier_create(int unit)
{
    int rv = BCM_E_NONE;

    bcm_cosq_classifier_t_init(&classifier);
    classifier.flags = BCM_COSQ_CLASSIFIER_FIELD;
    rv = bcm_cosq_classifier_create(unit, &classifier, &classifier_id);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_classifier_create FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Created classifier ID: 0x%x\n", classifier_id);

    bcm_cosq_classifier_t_init(&out_classifier);
    rv = bcm_cosq_classifier_get(unit, classifier_id, &out_classifier);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_classifier_get FAILED. rv %d\n", rv);
        return rv;
    }

    if (out_classifier.flags != classifier.flags) {
        printf("ERR. Retrieved classifier is not same as created one\n");
        return -1;
    }

    return rv;
}

int classifier_mapping_set(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_cosq_classifier_mapping_multi_set(unit, port, classifier_id, queue_group,
                                    array_count, &priority_array, &cosq_array);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_classifier_mapping_multi_set FAILED. rv %d\n", rv);
        return rv;
    }

    rv = bcm_cosq_classifier_mapping_multi_get(unit, port, classifier_id, &queue_group,
                       TC_NUM*3, &out_priority_array, &out_cosq_array, &array_count);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_classifier_mapping_multi_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Printing after bcm_cosq_classifier_mapping_multi_get\n");

    print out_priority_array;
    print out_cosq_array;

    return BCM_E_NONE;
}

int classifier_mapping_del(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_cosq_classifier_mapping_clear(unit, port, classifier_id);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_classifier_mapping_clear FAILED. rv %d\n", rv);
        return rv;
    }

    rv = bcm_cosq_classifier_destroy(unit, classifier_id);
    if (rv != BCM_E_NONE) {
        printf("ERR. bcm_cosq_classifier_destroy FAILED. rv %d\n", rv);
        return rv;
    }

    rv = bcm_cosq_classifier_mapping_multi_get(unit, port, classifier_id, &queue_group,
                                    TC_NUM*3, &out_priority_array, &out_cosq_array,
                                    &array_count);
    if (rv != BCM_E_NOT_FOUND) {
        printf("ERR. bcm_cosq_classifier_mapping_multi_get FAILED. rv %d\n", rv);
        return rv;
    }
    printf("Classifier mapping was not found after mapping_clear as expected\n");

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];
    int rv = BCM_E_NONE;

    printf("==> Set up classifier ...\n");

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }
    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Ingress port : %d\n", ingress_port);
    printf("Egress port: %d\n", egress_port);

    /* Set up L2 forwarding */
    if (BCM_E_NONE != l2_setup(unit)) {
        printf("l2_setup() failed.\n");
        return -1;
    }

    /* Creating a classifier profile */
    if (BCM_FAILURE((rv = classifier_create(unit)))) {
        printf("classifier_create() failed.\n");
        return -1;
    }

    /* Modifying the mapping of the created profile,
       clearing the mapping and destroying the profile */
    if (BCM_FAILURE((rv = classifier_mapping_set(unit)))) {
        printf("classifier_mapping_set() failed.\n");
        return -1;
    }

    /* Creating a classifier profile */
    if (BCM_FAILURE((rv = ifp_setup(unit)))) {
        printf("ifp_setup() failed.\n");
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

    printf("==> Verify unicast pkt from port %d forwarding to port %d\n", ingress_port, egress_port);

    /* Send a packet with priority 0 to ingress port, the IFP assign the packet to
     * new COS queue based on the classifier */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_ALL));
    snprintf(str, 256, "tx 1 pbm=%d data=0x00000000001100000000002281000002FFFF000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DDFC5C3CB; sleep quiet 1", ingress_port);
    bshell(unit, str);

    BCM_IF_ERROR_RETURN(flex_ctr_check(unit, cosq_array[0]));

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

    if (BCM_E_NONE != ifp_cleanup(unit)) {
        printf("ifp_cleanup() failed.\n");
        return -1;
    }

    /* Delete classifier */
    if (BCM_E_NONE != classifier_mapping_del(unit)) {
        printf("classifier_mapping_del() failed.\n");
        return -1;
    }

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

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; attach; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
         printf("test_setup() failed.\n");
         return -1;
    }

    /* NULL function here as check already done in appl_classifier_tests */
    if (BCM_FAILURE((rv = test_verify(unit)))) {
         printf("test_verify() failed.\n");
         return -1;
    }

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

