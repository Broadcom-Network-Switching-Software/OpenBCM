/*
 * Feature  : Flex counter to demonstrate usage of ingress packet attribute
 *
 * Usage    : BCM.0> cint th4_hsdk_flexctr_ingress_packet_attribute.c
 *
 * Config   : th4_hsdk_flexctr_64x400.config.yml
 *
 * Log file : th4_hsdk_flexctr_ingress_packet_attribute_log.txt
 *
 * Test Topology :
 *                      +----------------------+
 *                      |                      |
 *                      |        TH4           |
 *        ingress_port  |                      |  egress_port
 *       ------------->-+  Flexctr             +---------->----
 *                      |   Src: IngPort       |
 *                      |   Obj: IngPktAttr    |
 *                      |                      |
 *                      |                      |
 *                      +----------------------+
 *
 *
 * Summary:
 * ========
 * cint script which shows counting of tagged packets and packets on different
 * ingress ports
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port and egress port; configure them in loopback mode
 *     b) Necessary flex counter configurations are done.
 *        1. The ingress packet attribute of tag type and ingress logical port
 *           are selected as the objects
 *           the object
 *        2. Packets are counted on Ingress port [source]
 *
 *     Note: IFP configuration on egress port is meant for a verification
 *           purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packet that matches the description [Packets with vlan tag
 *        and untagged packets on any ingress port]
 *
 *     b) Expected Result:
 *     ===================
 *     The number of sent packets should match the flex ctr stat get output
 */

/* Reset C interpreter*/

cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
uint32_t stat_counter_id;
const int MAX_LOGICAL_PORT = 270;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
check_assigned_port(int *port_index_list,int no_entries, int index)
{
    int i=0;

    for (i=0; i < no_entries; i++) {
        if (port_index_list[i] == index)
        return 1;
    }

    /* no entry found */
    return 0;
}

/*
 *    Provides required number of ports
 */

int
port_numbers_get(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, index = 0;
    bcm_port_config_t port_config;
    bcm_pbmp_t ports_pbmp;
    int temp_ports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));

    ports_pbmp = port_config.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            temp_ports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("Eror in portNumbersGet \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (check_assigned_port(port_index_list, i, index) == 0) {
            port_list[i] = temp_ports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode
 */

int
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_port_t port;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode and installs an IFP rule. This IFP
 *    rule drops the packets ingressing on the specified port to CPU. This is to
 *    avoid continuous loopback of the packet from the egress port.
 */

int
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t port;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*    Configures the flex counters to count on tagged and untagged packets
 *    ingressing on all the front panel ports and thus demonstrates the ingress
 *    packets attributes feature of flex counters
 */

int
configure_flex_counter_ing_pkt_attr(int unit)
{
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
	bcm_flexctr_index_operation_t *index_op = NULL;
	bcm_flexctr_value_operation_t *value_a_op = NULL;
	bcm_flexctr_value_operation_t *value_b_op = NULL;

	/* Create a flex counter packet attribute profile with vlan tag*/

    bcm_flexctr_packet_attribute_profile_t attr_profile;
    bcm_flexctr_packet_attribute_selector_t *attr_selectors = NULL;
    bcm_flexctr_packet_attribute_map_t attr_map;
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entries = NULL;
    int options = 0, i = 0;
    int profile_id;
    uint32 offset_value;

    bcm_flexctr_packet_attribute_profile_t_init(&attr_profile);

    attr_profile.num_selectors = 1;
    attr_selectors = attr_profile.attr_selectors;

    attr_selectors[0].attr_type = bcmFlexctrPacketAttributeTypeIngVlanTag;
    attr_selectors[0].attr_mask = 0x1;

    rv = bcm_flexctr_packet_attribute_profile_create (unit, options, &attr_profile, &profile_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_packet_attribute_profile_create FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add offset/object entry to created packet
     * attribute map profile.Packet attribute offset
     * configurations: vlantag = 0, 1
     */

    bcm_flexctr_packet_attribute_map_t_init(&attr_map);
    attr_map.num_entries = attr_profile.num_selectors;
    attr_map_entries = attr_map.attr_map_entries;

    for (i=0; i<2 ; i++) {
        attr_map_entries[0].attr_type = bcmFlexctrPacketAttributeTypeIngVlanTag;
        attr_map_entries[0].attr_value = i;
        offset_value = i;
        rv = bcm_flexctr_packet_attribute_map_add (unit, profile_id, &attr_map,
                                                    offset_value);
        if (BCM_FAILURE(rv)) {
            printf("bcm_flexctr_packet_attribute_map_add FAILED: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.source = bcmFlexctrSourceIngPort;
    action.mode = bcmFlexctrCounterModeNormal;
    action.drop_count_mode = bcmFlexctrDropCountModeAll;
    action.index_num = MAX_LOGICAL_PORT * 2; /* 2 counters [1: untagged, 2: tagged] per port */

    /* Objects for counter index is ingress packet attribute and logical port */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticIngPktAttribute;
    index_op->mask_size[0] = 1;
    index_op->shift[0] = 0;
    index_op->object_id[0] = profile_id;
    index_op->object[1] = bcmFlexctrObjectStaticIngIngressPort;
    index_op->mask_size[1] = 15;
    index_op->shift[1] = 1;


    /* Increase counter per packet. */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 15;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_flexctr_action_create() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("stat_counter_id == %d \r\n", stat_counter_id);

    return BCM_E_NONE;
}

/*
 *    Configures the flex counter feature and the required set up to verify it
 */

int test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_port_config_t cfg;

    rv = port_numbers_get(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Choosing port %d as ingress port and %d as egress port\n", ingress_port, egress_port);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    rv = ingress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in ingress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    rv = egress_port_multi_setup(unit, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in egress_port_multi_setup %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*    Configure flex counters to count on tagged packets for diff ingress port
     *    [ingress packet attribute]
     */
    rv = configure_flex_counter_ing_pkt_attr(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter_egr_pkt_attr %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_config_get(unit, &cfg);
	if (BCM_FAILURE(rv)) {
        printf("bcm_port_config_get() FAILED: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    pbmp = cfg.port;

    BCM_PBMP_ITER(pbmp, port) {
        rv = bcm_port_stat_attach(unit, port, stat_counter_id);
        if (BCM_FAILURE (rv)) {
            printf ("bcm_port_stat_attach() FAILED for %d: %s\n", port, bcm_errmsg (rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Flex counter feature Verification */
int
test_verify(int unit)
{
    char str[512];
    int ctr_idx;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    bcm_error_t rv;
    int i;
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }
    bcm_vlan_t vlan=5;

    /*    Vlan creation and adding of ports is for verification of tagged packet
     *    attribute and this specific vlan is for verification purpose and can
     *    be any vlan
     */
    snprintf(str, 512, "vlan create %d pbm=%d,%d", vlan, ingress_port, egress_port);
    bshell(unit, str);

    printf("\nSending %d tagged packets to %d ingress port \n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d vlan=%d ", count, ingress_port, vlan);
    bshell(unit, str);
    sal_sleep(5);

    printf("\nSending %d untagged packets to %d ingress port \n", count+2, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d untagged=yes", count+2, ingress_port);
    bshell(unit, str);
    sal_sleep(5);

    /* Get counter value. */
    ctr_idx = ingress_port;

    uint32 counter_index[2] = {(ctr_idx * 2), (ctr_idx * 2) +1};

    for(i=0 ; i<2; i++) {
        sal_memset(&counter_value, 0, sizeof(counter_value));
        rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index[i], &counter_value);
        if (BCM_FAILURE(rv)) {
          printf("bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
          return rv;
        }

        /* print counter value */
        if (i==0)
            printf("Untagged packets for port %d : %d packets / %d bytes\n", ingress_port,
                                            COMPILER_64_LO(counter_value.value[0]),
                                            COMPILER_64_LO(counter_value.value[1]));
        else if(i==1)
            printf("Tagged packets for port %d : %d packets / %d bytes\n", ingress_port,
                                            COMPILER_64_LO(counter_value.value[0]),
                                            COMPILER_64_LO(counter_value.value[1]));
    }

    return BCM_E_NONE;
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; attach ; version");

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("test_setup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("\nCompleted test setup successfully\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("test_verify() failed\n");
        return -1;
    }
    printf("\nCompleted test verify successfully\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
