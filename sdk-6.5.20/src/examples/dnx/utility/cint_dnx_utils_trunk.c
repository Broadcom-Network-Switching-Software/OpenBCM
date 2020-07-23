/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
    int rv = BCM_E_NONE;
    int direction;

    direction = 1; /** IN */
    rv = trunk_utils_gport_header_types_single_direction_set(unit, direction, type_in, gport);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in trunk_utils_gport_header_types_single_direction_set\n");
        return rv;
    }

    direction = 2; /** OUT */
    rv = trunk_utils_gport_header_types_single_direction_set(unit, direction, type_out, gport);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in trunk_utils_gport_header_types_single_direction_set\n");
        return rv;
    }

    return rv;
}

/*
 * Create TRUNK port according to the information supplied in *trunk.
 */
int
cint_trunk_utils_create(
    int unit,
    cint_trunk_utils_s * trunk)
{
    int rv = BCM_E_NONE;
    int port_i;

    BCM_TRUNK_ID_SET(trunk->tid, trunk->pool, trunk->group);
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &trunk->tid);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_trunk_create\n");
        return rv;
    }

    BCM_GPORT_TRUNK_SET(trunk->gport, trunk->tid);

    rv = trunk_utils_gport_header_types_set(unit, trunk->headers_type_in, trunk->headers_type_out, trunk->gport);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in trunk_utils_gport_header_types_set trunk->gport\n");
        return rv;
    }

    for (port_i = 0; port_i < trunk->ports_nof; port_i++)
    {
        bcm_trunk_member_t * member = &trunk->members[port_i];
        int local_port = trunk->ports[port_i];

        bcm_trunk_member_t_init(member);
        BCM_GPORT_SYSTEM_PORT_ID_SET(member->gport, local_port);

        rv = trunk_utils_gport_header_types_set(unit, trunk->headers_type_in, trunk->headers_type_out, member->gport);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in trunk_utils_gport_header_types_set port_%d\n", port_i);
            return rv;
        }
    }

    bcm_trunk_info_t_init(&trunk->trunk_info);
    trunk->trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    rv = bcm_trunk_set(unit, trunk->tid, &trunk->trunk_info, trunk->ports_nof , trunk->members);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_trunk_set\n");
        return rv;
    }

    printf("Set trunk VLAN domain\n");
    rv = bcm_port_class_set(unit, trunk->gport, bcmPortClassId, trunk->vlan_domain);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set Vlan Domain = %d\n", trunk->vlan_domain);
        return rv;
    }

    rv = bcm_port_control_set(unit, trunk->gport, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_control_set\n");
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, trunk->gport, trunk->vlan);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, trunk->vlan, trunk->gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_port_stp_set(unit, trunk->gport, BCM_STG_STP_FORWARD);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_stp_set\n");
        return rv;
    }

    rv = bcm_port_learn_set(unit, trunk->gport, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_learn_set\n");
        return rv;
    }

    rv = bcm_vlan_control_port_set(unit, trunk->gport, bcmVlanTranslateIngressMissDrop, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_control_port_set\n");
        return rv;
    }

    printf("Enable Ingress visibility of the created gport\n");
    rv = bcm_instru_gport_control_set(unit, trunk->gport,
                                                 BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                 bcmInstruGportControlVisEnable, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_instru_gport_control_set\n");
        return rv;
    }

    rv = bcm_instru_gport_control_set(unit, trunk->gport,
                                                 BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                 bcmInstruGportControlVisForce, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_instru_gport_control_set\n");
        return rv;
    }

    printf("Enable Egress visibility of each of the created local ports\n");
    for (port_i = 0; port_i < trunk->ports_nof; port_i++)
    {
        bcm_trunk_member_t * member = &trunk->members[port_i];
        int local_port = trunk->ports[port_i];
        rv = bcm_instru_gport_control_set(unit, local_port,
                                                     BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                     bcmInstruGportControlVisEnable, TRUE);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_instru_gport_control_set\n");
            return rv;
        }

        rv = bcm_instru_gport_control_set(unit, local_port,
                                                     BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                     bcmInstruGportControlVisForce, TRUE);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_instru_gport_control_set\n");
            return rv;
        }
    }

    return rv;
}


