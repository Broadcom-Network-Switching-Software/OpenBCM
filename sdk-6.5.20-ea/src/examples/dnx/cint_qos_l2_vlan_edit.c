/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_qos_l2_vlan_it.c Purpose: qos mapping for IVE&EVE (uniform & pipeNextNameSpace model). 
 */

/*
 * Configuration:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/sand/Cint_vswitch_metro_mp_vlan_port.c
 * cint ../../../../src/examples/sand/cint_qos_l3_remark.c
 * cint ../../../../src/examples/dnx/cint_qos_l2_vlan_edit.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int inPort = 200; 
 * int outPort = 201;
 * rv = l2_qos_ive_nop_eve_replace_add_main(unit,inPort,outPort);
 * print rv; 
 */


int ing_qos_gport = -1;
int egr_qos_gport = -1;
int l2_in_map_id = -1;
int l2_phb_opcode = -1;
int l2_remark_opcode = -1;

/*
 * Add the QoS map configurations.
 * For bridging packets, map L2 ETH.PCPDEI to NWK_QOS/IN_DSCP_EXP
 * The ingress mapping relastionship is: NWK_QOS = ETH.PCPDEI + 1
 * The egress mapping relastionship is:  NWK_QOS = NWK_QOS - 2
 */
int
qos_map_l2_pcpdei_config(int unit)
{
    return qos_map_phb_remark_set(unit,
                                  BCM_QOS_MAP_L2_OUTER_TAG,
                                  BCM_QOS_MAP_L2_OUTER_TAG,
                                  BCM_QOS_MAP_L2_OUTER_TAG,
                                  ing_qos_gport,
                                  egr_qos_gport,
                                  7, 6);
}

/*
 * Clear the QoS map configurations.
 */
int
qos_map_l2_pcpdei_clear(int unit)
{
    return qos_map_phb_remark_clear(unit,
                                    BCM_QOS_MAP_L2_OUTER_TAG,
                                    BCM_QOS_MAP_L2_OUTER_TAG,
                                    BCM_QOS_MAP_L2_OUTER_TAG,
                                    ing_qos_gport,
                                    egr_qos_gport,
                                    7, 6);
}

/*
 * qos ingress L2 phb/remark mapping with outer/inner/double tag(s)
 * outer tag: ~outer-pcp -> tc  ~outer-cfi -> dp ~outer-pcp_dei -> nwk_qos
 * inner tag: inner-pcp -> TC, inner-cfi -> DP, inner-pcp_dei -> nwk_qos
 * double tags: (outer-pcp | inner-pcp) -> tc, (outer-cfi | inner-cfi)-> dp, inner-pcp_dei + outer-pcp_dei -> nwk_qos 
 */
int
qos_map_l2_ingress_mapping_set(int unit, uint32 mapping_type, int gport)
{
    bcm_qos_map_t l2_in_map;
    int rv = BCM_E_NONE;
    uint32 flags;
    int pcp_dei;
    int inner_pcp_dei;

    /** Clear structure */
    bcm_qos_map_t_init(&l2_in_map);

    /** 1. Create qos-profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    if (l2_in_map_id != -1)
    {
        l2_in_map_id = l2_in_map_id & 0xFFFF;
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, flags, &l2_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf("bcm_qos_map_create ingress cos-profile: %d\n", l2_in_map_id);

    /** Create opcode for PHB */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    if (l2_phb_opcode != -1)
    {
        l2_phb_opcode &=  0xFFFF;
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, flags, &l2_phb_opcode);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress opcode\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    printf("bcm_qos_map_create ingress PHB opcode: %d\n", l2_phb_opcode);

    /** Add the maps for PHB */
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_phb_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | mapping_type;
    rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_add() for ingress opcode for PHB\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*add qos mapping entry*/
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_PHB | mapping_type;
    if (mapping_type & BCM_QOS_MAP_L2_TWO_TAGS)
    {
        for (pcp_dei = 0; pcp_dei < 16; pcp_dei++) 
        {
            for (inner_pcp_dei = 0; inner_pcp_dei < 16; inner_pcp_dei++)
            {
                bcm_qos_map_t_init(&l2_in_map);
                l2_in_map.inner_pkt_pri = (inner_pcp_dei >> 1) & 0x7;
                l2_in_map.inner_pkt_cfi = inner_pcp_dei  & 1;
                l2_in_map.pkt_pri = (pcp_dei >> 1) & 0x7;
                l2_in_map.pkt_cfi = pcp_dei & 1;
                l2_in_map.int_pri =  ((inner_pcp_dei | pcp_dei) & 0xf) >> 1 ;
                l2_in_map.color = ((inner_pcp_dei | pcp_dei) & 0xf) & 1;
                rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_phb_opcode);
                if (rv != BCM_E_NONE) {
                    printf("error (%d) in bcm_qos_map_add() for ingress two tags qos-map for PHB\n", rv);
                    printf("(%s)\n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }
    else
    {
        for (pcp_dei = 0; pcp_dei < 16; pcp_dei++) {
            bcm_qos_map_t_init(&l2_in_map);
            if (mapping_type & BCM_QOS_MAP_L2_INNER_TAG)
            {
                l2_in_map.inner_pkt_pri = pcp_dei >> 1;
                l2_in_map.inner_pkt_cfi = pcp_dei & 1;
                l2_in_map.int_pri = pcp_dei >> 1;
                l2_in_map.color = pcp_dei & 1;
            }
            else /**outer*/
            {
                l2_in_map.pkt_pri = pcp_dei >> 1;
                l2_in_map.pkt_cfi = pcp_dei & 1;
                l2_in_map.int_pri = ((~pcp_dei) & 0xf) >> 1;
                l2_in_map.color = (~pcp_dei) & 1;
            }
            rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_phb_opcode);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in bcm_qos_map_add() for ingress qos-map for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /** Create opcode for remark*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l2_remark_opcode != -1)
    {
        l2_remark_opcode &=  0xFFFF;
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, flags, &l2_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress remark opcode\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_qos_map_create ingress REMARK opcode: %d\n", l2_remark_opcode);
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | mapping_type;
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_remark_opcode;
    rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_add() for ingress opcode for remarking\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /*add qos mapping entry*/
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | mapping_type;
    if (mapping_type & BCM_QOS_MAP_L2_TWO_TAGS)
    {
        for (pcp_dei = 0; pcp_dei < 16; pcp_dei++) 
        {
            for (inner_pcp_dei = 0; inner_pcp_dei < 16; inner_pcp_dei++)
            {
                bcm_qos_map_t_init(&l2_in_map);
                l2_in_map.inner_pkt_pri = (inner_pcp_dei >> 1) & 0x7;
                l2_in_map.inner_pkt_cfi = inner_pcp_dei & 1;
                l2_in_map.pkt_pri = (pcp_dei >> 1) & 0x7;
                l2_in_map.pkt_cfi = pcp_dei & 1;
                l2_in_map.remark_int_pri = (inner_pcp_dei << 4) | pcp_dei;
                rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_remark_opcode);
                if (rv != BCM_E_NONE) {
                    printf("error (%d) in bcm_qos_map_add() for ingress qos-map for two tags remark\n", rv);
                    printf("(%s)\n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }
    else
    {
        for (pcp_dei = 0; pcp_dei < 16; pcp_dei++) {
            bcm_qos_map_t_init(&l2_in_map);
            if (mapping_type & BCM_QOS_MAP_L2_INNER_TAG)
            {
                l2_in_map.inner_pkt_pri = pcp_dei >> 1;
                l2_in_map.inner_pkt_cfi = pcp_dei & 1;
                l2_in_map.remark_int_pri = pcp_dei;
            }
            else /**outer*/
            {
                l2_in_map.pkt_pri = pcp_dei >> 1;
                l2_in_map.pkt_cfi = pcp_dei & 1;
                l2_in_map.remark_int_pri = (~pcp_dei) & 0xf;
            }
            rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_remark_opcode);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in bcm_qos_map_add() for ingress qos-map for remark\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }
    rv = bcm_qos_port_map_set(unit, gport, l2_in_map_id, -1);
    if (rv) {
        printf("error (%d) setting ingress qos profile in InLif\n", rv);
        return rv;
    }
        
    return rv;
}

/*
 * qos ingress MPLS phb/remark mapping with EXP.
 * The example supposes qos profile and opcode have been created already for both PHB and REMARK.
 * tc = exp >> 1
 * dp = exp & 1
 * nwk_qos = exp << 4 | exp
 */
int
qos_map_mpls_ingress_mapping_set(int unit, int gport)
{
    bcm_qos_map_t l2_in_map;
    int rv = BCM_E_NONE;
    uint32 flags;
    int exp_value;

    /** Add the maps for PHB */
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_phb_opcode;
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_add() for ingress opcode for PHB\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*add qos mapping entry*/
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB;
    for (exp_value = 0; exp_value < 8; exp_value++)
    {
        bcm_qos_map_t_init(&l2_in_map);
        l2_in_map.exp = exp_value;
        l2_in_map.int_pri =  exp_value >> 1 ;
        l2_in_map.color = exp_value & 1;
        rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_phb_opcode);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress two tags qos-map for PHB\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /** Create opcode for remark*/
    printf("bcm_qos_map_create ingress REMARK opcode: %d\n", l2_remark_opcode);
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_remark_opcode;
    rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_add() for ingress opcode for remarking\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /*add qos mapping entry*/
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
    for (exp_value = 0; exp_value < 8; exp_value++)
    {
        bcm_qos_map_t_init(&l2_in_map);
        l2_in_map.exp = exp_value;
        l2_in_map.remark_int_pri = (exp_value << 4) | exp_value;
        rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_remark_opcode);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress qos-map for two tags remark\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_qos_port_map_set(unit, gport, l2_in_map_id, -1);
    if (rv) {
        printf("error (%d) setting ingress qos profile in InLif\n", rv);
        return rv;
    }
        
    return rv;
}


/*release qos resource*/
int
qos_map_l2_ingress_mapping_clean_up(int unit)
{
    int rv = BCM_E_NONE;

    if (l2_in_map_id != -1)
    {
        rv = bcm_qos_map_destroy(unit,l2_in_map_id);
        if (rv) {
            printf("fail bcm_qos_map_destroy map id (%d)\n", l2_in_map_id);
            return rv;
        }
    }
    if (l2_phb_opcode != -1)
    {
        rv = bcm_qos_map_destroy(unit,l2_phb_opcode);
        if (rv) {
            printf("fail bcm_qos_map_destroy phb opcode (%d)\n", l2_phb_opcode);
            return rv;
        }
    }
    if (l2_remark_opcode != -1)
    {
        rv = bcm_qos_map_destroy(unit,l2_remark_opcode);
        if (rv) {
            printf("fail bcm_qos_map_destroy remark opcode (%d)\n", l2_phb_opcode);
            return rv;
        }
    }
}

/*set vlan translation*/
int
vlan_translation_action_create_and_set(
    int unit,
    int is_ingress,
    int action_id,
    bcm_vlan_action_t outer_vlan_action,
    bcm_vlan_action_t inner_vlan_action,
    bcm_vlan_action_t outer_prio_action,
    bcm_vlan_action_t inner_prio_action,
    uint16 outer_tpid,
    uint16 inner_tpid)
{
    int rv;
    int flags;
    bcm_vlan_action_set_t action;

    /*
     * Create new Action IDs 
     */
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    flags |= BCM_VLAN_ACTION_SET_WITH_ID;
    rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Fill Action 
     */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_vlan_action;
    action.dt_outer_pkt_prio = outer_prio_action;
    action.dt_inner = inner_vlan_action;
    action.dt_inner_pkt_prio = inner_prio_action;

    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}

/*
 * Cross connect a inlif to outlif for verifying qos map for EVE.
 * No IVE action is added. In EVE stage, {replace, add} is performed.
 * The incoming packet should be single tagged, and outgoing packet will be double tagged.
 * For outgoing packet:
 *     outer-tag.pcpdei is mapped from nwk_qos.
 *     inner-tag.pcpdei is copied from packet's tag.pcpei.
 */
int
l2_qos_ive_nop_eve_replace_add_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int egress_outer_vid1 = 500;
    int egress_outer_vid2 = 600;
    int egress_inner_vid1 = 700;
    int egress_inner_vid2 = 800;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };      /* DA */

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize s_c_format */
    rv = port_tpid_class_add(unit, inPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }
    
    /** After IVE recognize s_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. IVE action settings - Nop 
     */
    rv = vlan_translation_action_create_and_set(unit, 1 /* Ingress */, ingress_action_id,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 4. EVE action settings - Replace and add 
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionReplace, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionOuterAdd, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 5. Map LLVP and VLAN Port profile to Action 
     */
    rv = vlan_translate_action_map_set(unit, 1 /* Ingress */ , ingress_action_id,
                                       tag_format_s, in_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }
    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s, out_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    /*
     * 6. VPN create 
     */
    rv = vswitch_create(unit, vsi_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create\n");
        return rv;
    }

    /*
     * 7. Create VLAN-Ports 
     */
    rv = vswitch_vlan_port_create(unit,
                                  in_vlan_port_id,
                                  inPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid1,
                                  match_inner_vid1,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN,
                                  in_vlan_port_edit_prfoile,
                                  egress_outer_vid1,
                                  egress_inner_vid1,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = vswitch_vlan_port_create(unit,
                                  out_vlan_port_id,
                                  outPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid2,
                                  match_inner_vid2,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN,
                                  in_vlan_port_edit_prfoile,
                                  egress_outer_vid2,
                                  egress_inner_vid2,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }

    /*
     * 8. Add MACT entry 
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}

/*
 * Cross connect a inlif to outlif for verifying qos map for IVE.
 * No EVE action is added. In IVE stage, {replace, replace} is performed.
 * The incoming packet should be double tagged, and outgoing packet is double tagged.
 * For outgoing packet:
 *     outer-tag.pcpdei is copied from packet's tag.inner-pcpei.
 *     inner-tag.pcpdei is copied from packet's tag.outer-pcpei.
 */
int
l2_qos_ive_replace_replace_eve_nop_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int tag_format_s_c = 16;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int ingress_outer_vid = 1000;
    int ingress_inner_vid = 2000;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };      /* DA */

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize s_c_format */
    rv = port_tpid_class_add(unit, inPort, s_tpid, c_tpid, tag_format_s_c);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /** After IVE recognize s_c_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, c_tpid, tag_format_s_c);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. IVE action settings - Replace and Replace, Exchange PCPDEI between outer and inner
     */
    rv = vlan_translation_action_create_and_set(unit, 1 /* Ingress */, ingress_action_id,
                                                bcmVlanActionReplace, bcmVlanActionReplace,
                                                bcmVlanActionInnerAdd, bcmVlanActionOuterAdd,
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 4. EVE action settings - Nop
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                bcmVlanActionNone, bcmVlanActionNone, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 5. Map LLVP and VLAN Port profile to Action 
     */
    rv = vlan_translate_action_map_set(unit, 1 /* Ingress */ , ingress_action_id,
                                       tag_format_s_c, in_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }
    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s_c, out_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    /*
     * 6. VPN create 
     */
    rv = vswitch_create(unit, vsi_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create\n");
        return rv;
    }

    /*
     * 7. Create VLAN-Ports 
     */
    rv = vswitch_vlan_port_create(unit,
                                  in_vlan_port_id,
                                  inPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid1,
                                  match_inner_vid1,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = vswitch_vlan_port_create(unit,
                                  out_vlan_port_id,
                                  outPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid2,
                                  match_inner_vid2,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }

    /*
     * 8. Add MACT entry 
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}

/*
 * Cross connect a inlif to outlif for verifying qos map for IVE.
 * No EVE action is added. In IVE stage, {Add, Add} is performed.
 * The incoming packet should be untagged, and outgoing packet is double tagged.
 * For outgoing packet:
 *     outer-tag.pcpdei is mapped from tc/dp.
 *     inner-tag.pcpdei is mapped from tc/dp.
 */
int
l2_qos_untag_ive_add_add_eve_nop_main(int unit, int inPort, int outPort)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int tag_format_s_c = 16;
    int tag_format_untag = 0;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int ingress_outer_vid = 1000;
    int ingress_inner_vid = 2000;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };      /* DA */

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize untagged */
    rv = port_tpid_class_add(unit, inPort, BCM_PORT_TPID_CLASS_TPID_INVALID, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_untag);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /** After IVE recognize s_c_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, c_tpid, tag_format_s_c);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. IVE action settings - Replace and Replace, Exchange PCPDEI between outer and inner
     */
    rv = vlan_translation_action_create_and_set(unit, 1 /* Ingress */, ingress_action_id,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 4. EVE action settings - Nop
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                bcmVlanActionNone, bcmVlanActionNone, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    /*
     * 5. Map LLVP and VLAN Port profile to Action 
     */
    rv = vlan_translate_action_map_set(unit, 1 /* Ingress */ , ingress_action_id,
                                       tag_format_untag, in_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }
    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s_c, out_vlan_port_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    /*
     * 6. VPN create 
     */
    rv = vswitch_create(unit, vsi_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create\n");
        return rv;
    }

    /*
     * 7. Create VLAN-Ports 
     */
    rv = vswitch_vlan_port_create(unit,
                                  in_vlan_port_id,
                                  inPort,
                                  vsi_id,
                                  BCM_VLAN_PORT_CREATE_INGRESS_ONLY,
                                  match_outer_vid1,
                                  match_inner_vid1,
                                  BCM_VLAN_PORT_MATCH_PORT,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }
    rv = vswitch_vlan_port_create(unit,
                                  out_vlan_port_id,
                                  outPort,
                                  vsi_id,
                                  0,
                                  match_outer_vid2,
                                  match_inner_vid2,
                                  BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED,
                                  in_vlan_port_edit_prfoile,
                                  0,
                                  0,
                                  out_vlan_port_edit_prfoile,
                                  TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_vlan_port_create\n");
        return rv;
    }

    /*
     * 8. Add vswitch cross connect 
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}


/** Set vlan translation actions for the given action-id.*/
int
vlan_translation_action_set(
    int unit,
    int is_ingress,
    int action_id,
    bcm_vlan_action_t outer_vlan_action,
    bcm_vlan_action_t inner_vlan_action,
    bcm_vlan_action_t outer_prio_action,
    bcm_vlan_action_t inner_prio_action,
    uint16 outer_tpid,
    uint16 inner_tpid)
{
    int rv;
    int flags;
    bcm_vlan_action_set_t action;

    /*
     * Fill Action 
     */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_vlan_action;
    action.dt_outer_pkt_prio = outer_prio_action;
    action.dt_inner = inner_vlan_action;
    action.dt_inner_pkt_prio = inner_prio_action;

    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /** Replace can be executed directly. This flag is not needed.*/
    /*flags |= BCM_VLAN_ACTION_SET_REPLACE;*/

    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}


/*
 * No EVE action. In IVE stage, {replace, replace} is performed.
 * The incoming packet should be double tagged, and outgoing packet is double tagged.
 * For Ingress PCP mapping(explicit mapping):
 *     {PCP, DEI=0} --> {PCP + 1, DEI=1}
 *     {PCP, DEI=1} --> {PCP - 1, DEI=0}
 * For outgoing packet:
 *     outer-tag.pcpdei is explicit mapping from packet outer-tag.pcpdei.
 *     inner-tag.pcpdei is same with outer-tag.pcpdei.
 */
int
vlan_translation_ingress_pcpdei_setting_run(
    int unit)
{
    int rv;
    int ingress_action_id = 5;
    int l2_qos_map_id;
    bcm_qos_map_t l2_qos_map;
    uint32 flags;
    int qos_value;
    int in_vlan_port_id = 10000;

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);

    /** Set pcpdei action for IVE*/
    rv = vlan_translation_action_set(unit, TRUE/*Ingress*/, ingress_action_id,
                                     bcmVlanActionReplace, bcmVlanActionReplace,
                                     bcmVlanActionAdd, bcmVlanActionAdd,
                                     0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_action_replace\n");
        return rv;
    }

    /** Create L2 PCPDEI MAP-ID*/
    flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_INGRESS;
    rv = bcm_qos_map_create(unit, flags, &l2_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_qos_map_create\n");
        return rv;
    }

    if (verbose > 1) {
        printf("pcp dep profile for In-AC is: 0x%08X\n", l2_qos_map_id);
    }

    /** Add L2 PCPDEI MAP*/
    flags = BCM_QOS_MAP_L2_VLAN_PCP;
    for (qos_value = 0; qos_value < 8; qos_value++) {
        bcm_qos_map_t_init(&l2_qos_map);
        l2_qos_map.color = 0;
        l2_qos_map.int_pri = qos_value;

        l2_qos_map.pkt_cfi = 1;
        l2_qos_map.pkt_pri = (qos_value + 1) & 0x7;

        rv = bcm_qos_map_add(unit, flags, &l2_qos_map, l2_qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_qos_map_add\n");
            return rv;
        }

        l2_qos_map.color = 1;
        l2_qos_map.int_pri = qos_value;

        l2_qos_map.pkt_cfi = 0;
        l2_qos_map.pkt_pri = (qos_value - 1) & 0x7;

        rv = bcm_qos_map_add(unit, flags, &l2_qos_map, l2_qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_qos_map_add\n");
            return rv;
        }
    }

    /** Add the map-id to in-AC*/
    rv = bcm_qos_port_map_set(unit, in_vlan_port_id, l2_qos_map_id, -1);
    if (rv != BCM_E_NONE)
    {
            printf("Error, bcm_qos_port_map_set\n");
            return rv;
    }

    return rv;
}

/*update vlan port with qos pipe model and egress output PCP/DEI*/
int
qos_vlan_port_update_with_qos_model(
    int unit,
    int vlan_port_id,
    uint8 pkt_pri,
    uint8 pkt_cfi,
    int qos_model)
{
    int rv;
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = vlan_port_id;
    rv = bcm_vlan_port_find(unit,&vlan_port);
    vlan_port.flags |= (BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE);
    vlan_port.pkt_pri = pkt_pri;
    vlan_port.pkt_cfi = pkt_cfi;
    vlan_port.egress_qos_model.egress_qos = qos_model;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    return rv;
}

/*
 * create egress qos profile, mapping qos var to pcp&dei
 */
int qos_l2_egress_map_create(
    int unit,
    int remark_profile,
    int remark_opcode)
{
    int rv = BCM_E_NONE;
    int flags;
    bcm_qos_map_t l2_eg_map;
    int qos_var;
    int dp;

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    if (remark_profile != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, flags, &remark_profile);
    if (rv != BCM_E_NONE) {
        printf("error in egress bcm_qos_map_create()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
    if (remark_opcode != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, flags, &remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in egress opcode bcm_qos_map_create()\n");
        return rv;
    }
    bcm_qos_map_t_init(&l2_eg_map);
    l2_eg_map.opcode = remark_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, remark_profile);
    if (rv != BCM_E_NONE)
    {
        printf("error in qos_map_egress_profile_map bcm_qos_map_add():OPCODE\n");
        return rv;
    }
    /*outer pcp mapping*/
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG;
    for (dp = 0; dp < 4; dp++)
    {
        for (qos_var = 0; qos_var < 256; qos_var++)
        {
            bcm_qos_map_t_init(&l2_eg_map);
            l2_eg_map.int_pri = qos_var;
            l2_eg_map.color = dp;
            l2_eg_map.pkt_pri = qos_var & 0x7;
            l2_eg_map.pkt_cfi = (~qos_var) & 0x1;
            rv = bcm_qos_map_add(unit, flags, &l2_eg_map, remark_opcode);
            if (rv != BCM_E_NONE)
            {
                printf("error in bcm_qos_map_add outer\n");
                return rv;
            }
        }
    }
    /*inner pcp mapping*/
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_INNER_TAG;
    for (dp = 0; dp < 4; dp++)
    {
        for (qos_var = 0; qos_var < 256; qos_var++)
        {
            bcm_qos_map_t_init(&l2_eg_map);
            l2_eg_map.int_pri = qos_var;
            l2_eg_map.color = dp;
            l2_eg_map.inner_pkt_pri = qos_var & 0x7;
            l2_eg_map.inner_pkt_cfi = (~qos_var) & 0x1;
            rv = bcm_qos_map_add(unit, flags, &l2_eg_map, remark_opcode);
            if (rv != BCM_E_NONE)
            {
                printf("error in bcm_qos_map_add inner\n");
                return rv;
            }
        }
    }
    /*next layer*/
    flags = BCM_QOS_MAP_REMARK;
    for (dp = 0; dp < 4; dp++)
    {
        for (qos_var = 0; qos_var < 256; qos_var++)
        {
            bcm_qos_map_t_init(&l2_eg_map);
            l2_eg_map.int_pri = qos_var;
            l2_eg_map.color = dp;
            l2_eg_map.remark_int_pri = qos_var;
            l2_eg_map.remark_color = dp;
            rv = bcm_qos_map_add(unit, flags, &l2_eg_map, remark_profile);
            if (rv != BCM_E_NONE)
            {
                printf("error in bcm_qos_map_add inner\n");
                return rv;
            }
        }
    }

    return rv;
}

/*
 * create egress qos profile, mapping qos var to both outer and inner pcp&dei
 */
int qos_l2_egress_map_two_tags(
    int unit,
    uint8 outer_pcp,
    uint8 outer_dei,
    uint8 inner_pcp,
    uint8 inner_dei)
{
    int rv = BCM_E_NONE;
    int flags;
    bcm_qos_map_t l2_eg_map;
    int qos_var;
    int dp;
    int remark_profile = 3;
    int remark_opcode = 13;

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID;
    rv = bcm_qos_map_create(unit, flags, &remark_profile);
    if (rv != BCM_E_NONE) {
        printf("error in egress bcm_qos_map_create()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID;
    rv = bcm_qos_map_create(unit, flags, &remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in egress opcode bcm_qos_map_create()\n");
        return rv;
    }
    bcm_qos_map_t_init(&l2_eg_map);
    l2_eg_map.opcode = remark_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, remark_profile);
    if (rv != BCM_E_NONE)
    {
        printf("error in qos_map_egress_profile_map bcm_qos_map_add():OPCODE\n");
        return rv;
    }
    /*outer/inner pcp mapping*/
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_TWO_TAGS;
    for (dp = 0; dp < 4; dp++)
    {
        for (qos_var = 0; qos_var < 256; qos_var++)
        {
            bcm_qos_map_t_init(&l2_eg_map);
            l2_eg_map.int_pri = qos_var;
            l2_eg_map.color = dp;
            l2_eg_map.pkt_pri = outer_pcp;
            l2_eg_map.pkt_cfi = outer_dei;
            l2_eg_map.inner_pkt_pri = inner_pcp;
            l2_eg_map.inner_pkt_cfi = inner_dei;
            rv = bcm_qos_map_add(unit, flags, &l2_eg_map, remark_opcode);
            if (rv != BCM_E_NONE)
            {
                printf("error in bcm_qos_map_add outer\n");
                return rv;
            }
        }
    }

    return rv;
}


/*main function to test AC QOS pipe model at forward stage*/
int vlan_port_qos_fwd_qos_model_main(int unit, int inPort, int outPort, uint8 pipe_pcp, uint8 pipe_dei, int qos_model)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int egress_action_id = 6;
    int out_vlan_edit_prfoile = 3;
    int in_vlan_port_id = 10000;
    int out_vlan_port_id = 10010;
    int match_vid1 = 100;
    int match_vid2 = 200;
    int egress_outer_vid = 500;
    int egress_inner_vid = 700;
    bcm_mac_t da = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; 
    bcm_vlan_port_t vp;
    bcm_vlan_port_translation_t port_trans;

    bcm_vlan_port_translation_t_init(&port_trans);

    BCM_GPORT_SUB_TYPE_LIF_SET(in_vlan_port_id, 0, in_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_vlan_port_id, in_vlan_port_id);
    BCM_GPORT_SUB_TYPE_LIF_SET(out_vlan_port_id, 0, out_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(out_vlan_port_id, out_vlan_port_id);

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 1. Set global TPIDs 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    /*
     * 2. Set LLVP configuration 
     */
    /** Incoming-packet recognize s_c_format */
    rv = port_tpid_class_add(unit, inPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }
    
    /** After IVE recognize s_format */
    rv = port_tpid_class_add(unit, outPort, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, tag_format_s);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /*
     * 3. Create VLAN-Ports 
     */
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = inPort;
    vp.match_vlan = match_vid1;
    vp.vlan_port_id = in_vlan_port_id;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, match_vid1, inPort, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = outPort;
    vp.match_vlan = match_vid2;
    vp.vlan_port_id = out_vlan_port_id;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    vp.pkt_pri = pipe_pcp;
    vp.pkt_cfi = pipe_dei;
    vp.egress_qos_model.egress_qos = qos_model;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, match_vid2, outPort, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    /*
     * 4. EVE action settings - Replace and add 
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionReplace, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionAdd, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id,
                                       tag_format_s, out_vlan_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    port_trans.new_outer_vlan = egress_outer_vid;
    port_trans.new_inner_vlan = egress_inner_vid;
    port_trans.gport = out_vlan_port_id;
    port_trans.vlan_edit_class_id = out_vlan_edit_prfoile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     * 5. Cross connect
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cross connect port1(0x%08X) <--> port2(0x%08X)\n", gports.port1, gports.port2);
        return rv;
    }

    return rv;
}

/*main function to test AC QOS pipe model at encap stage*/
int vlan_port_qos_encap_qos_model_main(int unit, int inPort, int outPort, uint8 pipe_pcp, uint8 pipe_dei, int qos_model)
{
    int rv;
    int egress_action_id = 6;
    int untag_format = 0;
    int out_vlan_edit_prfoile = 3;
    int egress_outer_vid = 500;
    int egress_inner_vid = 700;
    uint16 in_vlan = 15;
    uint16 out_vlan = 100;
    int encap_id;
    int encoded_fec;
    bcm_mac_t my_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    int vrf = 15;
    bcm_vlan_port_t vlan_port;
    l3_ingress_intf l3_ing_if;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_l3_host_t l3host;
    bcm_vlan_port_translation_t port_trans;

    /*
     * 1. Create VLAN-Ports 
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = inPort;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = in_vlan;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create in \n");
        return rv;
    }
    ing_qos_gport = vlan_port.vlan_port_id;
    rv = bcm_vlan_gport_add(unit, in_vlan, inPort, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = outPort;
    vlan_port.match_vlan = out_vlan;
    vlan_port.pkt_pri = pipe_pcp;
    vlan_port.pkt_cfi = pipe_dei;
    vlan_port.egress_qos_model.egress_qos = qos_model;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    egr_qos_gport = vlan_port.vlan_port_id;
    
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, out_vlan);
        return rv;
    }
    rv = bcm_vswitch_port_add(unit, out_vlan, egr_qos_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to bcm_vswitch_port_add\n", rv);
        return rv;
    }

    /*
     * 2. EVE action settings - Replace and add 
     */
    rv = vlan_translation_action_create_and_set(unit, 0 /* Egress */, egress_action_id,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                bcmVlanActionAdd, bcmVlanActionAdd, 
                                                0x9100, 0x8100);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translation_create_and_set\n");
        return rv;
    }

    rv = vlan_translate_action_map_set(unit, 0 /* Egress */ , egress_action_id, untag_format, out_vlan_edit_prfoile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vlan_translate_action_map_set\n");
        return rv;
    }

    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = egress_outer_vid;
    port_trans.new_inner_vlan = egress_inner_vid;
    port_trans.gport = egr_qos_gport;
    port_trans.vlan_edit_class_id = out_vlan_edit_prfoile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     *3. create in & out RIF 
     */
    rv = intf_eth_rif_create(unit, in_vlan, my_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    rv = intf_eth_rif_create(unit, out_vlan, my_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }
    /*Set Incoming ETH-RIF properties */
    l3_ingress_intf_init(&l3_ing_if);
    l3_ing_if.vrf = vrf;
    l3_ing_if.intf_id = in_vlan;
    rv = intf_ingress_rif_set(unit, &l3_ing_if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set \n");
        return rv;
    }

    /*
     * 4. Create ARP+AC
     */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = next_hop_mac;
    l3eg.vlan = out_vlan;
    l3eg.flags = 0;
    l3eg.vlan_port_id = egr_qos_gport;
    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }
    encap_id = l3eg.encap_id;
    /*
     * 5. build FEC points to out-RIF and next-hop MAC address
     */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.encap_id = encap_id;
    l3eg.intf = out_vlan;
    l3eg.port = outPort;
    l3eg.flags = 0;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /*
     * 6. add host entry with FEC-ptr
     */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = encoded_fec;      /* FEC */

    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rv);
    }

    return rv;
}


