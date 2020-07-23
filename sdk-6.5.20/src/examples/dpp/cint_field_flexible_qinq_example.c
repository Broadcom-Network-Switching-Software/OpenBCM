/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 * File: field_flexible_QinQ_setup.c 
 * Purpose: Example of how to use Flexible QinQ in Ingress FP. 
 *  
 * When inserting a 5-tuple entry to TCAM with a specific Inlif value as an action, it is expected 
 * that frame with that 5-tuple parameters (SIP,DIP,protocol, Src L4 port , Dst L4 port) will classified to that Inlif. 
 * Vlan Editing commands for that Inlif will be performed. 
 * 
 * CINT Usage: 
 *  
 *  1. Run:
 *      cint cint_field_flexible_qinq_example.c
 *      cint
 *      field_flexible_QinQ_example(int unit, bcm_port_t in_port, bcm_port_t out_port);
 *  
 *  2. In order to verify send packet with 5-tuples (SIP,DIP,protocol, L4ports) as  in entry, from ingress port indicated in script.
 *     Packet should be forwarded, and inner vlan should be added to single-tagged frames. 
 */



struct aport_tpid_info_s {
    int port;
    int outer_tpids[2];
    int nof_outers; /*0, 1, 2*/
    int inner_tpids[2];
    int nof_inners; /*0, 1 */
    int discard_frames; /* BCM_PORT_DISCARD_NONE/ALL/UNTAG/TAG */
    int tpid_class_flags; /* flags for bcm_port_tpid_class_set */
    int vlan_transation_profile_id; /* Default Ingress action command in case of no match in VTT L2 LIF lookup */
};

aport_tpid_info_s port_tpid_info1;
uint8 adv_translate_mode;

/* 
 *  nof_outers = 1,2
 *  nof_inners = 0,1
 *  nof_outers >= nof_inners
 *  nof_outers + nof_inners = 2
 */ 
void
port_tpid_init(int port, int nof_outers, int nof_inners){
    port_tpid_info1.port = port;
    port_tpid_info1.outer_tpids[0] = 0x8100;
    port_tpid_info1.outer_tpids[1] = 0x9100;
    port_tpid_info1.inner_tpids[0] = 0x9100;
    port_tpid_info1.nof_outers = nof_outers;
    port_tpid_info1.nof_inners = nof_inners;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_NONE;
    port_tpid_info1.tpid_class_flags = 0;
    port_tpid_info1.vlan_transation_profile_id = 0;

}

int
port_tpid_set(int unit){
    return _port_tpid_set(unit, 0);
}

int
port_lif_tpid_set(int unit){
    return _port_tpid_set(unit, 1);
}

int
_port_tpid_set(int unit, int is_lif){
    int rv;
    int indx;
    int color_set=0;/* has to be zero */


    /* assume port_tpid_init was called with legal params*/
    
    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port_tpid_info1.port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", port_tpid_info1.port);
        return rv;
    }
    
    /* set outer tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_outers; ++indx){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[indx], color_set);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add, tpid=%d, \n", port_tpid_info1.outer_tpids[indx]);
            return rv;
        }
    }
    
    /* set inner tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_inners; ++indx){
        rv = bcm_port_inner_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[indx]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[indx]);
            return rv;
        }
    }
    
    /* Only relevant for physical port */    
    if(!is_lif && !adv_translate_mode) {    
        /* set discard frames on port */
        rv = bcm_port_discard_set(unit, port_tpid_info1.port, port_tpid_info1.discard_frames);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set, discard-type=%d, \n", port_tpid_info1.discard_frames);
            return rv;
        }
    }
    
    return rv;
}

/*create LIF with no match criteria */
int
system_vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t vlan,
    int vsi,
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;
  
    vp->criteria = BCM_VLAN_PORT_MATCH_NONE;
    vp->port = port_id; 
    vp->match_vlan = 0; 
    vp->egress_vlan = 0; 
    vp->vsi = 0; 
    vp->flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    if (*gport!=0) {
        vp->flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); /* this is the LIF id */
    }
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        return rv;
    }

    *gport = vp->vlan_port_id;  

    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*create LIF with double-vlan match criteria */
int
system_vlan_port_double_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    int vsi,
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;

    vp->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp->port = port_id; 
    vp->match_vlan = outer_vlan; 
    vp->match_inner_vlan = inner_vlan;
    vp->egress_vlan = 0; 
    vp->vsi = 0; 
    vp->flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    if (*gport!=0) {
        vp->flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); /* this is the LIF id */
    }
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        return rv;
    }

    *gport = vp->vlan_port_id;  

    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*create LIF with single-vlan match criteria */
int
system_vlan_port_single_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t vlan,
    int vsi,
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;

    vp->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp->port = port_id; 
    vp->match_vlan = vlan; 
    vp->egress_vlan = vlan; 
    vp->vsi = 0; 
    vp->flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    if (*gport!=0) {
        vp->flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); /* this is the LIF id */
    }
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        return rv;
    }

    *gport = vp->vlan_port_id;  

    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*create LIF with port initial-vlan match criteria (for untagged frames) */
int
system_vlan_port_untagged_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t vlan,
    int vsi,
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;
    int p_vid = 100;
    rv =  bcm_port_untagged_vlan_set(unit, port_id, p_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_untagged_vlan_set unit %d, in_port %d, vid %d, rv %d\n", unit, in_port, p_vid, rv);
        return rv;
    }

    vp->criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
    vp->port = port_id; 
    vp->match_vlan = p_vid; 
    vp->vsi = p_vid; 
    vp->flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    if (*gport!=0) {
        vp->flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); /* this is the LIF id */
    }
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        return rv;
    }

    *gport = vp->vlan_port_id;  

    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }
    return BCM_E_NONE;
}



/*Add dtatic mac with specific vlan to MAC forwarding table*/
int
vlan_basic_bridge_add_l2_addr_to_gport(int unit, int gport_id, bcm_mac_t mac1, bcm_vlan_t v1){

    int rv;
    bcm_gport_t g1;
    bcm_l2_addr_t l2_addr1;

    /* local: MACT, add entry points to local-port */
    bcm_l2_addr_t_init(&l2_addr1,mac1,v1); 
    l2_addr1.flags = 0x00000020; /* static entry */
    g1 = gport_id;
    l2_addr1.port = g1;
    rv = bcm_l2_addr_add(unit,&l2_addr1);

    return rv;
}

/*Field Processor configuration for flexible QinQ example - configure 5-tuples attribytes for TCAM entry*/
int field_processor_configuration_set(/* in */ int unit,
                                      /* in */ int in_lif, 
                                      /* out */ bcm_field_group_t *group)

{
    int group_priority = 10;
    bcm_ip_t srcip =0xC0A80001,
             dstip =0xC0A80002,
	         mask = 0xFFFFFFFF;
    int srcport = 0x6000 , dstport = 0x7000;
    bcm_mac_t mac;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int grp;
    int result;

    /* Create Qualifier Set*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngressVlanTranslation);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);

    /* Create Action Set*/ 
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionIngressGportSet);
     
    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }


    result = bcm_field_qualify_SrcIp(unit, ent, srcip, mask);
    if (BCM_E_NONE != result) {
                printf("Error in bcm_field_qualify_SrcIp\n");
        return result;
    }

    result = bcm_field_qualify_DstIp(unit, ent, dstip, mask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstIp\n");
        return result;
    }

    result = bcm_field_qualify_L4SrcPort(unit, ent, srcport, 0xffff);
    if (BCM_E_NONE != result) {
                printf("Error in bcm_field_qualify_L4SrcPort\n");
        return result;
    }

    result = bcm_field_qualify_L4DstPort(unit, ent, dstport, 0xffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_L4DstPort\n");
        return result;
    }

    result = bcm_field_qualify_IpProtocol(unit, ent, 6, 0xFF);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_IpProtocol\n");
        return result;
    }
    

    /*Add the action value*/
    result = bcm_field_action_add(unit, ent, bcmFieldActionIngressGportSet, in_lif, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }
    
    result = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    *group = grp;

}

/*Main procedure for flexible QinQ example test: 
  1. Set ports tpids.
  2. Map local Port to vlan domain
  3. Set VT port profile
  4. create VSI
  5. create InLIF for TCAM entry hit (no match for ISEM)  and for ISEM keys: single-tag/double-tagged/untagged frames
  6. vlan edit command for case of TCAM hit - add vlan to ST, delete outer for DT, add vlan to UT
  7. set field processor configuration (create field group, add appropriate qset and aset to group, add TCAM entry)
  8. Add static MAC to forwarding table for out port.
 
  Expected result:
  1. send ST with TCAM hit params (sip 192.168.0.1, dip 192.168.0.2 protocol:TCP l4_src_port= 0x6000 l4_dst_port= 0x7000) - check vlan add for inlif 5000.
  2. send ST with no hit - check vlan transparent for inlif 5011.
  3. send DT with TCAM hit params (sip 192.168.0.1, dip 192.168.0.2 protocol:TCP l4_src_port= 0x6000 l4_dst_port= 0x7000)- check vlan remove for inlif 5000.
  4. send DT with no hit  - check vlan transparent for inlif 5012.
  5. send UT with TCAM hit params (sip 192.168.0.1, dip 192.168.0.2 protocol:TCP l4_src_port= 0x6000 l4_dst_port= 0x7000)- check vlan add for inlif 5000.
*/
int field_flexible_QinQ_example(/* in */ int unit,
                                /* in */ bcm_port_t ing_port,
                                /* in */ bcm_port_t egr_port )
{

    int result = 0;
    bcm_vlan_t vsi=100;
    bcm_ip_t srcip =0xC0A80001,
             dstip =0xC0A80002,
	         mask = 0xFFFFFFFF;
    int srcport = 0x6000 , dstport = 0x7000;
    bcm_mac_t mac;
    bcm_field_group_t group;
    bcm_vlan_port_t vp;
    bcm_vlan_t vlan = 100;
    bcm_vlan_t inner_vlan=50;
    bcm_gport_t in_gport_lif=9000;
    bcm_gport_t in_ut_gport_lif=9010;
    bcm_gport_t in_st_gport_lif=9011;
    bcm_gport_t in_dt_gport_lif=9012;
    bcm_gport_t out_gport_lif=10000;
    bcm_vlan_action_set_t action_hit;

    /* Save VLAN Edit mode (advanced/standard) */
    adv_translate_mode = soc_property_get(unit ,"bcm886xx_vlan_translate_mode", 0);

    /*Set TPID configuration for ingress port */
    port_tpid_init(ing_port,1,1);
    result = port_tpid_set(unit);
    if (result != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return result;
    }

    /*Set TPID configuration for egress port */
    port_tpid_init(egr_port,2,1);
    result = port_tpid_set(unit);
    if (result != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return result;
    }

    if (adv_translate_mode) {

        result = port__tag_classification__set(unit, ing_port, 5, 0x8100, 0xFFFFFFFF);
        if (result != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", ing_port, result);
            return result;
        }

        result = port__tag_classification__set(unit, ing_port, 6, 0x8100, 0x9100);
        if (result != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", ing_port, result);
            return result;
        }

        result = port__tag_classification__set(unit, egr_port, 5, 0x8100, 0xFFFFFFFF);
        if (result != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", egr_port, result);
            return result;
        }

        result = port__tag_classification__set(unit, egr_port, 6, 0x8100, 0x9100);
        if (result != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", egr_port, result);
            return result;
        }

    }

    /*Map local Port to vlan domain*/
    result = bcm_port_class_set(unit, ing_port, bcmPortClassId, ing_port);
     if (result != BCM_E_NONE) {
      printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, ing_port, result);
      return rv;
    }

     /*Set VT port profile*/
    result = bcm_vlan_control_port_set(unit,ing_port, bcmVlanPortPreferIP4, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_vlan_control_port_set\n");
        return result;
    }

    /*create VSI*/
    result  = bcm_vlan_create(unit, vsi);
    if (BCM_E_NONE != result) {
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vsi, result);
        return result;
    }

    /*create vlan port (Inlif)*/
    result = system_vlan_port_create(unit,ing_port,vlan,vsi,&in_gport_lif,&vp);
    if (BCM_E_NONE != result) {
        printf("Error in system_vlan_port_create\n", unit, vsi, result);
        return result;
    }


    result = system_vlan_port_double_create(unit,ing_port,vlan,inner_vlan,vsi,&in_dt_gport_lif,&vp);
     if (BCM_E_NONE != result) {
             printf("Error in system_vlan_port_double_create\n");
             return result;
     } 

     result = system_vlan_port_untagged_create(unit,ing_port,vlan,vsi,&in_ut_gport_lif,&vp);
     if (BCM_E_NONE != result) {
             printf("Error in system_vlan_port_untagged_create\n");
             return result;
     }

     result = system_vlan_port_single_create(unit,ing_port,vlan,vsi,&in_st_gport_lif,&vp);
     if (BCM_E_NONE != result) {
             printf("Error in system_vlan_port_single_create\n");
             return result;
     }  

    /*create vlan port (Outlif)*/
    result = system_vlan_port_create(unit,egr_port,vlan,vsi,&out_gport_lif,&vp);
    if (BCM_E_NONE != result) {
        printf("Error in system_vlan_port_create\n");
        return result;
    }
	
     if (!adv_translate_mode) {
        /* Ingress action example: Add inner vlan*/
        bcm_vlan_action_set_t_init(&action_hit);

        action_hit.ot_outer = bcmVlanActionNone;
        action_hit.ot_inner = bcmVlanActionAdd;
        action_hit.ut_outer = bcmVlanActionAdd;
        action_hit.dt_outer = bcmVlanActionDelete;
        action_hit.dt_inner = bcmVlanActionNone;
        action_hit.outer_tpid_action = bcmVlanTpidActionModify;
        action_hit.inner_tpid_action = bcmVlanTpidActionModify;
        action_hit.outer_tpid=0x8100;
        action_hit.inner_tpid=0x9100;
        action_hit.new_outer_vlan = vlan;
        action_hit.new_inner_vlan = 200;

        result = bcm_vlan_translate_action_create(unit, in_gport_lif, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action_hit);
        if (result != BCM_E_NONE) {
            printf("Error, in bcm_vlan_translate_action_create\n");
            return result;
        } 
     } else {
         /* ST ingress vlan editing */
        result = ive_eve_translation_set( unit,  in_gport_lif,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionAdd,  vlan,  200, 5 /*edit_profile*/, 5 /*tag_format*/,1);
        if (result != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(result));
            return result;
        }

        /* DT ingress vlan editing */
        result = ive_eve_translation_set( unit,  in_gport_lif,  0x8100,  0x9100,  bcmVlanActionDelete,  bcmVlanActionNone,  vlan,  200, 5 /*edit_profile*/, 6 /*tag_format*/,1);
        if (result != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(result));
            return result;
        }

        /* UT ingress vlan editing */
        result = ive_eve_translation_set( unit,  in_gport_lif,  0x8100,  0x9100,  bcmVlanActionAdd,  bcmVlanActionNone,  vlan,  200, 5 /*edit_profile*/, 0 /*tag_format*/,1);
        if (result != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(result));
            return result;
        }

        /* Clear egress vlan editing */
        bcm_port_tpid_class_t tpid_class;
        bcm_port_tpid_class_t_init(&tpid_class);

        tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
        tpid_class.port = egr_port;
        tpid_class.tag_format_class_id = 9; /* some unused tag format */
        tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        result = bcm_port_tpid_class_set(unit, &tpid_class);
        if (result != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_class_set, port - %d, rv - %d\n", egr_port, result);
            return rv;
        }
        result = ive_eve_translation_set( unit,  out_gport_lif, 0 ,  0,  bcmVlanActionNone,  bcmVlanActionNone,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 11 , 9,0);
        if (result != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(result));
            return result;
        }
    }
    
   /*PMF configuration of 5-tuples*/
   result = field_processor_configuration_set(unit, in_gport_lif, &group);
   if (BCM_E_NONE != result) {
        printf("Error in field_processor_configuration_set\n");
        return result;
    }
 
    /*Add Static MAC DA to port*/
    mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
    mac[5] = 2;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
      return result;
    }

    return result;
}


