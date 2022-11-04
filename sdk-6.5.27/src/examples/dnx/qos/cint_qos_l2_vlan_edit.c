/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_qos_l2_vlan_it.c Purpose: qos mapping for IVE&EVE (uniform & pipeNextNameSpace model). 
 */

/*
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_tpid.c
  cint ../../../../src/examples/sand/Cint_vswitch_metro_mp_vlan_port.c
  cint ../../../../src/examples/sand/cint_qos_l3_remark.c
  cint ../../../../src/examples/dnx/qos/cint_qos_l2_vlan_edit.c
  cint
  int unit = 0; 
  int rv = 0; 
  int inPort = 200; 
  int outPort = 201;
  rv = l2_qos_ive_nop_eve_replace_add_main(unit,inPort,outPort);
  print rv; 
 * Test Scenario - end
 *
 */


int ing_qos_gport = -1;
int egr_qos_gport = -1;
int l2_in_map_id = -1;
int l2_phb_opcode = -1;
int l2_remark_opcode = -1;
int l2_eg_map_id = -1;
int l2_eg_remark_opcode = -1;

/*
 * qos ingress L2 phb/remark mapping with outer/inner/double tag(s)
 * outer tag: ~outer-pcp -> tc  ~outer-cfi -> dp ~outer-pcp_dei -> nwk_qos
 * inner tag: inner-pcp -> TC, inner-cfi -> DP, inner-pcp_dei -> nwk_qos
 * double tags: (outer-pcp | inner-pcp) -> tc, (outer-cfi | inner-cfi)-> dp, inner-pcp_dei + outer-pcp_dei -> nwk_qos 
 */
int
qos_map_l2_ingress_mapping_set(int unit, uint32 mapping_type, int gport, int exist_profile)
{
    bcm_qos_map_t l2_in_map;
    uint32 flags;
    int pcp_dei;
    int inner_pcp_dei;

    /** Clear structure */
    bcm_qos_map_t_init(&l2_in_map);

    if (!exist_profile)
    {
        /** 1. Create qos-profile */
        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
        if (l2_in_map_id != -1)
        {
            l2_in_map_id = l2_in_map_id & 0xFFFF;
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &l2_in_map_id), "for ingress cos-profile");
        printf("bcm_qos_map_create ingress cos-profile: %d\n", l2_in_map_id);
    }

    /** Create opcode for PHB */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    if (l2_phb_opcode != -1)
    {
        l2_phb_opcode &=  0xFFFF;
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &l2_phb_opcode), "for ingress opcode");
    printf("bcm_qos_map_create ingress PHB opcode: %d\n", l2_phb_opcode);

    /** Add the maps for PHB */
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_phb_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | mapping_type;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id), "for ingress opcode for PHB");

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
                BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_phb_opcode),
                    "for ingress two tags qos-map for PHB");
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
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_phb_opcode),
                "for ingress qos-map for PHB");
        }
    }

    /** Create opcode for remark*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l2_remark_opcode != -1)
    {
        l2_remark_opcode &=  0xFFFF;
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &l2_remark_opcode),
        "for ingress remark opcode");

    printf("bcm_qos_map_create ingress REMARK opcode: %d\n", l2_remark_opcode);
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | mapping_type;
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_remark_opcode;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id),
        "for ingress opcode for remarking");
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
                BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_remark_opcode),
                    "for ingress qos-map for two tags remark");
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
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_remark_opcode),
                "for ingress qos-map for remark");
        }
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, gport, l2_in_map_id, -1), "setting ingress qos profile in InLif");
        
    return BCM_E_NONE;
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
    uint32 flags;
    int exp_value;

    /** Add the maps for PHB */
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_phb_opcode;
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id),
        "for ingress opcode for PHB");

    /*add qos mapping entry*/
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB;
    for (exp_value = 0; exp_value < 8; exp_value++)
    {
        bcm_qos_map_t_init(&l2_in_map);
        l2_in_map.exp = exp_value;
        l2_in_map.int_pri =  exp_value >> 1 ;
        l2_in_map.color = exp_value & 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_phb_opcode),
            "for ingress two tags qos-map for PHB");
    }

    /** Create opcode for remark*/
    printf("bcm_qos_map_create ingress REMARK opcode: %d\n", l2_remark_opcode);
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    bcm_qos_map_t_init(&l2_in_map);
    l2_in_map.opcode = l2_remark_opcode;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id),
        "for ingress opcode for remarking");
    /*add qos mapping entry*/
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
    for (exp_value = 0; exp_value < 8; exp_value++)
    {
        bcm_qos_map_t_init(&l2_in_map);
        l2_in_map.exp = exp_value;
        l2_in_map.remark_int_pri = (exp_value << 4) | exp_value;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_in_map, l2_remark_opcode),
            "for ingress qos-map for two tags remark");
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, gport, l2_in_map_id, -1),
        "setting ingress qos profile in InLif");
        
    return BCM_E_NONE;
}


/*release qos resource*/
int
qos_map_l2_ingress_mapping_clean_up(int unit)
{
    char error_msg[200]={0,};

    if (l2_in_map_id != -1)
    {
        snprintf(error_msg, sizeof(error_msg), "map id (%d)", l2_in_map_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_destroy(unit,l2_in_map_id), error_msg);
    }
    if (l2_phb_opcode != -1)
    {
        snprintf(error_msg, sizeof(error_msg), "phb opcode (%d)", l2_phb_opcode);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_destroy(unit,l2_phb_opcode), error_msg);
    }
    if (l2_remark_opcode != -1)
    {
        snprintf(error_msg, sizeof(error_msg), "remark opcode (%d)", l2_remark_opcode);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_destroy(unit,l2_remark_opcode), error_msg);
    }
    return BCM_E_NONE;
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
    int flags;
    bcm_vlan_action_set_t action;

    /*
     * Create new Action IDs 
     */
    flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    flags |= BCM_VLAN_ACTION_SET_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, flags, &action_id), "");

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

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, flags, action_id, &action), "");

    return BCM_E_NONE;
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
    char error_msg[200]={0,};
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id;
    int out_vlan_port_id;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int egress_outer_vid1 = 500;
    int egress_outer_vid2 = 600;
    int egress_inner_vid1 = 700;
    int egress_inner_vid2 = 800;
    bcm_vlan_port_t vlan_port;

    /*
     * 1. Create VSI
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_create_with_id(unit, vsi_id), "");

    /*
     * 2. Create VLAN-Ports
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = inPort;
    vlan_port.match_vlan = match_outer_vid1;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    in_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_outer_vid1, inPort, 0), "");

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = outPort;
    vlan_port.match_vlan = match_outer_vid2;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    out_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_outer_vid1, inPort, 0), "");

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 3. IVE action settings - Nop
     */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,  in_vlan_port_id,                   /* out ac  */
                                                       0x9100,                            /* outer_tpid */
                                                       0x8100,                            /* inner_tpid */
                                                       bcmVlanActionNone,                 /* outer_action */
                                                       bcmVlanActionNone,                 /* inner_action */
                                                       egress_outer_vid1,                 /* new_outer_vid */
                                                       egress_inner_vid1,                 /* new_inner_vid */
                                                       in_vlan_port_edit_prfoile,         /* vlan_edit_profile */
                                                       tag_format_s,                      /* tag_format - here is stag */
                                                       TRUE                               /* is_ive */
                                                       ), "create egress vlan editing");

    /*
     * 4. EVE action settings - Replace and add 
     */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, out_vlan_port_id,              /* outlif                  */
                                                                      0x9100,                        /* outer_tpid              */
                                                                      0x8100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionModify,       /* Inner tpid action modify*/
                                                                      bcmVlanActionReplace,          /* outer_action            */
                                                                      bcmVlanActionAdd,              /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionOuterAdd,         /* inner_pri_action        */
                                                                      egress_outer_vid2,             /* new_outer_vid           */
                                                                      egress_inner_vid2,             /* new_inner_vid           */
                                                                      out_vlan_port_edit_prfoile,    /* vlan_edit_profile       */
                                                                      tag_format_s,                  /* tag_format              */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    /*
     * 5. Add cross connect entry
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    snprintf(error_msg, sizeof(error_msg), "in cross connect port1(0x%08X) <--> port2(0x%08X)", gports.port1, gports.port2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);

    return BCM_E_NONE;
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
    char error_msg[200]={0,};
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int tag_format_s_c = 16;
    int ingress_action_id = 5;
    int egress_action_id = 6;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id;
    int out_vlan_port_id;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int egress_outer_vid1 = 500;
    int egress_outer_vid2 = 600;
    int egress_inner_vid1 = 700;
    int egress_inner_vid2 = 800;
    bcm_vlan_port_t vlan_port;

    /*
     * 1. Create VSI
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_create_with_id(unit, vsi_id), "");

    /*
     * 2. Create VLAN-Ports
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.port = inPort;
    vlan_port.match_vlan = match_outer_vid1;
    vlan_port.match_inner_vlan = match_inner_vid1;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    in_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_outer_vid1, inPort, 0), "");

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.port = outPort;
    vlan_port.match_vlan = match_outer_vid2;
    vlan_port.match_inner_vlan = match_inner_vid2;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    out_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_outer_vid2, outPort, 0), "");

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 3. IVE action settings - Replace and Replace, Exchange PCPDEI between outer and inner
     */
    action_id_1 = ingress_action_id;
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, in_vlan_port_id,               /* inlif                  */
                                                                      0x9100,                        /* outer_tpid              */
                                                                      0x8100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionModify,       /* Inner tpid action modify*/
                                                                      bcmVlanActionReplace,          /* outer_action            */
                                                                      bcmVlanActionReplace,          /* inner_action            */
                                                                      bcmVlanActionInnerAdd,         /* outer_pri_action        */
                                                                      bcmVlanActionOuterAdd,         /* inner_pri_action        */
                                                                      1000,                          /* new_outer_vid           */
                                                                      2000,                          /* new_inner_vid           */
                                                                      in_vlan_port_edit_prfoile,     /* vlan_edit_profile       */
                                                                      tag_format_s_c,                /* tag_format              */
                                                                      TRUE                           /* is_ive                  */
                                                                      ), "");

    /*
     * 4. EVE action settings - Nop
     */
    action_id_1 = egress_action_id;
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,  out_vlan_port_id,                  /* outlif  */
                                                       0x9100,                            /* outer_tpid */
                                                       0x8100,                            /* inner_tpid */
                                                       bcmVlanActionNone,                 /* outer_action */
                                                       bcmVlanActionNone,                 /* inner_action */
                                                       0,                                 /* new_outer_vid */
                                                       0,                                 /* new_inner_vid */
                                                       out_vlan_port_edit_prfoile,        /* vlan_edit_profile */
                                                       tag_format_s_c,                    /* tag_format - here is dtag */
                                                       FALSE                              /* is_ive */
                                                       ), "");

    /*
     * 5. Add cross connect entry
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    snprintf(error_msg, sizeof(error_msg), "in cross connect port1(0x%08X) <--> port2(0x%08X)", gports.port1, gports.port2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);

    return BCM_E_NONE;
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
    char error_msg[200]={0,};
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    int tag_format_s = 4;
    int tag_format_s_c = 16;
    int tag_format_untag = 0;
    int in_vlan_port_edit_prfoile = 2;
    int out_vlan_port_edit_prfoile = 3;
    int vsi_id = 10;
    int in_vlan_port_id;
    int out_vlan_port_id;
    int match_outer_vid1 = 100;
    int match_outer_vid2 = 200;
    int match_inner_vid1 = 300;
    int match_inner_vid2 = 400;
    int egress_outer_vid1 = 500;
    int egress_outer_vid2 = 600;
    int egress_inner_vid1 = 700;
    int egress_inner_vid2 = 800;
    bcm_vlan_port_t vlan_port;

    /*
     * 1. Create VSI
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_create_with_id(unit, vsi_id), "");

    /*
     * 2. Create VLAN-Ports
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = inPort;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    in_vlan_port_id = vlan_port.vlan_port_id;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.port = outPort;
    vlan_port.match_vlan = match_outer_vid2;
    vlan_port.match_inner_vlan = match_inner_vid2;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    out_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_outer_vid2, outPort, 0), "");

    ing_qos_gport = in_vlan_port_id;
    egr_qos_gport = out_vlan_port_id;

    /*
     * 3. IVE action settings - Replace and Replace, Exchange PCPDEI between outer and inner
     */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, in_vlan_port_id,               /* inlif                  */
                                                                      0x9100,                        /* outer_tpid              */
                                                                      0x8100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionModify,       /* Inner tpid action modify*/
                                                                      bcmVlanActionAdd,              /* outer_action            */
                                                                      bcmVlanActionAdd,              /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionAdd,              /* inner_pri_action        */
                                                                      1000,                          /* new_outer_vid           */
                                                                      2000,                          /* new_inner_vid           */
                                                                      in_vlan_port_edit_prfoile,     /* vlan_edit_profile       */
                                                                      tag_format_untag,               /* tag_format              */
                                                                      TRUE                           /* is_ive                  */
                                                                      ), "");

    /*
     * 4. EVE action settings - Nop
     */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,  out_vlan_port_id,                  /* outlif  */
                                                       0x9100,                            /* outer_tpid */
                                                       0x8100,                            /* inner_tpid */
                                                       bcmVlanActionNone,                 /* outer_action */
                                                       bcmVlanActionNone,                 /* inner_action */
                                                       0,                                 /* new_outer_vid */
                                                       0,                                 /* new_inner_vid */
                                                       out_vlan_port_edit_prfoile,        /* vlan_edit_profile */
                                                       tag_format_s_c,                    /* tag_format - here is dtag */
                                                       FALSE                              /* is_ive */
                                                       ), "");

    /*
     * 5. Add cross connect entry
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    snprintf(error_msg, sizeof(error_msg), "in cross connect port1(0x%08X) <--> port2(0x%08X)", gports.port1, gports.port2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);


    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, flags, action_id, &action), "");

    return BCM_E_NONE;
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
    int ingress_action_id = 5;
    int l2_qos_map_id;
    bcm_qos_map_t l2_qos_map;
    uint32 flags;
    int qos_value;

    /** Set pcpdei action for IVE*/
    BCM_IF_ERROR_RETURN_MSG(vlan_translation_action_set(unit, TRUE/*Ingress*/, ingress_action_id,
                                     bcmVlanActionReplace, bcmVlanActionReplace,
                                     bcmVlanActionAdd, bcmVlanActionAdd,
                                     0x9100, 0x8100), "");

    /** Create L2 PCPDEI MAP-ID*/
    flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_INGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &l2_qos_map_id), "");

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

        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_qos_map, l2_qos_map_id), "");

        l2_qos_map.color = 1;
        l2_qos_map.int_pri = qos_value;

        l2_qos_map.pkt_cfi = 0;
        l2_qos_map.pkt_pri = (qos_value - 1) & 0x7;

        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_qos_map, l2_qos_map_id), "");
    }

    /** Add the map-id to in-AC*/
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, ing_qos_gport, l2_qos_map_id, -1), "");

    return BCM_E_NONE;
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
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = vlan_port_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_find(unit,&vlan_port), "");
    vlan_port.flags |= (BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE);
    vlan_port.pkt_pri = pkt_pri;
    vlan_port.pkt_cfi = pkt_cfi;
    vlan_port.egress_qos_model.egress_qos = qos_model;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    return BCM_E_NONE;
}

/*
 * create egress qos profile, mapping qos var to pcp&dei
 *   outer tag: nwk_qos & 0x7 -> pcp, ~nwk_qos & 0x7 -> dei
 *   inner tag: nwk_qos & 0x7 -> pcp, ~nwk_qos & 0x7 -> dei
 *   next_layer: nwk_qos -> remark_int_pri, dp -> remark_color
 */
int qos_l2_egress_map_create(
    int unit,
    int exist_profile)
{
    int flags;
    bcm_qos_map_t l2_eg_map;
    int qos_var;
    int dp;

    /* Create qos-profile */
    if (!exist_profile)
    {
        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
        if (l2_eg_map_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &l2_eg_map_id), "");
    }

    /* Create opcode profile */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    if (l2_eg_remark_opcode != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &l2_eg_remark_opcode), "");

    /* Bind opcode profile to qos profile for L2 */
    bcm_qos_map_t_init(&l2_eg_map);
    l2_eg_map.opcode = l2_eg_remark_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, l2_eg_map_id), "OPCODE");

    /* Outer pcp mapping*/
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
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, l2_eg_remark_opcode), "");
        }
    }

    /* Inner pcp mapping*/
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
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, l2_eg_remark_opcode), "inner");
        }
    }

    /* Next layer mapping */
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
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, l2_eg_map_id), "inner");
        }
    }

    return BCM_E_NONE;
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
    int flags;
    bcm_qos_map_t l2_eg_map;
    int qos_var;
    int dp;
    int remark_profile = 3;
    int remark_opcode = 13;

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &remark_profile), "");

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &remark_opcode), "");
    bcm_qos_map_t_init(&l2_eg_map);
    l2_eg_map.opcode = remark_opcode;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, remark_profile), "OPCODE");
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
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, remark_opcode), "outer");
        }
    }

    return BCM_E_NONE;
}

/*
 * Configuration for Forwarding plus one header(Generally L3 header) remarking. 
 * In this Example, the Forwarding header is ETH, and Forwarding plus one header is IPv4.
 * The maps for remarking Qos is : remarking_qos = nwk_qos + 1 + DP.
 *
 * egress_remark_profile: egress remark profile, must be a valid profile.
 * opcode: opcode for plus one header QoS mapping. If -1, the example will create an opcode itself.
 * is_ipv6: If the confiiguration is for IPv6.
 */
int qos_map_l2_fwd_plus_one_remark(int unit, int egress_remark_profile, int opcode, int is_ipv6, int with_ecn)
{
    int fwd_plus_1_opcode;
    int qos_value, dp_value;
    uint32 flags;
    bcm_qos_map_t eg_qos_map;

    /** Check if the given profile is valid*/
    if (egress_remark_profile == -1)
    {
        printf("error (%d) in qos_map_l2_fwd_plus_one_remark. Given profile (%d) is invalid!\n", BCM_E_PARAM, egress_remark_profile);
        return BCM_E_PARAM;
    }

    /** Create an opcode for fwd plus one remark.*/
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
    if (with_ecn)
    {
        flags |= BCM_QOS_MAP_ECN;
    }
    if (opcode != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
        fwd_plus_1_opcode = opcode;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &fwd_plus_1_opcode), "for fwd plus one opcode");

    printf("Create an opcode for fwd plus one header remarking: 0x%08X\n", fwd_plus_1_opcode);

    /** Map given profile to the opcode */
    bcm_qos_map_t_init(&eg_qos_map);

    eg_qos_map.opcode = fwd_plus_1_opcode;
    flags = is_ipv6 ? BCM_QOS_MAP_IPV6 : BCM_QOS_MAP_IPV4;
    flags |= BCM_QOS_MAP_L3_L2 | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_REMARK;
    if (with_ecn)
    {
        flags |= BCM_QOS_MAP_ECN;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &eg_qos_map, egress_remark_profile),
        "for mapping profile to fwd plus one opcode");

    /** Add QoS map for the protocol: QoS = nwk_qos + 1 + dp*/
    for (qos_value = 0; qos_value < 256; qos_value++)
    {
        for (dp_value = 0; dp_value < 3; dp_value++)
        {
            bcm_qos_map_t_init(&eg_qos_map);

            flags = is_ipv6 ? BCM_QOS_MAP_IPV6 : BCM_QOS_MAP_IPV4;
            if (with_ecn)
            {
                flags |= BCM_QOS_MAP_ECN;
            }
            flags |= BCM_QOS_MAP_REMARK;
            eg_qos_map.color = dp_value;
            eg_qos_map.int_pri = qos_value;
            eg_qos_map.dscp = (qos_value + 1 + dp_value) & 0xFF;
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &eg_qos_map, fwd_plus_1_opcode),
                "for mapping qos value with fwd plus one opcode");
        }
    }

    return BCM_E_NONE;
}

/*main function to test AC QOS pipe model at forward stage*/
int vlan_port_qos_fwd_qos_model_main(int unit, int inPort, int outPort, uint8 pipe_pcp, uint8 pipe_dei, int qos_model)
{
    char error_msg[200]={0,};
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
     * 1. Create VLAN-Ports 
     */
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = inPort;
    vp.match_vlan = match_vid1;
    vp.vlan_port_id = in_vlan_port_id;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vp), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_vid1, inPort, 0), "");
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = outPort;
    vp.match_vlan = match_vid2;
    vp.vlan_port_id = out_vlan_port_id;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    vp.pkt_pri = pipe_pcp;
    vp.pkt_cfi = pipe_dei;
    vp.egress_qos_model.egress_qos = qos_model;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vp), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, match_vid2, outPort, 0), "");

    /*
     * 2. EVE action settings - Replace and add 
     */
    action_id_1 = egress_action_id;
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, out_vlan_port_id,              /* outlif                  */
                                                                      0x9100,                        /* outer_tpid              */
                                                                      0x8100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionModify,       /* Inner tpid action modify*/
                                                                      bcmVlanActionReplace,          /* outer_action            */
                                                                      bcmVlanActionAdd,              /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionAdd,              /* inner_pri_action        */
                                                                      egress_outer_vid,              /* new_outer_vid           */
                                                                      egress_inner_vid,              /* new_inner_vid           */
                                                                      out_vlan_edit_prfoile,         /* vlan_edit_profile       */
                                                                      tag_format_s,                  /* tag_format              */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    /*
     * 3. Cross connect
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    snprintf(error_msg, sizeof(error_msg), "in cross connect port1(0x%08X) <--> port2(0x%08X)", gports.port1, gports.port2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);

    return BCM_E_NONE;
}

/*main function to test AC QOS pipe model at encap stage*/
int vlan_port_qos_encap_qos_model_main(int unit, int inPort, int outPort, uint8 pipe_pcp, uint8 pipe_dei, int qos_model)
{
    char error_msg[200]={0,};
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
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "in");
    ing_qos_gport = vlan_port.vlan_port_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, in_vlan, inPort, 0), "");

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = outPort;
    vlan_port.match_vlan = out_vlan;
    vlan_port.pkt_pri = pipe_pcp;
    vlan_port.pkt_cfi = pipe_dei;
    vlan_port.egress_qos_model.egress_qos = qos_model;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    egr_qos_gport = vlan_port.vlan_port_id;

    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", out_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, out_vlan), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, out_vlan, egr_qos_gport), "");

    /*
     * 2. EVE action settings - Replace and add 
     */
    action_id_1 = egress_action_id;
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, egr_qos_gport,                 /* outlif                  */
                                                                      0x9100,                        /* outer_tpid              */
                                                                      0x8100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionModify,       /* Inner tpid action modify*/
                                                                      bcmVlanActionAdd,              /* outer_action            */
                                                                      bcmVlanActionAdd,              /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionAdd,              /* inner_pri_action        */
                                                                      egress_outer_vid,              /* new_outer_vid           */
                                                                      egress_inner_vid,              /* new_inner_vid           */
                                                                      out_vlan_edit_prfoile,         /* vlan_edit_profile       */
                                                                      untag_format,                  /* tag_format              */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    /*
     *3. create in & out RIF 
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, in_vlan, my_mac_address), "intf_in");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, out_vlan, my_mac_address), "intf_out");
    /*Set Incoming ETH-RIF properties */
    l3_ingress_intf_init(&l3_ing_if);
    l3_ing_if.vrf = vrf;
    l3_ing_if.intf_id = in_vlan;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &l3_ing_if), "");

    /*
     * 4. Create ARP+AC
     */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = next_hop_mac;
    l3eg.vlan = out_vlan;
    l3eg.flags = 0;
    l3eg.vlan_port_id = egr_qos_gport;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null),
        "create egress object ARP only");
    encap_id = l3eg.encap_id;
    /*
     * 5. build FEC points to out-RIF and next-hop MAC address
     */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.encap_id = encap_id;
    l3eg.intf = out_vlan;
    l3eg.port = outPort;
    l3eg.flags = 0;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg, &encoded_fec),
        "create egress object ARP only");

    /*
     * 6. add host entry with FEC-ptr
     */
    if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        bcm_l3_route_t l3_route;
        bcm_l3_route_t_init(l3_route);

        l3_route.l3a_subnet = host;
        l3_route.l3a_ip_mask = 0xffffffff;
        l3_route.l3a_vrf = vrf;
        l3_route.l3a_intf = encoded_fec;       /* FEC-ID */

        if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            l3_route.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, &l3_route), "");
    }
    else
    {
        bcm_l3_host_t_init(&l3host);
        l3host.l3a_flags = 0;
        l3host.l3a_ip_addr = host;
        l3host.l3a_vrf = vrf;
        l3host.l3a_intf = encoded_fec;      /* FEC */
    
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, &l3host), "");
    }

    return BCM_E_NONE;
}

/*main function to test AC EVE QoS */
int l2_qos_ive_nop_eve_replace_main(int unit, int port1, int port2)
{
    char error_msg[200]={0,};
    int vlan1 = 10;
    int vlan2 = 20;
    int in_vlan_port_id;
    int out_vlan_port_id;
    bcm_vlan_port_t vlan_port;

    /*
     * 1. Create VLAN-Ports
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port1;
    vlan_port.match_vlan = vlan1;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "in");

    in_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan1, port1, 0), "");

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port2;
    vlan_port.match_vlan = vlan2;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "in");

    out_vlan_port_id = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan2, port2, 0), "");

    /*
     * 2. EVE action settings - Replace
     */
    /* AC 1 */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, in_vlan_port_id,               /* outlif                  */
                                                                      0x8100,                        /* outer_tpid              */
                                                                      0x9100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionNone,         /* Inner tpid action none */
                                                                      bcmVlanActionReplace,          /* outer_action            */
                                                                      bcmVlanActionNone,             /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionNone,             /* inner_pri_action        */
                                                                      100,                           /* new_outer_vid           */
                                                                      0,                             /* new_inner_vid           */
                                                                      3,                             /* vlan_edit_profile       */
                                                                      4,                             /* tag_format - here is stag */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    /* AC 2 */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, out_vlan_port_id,              /* outlif                  */
                                                                      0x8100,                        /* outer_tpid              */
                                                                      0x9100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionNone,         /* Inner tpid action none  */
                                                                      bcmVlanActionReplace,          /* outer_action            */
                                                                      bcmVlanActionNone,             /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionNone,             /* inner_pri_action        */
                                                                      200,                           /* new_outer_vid           */
                                                                      0,                             /* new_inner_vid           */
                                                                      3,                             /* vlan_edit_profile       */
                                                                      4,                             /* tag_format - here is stag */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    /*
     * 3. Add cross connect entry
     */
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = in_vlan_port_id;
    gports.port2 = out_vlan_port_id;
    snprintf(error_msg, sizeof(error_msg), "in cross connect port1(0x%08X) <--> port2(0x%08X)", gports.port1, gports.port2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), error_msg);

    /*
     * 4. Create ingress qos profile and bind to AC
     */
    BCM_IF_ERROR_RETURN_MSG(qos_map_l2_ingress_mapping_set(unit, BCM_QOS_MAP_L2_OUTER_TAG, in_vlan_port_id, 0), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, out_vlan_port_id, l2_in_map_id, -1), "");

    /*
     * 5. Create egress qos profile and bind to AC
     */
    BCM_IF_ERROR_RETURN_MSG(qos_l2_egress_map_create(unit, 0), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, in_vlan_port_id, -1, l2_eg_map_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, out_vlan_port_id, -1, l2_eg_map_id), "");

    return BCM_E_NONE;
}

