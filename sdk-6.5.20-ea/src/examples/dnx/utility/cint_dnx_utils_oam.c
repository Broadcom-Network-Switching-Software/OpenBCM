/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_utils_oam.c Purpose: OAM utility functions.
 */

/**
 * Create cross-connected VLAN ports
 *
 *
 * @param unit
 * @param port1
 * @param port2
 * @param vid1
 * @param vid2
 * @param vlan_port_id_1
 * @param vlan_port_id_2
 *
 * @return int
 */
int dnx_oam_cross_connected_vlan_ports_create(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2,
    bcm_vlan_t vid1,
    bcm_vlan_t vid2,
    bcm_gport_t* vlan_port_id_1,
    bcm_gport_t* vlan_port_id_2)
{
    bcm_error_t rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t vp2;
    bcm_vswitch_cross_connect_t cross_connect;

    if(nof_mep == 0)
    {
        /* Set port classification ID */
        rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_port_class_set.\n", rv);
            return rv;
        }
    
        /* Set port classification ID */
        rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_port_class_set.\n", rv);
            return rv;
        }
    }
    /* Create VLAN ports */
    bcm_vlan_port_t_init(&vp1);
    vp1.flags = 0;
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp1.port = port1;
    vp1.match_vlan = vid1;
    rv = bcm_vlan_port_create(unit,&vp1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_port_create.\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid1, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_port_create.\n", rv);
        return rv;
    }

    printf("port=%d, vid=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp1.port, vp1.match_vlan,
           vp1.vlan_port_id, vp1.encap_id);

    *vlan_port_id_1 = vp1.vlan_port_id;

    bcm_vlan_port_t_init(&vp2);
    vp2.flags = 0;
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp2.port = port2;
    vp2.match_vlan = vid2;
    rv = bcm_vlan_port_create(unit,&vp2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid2, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_gport_add.\n", rv);
        return rv;
    }

    printf("port=%d, vid=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp2.port, vp2.match_vlan,
           vp2.vlan_port_id, vp2.encap_id);

    *vlan_port_id_2 = vp2.vlan_port_id;

    /**
     * Cross-connect the ports
     */
    bcm_vswitch_cross_connect_t_init(&cross_connect);
    cross_connect.port1 = vp1.vlan_port_id;
    cross_connect.port2 = vp2.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error no %d, in bcm_vswitch_cross_connect_add.\n", rv);
        return rv;
    }

    return rv;
}
