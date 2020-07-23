/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * set following soc properties
 * dma_desc_aggregator_enable_specific_PPDB_A_FEC_SUPER_ENTRY_BANK=1
 * mem_cache_enable_specific=1
 * mem_cache_enable_specific_PPDB_A_FEC_SUPER_ENTRY_BANK=1
 * dma_desc_aggregator_chain_length_max=500
 * dma_desc_aggregator_buff_size_kb=100
 * dma_desc_aggregator_timeout_usec=1000
 *
 * run:
 * cd ../../../../src/examples/dpp
 * cint cint_field_dma_access.c
 * cint
 * tcam_entry_add(unit);
 * fec_dma_entry_add(unit,in_port,out_port,in_vid,out_vid);
 *
 * stress scan:
 *      tcam stress scan: tcam_stress_scan(unit);
 *      fec dma and tcam stress scan: fec_dma_tcam_stress_scan(unit);
 */

int scan_entry_id_max = 2048;
bcm_if_t encap_id = 0;

int tcam_entry_add(int unit) {
    int rv, is_qax = FALSE, is_qux = FALSE;
    bcm_field_aset_t aset;
    bcm_field_qset_t qset;
    bcm_field_entry_t entry_id;
    bcm_field_presel_t presel_id;
    bcm_ip_t src_ip = 0x00000000;
    bcm_field_presel_set_t presel_set;
    bcm_field_group_config_t field_group;

    if (rv = is_qumran_ax_only(unit, &is_qax)) {
        printf("Failed(%d) is_qax\n", rv);
        return rv;
    }

    if (rv = is_qumran_ux_only(unit, &is_qux)) {
        printf("Failed(%d) is_qux\n", rv);
        return rv;
    }

    if (is_qux == TRUE) {
        scan_entry_id_max = 4000;
    } else if ((is_qax == TRUE) || (!is_device_or_above(unit, JERICHO))) {
        scan_entry_id_max = 12000;
    }

    rv = bcm_field_presel_create(unit, &presel_id);
    if (rv != BCM_E_NONE) {
        printf ("bcm_field_presel_create error %d\n", rv);
        return rv;
    }

    rv = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
    if (rv != BCM_E_NONE) {
        printf ("bcm_field_qualify_ForwardingType error %d\n", rv);
        return rv;
    }

    bcm_field_presel_set_t_init(&presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel_id);

    BCM_FIELD_QSET_INIT(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);

    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    /* Define a field group (Database) */
    bcm_field_group_config_t_init(&field_group);
    field_group.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    field_group.qset = qset;
    field_group.aset = aset;
    field_group.preselset = presel_set;
    rv = bcm_field_group_config_create(unit, &field_group);
    if (rv != BCM_E_NONE) {
        printf ("bcm_field_group_config_create error %d\n", rv);
        return rv;
    }

    /* Add entries (Rules) to the field-group */
    for (entry_id = 1; entry_id <= scan_entry_id_max; entry_id++) {
        rv = bcm_field_entry_create_id(unit, field_group.group, entry_id);
        if (rv != BCM_E_NONE) {
            printf ("bcm_field_entry_create_id error %d\n", rv);
            return rv;
        }

        rv = bcm_field_entry_prio_set(unit, entry_id, entry_id);
        if (rv != BCM_E_NONE) {
            printf ("bcm_field_entry_prio_set error %d\n", rv);
            return rv;
        }

        rv = bcm_field_qualify_SrcIp(unit, entry_id, src_ip++, 0xffffffff);
        if (rv != BCM_E_NONE) {
            printf ("bcm_field_qualify_SrcIp error %d\n", rv);
            return rv;
        }

        rv = bcm_field_action_add(unit, entry_id, bcmFieldActionDrop, 0, 0);
        if (rv != BCM_E_NONE) {
            printf ("bcm_field_action_add error %d\n", rv);
            return rv;
        }
    }

    rv = bcm_field_group_install(unit, field_group.group);
    if (rv != BCM_E_NONE) {
        printf ("bcm_field_group_install error %d\n", rv);
        return rv;
    }

    return rv;
}

int fec_dma_entry_add(int unit, int in_port, int out_port, int in_vid, int out_vid) {
    bcm_l2_station_t station;
    bcm_vlan_port_t in_vp;
    bcm_vlan_port_t out_vlan_port;
    bcm_l3_intf_t l3_in_rif;
    bcm_l3_intf_t l3_out_rif;
    bcm_l3_egress_t nh_ll_arp;
    bcm_if_t nh_fec;
    int rv, station_id, in_vsi = 2000, out_vsi = 3000;
    bcm_mac_t in_rif_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x13}, out_rif_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x14},
        l2_station_mask  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, nh_da = { 0x00, 0x00 , 0x00 , 0x00 , 0x00 , 0x14 };

    /* Configure TPID profile for each port */
    rv = bcm_port_tpid_delete_all(unit, in_port);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_tpid_delete_all error %d\n", rv);
        return rv;
    }

    rv = bcm_port_tpid_delete_all(unit, out_port);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_tpid_delete_all error %d\n", rv);
        return rv;
    }

    rv = bcm_port_tpid_add(unit, in_port, 0x8100, 0);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_tpid_add error %d\n", rv);
        return rv;
    }

    rv = bcm_port_tpid_add(unit, in_port, 0x88a8, 0);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_tpid_add error %d\n", rv);
        return rv;
    }

    rv = bcm_port_tpid_add(unit, out_port, 0x8100, 0);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_tpid_add error %d\n", rv);
        return rv;
    }

    rv = bcm_port_tpid_add(unit, out_port, 0x88a8, 0);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_tpid_add error %d\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(unit, in_port, bcmPortClassId, in_port);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_class_set error %d\n", rv);
        return rv;
    }

    rv = bcm_port_class_set(unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf ("bcm_port_class_set error %d\n", rv);
        return rv;
    }

    /* Configure global MAC[47:12] */
    bcm_l2_station_t_init(&station);
    sal_memcpy(station.dst_mac_mask, l2_station_mask, 6);
    sal_memcpy(station.dst_mac, in_rif_mac, 6);
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l2_station_add error %d\n", rv);
        return rv;
    }

    /* Configure In-AC for ingress L2 interface */
    bcm_vlan_port_t_init(&in_vp);
    in_vp.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vp.match_vlan  = in_vid;
    in_vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : in_vid;
    in_vp.vsi  = in_vsi;
    in_vp.port = in_port;
    rv = bcm_vlan_port_create(unit, &in_vp);
    if (rv != BCM_E_NONE) {
        printf ("bcm_vlan_port_create error %d\n", rv);
        return rv;
    }

    /* Configure Out-AC for egress L2 interface */
    bcm_vlan_port_t_init(&out_vlan_port);
    out_vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port.match_vlan  = out_vid;
    out_vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : out_vid;
    out_vlan_port.vsi  = out_vsi;
    out_vlan_port.port = out_port;
    rv = bcm_vlan_port_create(unit, &out_vlan_port);
    if (rv != BCM_E_NONE) {
        printf ("bcm_vlan_port_create error %d\n", rv);
        return rv;
    }

    /* Setup InRif */
    bcm_l3_intf_t_init(&l3_in_rif);
    sal_memcpy(l3_in_rif.l3a_mac_addr, in_rif_mac, 6);
    l3_in_rif.l3a_vid = in_vsi;
    l3_in_rif.l3a_vrf = 5;
    l3_in_rif.l3a_mtu = 1000;
    rv = bcm_l3_intf_create(unit, &l3_in_rif);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_intf_create error %d\n", rv);
        return rv;
    }

    /* Public default route */
    bcm_l3_ingress_t l3_ing;
    bcm_l3_ingress_t_init(&l3_ing);
    l3_ing.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_WITH_ID;
    rv = bcm_l3_ingress_create(unit, &l3_ing, l3_in_rif.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_ingress_create error %d\n", rv);
        return rv;
    }

    /* default VRF */
    bcm_l3_intf_t_init(&l3_out_rif);
    sal_memcpy(l3_out_rif.l3a_mac_addr, out_rif_mac, 6);
    l3_out_rif.l3a_vid = out_vsi;
    rv = bcm_l3_intf_create(unit, &l3_out_rif);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_intf_create error %d\n", rv);
        return rv;
    }

    bcm_l3_egress_t_init(&nh_ll_arp);
    nh_ll_arp.mac_addr = nh_da;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &nh_ll_arp, &nh_fec);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_create error %d\n", rv);
        return rv;
    }

    encap_id = nh_ll_arp.encap_id;

    return rv;
}

int tcam_stress_scan(int unit) {
    int rv, scan_entry_id;
    bcm_ip_t scan_data, scan_mask;

    for (scan_entry_id = 1; scan_entry_id <= scan_entry_id_max; scan_entry_id++) {
        rv = bcm_field_qualify_SrcIp_get(unit, scan_entry_id, &scan_data, &scan_mask);
        if (rv != BCM_E_NONE) {
            printf ("bcm_field_qualify_SrcIp_get failed on entry %d\n", scan_entry_id);
            return rv;
        }
    }

    return rv;
}

int fec_dma_tcam_stress_scan(int unit, int out_port) {
    int rv, count, scan_entry_id, count_max = 100;
    bcm_l3_egress_t nh_ll_arp_fec;
    bcm_ip_t scan_data, scan_mask;
    bcm_if_t nh_fec[100];

    for (scan_entry_id = 1; scan_entry_id <= scan_entry_id_max; ) {
        for (count = 0; count < count_max; count++) {
            bcm_l3_egress_t_init(&nh_ll_arp_fec);
            nh_ll_arp_fec.intf = 3000;
            nh_ll_arp_fec.encap_id = encap_id;
            nh_ll_arp_fec.port = out_port;
            rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &nh_ll_arp_fec, &nh_fec[count]);
            if (rv != BCM_E_NONE) {
                printf("bcm_l3_egress_create error %d\n", rv);
                return rv;
            }
        }

        rv = bcm_field_qualify_SrcIp_get(unit, scan_entry_id++, &scan_data, &scan_mask);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_qualify_SrcIp_get failed on entry %d\n", scan_entry_id);
            return rv;
        }

        for (count = 0; count < count_max; count++) {
            rv = bcm_l3_egress_destroy(unit, nh_fec[count]);
            if (rv != BCM_E_NONE) {
                printf("bcm_l3_egress_destroy error %d\n", rv);
                return rv;
            }
        }

        rv = bcm_field_qualify_SrcIp_get(unit, scan_entry_id++, &scan_data, &scan_mask);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_qualify_SrcIp_get failed on entry %d\n", scan_entry_id);
            return rv;
        }
    }

    return rv;
}

