/*
 * $Id: cint_dnx_multiast_bridge_ivl.c
 * Exp $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 *
 * Test Scenario - start
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
  cint ../../../../src/examples/dnx/multicast/cint_dnx_multiast_bridge_ivl.c
  cint
  ipmc_l3_mcast_l2_ivl_example(0,200,201,202,203,100,1092,1093);
  exit;
 *
 * IPv4 untag:
      tx 1 psrc=200 data=0x0000000000020000000000010899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * IPv4 ctag:
      tx 1 psrc=200 data=0x000000000002000000000001810004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * IPv4 stag:
      tx 1 psrc=201 data=0x000000000001000000000002910004450899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * IPv4 S+C tags:
      tx 1 psrc=201 data=0x00000000000100000000000291000445810004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * IPv6 untag( SIP1 00000000000000000000111111110000)
      tx 1 psrc=200 data=33330000003000000000557086DD600000000021067F00000000000000000000111111110000FFFF0000000000000000000012345678000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2
 *
 * IPv6 s-tag( SIP1)
     tx 1 psrc=200 data=3333000000300000000055709100044486DD600000000021067F00000000000000000000111111110000FFFF0000000000000000000012345678000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2
 *
 * IPv6 c-tag( SIP1)
     tx 1 psrc=200 data=3333000000300000000055708100044486DD600000000021067F00000000000000000000111111110000FFFF0000000000000000000012345678000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2
 *
 * IPv6 s+c tag( SIP2 00000100000000000000111111110000)
     tx 1 psrc=200 data=333300000030000000005570910004448100044586DD600000000021067F00000100000000000000111111110000FFFF0000000000000000000012345678000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2
 * Test Scenario - end
 *
 * packets will be routed from in_port to out-ports after MC group is hit
 * Two RIFs are configured with both BCM_L3_INGRESS_L3_MCAST_L2 and BCM_L3_INGRESS_IP6_L3_MCAST_L2 flag.
 */

int tag_format_untag = 0;
int tag_format_stag = 4;
int tag_format_ctag = 8;
int tag_format_dtag = 16;

/* Default global tpid configuration */
int tpid_value_ctag = 0x8100;
int tpid_value_stag = 0x9100;

/* vlan edit profiles */
int ing_vlan_edit_profile_uni = 1;
int ing_vlan_edit_profile_nni_dtag = 2;

struct mc_ivl_info_t
{
    int in_port;
    int out_ports[3];
    bcm_vlan_t vsi_ivl;
    bcm_vlan_t ivl_stag;
    bcm_vlan_t ivl_ctag;
    bcm_gport_t in_lifs[2];
};
mc_ivl_info_t mc_ivl_info;

int ipmc_l3_mcast_l2_ivl_example(int unit, int in_port, int out_port1, int out_port2, int out_port3, bcm_vlan_t vsi, bcm_vlan_t vid1, bcm_vlan_t vid2)
{
    uint32 sip_addr = 0xC0800101;
    uint32 sip_mask = 0xFFFF0000;
    uint32 dip_addr = 0xE0020202;
    uint32 dip_mask = 0xFFFFFFFF;
    bcm_ip6_t dip_v6_addr = {0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0x56, 0x78};
    bcm_ip6_t dip_v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip_v6_addr = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    bcm_ip6_t sip_v6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0};
    int ports[2][2] = {{out_port1, out_port2}, {out_port1, out_port3}};
    uint32 flags;
    int idx = 0;
    int mc_idx = 0;
    int ipmc_group[2] = {0x1000, 0x1001};
    int nof_mc_groups = 2;
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];
    int vrf = 1;
    int intf_out = 150;
    bcm_vlan_control_vlan_t control;
    l3_ingress_intf ingr_itf;
    bcm_vlan_t kaps_vid = 0;
    bcm_vlan_t lem_vid = 0;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_vlan_port_t vlan_port;
    int c_sip = 0x456;
    char error_msg[200]={0,};
    mc_ivl_info.in_port = in_port;
    mc_ivl_info.out_ports[0] = out_port1;
    mc_ivl_info.out_ports[1] = out_port2;
    mc_ivl_info.out_ports[2] = out_port3;
    mc_ivl_info.vsi_ivl = vsi;
    mc_ivl_info.ivl_stag = vid1;
    mc_ivl_info.ivl_ctag = vid2;

    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vsi), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(vlan=%d)", vid1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(vlan=%d)", vid2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vid2), error_msg);

    /**Configure VSI IVL mode */
    snprintf(error_msg, sizeof(error_msg), "in vsi= %d", vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_get(unit, vsi, &control), error_msg);
    control.flags2 = BCM_VLAN_FLAGS2_IVL;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, vsi, control), error_msg);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);

    snprintf(error_msg, sizeof(error_msg), "(vlan=%d)", vid1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, vid1, pbmp, ubmp), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(vlan=%d)", vid2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, vid2, pbmp, ubmp), error_msg);

    /* 1. Set In-Port to Eth-RIF */
    /* untagged packet */
    snprintf(error_msg, sizeof(error_msg), "vid1 =%d", vid1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, in_port, vid1), error_msg);

    /* create in_lif 0 for un-tag/s-tag forwarding */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_IVL;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vsi;
    vlan_port.port = in_port;

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    mc_ivl_info.in_lifs[0] = vlan_port.vlan_port_id;

    /* lif learn enable */
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", mc_ivl_info.in_lifs[0]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, mc_ivl_info.in_lifs[0]), error_msg);


    /* create in_lif 1 for double tag forwarding */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_IVL;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.match_vlan = vid1;
    vlan_port.match_inner_vlan = vid2;
    vlan_port.port = in_port;
    vlan_port.vsi = vsi;

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    mc_ivl_info.in_lifs[1] = vlan_port.vlan_port_id;

    /* lif learn enable */
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", mc_ivl_info.in_lifs[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, mc_ivl_info.in_lifs[1]), error_msg);

    /* 2. Set Out-Port default properties */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port1), "");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port2), "");
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port3), "");

    /* 3. Create IN-RIF and set its properties */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, vsi, cint_ipmc_info.intf_in_mac_address), "intf_in");

    /* 4. Set ingress interface */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = vsi;
    ingr_itf.vrf = vrf;
    ingr_itf.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST
                     | BCM_L3_INGRESS_L3_MCAST_L2 | BCM_L3_INGRESS_IP6_L3_MCAST_L2;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");


    for (mc_idx = 0; mc_idx < nof_mc_groups; mc_idx++)
    {
        for (idx = 0; idx < cint_ipmc_info.nof_replications; idx++)
        {
            /*
             * 4.2 Set the replication.
             */
            set_multicast_replication(&replications[idx], ports[mc_idx][idx], -1);

            /*
             * 4.3 Set some changes for the next replication.
             */
            intf_out++;
            cint_ipmc_info.intf_out_mac_address[5]++;
        }
        /*
         * 5. Create a multicast group
         */
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
        snprintf(error_msg, sizeof(error_msg), "ipmc_group = %d", ipmc_group[mc_idx]);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &ipmc_group[mc_idx]), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, ipmc_group[mc_idx], BCM_MULTICAST_INGRESS_GROUP, cint_ipmc_info.nof_replications, replications), "");
        if(mc_idx == 0)
        {
            kaps_vid = vid1;  /* stag*/
            lem_vid = vid2;  /* ctag*/
        }
        else
        {
            kaps_vid = vid2;  /* ctag */
            lem_vid = vid1;  /* stag*/
        }
        /* KAPS */
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, dip_addr, dip_mask, sip_addr, sip_mask, vsi, kaps_vid, ipmc_group[mc_idx], 0, 0), "");
        /* LEM */
        BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc_bridge(unit, dip_addr, dip_mask, sip_addr, 0 /*sip_mask*/, vsi, lem_vid, ipmc_group[mc_idx], 0, 0), "");

        /* KAPS */
        BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc_bridge(unit, dip_v6_addr, dip_v6_mask, sip_v6_addr, sip_v6_mask, kaps_vid, vsi, c_sip, ipmc_group[mc_idx], 0, 0), "");
        sip_v6_addr[2] ++;
        c_sip ++;
    }

    /*
         * Incoming is Un-TAG
         * IVE ={bcmVlanActionAdd,None}
         */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
                            unit,
                            mc_ivl_info.in_lifs[0],                  /* lif */
                            tpid_value_stag,                     /* outer_tpid */
                            0,                                   /* inner_tpid */
                            bcmVlanActionAdd,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            vid1,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_untag,                    /* tag_format */
                            TRUE                                 /* is_ive */
                            ), "(setting IVE to {bcmVlanActionAdd,None})");


    /* Incoming is DTAG
         *  IVE ={bcmVlanActionDelete,None}
         */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
                            unit,
                            mc_ivl_info.in_lifs[1],                  /* lif */
                            tpid_value_stag,                     /* outer_tpid */
                            tpid_value_ctag,                     /* inner_tpid */
                            bcmVlanActionDelete,                 /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            vid2,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_nni_dtag,      /* vlan_edit_profile */
                            tag_format_dtag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            ), "(setting IVE to {bcmVlanActionDelete,None})");


    return BCM_E_NONE;
}


/*
 * Clean up procedure
 */
int ipmc_l3_mcast_l2_ivl_cleanup(int unit)
{
    int ii = 0;
    char error_msg[200]={0,};
    bcm_pbmp_t pbmp;
    int ipmc_group[2] = {0x1000, 0x1001};
    int nof_mc_groups = 2;

    /* Destory muticast group */
    for (ii = 0; ii < nof_mc_groups; ii++)
    {
        snprintf(error_msg, sizeof(error_msg), "mc_group_id = %d", ipmc_group[ii]);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, ipmc_group[ii]), error_msg);
    }

    snprintf(error_msg, sizeof(error_msg), "(port_1=%d, vlan=1)", mc_ivl_info.in_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, mc_ivl_info.in_port, 1), error_msg);

    /*
     * IVE actions:
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS),
        "(BCM_VLAN_ACTION_SET_INGRESS)");

    for (ii = 0; ii < sizeof(mc_ivl_info.in_lifs)/sizeof(mc_ivl_info.in_lifs[0]); ++ii)
    {
        if (mc_ivl_info.in_lifs[ii] != 0)
        {
            snprintf(error_msg, sizeof(error_msg), "(port = %d)", mc_ivl_info.in_lifs[ii]);
            BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_delete(unit, mc_ivl_info.vsi_ivl, mc_ivl_info.in_lifs[ii]), error_msg);
            snprintf(error_msg, sizeof(error_msg), "(lif = %d)", mc_ivl_info.in_lifs[ii]);
            BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, mc_ivl_info.in_lifs[ii]), error_msg);
        }
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mc_ivl_info.in_port);
    snprintf(error_msg, sizeof(error_msg), "(vlan=%d)", mc_ivl_info.ivl_ctag);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_remove(unit, mc_ivl_info.ivl_ctag, pbmp), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(vlan=%d)", mc_ivl_info.ivl_stag);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_remove(unit, mc_ivl_info.ivl_stag, pbmp), error_msg);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy_all(unit), "");

    return BCM_E_NONE;
}

