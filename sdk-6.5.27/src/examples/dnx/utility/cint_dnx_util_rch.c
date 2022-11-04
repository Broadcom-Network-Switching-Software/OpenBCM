/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This file provides recycle header creation functionality
 */

/* fields specific to vrf redirect app */
struct vrf_redirect_params_s {
    bcm_vrf_t vrf; 
};


struct recycle_entry_params_s 
{
    bcm_l2_egress_recycle_app_t recycle_app;
    bcm_gport_t control_lif_gport; 
    vrf_redirect_params_s vrf_redirect_params;

};

/*
 * Create a Recycle Entry.
 * unit - relevant unit 
 * recycle_entry_params - recycle entry params 
 * recycle_entry_encap_id - returned encap id.
 */
int create_recycle_entry(
    int unit, 
    recycle_entry_params_s * recycle_entry_params, 
    int *recycle_entry_encap_id)
{
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = recycle_entry_params->recycle_app;
    recycle_entry.vlan_port_id = recycle_entry_params->control_lif_gport;

    if (recycle_entry.recycle_app == bcmL2EgressRecycleAppRedirectVrf) {
        recycle_entry.vrf = recycle_entry_params->vrf_redirect_params.vrf;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "");
    *recycle_entry_encap_id = recycle_entry.encap_id;
    return BCM_E_NONE; 
}

/* create recycle entry with default values */
int create_recycle_entry_with_defaults(
    int unit,
    int *recycle_entry_encap_id)
{
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    recycle_entry_params_s recycle_entry_params;
    recycle_entry_params.recycle_app = bcmL2EgressRecycleAppDropAndContinue;
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id), ""); 
    /** need to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0)
    {
        int cs_profile_id = 5;
        BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, *recycle_entry_encap_id, cs_profile_id), "");
    }
    return BCM_E_NONE; 
}

/* validation on recyle entry params */
int create_recycle_entry_verify(
    recycle_entry_params_s * recycle_entry_params, 
    bcm_l2_egress_recycle_app_t expected_recycle_app) {

    if (recycle_entry_params->recycle_app != expected_recycle_app) {
        printf("unexpected recycle app \n");
        return BCM_E_PARAM;
    }
    return BCM_E_NONE; 
}

/* create recycle entry for drop and continue */
int create_drop_and_continue_recyle_entry(
    int unit,
    recycle_entry_params_s * recycle_entry_params, 
    int *recycle_entry_encap_id)
{
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry_verify(recycle_entry_params, bcmL2EgressRecycleAppDropAndContinue), "");
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id), ""); 
    /** need to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0)
    {
        int cs_profile_id = 5;
        BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, *recycle_entry_encap_id, cs_profile_id), "");
    }
    return BCM_E_NONE; 
}

/* create recycle entry for vrf redirect */
int create_vrf_redirect_recycle_entry(
    int unit, 
    recycle_entry_params_s * recycle_entry_params, 
    int *recycle_entry_encap_id)
{
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry_verify(recycle_entry_params, bcmL2EgressRecycleAppRedirectVrf), "");
    BCM_IF_ERROR_RETURN_MSG(create_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id), "");
    /** need to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0)
    {
        int cs_profile_id = 5;
        BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, *recycle_entry_encap_id, cs_profile_id), "");
    }
    return BCM_E_NONE; 
}

int update_recycle_entry(
        int unit,
        recycle_entry_params_s * recycle_entry_params,
        int recycle_entry_encap_id)
{
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_get(unit, recycle_entry_encap_id, &recycle_entry), "");

    recycle_entry.flags = recycle_entry.flags | BCM_L2_EGRESS_REPLACE | BCM_L2_EGRESS_WITH_ID;
    recycle_entry.vlan_port_id = recycle_entry_params->control_lif_gport;

    if (recycle_entry.recycle_app == bcmL2EgressRecycleAppRedirectVrf) {
        recycle_entry.vrf = recycle_entry_params->vrf_redirect_params.vrf;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "");
    return BCM_E_NONE;
}


