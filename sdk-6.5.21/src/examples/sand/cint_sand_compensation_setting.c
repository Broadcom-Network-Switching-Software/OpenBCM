/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

int cint_packet_length_adjust_cb (
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int* ingress_compensation_ptr = user_data;
    if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, *ingress_compensation_ptr);
        if(rv != BCM_E_NONE)
        {
            return rv;
        }
    }
    return rv;
}

/*
 * Example of usage: cint_compensation_set(unit, 2, -2)
 */
int cint_compensation_set(
    int unit,
    int ingress_comp,
    int egress_comp)
{
    int rv = BCM_E_NONE;
    int port;
    bcm_gport_t gport;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("failed to get port bmps in cint_compensation_set");
        return rv;
    }
    rv = bcm_cosq_gport_traverse(unit, cint_packet_length_adjust_cb, &ingress_comp);
    if (rv != BCM_E_NONE)
    {
        printf("failed to traverse gports and set ingress compensation");
        return rv;
    }

    BCM_PBMP_ITER(config.all, port)
    {
        if(!BCM_PBMP_MEMBER(config.sfi, port))
        {
            BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
            rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, egress_comp);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_control_set - unicast\n");
                return rv;
            }
            BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport, port);
            rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, egress_comp);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_cosq_control_set - multicast\n");
                return rv;
            }
        }
    }
    printf("cint_compensation_set: PASS.\n");
    return rv;
}

/*
 * Example of usage: cint_compensation_set(unit, 2, -2)
 */
int cint_egress_port_compensation_set(
    int unit,
    int port,
    int egress_comp)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;

    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
    rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, egress_comp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_cosq_control_set - unicast\n");
        return rv;
    }
    printf("cint_compensation_set: PASS.\n");
    return rv;
}
