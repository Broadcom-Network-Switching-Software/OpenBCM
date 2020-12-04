/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : IFA [ SDK - 6.5.19 and CANCUN - 6.3.3 ]
 *
 * Usage    : BCM.0> cint td3_ifa_transit.c
 *
 * config   : td3_ifa_init_transit_config.bcm
 *
 * Log file : td3_ifa_transit_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |                   |
 *        egress_ port  |        TD3        |  ingress_port
 *       ----<----------+                   +<----------
 *                      |                   |
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate IFA transit flow
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port and egress port configure them in Loopback mode.
 *     b) Do the necessary IFA transit configurations.
 *        - IFP is configured to qualify IFA packets and add the action of
 *          IntEncapEnable.
 *        - EVXLT is programmed with the metadata elements
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ===============================================
 *     a) Send the packet that matches the description through ingress port
 *        and observe the packet that egresses
 *
 *     b) Expected Result:
 *     ===================
 *     The egress packet should have metadata of the transit node appended[32 B]
 */

cint_reset();
bcm_port_t ingress_port;
bcm_port_t egress_port;
uint32 ifa20_proto = 253; /* Experimental protocol number */
uint32 req_vec = 0x33;
uint32 lns = 0x1;
uint8 ver2 = 1;
uint8 hop_limit = 0;
uint8 cur_len = 0;

bcm_mac_t dmac = "00:00:00:00:22:22";
bcm_vlan_t vlan = 21;
bcm_gport_t gport_egress = BCM_GPORT_INVALID;

/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
checkPortAssigned(int *port_index_list,int no_entries, int index)
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
 *    Provides required number of ports
 */

int
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &configP));

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
        index = sal_rand() % port;
        if (checkPortAssigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

/*
 *    Global IFA configuration
 */

int
ifa_global_setting(int unit)
{
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2Enable, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2HeaderType, ifa20_proto));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataFirstLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataSecondLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFAMetaDataAdaptTable, BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE));

    return BCM_E_NONE;
}

/*
 *     Create vlan and add port to vlan
 */

int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t in_port, bcm_port_t egr_port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, upbmp;

    rv = bcm_vlan_create(unit, vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create() for vlan %d: %s.\n", vlan, bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    BCM_PBMP_PORT_ADD(pbmp, egr_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, upbmp));

    return BCM_E_NONE;
}

/*
 *    Steps to configure switching
 */

int
switch_config(int unit)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2_addr;

    rv = create_vlan_add_port(unit, vlan, ingress_port, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, dmac, vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port  = egress_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE(rv)) {
        printf("Error in l2 addr add %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    IFA FP configuration
 */

int
ifa_ifp(int unit)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;
    bcm_error_t rv;

    bcm_field_group_config_t_init(&group_config);

    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_ASET_INIT(group_config.aset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyPktType);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIfaRequestVector);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIfaLocalNameSpace);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIfaVersion2);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIfaHopLimitEqualToZero);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyIfaCurrentLengthEqualToZero);

    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionIntEncapEnable);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ingress_port, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry, bcmFieldPktTypeUdpIfa));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaRequestVector(unit, entry, req_vec, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaLocalNameSpace(unit, entry, lns, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaVersion2(unit, entry, ver2, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaHopLimitEqualToZero(unit, entry, hop_limit, -1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IfaCurrentLengthEqualToZero(unit, entry, cur_len, -1));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionIntEncapEnable, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 *    IFA EVXLT configuration
 */

int
ifa_evxlt(int unit)
{
    bcm_flow_encap_config_t einfo;
    uint32 flow_handle_md = 0;
    bcm_flow_option_id_t encap_flow_option = 0;
    bcm_flow_logical_field_t lfield[3];
    bcm_error_t rv;

    rv = bcm_port_gport_get(unit, egress_port, gport_egress);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_gport_get() %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Configuring Metadata info on EVXLT1 table
     * DGPP is used to lookup EVXLT1
     */
    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IFA", &flow_handle_md));

    /* Adapt_1 single Lookup 1 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_md, "IFA_MD_LOOKUP_1_ADAPT1_SINGLE", &encap_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_md;
    einfo.flow_option = encap_flow_option;
    einfo.flow_port = gport_egress;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_md, "OPAQUE_DATA", &lfield[0].id));
    lfield[0].value = 0xbbbbb;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_md, "LNS", &lfield[1].id));
    lfield[1].value = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_md, "DEVICE_ID", &lfield[2].id));
    lfield[2].value = 0xCCCCC;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));

    /* Configure Adapt1 single Lookup2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_md, "IFA_MD_LOOKUP_2_ADAPT1_SINGLE", &encap_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_md;
    einfo.flow_option = encap_flow_option;
    einfo.flow_port = gport_egress;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_md, "MMU_STAT_0", &lfield[0].id));
    lfield[0].value = 0x900d5eed;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_md, "MMU_STAT_1", &lfield[1].id));
    lfield[1].value = 0x900dbead;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 2, lfield));

    return BCM_E_NONE;
}

/*
 *    Timing configuration for timestamps
 */

int
time_config(int unit)
{
    bcm_time_init(unit);
    bcm_time_interface_t intf;
    bcm_error_t rv;

    bcm_time_interface_t_init(&intf);
    intf.id = 0;
    intf.flags |= BCM_TIME_ENABLE;

    rv = bcm_time_interface_add(unit, intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in time interface add %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_time_ts_counter_t counter;
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;

    bcm_time_ts_counter_get(unit, &counter);
    stages = BCM_TIME_STAGE_EGRESS;
    tod.time_format = counter.time_format;
    tod.ts_adjustment_counter_ns = counter.ts_counter_ns;

    /* Get seconds from the timestamp value */
    secs = counter.ts_counter_ns;
    COMPILER_64_SET(div, 0, 1000000000);
    COMPILER_64_UDIV_64(secs, div);
    tod.ts.seconds = secs;

    /* Get nanoseconds from the timestamp value */
    nano_secs = counter.ts_counter_ns;
    COMPILER_64_UMUL_32(secs, 1000000000);
    COMPILER_64_SUB_64(nano_secs, secs);
    tod.ts.nanoseconds = COMPILER_64_LO(nano_secs);

    rv = bcm_time_tod_set(unit, stages, &tod);
    if (BCM_FAILURE(rv)) {
        printf("Error in tod set %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the feature and the required set up to verify it
 */

int test_setup(int unit)
{
    int port_list[2];
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    bcm_error_t rv;

    rv = portNumbersGet(unit, port_list, 2);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    printf("Choosing port %d as ingress port and %d as egress port\n", ingress_port, egress_port);

    /* Set port mac loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, lb_mode));

    rv = ifa_global_setting(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA global settings %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_ifp(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA FP configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = switch_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in switching configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_evxlt(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA EVXLT configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = time_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in time config %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Transmit the below packet on ingress_port and verify the functionality of
 *    the feature
 */

int test_verify(int unit)
{
    char str[512];

    snprintf(str, 512, "port %d discard=all", egress_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", egress_port);
    bshell(unit, str);
    bshell(unit,"pw start");
    bshell(unit,"pw report +raw");

    /* Send a packet to ingress port*/
    printf("Packet comes into ingress port %d\n", ingress_port);

    snprintf(str, 512, "tx 1 pbm=%d    data=00000000222200000000111181000015080045000092000000000afd975b0b0b01000a0a02002f1100ff1f001f690056d8cd33223f081ddddd0a00000000005f005b009795fc3b9acc9c000aaaaa900ddeed900dfeed000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d; sleep 5", ingress_port);

    printf("%s\n", str);
    bshell(unit, str);

    return BCM_E_NONE;
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int test_execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
        return -1;
    }
    printf("Completed test setup successfully.\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("testVerify() failed\n");
        return -1;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print test_execute();
}
