/*
 * Feature  : FP class id
 *
 * Usage    : BCM.0> cint th4_hsdk_vfp_ifp_efp_classid.c
 *
 * config   : th4_hsdk_fp_64x400.config.yml
 *
 * Log file : th4_hsdk_vfp_ifp_efp_classid_log.txt
 *
 * Test Topology :
 *                      +--------------------+
 *                      |                    |
 *                      |       TH4          |
 *         ingress_port |                    |  egress_port
 *        --------->----+    class id        +------->------
 *             vlan 2   |                    |  vlan 30
 *                      |                    |
 *                      |                    |
 *                      +--------------------+
 *
 *
 * Summary:
 * ========
 * Cint example to show carrying of class-id from VFP to IFP and then from IFP
 * to EFP using BCM APIs.
 *
 * The following example illustrates the 12-tuple fields of OpenFlow being
 * qualified across VFP,IFP and EFP TCAM each coming up in Single mode. A stat
 * is attached in the EFP to count the number of packets that matches the
 * corresponding flow and also change to new outer vlan.
 *
 * VFP Entry
 *     Qualifier 1: InPort                                 - port 35
 *     Qualifier 2: Source Mac Address                     - 0x1
 *     Qualifier 3: Destination Mac Address                - 0x2
 *     Qualifier 4: Ether Type                             - 0x0800
 *     Qualifier 5: Outer Vlan Id                          - 0x5
 *     Qualifier 6: Outer Vlan Priority                    - 0x2
 *     Qualifier 7: Ip Protocol                            - 0x6 [TCP]
 *     Qualifier 8: DSCP                                   - 0x18
 *     Action     : Assign Upper 10 bits of VFP Class id   - 0x32
 *
 * IFP Entry
 *     Qualifier 1: Source IP Address                      - 10.10.10.40
 *     Qualifier 2: Destination IP Address                 - 192.168.01.20
 *     Qualifier 3: Source Class Id assined in VFP         - 0x32
 *     Action     : Assign new Class id to qualify in EFP  - 0x64
 *
 * EFP Entry
 *     Qualifier 1: L4 Source Port                         - 0x1404
 *     Qualifier 2: L4 Destination Port                    - 0x1AE8
 *     Qualifier 3: Ingress Class Id assined in IFP        - 0x64
 *     Action 1   : Assign new Outer Vlan                  - 0x6
 *     Action 2   : Count the packets
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress port and the other as egress
 *        port.
 *     b) Install an IFP rule to copy egress packets to CPU and also drop them.
 *        start packet watcher.
 *     c) The script configures entries in VFP, IFP and EFP with the qualifiers
 *        and actions as listed above.
 *
 *     Note: IFP rule in step b is meant for a verification purpose and is not
 *           part of the feature configuration.
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ==============================================
 *     a) Check the configurations by 'fp show',
 *     b) Transmit the packet. The packet used matches the configured script.
 *     c) Expected Result:
 *     ===================
 *       We see that the outer vlan of the packet has changed and the number of
 * 		 sent packets should match the flex ctr stat get output.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t egress_port;
bcm_port_t ingress_port;
bcm_vlan_t ing_vid = 5;
bcm_vlan_t egr_vid = 30;
uint32_t stat_counter_id, flexctr_obj_val = 0;
bcm_field_group_t efp_group;
bcm_field_entry_t efp_entry;

uint32 vfp_source_class_id = 50;
uint32 ifp_source_class_id = 100;

/* Qualifier set for VFP TCAM */
int vfp_qset[] = {
    bcmFieldQualifyStageLookup,
    bcmFieldQualifyInPort,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlanId,
    bcmFieldQualifyOuterVlanPri,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    -1,
};

/* Qualifier set for IFP TCAM */
int ifp_qset[] = {
    bcmFieldQualifyStageIngress,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifySrcClassField,
    -1,
};

/* Qualifier set for EFP TCAM */
int efp_qset[] = {
    bcmFieldQualifyStageEgress,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyIngressClassField,
    -1,
};

/* VFP configuration structure */
struct vfp_cfg_t{
    bcm_port_t inport;
    bcm_port_t inport_mask;
    bcm_mac_t src_mac;
    bcm_mac_t src_mask;
    bcm_mac_t dst_mac;
    bcm_mac_t dst_mask;
    uint16 ether_type;
    uint16 ether_type_mask;
    bcm_vlan_t vlan_id;
    bcm_vlan_t vlan_mask;
    uint8 vlan_pri;
    uint8 vlan_pri_mask;
    uint8 ip_protocol;
    uint8 ip_protocol_mask;
    uint8 dscp;
    uint8 dscp_mask;
    uint32 vfp_src_cls_id;
};

vfp_cfg_t vfp_cfg = {
    ingress_port,
    0xFF,
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    0x0800,
    0xFFFF,
    ing_vid,
    0xFF,
    0x2,
    0xF,
    0x06,
    0xFF,
    0x18,
    0xFF,
    vfp_source_class_id,
};

/* IFP configuration structure */
struct ifp_cfg_t{
    bcm_ip_t sip;
    bcm_ip_t sip_mask;
    bcm_ip_t dip;
    bcm_ip_t dip_mask;
    uint32 vfp_src_cls_id;
    uint32 vfp_src_cls_mask;
    uint32 ifp_src_cls_id;
};

ifp_cfg_t ifp_cfg = {
    0x0A0A0A28,
    0xFFFFFFFF,
    0xC0A80114,
    0xFFFFFFFF,
    vfp_source_class_id,
    0xFFFFFFFF,
    ifp_source_class_id,
};

/* EFP configuration structure */
struct efp_cfg_t{
    bcm_field_group_mode_type_t gmode;
    bcm_field_group_packet_type_t ptype;
    bcm_l4_port_t l4_src_port;
    bcm_l4_port_t l4_src_port_mask;
    bcm_l4_port_t l4_dst_port;
    bcm_l4_port_t l4_dst_port_mask;
    uint32 ifp_src_cls_id;
    uint32 ifp_src_cls_mask;
    bcm_vlan_t outer_vlan_new;
};

efp_cfg_t efp_cfg ={
    bcmFieldGroupModeTypeL3Single,
    bcmFieldGroupPacketTypeIp4,
    0x1404,
    0xFFFF,
    0x1AE8,
    0xFFFF,
    ifp_source_class_id,
    0xFFFFFFFF,
    egr_vid
};



/*
 *    Checks if given port/index is already present
 *    in the list so that same port number is not generated again
 */

int
check_port_assigned(int *port_index_list,int no_entries, int index)
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
port_numbers_get(int unit, int *port_list, int num_ports)
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
        printf("port_numbers_get() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand() % port;
        if (check_port_assigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
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
    bcm_port_t     port;

    BCM_PBMP_ITER(pbm, port) {
        printf("Ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    }

    return BCM_E_NONE;
}

/*
 *    Configures the port in loopback mode and installs an IFP rule. This IFP
 *    rule copies the packets ingressing on the specified port to CPU. It is
 *    also configured to drop the packets on that port. This is to avoid
 *    continuous loopback of the packet from the egress port.
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
        printf("Egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 5, 5));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

int test_failed = 0; /*Final result will be stored here */
int rx_count;   /* Global received packet count */

/*    Expected Egress packets     */

unsigned char expected_egress_packet[68] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00, 0x40, 0x1E,
        0x08, 0x00, 0x45, 0x18, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x40, 0x06, 0xA4, 0xC4, 0x0A, 0x0A,
        0x0A, 0x28, 0xC0, 0xA8, 0x01, 0x14, 0x14, 0x04, 0x1A, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0xA4, 0xFB, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
        0x64, 0x0D, 0x7C, 0x14
};

/*    Rx callback function for applications using the HSDK PKTIO    */

bcm_pktio_rx_t
pktio_Rx_call_back(int unit, bcm_pktio_pkt_t * packet, void *cookie)
{
    int *count = (auto) cookie;
    int result_packet_check = -1;
    int result_vlan_check = -1;
    void *buffer;
    uint32 length;
    uint32 vlan;

    (*count)++; /* Bump received packet count */

    /* Get basic packet info */
    if (BCM_FAILURE
        (bcm_pktio_pkt_data_get(unit, packet, (void *) &buffer, &length))) {

        return BCM_PKTIO_RX_NOT_HANDLED;
    }

    /* Get vlan metadata */
    BCM_IF_ERROR_RETURN(bcm_pktio_pmd_field_get
                        (unit, packet, bcmPktioPmdTypeRx,
                         BCM_PKTIO_RXPMD_OUTER_VID, &vlan));

    result_packet_check = sal_memcmp(buffer, expected_egress_packet, length);

    if(vlan != egr_vid) {
        result_vlan_check = 0;
    }

    test_failed = test_failed || (result_packet_check != 0) || (result_vlan_check == 0);

    return BCM_PKTIO_RX_NOT_HANDLED;
}

/*    Register callback function for received packets    */

int
register_pktioRx_callback(int unit)
{
    const uint8 priority = 101;
    const uint32 flags = 0;      /* Flags only used on match (for now) */

    return bcm_pktio_rx_register
      (unit, "rx_cb", pktio_Rx_call_back, priority, &rx_count, flags);
}

int
configure_vfp(int unit)
{
    bcm_field_entry_t entry ;
    bcm_field_group_config_t group_config;
    bcm_field_qualify_t *qset = vfp_qset;

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ingress_port,vfp_cfg.inport_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcMac(unit, entry, vfp_cfg.src_mac, vfp_cfg.src_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, vfp_cfg.dst_mac, vfp_cfg.dst_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_EtherType(unit, entry, vfp_cfg.ether_type, vfp_cfg.ether_type_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vfp_cfg.vlan_id, vfp_cfg.vlan_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanPri(unit, entry, vfp_cfg.vlan_pri, vfp_cfg.vlan_pri_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, entry, vfp_cfg.ip_protocol, vfp_cfg.ip_protocol_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DSCP(unit, entry, vfp_cfg.dscp, vfp_cfg.dscp_mask));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionClassSourceSet, vfp_cfg.vfp_src_cls_id, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

int
configure_ifp(int unit)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;
    bcm_field_qualify_t *qset = ifp_qset;

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp(unit, entry, ifp_cfg.sip, ifp_cfg.sip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstIp(unit, entry, ifp_cfg.dip, ifp_cfg.dip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcClassField(unit, entry, ifp_cfg.vfp_src_cls_id, ifp_cfg.vfp_src_cls_mask));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionEgressClassSelect, BCM_FIELD_EGRESS_CLASS_SELECT_NEW, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionNewClassId, ifp_cfg.ifp_src_cls_id, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

int
configure_efp(int unit)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;
    bcm_field_qualify_t *qset = efp_qset;

    bcm_field_group_config_t_init(&group_config);
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE |
                          BCM_FIELD_GROUP_CREATE_AUTO_EXPANSION_DISABLE;
    group_config.mode_type = efp_cfg.gmode;
    group_config.packet_type = efp_cfg.ptype;

    BCM_FIELD_QSET_INIT(group_config.qset);
    while (*qset != -1) {
        BCM_FIELD_QSET_ADD(group_config.qset, *qset);
        qset++;
    }

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
    efp_group = group_config.group;

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    efp_entry = entry;

    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry, efp_cfg.l4_src_port, efp_cfg.l4_src_port_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4DstPort(unit, entry, efp_cfg.l4_dst_port, efp_cfg.l4_dst_port_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IngressClassField(unit, entry, efp_cfg.ifp_src_cls_id, efp_cfg.ifp_src_cls_mask));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionOuterVlanNew, efp_cfg.outer_vlan_new, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));


    return BCM_E_NONE;
}

/* Create flex counter action with L3 Route table as source and user set index */

int
configure_efp_flex_counter(int unit)
{
    int options = 0;
    bcm_error_t  rv;
    bcm_flexctr_action_t action;
    bcm_field_flexctr_config_t flexctr_cfg;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;

    bcm_flexctr_action_t_init(&action);
    action.flags = 0;
    action.hint = efp_group;
    action.hint_type = bcmFlexctrHintFieldGroupId;
    action.source = bcmFlexctrSourceEfp;
    action.mode = bcmFlexctrCounterModeNormal;
    action.drop_count_mode = bcmFlexctrDropCountModeAll;
    action.index_num = 1; /* Number of required counters */

    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectStaticEgrFieldStageEgress;
    index_op->object_id[0] = 0;
    index_op->mask_size[0] = 8;
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

    /* Create an ingress action */
    rv = bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flexctr_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

    flexctr_cfg.flexctr_action_id = stat_counter_id;
    flexctr_cfg.counter_index = flexctr_obj_val;
    rv = bcm_field_entry_flexctr_attach(unit, efp_entry, &flexctr_cfg);
    if (BCM_FAILURE(rv)) {
       printf("bcm_field_entry_flexctr_attach() FAILED: %s\n", bcm_errmsg(rv));
       return rv;
    }

    return BCM_E_NONE;
}

/*
 *    Configures the IPV4 unicast route and the required set up to verify it
 */

int
test_setup(int unit)
{
    int num_ports = 2;
    int port_list[num_ports];
    bcm_error_t rv;
    bcm_pbmp_t pbmp;

    rv = port_numbers_get(unit, port_list, num_ports);
    if (BCM_FAILURE(rv)) {
        printf("Error in port_numbers_get %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

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

    bshell(unit, "pw start report +raw ");

    rv = register_pktioRx_callback(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in register_pktioRx_callback %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_vfp(unit);
    if (BCM_FAILURE(rv)) {
        printf("error in configure_vfp %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_ifp(unit);
    if (BCM_FAILURE(rv)) {
        printf("error in configure_ifp %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_efp(unit);
    if (BCM_FAILURE(rv)) {
        printf("error in configure_ifp %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_efp_flex_counter(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_flex_counter %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Flex counter verification */

int
test_verify(int unit)
{
    char str[512];
    int rv;
    bcm_flexctr_counter_value_t counter_value;
    uint32 num_entries = 1;
    uint32 counter_index = flexctr_obj_val;
    int count = sal_rand() % 10;
    if (count == 0) {
      count = 1;
    }

    /*  Vlan creation is just for verification purpose and depends on the packet
     *  being sent and is not a needed step
     */

    snprintf(str, 512, "vlan create %d pbm=%d,%d", ing_vid, ingress_port, egress_port);
    bshell(unit, str);

    print "fp show";
    bshell(unit, "fp show");

    printf("Sending %d packets to ingress port %d\n", count, ingress_port);
    snprintf(str, 512, "tx %d pbm=%d data=0x0000000000020000000000018100400508004518002E000000004006A4C40A0A0A28C0A8011414041AE8000000000000000050000000A4FB0000000102030405640D7C14", count, ingress_port);
    bshell(unit, str);
    sal_sleep(5);

    printf("\n------------------------------------- \n");
    printf("    RESULT OF PACKET VERIFICATION  ");
    printf("\n------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n------------------------------------- \n\n");

    /* test_fail is asserted by pktio_Rx_call_back.
       sleep command in needed to give pktio_Rx_call_back chance to run. */
    if (test_failed == 1)
    {
       return BCM_E_FAIL;
    }

    /* Get counter value. */
    sal_memset(&counter_value, 0, sizeof(counter_value));
    rv = bcm_flexctr_stat_get(unit, stat_counter_id, num_entries, &counter_index,
                              &counter_value);
    if (BCM_FAILURE(rv)) {
      printf("Error in bcm_flexctr_stat_get %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* print counter value. */
    printf("Stat: %d packets / %d bytes\n", COMPILER_64_LO(counter_value.value[0]),
                                               COMPILER_64_LO(counter_value.value[1]));
    if (COMPILER_64_LO(counter_value.value[0]) != count) {
      printf("Error in packet verification. Expected packet stat %d but got %d\n", count, COMPILER_64_LO(counter_value.value[0]));
      test_failed = 1;
    };

    printf("\n-------------------------------------------- \n");
    printf("    RESULT OF FLEX COUNTER VERIFICATION  ");
    printf("\n-------------------------------------------- \n");
    printf("Test = [%s]",(test_failed == 0)?"PASS":"FAIL");
    printf("\n-------------------------------------------- \n\n");


    if(test_failed ==0)
    {
       return BCM_E_NONE;
    }
    else
    {
       return BCM_E_FAIL;
    }
}

/*
 *    This functions does the following
 *    a)test setup [configures the feature]
 *    b)test verify [verifies the configured feature]
 */

int
execute()
{
    bcm_error_t rv;
    int unit = 0;

    rv = test_setup(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_setup %s \n", bcm_errmsg(rv));
        return rv;
    }
    printf("\nCompleted test setup successfully.\n\n");

    rv = test_verify(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in test_verify %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Completed test verify successfully.\n");

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
