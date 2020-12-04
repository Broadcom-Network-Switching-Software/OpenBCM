/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : IFA [ SDK - 6.5.19 and CANCUN - 6.3.3 ]
 *
 * Usage    : BCM.0> cint td3_ifa_trunk_init.c
 *
 * config   : td3_ifa_init_transit_config.bcm
 *
 * Log file : td3_ifa_trunk_init_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *        egress_port_1 |                   |
 *       ----<----------+       TD3         |  ingress_port
 *                      |                   +<----------
 *                      |                   |
 *       ----<----------+                   |
 *        egress_port_2 |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate IFA init for trunk
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup):
 *   =============================================
 *     a) Select ingress port and egress ports configure them in Loopback mode.
 *     b) Do the necessary IFA init 2-pass configurations for trunk
 *       1. In the first pass : Packets are sampled, IFA header and metadata
 *          header are added and the copied packet is mirrored to loopback port
 *       2. In the second pass : IFA metadata is added and sent out of the
 *          egress port
 *
 *   2) Step2 - Verification (Done in test_verify):
 *   ================================================
 *     a) Send the packet that matches the description through ingress port
 *        and observe the packet that egresses
 *
 *     b) Expected Result:
 *     ===================
 *     The egress packet should have configured IFA header, metadata header and
 *     metadata [ IFA hdr:4 bytes + MD hdr:4 bytes + MD:32 bytes ] added
 */

cint_reset();
bcm_port_t ingress_port;
bcm_port_t egress_port;
bcm_port_t egress_port_2;
uint32 ifa20_proto = 253; /* Experimental protocol number */
bcm_ip_t dst_ip = 0x0a0a0200;
bcm_ip_t dst_ip_mask = 0xffffffff;
bcm_ip_t src_ip = 0x0b0b0100;
bcm_ip_t src_ip_mask = 0xffffffff;
uint8 ip_protocol = 0x11;
uint8 ip_protocol_mask = 0xFF;
bcm_l4_port_t l4_src_port = 0x1F00;
bcm_l4_port_t l4_src_port_mask = 0xFFFF;
bcm_l4_port_t l4_dst_port = 0x1F69;
bcm_l4_port_t l4_dst_port_mask = 0xFFFF;

int max_payload_len = 1500;

bcm_mac_t local_mac_network_ingress = "00:00:00:00:11:11";
bcm_mac_t remote_mac_network_ingress = "00:00:00:00:22:22";

bcm_mac_t pkt_in_dst_mac_ingress = "00:00:00:00:bb:bb";

bcm_vlan_t vid_acc_ingress = 100;
bcm_vlan_t vid_network_ingress = 21;

bcm_if_t intf_id_acc_ingress = 100;
bcm_if_t intf_id_network_ingress = 21;

bcm_vrf_t vrf_ingress = 100;

bcm_gport_t gport_ingress = BCM_GPORT_INVALID;
bcm_gport_t gport_egress = BCM_GPORT_INVALID;
bcm_gport_t gport_egress_2 = BCM_GPORT_INVALID;

bcm_if_t egr_obj_network_ingress;

/* S-FLOW Sampling Configuration */
bcm_gport_t mgport;
bcm_mirror_destination_t mdest;
int mport= 65; /* Mirroring to internal loopback port LB0 */

uint32 opaque_val = 0xc1;

bcm_trunk_t tid = 8;
bcm_gport_t trunk_gport = BCM_GPORT_INVALID;

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
ifa_global_setting(int unit, bcm_gport_t gport0)
{
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport0, bcmPortControlPacketProcssingPort, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2Enable, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2HeaderType, ifa20_proto));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataFirstLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFA2MetaDataSecondLookupType, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchIFAMetaDataAdaptTable, BCM_SWITCH_IFA_METADATA_ADAPT1_SINGLE));

    return BCM_E_NONE;
}

/* Trunk configuration */

int
trunk_config(int unit, int *port, int nport, int psc)
{
    int i, rv = 0;
    int flags = BCM_TRUNK_FLAG_WITH_ID;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t trunk_member[nport];
    bcm_gport_t gport;
    bcm_l2_addr_t l2addr;

    BCM_IF_ERROR_RETURN(bcm_trunk_init(unit));

    /* Add ports to trunk group */
    for(i=0; i<nport; i++) {
        bcm_trunk_member_t_init(trunk_member[i]);
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port[i], gport));
        trunk_member[i].gport = gport;
    }

    /* Create trunk group, allocate trunk group id */
    rv = bcm_trunk_create(unit, flags, &tid);
    if (BCM_FAILURE(rv)) {
        printf("Error in trunk creation:%s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_trunk_info_t_init(&trunk_info);

    trunk_info.psc        = psc;
    trunk_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

    rv = bcm_trunk_set(unit, tid, &trunk_info, nport, trunk_member);
    if (BCM_FAILURE(rv)) {
        printf("Error in trunk set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_TRUNK_SET(trunk_gport, tid);

    return BCM_E_NONE;
}

/* Create vlan and add port to vlan */

int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port, bcm_gport_t *gport)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, upbmp;

    rv =  bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressEnable, 1);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_control_port_set for  bcmVlanTranslateIngressEnable: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_gport_get(unit, port, gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_gport_get() for port %d : %s.\n", port, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in bcm_vlan_create() for vlan %d: %s.\n", vlan, bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, upbmp));

    return BCM_E_NONE;
}

/* Create L3 interface */

int
create_l3_intf_my_station(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t l3_intf_id, bcm_vrf_t vrf)
{
    bcm_error_t rv;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
    l3_intf.l3a_intf_id = l3_intf_id;
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_vrf = vrf;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));

    return BCM_E_NONE;
}

/* Create L3 egress object */

int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport, bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.flags2 = BCM_L3_TGID;
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egr, egr_if));

    return BCM_E_NONE;
}

/* Configure L3 host */

int
l3_host_config(int unit, bcm_vrf_t vrf, bcm_if_t egr_if, bcm_ip_t dest_ip)
{
    bcm_error_t rv;
    bcm_l3_host_t hinfo;

    bcm_l3_host_t_init(hinfo);
    hinfo.l3a_vrf     = vrf;
    hinfo.l3a_intf    = egr_if;
    hinfo.l3a_ip_addr = dest_ip;
    BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &hinfo));

    return BCM_E_NONE;
}

/* Steps to configure route */

int
route_config(int unit)
{
    bcm_error_t rv;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, 0, bcmPortControlIP4, 1));

    rv = create_vlan_add_port(unit, vid_acc_ingress, ingress_port, &gport_ingress);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vid_network_ingress, egress_port, &gport_egress);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vid_network_ingress, egress_port_2, &gport_egress_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf_my_station(unit, pkt_in_dst_mac_ingress, vid_acc_ingress, intf_id_acc_ingress, vrf_ingress);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf_my_station(unit, local_mac_network_ingress, vid_network_ingress, intf_id_network_ingress, vrf_ingress);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_egr_obj(unit, intf_id_network_ingress, remote_mac_network_ingress, trunk_gport, &egr_obj_network_ingress);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l3_host_config(unit, vrf_ingress, egr_obj_network_ingress, dst_ip);
    if(BCM_FAILURE(rv)) {
        printf("Error in configuring host: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Mirroring and SFLOW configuration */

int
mirror_config(int unit)
{
    bcm_error_t rv;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, mport, &mgport));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, mgport, bcmPortControlInternalLoopback, 1)); /* Disable src mod/src port override. */
    BCM_IF_ERROR_RETURN(bcm_port_vlan_member_set(unit, mgport, 0)); /* Disable egress vlan checks. */

    /* Configuring mirror destination for flex samples to int lb port */
    bcm_mirror_destination_t_init(&mdest);
    mdest.gport = mgport;
    rv = bcm_mirror_destination_create(unit, &mdest);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror destination create: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_mirror_port_dest_add(unit, 0, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_SFLOW, mdest.mirror_dest_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in mirror port add: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Setting Flex Sample destination to mirror */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlSampleFlexDest, BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ingress_port, bcmPortControlSampleFlexRate, 1));

    return BCM_E_NONE;
}

/* IFA first pass FP configuration */

int
ifa_first_pass(int unit)
{
    bcm_field_group_config_t group_config_pass_1;
    bcm_field_entry_t entry_pass_1;
    bcm_error_t rv;

    bcm_field_group_config_t_init(&group_config_pass_1);

    BCM_FIELD_QSET_INIT(group_config_pass_1.qset);
    BCM_FIELD_ASET_INIT(group_config_pass_1.aset);

    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(group_config_pass_1.qset, bcmFieldQualifyL4DstPort);

    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionEncapIfaMetadataHdr);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionLoopbackType);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionIngSampleEnable);
    BCM_FIELD_ASET_ADD(group_config_pass_1.aset, bcmFieldActionAssignExtractionCtrlId);

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config_pass_1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_1.group, &entry_pass_1));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp(unit, entry_pass_1, src_ip, src_ip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstIp(unit, entry_pass_1, dst_ip, dst_ip_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, entry_pass_1, ip_protocol, ip_protocol_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4SrcPort(unit, entry_pass_1, l4_src_port, l4_src_port_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4DstPort(unit, entry_pass_1, l4_dst_port, l4_dst_port_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry_pass_1, ingress_port, -1));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionAssignExtractionCtrlId, 0xce, 0xff));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionEncapIfaMetadataHdr, opaque_val, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionLoopbackType, bcmFieldLoopbackTypeMasquerade, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_1, bcmFieldActionIngSampleEnable, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_1));

    return BCM_E_NONE;
}

/* IFA second pass FP configuration */

int
ifa_second_pass(int unit)
{
    bcm_field_group_config_t group_config_pass_2;
    bcm_field_entry_t  entry_pass_2;
    bcm_error_t rv;

    bcm_field_group_config_t_init(&group_config_pass_2);
    BCM_FIELD_QSET_INIT(group_config_pass_2.qset);
    BCM_FIELD_ASET_INIT(group_config_pass_2.aset);

    BCM_FIELD_QSET_ADD(group_config_pass_2.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config_pass_2.qset, bcmFieldQualifyPktType);
    BCM_FIELD_QSET_ADD(group_config_pass_2.qset, bcmFieldQualifySysHdrType);

    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionAssignEditCtrlId); /* EditCtrlId: IFA_ENABLE */
    BCM_FIELD_ASET_ADD(group_config_pass_2.aset, bcmFieldActionAssignExtractionCtrlId); /*ExtractionCtrlId: IFA_ENABLE */

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config_pass_2));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config_pass_2.group, &entry_pass_2));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_SysHdrType(unit,entry_pass_2, bcmFieldSysHdrLoopbackCpuMasquerade));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_PktType(unit, entry_pass_2, bcmFieldPktTypeUdpIfa));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_2, bcmFieldActionAssignExtractionCtrlId, 0xee, 0xff));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_pass_2, bcmFieldActionAssignEditCtrlId, 0xb0, 0xff));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_pass_2));

    return BCM_E_NONE;
}

/* IFA first + second pass EVXLT configuration */

int
ifa_evxlt(int unit)
{
    bcm_flow_encap_config_t einfo;
    bcm_flow_option_id_t flow_option_ifa = 0;
    uint32 flow_handle_hdr_md = 0;
    bcm_flow_option_id_t encapA_flow_option= 0, encapB_flow_option = 0;

    /* Configuring IFA hdr and MD hdr [first pass] on EVXLT2 table */
    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "IFA", &flow_handle_hdr_md));
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_BASE_MD_HEADER_ASSIGN_LOOKUP", &encapA_flow_option));

    bcm_flow_encap_config_t_init(&einfo);
    bcm_flow_logical_field_t lfield[3];

    /* Insert BASE_HEADER, METADATA_HEADER */
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapA_flow_option;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;

    /* IFA header */
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DIRECT_ASSIGNMENT_10_ACTION_SET", &lfield[0].id));
    lfield[0].value = 0x2f110228;

    /* IFA MD header */
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DIRECT_ASSIGNMENT_11_ACTION_SET", &lfield[1].id));
    lfield[1].value = 0x33229a00;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "OPAQUE4", &lfield[2].id));
    lfield[2].value = opaque_val;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));

    /*
     * Configuring Metadata [second pass] info on EVXLT1 table
     * DGPP is used to lookup EVXLT1
     */
    /* Metadata for trunk port 1 */
    /* Adapt_1 single Lookup 1 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_1_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_egress;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "OPAQUE_DATA", &lfield[0].id));
    lfield[0].value = 0xaaaaa;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "LNS", &lfield[1].id));
    lfield[1].value = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DEVICE_ID", &lfield[2].id));
    lfield[2].value = 0xDDDDD;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));

    /* Configure Adapt1 single Lookup2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_2_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_egress;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_0", &lfield[0].id));
    lfield[0].value = 0x900ddeed;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_1", &lfield[1].id));
    lfield[1].value = 0x900dfeed;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 2, lfield));

    /*
     * Configuring Metadata [second pass] info on EVXLT1 table
     * DGPP is used to lookup EVXLT1
     */
    /* Metadata for trunk port 2 */
    /* Adapt_1 single Lookup 1 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_1_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_egress_2;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "OPAQUE_DATA", &lfield[0].id));
    lfield[0].value = 0xbbbbb;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "LNS", &lfield[1].id));
    lfield[1].value = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "DEVICE_ID", &lfield[2].id));
    lfield[2].value = 0xEEEEE;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 3, lfield));


    /* Configure Adapt1 single Lookup2 */
    BCM_IF_ERROR_RETURN(bcm_flow_option_id_get(unit, flow_handle_hdr_md, "IFA_MD_LOOKUP_2_ADAPT1_SINGLE", &encapB_flow_option));

    /* Insert METADATA */
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle_hdr_md;
    einfo.flow_option = encapB_flow_option;
    einfo.flow_port = gport_egress_2;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_0", &lfield[0].id));
    lfield[0].value = 0x900dface;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle_hdr_md, "MMU_STAT_1", &lfield[1].id));
    lfield[1].value = 0xd009cafe;

    einfo.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_FLEX_DATA_VALID | BCM_FLOW_ENCAP_FLEX_KEY_VALID;

    BCM_IF_ERROR_RETURN(bcm_flow_encap_add(unit, &einfo, 2, lfield));

    return BCM_E_NONE;
}

/* Timing configuration for timestamps */

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
    bcm_gport_t gport0;
    bcm_module_t module = 0;
    bcm_error_t rv;
    int port_list[3];
    bcm_port_t port_array[2];
    int lb_mode = BCM_PORT_LOOPBACK_MAC;
    int nport = (sizeof(port_array)) / (sizeof(port_array[0]));

    rv = portNumbersGet(unit, port_list, 3);
    if (BCM_FAILURE(rv)) {
        printf("Error in portNumbersGet %s\n", bcm_errmsg(rv));
        return rv;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    egress_port_2 = port_list[2];
    printf("Choosing port %d as ingress port and %d,%d as egress ports\n", ingress_port, egress_port, egress_port_2);

    /* Set port mac loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, lb_mode));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port_2, lb_mode));

    BCM_GPORT_MODPORT_SET(gport0, module, ingress_port);

    port_array[0] = egress_port;
    port_array[1] = egress_port_2;
    rv = trunk_config(unit, port_array, nport, BCM_TRUNK_PSC_ROUND_ROBIN);
    if (BCM_FAILURE(rv)) {
        printf("Error in trunk configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_global_setting(unit, gport0);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA global settings %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = route_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in route configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = mirror_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in mirror configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_first_pass(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA first pass configuration %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = ifa_second_pass(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in IFA second pass configuration %s\n", bcm_errmsg(rv));
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
    snprintf(str, 512, "port %d discard=all", egress_port_2);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", egress_port);
    bshell(unit, str);
    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", egress_port_2);
    bshell(unit, str);
    bshell(unit,"pw start");
    bshell(unit,"pw report +raw");

    /* Send a packet to ingress port*/
    printf("Packet comes into ingress port %d\n", ingress_port);
    snprintf(str, 512, "tx 2 pbm=%d  data=00000000BBBB00000000AAAA8100006408004500006A000000000B11976F0B0B01000A0A02001F001F690056D8CD000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F404142434445464748494A4B4C4DEA73CF34;sleep 5", ingress_port);
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
