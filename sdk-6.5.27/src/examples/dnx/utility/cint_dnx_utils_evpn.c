/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * File: cint_dnx_utils_evpn.c
 * Purpose: Utility functions to setup EVPN application.
 *
 * This file is meant to contain stand alone structures and utility functions
 * to create EVPN/IML services using the different APIs to setup all the details.
 * The affected DBs are EVPN InLIF, EVPN EEDB and the ESI ESEM DB and TCAM DB.
 *
 * Global configurations like MPLS LSP tunnels, Split-Horizon setup, RIFs, etc.
 * should be created by the user application and are not part of this utilities
 * collection.
 *
 * Forwarding and BUM configuration are also not provided in this example.
 */

/*
 * Arbitrary maximal number of peers that are dual-homed with the current device.
 * The real limit in BCM88690 is the ESEM utilization limit. The number here is
 * selected to limit the memory used by the current application reference to a
 * reasonable amount.
 */
int MAX_DUAL_HOMED_CES = 16;
/*
 * Each ethernet segment (ES) is assumed to have it's own system port. The peer
 * advertises the ESI label it expects to signal the ES. The information in this
 * struct in combination with the peer unique peer_id, encompases all the info
 * needed for ESI encapsulation entry in the egress DB.
 */
struct evpn_util_peer_dual_homing_info_s
{
    bcm_gport_t * src_port;
    bcm_mpls_label_t esi_label;
};

struct evpn_iml_protection_s
{
    int enabled;
    bcm_failover_t failover_id;
    int failover_path;
};

struct evpn_util_peer_info_s
{
    /****
     * Configurations
     ****/
    /*
     * General
     */
    bcm_vpn_t vpn;
    /*
     * Peer expected/used EVPN/IML services labels
     */
    bcm_mpls_label_t evpn_label;
    bcm_mpls_label_t iml_label;
    evpn_iml_protection_s iml_ingress_protection;
    evpn_iml_protection_s iml_egress_protection;
    /*
     * Lif profile for IML+ESI termination
     */
    int iml_esi_acl_lif_profile;
    /*
     * Dual homed peer information
     * if the peer is dual homed to this PE, assign it a profile
     * and set the information of the ESIs that are dual-homed with
     * the peer.
     */
    int iml_dual_homed_peer_profile;
    int nof_dual_homed_ces;
    evpn_util_peer_dual_homing_info_s dual_homed_ce_info[MAX_DUAL_HOMED_CES];
    /*
     * Split horizon information
     */
    int network_group_id;
    /** If nwk group is taken from esem at the egress device, it is the users responsibility
     * to create a default native vlan port with the correct group id. */
    bcm_vlan_port_t * default_native_ac_for_split_horizon;
    /*
     * Generated objects
     */
    bcm_mpls_egress_label_t evpn_egress_label;
    bcm_mpls_tunnel_switch_t evpn_tunnel_switch;
    bcm_mpls_egress_label_t iml_egress_label;
    bcm_mpls_tunnel_switch_t iml_tunnel_switch;
    bcm_mpls_tunnel_switch_t iml_esi_tunnel_switch;
};

/*
 * Creates EVPN and IML services for traffic towards a peer.
 * Creates EVPN and IML termination for traffic arriving from a peer.
 */
int
evpn_util_peer_services_create(
    int unit,
    evpn_util_peer_info_s * info)
{

    BCM_IF_ERROR_RETURN_MSG(evpn_util_evpn_encap_create(unit, info), "");
    BCM_IF_ERROR_RETURN_MSG(evpn_util_iml_encap_create(unit, info), "");
    BCM_IF_ERROR_RETURN_MSG(evpn_util_evpn_term_create(unit, info), "");
    BCM_IF_ERROR_RETURN_MSG(evpn_util_iml_term_create(unit, info), "");

    return BCM_E_NONE;
}

/*
 * Creates EVPN egress label.
 * Created object will be saved to info->evpn_egress_label
 */
int
evpn_util_evpn_encap_create(
    int unit,
    evpn_util_peer_info_s * info)
{
    bcm_gport_t gport;
    bcm_mpls_egress_label_t *plabel = &info->evpn_egress_label;
    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = info->evpn_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_EVPN | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel), "evpn label");

    printf("EVPN encapsulation created. ID = 0x%08x\n", plabel->tunnel_id);

    /*
     * tunnel_id is generated as ITF in bcm_mpls_tunnel_initiator_create
     * convert to GPort for further processing.
     */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, plabel->tunnel_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, gport, bcmPortClassForwardEgress,
                            info->network_group_id), "iml egress label");
    if (nwk_grp_from_esem)
    {
        bcm_port_match_info_t match_info;
        match_info.match = BCM_PORT_MATCH_PORT;
        match_info.flags |= BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY;
        match_info.port = gport;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, info->default_native_ac_for_split_horizon->vlan_port_id, &match_info),
            "IML to default native AC");
    }
    printf("    NWK Group ID: %d\n", info->network_group_id);

    return BCM_E_NONE;
}

/*
 * Creates IML egress label.
 * Created object will be saved to info->iml_egress_label
 */
int
evpn_util_iml_encap_create(
    int unit,
    evpn_util_peer_info_s * info)
{
    char error_msg[200]={0,};
    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    bcm_mpls_egress_label_t *plabel = &info->iml_egress_label;
    bcm_gport_t gport;

    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = info->iml_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    if (info->iml_egress_protection.enabled)
    {
        plabel->flags |= BCM_MPLS_EGRESS_LABEL_PROTECTION;
        plabel->egress_failover_id = info->iml_egress_protection.failover_id;
        plabel->egress_failover_if_id = info->iml_egress_protection.failover_path;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel), "");
    printf("IML encapsulation created. ID = 0x%08x\n", plabel->tunnel_id);
    /*
     * tunnel_id is generated as ITF in bcm_mpls_tunnel_initiator_create
     * convert to GPort for further processing.
     */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, plabel->tunnel_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, gport, bcmPortClassForwardEgress,
                            info->network_group_id), "iml egress label");
    if (nwk_grp_from_esem)
    {
        bcm_port_match_info_t match_info;
        match_info.match = BCM_PORT_MATCH_PORT;
        match_info.flags |= BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY;
        match_info.port = gport;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, info->default_native_ac_for_split_horizon->vlan_port_id, &match_info),
            "IML to default native AC");
    }
    printf("    NWK Group ID: %d\n", info->network_group_id);

    if (info->nof_dual_homed_ces > 0)
    {
        int ii;
        snprintf(error_msg, sizeof(error_msg), "for dual homed peer id (%d) of lif (0x%08x)",
            info->iml_dual_homed_peer_profile, gport);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, gport, bcmPortClassEgress, info->iml_dual_homed_peer_profile), error_msg);
        printf("    Set dual homed peer profile to 0x%03x\n", info->iml_dual_homed_peer_profile);

        for (ii = 0; ii < info->nof_dual_homed_ces; ++ii)
        {
            bcm_mpls_esi_info_t esi_info;
            bcm_mpls_esi_info_t_init(&esi_info);
            esi_info.src_port =  *(info->dual_homed_ce_info[ii].src_port);
            esi_info.out_class_id = info->iml_dual_homed_peer_profile;
            esi_info.esi_label = info->dual_homed_ce_info[ii].esi_label;
            BCM_IF_ERROR_RETURN_MSG(bcm_mpls_esi_encap_add(unit, &esi_info), "");
            printf("        ESI label added (0x%05x) for src_port = 0x%x\n", esi_info.esi_label, esi_info.src_port);
        }
    }

    return BCM_E_NONE;
}

/*
 * Creates EVPN label termination.
 * Created object will be saved to info->evpn_tunnel_switch
 */
int
evpn_util_evpn_term_create(
    int unit,
    evpn_util_peer_info_s * info)
{
    bcm_mpls_tunnel_switch_t *tunnel = &info->evpn_tunnel_switch;

    bcm_mpls_tunnel_switch_t_init(tunnel);
    tunnel->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel->flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    tunnel->flags2 = (info->vpn ? 0 : BCM_MPLS_SWITCH2_CROSS_CONNECT);
    tunnel->label = info->evpn_label;
    tunnel->vpn = info->vpn;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, tunnel), "evpn");
    printf("EVPN termination - created. ID = 0x%08x\n", tunnel->tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, tunnel->tunnel_id, bcmPortClassForwardIngress,
                            info->network_group_id), "iml ingress label");
    printf("     NWK Group set to %d\n", info->network_group_id);

    return BCM_E_NONE;
}

/*
 * Creates IML label termination; will be saved to info->iml_tunnel_switch
 * Creates IML+ESI labels termination; will be saved to info->iml_esi_tunnel_switch.
 */
int
evpn_util_iml_term_create(
    int unit,
    evpn_util_peer_info_s * info)
{
    bcm_mpls_tunnel_switch_t *pswitch;
    int expect_esi;

    /** If the feature of mpls_term_till_bos is supported, no needs _EXPECT_BOS and the 2nd IML entry.*/
    int feature_mpls_term_till_bos = *dnxc_data_get(unit, "mpls", "general", "mpls_term_till_bos", NULL);

    for (expect_esi = feature_mpls_term_till_bos; expect_esi < 2; ++expect_esi)
    {
        pswitch = expect_esi ? &info->iml_esi_tunnel_switch : &info->iml_tunnel_switch;

        bcm_mpls_tunnel_switch_t_init(pswitch);
        pswitch->action = BCM_MPLS_SWITCH_ACTION_POP;
        pswitch->flags = BCM_MPLS_SWITCH_EVPN_IML;
        if ((!expect_esi) && (!feature_mpls_term_till_bos))
        {
            pswitch->flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
        }
        pswitch->label = info->iml_label;
        pswitch->vpn = info->vpn;
        if (pswitch->vpn == 0)
        {
            pswitch->flags2 |= BCM_MPLS_SWITCH2_CROSS_CONNECT;
        }
        if (info->iml_ingress_protection.enabled)
        {
            pswitch->failover_id = info->iml_ingress_protection.failover_id;
            pswitch->failover_tunnel_id = info->iml_ingress_protection.failover_path;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, pswitch), "iml label");
        printf("IML termination - created. ID = 0x%08x\n", pswitch->tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, pswitch->tunnel_id, bcmPortClassForwardIngress,
                                info->network_group_id), "iml ingress label");
        printf("    NWK Group set to %d\n", info->network_group_id);

        /*
         * mpls_term_till_bos == 0: Set In-LIF profile for iPMF selection of ESI filter application
         * mpls_term_till_bos == 1: Not needed, since iPMF qualifies IML-range-Qual
         */
        if ((expect_esi) && (!feature_mpls_term_till_bos))
        {
            printf("Set In-LIF profile to select ESI FP ingress application\n");
            BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, pswitch->tunnel_id, bcmPortClassFieldIngressVport,
                                    info->iml_esi_acl_lif_profile), "iml label");
            printf("    ACL In-LIF profile set to %d\n", info->iml_esi_acl_lif_profile);
        }

    }
    return BCM_E_NONE;
}

/****************************************************************
 ****************************************************************
 ****************************************************************
 *
 *              ESI Filter utilities
 *
 ****************************************************************
 ****************************************************************
 ****************************************************************/

/*
 * Setup Field Processor based ESI filter.
 */
int
evpn_util_esi_filter_init(
    int unit,
    int in_lif_acl_profile)
{

    BCM_IF_ERROR_RETURN_MSG(cint_field_evpn_ingress(unit, in_lif_acl_profile), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_evpn_egress(unit), "");

    return BCM_E_NONE;
}

/*
 * Add ESI Filter entry to egress FP DB.
 * Assumes all the ports for a certain esi label are added at once.
 */
int
evpn_util_esi_filter_entry_add(
    int unit,
    bcm_mpls_label_t esi_label,
    int *forbidden_ports,
    int nof_forbidden_ports)
{
    bcm_field_entry_t esi_port_handle;
    bcm_field_entry_t esi_label_handle;

    while (nof_forbidden_ports--)
    {
        int forbidden_port = forbidden_ports[nof_forbidden_ports];

        printf("Adding ESI Filter entry with esi_label=0x%x forbidden_port=0x%x\n", esi_label, forbidden_port);
        BCM_IF_ERROR_RETURN_MSG(cint_field_evpn_egress_esi_port_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &esi_port_handle,
                                                       esi_label, forbidden_port), "");
    }
    printf("Adding ESI Filter pass entry with esi_label=0x%x\n", esi_label);
    BCM_IF_ERROR_RETURN_MSG(cint_field_evpn_egress_esi_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id, &esi_label_handle,
                                              esi_label), "");


    return BCM_E_NONE;
}
