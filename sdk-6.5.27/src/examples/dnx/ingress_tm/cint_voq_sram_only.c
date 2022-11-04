/** file cint_voq_sram_only.c
 *  Used as an example on how to configure a voq to SRAM only mode
 *
 *  Note - this cint assumes DNX reference application obtain VOQ ID
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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


    /** get VOQ ID - assuming DNX reference application */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, port, &base_voq), "");

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);

    /**
     * Set  delay tolerace profile to SRAM only
     */

    /** get delay tolerace profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_get(unit, voq_gport, 0, &profile, &weight), "");

    /** Modify delay tolerance profile to SRAM only mode */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_delay_tolerance_level_get(unit, profile, &delay_tolerance), "");

    delay_tolerance.flags |= BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_delay_tolerance_level_set(unit, profile, &delay_tolerance), "");

    /**
     * Set rate class to SRAM only
     */

    /** get rate class */
    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_profile_mapping_get(unit, voq_gport, 0, 0, &profile_mapping), "");

    /** set rate class to SRAM only */
    rate_class_gport = profile_mapping.mapped_profile;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, rate_class_gport, 0, bcmCosqControlOCBOnly, 1), "");

    return BCM_E_NONE;
}

