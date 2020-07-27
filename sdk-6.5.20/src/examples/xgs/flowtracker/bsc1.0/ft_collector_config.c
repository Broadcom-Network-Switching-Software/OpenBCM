/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


const int FTE_MAX_INFO_ELEMENTS = 40;

/* FT Export global params  */
bcm_vlan_t    fte_coll_vlan        = 0x600;
bcm_mac_t     fte_coll_src_mac     = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
uint16        fte_coll_tpid        = 0x8100;
bcm_ip_t      fte_coll_src_ip      = 0x0a82562a;
bcm_ip_t      fte_coll_dst_ip      = 0x0a82b00a;
uint8         fte_coll_dscp        = 0xE1;
uint8         fte_coll_ttl         = 9;
uint16        fte_coll_mtu         = 1500;
uint16        fte_set_id           = 0x1efe;
bcm_l4_port_t fte_coll_l4_src_port = 0x1F91;
bcm_l4_port_t fte_coll_l4_dst_port = 0x0807;

bcm_port_t fte_coll_port = 0;
bcm_flowtracker_collector_t fte_coll_id;
int fte_export_profile_id;
bcm_flowtracker_export_template_t fte_tmpl_id;



int fte_collector_setup(bcm_vlan_t fte_coll_vlan, bcm_mac_t fte_coll_dst_mac)
{
    int unit = 0;
    bcm_pbmp_t        pbmp, upbmp;
    bcm_l2_addr_t     l2_addr;
    bcm_field_qset_t  qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    /* Create vlan to redirect export packets */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, fte_coll_vlan));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, fte_coll_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, fte_coll_vlan, pbmp, upbmp));

    /* Add static MAC address */
    bcm_l2_addr_t_init(&l2_addr, fte_coll_dst_mac, fte_coll_vlan);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port  = fte_coll_port;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

    /* Enable L2StaticMoveToCPU so that the packet goes to CPU on LB. */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL2StaticMoveToCpu, TRUE));

    return 0;
}



int FT_NUM_EXPORT_ELEMENTS = 41;

struct fte_export_element_map_s {
    bcm_flowtracker_tracking_param_type_t tracking_param;
    bcm_flowtracker_export_element_type_t export_element;
    int data_size;
};

fte_export_element_map_s  fte_export_element_maps[FT_NUM_EXPORT_ELEMENTS] =  {
    { bcmFlowtrackerTrackingParamTypeSrcIPv4,           bcmFlowtrackerExportElementTypeSrcIPv4,           4,  },
    { bcmFlowtrackerTrackingParamTypeDstIPv4,           bcmFlowtrackerExportElementTypeDstIPv4,           4,  },
    { bcmFlowtrackerTrackingParamTypeL4SrcPort,         bcmFlowtrackerExportElementTypeL4SrcPort,         2,  },
    { bcmFlowtrackerTrackingParamTypeL4DstPort,         bcmFlowtrackerExportElementTypeL4DstPort,         2,  },
    { bcmFlowtrackerTrackingParamTypeSrcIPv6,           bcmFlowtrackerExportElementTypeSrcIPv6,           16, },
    { bcmFlowtrackerTrackingParamTypeDstIPv6,           bcmFlowtrackerExportElementTypeDstIPv6,           16, },
    { bcmFlowtrackerTrackingParamTypeIPProtocol,        bcmFlowtrackerExportElementTypeIPProtocol,        1,  },
    { bcmFlowtrackerTrackingParamTypePktCount,          bcmFlowtrackerExportElementTypePktCount,          4,  },
    { bcmFlowtrackerTrackingParamTypeByteCount,         bcmFlowtrackerExportElementTypeByteCount,         4,  },
    { bcmFlowtrackerTrackingParamTypeVRF,               bcmFlowtrackerExportElementTypeVRF,               2,  },
    { bcmFlowtrackerTrackingParamTypeTTL,               bcmFlowtrackerExportElementTypeTTL,               1,  },
    { bcmFlowtrackerTrackingParamTypeIPLength,          bcmFlowtrackerExportElementTypeIPLength,          2,  },
    { bcmFlowtrackerTrackingParamTypeIP6Length,         bcmFlowtrackerExportElementTypeIP6Length,         2,  },
    { bcmFlowtrackerTrackingParamTypeTcpWindowSize,     bcmFlowtrackerExportElementTypeTcpWindowSize,     2,  },
    { bcmFlowtrackerTrackingParamTypeDosAttack,         bcmFlowtrackerExportElementTypeDosAttack,         4,  },
    { bcmFlowtrackerTrackingParamTypeVxlanNetworkId,    bcmFlowtrackerExportElementTypeVxlanNetworkId,    3,  },
    { bcmFlowtrackerTrackingParamTypeNextHeader,        bcmFlowtrackerExportElementTypeNextHeader,        1,  },
    { bcmFlowtrackerTrackingParamTypeHopLimit,          bcmFlowtrackerExportElementTypeHopLimit,          1,  },
    { bcmFlowtrackerTrackingParamTypeInnerSrcIPv4,      bcmFlowtrackerExportElementTypeInnerSrcIPv4,      4,  },
    { bcmFlowtrackerTrackingParamTypeInnerDstIPv4,      bcmFlowtrackerExportElementTypeInnerDstIPv4,      4,  },
    { bcmFlowtrackerTrackingParamTypeInnerL4SrcPort,    bcmFlowtrackerExportElementTypeInnerL4SrcPort,    2,  },
    { bcmFlowtrackerTrackingParamTypeInnerL4DstPort,    bcmFlowtrackerExportElementTypeInnerL4DstPort,    2,  },
    { bcmFlowtrackerTrackingParamTypeInnerSrcIPv6,      bcmFlowtrackerExportElementTypeInnerSrcIPv6,      16, },
    { bcmFlowtrackerTrackingParamTypeInnerDstIPv6,      bcmFlowtrackerExportElementTypeInnerDstIPv6,      16, },
    { bcmFlowtrackerTrackingParamTypeInnerIPProtocol,   bcmFlowtrackerExportElementTypeInnerIPProtocol,   1,  },
    { bcmFlowtrackerTrackingParamTypeInnerTTL,          bcmFlowtrackerExportElementTypeInnerTTL,          1,  },
    { bcmFlowtrackerTrackingParamTypeInnerIPLength,     bcmFlowtrackerExportElementTypeInnerIPLength,     2,  },
    { bcmFlowtrackerTrackingParamTypeInnerNextHeader,   bcmFlowtrackerExportElementTypeInnerNextHeader,   1,  },
    { bcmFlowtrackerTrackingParamTypeInnerHopLimit,     bcmFlowtrackerExportElementTypeInnerHopLimit,     1,  },
    { bcmFlowtrackerTrackingParamTypeInnerIP6Length,    bcmFlowtrackerExportElementTypeInnerIP6Length,    2,  },
    { bcmFlowtrackerTrackingParamTypeTcpFlags,          bcmFlowtrackerExportElementTypeTcpFlags,          1,  },
    { bcmFlowtrackerTrackingParamTypeOuterVlanTag,      bcmFlowtrackerExportElementTypeOuterVlanTag,      2,  },
    { bcmFlowtrackerTrackingParamTypeTimestampNewLearn,      bcmFlowtrackerExportElementTypeTimestampNewLearn,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampFlowStart,      bcmFlowtrackerExportElementTypeTimestampFlowStart,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampFlowEnd,      bcmFlowtrackerExportElementTypeTimestampFlowEnd,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1,      bcmFlowtrackerExportElementTypeTimestampCheckEvent1,      6,  },
    { bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2,      bcmFlowtrackerExportElementTypeTimestampCheckEvent2,      6,  },
    { bcmFlowtrackerTrackingParamTypeInnerDosAttack,   bcmFlowtrackerExportElementTypeInnerDosAttack,      4,  },
    { bcmFlowtrackerTrackingParamTypeTunnelClass,      bcmFlowtrackerExportElementTypeTunnelClass,      3,  },
    { bcmFlowtrackerTrackingParamTypeFlowtrackerCheck,      bcmFlowtrackerExportElementTypeFlowtrackerCheck,      2,  },
};

int fte_tracking_param_xlate(int unit,
                             bcm_flowtracker_tracking_param_type_t elem,
                             uint32 *size,
                             bcm_flowtracker_export_element_type_t *info_elem)
{
    int ix;

    for (ix = 0; ix < FT_NUM_EXPORT_ELEMENTS; ix++) {
        if (fte_export_element_maps[ix].tracking_param == elem) {
            *info_elem = fte_export_element_maps[ix].export_element;
            *size = fte_export_element_maps[ix].data_size;

        printf ("translating tracking param %d, export_elemt = %d, size = %d\n", elem, *info_elem, *size);

            break;
        }
    }

    return BCM_E_NONE;
}

int fte_template_install(int unit,
                         bcm_flowtracker_group_t ft_group,
                         bcm_flowtracker_collector_t ft_collector_id,
                         bcm_flowtracker_export_template_t *template_id)
{

    int ix, ix1, elem, rv;
    uint32 options = 0x0;
    int num_checks;
    int m_tracking_params;
    int c_tracking_params = 0;
    bcm_flowtracker_check_t check_ids[10];
    bcm_flowtracker_tracking_param_info_t tracking_params[FTE_MAX_INFO_ELEMENTS];
    bcm_flowtracker_export_element_info_t export_elems_ip[FTE_MAX_INFO_ELEMENTS];
    bcm_flowtracker_export_element_info_t export_elems_op[FTE_MAX_INFO_ELEMENTS];

    m_tracking_params = FTE_MAX_INFO_ELEMENTS;
    rv = bcm_flowtracker_group_tracking_params_get(unit,
                                                   ft_group,
                                                   m_tracking_params,
                                                   tracking_params,
                                                   &c_tracking_params);
    if (BCM_FAILURE(rv)) {
        printf("Failed to fetch tracking params for group: %d\n", ft_group);
        return BCM_E_INTERNAL;
    }

    /* Init ip/op element */
    for (ix = 0; ix < FTE_MAX_INFO_ELEMENTS; ix++) {
        export_elems_ip[ix].data_size = 0;
        export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeReserved;

        export_elems_op[ix].data_size = 0;
        export_elems_op[ix].element = bcmFlowtrackerExportElementTypeReserved;
    }

    for (ix = 0; ix < c_tracking_params; ix++) {
        if (tracking_params[ix].param == bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            continue;
        }
        rv = fte_tracking_param_xlate(unit,
                                      tracking_params[ix].param,
                                      &(export_elems_ip[ix1].data_size),
                                      &(export_elems_ip[ix1].element));
        ix1++;
        if (BCM_FAILURE(rv)) {
            printf ("translating tracking param %d to export element failed\n", tracking_params[ix].param);
            return BCM_E_INTERNAL;
        }
    }
    ix = ix1;

    if (0) {
        bcm_flowtracker_group_check_get_all(unit, ft_group, 10, check_ids, &num_checks);
        for (ix1 = 0; ix1 < num_checks; ix1++) {
            printf ("\n [%d].. Adding Check = %x\n", ix1, check_ids[ix1]);
            export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowtrackerCheck;
            export_elems_ip[ix].check_id = check_ids[ix1];
            ix++;
        }
    }
    c_tracking_params = ix;

    /* Add flowgroup to info elems  */
    export_elems_ip[ix].element = bcmFlowtrackerExportElementTypeFlowtrackerGroup;
    export_elems_ip[ix].data_size = 2;
    c_tracking_params++;


    m_tracking_params = 10;
    rv = bcm_flowtracker_export_template_validate(unit,
                                                  ft_group,
                                                  c_tracking_params,
                                                  export_elems_ip,
                                                  0,
                                                  NULL,
                                                  &m_tracking_params);
    printf("\nm_tracking_params = %d\n", m_tracking_params);
    if (BCM_FAILURE(rv)) {
        printf ("validate template for template with id = %d Failed to get MAX rv = %d\n", *template_id, rv);
        return BCM_E_INTERNAL;
    }

    rv = bcm_flowtracker_export_template_validate(unit,
                                                  ft_group,
                                                  c_tracking_params,
                                                  export_elems_ip,
                                                  m_tracking_params,
                                                  export_elems_op,
                                                  &m_tracking_params);
    if (BCM_FAILURE(rv)) {
        printf ("validate template for template with id = %d\n", *template_id);
        return BCM_E_INTERNAL;
    }

    printf("***********************************\n");
    printf("Input information elements: \n");
    for (ix = 0; ix < c_tracking_params; ix++) {
        elem = (export_elems_ip[ix].element);
        printf("\tinput info element (%d): elem = %d and size = %d\n", ix, elem, export_elems_ip[ix].data_size);
    }
    printf("***********************************\n");


    printf("***********************************\n");
    printf("Output information elements: \n");
    for (ix = 0; ix < m_tracking_params; ix++) {
        elem = (export_elems_op[ix].element);
        printf("\toutput info element (%d): elem = %d and size = %d\n", ix, elem, export_elems_op[ix].data_size);
    }
    printf("***********************************\n");

    rv = bcm_flowtracker_export_template_create(unit, options,
                                                template_id,
                                                fte_set_id,
                                                m_tracking_params,
                                                export_elems_op);
    if (BCM_FAILURE(rv)) {
        printf ("creating template failed template_id = %d\n", *template_id);
        return BCM_E_INTERNAL;
    }

    rv = bcm_flowtracker_group_collector_attach(unit, ft_group, ft_collector_id, fte_export_profile_id, *template_id);
    if (BCM_FAILURE(rv)) {
        printf ("Attaching template (%d) and collector (%d) to group (%d) failed rv = %d\n",
                 *template_id, ft_collector_id, ft_group, rv);
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

int ut_fte_collector_create(int case_id)
{
    int unit = 0;
    int rv = BCM_E_NONE;
    int collector_id = -1;
    int export_profile_id = -1;
    int replace = 0;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t profile_info;

    uint32 options = 0;

    bcm_mac_t fte_coll_dst_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xc0};

    bcm_collector_info_t_init(&collector_info);
    bcm_collector_export_profile_t_init(&profile_info);

    switch(case_id) {
        case 1:
            {
                fte_collector_setup(fte_coll_vlan, fte_coll_dst_mac);

                sal_memcpy(collector_info.eth.dst_mac, fte_coll_dst_mac, 6);
                sal_memcpy(collector_info.eth.src_mac, fte_coll_src_mac, 6);

                collector_info.eth.vlan_tag_structure =
                    BCM_FLOWTRACKER_COLLECTOR_ETH_HDR_SINGLE_TAGGED;
                collector_info.eth.outer_vlan_tag = fte_coll_vlan;
                collector_info.eth.outer_tpid     = fte_coll_tpid;
                collector_info.ipv4.src_ip        = fte_coll_src_ip;
                collector_info.ipv4.dst_ip        = fte_coll_dst_ip;
                collector_info.ipv4.dscp          = fte_coll_dscp;
                collector_info.ipv4.ttl           = fte_coll_ttl;
                collector_info.udp.src_port       = fte_coll_l4_src_port;
                collector_info.udp.dst_port       = fte_coll_l4_dst_port;
                collector_info.ipfix.version = 0xA;
                collector_info.ipfix.initial_sequence_num = 0x2;
                collector_info.transport_type     =
                    bcmCollectorTransportTypeIpv4Udp;

                collector_id = 0;

                profile_info.wire_format = bcmCollectorWireFormatIpfix;
                profile_info.max_packet_length = fte_coll_mtu;
                break;
            }
        default:
            return -1;

    }
    if (replace) {
        options |= BCM_FLOWTRACKER_COLLECTOR_REPLACE;
    }

    rv = bcm_collector_create(unit,
            options, &collector_id, collector_info);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to create collector ret=%d\n", rv);
        return BCM_E_INTERNAL;
    }

    fte_coll_id = collector_id;

    rv = bcm_collector_export_profile_create(unit, 0, &export_profile_id, &profile_info);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to create export profile ret=%d\n", rv);
        return BCM_E_INTERNAL;
    }

    fte_export_profile_id = export_profile_id;

    return collector_id;
}

int ut_fte_collector_add(int ft_group, int fte_coll_id)
{
    int rv = 0;
    int unit = 0;
    rv = fte_template_install(unit, ft_group, fte_coll_id, &fte_tmpl_id);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to install template: rv=%d\n", rv);
        return BCM_E_INTERNAL;
    }
    return rv;
}


int ut_fte_collector_destroy(int ft_group, int collector_id)
{
    int rv = 0;
    int unit = 0;

    rv = bcm_flowtracker_group_collector_detach(unit, ft_group, collector_id, fte_export_profile_id, fte_tmpl_id);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to detach collector from group: rv=%d\n", rv);
        return BCM_E_INTERNAL;
    }

    rv = bcm_collector_export_profile_destroy(unit, fte_export_profile_id);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to destroy export profile[%d]: rv=%d\n", fte_export_profile_id, rv);
        return BCM_E_INTERNAL;
    }

    rv = bcm_flowtracker_export_template_destroy(unit, fte_tmpl_id);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to destroy template[%d]: rv=%d\n", fte_tmpl_id, rv);
        return BCM_E_INTERNAL;
    }

    rv = bcm_collector_destroy(unit, collector_id);
    if (rv != BCM_E_NONE) {
        printf("\n Failed to destroy collector[%d]: rv=%d\n", collector_id, rv);
        return BCM_E_INTERNAL;
    }

    fte_coll_id = -1;

    return rv;
}

