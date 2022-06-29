/** file cint_dnx_tsn_port_preemption.c
 * Used as an application reference to users to manage port preemption
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/**
 * \brief - enable/disable support of port preemption on the RX side
 * to change to 2 RMCs , compensation needs to be disabled
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - local port on which to enable support
 */
int cint_tsn_port_preemption_support_enable(int unit, int port)
{
    int rv = BCM_E_NONE;
    bcm_port_prio_config_t priority_config;

    /** disable port */
    rv = bcm_port_enable_set(unit, port, 0);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_enable_set \n");
        return rv;
    }

    /** configure 2 RMCs */
    rv = bcm_cosq_control_set(unit, port, -1, bcmCosqControlIngressPortDropPortProfileMap, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_cosq_control_set \n");
        return rv;
    }

    priority_config.nof_priority_groups = 2;
    priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerHigh;
    priority_config.priority_groups[0].source_priority = BCM_PORT_F_PRIORITY_0  | BCM_PORT_F_PRIORITY_1;
    priority_config.priority_groups[0].num_of_entries = -1;
    priority_config.priority_groups[1].sch_priority = bcmPortNifSchedulerLow;
    priority_config.priority_groups[1].source_priority = BCM_PORT_F_PRIORITY_2  | BCM_PORT_F_PRIORITY_3;
    priority_config.priority_groups[1].num_of_entries = -1;

    rv = bcm_port_priority_config_set(unit, port, &priority_config);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_priority_config_set \n");
        return rv;
    }

    rv = bcm_port_preemption_control_set(unit, port, bcmPortPreemptControlPreemptionSupport, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }

    /** enable port */
    rv = bcm_port_enable_set(unit, port, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_enable_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - enable\disable static TX port preemption
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - local port on which to enable support
 * enable [INPUT] - enable/disable
 */
int cint_tsn_static_port_preemption_tx_enable(int unit, int port, int enable)
{
    int rv = BCM_E_NONE;

    rv = bcm_port_preemption_control_set(unit, port, bcmPortPreemptControlEnableTx, enable);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - enable\disable dynamic TX port preemption
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - local port on which to enable support
 * verify_time [INPUT] - wait time between verification attempts
 * verify_attempts [INPUT] - number of verification attempts
 * enable [INPUT] - enable/disable
 */
int cint_tsn_dynamic_port_preemption_tx_enable(int unit, int port, int verify_time, int verify_attemps, int enable)
{
    int rv = BCM_E_NONE;

    /* set preemption verify time=10ms and attempts=3 */
    rv = bcm_port_preemption_control_set(unit, port, bcmPortPreemptControlVerifyTime, verify_time);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }
    rv = bcm_port_preemption_control_set(unit, port, bcmPortPreemptControlVerifyAttempts, verify_attemps);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }

    /* dynamic activation - set preemption verify enable */
    rv = bcm_port_preemption_control_set(unit, port, bcmPortPreemptControlVerifyEnable, enable);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }
    rv = bcm_port_preemption_control_set(unit, port, bcmPortPreemptControlEnableTx, enable);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - configure queue priority
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - local port on which to enable support
 * cosq [INPUT] - index for a specific queue
 * is_express [INPUT] - indicate if we want express or preemptable traffic
 */
int cint_tsn_port_preemption_configure_queue_priority(int unit, int port, int cosq, int is_express)
{
    int rv = BCM_E_NONE;
    bcm_gport_t tc_gport;
    int priority;

    BCM_COSQ_GPORT_PORT_TC_SET(tc_gport, port);

    if (is_express)
    {
        priority = BCM_COSQ_SP0;
    } else {
        priority = BCM_COSQ_SP1;
    }

    rv = bcm_cosq_control_set(unit, tc_gport, cosq, bcmCosqControlPrioritySelect, priority);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_port_preemption_control_set \n");
        return rv;
    }

    return BCM_E_NONE;
}
