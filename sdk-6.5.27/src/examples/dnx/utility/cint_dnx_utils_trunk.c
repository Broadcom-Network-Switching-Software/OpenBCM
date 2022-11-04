/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * File: cint_dnx_utils_trunk.c
 * Purpose: Utility functions for trunk.
 */
/*
 *
 * Usage:
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_trunk.c
 * cint
 * cint_trunk_utils_s trunk;
 * Fill trunk with all the parametrs;
 * print cint_trunk_utils_create(unit,&trunk);
 */

int TRUNK_PORTS_NOF_MAX = 8;

/*
 * Structure to hold information for TRUNK creation and usage.
 */
struct cint_trunk_utils_s
{
    int ports[TRUNK_PORTS_NOF_MAX];
    int ports_nof;
    bcm_trunk_t tid;
    int pool;
    int group;
    int headers_type_in;
    int headers_type_out;
    int vlan_domain;
    int vlan;

    bcm_trunk_member_t members[TRUNK_PORTS_NOF_MAX];
    bcm_trunk_info_t trunk_info;

    bcm_gport_t gport;
};


int
trunk_utils_gport_header_types_single_direction_set(
    int unit,
    int direction,
    int value_type,
    bcm_gport_t gport)
{
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;

    key.index = direction;
    key.type = bcmSwitchPortHeaderType;
    value.value = value_type;

    return bcm_switch_control_indexed_port_set(unit, gport, key, value);
}

int
trunk_utils_gport_header_types_set(
    int unit,
    int type_in,
    int type_out,
    bcm_gport_t gport)
{
    int direction;

    direction = 1; /** IN */
    BCM_IF_ERROR_RETURN_MSG(trunk_utils_gport_header_types_single_direction_set(unit, direction, type_in, gport), "");

    direction = 2; /** OUT */
    BCM_IF_ERROR_RETURN_MSG(trunk_utils_gport_header_types_single_direction_set(unit, direction, type_out, gport), "");

    return BCM_E_NONE;
}

/*
 * Create TRUNK port according to the information supplied in *trunk.
 */
int
cint_trunk_utils_create(
    int unit,
    cint_trunk_utils_s * trunk)
{
    char error_msg[200]={0,};
    int port_i;

    BCM_TRUNK_ID_SET(trunk->tid, trunk->pool, trunk->group);
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &trunk->tid), "");

    BCM_GPORT_TRUNK_SET(trunk->gport, trunk->tid);

    BCM_IF_ERROR_RETURN_MSG(trunk_utils_gport_header_types_set(unit, trunk->headers_type_in, trunk->headers_type_out, trunk->gport),
        "trunk->gport");

    for (port_i = 0; port_i < trunk->ports_nof; port_i++)
    {
        bcm_trunk_member_t * member = &trunk->members[port_i];
        int local_port = trunk->ports[port_i];

        bcm_trunk_member_t_init(member);
        BCM_GPORT_SYSTEM_PORT_ID_SET(member->gport, local_port);

        snprintf(error_msg, sizeof(error_msg), "port_%d", port_i);
        BCM_IF_ERROR_RETURN_MSG(trunk_utils_gport_header_types_set(unit, trunk->headers_type_in, trunk->headers_type_out, member->gport), error_msg);
    }

    bcm_trunk_info_t_init(&trunk->trunk_info);
    trunk->trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_set(unit, trunk->tid, &trunk->trunk_info, trunk->ports_nof , trunk->members), "");

    printf("Set trunk VLAN domain\n");
    snprintf(error_msg, sizeof(error_msg), "Vlan Domain = %d", trunk->vlan_domain);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, trunk->gport, bcmPortClassId, trunk->vlan_domain), error_msg);

    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, trunk->gport, bcmPortControlBridge, 1), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, trunk->gport, trunk->vlan), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, trunk->vlan, trunk->gport, 0), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_stp_set(unit, trunk->gport, BCM_STG_STP_FORWARD), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_learn_set(unit, trunk->gport, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_port_set(unit, trunk->gport, bcmVlanTranslateIngressMissDrop, 1), "");

    printf("Enable Ingress visibility of the created gport\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_gport_control_set(unit, trunk->gport,
                                                 BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                 bcmInstruGportControlVisEnable, TRUE), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_instru_gport_control_set(unit, trunk->gport,
                                                 BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                 bcmInstruGportControlVisForce, TRUE), "");

    printf("Enable Egress visibility of each of the created local ports\n");
    for (port_i = 0; port_i < trunk->ports_nof; port_i++)
    {
        bcm_trunk_member_t * member = &trunk->members[port_i];
        int local_port = trunk->ports[port_i];
        BCM_IF_ERROR_RETURN_MSG(bcm_instru_gport_control_set(unit, local_port,
                                                     BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                     bcmInstruGportControlVisEnable, TRUE), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_instru_gport_control_set(unit, local_port,
                                                     BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                     bcmInstruGportControlVisForce, TRUE), "");
    }

    return BCM_E_NONE;
}


