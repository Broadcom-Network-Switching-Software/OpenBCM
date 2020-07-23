/** file cint_voq_sram_only.c
 *  Used as an example on how to configure a voq to SRAM only mode
 *
 *  Note - this cint assumes DNX reference application obtain VOQ ID
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \brief - Set base voq which points to 'port' to SRAM only mode.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Destination logical port
 */
int
cint_voq_sram_only_set(int unit, bcm_port_t port)
{
    bcm_cosq_delay_tolerance_t delay_tolerance;
    bcm_switch_profile_mapping_t profile_mapping;
    int base_voq, profile, weight;
    bcm_gport_t voq_gport, rate_class_gport;
    int rv;


    /** get VOQ ID - assuming DNX reference application */
    rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, port, &base_voq);
    if (BCM_FAILURE(rv))
    {
        printf("appl_dnx_e2e_scheme_logical_port_to_base_voq_get failed \n");
        return rv;
    }

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);

    /**
     * Set  delay tolerace profile to SRAM only
     */

    /** get delay tolerace profile */
    rv = bcm_cosq_gport_sched_get(unit, voq_gport, 0, &profile, &weight);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_cosq_gport_sched_get failed \n");
        return rv;
    }

    /** Modify delay tolerance profile to SRAM only mode */
    bcm_cosq_delay_tolerance_level_get(unit, profile, &delay_tolerance);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_cosq_delay_tolerance_level_get failed \n");
        return rv;
    }

    delay_tolerance.flags |= BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;

    rv = bcm_cosq_delay_tolerance_level_set(unit, profile, &delay_tolerance);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_cosq_delay_tolerance_level_set failed \n");
        return rv;
    }

    /**
     * Set rate class to SRAM only
     */

    /** get rate class */
    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
    rv = bcm_cosq_profile_mapping_get(unit, voq_gport, 0, 0, &profile_mapping);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_cosq_profile_mapping_get failed \n");
        return rv;
    }

    /** set rate class to SRAM only */
    rate_class_gport = profile_mapping.mapped_profile;
    rv = bcm_cosq_control_set(unit, rate_class_gport, 0, bcmCosqControlOCBOnly, 1);
    if (BCM_E_NONE != rv)
    {
        printf("bcm_cosq_control_set failed \n");
        return rv;
    }

    return BCM_E_NONE;
}

