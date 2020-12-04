/* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_pon_application.c
* Purpose: A utility for setting up PON applications.
*
* Function Introduction:
*
* pon_app_init:
* To Initialize PON application configurations:
*  1. Add the following port configureations to config-sand.bcm
*        - ucode_port_129.BCM88650=10GBase-R15
*        - ucode_port_5.BCM88650=10GBase-R14
*  2. Add the following PON application enabling configurations to config-sand.bcm
*        - pon_application_support_enabled_5.BCM88650=TRUE
*        - vlan_match_criteria_mode=PON_PCP_ETHERTYPE
*  3. Add the following configuration to config-sand.bcm if AC isolation is needed
*        - bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)
*  4. Add the following configuration to config-sand.bcm if advanced VLAN mode is needed
*        - bcm886xx_vlan_translate_mode=1
*  5. Set PON port and NNI port to recognize VLAN tags.
*        - Normal VLAN mode: call port_tpid_init(), bcm_port_tpid_add(), bcm_port_inner_tpid_set().
*        - Advanced VLAN mode: bcm_port_tpid_add(), bcm_port_inner_tpid_set(), bcm_port_tpid_class_set();
*  6. Remove Ports from VLAN 1.
*        - call bcm_vlan_gport_delete_all()
*  7. Disable membership in PON ports (SDK initialization already disabled membership in NNI ports).
*        - call bcm_port_vlan_member_set()
*  8. Enable additional port tunnel lookup in PON ports.
*        - call bcm_vlan_control_port_set()
*  9. Create default pon l2 ingress mapping profile.
*        - call bcm_qos_map_create() with BCM_QOS_MAP_INGRESS|BCM_QOS_MAP_L2_VLAN_PCP.
*        - call bcm_qos_map_add() with BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG|BCM_QOS_MAP_L2_VLAN_PCP.
*  a. Create a PMF rule to restore the tunnel_id tag of packets sent to CPU.
*  b. Create a NNI trunk group in which the load balance is DA+SA hash.
*        - call bcm_switch_control_port_set() with bcmSwitchTrunkHashPktHeaderSelect to set the header type
*               in the hash compute.
*        - call bcm_switch_control_port_set() with bcmSwitchTrunkHashPktHeaderCount to set the header count
*               in the hash compute.
*        - call bcm_switch_control_set() with bcmSwitchHashL2Field0 to set the DA+SA hash. 
*        - call bcm_trunk_set() to set the trunk membership.
*
* pon_lif_create:
* Create NNI LIF according to the LIF type. PON LIF can be created based on Port or Flow.
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED, 
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE,
*          BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE
*
* nni_lif_create:
* Create NNI LIF according to the LIF type. NNI LIF can be created on a trunk group by port_nni = trunk_gport
*        - Call bcm_vlan_port_create()with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*
* pon_port_ingress_vt_set:
* Set up the ingress VLAN editor of PON LIF according to service type. Translate svlan or cvlan.
*        - Normal VLAN mode: Call bcm_vlan_translate_action_create()
*        - Advanced VLAN mode:  Call bcm_vlan_translate_action_id_create(), bcm_vlan_translate_action_id_set(), 
*                                                 bcm_vlan_port_translation_set(), bcm_vlan_translate_action_class_set with BCM_VLAN_ACTION_SET_INGRESS.
*
* pon_port_egress_vt_set:
* Set up the egress VLAN editor of PON LIF according to service type. Translate svlan or cvlan.
*        - Normal VLAN mode: Call bcm_vlan_translate_egress_action_add()
*        - Advanced VLAN mode:  Call bcm_vlan_translate_action_id_create(), bcm_vlan_translate_action_id_set(), 
*                                                 bcm_vlan_port_translation_set(), bcm_vlan_translate_action_class_set with BCM_VLAN_ACTION_SET_EGRESS.
 */

enum pon_service_mode_e {
    /* unmatched */
    unmatch_to_untag,		/*TUNNEL_ID <-> UNTAG*/
    unmatch_to_s_tag,		/*TUNNEL_ID <-> CVLAN*/
    unmatch_to_c_tag,		/*TUNNEL_ID <-> SVLAN*/
    unmatch_to_s_c_tag,		/*TUNNEL_ID <-> SVLAN + CVLAN*/
    unmatch_to_c_c_tag,		/*TUNNEL_ID <-> CVLAN + CVLAN*/
    unmatch_to_s_s_tag,		/*TUNNEL_ID <-> SVLAN + SVLAN*/
    
    /* untagged */
    untag_to_untag,			/*TUNNEL_ID <-> UNTAG*/
    untag_to_s_tag,			/*TUNNEL_ID <-> SVLAN*/
    untag_to_c_tag,			/*TUNNEL_ID <-> CVLAN*/
    untag_to_s_c_tag,		/*TUNNEL_ID <-> SVLAN + CVLAN*/
    untag_to_c_c_tag,		/*TUNNEL_ID <-> CVLAN + CVLAN*/
    untag_to_s_s_tag,		/*TUNNEL_ID <-> SVLAN + SVLAN*/	
    
    /* SVLAN */
    s_tag_to_untag,			/*TUNNEL_ID + SVLAN <-> UNTAG*/
    s_tag_to_s_tag,			/*TUNNEL_ID + SVLAN <-> SVLAN*/
    s_tag_to_s2_tag,		/*TUNNEL_ID + SVLAN <-> SVLAN2*/
    s_tag_to_c2_tag,		/*TUNNEL_ID + SVLAN <-> CVLAN2*/
    s_tag_to_s2_c2_tag,		/*TUNNEL_ID + SVLAN <-> SVLAN2 + CVLAN2*/
    s_tag_to_c2_c2_tag,		/*TUNNEL_ID + SVLAN <-> CVLAN2 + CVLAN2*/
    s_tag_to_s2_s2_tag,		/*TUNNEL_ID + SVLAN <-> SVLAN2 + SVLAN2*/
    
    /* CVLAN */
    c_tag_to_untag,			/*TUNNEL_ID + SVLAN <-> UNTAG*/
    c_tag_to_c_tag,			/*TUNNEL_ID + SVLAN <-> CVLAN*/
    c_tag_to_c2_tag,		/*TUNNEL_ID + SVLAN <-> CVLAN2*/
    c_tag_to_s2_tag,		/*TUNNEL_ID + SVLAN <-> SVLAN2*/	
    c_tag_to_s2_c2_tag,		/*TUNNEL_ID + SVLAN <-> SVLAN2 + CVLAN2*/
    c_tag_to_c2_c2_tag,		/*TUNNEL_ID + SVLAN <-> CVLAN2 + CVLAN2*/
    c_tag_to_s2_s2_tag,		/*TUNNEL_ID + SVLAN <-> SVLAN2 + SVLAN2*/
    
    /* SVLAN +CVLAN */
    s_c_tag_to_untag,		/*TUNNEL_ID + SVLAN + CVLAN <-> UNTAG*/
    s_c_tag_to_s_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN*/
    s_c_tag_to_s2_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN2*/
    s_c_tag_to_c_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> CVLAN*/
    s_c_tag_to_c2_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> CVLAN2*/
    s_c_tag_to_s_c_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN + CVLAN*/
    s_c_tag_to_s2_c_tag,	/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN2 + CVLAN*/
    s_c_tag_to_s_c2_tag,	/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN + CVLAN2*/
    s_c_tag_to_s2_c2_tag,	/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN2 + CVLAN2*/
    s_c_tag_to_c_c_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> CVLAN + CVLAN*/
    s_c_tag_to_c2_c2_tag,	/*TUNNEL_ID + SVLAN + CVLAN  <-> CVLAN2 + CVLAN2*/
    s_c_tag_to_s_s_tag,		/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN + SVLAN*/
    s_c_tag_to_s2_s2_tag,	/*TUNNEL_ID + SVLAN + CVLAN  <-> SVLAN2 + SVLAN2*/
    
    /* CVLAN +CVLAN */
    c_c_tag_to_untag,		/*TUNNEL_ID + CVLAN + CVLAN <-> UNTAG*/
    c_c_tag_to_c_tag,		/*TUNNEL_ID + CVLAN + CVLAN  <-> CVLAN*/
    c_c_tag_to_c2_tag,		/*TUNNEL_ID + CVLAN + CVLAN  <-> CVLAN2*/
    c_c_tag_to_s2_tag,		/*TUNNEL_ID + CVLAN + CVLAN  <-> SVLAN2*/	
    c_c_tag_to_c2_c_tag,	/*TUNNEL_ID + CVLAN + CVLAN  <-> CVLAN2 + CVLAN*/
    c_c_tag_to_c_c2_tag,	/*TUNNEL_ID + CVLAN + CVLAN  <-> CVLAN + CVLAN2*/
    c_c_tag_to_c2_c2_tag,	/*TUNNEL_ID + CVLAN + CVLAN  <-> CVLAN2 + CVLAN2*/
    c_c_tag_to_s_c_tag,		/*TUNNEL_ID + CVLAN + CVLAN  <-> SVLAN + CVLAN*/
    c_c_tag_to_s2_c2_tag,	/*TUNNEL_ID + CVLAN + CVLAN  <-> SVLAN2 + CVLAN2*/
    c_c_tag_to_c_c_tag,		/*TUNNEL_ID + CVLAN + CVLAN  <-> CVLAN + CVLAN*/	
    c_c_tag_to_s_s_tag,		/*TUNNEL_ID + CVLAN + CVLAN  <-> SVLAN + SVLAN*/
    c_c_tag_to_s2_s2_tag,	/*TUNNEL_ID + CVLAN + CVLAN  <-> SVLAN2 + SVLAN2*/
    
    /* SVLAN +SVLAN */
    s_s_tag_to_untag,		/*TUNNEL_ID + SVLAN + SVLAN <-> UNTAG*/
    s_s_tag_to_s_tag,		/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN*/
    s_s_tag_to_s2_tag,		/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN2*/
    s_s_tag_to_c2_tag,		/*TUNNEL_ID + SVLAN + SVLAN  <-> CVLAN2*/	
    s_s_tag_to_s_s_tag,		/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN + SVLAN*/
    s_s_tag_to_s2_s_tag,	/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN2 + SVLAN*/
    s_s_tag_to_s_s2_tag,	/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN + SVLAN2*/
    s_s_tag_to_s2_s2_tag,	/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN2 + SVLAN2*/
    s_s_tag_to_s_c_tag,		/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN + CVLAN*/
    s_s_tag_to_s2_c2_tag,	/*TUNNEL_ID + SVLAN + SVLAN  <-> SVLAN2 + CVLAN2*/
    s_s_tag_to_c_c_tag,		/*TUNNEL_ID + SVLAN + SVLAN  <-> CVLAN + CVLAN*/
    s_s_tag_to_c2_c2_tag	/*TUNNEL_ID + SVLAN + SVLAN  <-> CVLAN2 + CVLAN2*/
};

enum lif_type_e {
    match_unmatch,
    match_untag,    
    match_s_tag,
    match_c_tag,
    match_s_c_tag,
    match_c_c_tag,
    match_s_s_tag
};

/*
+--------------------------------------------+
| Frame tag format     | tag_format_class_id |
+--------------------------------------------+
|  no VLAN TAGs        |          0          |
+--------------------------------------------+
|  C-VLAN TAG          |          1          |
+--------------------------------------------+
|  S-VLAN TAG          |          2          |
+--------------------------------------------+
|  Priority TAG        |          3          |
+--------------------------------------------+
|  I-TAG               |          4          |
+--------------------------------------------+
|  C-C-VLAN TAGs       |          5          |
+--------------------------------------------+
|  S-C-VLAN TAGs       |          6          |
+--------------------------------------------+
| Priority-C-VLAN TAGs |          7          |
+--------------------------------------------+
|  C-S-VLAN TAGs       |          9          |
+--------------------------------------------+
|  S-S-VLAN TAGs       |          10         |
+--------------------------------------------+
| Priority-S-VLAN TAGs |          11         |
+--------------------------------------------+
| Any TAGs             |     0xFFFFFFFF      |
+--------------------------------------------+
*/


/* TAG_STRUCT_EGRESS_SW_ID_TO_HW_ID or TAG_STRUCT_INGRESS_SW_ID_TO_HW_ID for detail */
enum tag_struct_e {
    no_tag = 0x0,
    s_tag = 0x4,
    c_tag = 0x8,
    s_c_tag = 0x10,
    c_c_tag = 0x2,
    s_s_tag = 0x1C,
    sp_c_tag = 0x18,
    any_tag = 0xFFFFFFFF
};

enum ing_vlan_action_id_e {
    ing_nop = 0,
    ing_add_s_tag, /* U <> S */
    ing_add_c_tag, /* U <> S */
    ing_add_s_c_tag, /* U <> S + C */
    ing_add_c_c_tag, /* U <> C + C */
    ing_add_s_s_tag, /* U <> S + S */		
    
    ing_del_s_tag, /* S <> U, S + C <> S */
    ing_del_c_tag, /* C <> U, S + C <> S */
    ing_del_s_c_tag, /* S <> U, S + C <> S */
    ing_del_c_c_tag, /* S <> U, S + C <> S */
    ing_del_s_s_tag, /* S <> U, S + C <> S */
    
    ing_rep_s_tag, /* S <> S', C <> S' */
    ing_rep_c_tag, /* S <> C', C <> C' */
    ing_rep_s_c_tag, /* S + C <> S' + C, S + C <> S + C', S + C <> S' + C', C + C <> S' + C', S + S <> S' + C' */
    ing_rep_c_c_tag, /* C + C <> C' + C, C + C <> C + C', C + C <> C' + C', S + C <> C' + C', S + S <> C' + C' */
    ing_rep_s_s_tag, /* S + S <> S' + S, S + S <> S + S', S + S <> S' + S', S + C <> S' + S', C + C <> S' + S' */
    
    ing_rep_c_add_c_tag, /* S <> C' + C', C<> C' + C' */
    ing_rep_s_add_c_tag, /* S <> S' + C', C<> S' + C' */
    ing_rep_s_add_s_tag, /* S <> S' + S', C<> S' + S' */
    
    ing_rep_s_del_c_tag, /* S + C <> S', C + C <> S'*/
    ing_rep_c_del_c_tag, /* S + C <> C', C + C <> C'*/
    ing_rep_s_del_s_tag, /* S + S <> S'*/
    ing_rep_c_del_s_tag	/* S + S <> C'*/
};

enum egr_vlan_action_id_e {
    egr_add_vsi = 0,
    egr_nop,
    egr_del_s_tag, /* U <> S */
    egr_del_c_tag, /* U <> S */
    egr_del_s_c_tag, /* U <> S + C */
    egr_del_c_c_tag, /* U <> C + C */
    egr_del_s_s_tag, /* U <> S + S */		
    
    egr_add_s_tag, /* S <> U, S + C <> S */
    egr_add_c_tag, /* C <> U, S + C <> S */
    egr_add_s_c_tag, /* S <> U, S + C <> S */
    egr_add_c_c_tag, /* S <> U, S + C <> S */
    egr_add_s_s_tag, /* S <> U, S + C <> S */
    
    egr_rep_s_tag, /* S <> S', C <> S' */
    egr_rep_c_tag, /* S <> C', C <> C' */
    egr_rep_s_c_tag, /* S + C <> S' + C, S + C <> S + C', S + C <> S' + C', C + C <> S' + C', S + S <> S' + C' */
    egr_rep_c_c_tag, /* C + C <> C' + C, C + C <> C + C', C + C <> C' + C', S + C <> C' + C', S + S <> C' + C' */
    egr_rep_s_s_tag, /* S + S <> S' + S, S + S <> S + S', S + S <> S' + S', S + C <> S' + S', C + C <> S' + S' */
    
    egr_rep_c_del_c_tag, /* S <> C' + C', C<> C' + C' */
    egr_rep_c_del_s_tag, /* S <> C' + C', C<> C' + C' */
    egr_rep_s_del_c_tag, /* S <> S' + C', C<> S' + C' */
    egr_rep_s_del_s_tag, /* S <> S' + S', C<> S' + S' */
    
    egr_rep_s_add_c_tag, /* S + C <> S', C + C <> S'*/
    egr_rep_c_add_c_tag, /* S + C <> C', C + C <> C'*/
    egr_rep_s_add_s_tag, /* S + S <> S'*/
    egr_rep_c_add_s_tag	/* S + S <> C'*/
};
int util_verbose = 0;
int s_tpid = 0x9100; 
int c_tpid = 0x8100; 
int lif_offset = 8*1024; /*Offset for downstream flooding multicast group(MC = VSI + lif_offset)*/

/*
 * Create PON LIF according to the LIF type.
 * PON LIF can be created based on Port or Flow.
 */
int pon_lif_create(int unit, bcm_port_t port_pon, int lif_type, 
    bcm_tunnel_id_t tunnel_id_in, bcm_tunnel_id_t tunnel_id_out, 
    bcm_vlan_t up_ovlan, bcm_vlan_t up_ivlan, 
    int up_pcp, bcm_port_ethertype_t up_ethertype, bcm_gport_t *gport)
{
    int rv;
    int index;
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);
	vlan_port.port = port_pon;        
    if (*gport!=0) {
        vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id,*gport); 
    }        
    vlan_port.match_tunnel_value  = tunnel_id_in;
    vlan_port.egress_tunnel_value = tunnel_id_out;

    switch(lif_type) {
		case match_unmatch:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
            break;
        case match_untag:            
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED;
            break;            
        case match_s_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
            vlan_port.match_vlan = up_ovlan;
            break;
        case match_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN;
            vlan_port.match_vlan = up_ovlan;
            break;
        case match_s_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
            vlan_port.match_vlan = up_ovlan;
            vlan_port.match_inner_vlan = up_ivlan;
            break;
        case match_c_c_tag: 
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED;
            vlan_port.match_vlan = up_ovlan;
            vlan_port.match_inner_vlan = up_ivlan;
            break;
        case match_s_s_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED;
            vlan_port.match_vlan = up_ovlan;
            vlan_port.match_inner_vlan = up_ivlan;
            break;            

        default:
            printf("ERR: pon_lif_create INVALID PARAMETER lif_type %d\n", lif_type);
            return BCM_E_PARAM;
    }        
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("pon_lif_create index failed! %s\n", bcm_errmsg(rv));
    }    
    *gport = vlan_port.vlan_port_id;

	if(util_verbose) {
        printf("pon_lif created:\n");
        print vlan_port;
    }       
  
    return rv;
}


/*
 * Create NNI LIF according to the LIF type.
 * nni LIF can be created on a trunk group by port_nni = trunk_gport
 */
int nni_lif_create(int unit, bcm_port_t port_nni, int lif_type, int is_with_id, 
    bcm_vlan_t down_ovlan, bcm_vlan_t down_ivlan, bcm_vlan_t down_pcp, bcm_gport_t *gport, bcm_if_t *encap_id)
{
    int rv;
    int index;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_t vid;

    index = lif_type;
    
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = port_nni;
    if (is_with_id) /* Create NNI LIF with same LIF ID */
    {
        vlan_port.flags = BCM_VLAN_PORT_ENCAP_WITH_ID | BCM_VLAN_PORT_WITH_ID | 0x800 /* BCM_VLAN_PORT_MATCH_ONLY */;
        vlan_port.encap_id = *encap_id;
        vlan_port.vlan_port_id = *gport;
    } else {
        /* Set ingress vlan editor of NNI LIF to do nothing, when creating NNi LIF */
        if (*gport!=0) {
            vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id,*gport); 
        }
    }    

    switch(lif_type) {
        case match_untag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
            break;
        case match_s_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan  = down_ovlan;
            break;
        case match_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
            vlan_port.match_vlan  = down_ovlan;
            break;
        case match_s_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.match_vlan  = down_ovlan;
            vlan_port.match_inner_vlan  = down_ivlan;
            break;
        case match_c_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED;
            vlan_port.match_vlan  = down_ovlan;
            vlan_port.match_inner_vlan  = down_ivlan;
            break;
        case match_s_s_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED;
            vlan_port.match_vlan  = down_ovlan;
            vlan_port.match_inner_vlan  = down_ivlan;
            break;
        default:
            printf("ERR: nni_lif_create INVALID PARAMETER lif_type %d\n", lif_type);
            return BCM_E_PARAM;
    }

    if(util_verbose) {
        printf("nni_lif created:\n");
        print vlan_port;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("nni_lif_create index failed! %s\n", bcm_errmsg(rv));
        return rv;
    }

    *gport = vlan_port.vlan_port_id;
    if(util_verbose) {
        print vlan_port.vlan_port_id;
    }

    return rv;
}

/*
 * multicast utilities.
 */
int open_ingress_mc_group(int unit, int mc_group_id){
    int rv = BCM_E_NONE;
    int flags;
    bcm_multicast_t mc_group = mc_group_id;
    
    /* destroy before open, to ensure it not exist */  
    rv = bcm_multicast_destroy(unit, mc_group);
    if ((rv != BCM_E_NONE) && (rv!= BCM_E_NOT_FOUND)) {
        printf("Error, in bcm_multicast_destroy\n");
        return rv;
    }
 
    /* create ingress MC */
    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc create, flags $flags mc_group $mc_group \n");
        return rv;
    }
    return rv;
}

/*
 * Add port to multicast group.
 */
int multicast_vlan_port_add(int unit, int mc_group_id, int sys_port, int gport){
    int rv;
    int encap_id;    
    bcm_multicast_replication_t replications;
    
    rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }    

    bcm_multicast_replication_t_init(&replications);
    replications.encap1 = encap_id;
    replications.port = sys_port;
    
    rv = bcm_multicast_add(unit, mc_group_id, BCM_MULTICAST_INGRESS_GROUP, 1, &replications);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, replications.port, replications.encap1);
      return rv;
    }
    
    return rv;
}
/*
 * Create VSI and flooding multicast groups.
 */
int vswitch_create(int unit, bcm_vlan_t  *new_vsi){
    int rv;
    bcm_vlan_t vsi;
    bcm_vlan_control_vlan_t vsi_control;
   
    /* 1. create vswitch */
    rv = bcm_vswitch_create(unit,new_vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }
    vsi = *new_vsi;
    if(util_verbose){
        printf("created vswitch   0x%08x\n\r",vsi);
    }
     
    /* 2. create multicast for PON-->NNI: 
     */
    rv = open_ingress_mc_group(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, open_ingress_mc_group\n");
        return rv;
    }
    if(util_verbose){
        printf("created multicast   0x%08x\n\r",vsi);
    }
    
    /* 3. create multicast for NNI-->PON: 
     */
    if (lif_offset != 0) {
        rv = open_ingress_mc_group(unit, vsi+lif_offset);
        if (rv != BCM_E_NONE) {
            printf("Error, open_ingress_mc_group\n");
            return rv;
        }
        if(util_verbose){
            printf("created multicast   0x%08x\n\r",vsi+lif_offset);
        }
    }

    return BCM_E_NONE;
}

/*
 * Set up the ingress VLAN editor of PON LIF according to service type.
 * Translate svlan or cvlan.
 */
int pon_port_ingress_vt_set(int unit, int service_type, bcm_vlan_t down_ovlan, bcm_vlan_t down_ivlan, 
                            bcm_gport_t pon_gport, int qos_map_id, int* action_id)
{
    int rv;
    int ing_tag_format;
    int ing_vlan_action_id;
    int ing_vlan_edit_class_id;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_2;
    bcm_vlan_action_set_t_init(&action);
    
    switch (service_type)
    {
    /* UNTAG */
    case untag_to_untag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        ing_tag_format = no_tag;
        ing_vlan_action_id = ing_nop;
        ing_vlan_edit_class_id = 1;
        break;
    
    case untag_to_s_tag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = no_tag;
        ing_vlan_action_id = ing_add_s_tag;
        ing_vlan_edit_class_id = 2;
        break;
    
    case untag_to_c_tag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = no_tag;
        ing_vlan_action_id = ing_add_c_tag;
        ing_vlan_edit_class_id = 3;
        break;
    
    case untag_to_s_c_tag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = no_tag;
        ing_vlan_action_id = ing_add_s_c_tag;
        ing_vlan_edit_class_id = 4;
        break;
    
    case untag_to_c_c_tag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = no_tag;
        ing_vlan_action_id = ing_add_c_c_tag;
        ing_vlan_edit_class_id = 5;
        break;
    
    case untag_to_s_s_tag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = no_tag;
        ing_vlan_action_id = ing_add_s_s_tag;
        ing_vlan_edit_class_id = 6;
        break;
        
    /* SVLAN */
    case s_tag_to_untag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_del_s_tag;
        ing_vlan_edit_class_id = 1;
        break;
        
    case s_tag_to_s_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_nop;
        ing_vlan_edit_class_id = 2;
        break;
        
    case s_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_rep_s_tag;
        ing_vlan_edit_class_id = 3;
        break;
        
    case s_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_rep_c_tag;
        ing_vlan_edit_class_id = 4;
        break;
        
    case s_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_rep_s_add_c_tag;
        ing_vlan_edit_class_id = 5;
        break;
        
    case s_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_rep_c_add_c_tag;
        ing_vlan_edit_class_id = 6;
        break;
        
    case s_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = s_tag;
        ing_vlan_action_id = ing_rep_s_add_s_tag;
        ing_vlan_edit_class_id = 7;
        break;
        
    /* CVLAN */
    case c_tag_to_untag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_del_c_tag;
        ing_vlan_edit_class_id = 1;
        break;
        
    case c_tag_to_c_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_nop;
        ing_vlan_edit_class_id = 2;
        break;
        
    case c_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_rep_c_tag;
        ing_vlan_edit_class_id = 3;
        break;
        
    case c_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_rep_s_tag;
        ing_vlan_edit_class_id = 4;
        break;
        
    case c_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_rep_s_add_c_tag;
        ing_vlan_edit_class_id = 5;
        break;
        
    case c_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_rep_c_add_c_tag;
        ing_vlan_edit_class_id = 6;
        break;
        
    case c_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = c_tag;
        ing_vlan_action_id = ing_rep_s_add_s_tag;
        ing_vlan_edit_class_id = 7;
        break;
        
    /* SVLAN + CVLAN */
    case s_c_tag_to_untag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_del_s_c_tag;
        ing_vlan_edit_class_id = 1;
        break;
        
    case s_c_tag_to_s_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_s_del_c_tag;
        ing_vlan_edit_class_id = 2;
        break;
        
    case s_c_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_s_del_c_tag;
        ing_vlan_edit_class_id = 3;
        break;
        
    case s_c_tag_to_c_tag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_del_s_tag;
        ing_vlan_edit_class_id = 4;
        break;
        
    case s_c_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_c_del_c_tag;
        ing_vlan_edit_class_id = 5;
        break;
        
    case s_c_tag_to_s_c_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_nop;
        ing_vlan_edit_class_id = 6;
        break;
        
    case s_c_tag_to_s2_c_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_s_c_tag;
        ing_vlan_edit_class_id = 7;
        break;
        
    case s_c_tag_to_s_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_s_c_tag;
        ing_vlan_edit_class_id = 8;
        break;
        
    case s_c_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_s_c_tag;
        ing_vlan_edit_class_id = 9;
        break;
        
    case s_c_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_c_c_tag;
        ing_vlan_edit_class_id = 10;
        break;
        
    case s_c_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = s_c_tag;
        ing_vlan_action_id = ing_rep_s_s_tag;
        ing_vlan_edit_class_id = 11;
        break;
        
    /* CVLAN + CVLAN */
    case c_c_tag_to_untag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_del_c_c_tag;
        ing_vlan_edit_class_id = 1;
        break;
        
    case c_c_tag_to_c_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_c_del_c_tag;
        ing_vlan_edit_class_id = 2;
        break;
        
    case c_c_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_c_del_c_tag;
        ing_vlan_edit_class_id = 3;
        break;
        
    case c_c_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_s_del_c_tag;
        ing_vlan_edit_class_id = 4;
        break;
        
    case c_c_tag_to_c_c_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_nop;
        ing_vlan_edit_class_id = 5;
        break;
        
    case c_c_tag_to_c2_c_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_c_c_tag;
        ing_vlan_edit_class_id = 6;
        break;
        
    case c_c_tag_to_c_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_c_c_tag;
        ing_vlan_edit_class_id = 7;
        break;
        
    case c_c_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_c_c_tag;
        ing_vlan_edit_class_id = 8;
        break;
        
    case c_c_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_s_c_tag;
        ing_vlan_edit_class_id = 9;
        break;
        
    case c_c_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = c_c_tag;
        ing_vlan_action_id = ing_rep_s_s_tag;
        ing_vlan_edit_class_id = 10;
        break;
        
    /* SVLAN + SVLAN */
    case s_s_tag_to_untag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_del_s_s_tag;
        ing_vlan_edit_class_id = 1;
        break;
        
    case s_s_tag_to_s_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_s_del_s_tag;
        ing_vlan_edit_class_id = 2;
        break;
        
    case s_s_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_s_del_s_tag;
        ing_vlan_edit_class_id = 3;
        break;
        
    case s_s_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_c_del_s_tag;
        ing_vlan_edit_class_id = 4;
        break;
        
    case s_s_tag_to_s_s_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_nop;
        ing_vlan_edit_class_id = 5;
        break;
        
    case s_s_tag_to_s2_s_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_s_s_tag;
        ing_vlan_edit_class_id = 6;
        break;
        
    case s_s_tag_to_s_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_s_s_tag;
        ing_vlan_edit_class_id = 7;
        break;
        
    case s_s_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_s_s_tag;
        ing_vlan_edit_class_id = 8;
        break;
        
    case s_s_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_s_c_tag;
        ing_vlan_edit_class_id = 9;
        break;
        
    case s_s_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        ing_tag_format = s_s_tag;
        ing_vlan_action_id = ing_rep_c_c_tag;
        ing_vlan_edit_class_id = 10;
        break;
        
    default:
        return BCM_E_PARAM;
    }
    
    /* print ing_tag_format; */
    /* print ing_vlan_action_id; */
    /* print ing_vlan_edit_class_id; */
    
    rv = bcm_vlan_translate_action_id_get(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action_2);
    if (rv == BCM_E_NOT_FOUND) {
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ing_vlan_action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
            return rv;
        }
        
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
            return rv;
        }
    }
    
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = down_ovlan;
    port_trans.new_inner_vlan = down_ivlan;
    port_trans.gport = pon_gport;
    port_trans.vlan_edit_class_id = ing_vlan_edit_class_id;
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set ing $rv\n");
        return rv;
    }
    
    if (NULL != action_id) {
        *action_id = ing_vlan_action_id;
    }
    
    if (ing_tag_format == any_tag) {
        /* Set translation action class for single tagged packets */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
        action_class.tag_format_class_id = no_tag;
        action_class.vlan_translation_action_id = ing_vlan_action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }
        
        action_class.tag_format_class_id = s_tag;
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }
        
        action_class.tag_format_class_id = c_tag;
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }
        
        action_class.tag_format_class_id = s_c_tag;
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }
        
        action_class.tag_format_class_id = c_c_tag;
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }
    } else {
    /* Set translation action class for single tagged packets */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
        action_class.tag_format_class_id = ing_tag_format;
        action_class.vlan_translation_action_id = ing_vlan_action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
            return rv;
        }
    }
    
    return rv;
}

/*
 * Set up the egress VLAN editor of PON LIF according to service type.
 * translate svlan or cvlan.
 */
int pon_port_egress_vt_set(int unit, int service_type, bcm_tunnel_id_t tunnel_id, bcm_vlan_t up_ovlan, bcm_vlan_t up_ivlan, bcm_gport_t pon_gport)
{
    int rv;
    int triple_tags_service=0;
    int egr_tag_format;
    int egr_vlan_action_id;
    int egr_vlan_edit_class_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t action_2;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t_init(&action);
    
    switch (service_type)
    {
    /* UNTAG */
    case untag_to_untag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        
        egr_tag_format = no_tag;
        egr_vlan_action_id = egr_nop;
        egr_vlan_edit_class_id = 1;
        break;

    case s_tag_to_untag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;

        egr_tag_format = no_tag;
        egr_vlan_action_id = egr_add_s_tag;
        egr_vlan_edit_class_id = 2;
        break;

    case c_tag_to_untag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;

        egr_tag_format = no_tag;
        egr_vlan_action_id = egr_add_c_tag;
        egr_vlan_edit_class_id = 3;
        break;

    
    case s_c_tag_to_untag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;     
        
        egr_tag_format = no_tag;
        egr_vlan_action_id = egr_add_s_c_tag;
        egr_vlan_edit_class_id = 4;     
        break;    

    case c_c_tag_to_untag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;
        
        egr_tag_format = no_tag;
        egr_vlan_action_id = egr_add_c_c_tag;
        egr_vlan_edit_class_id = 5;        
        break;    

    case s_s_tag_to_untag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;
        
        egr_tag_format = no_tag;
        egr_vlan_action_id = egr_add_s_s_tag;
        egr_vlan_edit_class_id = 6;        
        break;    

    /* SVLAN */
    case untag_to_s_tag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionNone;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_del_s_tag;
        egr_vlan_edit_class_id = 1;
        break;

    case s_tag_to_s_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_nop;
        egr_vlan_edit_class_id = 2;
        break;

    case s_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_s_tag;
        egr_vlan_edit_class_id = 3;
        break;

    case c_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_c_tag;
        egr_vlan_edit_class_id = 4;
        break;

    case s_c_tag_to_s_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;    

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_s_add_c_tag;
        egr_vlan_edit_class_id = 5;        
        break;    

    case s_c_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;    

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_s_add_c_tag;
        egr_vlan_edit_class_id = 6;        
        break;
        
    case c_c_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_c_add_c_tag;
        egr_vlan_edit_class_id = 7;        
        break;

    case s_s_tag_to_s_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_s_add_s_tag;
        egr_vlan_edit_class_id = 8;        
        break;    

    case s_s_tag_to_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_tag;
        egr_vlan_action_id = egr_rep_s_add_s_tag;
        egr_vlan_edit_class_id = 9;        
        break;

    /* CVLAN */
    case untag_to_c_tag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionNone;
        
        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_del_c_tag;
        egr_vlan_edit_class_id = 1;
        break;
    
    case s_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_rep_s_tag;
        egr_vlan_edit_class_id = 2;
        break;

    case c_tag_to_c_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_nop;
        egr_vlan_edit_class_id = 3;
        break;

    case c_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = c_tpid;

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_rep_c_tag;
        egr_vlan_edit_class_id = 4;
        break;
        
    case s_c_tag_to_c_tag:
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;    

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_add_s_tag;
        egr_vlan_edit_class_id = 5;        
        break;

    case s_c_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;    

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_rep_s_add_c_tag;
        egr_vlan_edit_class_id = 6;        
        break;
    
    case c_c_tag_to_c_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_rep_c_add_c_tag;
        egr_vlan_edit_class_id = 7;     
        break;    

    case c_c_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_rep_c_add_c_tag;
        egr_vlan_edit_class_id = 8;     
        break;    

    case s_s_tag_to_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = c_tag;
        egr_vlan_action_id = egr_rep_s_add_s_tag;
        egr_vlan_edit_class_id = 9;        
        break;

    /* SVLAN + CVLAN */
    case untag_to_s_c_tag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_del_s_c_tag;
        egr_vlan_edit_class_id = 1;
        break;

    case s_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_s_del_c_tag;
        egr_vlan_edit_class_id = 2;
        break;
    
    case c_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_c_del_c_tag;
        egr_vlan_edit_class_id = 3;
        break;

    case s_c_tag_to_s_c_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_nop;
        egr_vlan_edit_class_id = 4;        
        break;

    case s_c_tag_to_s2_c_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_s_c_tag;
        egr_vlan_edit_class_id = 5;        
        break;

    case s_c_tag_to_s_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_s_c_tag;
        egr_vlan_edit_class_id = 6;        
        break;

    case s_c_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_s_c_tag;
        egr_vlan_edit_class_id = 7;        
        break;

    case c_c_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_c_c_tag;
        egr_vlan_edit_class_id = 8;        
        break;

    case s_s_tag_to_s2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_c_tag;
        egr_vlan_action_id = egr_rep_s_s_tag;
        egr_vlan_edit_class_id = 9;        
        break;

    /* CVLAN + CVLAN */
    
    case untag_to_c_c_tag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_del_c_c_tag;
        egr_vlan_edit_class_id = 1;
        break;

    case s_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_s_del_c_tag;
        egr_vlan_edit_class_id = 2;
        break;

    case c_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_c_del_c_tag;
        egr_vlan_edit_class_id = 3;
        break;

    case s_c_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_s_c_tag;
        egr_vlan_edit_class_id = 4;        
        break;
    
    case c_c_tag_to_c_c_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_nop;
        egr_vlan_edit_class_id = 5;        
        break;

    case c_c_tag_to_c2_c_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_c_c_tag;
        egr_vlan_edit_class_id = 6;        
        break;

    case c_c_tag_to_c_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_c_c_tag;
        egr_vlan_edit_class_id = 7;        
        break;

    case c_c_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_c_c_tag;
        egr_vlan_edit_class_id = 8;        
        break;

    case s_s_tag_to_c2_c2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = c_c_tag;
        egr_vlan_action_id = egr_rep_s_s_tag;
        egr_vlan_edit_class_id = 9;        
        break;    

    /* SVLAN + SVLAN */    
    case untag_to_s_s_tag:
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_del_s_s_tag;
        egr_vlan_edit_class_id = 1;
        break;    

    case s_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = s_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_s_del_s_tag;
        egr_vlan_edit_class_id = 2;        
        break;    

    case c_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = c_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_c_del_s_tag;
        egr_vlan_edit_class_id = 3;        
        break;    

    case s_c_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_s_c_tag;
        egr_vlan_edit_class_id = 4;        
        break;    

    case c_c_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = c_tpid;
        action.inner_tpid = c_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_c_c_tag;
        egr_vlan_edit_class_id = 5;        
        break;

    case s_s_tag_to_s_s_tag:
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_nop;
        egr_vlan_edit_class_id = 6;        
        break;

    case s_s_tag_to_s2_s_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_s_s_tag;
        egr_vlan_edit_class_id = 7;        
        break;

    case s_s_tag_to_s_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_s_s_tag;
        egr_vlan_edit_class_id = 8;        
        break;

    case s_s_tag_to_s2_s2_tag:
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.outer_tpid = s_tpid;
        action.inner_tpid = s_tpid;

        egr_tag_format = s_s_tag;
        egr_vlan_action_id = egr_rep_s_s_tag;
        egr_vlan_edit_class_id = 9;        
        break;    
        
    default:
        return BCM_E_PARAM;
    }

    rv = bcm_vlan_translate_action_id_get(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action_2);
    if (rv == BCM_E_NOT_FOUND) {
        /* print egr_tag_format; */
        /* print egr_vlan_action_id; */
        /* print egr_vlan_edit_class_id; */

        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &egr_vlan_action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create egr $rv\n");
            return rv;
        }

        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set egr $rv\n");
            return rv;
        }
    }
    
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = up_ovlan;
    port_trans.new_inner_vlan = up_ivlan;
    port_trans.gport = pon_gport;
    port_trans.vlan_edit_class_id = egr_vlan_edit_class_id;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set egr $rv\n");
        return rv;
    }

    if (egr_tag_format == any_tag) {
        /* Set translation action class for single tagged packets */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = egr_vlan_edit_class_id;
        action_class.tag_format_class_id = no_tag;
        action_class.vlan_translation_action_id = egr_vlan_action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }

        action_class.tag_format_class_id = s_tag;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }

        action_class.tag_format_class_id = c_tag;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }

        action_class.tag_format_class_id = s_c_tag;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }

        action_class.tag_format_class_id = c_c_tag;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }

        action_class.tag_format_class_id = s_s_tag;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }
    } else {
        /* Set translation action class for single tagged packets */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = egr_vlan_edit_class_id;
        action_class.tag_format_class_id = egr_tag_format;
        action_class.vlan_translation_action_id = egr_vlan_action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set(unit,    &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
            return rv;
        }
    }

    return rv;
}

