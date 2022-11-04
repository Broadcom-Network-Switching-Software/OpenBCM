/*
 * An exapmle of injected BFD over Srv6 with T.Insert
 *
 * Test Scenario - start
 *
 * ./bcm.user
 * config add appl_param_srv6_reduce_enable.0=1
 * config add appl_param_srv6_psp_enable.0=1
 * config add tm_port_header_type_in_202.0=ETH
 * config add ucode_port_40.0=RCY.0:core_1.40
 * config add tm_port_header_type_in_40.0=RCH_0 (or IBCH1_MODE, if supported)
 * config add tm_port_header_type_out_40.0=ETH
 * 0: tr 141 NoInit=1
 * config add custom_feature_init_verify=1
 * config delete custom_feature_multithread_en*
 * config delete custom_feature_kbp_multithread_en*
 * 0: tr 141 NoDeinit=1
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../../../src/examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../../../src/examples/dnx/crps/cint_crps_etpp_config.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/srv6/cint_srv6_basic.c
 * cint ../../../../src/examples/dnx/srv6/cint_dnx_srv6_test.c
 * cint ../../../../src/examples/dnx/bfd/cint_dnx_bfd_srv6_t_insert.c
 * cint
 * srv6_ingress_tunnel_config(0,200,202,201,1,0,0,1,0,0);
 * exit;
 *
 * cint
 * bfd_into_srv6_t_insert_new_fai_pmf_configuration(0,2,12);
 * bfd_into_srv6_t_insert_endpoint_create(0);
 * exit;
 *
 * // BFD packet from OAMP should be:
 * // Data: 0x00000000cd1d00123456789a8100f06486dd67f0000000382b8000000000000000001111222233334444abcddbca123443211010989845679abc110204010000000011112222111122221234f000ff00ff13c00112b00020b56e20c00318000100040003000400000000000f424000000000
 *
 * Test Scenario - end
 */

int tx_port = 40;
bcm_l2_egress_t recycle_entry;
int bfd_endpoint_id;

/* Create recycle encap */
int
dnx_recycle_encap_create (
  int unit)
{
    bcm_vlan_port_t vlan_port;

    /* Create CTRL-LIF LIF */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /* Create recycle encap */
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppRedirectVrf;
    recycle_entry.vrf = 1;
    recycle_entry.vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "");

    return BCM_E_NONE;
}

/* Create BFD endpoint over SRv6 t insert */
int
bfd_into_srv6_t_insert_endpoint_create (
  int unit)
{
    uint32 local_discr=0x10004;
    int bfd_period=1000;
    uint32 remote_discr=0x30004;
    uint32 sip=0xC0800101;
    bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44};
    bcm_bfd_endpoint_info_t ep;
    int device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));

    /** create recycle encap */
    BCM_IF_ERROR_RETURN_MSG(dnx_recycle_encap_create(unit), "");

    bcm_bfd_endpoint_info_t_init(&ep);
    ep.type = bcmBFDTunnelTypeUdp;
    if (!device_type_v2) {
        ep.ipv6_extra_data_index = 16384 + (local_discr);
    }
    sal_memcpy(ep.src_ip6_addr, src_ipv6_tmp, sizeof(ep.src_ip6_addr));
    sal_memcpy(ep.dst_ip6_addr, cint_srv6_tunnel_info.route_ipv6_dip, sizeof(ep.dst_ip6_addr));

    ep.loc_clear_threshold = 1;
    ep.ip_ttl = 0x80;
    ep.ip_tos = 0x7F;
    ep.udp_src_port = 0xC001;

    ep.egress_if = 0;
    ep.int_pri = 4;
    ep.ip_subnet_length = device_type_v2 ? 0 : 16;
    ep.bfd_period = bfd_period;
    ep.local_min_rx = 1000000;
    ep.local_min_rx = 1000000;

    ep.local_state = 3;
    ep.local_flags = 0;
    ep.local_detect_mult = 3;
    ep.remote_detect_mult = 3;
    ep.remote_discr = remote_discr;
    ep.local_discr = local_discr;
    ep.remote_gport = BCM_GPORT_INVALID;

    BCM_GPORT_SYSTEM_PORT_ID_SET(ep.tx_gport, tx_port);
    ep.egress_if = recycle_entry.encap_id;

    ep.flags = BCM_BFD_ENDPOINT_IN_HW;
    ep.flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    ep.flags |= BCM_BFD_ENDPOINT_IPV6;

    BCM_IF_ERROR_RETURN_MSG(bcm_bfd_endpoint_create(unit, &ep), "");

    bfd_endpoint_id = ep.id;

    printf("Created endpoint (%d) local_discr (0x%08x) remote_discr (0x%08x) tx_gport (0x%08x)\n", ep.id, ep.local_discr, ep.remote_discr, ep.tx_gport);

    return BCM_E_NONE;
}

/* Update forwarding additional info using PMF for BFD packet */
int
bfd_into_srv6_t_insert_new_fai_pmf_configuration (
    int unit,
    uint32_t presel_id,
    uint32 new_fai)
{
    uint32_t pmf1_ctx_id;
    uint32_t qual_id_fai_3_0;
    int bfd_fg_id;
    void *dest_char;

    bcm_field_context_info_t context_info;
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "bfd_multihop_pmf1", sizeof(context_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &pmf1_ctx_id), "");

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;
    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);
    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.nof_qualifiers = 1;
    ipmf_presel_entry_data.context_id = pmf1_ctx_id;
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[0].qual_arg = 2;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeBfdMultiHop;
    ipmf_presel_entry_data.qual_data[0].qual_mask = 31;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data), "");

    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 4;
    dest_char = &(qual_info.name);
    sal_strncpy_s(dest_char, "BFD_FAI_BITS_3_0", sizeof(qual_info.name));
    BCM_IF_ERROR_RETURN_MSG(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_fai_3_0), "");
    bcm_field_group_info_t fg_info;
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "BFD_Ingress", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_id_fai_3_0;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_with_valid_bit[0] = FALSE;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &bfd_fg_id), "");
    bcm_field_group_attach_info_t fg_attach_info;
    bcm_field_group_attach_info_t_init(&fg_attach_info);

    fg_attach_info.key_info.nof_quals = fg_info.nof_quals;
    fg_attach_info.payload_info.nof_actions = fg_info.nof_actions;
    int ii;
    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        fg_attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        fg_attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }
    fg_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    fg_attach_info.key_info.qual_info[0].input_arg = new_fai;

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, bfd_fg_id, pmf1_ctx_id, &fg_attach_info), "");

    printf("PMF for multihop BFD is created successfully\n");
    return BCM_E_NONE;
}
