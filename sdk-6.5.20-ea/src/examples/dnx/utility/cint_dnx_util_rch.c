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
    vrf_redirect_params_s vrf_redirect_params;
};

/*
 * Create a Recycle Entry.
 * unit - relevant unit
 * recycle_entry_encap_id - returned encap id.
 */
int create_recycle_entry(
    int unit,
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppDropAndContinue;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    *recycle_entry_encap_id = recycle_entry.encap_id;

    return rv;
}

/* create recycle entry for vrf redirect */
int create_vrf_redirect_recycle_entry(
    int unit, 
    recycle_entry_params_s * recycle_entry_params, 
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = recycle_entry_params->recycle_app;

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
