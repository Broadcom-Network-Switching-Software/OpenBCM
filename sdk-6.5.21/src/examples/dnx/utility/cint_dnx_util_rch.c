/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
    int rv = BCM_E_NONE;
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = recycle_entry_params->recycle_app;
    recycle_entry.vlan_port_id = recycle_entry_params->control_lif_gport;

    if (recycle_entry.recycle_app == bcmL2EgressRecycleAppRedirectVrf) {
        recycle_entry.vrf = recycle_entry_params->vrf_redirect_params.vrf;
    }

    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    *recycle_entry_encap_id = recycle_entry.encap_id;
    return rv; 
}

/* create recycle entry with default values */
int create_recycle_entry_with_defaults(
    int unit,
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    recycle_entry_params_s recycle_entry_params;
    recycle_entry_params.recycle_app = bcmL2EgressRecycleAppDropAndContinue;
    rv = create_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_recycle_entry \n");
        return rv;
    }
    return rv; 
}

/* validation on recyle entry params */
int create_recycle_entry_verify(
    recycle_entry_params_s * recycle_entry_params, 
    bcm_l2_egress_recycle_app_t expected_recycle_app) {

    int rv = BCM_E_NONE; 
    if (recycle_entry_params->recycle_app != expected_recycle_app) {
        printf("unexpected recycle app \n");
        rv = BCM_E_PARAM;
    }
    return rv; 
}

/* create recycle entry for drop and continue */
int create_drop_and_continue_recyle_entry(
    int unit,
    recycle_entry_params_s * recycle_entry_params, 
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    rv = create_recycle_entry_verify(recycle_entry_params, bcmL2EgressRecycleAppDropAndContinue);
    if (rv != BCM_E_NONE) {
        printf("Error, create_drop_and_continue_recyle_entry \n");
        return rv;
    }
    rv = create_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_recycle_entry \n");
        return rv;
    }
    return rv; 
}

/* create recycle entry for vrf redirect */
int create_vrf_redirect_recycle_entry(
    int unit, 
    recycle_entry_params_s * recycle_entry_params, 
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    rv = create_recycle_entry_verify(recycle_entry_params, bcmL2EgressRecycleAppRedirectVrf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_recycle_entry_verify \n");
        return rv;
    }
    rv = create_recycle_entry(unit, recycle_entry_params, recycle_entry_encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_recycle_entry \n");
        return rv;
    }
    return rv; 
}


