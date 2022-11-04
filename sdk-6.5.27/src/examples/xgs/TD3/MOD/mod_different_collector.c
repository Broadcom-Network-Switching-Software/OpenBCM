cint_reset();

int unit = 0;

//uint32               NUM_GROUPS = 4;
uint32               NUM_GROUPS = 1;
uint32               NUM_PIPES  = 2;
uint32               MAX_GROUP_ACTIONS = 1;
uint32               NUM_COLLECTORS = 2;

bcm_port_t CPU_PORT = 0;

bcm_field_group_t ifp_group;
bcm_field_entry_t em_miss[256][100];
bcm_field_entry_t first_drop[256][100];
bcm_field_entry_t drop_sample[256][100];
bcm_field_entry_t collector_copy_to_cpu;
int               ifp_entry_cnt[256];

//uint32 sample_rate = 1000;
uint32 sample_rate = 1;
uint32 sample_seed = 0xAABBCC;

bcm_port_config_t pc;

uint8 DMA_CHANNEL  = 9;
uint8 EM_MISS_COSQ = 47;
uint8 DROP_COSQ    = 47;

bcm_port_t cpu_port  = 0;
bcm_port_t coll_port[NUM_COLLECTORS] = {27,27};

bcm_collector_t             coll_id[NUM_COLLECTORS];
bcm_mac_t                   coll_dst_mac     = {0x6c, 0x41, 0x6a, 0xf4, 0x0c, 0x4a};
bcm_mac_t                   coll_src_mac     = {0x00, 0x0a, 0xf7, 0x81, 0x88, 0x9a};
uint16                      coll_tpid        = 0x8100;
bcm_vlan_t                  coll_vlan[NUM_COLLECTORS]        = {10,70};
bcm_ip_t                    coll_src_ip      = 0x0a82562a;
bcm_ip_t                    coll_dst_ip[NUM_COLLECTORS]      = {0x0a82b00a, 0x0a82b00b};
uint8                       coll_dscp        = 0xB8;
uint8                       coll_ttl         = 16;
bcm_l4_port_t               coll_l4_src_port = 0x1F91;
bcm_l4_port_t               coll_l4_dst_port = 0x0807;
uint16                      coll_mtu         = 1500;


int             export_profile_id[NUM_COLLECTORS];
bcm_collector_wire_format_t  wire_format[NUM_COLLECTORS] = {bcmCollectorWireFormatProtoBuf, bcmCollectorWireFormatIpfix};

bcm_flowtracker_export_template_t tmpl_id;
uint16                            set_id      = 257;

uint32 group_flow_limit       = 10000;
//uint32 aging_interval_msecs   = 254 * 500;
uint32 aging_interval_msecs   = 24000;
//uint32 aging_interval_msecs   = 5000;

uint32 MAX_TMPL_ELEMENTS = 10;

bcm_policer_config_t policer_config,efp_policer_config;
bcm_policer_t        policer_id,efp_policer_id;
uint32               ctc_pps = 10000;


bcm_udf_id_t        vxlan_udf_id[5];
bcm_udf_id_t        prop_vxlan_udf_id;

bcm_field_presel_t       presel_id[NUM_PIPES][NUM_GROUPS + 1];
bcm_field_group_t        em_groups[NUM_GROUPS + 1][NUM_PIPES];

/*L3 parameters */
bcm_mac_t local_mac_network_ingress = "00:00:00:00:11:11";
bcm_mac_t remote_mac_network_ingress = "00:00:00:00:22:22";

bcm_mac_t pkt_in_src_mac_ingress = "00:00:00:00:aa:aa";
bcm_mac_t pkt_in_dst_mac_ingress = "00:00:00:00:bb:bb";

bcm_mac_t local_mac_network_ingress2 = "00:00:00:00:33:33";
bcm_mac_t remote_mac_network_ingress2 = "00:00:00:00:44:44";

bcm_mac_t pkt_in_src_mac_ingress2 = "00:00:00:00:05:06";
bcm_mac_t pkt_in_dst_mac_ingress2 = "00:00:00:00:07:08";

bcm_vlan_t vid_acc_ingress2 = 200;
bcm_vlan_t vid_network_ingress2 = 300;

bcm_if_t intf_id_acc_ingress2 = 200;
bcm_if_t intf_id_network_ingress2 = 300;

bcm_vlan_t vid_acc_ingress = 100;
bcm_vlan_t vid_network_ingress = 21;

bcm_if_t intf_id_acc_ingress = 100;
bcm_if_t intf_id_network_ingress = 21;

bcm_vrf_t vrf_ingress = 100;
bcm_vrf_t vrf_ingress2 = 50;
bcm_port_t port_acc_ingress = 25;
bcm_port_t port_network_ingress = 26;

bcm_gport_t gport_acc_ingress = BCM_GPORT_INVALID;
bcm_gport_t gport_acc_ingress2 = BCM_GPORT_INVALID;
bcm_gport_t gport_network_ingress2 = BCM_GPORT_INVALID;
bcm_gport_t gport_network_ingress = BCM_GPORT_INVALID;

bcm_if_t egr_obj_acc_ingress;
bcm_if_t egr_obj_network_ingress,egr_obj_network_ingress2;
bcm_ip_t pkt_in_dst_ip = 0x0a0a0200;

/*
 * Function:
 *   drop_reasons_set
 * Purpose:
 *      Set or unset drop reasons
 * Parameters:
 *        enable - (IN) 1 - enable, 0 - disable
 * Notes:
 */
int drop_reasons_set(int enable)
{
    bcm_rx_reasons_t reasons;
    
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                               bcmSwitchUnknownVlanToCpu,
                                               enable));
    

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                               bcmSwitchParityErrorToCpu,
                                               enable));
    /*
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                              bcmSwitchV4L3ErrToCpu,
                                               enable));*/
     
     /*BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                              bcmSwitchL3UcTtlErrToCpu,
                                               enable));*/
     /*
     BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                              bcmSwitchDosAttackToCpu,
                                               enable));
     BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                              bcmSwitchDosAttackSipEqualDip,
                                               enable));*/
					       
     BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                              bcmSwitchV4L3DstMissToCpu,
                                               enable));
    
    /* Redirect Parity error to Drop CoSQ */
    BCM_RX_REASON_CLEAR_ALL(reasons);
    BCM_RX_REASON_SET(reasons, bcmRxReasonParityError);


    BCM_IF_ERROR_RETURN(
             bcm_rx_cosq_mapping_set(unit, 1,
                                     reasons, reasons,
                                     0, 0,
                                     0, 0,
                                     DROP_COSQ));
    return BCM_E_NONE;
}

/*
 * Function:
 *   setup
 * Purpose:
 *      Initial setup
 * Parameters:
 *        None
 * Notes:
 */
int setup()
{
    bcm_pbmp_t               pbmp, upbmp;
    bcm_l2_addr_t            l2_addr;
    bcm_field_qset_t         qset;
    bcm_field_group_t        group;
    bcm_field_entry_t        entry;
    bcm_port_t               port;
    bcm_gport_t              mtp_gport;
    bcm_mirror_destination_t mirror_dest;
    int i;

    bcm_port_config_t_init(&pc);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &pc));

    /* Associate UC cosq with DMA channel */
    BCM_IF_ERROR_RETURN(bcm_rx_queue_channel_set(unit, EM_MISS_COSQ, DMA_CHANNEL));
    BCM_IF_ERROR_RETURN(bcm_rx_queue_channel_set(unit, DROP_COSQ, DMA_CHANNEL));

    for (i = 0; i < NUM_COLLECTORS; i++) {
        /* Create vlan to redirect export packets */
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, coll_vlan[i]));
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(upbmp);
        BCM_PBMP_PORT_ADD(pbmp, cpu_port);
        BCM_PBMP_PORT_ADD(pbmp, coll_port[i]);
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, coll_vlan[i], pbmp, upbmp));


        /* Add static MAC address */
        bcm_l2_addr_t_init(&l2_addr, coll_dst_mac, coll_vlan[i]);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port  = coll_port[i];
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }

    /* Create policer */
    if (ctc_pps != 0) {
        bcm_policer_config_t_init(&policer_config);
        policer_config.flags      = BCM_POLICER_MODE_PACKETS;
        policer_config.mode       = bcmPolicerModeCommitted;
        policer_config.ckbits_sec = ctc_pps;
	policer_config.ckbits_burst = 1;
        BCM_IF_ERROR_RETURN(bcm_policer_create(unit, &policer_config,
                                               &policer_id));
    }

    /* Set VxLAN UDP dest port */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                               bcmSwitchVxlanUdpDestPortSet,
                                               0xfff));

    /* Set EM to Pipe Local */
    BCM_IF_ERROR_RETURN(
            bcm_field_group_oper_mode_set(unit,
                                          bcmFieldQualifyStageIngressExactMatch,
                                          bcmFieldGroupOperModePipeLocal));

    /* Set the drop reasons */
    BCM_IF_ERROR_RETURN(drop_reasons_set(1));

    /* Set sample seed */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                                               bcmSwitchSampleFlexRandomSeed,
                                               sample_seed));

    /* Retrieve gport handles */
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, CPU_PORT, &mtp_gport));

    /* Configure MTP port.  */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = mtp_gport;

    mirror_dest.flags |= BCM_MIRROR_DEST_PORT;
    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, &mirror_dest));

    /* Enable SFLOW mirroring */
    BCM_IF_ERROR_RETURN(
            bcm_mirror_port_dest_add(unit, 5,
                                     (BCM_MIRROR_PORT_SFLOW | BCM_MIRROR_PORT_INGRESS),
                                     mirror_dest.mirror_dest_id));

    /* Enable sampling for the ports */
    BCM_PBMP_ITER(pc.all, port) {
        BCM_IF_ERROR_RETURN(
               bcm_port_control_set(unit, port,
                                    bcmPortControlSampleFlexRate, sample_rate));

        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                                                 bcmPortControlSampleFlexDest,
                                                 BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *   vfp_src_class_set
 * Purpose:
 *      Assign VFP class Id depending on port
 * Parameters:
 *        None
 * Notes:
 */
int vfp_src_class_set()
{
    bcm_field_group_t vfp_group;
    bcm_field_qset_t  qset;
    bcm_field_entry_t entry;
    bcm_port_t        port;
    int               class_id;
    int               vlan_id=100;
    uint8 ip_protocol = 0x11;
    uint8 ip_protocol_mask = 0xFF;
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageLookup);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    //BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    //BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    //BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_IF_ERROR_RETURN(
           bcm_field_group_create(unit, qset,
                                  BCM_FIELD_GROUP_PRIO_ANY, &vfp_group));

    
    //BCM_PBMP_ITER(pc.all, port) {

        //if ((port % (NUM_GROUPS + 1)) == 0) {
        //    continue;
        //}
        //class_id = port % (NUM_GROUPS + 1);
        class_id=1;

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, vfp_group, &entry));
        
	BCM_IF_ERROR_RETURN(
                 bcm_field_qualify_InPort(unit,
                                          entry, port_acc_ingress, 0xFFFFFFFF)); 
	//BCM_IF_ERROR_RETURN(bcm_field_qualify_IpProtocol(unit, entry, ip_protocol, ip_protocol_mask));
	//BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, 50, 0xFFFF));
	//BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, "00:00:00:00:BB:BB", "FF:FF:FF:FF:FF:FF"));
        BCM_IF_ERROR_RETURN(
                  bcm_field_action_add(unit, entry,
                                       bcmFieldActionClassSourceSet,
                                       class_id, 0));
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    //}

    return BCM_E_NONE;
}

/*
 * Function:
 *   presel_create
 * Purpose:
 *      Create presel to match on VFP class Id
 * Parameters:
 *        None
 * Notes:
 */
int presel_create()
{
    bcm_port_t            port;
    bcm_field_src_class_t data;
    bcm_field_src_class_t mask;
    int                   pipe;
    int                   group;

    for (pipe = 0; pipe < NUM_PIPES; pipe++) {
        BCM_IF_ERROR_RETURN(
                 bcm_field_source_class_mode_set(unit,
                                                 bcmFieldStageIngressExactMatch,
                                                 pc.per_pipe[pipe],
                                                 bcmFieldSrcClassModeDefault));

        for (group = 1; group <= NUM_GROUPS; group++) {
            BCM_IF_ERROR_RETURN(bcm_field_presel_create(unit, &(presel_id[pipe][group])));


            BCM_IF_ERROR_RETURN(
                    bcm_field_qualify_Stage(unit,
                                            presel_id[pipe][group] | BCM_FIELD_QUALIFY_PRESEL,
                                            bcmFieldStageIngressExactMatch));

            bcm_field_src_class_t_init(&data);
            bcm_field_src_class_t_init(&mask);

            data.src_class_field = group;
            mask.src_class_field = 0xFFFF;


            BCM_IF_ERROR_RETURN(
                        bcm_field_qualify_MixedSrcClassId(unit,
                                                          presel_id[pipe][group] | BCM_FIELD_QUALIFY_PRESEL,
                                                          pc.per_pipe[pipe], data, mask));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   udf_create
 * Purpose:
 *      Create UDFs
 * Parameters:
 *        None
 * Notes:
 */
int udf_create()
{
    bcm_udf_chunk_info_t  info;
    bcm_udf_alloc_hints_t hints;
    uint32                chunk_bmap;
    uint32                chunks;
    int                   offset;
    int                   width;
    int                   rv;

    /* Inner Src IPv4 */
    offset = 46;
    width  = 4;

    chunks = width / 2;
    chunk_bmap = (1 << chunks) - 1;

    bcm_udf_chunk_info_t_init(&info);
    info.offset              = offset * 8;
    info.width               = width  * 8;
    info.chunk_bmap          = chunk_bmap;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpVxlan;

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;

    BCM_IF_ERROR_RETURN(bcm_udf_chunk_create(unit, &hints, &info, &(vxlan_udf_id[0])));
    printf("UDF ID created %d\n",vxlan_udf_id[0]);

    /* Inner Dst IPv4 */
    offset = 50;
    width  = 4;

    chunks = width / 2;
    chunk_bmap = (1 << chunks) - 1;
    chunk_bmap <<= 2;

    bcm_udf_chunk_info_t_init(&info);
    info.offset              = offset * 8;
    info.width               = width  * 8;
    info.chunk_bmap          = chunk_bmap;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpVxlan;

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;

    BCM_IF_ERROR_RETURN(bcm_udf_chunk_create(unit, &hints, &info, &(vxlan_udf_id[1])));
    printf("UDF ID created %d\n",vxlan_udf_id[1]);

    /* Inner Src Port */
    offset = 54;
    width  = 2;

    chunks = width / 2;
    chunk_bmap = (1 << chunks) - 1;
    chunk_bmap <<= 4;

    bcm_udf_chunk_info_t_init(&info);
    info.offset              = offset * 8;
    info.width               = width  * 8;
    info.chunk_bmap          = chunk_bmap;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpVxlan;

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;

    BCM_IF_ERROR_RETURN(bcm_udf_chunk_create(unit, &hints, &info, &(vxlan_udf_id[2])));
    printf("UDF ID created %d\n",vxlan_udf_id[2]);

    /* Inner Dst Port */
    offset = 56;
    width  = 2;

    chunks = width / 2;
    chunk_bmap = (1 << chunks) - 1;
    chunk_bmap <<= 6;

    bcm_udf_chunk_info_t_init(&info);
    info.offset              = offset * 8;
    info.width               = width  * 8;
    info.chunk_bmap          = chunk_bmap;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpVxlan;

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;

    BCM_IF_ERROR_RETURN(bcm_udf_chunk_create(unit, &hints, &info, &(vxlan_udf_id[3])));
    printf("UDF ID created %d\n",vxlan_udf_id[3]);

    /* Inner Protocol */
    offset = 43;
    width  = 2;

    chunks = width / 2;
    chunk_bmap = (1 << chunks) - 1;
    chunk_bmap <<= 7;

    bcm_udf_chunk_info_t_init(&info);
    info.offset              = offset * 8;
    info.width               = width  * 8;
    info.chunk_bmap          = chunk_bmap;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpVxlan;

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;

    BCM_IF_ERROR_RETURN(bcm_udf_chunk_create(unit, &hints, &info, &(vxlan_udf_id[4])));
    printf("UDF ID created %d\n",vxlan_udf_id[4]);

    /* Proprietary VNID */
    offset = 12;
    width  = 4;

    chunks = width / 2;
    chunk_bmap = (1 << chunks) - 1;

    bcm_udf_chunk_info_t_init(&info);
    info.offset              = offset * 8;
    info.width               = width  * 8;
    info.chunk_bmap          = chunk_bmap;
    info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpUnknownL5;

    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;

    BCM_IF_ERROR_RETURN(bcm_udf_chunk_create(unit, &hints, &info, &prop_vxlan_udf_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *   vxlan_em_group_create
 * Purpose:
 *      Create EM group to track VxLAN headers
 * Parameters:
 *        None
 * Notes:
 */
int vxlan_em_group_create(bcm_flowtracker_group_t flow_group_id)
{
    bcm_field_aset_t         actn;
    bcm_field_qset_t         qset;
    bcm_field_hint_t         hint;
    bcm_field_hintid_t       hint_id;
    bcm_field_group_config_t gcfg;
    int                      pipe;
    int                      rv;

    /* Create the hint Id */
    BCM_IF_ERROR_RETURN(bcm_field_hints_create(unit, &hint_id));

    /* Inner Src IP hint */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual      = bcmFieldQualifyUdf;
    hint.udf_id    = vxlan_udf_id[0];
    hint.start_bit = 0;
    hint.end_bit   = 31;
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    /* Inner Dest IP hint */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual      = bcmFieldQualifyUdf;
    hint.udf_id    = vxlan_udf_id[1];
    hint.start_bit = 0;
    hint.end_bit   = 31;
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    /* Inner Src Port hint */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual      = bcmFieldQualifyUdf;
    hint.udf_id    = vxlan_udf_id[2];
    hint.start_bit = 0;
    hint.end_bit   = 15;
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    /* Inner Dest Port hint */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual      = bcmFieldQualifyUdf;
    hint.udf_id    = vxlan_udf_id[3];
    hint.start_bit = 0;
    hint.end_bit   = 15;
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    /* Inner IP Protocol hint */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual      = bcmFieldQualifyUdf;
    hint.udf_id    = vxlan_udf_id[4];
    hint.start_bit = 8;
    hint.end_bit   = 15;
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    for (pipe = 0; pipe < NUM_PIPES; pipe++) {
        /* Qualifiers(keys) to be used for this Qset */
        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngressExactMatch);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVxlanNetworkId);
        BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 5,
                                                        vxlan_udf_id, &qset));

        /* ASET */
        BCM_FIELD_ASET_INIT(actn);
        BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);

        bcm_field_group_config_t_init(&gcfg);
        gcfg.flags    = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
                        BCM_FIELD_GROUP_CREATE_WITH_ASET      |
                        BCM_FIELD_GROUP_CREATE_WITH_PORT;
        gcfg.qset     = qset;
        gcfg.aset     = actn;
        gcfg.priority = 0;
        gcfg.hintid   = hint_id;
        gcfg.mode     = bcmFieldGroupModeAuto;
        BCM_FIELD_PRESEL_ADD(gcfg.preselset, presel_id[pipe][flow_group_id]);

        if (BCM_PBMP_IS_NULL(pc.per_pipe[pipe])) {
            /* No ports in this pipe */
            continue;
        } else {
            BCM_PBMP_ASSIGN(gcfg.ports, pc.per_pipe[pipe]);
        }


        BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg));

        em_groups[flow_group_id][pipe] = gcfg.group;
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *   ipv4_em_group_create
 * Purpose:
 *      Create EM group to track IPv4 headers
 * Parameters:
 *        None
 * Notes:
 */
int ipv4_em_group_create(bcm_flowtracker_group_t flow_group_id)
{
    bcm_field_aset_t         actn;
    bcm_field_qset_t         qset;
    bcm_field_group_config_t gcfg;
    int                      pipe;
    int                      rv;

    for (pipe = 0; pipe < NUM_PIPES; pipe++) {
        /* Qualifiers(keys) to be used for this Qset */
        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngressExactMatch);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4SrcPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

        /* ASET */
        BCM_FIELD_ASET_INIT(actn);
        BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);

        bcm_field_group_config_t_init(&gcfg);
        gcfg.flags    = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
                        BCM_FIELD_GROUP_CREATE_WITH_ASET      |
                        BCM_FIELD_GROUP_CREATE_WITH_PORT;
        gcfg.qset     = qset;
        gcfg.aset     = actn;
        gcfg.priority = 0;
        gcfg.mode     = bcmFieldGroupModeAuto;
        BCM_FIELD_PRESEL_ADD(gcfg.preselset, presel_id[pipe][flow_group_id]);

        if (BCM_PBMP_IS_NULL(pc.per_pipe[pipe])) {
            /* No ports in this pipe */
            continue;
        } else {
            BCM_PBMP_ASSIGN(gcfg.ports, pc.per_pipe[pipe]);
        }


        BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg));

        em_groups[flow_group_id][pipe] = gcfg.group;
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *   prop_vxlan_em_group_create
 * Purpose:
 *      Create EM group to track IPv4 headers + proprietary vxlan
 * Parameters:
 *        None
 * Notes:
 */
int prop_vxlan_em_group_create(bcm_flowtracker_group_t flow_group_id)
{
    bcm_field_aset_t         actn;
    bcm_field_qset_t         qset;
    bcm_field_hint_t         hint;
    bcm_field_hintid_t       hint_id;
    bcm_field_group_config_t gcfg;
    int                      pipe;
    int                      rv;

    /* Create the hint Id */
    BCM_IF_ERROR_RETURN(bcm_field_hints_create(unit, &hint_id));

    /* Prop VxLAN hint */
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual      = bcmFieldQualifyUdf;
    hint.udf_id    = prop_vxlan_udf_id;
    hint.start_bit = 8;
    hint.end_bit   = 31;
    BCM_IF_ERROR_RETURN(bcm_field_hints_add(unit, hint_id, &hint));

    for (pipe = 0; pipe < NUM_PIPES; pipe++) {
        /* Qualifiers(keys) to be used for this Qset */
        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngressExactMatch);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4SrcPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
        BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1,
                                                        &prop_vxlan_udf_id, &qset));

        /* ASET */
        BCM_FIELD_ASET_INIT(actn);
        BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);

        bcm_field_group_config_t_init(&gcfg);
        gcfg.flags    = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
                        BCM_FIELD_GROUP_CREATE_WITH_ASET      |
                        BCM_FIELD_GROUP_CREATE_WITH_PORT;
        gcfg.qset     = qset;
        gcfg.aset     = actn;
        gcfg.priority = 0;
        gcfg.hintid   = hint_id;
        gcfg.mode     = bcmFieldGroupModeAuto;
        BCM_FIELD_PRESEL_ADD(gcfg.preselset, presel_id[pipe][flow_group_id]);

        if (BCM_PBMP_IS_NULL(pc.per_pipe[pipe])) {
            /* No ports in this pipe */
            continue;
        } else {
            BCM_PBMP_ASSIGN(gcfg.ports, pc.per_pipe[pipe]);
        }


        BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg));

        em_groups[flow_group_id][pipe] = gcfg.group;
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *   ifp_group_create
 * Purpose:
 *      Create IFP group to redirect packets to R5
 * Parameters:
 *        None
 * Notes:
 */
int ifp_group_create()
{
    bcm_field_qset_t          qset;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyExactMatchHitStatus);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyExactMatchActionClassId);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset,
                                               BCM_FIELD_GROUP_PRIO_ANY,
                                               &ifp_group));
    return BCM_E_NONE;
}

/*
 * Function:
 *   ifp_entry_common_setup
 * Purpose:
 *      Setup the common part of IFP rule
 * Parameters:
 *        group         - (IN)    Field group
 *        entry         - (INOUT) Field entry
 *        flow_group_id - (IN)    Flow group Id
 *        port          - (IN)    In Port
 * Notes:
 */
int ifp_entry_common_setup(bcm_field_group_t group, bcm_field_entry_t *entry,
                           uint8 flow_group_id, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, *entry,
                                                 port_acc_ingress,
                                                0xFFFFFFFF));
    
    BCM_IF_ERROR_RETURN(
           bcm_field_action_add(unit, *entry,
                                bcmFieldActionEgressClassSelect,
                                BCM_FIELD_EGRESS_CLASS_SELECT_NEW, 0));

    BCM_IF_ERROR_RETURN(
          bcm_field_action_add(unit, *entry,
                               bcmFieldActionHiGigClassSelect,
                               BCM_FIELD_HIGIG_CLASS_SELECT_EGRESS, 0));

    /* Drop action not needed, added here for simplifying the test */
    BCM_IF_ERROR_RETURN(
          bcm_field_action_add(unit, *entry,
                               bcmFieldActionDrop, 0, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *   em_miss_ifp_entry_create
 * Purpose:
 *      Create IFP rules to copy EM miss packets to R5.
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int em_miss_ifp_entry_create(uint8 flow_group_id)
{
    bcm_port_t    port;
    int           rule_cnt = 0;
    uint16        class_id;
    int           rv;

    //BCM_PBMP_ITER(pc.all, port) {

        //if ((port % (NUM_GROUPS + 1)) != flow_group_id) {
          //  continue;
        //}

        rv = ifp_entry_common_setup(ifp_group,
                                    &(em_miss[flow_group_id][rule_cnt]),
                                    flow_group_id, port);
        if (rv !=  BCM_E_NONE) {
            printf("ifp_entry_common_setup() failed rv=%d \n", rv);
            return rv;
        }

        /* Assign group Id as class Id */
        BCM_IF_ERROR_RETURN(
               bcm_field_action_add(unit, em_miss[flow_group_id][rule_cnt],
                                    bcmFieldActionNewClassId, flow_group_id, 0));

        /* Copy only packets that miss the EM table */
        BCM_IF_ERROR_RETURN(
                bcm_field_qualify_ExactMatchHitStatus(unit,
                                                      em_miss[flow_group_id][rule_cnt],
                                                      0, 0, 0xFF));
        
        /* Qualify on Drop packets */
	
        BCM_IF_ERROR_RETURN(
               bcm_field_qualify_Drop(unit, em_miss[flow_group_id][rule_cnt],
                                      1, 0xffffffff)); 
        /* Attach policer to entry */
        if (ctc_pps != 0) {
            BCM_IF_ERROR_RETURN(
                    bcm_field_entry_policer_attach(unit, em_miss[flow_group_id][rule_cnt],
                                                   0, policer_id));

            /* Copy green packets to CPU */
            BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, em_miss[flow_group_id][rule_cnt],
                                                     bcmFieldActionGpCopyToCpu,
                                                     0, 0));

        } else {
            /* Copy all packets to CPU */
            BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, em_miss[flow_group_id][rule_cnt],
                                                     bcmFieldActionCopyToCpu,
                                                     0, 0));
        }

        /* Copy the packets to cosq mapped to core0 */
        BCM_IF_ERROR_RETURN(
                            bcm_field_action_add(unit,
                                                 em_miss[flow_group_id][rule_cnt],
                                                 bcmFieldActionCosQCpuNew,
                                                 EM_MISS_COSQ, 0));

        /* Install the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, em_miss[flow_group_id][rule_cnt]));
	printf("Entry Id is for EM miss rule is \n");
	print em_miss[flow_group_id][rule_cnt];
    //}
    ifp_entry_cnt[flow_group_id] = rule_cnt;

    return BCM_E_NONE;
}

/*
 * Function:
 *   first_drop_ifp_entry_create
 * Purpose:
 *      Create IFP rules to copy first drop packets to R5.
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int first_drop_ifp_entry_create(uint8 flow_group_id)
{
    bcm_port_t    port;
    int           rule_cnt = 0;
    uint16        class_id;
    int           rv;

    //BCM_PBMP_ITER(pc.all, port) {

        //if ((port % (NUM_GROUPS + 1)) != flow_group_id) {
        //    continue;
        //}
        /* First Drop pkt */
        rv = ifp_entry_common_setup(ifp_group,
                                    &(first_drop[flow_group_id][rule_cnt]),
                                    flow_group_id, port);
        if (rv !=  BCM_E_NONE) {
            printf("ifp_entry_common_setup() failed rv=%d \n", rv);
            return rv;
        }

        /* Assign group Id as class Id */
        BCM_IF_ERROR_RETURN(
               bcm_field_action_add(unit, first_drop[flow_group_id][rule_cnt],
                                    bcmFieldActionNewClassId, flow_group_id, 0));

        /* Qualify on Drop packets */
        BCM_IF_ERROR_RETURN(
               bcm_field_qualify_Drop(unit, first_drop[flow_group_id][rule_cnt],
                                      1, 0xffffffff));

        /* Copy only packets that hit the EM table */
        BCM_IF_ERROR_RETURN(
                bcm_field_qualify_ExactMatchHitStatus(unit,
                                                      first_drop[flow_group_id][rule_cnt],
                                                      0, 1, 0xFF));

        /* Check if EM Class ID != Drop */
        BCM_IF_ERROR_RETURN(
             bcm_field_qualify_ExactMatchActionClassId(unit,
                                                       first_drop[flow_group_id][rule_cnt],
                                                       0,
                                                       0,
                                                       0x1));

        /* Copy all packets to CPU */
	 /* Attach policer to entry */
        if (ctc_pps != 0) {
            BCM_IF_ERROR_RETURN(
                    bcm_field_entry_policer_attach(unit, first_drop[flow_group_id][rule_cnt],
                                                   0, policer_id));

            /* Copy green packets to CPU */
            BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, first_drop[flow_group_id][rule_cnt],
                                                     bcmFieldActionGpCopyToCpu,
                                                     0, 0));

        } else {
            /* Copy all packets to CPU */
            BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, first_drop[flow_group_id][rule_cnt],
                                                     bcmFieldActionCopyToCpu,
                                                     0, 0));
        }

        /* Copy the packets to cosq mapped to core0 */
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit,
                                                 first_drop[flow_group_id][rule_cnt],
                                                 bcmFieldActionCosQCpuNew,
                                                 DROP_COSQ, 0));

        /* Install the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, first_drop[flow_group_id][rule_cnt]));

    //}

    return BCM_E_NONE;
}

/*
 * Function:
 *   drop_sample_ifp_entry_create
 * Purpose:
 *      Create IFP rules to sample dropped packets to R5.
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int drop_sample_ifp_entry_create(uint8 flow_group_id)
{
    bcm_port_t    port;
    int           rule_cnt = 0;
    uint16        class_id;
    int           rv;

    //BCM_PBMP_ITER(pc.all, port) {

        //if ((port % (NUM_GROUPS + 1)) != flow_group_id) {
          //  continue;
        //}
        /* Drop sample pkt */
        rv = ifp_entry_common_setup(ifp_group,
                                    &(drop_sample[flow_group_id][rule_cnt]),
                                    flow_group_id, port);
        if (rv !=  BCM_E_NONE) {
            printf("ifp_entry_common_setup() failed rv=%d \n", rv);
            return rv;
        }

        /* Assign group Id as class Id, set the MSB for EFP to drop
         * non-sampled packets
         */
        class_id = (1 << 8) | flow_group_id;
	printf("Class id is %d",class_id);
        BCM_IF_ERROR_RETURN(
               bcm_field_action_add(unit, drop_sample[flow_group_id][rule_cnt],
                                    bcmFieldActionNewClassId, class_id, 0));

        /* Qualify on Drop packets */
        BCM_IF_ERROR_RETURN(
               bcm_field_qualify_Drop(unit, drop_sample[flow_group_id][rule_cnt],
                                      1, 0xffffffff));

        /* Copy only packets that hit the EM table */
        BCM_IF_ERROR_RETURN(
                bcm_field_qualify_ExactMatchHitStatus(unit,
                                                      drop_sample[flow_group_id][rule_cnt],
                                                      0, 1, 0xFF));

        /* Check if EM Class ID == Drop */
        BCM_IF_ERROR_RETURN(
             bcm_field_qualify_ExactMatchActionClassId(unit,
                                                       drop_sample[flow_group_id][rule_cnt],
                                                       0,
                                                       BCM_FLOWTRACKER_EXACT_MATCH_CLASS_ID_FLOW_DROP,
                                                       0x1));

        /* Enable Sampling */
        BCM_IF_ERROR_RETURN(
              bcm_field_action_add(unit, drop_sample[flow_group_id][rule_cnt],
                                   bcmFieldActionIngSampleEnable, 0, 0));

        
	 if (ctc_pps != 0) {
            BCM_IF_ERROR_RETURN(
                    bcm_field_entry_policer_attach(unit, drop_sample[flow_group_id][rule_cnt],
                                                  0, policer_id));

         } 			   
        /* Copy the packets to cosq mapped to core0 */
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit,
                                                 drop_sample[flow_group_id][rule_cnt],
                                                 bcmFieldActionCosQCpuNew,
                                                 DROP_COSQ, 0));

        /* Install the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, drop_sample[flow_group_id][rule_cnt]));

        rule_cnt++;
    //}

    return BCM_E_NONE;
}

/*
 * Function:
 *   ifp_entry_delete
 * Purpose:
 *      Delete the IFP rule that was created for the flow group
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int ifp_entry_delete(uint8 flow_group_id)
{
    int i = 0;

    for (i = 0; ifp_entry_cnt[flow_group_id]; i++) {
        BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit,
                                                    em_miss[flow_group_id][i]));
        BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit,
                                                    first_drop[flow_group_id][i]));
        BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit,
                                                    drop_sample[flow_group_id][i]));
    }
    //em_miss_cnt[flow_group_id] = 0;
    return BCM_E_NONE;
}

int collector_create(int id, int *coll_ret_id)
{
    bcm_collector_info_t collector_info;
    bcm_pbmp_t           pbmp, upbmp;
    int                  rv;

    bcm_collector_info_t_init(&collector_info);

    sal_memcpy(collector_info.eth.dst_mac, coll_dst_mac, 6);
    sal_memcpy(collector_info.eth.src_mac, coll_src_mac, 6);

    collector_info.eth.vlan_tag_structure = BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
    collector_info.eth.outer_vlan_tag     = coll_vlan[id];
    collector_info.eth.outer_tpid         = coll_tpid;
    collector_info.ipv4.src_ip            = coll_src_ip;
    collector_info.ipv4.dst_ip            = coll_dst_ip[id];
    collector_info.udp.src_port           = coll_l4_src_port;
    collector_info.udp.dst_port           = coll_l4_dst_port;
    collector_info.transport_type         = bcmCollectorTransportTypeIpv4Udp;
    collector_info.ipfix.observation_domain_id         = 0xabcd;
    collector_info.protobuf.system_id_length       = 10;
    sal_memcpy(collector_info.protobuf.system_id, "1233456789", collector_info.protobuf.system_id_length );

    rv = bcm_collector_create(unit, BCM_COLLECTOR_WITH_ID, &id, &collector_info);
    BCM_IF_ERROR_RETURN(rv);

    *coll_ret_id = id;
    return BCM_E_NONE;
}

int collector_get(bcm_collector_t id)
{
    bcm_collector_info_t collector_info;
    int                  transport_type;
    int                  rv;

    bcm_collector_info_t_init(&collector_info);

    rv = bcm_collector_get(unit, id, &collector_info);
    BCM_IF_ERROR_RETURN(rv);

    transport_type = collector_info.transport_type;

    printf("Transport type      : %d \n", transport_type);
    printf("Dst Mac             : %02x:%02x:%02x:%02x:%02x:%02x \n", collector_info.eth.dst_mac[0],
                                                                     collector_info.eth.dst_mac[1],
                                                                     collector_info.eth.dst_mac[2],
                                                                     collector_info.eth.dst_mac[3],
                                                                     collector_info.eth.dst_mac[4],
                                                                     collector_info.eth.dst_mac[5]);
    printf("Src Mac             : %02x:%02x:%02x:%02x:%02x:%02x \n", collector_info.eth.src_mac[0],
                                                                     collector_info.eth.src_mac[1],
                                                                     collector_info.eth.src_mac[2],
                                                                     collector_info.eth.src_mac[3],
                                                                     collector_info.eth.src_mac[4],
                                                                     collector_info.eth.src_mac[5]);
    printf("Vlan tag structure  : %u \n", collector_info.eth.vlan_tag_structure);
    printf("Outer vlan tag      : %u \n", collector_info.eth.outer_vlan_tag);
    printf("Outer tpid          : 0x%04X \n", collector_info.eth.outer_tpid);
    printf("Src Ip              : 0x%08X \n", collector_info.ipv4.src_ip);
    printf("Dst Ip              : 0x%08X \n", collector_info.ipv4.dst_ip);
    printf("DSCP                : 0x%02X \n", collector_info.ipv4.dscp);
    printf("TTL                 : %u \n", collector_info.ipv4.ttl);
    printf("Src Port            : 0x%04X \n", collector_info.udp.src_port);
    printf("Dst Port            : 0x%04X \n", collector_info.udp.dst_port);
    printf("UDP checksum        : %s \n",
           collector_info.udp.flags & BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE ? "Yes" : "No");

    return BCM_E_NONE;
}

int export_profile_create(int id, int *ret_id)
{
    bcm_collector_export_profile_t profile_info;
    int                            rv;

    bcm_collector_export_profile_t_init(&profile_info);

    profile_info.wire_format = wire_format[id];
    profile_info.export_interval   = 1000000;
    profile_info.max_packet_length = 1500;

    rv = bcm_collector_export_profile_create(unit, 0,
                                             &id, &profile_info);
    BCM_IF_ERROR_RETURN(rv);
    *ret_id = id;

    return BCM_E_NONE;
}

int export_profile_get(int id)
{
    bcm_collector_export_profile_t profile_info;
    int                            wire_format;
    int                            rv;

    bcm_collector_export_profile_t_init(&profile_info);

    rv = bcm_collector_export_profile_get(unit, id, &profile_info);
    BCM_IF_ERROR_RETURN(rv);

    wire_format = profile_info.wire_format;
    printf("Wire Format     : %d \n", wire_format);
    printf("Export Interval : %u usecs \n", profile_info.export_interval);
    printf("MTU             : %u \n", profile_info.max_packet_length);

    return rv;
}

/*
 * Function:
 *   flow_limit_set
 * Purpose:
 *      Set the flow limit on a group
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 *        flow_limit    - (IN) flow limit
 * Notes:
 */
int flow_limit_set(bcm_flowtracker_group_t flow_group_id, uint32 flow_limit)
{
    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_flow_limit_set(unit, flow_group_id,
                                                                  flow_limit));
    return BCM_E_NONE;
}

/*
 * Function:
 *   group_coll_attach
 * Purpose:
 *      Attach a collector & template to a group
 * Parameters:
 *        flow_group_id   - (IN) Flow group Id
 *
 * Notes:
 */
int group_coll_attach(bcm_flowtracker_group_t flow_group_id, int index)
{
    BCM_IF_ERROR_RETURN(
             bcm_flowtracker_group_collector_attach(unit, flow_group_id,
                                                    coll_id[index], export_profile_id[index],
                                                    tmpl_id));
    return BCM_E_NONE;
}

/*
 * Function:
 *   group_coll_detach
 * Purpose:
 *      Detach collector and template from group
 * Parameters:
 *        flow_group_id   - (IN) Flow group Id
 * Notes:
 *      Since only one collector and template is supported, there is no use case
 *      to call this API
 */
int group_coll_detach(bcm_flowtracker_group_t flow_group_id, int index)
{
    BCM_IF_ERROR_RETURN(
             bcm_flowtracker_group_collector_detach(unit, flow_group_id,
                                                    coll_id[index], export_profile_id[index],
                                                    tmpl_id));
    return BCM_E_NONE;
}

/*
 * Function:
 *   template_create
 * Purpose:
 *      Create a template
 * Notes:
 */
int template_create()
{
    bcm_flowtracker_export_element_info_t elements[7] = {
        {0, bcmFlowtrackerExportElementTypeInnerSrcIPv4,              4, 1},  /* IPv4 Src IP */
        {0, bcmFlowtrackerExportElementTypeInnerDstIPv4,              4, 2},  /* IPv4 Dst IP */
        {0, bcmFlowtrackerExportElementTypeInnerL4SrcPort,            2, 3},  /* L4 Src Port */
        {0, bcmFlowtrackerExportElementTypeInnerL4DstPort,            2, 4},  /* L4 Dst Port */
        {0, bcmFlowtrackerExportElementTypeInnerIPProtocol,           1, 5},  /* IPv4 protocol */
        {0, bcmFlowtrackerExportElementTypePktCount,             4, 6},  /* 32 bit packet counter */
        {0, bcmFlowtrackerExportElementTypeByteCount,            6, 7},  /* 48 bit byte counter */
    };

    BCM_IF_ERROR_RETURN(
            bcm_flowtracker_export_template_create(unit, 0, &tmpl_id,
                                                   set_id, 7, &elements));
    printf ("Template ID created: %d:\r\n", tmpl_id);
    return BCM_E_NONE;
}

int template_xmit_start()
{  
    bcm_flowtracker_template_transmit_config_t cfg;

    bcm_flowtracker_template_transmit_config_t_init(&cfg);

    cfg.retransmit_interval_secs = 1;
    cfg.initial_burst            = 5;
    BCM_IF_ERROR_RETURN(
           bcm_flowtracker_template_transmit_config_set(unit, tmpl_id, coll_id, &cfg));

    return BCM_E_NONE;
}

/*
 * Function:
 *      template_xmit_get
 * Purpose:
 *      Get template xmit config
 * Parameters:
 *      None
 * Notes:
 */
int template_xmit_get()
{
    bcm_flowtracker_template_transmit_config_t cfg;

    bcm_flowtracker_template_transmit_config_t_init(&cfg);

    BCM_IF_ERROR_RETURN(
               bcm_flowtracker_template_transmit_config_get(unit, 1, 1, &cfg));

    printf("Interval  : %d \n", cfg.retransmit_interval_secs);
    printf("Burst     : %d \n", cfg.initial_burst);

    return BCM_E_NONE;
}

/*
 * Function:
 *   coll_tmpl_attach
 * Purpose:
 *      Attach a collector & template to a group
 * Parameters:
 *        flow_group_id   - (IN) Flow group Id
 *
 * Notes:
 */
int coll_tmpl_get(bcm_flowtracker_group_t flow_group_id)
{
    int list_size;
    bcm_flowtracker_collector_t collectors[1];
    int export_profiles[1];
    bcm_flowtracker_export_template_t templates[1];

    BCM_IF_ERROR_RETURN(
             bcm_flowtracker_group_collector_attach_get_all(unit, flow_group_id,
                                                            0, NULL, NULL, NULL,
                                                            &list_size));

    printf("Num collectors    : %d \n", list_size);
    if (list_size != 0) {
        BCM_IF_ERROR_RETURN(
                   bcm_flowtracker_group_collector_attach_get_all(unit, flow_group_id,
                                                                  1, collectors,
                                                                  export_profiles,
                                                                  templates,
                                                                  &list_size));
        printf("Collector Id      : %d \n", collectors[0]);
        printf("Export profile Id : %d \n", export_profiles[0]);
        printf("Template id       : %d \n", templates[0]);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   aging_interval_set
 * Purpose:
 *      Set the aging interval for a group
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 *        aging_interval - (IN) Aging interval in msecs
 * Notes:
 */
int aging_interval_set(bcm_flowtracker_group_t flow_group_id, uint32 aging_interval)
{
   BCM_IF_ERROR_RETURN(bcm_flowtracker_group_age_timer_set(unit, flow_group_id,
                                                           aging_interval));
   return BCM_E_NONE;
}

/*
 * Function:
 *   flow_group_create
 * Purpose:
 *      Create a flow group
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 *        field_groupd   - (IN) EM group Id
 *        default_group  - (IN) 1 - Default group
 * Notes:
 */
int flow_group_create(bcm_flowtracker_group_t flow_group_id,
                      bcm_field_group_t *field_groups, int default_group)
{
    bcm_flowtracker_group_info_t          flow_group_info;
    uint32                                options = 0;

    options = BCM_FLOWTRACKER_GROUP_WITH_ID;


    bcm_flowtracker_group_info_t_init(&flow_group_info);
    flow_group_info.field_group[0] = field_groups[0];
    flow_group_info.field_group[1] = field_groups[1];
    flow_group_info.field_group[2] = field_groups[2];
    flow_group_info.field_group[3] = field_groups[3];

    if (default_group) {
        flow_group_info.group_flags |= BCM_FLOWTRACKER_GROUP_DEFAULT;
    }

    BCM_IF_ERROR_RETURN(
                 bcm_flowtracker_group_create(unit, options,
                                              &flow_group_id,
                                              &flow_group_info));

    return BCM_E_NONE;
}


/*
 * Function:
 *   flow_group_destroy
 * Purpose:
 *      Delete a group
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int flow_group_destroy(bcm_flowtracker_group_t flow_group_id)
{
    BCM_IF_ERROR_RETURN(ifp_entry_delete(flow_group_id));
    BCM_IF_ERROR_RETURN(flow_limit_set(flow_group_id, 0));
    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_destroy(unit, flow_group_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *   flow_group_clear_all
 * Purpose:
 *      Clear all flows in a group
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int flow_group_clear_all(bcm_flowtracker_group_t flow_group_id)
{
    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_clear(unit, flow_group_id,
                                                    BCM_FLOWTRACKER_GROUP_CLEAR_ALL));
    return BCM_E_NONE;
}

/*
 * Function:
 *      flow_group_clear_data
 * Purpose:
 *      Clear the packet and byte count of all flows in a group
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int flow_group_clear_data(bcm_flowtracker_group_t flow_group_id)
{
    BCM_IF_ERROR_RETURN(
             bcm_flowtracker_group_clear(unit, flow_group_id,
                                         BCM_FLOWTRACKER_GROUP_CLEAR_FLOW_DATA_ONLY));
    return BCM_E_NONE;
}

/*
 * Function:
 *      flow_group_get
 * Purpose:
 *      Get details pertaining to a flow group
 * Parameters:
 *        flow_group_id - (IN) Flow group Id
 * Notes:
 */
int flow_group_get(bcm_flowtracker_group_t flow_group_id)
{
   bcm_flowtracker_group_info_t           flow_group_info;
   uint32                                 flow_limit, flow_count;
   uint32                                 aging_interval;
   bcm_flowtracker_group_info_t_init(&flow_group_info);

   /* Get observation domain Id */
   BCM_IF_ERROR_RETURN(bcm_flowtracker_group_get(unit, flow_group_id,
                                                 &flow_group_info));

   /* Get flow limit */
   BCM_IF_ERROR_RETURN(bcm_flowtracker_group_flow_limit_get(unit, flow_group_id,
                                                            &flow_limit));

   /* Get the number of flows learnt on the group */
   BCM_IF_ERROR_RETURN(bcm_flowtracker_group_flow_count_get(unit, flow_group_id,
                                                            &flow_count));

   /* Get the aging interval */
   BCM_IF_ERROR_RETURN(bcm_flowtracker_group_age_timer_get(unit, flow_group_id,
                                                            &aging_interval));

   printf("Group Id              : %d \n", flow_group_id);
   printf("Flags                 : 0x%08x \n", flow_group_info.group_flags);
   //printf("Field Group Id        : %d \n", flow_group_info.field_group);
   printf("Observation Domain Id : 0x%08x \n", flow_group_info.observation_domain_id);
   printf("Flow Limit            : %u \n", flow_limit);
   printf("Flow Count            : %u \n", flow_count);
   printf("Aging Interval        : %u \n", aging_interval);
   printf("\n");

   return BCM_E_NONE;
}

/*
 * Function:
 *      flow_group_get_all
 * Purpose:
 *      Get details pertaining to all flow groups
 * Parameters:
 *      None
 * Notes:
 */
int flow_group_get_all()
{
    bcm_flowtracker_group_t flow_group_id;

    for (flow_group_id = 1; flow_group_id <= NUM_GROUPS; flow_group_id++) {
        BCM_IF_ERROR_RETURN(flow_group_get(flow_group_id));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      vxlan_flow_data_get
 * Purpose:
 *      Get details of a vxlan flow
 *
 * Parameters: None
 *
 */
int vxlan_flow_data_get()
{
    bcm_flowtracker_flow_key_t  key;
    bcm_flowtracker_flow_data_t data;
    int                         pkt_count, byte_count;

    key.inner_src_ip.addr.ipv4_addr = 0xAABBCCDD;
    key.inner_dst_ip.addr.ipv4_addr = 0x0C0C0D0D;
    key.inner_l4_src_port           = 0xCE4D;
    key.inner_l4_dst_port           = 0xAB89;
    key.vxlan_network_id            = 0x12345;

    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_data_get(unit, 1,
                                                       &key, &data));

    /* C-INT printf does not handle uint64, convert lower 32 bytes to int */
    COMPILER_64_TO_32_LO(pkt_count, data.packet_count);
    COMPILER_64_TO_32_LO(byte_count, data.byte_count);

    printf("pkt count    : %8u \n", pkt_count);
    printf("byte count   : %8u \n", byte_count);

    return BCM_E_NONE;
}

/*
 * Function:
 *      ipv4_flow_data_get
 * Purpose:
 *      Get details of an ipv4 flow
 *
 * Parameters: None
 *
 */
int ipv4_flow_data_get(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_flow_key_t  key;
    bcm_flowtracker_flow_data_t data;
    int                         pkt_count, byte_count;

    key.src_ip.addr.ipv4_addr = 0x0B0B0100;
    key.dst_ip.addr.ipv4_addr = 0x0A0A0200;
    key.l4_src_port           = 0x1F00;
    key.l4_dst_port           = 0x1F69;
    key.ip_protocol           = 0x11;

    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_data_get(unit, flow_group_id,
                                                       &key, &data));

    /* C-INT printf does not handle uint64, convert lower 32 bytes to int */
    COMPILER_64_TO_32_LO(pkt_count, data.packet_count);
    COMPILER_64_TO_32_LO(byte_count, data.byte_count);

    printf("pkt count    : %8u \n", pkt_count);
    printf("byte count   : %8u \n", byte_count);

    return BCM_E_NONE;
}

/*
 * Function:
 *      prop_vxlan_flow_data_get
 * Purpose:
 *      Get details of a proprietary vxlan flow
 *
 * Parameters: None
 *
 */
int prop_vxlan_flow_data_get()
{
    bcm_flowtracker_flow_key_t  key;
    bcm_flowtracker_flow_data_t data;
    int                         pkt_count, byte_count;

    key.src_ip.addr.ipv4_addr = 0x0A0A0101;
    key.dst_ip.addr.ipv4_addr = 0x0A0A0200;
    key.l4_src_port           = 0x003E;
    key.l4_dst_port           = 0x0FFF;
    key.custom[0]             = 0x01;
    key.custom[1]             = 0x23;
    key.custom[2]             = 0x45;

    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_data_get(unit, 3,
                                                       &key, &data));

    /* C-INT printf does not handle uint64, convert lower 32 bytes to int */
    COMPILER_64_TO_32_LO(pkt_count, data.packet_count);
    COMPILER_64_TO_32_LO(byte_count, data.byte_count);

    printf("pkt count    : %8u \n", pkt_count);
    printf("byte count   : %8u \n", byte_count);

    return BCM_E_NONE;
}

/*
 * Function:
 *   vxlan_tracking_params_set
 * Purpose:
 *      Set vxlan tracking params on a group
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int vxlan_tracking_params_set(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_tracking_param_info_t tracking_params[6];

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[0]));
    tracking_params[0].param = bcmFlowtrackerTrackingParamTypeVxlanNetworkId;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[1]));
    tracking_params[1].param = bcmFlowtrackerTrackingParamTypeInnerSrcIPv4;
    tracking_params[1].udf_id = vxlan_udf_id[0];
    printf("UDF ID 1 is %d",vxlan_udf_id[0]);

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[2]));
    tracking_params[2].param = bcmFlowtrackerTrackingParamTypeInnerDstIPv4;
    tracking_params[2].udf_id = vxlan_udf_id[1];
    printf("UDF ID 2 is %d",vxlan_udf_id[1]);
    
    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[3]));
    tracking_params[3].param = bcmFlowtrackerTrackingParamTypeInnerL4SrcPort;
    tracking_params[3].udf_id = vxlan_udf_id[2];
    
    printf("UDF ID 3 is %d",vxlan_udf_id[2]);
    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[4]));
    tracking_params[4].param = bcmFlowtrackerTrackingParamTypeInnerL4DstPort;
    tracking_params[4].udf_id = vxlan_udf_id[3];
    printf("UDF ID 4 is %d",vxlan_udf_id[3]);

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[5]));
    tracking_params[5].param = bcmFlowtrackerTrackingParamTypeInnerIPProtocol;
    tracking_params[5].udf_id = vxlan_udf_id[4];
    printf("UDF ID 5 is %d",vxlan_udf_id[4]);

    BCM_IF_ERROR_RETURN(
           bcm_flowtracker_group_tracking_params_set(unit,
                                                     flow_group_id, 6,
                                                     tracking_params));
    return BCM_E_NONE;
}

/*
 * Function:
 *   ipv4_tracking_params_set
 * Purpose:
 *      Set ipv4 tracking params on a group
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int ipv4_tracking_params_set(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_tracking_param_info_t tracking_params[6];

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[0]));
    tracking_params[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[1]));
    tracking_params[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[2]));
    tracking_params[2].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[3]));
    tracking_params[3].param = bcmFlowtrackerTrackingParamTypeL4DstPort;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[4]));
    tracking_params[4].param = bcmFlowtrackerTrackingParamTypeIPProtocol;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[5]));
    tracking_params[5].param = bcmFlowtrackerTrackingParamTypeIngPort;

    BCM_IF_ERROR_RETURN(
           bcm_flowtracker_group_tracking_params_set(unit,
                                                     flow_group_id, 6,
                                                     tracking_params));
    return BCM_E_NONE;
}

/*
 * Function:
 *   prop_vxlan_tracking_params_set
 * Purpose:
 *      Set proprietary vxlan tracking params on a group
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int prop_vxlan_tracking_params_set(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_tracking_param_info_t tracking_params[6];

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[0]));
    tracking_params[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[1]));
    tracking_params[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[2]));
    tracking_params[2].param = bcmFlowtrackerTrackingParamTypeIPProtocol;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[3]));
    tracking_params[3].param  = bcmFlowtrackerTrackingParamTypeCustom;
    tracking_params[3].udf_id = prop_vxlan_udf_id;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[4]));
    tracking_params[4].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;

    bcm_flowtracker_tracking_param_info_t_init(&(tracking_params[5]));
    tracking_params[5].param = bcmFlowtrackerTrackingParamTypeL4DstPort;


    BCM_IF_ERROR_RETURN(
           bcm_flowtracker_group_tracking_params_set(unit,
                                                     flow_group_id, 6,
                                                     tracking_params));
    return BCM_E_NONE;
}

/*
 * Function:
 *   tracking_params_get
 * Purpose:
 *      Get tracking params on a group
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int tracking_params_get(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_tracking_param_info_t tracking_params[10];
    int                                   num_params;
    int                                   param;
    int                                   i;

    BCM_IF_ERROR_RETURN(
           bcm_flowtracker_group_tracking_params_get(unit, flow_group_id,
                                                     0, NULL, &num_params));

    printf("Num tracking params   : %d \n", num_params);

    BCM_IF_ERROR_RETURN(
           bcm_flowtracker_group_tracking_params_get(unit, flow_group_id,
                                                     num_params, tracking_params,
                                                     &num_params));

    for (i = 0; i < num_params; i++) {
        param = tracking_params[i].param;
        printf("\tParam     : %d \n", param);
        printf("\tUDF Id    : %d \n", tracking_params[i].udf_id);
        printf("\n");
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   export_trigger_set
 * Purpose:
 *      Set the export triggers
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int export_trigger_set(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_export_trigger_info_t trigger;

    sal_memset(&trigger, 0, sizeof(trigger));

    BCM_FLOWTRACKER_TRIGGER_CLEAR_ALL(trigger);
    BCM_FLOWTRACKER_TRIGGER_SET(trigger, bcmFlowtrackerExportTriggerTimer);
    BCM_FLOWTRACKER_TRIGGER_SET(trigger, bcmFlowtrackerExportTriggerNewLearn);
    BCM_FLOWTRACKER_TRIGGER_SET(trigger, bcmFlowtrackerExportTriggerAgeOut);
    BCM_FLOWTRACKER_TRIGGER_SET(trigger, bcmFlowtrackerExportTriggerDrop);

    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_export_trigger_set(unit,
                                                                 flow_group_id,
                                                                 &trigger));

    return BCM_E_NONE;
}

/*
 * Function:
 *   export_trigger_delete
 * Purpose:
 *      Delete the export triggers
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int export_trigger_delete(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_export_trigger_info_t trigger;

    sal_memset(&trigger, 0, sizeof(trigger));

    BCM_FLOWTRACKER_TRIGGER_CLEAR_ALL(trigger);

    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_export_trigger_set(unit,
                                                                 flow_group_id,
                                                                 &trigger));

    return BCM_E_NONE;
}

/*
 * Function:
 *   export_trigger_get
 * Purpose:
 *      Get the export triggers
 * Parameters:
 *        flow_group_id  - (IN) Flow group Id
 * Notes:
 */
int export_trigger_get(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_export_trigger_info_t trigger;
    int                                   i;

    sal_memset(&trigger, 0, sizeof(trigger));
    BCM_IF_ERROR_RETURN(bcm_flowtracker_group_export_trigger_get(unit,
                                                                 flow_group_id,
                                                                 &trigger));

    printf("Triggers  : ");
    for (i = 0; i < bcmFlowtrackerExportTriggerCount; i++) {
        if (BCM_FLOWTRACKER_TRIGGER_GET(trigger, i)) {
            printf("%d ", i);
        }
    }
    printf("\n");

    return BCM_E_NONE;
}
/*
 * Function:
 *      group_actions_set
 * Purpose:
 *      Set group actions
 * Parameters:
 *      flow_group_id - (IN) Flow group Id
 * Notes:
 */
int group_actions_set(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_group_action_info_t action_list[1];
    uint32                              flags;

    flags = 0;
    /*bcmFlowtrackerGroupActionDropMonitor*/
    /*bcmFlowtrackerGroupActionFspReinject*/
    action_list[0].action = bcmFlowtrackerGroupActionDropMonitor;
    action_list[0].params.param0 = 0;
    action_list[0].params.param1 = 47;


    BCM_IF_ERROR_RETURN(
        bcm_flowtracker_group_actions_set(unit, flow_group_id,
                                          flags, 1, action_list));

    return BCM_E_NONE;
}

/*
 * Function:
 *      group_actions_delete
 * Purpose:
 *      Delete actions on a group
 * Parameters:
 *      flow_group_id - (IN) Flow group Id
 * Notes:
 */
int group_actions_delete(bcm_flowtracker_group_t flow_group_id)
{
    uint32  flags = 0;

    BCM_IF_ERROR_RETURN(
        bcm_flowtracker_group_actions_set(unit, flow_group_id,
                                          flags, 0, NULL));

    return BCM_E_NONE;
}

/*
 * Function:
 *      group_actions_get
 * Purpose:
 *      Get actions on a group
 * Parameters:
 *      flow_group_id - (IN) Flow group Id
 * Notes:
 */
int group_actions_get(bcm_flowtracker_group_t flow_group_id)
{
    bcm_flowtracker_group_action_info_t action_list[MAX_GROUP_ACTIONS];
    uint32                              flags;
    int                                 i;
    int                                 num_actions;
    int                                 action;

    flags = 0;

    BCM_IF_ERROR_RETURN(
        bcm_flowtracker_group_actions_get(unit, flow_group_id, flags,
                                          0, NULL,
                                          &num_actions));

    printf("Num actions   : %d \n", num_actions);

    BCM_IF_ERROR_RETURN(
        bcm_flowtracker_group_actions_get(unit, flow_group_id, flags,
                                          num_actions, action_list,
                                          &num_actions));
    for (i = 0; i < num_actions; i++) {
        action = action_list[i].action;
        printf("\n");
        printf("Action        : %d \n", action);
        printf("param0        : %d \n", action_list[i].params.param0);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   fp_catch_all
 * Purpose:
 *      Create catch all rules
 * Parameters:
 *        None
 * Notes:
 */
int fp_catch_all()
{
    bcm_field_qset_t          qset;
    bcm_field_group_t         group;
    bcm_field_entry_t         entry,entry2;
    
    bcm_policer_config_t_init(&efp_policer_config);
    efp_policer_config.flags      = BCM_POLICER_MODE_PACKETS;
    efp_policer_config.mode       = bcmPolicerModeCommitted;
    efp_policer_config.ckbits_sec = ctc_pps;
    efp_policer_config.ckbits_burst = 1;
    BCM_IF_ERROR_RETURN(bcm_policer_create(unit, &efp_policer_config,
                                               &efp_policer_id));

    /* EFP rule to drop non-sampled packets */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOutPort);

    BCM_IF_ERROR_RETURN(
          bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IngressClassField(unit, entry, 0x100, 0x100));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_MirrorCopy(unit, entry, 0, 1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OutPort(unit, entry, CPU_PORT, -1));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    
     //Add additional EFP rule to police sampled packets at EFP
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry2));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IngressClassField(unit, entry2, 0x100, 0x100));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_MirrorCopy(unit, entry2, 1, 1));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OutPort(unit, entry2, CPU_PORT, -1)); 
    BCM_IF_ERROR_RETURN(bcm_field_entry_policer_attach(unit, entry2,0, efp_policer_id));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry2, bcmFieldActionYpDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry2, bcmFieldActionRpDrop, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry2));
    printf("Entry ID for Droppping non-sampled packets is %d \n",entry);
    printf("Entry ID for Policing sampled packets is %d\n",entry2);

    return BCM_E_NONE;
}

/*
 * Function:
 *   ifp_entry_common_setup
 * Purpose:
 *      Setup the common part of IFP rule
 * Parameters:
 *        group         - (IN)    Field group
 *        entry         - (INOUT) Field entry
 *        flow_group_id - (IN)    Flow group Id
 *        port          - (IN)    In Port
 * Notes:
 */
int ifp_entry_collector_copy_to_cpu(bcm_field_group_t group, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &collector_copy_to_cpu));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, collector_copy_to_cpu,
                                                 port,
                                                 0xFFFFFFFF));

        BCM_IF_ERROR_RETURN(
          bcm_field_action_add(unit, collector_copy_to_cpu,
                               bcmFieldActionCopyToCpu, 1, 1));
    BCM_IF_ERROR_RETURN(
          bcm_field_action_add(unit, collector_copy_to_cpu,
                               bcmFieldActionDrop, 0, 0));
	BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, collector_copy_to_cpu));

    return BCM_E_NONE;
}

int create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port, bcm_gport_t *gport)
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
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_vlan_create() for vlan %d: %s.\n", vlan, bcm_errmsg(rv));
			return rv;
	}
	
	BCM_PBMP_CLEAR(pbmp);
	BCM_PBMP_CLEAR(upbmp);
	BCM_PBMP_PORT_ADD(pbmp, port);
	rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_vlan_port_add() for vlan %d, port %d: %s.\n", vlan, port, bcm_errmsg(rv));
			return rv;
	}
	
	printf("create_vlan_add_port() run sucessfully for vlan %d  port %d\n", vlan, port);
	return BCM_E_NONE;	
}

/* Create L3 interface */
int create_l3_intf_my_station(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t l3_intf_id, bcm_vrf_t vrf)
{
	bcm_error_t rv;
	bcm_l3_intf_t l3_intf;
	
	bcm_l3_intf_t_init(&l3_intf);
	l3_intf.l3a_flags =       BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
	l3_intf.l3a_intf_id =     l3_intf_id;	
	l3_intf.l3a_vid =         vlan;
	l3_intf.l3a_vrf =         vrf;
	sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));
	rv = bcm_l3_intf_create(unit, &l3_intf);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_l3_intf_create() for my station for vlan %d vrf %d : %s.\n", vlan, vrf, bcm_errmsg(rv));
			return rv;
	}
	
	printf("create_l3_intf_my_station() run sucessfully for vlan %d  vrf %d\n", vlan, vrf);
  return BCM_E_NONE;
}

/* Create L3 interface */
int create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t l3_intf_id, bcm_vrf_t vrf)
{
	bcm_error_t rv;
	bcm_l3_intf_t l3_intf;

	bcm_l3_intf_t_init(&l3_intf);
	l3_intf.l3a_flags = 		BCM_L3_WITH_ID;
	l3_intf.l3a_intf_id = 	l3_intf_id;
	l3_intf.l3a_vid = 			vlan;
	l3_intf.l3a_vrf =       vrf;
	sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

	rv = bcm_l3_intf_create(unit, &l3_intf);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_l3_intf_create() for egress if. vlan %d: %s.\n", vlan, bcm_errmsg(rv));
			return rv;
	}
	
	printf("create_l3_intf() run sucessfully for egress if. vlan %d \n", vlan);
  return BCM_E_NONE;
}

/* Create L3 egress object */
int create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport, bcm_if_t *egr_if)
{
	bcm_error_t rv;
	bcm_l3_egress_t l3_egr;

	bcm_l3_egress_t_init(&l3_egr);
	l3_egr.intf = l3_if;
	l3_egr.port = gport;
	sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));
	rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_if);

	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_l3_egress_create() for egress if.  %s.\n",  bcm_errmsg(rv));
			return rv;
	}
	
	printf("create_egr_obj() run sucessfully for egress if. \n");
  return BCM_E_NONE;
}

int l3_host_config(int unit, bcm_vrf_t vrf, bcm_if_t egr_if, bcm_ip_t dest_ip)
{
	bcm_error_t rv;
	bcm_l3_host_t hinfo;
	
	bcm_l3_host_t_init(hinfo);
	hinfo.l3a_vrf     = vrf;
	hinfo.l3a_intf    = egr_if;
	hinfo.l3a_ip_addr = dest_ip;
	rv = bcm_l3_host_add(unit, &hinfo);
	
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_l3_host_add() for vrf %d: %s.\n", vrf, bcm_errmsg(rv));
			return rv;
	}
	
	printf("l3_host_config() run sucessfully \n");
  return BCM_E_NONE;
}

int switch_control_set()
{
	bcm_error_t rv;
	rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_switch_control_set for  bcmSwitchL3EgressMode: %s.\n", bcm_errmsg(rv));
			return rv;
	}
	
	rv = bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_vlan_control_set for  bcmVlanTranslate: %s.\n", bcm_errmsg(rv));
			return rv;
	}
	/*
	rv = bcm_switch_control_set(unit, bcmSwitchIntL4DestPort1, int_udp_port); 
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_switch_control_set for  bcmSwitchIntL4DestPort1: %s.\n", bcm_errmsg(rv));
			return rv;
	}
	*/
	rv = bcm_port_control_set(unit, 0, bcmPortControlIP4, 1);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_switch_control_set for bcmPortControlIP4: %s.\n", bcm_errmsg(rv));
			return rv;
	}
	int max_payload_len = 1400;
	rv = bcm_switch_control_set(unit, bcmSwitchIntMaxPayloadLength,max_payload_len);
	if (BCM_FAILURE(rv)) {
			printf("Error in bcm_switch_control_set for bcmSwitchIntMaxPayloadLength: %s.\n", bcm_errmsg(rv));
			return rv;
	}
	
	
	printf("switch_control_set() run sucessfully\n");
	return BCM_E_NONE;
}

void run()
{
    int rv;
    int i, j;
    bcm_collector_wire_format_t wire_format = bcmCollectorWireFormatProtoBuf;

    rv = setup();
    if (rv != BCM_E_NONE) {
        printf("setup() failed rv=%d \n", rv);
        return rv;
    }

    rv = fp_catch_all();
    if (rv != BCM_E_NONE) {
        printf("fp_catch_all() failed i=%d rv=%d \n", i, rv);
        return;
    }

    rv = vfp_src_class_set();
    if (rv != BCM_E_NONE) {
        printf("vfp_src_class_set() failed i=%d rv=%d \n", i, rv);
        return;
    }

    rv = presel_create();
    if (rv != BCM_E_NONE) {
        printf("presel_create() failed rv=%d \n", rv);
        return;
    }


    rv = udf_create();
    if (rv != BCM_E_NONE) {
        printf("udf_create() failed rv=%d \n", rv);
        return;
    }

    rv = ipv4_em_group_create(1);
    if (rv != BCM_E_NONE) {
        printf("ipv4_em_group_create() failed rv=%d \n", rv);
        return;
    }

    /*rv = vxlan_em_group_create(2);
    if (rv != BCM_E_NONE) {
        printf("vxlan_em_group_create() failed rv=%d \n", rv);
        return;
    }

    rv = prop_vxlan_em_group_create(3);
    if (rv != BCM_E_NONE) {
        printf("prop_vxlan_em_group_create() failed rv=%d \n", rv);
        return;
    }*/

    rv = ifp_group_create();
    if (rv != BCM_E_NONE) {
        printf("em_miss_ifp_group_create() failed rv=%d \n", rv);
        return;
    }


    for (i = 1; i <= NUM_GROUPS; i++) {
        rv = flow_group_create(i, em_groups[i], 0);
        if (rv != BCM_E_NONE) {
            printf("flow_group_create() failed i=%d rv=%d \n", i, rv);
            return;
        }
    }

    /* Create default group */
    rv = flow_group_create(NUM_GROUPS+1, 0, 1);
    if (rv != BCM_E_NONE) {
        printf("Default flow_group_create() failed i=%d rv=%d \n", i, rv);
        return;
    }

    rv = ipv4_tracking_params_set(1);
    if (rv != BCM_E_NONE) {
        printf("ipv4_tracking_params_set() failed rv=%d \n", rv);
        return;
    }

    /*rv = vxlan_tracking_params_set(2);
    if (rv != BCM_E_NONE) {
        printf("vxlan_tracking_params_set() failed rv=%d \n", rv);
        return;
    }

    rv = prop_vxlan_tracking_params_set(3);
    if (rv != BCM_E_NONE) {
        printf("prop_vxlan_tracking_params_set() failed rv=%d \n", rv);
        return;
    }*/

    for (i = 1; i <= NUM_GROUPS; i++) {
        rv = aging_interval_set(i, aging_interval_msecs);
        if (rv != BCM_E_NONE) {
            printf("aging_interval_set() failed i=%d rv=%d \n", i, rv);
            return;
        }
    }

    printf("Creating Collector configurations\r\n");
    for (i = 0; i < NUM_COLLECTORS; i++) {
        rv = collector_create(i, &coll_id[i]);
        if (rv != BCM_E_NONE) {
            printf("collector_create() failed for i %d rv=%d \n", i, rv);
        }
        printf("Collector ID[%d]=%d\r\n",i, coll_id[i]);

        rv = export_profile_create(i, &export_profile_id[i]);
        if (rv != BCM_E_NONE) {
            printf("export_profile_create() failed rv=%d for id %d\n", rv, export_profile_id1);
        }
    }

    printf("Completed Collector configurations\r\n");

    rv = template_create();
    if (rv != BCM_E_NONE) {
        printf("template_create() failed rv=%d \n", rv);
    }

    /*rv = template_xmit_start();
    if (rv != BCM_E_NONE) {
        printf("template_xmit_start() failed rv=%d \n", rv);
        return;
    }*/

    for (i = 1; i <= NUM_GROUPS; i++) {
        for (j = 0; j < NUM_COLLECTORS; j++) {
            rv = group_coll_attach(i, j);
            if (rv != BCM_E_NONE) {
                printf("group_coll_attach() failed group id=%d rv=%d coll idx %d\n", i, rv, j);
                return;
            }
        }
    }

    for (i = 1; i <= NUM_GROUPS; i++) {
        rv = flow_limit_set(i, group_flow_limit);
        if (rv != BCM_E_NONE) {
            printf("flow_limit_set() failed i=%d rv=%d \n", i, rv);
            return;
        }
    }

    for (i = 1; i <= NUM_GROUPS; i++) {
        rv = em_miss_ifp_entry_create(i);
        if (rv != BCM_E_NONE) {
            printf("ifp_entry_create() failed rv=%d \n", rv);
            return;
        }
    
        
        /*rv = first_drop_ifp_entry_create(i);
        if (rv != BCM_E_NONE) {
            printf("first_drop_ifp_group_create() failed rv=%d \n", rv);
            return;
        }*/

        rv = drop_sample_ifp_entry_create(i);
        if (rv != BCM_E_NONE) {
            printf("drop_sample_ifp_group_create() failed rv=%d \n", rv);
            return;
        }
        
    }
     
    for (i = 1; i <= NUM_GROUPS; i++) {
        rv = group_actions_set(i);
        if (rv != BCM_E_NONE) {
            printf("group_actions_set() failed rv=%d \n", rv);
            return;
        }
    }
    
     
    for (i = 1; i <= NUM_GROUPS; i++) {
        rv = export_trigger_set(i);
        if (rv != BCM_E_NONE) {
            printf("export_trigger_set() failed rv=%d \n", rv);
            return;
        }
    }
    
    for (i = 0; i < NUM_COLLECTORS; i++) {
        rv = ifp_entry_collector_copy_to_cpu(ifp_group,coll_port[i]) ;
        if (rv != BCM_E_NONE) {
            printf("ifp_entry_collector_copy_to_cpu() failed rv=%d \n", rv);
            return;
        }
    }
    print switch_control_set();
    print create_vlan_add_port(unit, vid_acc_ingress, port_acc_ingress, &gport_acc_ingress);
    print create_vlan_add_port(unit, vid_network_ingress, port_network_ingress, &gport_network_ingress);
    print create_l3_intf_my_station(unit, pkt_in_dst_mac_ingress, vid_acc_ingress, intf_id_acc_ingress, vrf_ingress);
    print create_l3_intf_my_station(unit, local_mac_network_ingress, vid_network_ingress, intf_id_network_ingress, vrf_ingress);
    print create_egr_obj(unit, intf_id_network_ingress, remote_mac_network_ingress, gport_network_ingress, &egr_obj_network_ingress);
    //print l3_host_config(unit, vrf_ingress, egr_obj_network_ingress, pkt_in_dst_ip);
}

run();


