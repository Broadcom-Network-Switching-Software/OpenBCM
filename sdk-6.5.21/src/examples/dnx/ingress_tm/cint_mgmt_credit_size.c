/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Cint mgmt credit size
 * The role of this cint is to demonstrate the required set of confiugrations when credit size is different between 2 devices.
 * i.e assuming 2 FAPs and a Fabric
 *
 *      in_port (IXIA port)    port, loopback
 *          |                   /\
 * +-------------------+-------------------+
 * |                   |                   |
 * |    FAP 0          |        FAP 1      |
 * |                   |                   |
 * +---------------------------------------+
 * |                Fabric                 |
 * |                                       |
 * +---------------------------------------+
 *
 * * Getting traffic from fap0 'fap_0_in_port' with 'mac_address' as destination adress.
 * * Forwarding the traffic to fap1 'fap_1_port' (sw loopback).
 * * Forwarding the traffic back to fap0 'fap_0_in_port' (sw loopback).
 * * fap 0 credit size is different from fap1 credit size. This requires setting for each local fap the remote faps credit size
 * * and creating a delay tolerance profile (IPS) which fulfills the destination device credit size and destination port rate.
 */

/**
 * Set remote fap credit size for a local fap
 */
int cint_bcm_mgmt_remote_credit_size_set(int fap_unit, int dest_fap_unit, int credit_size)
{
    int rv = BCM_E_NONE;
    int nof_modids;
    int dest_modid;
    int modid_count;
    int i_dest_modid;

    /** get number of modids on destination fap */
    rv = bcm_stk_modid_count(dest_fap_unit, &nof_modids);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_stk_modid_count\n");
        return rv;
    }

    bcm_stk_modid_config_t dest_modid_array[nof_modids];
    rv = bcm_stk_modid_config_get_all(dest_fap_unit, nof_modids, dest_modid_array, &modid_count);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_stk_modid_config_get_all \n");
        return rv;
    }

    /** set remote credit size for all remote modids */
    for (i_dest_modid = 0; i_dest_modid < nof_modids; i_dest_modid++)
    {
        dest_modid = dest_modid_array[i_dest_modid].modid;
        rv = bcm_cosq_dest_credit_size_set(fap_unit, dest_modid, credit_size);
        if (BCM_FAILURE(rv))
        {
           printf("Error, bcm_cosq_dest_credit_size_set (dest_fap_id=%d, credit_size=%d) \n", dest_modid, credit_size);
           return rv;
        }
    }

    printf("cint_bcm_mgmt_remote_credit_size_set: PASS\n\n");
    return BCM_E_NONE;
}

/*
 * * Create delay tolerance profile according to destination fap credit size and destination port rate.
 * * Map VoQ to the created delay tolerance profile
 * * Note - the VoQ ID assumes DNX reference application.
 */
int cint_bcm_mgmt_delay_tolerenace_level_create(int fap_unit, int dest_fap_unit, bcm_port_t dest_port, int credit_size)
{
    int rv = BCM_E_NONE;
    int core;
    bcm_cosq_delay_tolerance_t credit_request_profile;
    bcm_cosq_delay_tolerance_preset_attr_t credit_request_attr;
    bcm_port_resource_t port_resource;
    int profile = 20; /** assuming profile 20 is free */
    int dest_modid, base_voq, cos;
    bcm_gport_t voq_gport;

    /** create delay tolerance profile according to speed and credit size */
    {
        /** get set of recommended values */
        rv = bcm_port_resource_get(dest_fap_unit, dest_port, &port_resource);
        if (BCM_FAILURE(rv))
        {
           printf("Error, bcm_port_resource_get (port=%d) \n", dest_port);
           return rv;
        }

        bcm_cosq_delay_tolerance_preset_attr_t_init(&credit_request_attr);
        credit_request_attr.rate =  port_resource.speed/1000;
        credit_request_attr.credit_size =  credit_size;
        credit_request_attr.flags = BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED;
        rv = bcm_cosq_delay_tolerance_preset_get(fap_unit, &credit_request_attr, &credit_request_profile);
        if (BCM_FAILURE(rv))
        {
           printf("Error, bcm_cosq_delay_tolerance_preset_get \n");
           return rv;
        }

        /** set the threshold */
        rv = bcm_cosq_delay_tolerance_level_set(fap_unit, profile, &credit_request_profile);
        if (BCM_FAILURE(rv))
        {
           printf("Error, bcm_cosq_delay_tolerance_level_set (profile=%d, credit_size=%d) \n", profile, credit_size);
           return rv;
        }
    }

    /** map designated voq to the created delay tolerance profile */
    {
        int modid_count, actual_modid_count;
        rv = bcm_stk_modid_count(dest_fap_unit, &modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_count\n");
            return rv;
        }
        bcm_stk_modid_config_t modid_array[modid_count];

        rv = bcm_stk_modid_config_get_all(dest_fap_unit, modid_count, modid_array, &actual_modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_config_get_all\n");
            return rv;
        }
        dest_modid = modid_array[0].modid;

        /** get voq id - assuming DNX reference application */
        rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(fap_unit, dest_modid, dest_port, &base_voq);
        if (BCM_FAILURE(rv))
        {
           printf("Error, appl_dnx_e2e_scheme_logical_port_to_base_voq_get (dest_modid=%d, dest_port=%d) \n", dest_modid, dest_port);
           return rv;
        }

        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);

        /** map voq to delay tolerance profile */
        for (cos = 0; cos < 8; cos++)
        {
            rv = bcm_cosq_gport_sched_set(fap_unit, voq_gport, cos, profile, 0);
            if (BCM_FAILURE(rv))
            {
               printf("Error, bcm_cosq_gport_sched_set (base_voq=%d, cos=%d, profile=%d) \n", base_voq, cos, profile);
               return rv;
            }
        }
    }

    printf("cint_bcm_mgmt_delay_tolerenace_level_create: PASS\n\n");
    return BCM_E_NONE;
}

/*
 * MGMT MGMT CREDIT SIZE END
 */

