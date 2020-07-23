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
    otag_to_untag,          /*TUNNEL_ID + CVLAN <-> UNTAG*/
    otag_to_otag,           /*TUNNEL_ID + CVLAN <-> CVLAN*/
    otag_to_otag2,          /*TUNNEL_ID + CVLAN <-> CVLAN'*/
    otag_to_o_i_tag,        /*TUNNEL_ID + CVLAN <-> SVLAN + CVLAN*/
    otag_to_o_i2_tag,       /*TUNNEL_ID + CVLAN <-> SVLAN + CVLAN'*/
    otag_to_o2_i2_tag,      /*TUNNEL_ID + CVLAN <-> SVLAN' + CVLAN'*/
    o_i_tag_to_o_i_tag,     /*TUNNEL_ID + SVLAN + CVLAN <-> SVLAN + CVLAN*/
    o_i_tag_to_o_i_tag_2,   /*TUNNEL_ID + CVLAN + CVLAN <-> SVLAN + CVLAN*/
    o_i_tag_to_o2_i_tag,    /*TUNNEL_ID + SVLAN + CVLAN <-> SVLAN' + CVLAN*/
    o_i_tag_to_o2_i_tag_2,  /*TUNNEL_ID + CVLAN + CVLAN <-> SVLAN' + CVLAN*/
    o_i_tag_to_o_i2_tag,    /*TUNNEL_ID + SVLAN + CVLAN <-> SVLAN + CVLAN'*/
    o_i_tag_to_o_i2_tag_2,  /*TUNNEL_ID + CVLAN + CVLAN <-> SVLAN + CVLAN'*/
    o_i_tag_to_o2_i2_tag,   /*TUNNEL_ID + SVLAN + CVLAN <-> SVLAN' + CVLAN'*/
    o_i_tag_to_o2_i2_tag_2, /*TUNNEL_ID + CVLAN + CVLAN <-> SVLAN' + CVLAN'*/
    untag_to_untag,         /*TUNNEL_ID <-> UNTAG*/
    untag_to_otag,          /*TUNNEL_ID <-> CVLAN*/
    untag_to_o_i_tag,       /*TUNNEL_ID <-> SVLAN + CVLAN*/
    transparent_all_tags,   /*TLS*/
    triple_tag_manipulation    /*3 tag manipulation, tunnel ID + SVLAN + CVLAN, and tunnel ID will added by system automatically*/
};

enum lif_type_e {
    match_untag,
    match_otag,
    match_o_i_tag,
    match_all_tags,  /*TLS*/
    match_otag_cos,
    match_o_i_tag_cos,
    match_otag_cos_ethertype,
    match_o_i_tag_cos_ethertype,
    match_ethertype,
    match_otag_ethertype,
    match_o_i_tag_ethertype,
    match_t_o_i_tag /* t - Tunnel ID s - SVLAN c - CVLAN, Only support at PON port */
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
uint16 no_tag = 0;
uint16 c_tag = 1;
uint16 s_tag = 2;
uint16 prio_tag = 3;
uint16 i_tag = 4;
uint16 c_c_tag = 5;
uint16 s_c_tag = 6;
uint16 prio_c_tag = 7;
uint16 c_s_tag = 9;
uint16 s_s_tag = 10;
uint16 prio_s_tag = 11;
int any_tag = 0xFFFFFFFF;

enum ing_vlan_action_id_e {
    ing_no_op_0=0,
    ing_no_op_1,
    ing_no_op_2,
    ing_no_op_3,
    ing_mod_tpid_0, /* C <> C */
    ing_mod_tpid_1, /* S+C <> S+C */
    ing_add_c_tag,
    ing_add_s_c_tag,
    ing_del_s_tag,
    ing_del_c_tag,
    ing_del_s_c_tag,
    ing_rep_s_tag,
    ing_rep_c_tag_0, /* C <> C' */
    ing_rep_c_tag_1, /* S+C <> S+C' */
    ing_rep_s_c_tag,
    ing_rep_c_tag_add_c_tag,
    ing_rep_c_tag_del_ctag
};

enum egr_vlan_action_id_e {
    egr_add_vsi = 0,
    egr_no_op_tag = 1,
    egr_no_op_0, /* C <> C*/
    egr_no_op_1, /* S+C <> S+C*/
    egr_add_tu_0, /* transparent */
    egr_add_tu_1, /* C<>C */
    egr_add_tu_2, /* S+C<>S+C */
    egr_add_tu_3, /* C+C<>S+C */
    egr_add_tu_c_tag,
    egr_rep_tu,
    egr_add_tu_rep_s_c_tag,   /*S+C<>S'+C'*/
    egr_add_tu_rep_s_c_tag_2, /*C+C<>S'+C'*/
    egr_rep_tu_c_tag,
    egr_rep_tu_add_c_tag,
    egr_rep_tu_add_s_tag,
    egr_rep_tu_add_s_tag_2,   /*C+C<>S'+C*/
    egr_rep_tu_del_c_tag
};


int util_verbose = 0;
bcm_vlan_port_t pon_vlan_port[10];
bcm_vlan_port_t nni_vlan_port;
int pon_port_index=0;
int local_route_flag = 0;
/* Map ingress and egress as follows
 * Ingress      INTERNAL
 * PKT_PRI/CFI  TC/DP

 *       0/0    0/GREEN
 *       0/1    0/YELLOW
 *       1/0    1/GREEN
 *       1/1    1/YELLOW
 *       2/0    2/GREEN
 *       2/1    2/YELLOW
 *       3/0    3/GREEN
 *       3/1    3/YELLOW
 *       4/0    4/GREEN
 *       4/1    4/YELLOW
 *       5/0    5/GREEN
 *       5/1    5/YELLOW
 *       6/0    6/GREEN
 *       6/1    6/YELLOW
 *       7/0    7/GREEN
 *       7/1    7/YELLOW
 */
int pon_in_map_id_dft;
int nni_trunk_id = 0;
bcm_field_group_t pon_tunnel_id_cap_grp = 2;
int pon_tunnel_id_cap_grp_pri = 61;
int is_advanced_vlan_translation_mode = 0;
int outer_tpid = 0x8100; /* 0x88a8 */
int inner_tpid = 0x9100; /* 0x8100 */
int lif_offset = 16*1024; /*Offset for downstream flooding multicast group(MC = VSI + lif_offset)*/
int fwd_group = 0; /* Whether use forward group */
int vlan_port_with_vsi = (-1);
/* 
 * Create default pon l2 ingress mapping profile.
 * When adding a new VLAN tag, cos of the new tag is based on the default mapping profile.
 */
int qos_map_l2_ing_dft_profile(int unit)
{
    bcm_qos_map_t l2_in_map;
    int flags = 0;
    int idx = 0;
    int rv = BCM_E_NONE;

    /* Clear structure */
    bcm_qos_map_t_init(&l2_in_map);
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pon_in_map_id_dft);
    if (rv != BCM_E_NONE) {
        printf("error in ingress PCP bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    for (idx=0; idx<16; idx++) {      
        flags = BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG|BCM_QOS_MAP_L2_VLAN_PCP;
        
        /* Set ingress pkt_pri/cfi Priority */
        l2_in_map.pkt_pri = idx>>1;
        l2_in_map.pkt_cfi = idx&1;
        
        /* Set internal priority for this ingress pri */
        l2_in_map.int_pri = idx>>1;
        
        /* Set color for this ingress Priority 0:bcmColorGreen 1:bcmColorYellow */
        l2_in_map.color = idx&1;

        rv = bcm_qos_map_add(unit, flags, &l2_in_map, pon_in_map_id_dft);    
        if (rv != BCM_E_NONE) {
            printf("error in PCP ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/* Create a PMF rule to restore the tunnel_id tag of packets sent to CPU */
int pon_pmf_init(int unit, int port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;

    /* Check if the group is already created. */
    rv = bcm_field_group_get(unit, pon_tunnel_id_cap_grp, &qset);
    if (rv != BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);
    rv = bcm_field_group_create_mode_id(unit, qset, pon_tunnel_id_cap_grp_pri, bcmFieldGroupModeAuto, pon_tunnel_id_cap_grp);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR:: bcm_field_group_create_mode_id returned %s\n", bcm_errmsg(rv));
        return rv;
    }   
    
    rv = bcm_field_group_action_set(unit, pon_tunnel_id_cap_grp, aset);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_action_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_field_entry_create(unit, pon_tunnel_id_cap_grp, &ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_field_qualify_InPort(unit, ent, port, 0xffffffff); 
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_qualify_InPort returned %s\n", bcm_errmsg(rv));
    }
    
    rv = bcm_field_action_add(unit, ent, bcmFieldActionStartPacketStrip, bcmFieldStartToL2Strip, 0x3C /*-4 Bytes in two s complement*/);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_action_add returned %s\n", bcm_errmsg(rv));
    }
    
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_install returned %s\n", bcm_errmsg(rv));
    }

    return rv;
}

/*Set header count and type in the hash computing*/
int pon_trunk_hashing_set(int unit, int in_port, int nof_headers, int forward) 
{  
    int rc;  
    int arg;  
    bcm_switch_control_t type;  

    /* select which starting header to use for LAG hashing */  
    type = bcmSwitchTrunkHashPktHeaderSelect;  
    if (forward) { 
    /* first header for LAG hashing will be the forwarding header */      
        arg = BCM_HASH_HEADER_FORWARD;   
    }  
    else { 
    /* first header will be the header below the forwarding header (the terminated header) */     
        arg = BCM_HASH_HEADER_TERMINATED;  
    }  
    rc = bcm_switch_control_port_set(unit, in_port, type, arg);  
    if (rc != BCM_E_NONE) {    
        printf ("bcm_switch_control_port_set with arg %d failed: %d \n", arg, rc);  
    }  
    /* select number of headers to consider in LAG hashing */  
    type = bcmSwitchTrunkHashPktHeaderCount;  
    arg = nof_headers;  
    rc = bcm_switch_control_port_set(unit, in_port, type, arg);  
    if (rc != BCM_E_NONE) {    
        printf ("bcm_switch_control_port_set with arg %d failed: %d \n", arg, rc);  
    }  
    return rc;
}

/*trunk create on NNI ports*/
void nni_trunk_create(int unit, bcm_port_t port_pon, bcm_trunk_t tid, int out_local_port_num, int *out_local_ports)
{    
    int rv = 0;    
    bcm_trunk_member_t member_array[20];    
    bcm_trunk_info_t trunk_info;
    int header_num = 2;
    int header_type = 1;
    int i;
    int type; 
    int arg;    

    sal_memset(member_array, 0, sizeof(member_array));    
    sal_memset(&trunk_info, 0, sizeof(trunk_info)); 
                
    pon_trunk_hashing_set(unit, port_pon, header_num, header_type);     
                    
    /* Creates #tid trunk (no memory allocation and no members inside) */    
    print bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid); 
          
    /* Prepare local ports to gports */    
    type = bcmSwitchHashL2Field0;    
    arg = BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI | \
          BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI;    
    print bcm_switch_control_set(unit, type, arg);
          
    for (i = 0; i < out_local_port_num; i++) {       
        BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, out_local_ports[i]);      
    }    
       
    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;      
             
    /*  Adds members (in member_array) to trunk and activates trunk mechanism */    
    print bcm_trunk_set(unit, tid, &trunk_info, out_local_port_num, member_array);
}

int pon_tpid_profile_init(int unit, bcm_port_t port)
{
    int rv = 0;
    int index;
    bcm_port_tpid_class_t port_tpid_class;

    if (is_advanced_vlan_translation_mode) {
         /* remove old tpids on port */
        rv = bcm_port_tpid_delete_all(unit,port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_delete_all $rv\n");
            return rv;
        }

        /* identify TPID on port */
        rv = bcm_port_tpid_add(unit, port, outer_tpid,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_add $rv\n");
            return rv;
        }
        
        /* identify TPID on port */
        rv = bcm_port_inner_tpid_set(unit, port, inner_tpid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_inner_tpid_set $rv\n");
            return rv;
        }
                
        /* outer: TPID1, inner: ANY ==> S_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = outer_tpid;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = s_tag;
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set s-tag $rv\n");
            return rv;
        }

        /* outer: TPID2, inner: ANY ==> c_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = inner_tpid;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = c_tag;
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set c-tag $rv\n");
            return rv;
        }

        /* outer: TPID1, inner: TPID2 ==> S_c_tag */
        /* when TPID found as outer, then consider as tag and accepted format */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = outer_tpid;
        port_tpid_class.tpid2 = inner_tpid;
        port_tpid_class.tag_format_class_id = s_c_tag;
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
            return rv;
        }

        /* outer: TPID2, inner: TPID2 ==> C_c_tag */
        /* when TPID found as outer, then consider as tag and accepted format */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = inner_tpid;
        port_tpid_class.tpid2 = inner_tpid;
        port_tpid_class.tag_format_class_id = c_c_tag;
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set c-c-tag $rv\n");
            return rv;
        }
    } else {
        /* set ports to identify double tags packets and treat packets with ctags to stag in Arad */
        port_tpid_init(port, 1, 1);
        port_tpid_info1.outer_tpids[0] = outer_tpid;
        port_tpid_info1.inner_tpids[0] = inner_tpid;
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set $rv\n");
            return rv;
        }

        /* set inner port tpids to recognize single ctag frames */
        for(index = 0; index < port_tpid_info1.nof_inners; ++index){
            rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_tpid_add, tpid=%d, $rv\n", port_tpid_info1.inner_tpids[index]);
                return rv;
            }
        }
    }

    return rv;
}

int pon_port_discard_set(int unit, bcm_port_t port, int flags)
{
    int rv = 0;
    bcm_port_tpid_class_t port_tpid_class;

    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.port = port;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_DISCARD;
    port_tpid_class.vlan_translation_action_id = 0;
    
    if ((flags == BCM_PORT_DISCARD_ALL) || (flags == BCM_PORT_DISCARD_UNTAG)) {
        port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
        port_tpid_class.tag_format_class_id = no_tag;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set no_tag $rv\n");
            return rv;
        }
    }

    if ((flags == BCM_PORT_DISCARD_ALL) || (flags == BCM_PORT_DISCARD_TAG)) {
        port_tpid_class.tpid1 = inner_tpid;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = c_tag;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set c-tag $rv\n");
            return rv;
        }

        port_tpid_class.tpid1 = outer_tpid;
        port_tpid_class.tpid2 = inner_tpid;
        port_tpid_class.tag_format_class_id = s_c_tag;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
            return rv;
        }
    }
    return rv;
}

int ing_vlan_action_only_modify_tpid_init(int unit)
{
    int rv = BCM_E_NONE;
    int ing_vlan_action_id = ing_mod_tpid_0;
    int ing_vlan_edit_class_id = 1;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);

    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ing_vlan_action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
        return rv;
    }

    action.outer_tpid = inner_tpid;
    action.inner_tpid = outer_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
        return rv;
    }

    /* Set translation action class for c_tag packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
    action_class.tag_format_class_id = c_tag;
    action_class.vlan_translation_action_id = ing_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }

    /* Set translation action class for c_c_tag packets */
    action_class.tag_format_class_id = c_c_tag;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }

    ing_vlan_action_id = ing_mod_tpid_1;
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ing_vlan_action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
        return rv;
    }

    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, ing_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set ing $rv\n");
        return rv;
    }

    /* Set translation action class for untag packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ing_vlan_edit_class_id;
    action_class.tag_format_class_id = no_tag;
    action_class.vlan_translation_action_id = ing_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }

    /* Set translation action class for s_tag packets */
    action_class.tag_format_class_id = s_tag;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }

    /* Set translation action class for s_c_tag packets */
    action_class.tag_format_class_id = s_c_tag;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set ing $rv\n");
        return rv;
    }

    return rv;
}

int egr_vlan_action_no_op_init(int unit)
{
    int rv = 0;
    int egr_vlan_action_id = egr_no_op_0;
    int egr_vlan_edit_class_id = 1;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    
    bcm_vlan_action_set_t_init(&action);

    
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &egr_vlan_action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create egr $rv\n");
        return rv;
    }

    action.outer_tpid = inner_tpid;
    action.inner_tpid = outer_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set egr $rv\n");
        return rv;
    }

    /* Set translation action class for c_tag packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = egr_vlan_edit_class_id;
    action_class.tag_format_class_id = c_tag;
    action_class.vlan_translation_action_id = egr_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }

    /* Set translation action class for c_c_tag packets */
    action_class.tag_format_class_id = c_c_tag;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }

    egr_vlan_action_id = egr_no_op_1;
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &egr_vlan_action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create egr $rv\n");
        return rv;
    }

    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, egr_vlan_action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set egr $rv\n");
        return rv;
    }

    /* Set translation action class for untag packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = egr_vlan_edit_class_id;
    action_class.tag_format_class_id = no_tag;
    action_class.vlan_translation_action_id = egr_vlan_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }
    
    /* Set translation action class for s_tag packets */
    action_class.tag_format_class_id = s_tag;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }

    /* Set translation action class for s_c_tag packets */
    action_class.tag_format_class_id = s_c_tag;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }

    return rv;
}

/*
 * Initialize PON application configurations.
 */
int pon_app_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni_1, bcm_port_t port_nni_2, int is_nni_trunk)
{
    int rv = 0;
    int index;
    int out_local_port[2];
    int out_local_port_num = 2;
    int device_is_qax = 0;

    is_qumran_ax(unit, &device_is_qax);
    /* For QAX device, MAX number of MC group is 16*1024 */
    if (device_is_qax == 1) {
        lif_offset = 8*1024;
    }
    is_advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    bcm_port_class_set(unit, port_pon, bcmPortClassId, port_pon);
    bcm_port_class_set(unit, port_nni_1, bcmPortClassId, port_nni_1);
    if (is_nni_trunk)
        /*nni_1 and nni_2 in a trunk group*/
        bcm_port_class_set(unit, port_nni_2, bcmPortClassId, port_nni_1);

    pon_tpid_profile_init(unit, port_pon);
    pon_tpid_profile_init(unit, port_nni_1);

    if (is_nni_trunk) {
        pon_tpid_profile_init(unit, port_nni_2);
    }

    if (is_advanced_vlan_translation_mode) {
        /* In case of port tpid_profile != 0, need to init a VLAN action 
         * to modify the tpid_profile of IVE to the correct vlaue 
         */
        if ((outer_tpid != 0x8100) ||( (outer_tpid == 0x8100) && (inner_tpid != 0x9100))) {
            ing_vlan_action_only_modify_tpid_init(unit);
        }
        egr_vlan_action_no_op_init(unit);
    }
    

    /* Remove Ports from VLAN 1 (Done by init application) */
    bcm_vlan_gport_delete_all(unit, 1);

    /* Disable membership in PON ports */
    rv = bcm_port_vlan_member_set(unit, port_pon, 0x0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set\n");
        print rv;
        return rv;
    }

    /* Enable additional port tunnel lookup in PON ports */
    rv = bcm_vlan_control_port_set(unit, port_pon, bcmVlanPortLookupTunnelEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_port_set %s\n", port_pon, bcm_errmsg(rv));
    }

    /* Enable tunnel id capture */
    pon_pmf_init(unit, port_pon);

    /*create default pon l2 ingress mapping profile*/
    qos_map_l2_ing_dft_profile(unit);

    if (is_nni_trunk) {
        out_local_port[0] = port_nni_1;     
        out_local_port[1] = port_nni_2;    
        print nni_trunk_create(unit, port_pon, nni_trunk_id, out_local_port_num, out_local_port);
    }

    return rv;
}

/*
 * Create PON LIF according to the LIF type.
 * PON LIF can be created based on Port or Flow.
 */
int pon_lif_create(int unit, bcm_port_t port_pon, int lif_type, int is_flow, int queue,
    bcm_tunnel_id_t tunnel_id_in, bcm_tunnel_id_t tunnel_id_out, bcm_vlan_t up_ovlan, bcm_vlan_t up_ivlan, 
    int up_pcp, bcm_port_ethertype_t up_ethertype, bcm_gport_t *gport)
    {
        int rv;
        int index;
        bcm_vlan_port_t vlan_port;
    
        bcm_vlan_port_t_init(&vlan_port);

        if (is_flow) {
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(vlan_port.port, queue);
        } else {
            vlan_port.port = port_pon;
        }        
        if (*gport!=0) {
            vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id,*gport); 
        }        
        /* Set ingress vlan editor of PON LIF to do nothing, when creating PON LIF */
        if (soc_property_get(unit , "custom_feature_l3_source_bind_arp_relay",0) || fwd_group) {
            vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_NETWORK | BCM_VLAN_PORT_FORWARD_GROUP);
        } else {
            vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_NETWORK);
        }
        vlan_port.match_tunnel_value  = tunnel_id_in;
        vlan_port.egress_tunnel_value = tunnel_id_out;
        if (vlan_port_with_vsi != (-1)) {
            vlan_port.vsi = vlan_port_with_vsi;
        }
    
        switch(lif_type) {
            case match_untag:            
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
                break;
            case match_otag:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
                vlan_port.match_vlan = up_ovlan;
                break;
            case match_o_i_tag:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
                vlan_port.match_vlan        = up_ovlan;
                vlan_port.match_inner_vlan  = up_ivlan;
                break;
            case match_otag_cos:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN;
                vlan_port.match_vlan  = up_ovlan;
                vlan_port.match_pcp   = up_pcp;
                break;
            case match_o_i_tag_cos:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED;
                vlan_port.match_vlan        = up_ovlan;
                vlan_port.match_inner_vlan  = up_ivlan;
                vlan_port.match_pcp         = up_pcp;
                break;
            case match_all_tags: /* transparent mode */
                vlan_port.flags |= BCM_VLAN_PORT_TRANSPARENT;
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
                break;
            case match_otag_cos_ethertype:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE;
                vlan_port.match_vlan      = up_ovlan;
                vlan_port.match_pcp       = up_pcp;
                vlan_port.match_ethertype = up_ethertype;
                break;
            case match_o_i_tag_cos_ethertype:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE;
                vlan_port.match_ethertype   = up_ethertype;
                vlan_port.match_vlan        = up_ovlan;
                vlan_port.match_inner_vlan  = up_ivlan;
                vlan_port.match_pcp         = up_pcp;
                break;
            case match_ethertype:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE;
                vlan_port.match_ethertype = up_ethertype;
                break;
            case match_otag_ethertype:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_ETHERTYPE;
                vlan_port.match_ethertype   = up_ethertype;
                vlan_port.match_vlan        = up_ovlan;
                break;
            case match_o_i_tag_ethertype:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED_ETHERTYPE;
                vlan_port.match_ethertype   = up_ethertype;
                vlan_port.match_vlan        = up_ovlan;
                vlan_port.match_inner_vlan  = up_ivlan;
                break;
            case match_t_o_i_tag:
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
                vlan_port.match_vlan        = up_ovlan;
                vlan_port.match_inner_vlan  = up_ivlan;
                /* Set egress_vlan and egress_inner_vlan with non-zero to create 3 tags manipulation vlan port */
                vlan_port.egress_vlan       = up_ovlan;
                vlan_port.egress_inner_vlan = up_ivlan;
                break;
    
            default:
                printf("ERR: pon_lif_create INVALID PARAMETER lif_type %d\n", lif_type);
                return BCM_E_PARAM;
        }
    
        if(util_verbose) {
            printf("pon_lif created:\n");
            print vlan_port;
        }
        
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("pon_lif_create index failed! %s\n", bcm_errmsg(rv));
        }
    
        *gport = vlan_port.vlan_port_id;
        if((1 == local_route_flag)&& is_device_or_above(unit, JERICHO)){
            bcm_vlan_port_t_init(&pon_vlan_port[pon_port_index]);
            pon_vlan_port[pon_port_index].port=vlan_port.port;
            pon_vlan_port[pon_port_index].criteria=vlan_port.criteria;
            pon_vlan_port[pon_port_index].match_tunnel_value=vlan_port.match_tunnel_value;
            pon_vlan_port[pon_port_index].egress_tunnel_value=vlan_port.egress_tunnel_value;
            pon_vlan_port[pon_port_index].flags=vlan_port.flags;
            pon_vlan_port[pon_port_index].match_ethertype=vlan_port.match_ethertype;
            pon_vlan_port[pon_port_index].match_vlan=vlan_port.match_vlan;
            pon_vlan_port[pon_port_index].match_inner_vlan=vlan_port.match_inner_vlan;
            pon_vlan_port[pon_port_index].egress_vlan=vlan_port.egress_vlan;
            pon_vlan_port[pon_port_index].egress_inner_vlan=vlan_port.egress_inner_vlan;
            pon_vlan_port[pon_port_index].match_pcp=vlan_port.match_pcp;
            pon_vlan_port[pon_port_index].vlan_port_id=vlan_port.vlan_port_id;
            printf(" port %d , gport %d,\n",pon_vlan_port[pon_port_index].port,pon_vlan_port[pon_port_index].vlan_port_id);
            pon_port_index++;
        }
            
        if(util_verbose) {
            print vlan_port.vlan_port_id;
        }
      
        return rv;
    }


/*flag 1: PON, flag 0:NNI*/
int set_port_as_hub(int unit,int flag,bcm_gport_t vlan_port_id,int enable)
{
    int rv;
    int i=0;
    int index=0;
    int found=0;
   if(flag) {
    for(i=0;i<10;i++){
      if(pon_vlan_port[i].vlan_port_id == vlan_port_id)
        {
           index = i;
           found =1;
       }
    }
    if(found == 0)
     {
        return BCM_E_NOT_FOUND;
    }
  }
   
  
   if(flag){

       rv =bcm_vlan_port_find(unit, &pon_vlan_port[index]);
       if (rv != BCM_E_NONE)
       {
           printf("bcm_vlan_port_find error \n");
       }
       
       pon_vlan_port[index].flags=0;
       
       if (soc_property_get(unit , "custom_feature_l3_source_bind_arp_relay",0) || fwd_group) {
           pon_vlan_port[index].flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE  | BCM_VLAN_PORT_FORWARD_GROUP);
       } else {
           pon_vlan_port[index].flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE );
       }
    
        if(1 == enable){
             pon_vlan_port[index].flags |= ( BCM_VLAN_PORT_NETWORK | BCM_VLAN_PORT_REPLACE|BCM_VLAN_PORT_WITH_ID);
        }
        else{
            pon_vlan_port[index].flags |= ( BCM_VLAN_PORT_REPLACE|BCM_VLAN_PORT_WITH_ID);
        }

        rv = bcm_vlan_port_create(unit, &pon_vlan_port[index]);
        if (rv != BCM_E_NONE)
        {
            printf("set port to HUB failed! %s\n", bcm_errmsg(rv));
        }

    }
   else{

       rv =bcm_vlan_port_find(unit, &nni_vlan_port);
       if (rv != BCM_E_NONE)
       {
           printf("bcm_vlan_port_find error \n");
       }

       nni_vlan_port.flags=0;
       nni_vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
       
        if(1 == enable){
             nni_vlan_port.flags |= ( BCM_VLAN_PORT_NETWORK | BCM_VLAN_PORT_REPLACE|BCM_VLAN_PORT_WITH_ID);
        }
        else{
            nni_vlan_port.flags |= ( BCM_VLAN_PORT_REPLACE|BCM_VLAN_PORT_WITH_ID);
        }
        
        rv = bcm_vlan_port_create(unit, &nni_vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("set port to HUB failed! %s\n", bcm_errmsg(rv));
        }
    }
    
    return rv;
}

/*
 * Create PON forward group LIF according to learn fec.
 * PON LIF can be created based on Port or Flow.
 */
int pon_lif_create_fwd_grp_with_learn_fec(int unit, bcm_port_t port_pon, int lif_type, int is_flow, int queue,
    bcm_tunnel_id_t tunnel_id_in, bcm_tunnel_id_t tunnel_id_out, bcm_vlan_t up_ovlan, bcm_vlan_t up_ivlan, 
    int up_pcp, bcm_port_ethertype_t up_ethertype, bcm_if_t fec_gport, bcm_gport_t *gport)
{
    int rv;
    int index;
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    if (is_flow) {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(vlan_port.port, queue);
    } else {
        vlan_port.port = port_pon;
    }        
    if (*gport!=0) {
        vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id,*gport); 
    }        
    /* Set ingress vlan editor of PON LIF to do nothing, when creating PON LIF */
    if (soc_property_get(unit , "custom_feature_l3_source_bind_arp_relay",0) || fwd_group) {
        vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_NETWORK | BCM_VLAN_PORT_FORWARD_GROUP);
        vlan_port.failover_port_id = fec_gport;
    } else {
        vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_NETWORK);
    } 

    vlan_port.match_tunnel_value  = tunnel_id_in;
    vlan_port.egress_tunnel_value = tunnel_id_out;
    if (vlan_port_with_vsi != (-1)) {
        vlan_port.vsi = vlan_port_with_vsi;
    }

    switch(lif_type) {
        case match_untag:            
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
            break;
        case match_otag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
            vlan_port.match_vlan = up_ovlan;
            break;
        case match_o_i_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
            vlan_port.match_vlan        = up_ovlan;
            vlan_port.match_inner_vlan  = up_ivlan;
            break;
        case match_otag_cos:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN;
            vlan_port.match_vlan  = up_ovlan;
            vlan_port.match_pcp   = up_pcp;
            break;
        case match_o_i_tag_cos:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_STACKED;
            vlan_port.match_vlan        = up_ovlan;
            vlan_port.match_inner_vlan  = up_ivlan;
            vlan_port.match_pcp         = up_pcp;
            break;
        case match_all_tags: /* transparent mode */
            vlan_port.flags |= BCM_VLAN_PORT_TRANSPARENT;
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
            break;
        case match_otag_cos_ethertype:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_ETHERTYPE;
            vlan_port.match_vlan      = up_ovlan;
            vlan_port.match_pcp       = up_pcp;
            vlan_port.match_ethertype = up_ethertype;
            break;
        case match_o_i_tag_cos_ethertype:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_PCP_VLAN_VLAN_ETHERTYPE;
            vlan_port.match_ethertype   = up_ethertype;
            vlan_port.match_vlan        = up_ovlan;
            vlan_port.match_inner_vlan  = up_ivlan;
            vlan_port.match_pcp         = up_pcp;
            break;
        case match_ethertype:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_ETHERTYPE;
            vlan_port.match_ethertype = up_ethertype;
            break;
        case match_otag_ethertype:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_ETHERTYPE;
            vlan_port.match_ethertype   = up_ethertype;
            vlan_port.match_vlan        = up_ovlan;
            break;
        case match_o_i_tag_ethertype:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED_ETHERTYPE;
            vlan_port.match_ethertype   = up_ethertype;
            vlan_port.match_vlan        = up_ovlan;
            vlan_port.match_inner_vlan  = up_ivlan;
            break;
        case match_t_o_i_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
            vlan_port.match_vlan        = up_ovlan;
            vlan_port.match_inner_vlan  = up_ivlan;
            /* Set egress_vlan and egress_inner_vlan with non-zero to create 3 tags manipulation vlan port */
            vlan_port.egress_vlan       = up_ovlan;
            vlan_port.egress_inner_vlan = up_ivlan;
            break;

        default:
            printf("ERR: pon_lif_create INVALID PARAMETER lif_type %d\n", lif_type);
            return BCM_E_PARAM;
    }

    if(util_verbose) {
        printf("pon_lif created:\n");
        print vlan_port;
    }
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("pon_lif_create index failed! %s\n", bcm_errmsg(rv));
    }

    *gport = vlan_port.vlan_port_id;

    if(util_verbose) {
        print vlan_port.vlan_port_id;
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
        vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
        if (*gport!=0) {
            vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id,*gport); 
        }
    }    
    if (vlan_port_with_vsi != (-1)) {
        vlan_port.vsi = vlan_port_with_vsi;
    }

    switch(lif_type) {
        case match_all_tags:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
            break;
        case match_otag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan  = down_ovlan;
            break;
        case match_otag_cos:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
            vlan_port.match_vlan  = down_ovlan;
            vlan_port.match_pcp  = down_pcp;
            break;
        case match_o_i_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.match_vlan        = down_ovlan;
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

    if((1 == local_route_flag)&& is_device_or_above(unit, JERICHO)){
        bcm_vlan_port_t_init(&nni_vlan_port);
        nni_vlan_port.port=vlan_port.port;
        nni_vlan_port.criteria=vlan_port.criteria;
        nni_vlan_port.match_tunnel_value=vlan_port.match_tunnel_value;
        nni_vlan_port.egress_tunnel_value=vlan_port.egress_tunnel_value;
        nni_vlan_port.flags=vlan_port.flags;
        nni_vlan_port.match_ethertype=vlan_port.match_ethertype;
        nni_vlan_port.match_vlan=vlan_port.match_vlan;
        nni_vlan_port.match_inner_vlan=vlan_port.match_inner_vlan;
        nni_vlan_port.egress_vlan=vlan_port.egress_vlan;
        nni_vlan_port.egress_inner_vlan=vlan_port.egress_inner_vlan;
        nni_vlan_port.vlan_port_id = vlan_port.vlan_port_id;
        printf("nni: port %d , gport %d,\n",nni_vlan_port.port,nni_vlan_port.vlan_port_id);
    }

    if(util_verbose) {
        print vlan_port.vlan_port_id;
    }

    if (!is_advanced_vlan_translation_mode) {
        /* Set egress vlan editor of NNI LIF to do nothing, when creating NNi LIF */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        action.ot_outer = bcmVlanActionNone;
        action.it_outer = bcmVlanActionNone;
        rv = bcm_vlan_translate_egress_action_add(unit, vlan_port.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
          return rv;
        }
    } else {
        /* In case of port tpid_profile != 0, need to init a VLAN action 
         * to modify the tpid_profile of IVE to the correct vlaue 
         */
        if ((outer_tpid != 0x8100) ||((outer_tpid == 0x8100) && (inner_tpid != 0x9100))) {
            bcm_vlan_port_translation_t_init(&port_trans);
            port_trans.gport = vlan_port.vlan_port_id;
            port_trans.vlan_edit_class_id = 1;
            port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
            rv = bcm_vlan_port_translation_set(unit, &port_trans);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_translation_set egr $rv\n");
                return rv;
            }
        }

        /* Set egress VLAN action of NNI LIF to do nothing */
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.gport = vlan_port.vlan_port_id;
        port_trans.vlan_edit_class_id = 1;
        port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set egr $rv\n");
            return rv;
        }
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
    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID|BCM_MULTICAST_TYPE_L2;
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

    int encap_id;
    int rv;
    
    rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }
    
    return rv;
}

/*
 * delete port from multicast group.
 */
int multicast_vlan_port_del(int unit, int mc_group_id, int sys_port, int gport)
{
    int encap_id;
    int rv;
    
    rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    rv = bcm_multicast_ingress_delete(unit, mc_group_id, sys_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
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
 * Create VSI with id and flooding multicast groups.
 */
int vswitch_create_with_id(int unit, bcm_vlan_t  *new_vsi){
    int rv;
    bcm_vlan_t vsi;
    bcm_vlan_control_vlan_t vsi_control;
   
    /* 1. create vswitch */
    rv = bcm_vswitch_create_with_id(unit,*new_vsi);
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
 * Destroy the multicast group and destroy the VSI.
 */
int vswitch_delete(int unit, bcm_vlan_t vsi){
    int rv;

    /*1. delete multicast group*/
    rv = bcm_multicast_destroy(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_destroy \n");
         return rv;
    }
    
    if(util_verbose){
        printf("delete multicast   0x%08x\n\r",vsi);
    }
    
    if (lif_offset != 0) {
        rv = bcm_multicast_destroy(unit, vsi+lif_offset);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_destroy \n");
            return rv;
        }
        if(util_verbose){
            printf("delete multicast   0x%08x\n\r",vsi+lif_offset);
        }
    }
   
    /*2. destroy vswitch */
    rv = bcm_vswitch_destroy(unit,vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_destroy \n");
        return rv;
    }

    if(util_verbose){
        printf("destroy vswitch   0x%08x\n\r",vsi);
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
    case otag_to_untag:
        /* PON LIF ingress action: delete outer tag. */        
        if (!is_advanced_vlan_translation_mode) {
            action.ot_outer = bcmVlanActionDelete;
        } else {
            action.dt_outer = bcmVlanActionDelete;
            ing_tag_format = c_tag;
            ing_vlan_action_id = ing_del_c_tag;
            ing_vlan_edit_class_id = 2;
        }
        action.outer_tpid = inner_tpid;
        action.inner_tpid = outer_tpid;
        break;

    case otag_to_otag:
        /* PON LIF ingress action: do nothing. */
        if (!is_advanced_vlan_translation_mode) {
            action.ot_outer = bcmVlanActionNone;
            action.ot_inner = bcmVlanActionNone;
            if (qos_map_id > 0) {
                action.ot_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
        } else {
            action.dt_outer = bcmVlanActionNone;
            action.dt_inner = bcmVlanActionNone;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
            ing_tag_format = c_tag;
            ing_vlan_action_id = ing_no_op_0;
            ing_vlan_edit_class_id = 1;
        }
        action.outer_tpid = inner_tpid;
        action.inner_tpid = outer_tpid;
        break;

    case otag_to_otag2:
        /* PON LIF ingress action: replace inner tag. */        
        if (!is_advanced_vlan_translation_mode) {
            action.ot_outer = bcmVlanActionReplace;
            action.new_outer_vlan = down_ovlan;
            if (qos_map_id > 0) {
                action.ot_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
        } else {
            action.dt_outer = bcmVlanActionReplace;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }

            ing_tag_format = c_tag;
            ing_vlan_action_id = ing_rep_c_tag_0;
            ing_vlan_edit_class_id = 4;
        }
        action.outer_tpid = inner_tpid;
        action.inner_tpid = outer_tpid;
        break;

    case otag_to_o_i_tag:
        /* PON LIF ingress action: replace outer tag and add inner tag. */
        if (!is_advanced_vlan_translation_mode) {
            action.ot_outer = bcmVlanActionReplace;        
            action.ot_inner = bcmVlanActionAdd;
            action.new_outer_vlan = down_ovlan;
            action.new_inner_vlan = down_ivlan;        
            action.ot_inner_pkt_prio = bcmVlanActionCopy;
            if (qos_map_id > 0) {
                action.ot_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
        } else {
            action.dt_outer = bcmVlanActionReplace;        
            action.dt_inner = bcmVlanActionAdd;
            action.dt_inner_pkt_prio = bcmVlanActionCopy;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
            ing_tag_format = c_tag;
            ing_vlan_action_id = ing_rep_c_tag_add_c_tag;
            ing_vlan_edit_class_id = 5;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case otag_to_o_i2_tag:
        /* PON LIF ingress action: replace outer tag and add inner tag. */
        if (!is_advanced_vlan_translation_mode) {
            action.ot_outer = bcmVlanActionReplace;        
            action.ot_inner = bcmVlanActionAdd;
            action.new_outer_vlan = down_ovlan;
            action.new_inner_vlan = down_ivlan;
            action.ot_inner_pkt_prio = bcmVlanActionCopy;
            if (qos_map_id > 0) {
                action.ot_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
        } else {
            action.dt_outer = bcmVlanActionReplace;        
            action.dt_inner = bcmVlanActionAdd;
            action.dt_inner_pkt_prio = bcmVlanActionCopy;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
            ing_tag_format = c_tag;
            ing_vlan_action_id = ing_rep_c_tag_add_c_tag;
            ing_vlan_edit_class_id = 6;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case otag_to_o2_i2_tag:        
        /* PON LIF ingress action: replace outer tag and add inner tag. */
        if (!is_advanced_vlan_translation_mode) {
            action.ot_outer = bcmVlanActionReplace;        
            action.ot_inner = bcmVlanActionAdd;
            action.new_outer_vlan = down_ovlan;
            action.new_inner_vlan = down_ivlan;
            action.ot_inner_pkt_prio = bcmVlanActionCopy;
            if (qos_map_id > 0) {
                action.ot_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
        } else {
            action.dt_outer = bcmVlanActionReplace;        
            action.dt_inner = bcmVlanActionAdd;
            action.dt_inner_pkt_prio = bcmVlanActionCopy;
            if (qos_map_id > 0) {
                action.dt_outer_pkt_prio = bcmVlanActionReplace;
                action.priority = qos_map_id;
            }
            ing_tag_format = c_tag;
            ing_vlan_action_id = ing_rep_c_tag_add_c_tag;
            ing_vlan_edit_class_id = 7;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case o_i_tag_to_o_i_tag:
        /* PON LIF ingress action: do nothing. */
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            ing_tag_format = s_c_tag;
            ing_vlan_action_id = ing_no_op_0;
            ing_vlan_edit_class_id = 1;
        }
        break;

    case o_i_tag_to_o_i_tag_2:
        /* PON LIF ingress action: do nothing. */
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            ing_tag_format = c_c_tag;
            ing_vlan_action_id = ing_mod_tpid_1;
            ing_vlan_edit_class_id = 1;
        }
        break;

    case o_i_tag_to_o2_i_tag:
        /* PON LIF ingress action: replace outer tag. */
        action.dt_outer = bcmVlanActionReplace;
        action.new_outer_vlan = down_ovlan;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            
            ing_tag_format = s_c_tag;
            ing_vlan_action_id = ing_rep_s_tag;
            ing_vlan_edit_class_id = 2;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case o_i_tag_to_o_i2_tag_2:
        /* PON LIF ingress action: replace inner tag. */
        action.dt_inner = bcmVlanActionReplace;
        action.new_inner_vlan = down_ivlan;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            ing_tag_format = c_c_tag;
            ing_vlan_action_id = ing_rep_c_tag_1;
            ing_vlan_edit_class_id = 3;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case o_i_tag_to_o2_i2_tag:
        /* PON LIF ingress action: replace outer tag and replace inner tag. */
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.new_outer_vlan = down_ovlan;
        action.new_inner_vlan = down_ivlan;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            ing_tag_format = s_c_tag;
            ing_vlan_action_id = ing_rep_s_c_tag;
            ing_vlan_edit_class_id = 4;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case o_i_tag_to_o2_i2_tag_2:
        /* PON LIF ingress action: replace outer tag and replace inner tag. */
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.new_outer_vlan = down_ovlan;
        action.new_inner_vlan = down_ivlan;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            ing_tag_format = c_c_tag;
            ing_vlan_action_id = ing_rep_s_c_tag;
            ing_vlan_edit_class_id = 4;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case untag_to_untag:
        /* PON LIF ingress action: do nothing. */
        if (!is_advanced_vlan_translation_mode) {
            action.ut_outer = bcmVlanActionNone;
            action.ut_inner = bcmVlanActionNone;
        } else {
            action.dt_outer = bcmVlanActionNone;
            action.dt_inner = bcmVlanActionNone;
            ing_tag_format = no_tag;
            ing_vlan_action_id = ing_no_op_0;
            ing_vlan_edit_class_id = 1;
        }
        break;

    case untag_to_otag:
        /* PON LIF ingress action: add inner tag. */
        if (!is_advanced_vlan_translation_mode) {
            action.ut_outer = bcmVlanActionAdd;
            action.new_outer_vlan = down_ovlan;        
            action.ut_outer_pkt_prio = bcmVlanActionAdd;
        }
        else {
            action.dt_outer = bcmVlanActionAdd;
            action.dt_outer_pkt_prio = bcmVlanActionAdd;
            ing_tag_format = no_tag;
            ing_vlan_action_id = ing_add_c_tag;
            ing_vlan_edit_class_id = 2;
        }
        if (qos_map_id > 0) {
            action.priority = qos_map_id;
        } else {
            action.priority = pon_in_map_id_dft;
        }
        action.outer_tpid = inner_tpid;
        action.inner_tpid = outer_tpid;
        break;

    case untag_to_o_i_tag:
        /* PON LIF ingress action: add outer tag and add inner tag. */
        if (!is_advanced_vlan_translation_mode) {
            action.ut_outer = bcmVlanActionAdd;        
            action.ut_inner = bcmVlanActionAdd;
            action.new_outer_vlan = down_ovlan;
            action.new_inner_vlan = down_ivlan;
            action.ut_outer_pkt_prio = bcmVlanActionAdd;        
            action.ut_inner_pkt_prio = bcmVlanActionAdd;
        } else {
            action.dt_outer = bcmVlanActionAdd;
            action.dt_inner = bcmVlanActionAdd;
            action.dt_outer_pkt_prio = bcmVlanActionAdd;
            action.dt_inner_pkt_prio = bcmVlanActionAdd;
            ing_tag_format = no_tag;
            ing_vlan_action_id = ing_add_s_c_tag;
            ing_vlan_edit_class_id = 3;
        }
        if (qos_map_id > 0) {
            action.priority = qos_map_id;
        } else {
            action.priority = pon_in_map_id_dft;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    case transparent_all_tags:
        /* PON LIF ingress action: do nothing. */
        action.ut_outer = bcmVlanActionNone;
        action.ot_outer = bcmVlanActionNone;
        action.it_outer = bcmVlanActionNone;
        action.dt_outer = bcmVlanActionNone;
        ing_tag_format = any_tag;
        ing_vlan_action_id = ing_no_op_0;
        ing_vlan_edit_class_id = 1;
        break;
    case triple_tag_manipulation:
        /* Change svlan and clvan, and AC will remove tunnel ID
         *
         * Note: this mode only support at PON egress action 
         */
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionReplace;
        action.new_outer_vlan = down_ovlan;
        action.new_inner_vlan = down_ivlan;
        if (qos_map_id > 0) {
            action.dt_outer_pkt_prio = bcmVlanActionReplace;
            action.priority = qos_map_id;
        }
        if (is_advanced_vlan_translation_mode) {
            ing_tag_format = s_c_tag;
            ing_vlan_action_id = ing_rep_s_c_tag;
            ing_vlan_edit_class_id = 4;
        }
        action.outer_tpid = outer_tpid;
        action.inner_tpid = inner_tpid;
        break;

    default:
        return BCM_E_PARAM;
    }

    if (is_advanced_vlan_translation_mode) {
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
        if (qos_map_id > 0) { 
            rv = bcm_qos_port_map_set(unit, pon_gport, qos_map_id, -1);  
            if (rv != BCM_E_NONE) { 
                printf("Error, bcm_qos_port_map_set ing $rv\n");  
                return rv;
            }
        }
    } else {
        if ((action.outer_tpid != 0) && (action.inner_tpid != 0)) {
            action.outer_tpid_action = bcmVlanTpidActionModify;
            action.inner_tpid_action = bcmVlanTpidActionModify;        
        }
        rv = bcm_vlan_translate_action_create(unit, pon_gport, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_translate_action_create %s\n", bcm_errmsg(rv));
            return rv;
        }
        
        if (NULL != action_id) {
            *action_id = action.action_id;
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
	int pon_pp_port_mapping_bypass = soc_property_get(unit, spn_PON_PP_PORT_MAPPING_BYPASS, 0);

    if (is_advanced_vlan_translation_mode) {
        switch (service_type)
        {
        case otag_to_untag:
            action.dt_outer = bcmVlanActionAdd;
            action.outer_tpid = outer_tpid;
            if (!pon_pp_port_mapping_bypass) {
                action.dt_inner = bcmVlanActionAdd;
                action.inner_tpid = inner_tpid;
            }
            egr_tag_format = no_tag;
            egr_vlan_action_id = egr_add_tu_c_tag;
            egr_vlan_edit_class_id = 2;
            break;

        case otag_to_otag:
            if (pon_pp_port_mapping_bypass) {
               action.dt_outer = bcmVlanActionNone;
            } else {
               action.dt_outer = bcmVlanActionAdd;
            }
            action.dt_inner = bcmVlanActionNone;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = c_tag;
            egr_vlan_action_id = egr_add_tu_1;
            egr_vlan_edit_class_id = 2;
            break;

        case otag_to_otag2:
            action.dt_outer = bcmVlanActionReplace;
            if (!pon_pp_port_mapping_bypass) {
                action.dt_inner = bcmVlanActionAdd;
            }
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = c_tag;
            egr_vlan_action_id = egr_rep_tu_add_c_tag;
            egr_vlan_edit_class_id = 3;
            break;

        case otag_to_o_i_tag:
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionDelete;
            } else {
                action.dt_outer = bcmVlanActionReplace;
            }
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_rep_tu;
            egr_vlan_edit_class_id = 3;
            break;

        case otag_to_o_i2_tag:
            action.dt_outer = bcmVlanActionReplace;
            if (pon_pp_port_mapping_bypass) {
                action.dt_inner = bcmVlanActionDelete;
                action.outer_tpid_action = bcmVlanTpidActionModify;
                action.outer_tpid = inner_tpid;
            } else {
                action.dt_inner = bcmVlanActionReplace;
                action.outer_tpid = outer_tpid;
                action.inner_tpid = inner_tpid;
            }

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_rep_tu_c_tag;
            egr_vlan_edit_class_id = 3;
            break;

        case otag_to_o2_i2_tag:
            action.dt_outer = bcmVlanActionReplace;
            if (pon_pp_port_mapping_bypass) {
                action.dt_inner = bcmVlanActionDelete;
                action.outer_tpid_action = bcmVlanTpidActionModify;
                action.outer_tpid = inner_tpid;
            } else {
                action.dt_inner = bcmVlanActionReplace;
                action.outer_tpid = outer_tpid;
                action.inner_tpid = inner_tpid;
            }

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_rep_tu_c_tag;
            egr_vlan_edit_class_id = 3;
            break;

        case o_i_tag_to_o_i_tag:
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionNone;
            } else {
                action.dt_outer = bcmVlanActionAdd;
            }
            action.outer_tpid = outer_tpid;
            action.inner_tpid = outer_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_2;
            egr_vlan_edit_class_id = 4;
            break;

        case o_i_tag_to_o_i_tag_2:
            action.dt_outer = bcmVlanActionAdd;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_3;
            egr_vlan_edit_class_id = 5;
            break;

        case o_i_tag_to_o2_i_tag:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionAdd;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = outer_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_rep_tu_add_s_tag;
            egr_vlan_edit_class_id = 6;
            break;

        case o_i_tag_to_o2_i_tag_2:
            action.dt_outer = bcmVlanActionReplace;
            if (pon_pp_port_mapping_bypass) {
                action.dt_inner = bcmVlanActionReplace;
            } else {
                action.dt_inner = bcmVlanActionAdd;
            }
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_rep_tu_add_s_tag_2;
            egr_vlan_edit_class_id = 7;
            break;

        case o_i_tag_to_o_i2_tag:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_rep_s_c_tag;
            egr_vlan_edit_class_id = 8;
            triple_tags_service = 1;
            break;

        case o_i_tag_to_o_i2_tag_2:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.outer_tpid = inner_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_rep_s_c_tag_2;
            egr_vlan_edit_class_id = 9;
            triple_tags_service = 1;
            break;

        case o_i_tag_to_o2_i2_tag:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_rep_s_c_tag;
            egr_vlan_edit_class_id = 10;
            triple_tags_service = 1;
            break;

        case o_i_tag_to_o2_i2_tag_2:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.outer_tpid = inner_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_rep_s_c_tag_2;
            egr_vlan_edit_class_id = 11;
            triple_tags_service = 1;
            break;

        case untag_to_untag:
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionNone;
            } else {
                action.dt_outer = bcmVlanActionAdd;
            }

            egr_tag_format = no_tag;
            egr_vlan_action_id = egr_add_tu_0;
            egr_vlan_edit_class_id = 3;
            break;

        case untag_to_otag:
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionDelete;
            } else {
                action.dt_outer = bcmVlanActionReplace;
            }
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = c_tag;
            egr_vlan_action_id = egr_rep_tu;
            egr_vlan_edit_class_id = 4;
            break;

        case untag_to_o_i_tag:
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionDelete;
            } else {
                action.dt_outer = bcmVlanActionReplace;
            }
            action.dt_inner = bcmVlanActionDelete;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_rep_tu_del_c_tag;
            egr_vlan_edit_class_id = 10;
            break;

        case transparent_all_tags:
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionNone;
            } else {
                action.dt_outer = bcmVlanActionAdd;
            }
            egr_tag_format = any_tag;
            egr_vlan_action_id = egr_add_tu_0;
            egr_vlan_edit_class_id = 2;
            break;
        case triple_tag_manipulation:
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.outer_tpid = outer_tpid;
            action.inner_tpid = inner_tpid;

            egr_tag_format = s_c_tag;
            egr_vlan_action_id = egr_add_tu_rep_s_c_tag;
            egr_vlan_edit_class_id = 7;
            triple_tags_service = 1;
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
        if (triple_tags_service || pon_pp_port_mapping_bypass) {
            port_trans.new_outer_vlan = up_ovlan;
            port_trans.new_inner_vlan = up_ivlan;
        } else {
            port_trans.new_outer_vlan = tunnel_id;
            port_trans.new_inner_vlan = up_ovlan;
        }
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
            rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
                return rv;
            }

            action_class.tag_format_class_id = s_tag;
            rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
                return rv;
            }

            action_class.tag_format_class_id = c_tag;
            rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
                return rv;
            }

            action_class.tag_format_class_id = s_c_tag;
            rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
                return rv;
            }

            action_class.tag_format_class_id = c_c_tag;
            rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
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
            rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
                return rv;
            }
        }
    } else {
        if (service_type == otag_to_otag2) {
            action.ot_outer = bcmVlanActionReplace;
            action.new_outer_vlan = up_ovlan;
        } else if (service_type == otag_to_o_i_tag) {
            action.dt_outer = bcmVlanActionDelete;
        } else if ((service_type == otag_to_o_i2_tag) || (service_type == otag_to_o2_i2_tag)) {
            if (pon_pp_port_mapping_bypass) {
                action.dt_outer = bcmVlanActionReplace;
                action.dt_inner = bcmVlanActionDelete;
                action.outer_tpid_action = bcmVlanTpidActionModify;
                action.outer_tpid = inner_tpid;
                action.new_outer_vlan = up_ovlan;
            } else {
                action.dt_outer = bcmVlanActionDelete;
                action.dt_inner = bcmVlanActionReplace;
                action.new_inner_vlan = up_ovlan;
            }
        } else if (service_type == o_i_tag_to_o2_i_tag) {
            action.dt_outer = bcmVlanActionReplace;
            action.new_outer_vlan = up_ovlan;
        } else if ((service_type == o_i_tag_to_o2_i2_tag) || (service_type == triple_tag_manipulation)) {
            action.dt_outer = bcmVlanActionReplace;
            action.dt_inner = bcmVlanActionReplace;
            action.new_outer_vlan = up_ovlan;
            action.new_inner_vlan = up_ivlan;
        } else if (service_type == untag_to_otag) {
            action.ot_outer = bcmVlanActionDelete;
        } else if (service_type == untag_to_o_i_tag) {
            action.dt_outer = bcmVlanActionDelete;        
            action.dt_inner = bcmVlanActionDelete;
        } else {
            return BCM_E_NONE;
        }
        
        rv = bcm_vlan_translate_egress_action_add(unit, pon_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
          return rv;
        }
    }

    return rv;
}

/*
 * Remove vlan-port from vsi and delete it.
 */
int vswitch_delete_port(int unit, bcm_vlan_t vsi, bcm_gport_t gport){
    int rv;

    rv = bcm_vswitch_port_delete(unit, vsi, gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_delete\n");
        return rv;
    }
    if(util_verbose){
        printf("remove port   0x%08x from vsi   0x%08x \n\r",gport, vsi);
    }

    rv = bcm_vlan_port_destroy(unit,gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }
    if(util_verbose){
        printf("delete port   0x%08x \n\r",gport);
    }

    return rv;
}



int disable_tag_swap_manually(int unit, int port)
{
    char cmd[300] = {0};
    int rv;
    int is_jer_plus;

    rv = is_jericho_plus_only(unit, &is_jer_plus);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_jericho_plus_only\n", rv);
        return rv;
    }

    if (is_jer_plus) {
        sprintf(cmd, "mod IRE_TAG_SWAP_0_CFG %d 1 TAG_SWAP_ENABLE=0", port);
        bshell(unit, cmd);
        sprintf(cmd, "mod IRE_TAG_SWAP_1_CFG %d 1 TAG_SWAP_ENABLE=0", port);
        bshell(unit, cmd);
    }
    return rv;
}



