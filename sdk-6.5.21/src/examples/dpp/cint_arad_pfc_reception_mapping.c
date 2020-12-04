/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_arad_pfc_reception_mapping.c
* Purpose: Examples of how to map PFC source to port target and map PFC source to relevant priority in all ports on same interface.
*
* 1. map PFC source to port target
*
*  Example - map PFC 0 to OTM port 1 and map PFC 1 to OTM port 2
*
*  Environment:
*    - Both OTM port 1 and OTM port 2 are mapped on the same physical port 14.
*      some relevant SOC properties
*      - ucode_port_1.BCM88650=10GBase-R14.0
*      - ucode_port_2.BCM88650=10GBase-R14.0
*      - port_priorities_1.BCM88650=2
*      - port_priorities_2.BCM88650=2
*  
*  Expectation:
*    1) when PFC 0 indication received, only traffic on port 1 is stopped.
*    2) when PFC 1 indication received, only traffic on port 2 is stopped.
*
*  To Activate Above Settings Run:
*    BCM> cint examples/dpp/cint_arad_pfc_reception_mapping.c
*    BCM> cint
*    cint> 
*    cint> cint_pfc_mapping_port_over_pfc_set_example(unit, 1, 0);
*    cint> cint_pfc_mapping_port_over_pfc_set_example(unit, 2, 1);
* 
*
* 2. map PFC source to relevant priority in all ports on same interface
*
*  Example - map PFC 0 to priority 0 in port 1 and port 2 on same interface.
*
*  Environment:
*    - Both OTM port 1 and OTM port 2 are mapped on the same physical port 14.
*      some relevant SOC properties
*      - ucode_port_1.BCM88650=10GBase-R14.0
*      - ucode_port_2.BCM88650=10GBase-R14.0
*      - port_priorities_1.BCM88650=2
*      - port_priorities_2.BCM88650=2
*  
*  Expectation:
*    when PFC 0 indication received, traffic of priority 0 on port 1 and traffic of priority 0 on port 2 are stopped.
*
*  To Activate Above Settings Run:
*    BCM> cint examples/dpp/cint_arad_pfc_reception_mapping.c
*    BCM> cint
*    cint> 
*    cint> cint_pfc_mapping_intf_cosq_pfc_set_example(unit, 1, 0);
*    Or 
*    cint> cint_pfc_mapping_intf_cosq_pfc_set_example(unit, 2, 0);
 */

int cint_pfc_mapping_port_over_pfc_set_example(
    int unit,
    int source_port, 
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;

    /* set target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_port_over_pfc_set_example\n");
        return rv;
    }
    
    /* set the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
	target.cosq = -1;
    target.flags = BCM_COSQ_FC_PORT_OVER_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int cint_pfc_mapping_port_over_pfc_unset_example(
    int unit,
    int source_port,
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
    
    /* unset the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
    target.cosq = -1;
    target.flags = BCM_COSQ_FC_PORT_OVER_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_delete(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* unset target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_port_over_pfc_unset_example\n");
        return rv;
    }

    return rv;
}

int cint_pfc_mapping_intf_cosq_pfc_set_example(
    int unit,
    int source_port,
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
        
    /*Setting target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_intf_cosq_pfc_set_example\n");
        return rv;
    }
    
    /* Set the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    /* source_port should be one of the ports mapped to the interface we would like to stop */
    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
    target.flags = BCM_COSQ_FC_INTF_COSQ_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int cint_pfc_mapping_intf_cosq_pfc_unset_example(
    int unit,
    int source_port,
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
  
    /* unset the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    /* source_port should be one of the ports mapped to the interface we would like to stop */
    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
    target.flags = BCM_COSQ_FC_INTF_COSQ_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_delete(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* unset target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_intf_cosq_pfc_set_example\n");
        return rv;
    }

    return rv;
}

