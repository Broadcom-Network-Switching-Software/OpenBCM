/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
* File: cint_bridge_application.c
* Purpose: Example of Basic Bridging VLAN membership
*
* To Activate Settings Run: 
*      BCM> cint ../../../../src/examples/sand/cint_sand_utils_global.c 
*      BCM> cint ../../../../src/examples/sand/cint_sand_utils_vlan_translate.c 
*      BCM> cint ../../../../src/examples/sand/cint_bridge_application.c
*      BCM> cint 
*      cint> bridge_application_init(unit, in_port, out_port);
*      cint> bridge_application_run(unit);
*/

struct bridge_application_info_s
{
    int in_port;
    int out_port;
    int in_vid;
    int out_vid;
    int vsi;
    int in_gport;
    int out_gport;
    bcm_mac_t mac;
};

bridge_application_info_s bridge_application_info;

int
bridge_application_init(
    int unit,
    int in_port,
    int out_port)
{
    bridge_application_info.in_port = in_port;
    bridge_application_info.out_port = out_port;
    bridge_application_info.in_vid = 1;
    bridge_application_info.vsi = 2;
    bridge_application_info.out_vid = 3;

    bridge_application_info.mac[0] =
        bridge_application_info.mac[1] =
        bridge_application_info.mac[2] = bridge_application_info.mac[3] = bridge_application_info.mac[4] = 0;
    bridge_application_info.mac[5] = 2;

    return BCM_E_NONE;
}

int
bridge_application_conf(
    int unit,
    int in_port,
    int out_port,
    int vid,
    int vsi)
{
    int rv;
    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_create, vid\n");
    }
    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_create, vsi\n");
    }

    rv = bcm_port_class_set(unit, in_port, bcmPortClassId, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set, in_port\n");
        return rv;
    }

    bcm_vlan_port_t in_vlan_port;
    bcm_vlan_port_t_init(&in_vlan_port);
    in_vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    in_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vlan_port.match_vlan = vid;
    in_vlan_port.vsi = vsi;
    in_vlan_port.port = in_port;
    rv = bcm_vlan_port_create(unit, &in_vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create, ingress side\n");
        /*return rv;*/
    }

    /*
     * Create mact entry.
     */
    bcm_l2_addr_t l2_addr;
    bridge_application_info.mac[0] =
        bridge_application_info.mac[1] =
        bridge_application_info.mac[2] = bridge_application_info.mac[3] = bridge_application_info.mac[4] = 0;
    bridge_application_info.mac[5] = 2;

    bcm_l2_addr_t_init(&l2_addr, bridge_application_info.mac, vsi);
    l2_addr.flags = BCM_L2_STATIC;      /* static entry */
    l2_addr.port = out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

}

/*vsi != vlan*/
int
bridge_application_run(
    int unit)
{
    int rv;

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    /*
     * Enable the vids and vsi on device. 
     * We don't create in_vid because vid 1 already exists.
     */

    rv = bcm_vlan_create(unit, bridge_application_info.out_vid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_create, out_vid\n");
        return rv;
    }

    rv = bcm_vlan_create(unit, bridge_application_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_create, vsi\n");
        return rv;
    }

    /*
     *  Map ports to their vlan domains.
     */
    rv = bcm_port_class_set(unit, bridge_application_info.in_port, bcmPortClassId, bridge_application_info.in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set, in_port\n");
        return rv;
    }

    rv = bcm_port_class_set(unit, bridge_application_info.out_port, bcmPortClassId, bridge_application_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set, out_port\n");
        return rv;
    }

    /*
     * Create mapping between <in_port, in_vid> -> vsi. 
     */

    bcm_vlan_port_t in_vlan_port;
    bcm_vlan_port_t_init(&in_vlan_port);
    in_vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    in_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vlan_port.match_vlan = bridge_application_info.in_vid;
    in_vlan_port.vsi = bridge_application_info.vsi;
    in_vlan_port.port = bridge_application_info.in_port;
    rv = bcm_vlan_port_create(unit, &in_vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create, ingress side\n");
        return rv;
    }

    bridge_application_info.in_gport = in_vlan_port.vlan_port_id;

    /*
     *  Create mapping and translation between <out_port, vsi> -> out_vid.
     */

    /*
     * Create outlif for vlan translation. 
     */
    bcm_vlan_port_t out_vlan_port;
    bcm_vlan_port_t_init(&out_vlan_port);
    out_vlan_port.match_vlan = bridge_application_info.out_vid;
    out_vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : bridge_application_info.out_vid;
    out_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port.port = bridge_application_info.out_port;

    if (!is_jericho2)
    {
        /*
         * JER1 
         */

        out_vlan_port.vsi = bridge_application_info.vsi;

        rv = bcm_vlan_port_create(unit, &out_vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create, egress side\n");
            return rv;
        }

        bridge_application_info.out_gport = out_vlan_port.vlan_port_id;
    }
    else
    {
        /*
         * JR2 
         */

        out_vlan_port.vsi = 0;

        rv = bcm_vlan_port_create(unit, &out_vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create, egress side\n");
            return rv;
        }

        bridge_application_info.out_gport = out_vlan_port.vlan_port_id;

        rv = bcm_vswitch_port_add(unit, bridge_application_info.vsi, out_vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }

        /*
         * Need to add VLAN EDITING
         */

        /*
         * Update LLVP
         */
        bcm_port_tpid_class_t port_tpid_class;

        bcm_port_tpid_class_t_init(&port_tpid_class);

        port_tpid_class.tpid1 = 0x8100;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.port = bridge_application_info.in_port;
        port_tpid_class.tag_format_class_id = 4;
        port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;

        printf("bcm_port_tpid_class_set(port=%d, tag_format_class_id=%d)\n", port_tpid_class.port,
               port_tpid_class.tag_format_class_id);

        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_tpid_class_set( in_port )\n");
            return rv;
        }

        rv = vlan_translate_ive_eve_translation_set(unit, bridge_application_info.in_gport,     /* lif */
                                                    0x8100,     /* outer_tpid */
                                                    0x9100,     /* inner_tpid */
                                                    bcmVlanActionReplace,       /* outer_action */
                                                    bcmVlanActionNone,  /* inner_action */
                                                    bridge_application_info.out_vid,    /* new_outer_vid */
                                                    0,  /* new_inner_vid */
                                                    17, /* vlan_edit_profile */
                                                    port_tpid_class.tag_format_class_id,        /* tag_format */
                                                    TRUE        /* is_ive */
            );
    }

    /*
     * Create mact entry.
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, bridge_application_info.mac, bridge_application_info.vsi);
    l2_addr.flags = BCM_L2_STATIC;      /* static entry */
    l2_addr.port = bridge_application_info.out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

int
bridge_application_cleanup(
    int unit)
{
    int rv;

    rv = bcm_l2_addr_delete(unit, bridge_application_info.mac, bridge_application_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_delete.\n");
        print rv;
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, bridge_application_info.out_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_destroy, egress side.\n");
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, bridge_application_info.in_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_destroy, ingress side.\n");
        return rv;
    }

    rv = bcm_vlan_destroy(unit, bridge_application_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_destroy, vsi.\n");
        return rv;
    }

    rv = bcm_vlan_destroy(unit, bridge_application_info.out_vid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_destroy, egress vid.\n");
        return rv;
    }

    rv = bcm_port_class_set(unit, bridge_application_info.in_port, bcmPortClassId, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set, in_port\n");
        return rv;
    }

    rv = bcm_port_class_set(unit, bridge_application_info.out_port, bcmPortClassId, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set, out_port\n");
        return rv;
    }

    return rv;
}
