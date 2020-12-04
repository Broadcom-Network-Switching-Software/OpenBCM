/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 * File: field_presel_mgmt_advanced_mode.c 
 * Purpose: Example of how to use presel management Advanced mode .  
 *  
 * In order to enable presels managemnt advanced mode, it is necessary 
 * to set a soc property to indicate it. 
 * When configuring a program selection with src port qualifier and entry with specific DA and drop action for that field group, it is expected 
 * that frame which ingress the device in that source port and have that DA will be dropped. 
 * Vlan Editing commands for that Inlif will be performed.  
 * 
 * CINT Usage: 
 *  
 *  1. Set soc property:
 *      field_presel_mgmt_advanced_mode=1
 *   
 *  2. Run:
 *      cint cint_field_presel_mgmt_advanced_mode.c
 *      cint
 *      field_presels_example(int unit, ,int stage, int single_presel , int in_port, int out_port);
 *  
 *      where if stage==0 : ingress stage, else egress stage
 *            if single_presel==1 => configure single presel
 *  
 *  3. In order to verify presel was commited correctly to HW,
 *     send packet with DA address in entry, from ingress port.
 *     Packet should be dropped.
 *  
 *  4. destroy presel by running : presel_destroy(int unit , int presel_id)
 *  
 *  5. In order to verify presel was removed correctly from HW,
 *     send packet with DA address in entry, from ingress port.
 *     Packet should pass and received by egress port.
 */

struct port_tpid_info1_s {
    int port;
    int outer_tpids[2];
    int nof_outers; /*0, 1, 2*/
    int inner_tpids[2];
    int nof_inners; /*0, 1 */
    int discard_frames; /* BCM_PORT_DISCARD_NONE/ALL/UNTAG/TAG */
    uint32 tpid_class_flags; /* flags for bcm_port_tpid_class_set */
    int vlan_transation_profile_id; /* Default Ingress action command in case of no match in VTT L2 LIF lookup */
};

port_tpid_info1_s port_tpid_info1;

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
    port_tpid_info1.outer_tpids[1] = 0x88a0;
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
    if(!is_lif) {    
        /* set discard frames on port */
        rv = bcm_port_discard_set(unit, port_tpid_info1.port, port_tpid_info1.discard_frames);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set, discard-type=%d, \n", port_tpid_info1.discard_frames);
            return rv;
        }
    }
    
    return rv;
}


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

/* 
 * Utility function to retrieve the modid associated with port;
 * For use with the qualifiers for DstPort and SrcPort
 */
int local_port_modid_get(int unit, bcm_port_t local_port, int *modid) 
{
    int rv = 0;
    int dev_modid;
    int core;
    uint32 flags;
    
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    char *proc_name = "local_port_modid_get";

    rv = bcm_stk_my_modid_get(unit, &dev_modid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_my_modid_get\n");
        return rv;
    }
    
    rv = bcm_port_get(unit, local_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_get\n");
        return rv;
    }
    
    core = mapping_info.core;
    *modid = dev_modid + core;

    printf("%s(): For local destination port (%d), core is %d, port_modid is %d\r\n",proc_name, local_port, core, *modid);
    
    return rv;
}

int field_single_presels_setup(/* in */ int unit,
                        /* in */ int in_stage,
                        /* in */ bcm_port_t ing_port,
                        /* in */ bcm_port_t egr_port )
{

    int group_priority = 10;
    bcm_gport_t local_gport;
    bcm_field_presel_set_t psset;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int result;
    int auxRes;
    bcm_field_stage_t stage;
    int group_id = 0; 
    bcm_field_group_t grp = 0;
    bcm_field_stage_info_t info;
    int presel_id;
    int presel_flags = 0;
    int modid;

    stage = (in_stage) ? bcmFieldStageEgress : bcmFieldStageIngress;
	result = bcm_field_stage_info_get(unit, stage, &info );
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_stage_info_get\n");
        return result;
    }	

    printf("Max number of presels = %d\n", info.field_presel_max_id);

    presel_id = 2/*sal_rand()%info.field_presel_max_id*/;
    printf("presel_id = %d\n", presel_id);
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        if(in_stage) {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        } else {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        }
        result = bcm_field_presel_create_stage_id(unit, stage, presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_stage_id\n");
                auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
                return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_id\n");
                auxRes = bcm_field_presel_destroy(unit, presel_id);
                return result;
        }
    }

    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, 1);
    BCM_PBMP_PORT_ADD(pbm, 32);
    BCM_PBMP_PORT_ADD(pbm, 64);
    for(i=0; i<256; i++) { BCM_PBMP_PORT_ADD(pbm_mask, i); }


    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /* Depending on stage, configure the presel stage and in/out port qualifier */	
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    if (!in_stage) {
        result = local_port_modid_get(unit, ing_port, &modid);
        if (BCM_E_NONE != result) {
            printf("Error in local_port_modid_get\n");
            return result;
        }
        result = bcm_field_qualify_SrcPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, modid, -1, ing_port, 0xffffffff);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_SrcPort\n");
            return result;
        }
    }
    else
    {
        result = local_port_modid_get(unit, egr_port, &modid);
        if (BCM_E_NONE != result) {
            printf("Error in local_port_modid_get\n");
            return result;
        }
        result = bcm_field_qualify_OutPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, modid, -1, egr_port, 0xffffffff);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_DstPort\n");
            return result;
        }
    }

    BCM_FIELD_QSET_INIT(qset);

    if(in_stage == 0) 	{
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    } else {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    }

    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
            
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    
    result = bcm_field_group_create_id(unit, qset, group_priority, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create_id(unit, grp, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create_id\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }


    macData[0] = 0x00;
    macData[1] = 0x00;
    macData[2] = 0x00;
    macData[3] = 0x00;
    macData[4] = 0x00;
    macData[5] = 0x11;

    macMask[0] = 0xff;
    macMask[1] = 0xff;
    macMask[2] = 0xff;
    macMask[3] = 0xff;
    macMask[4] = 0xff;
    macMask[5] = 0xff;
    
    result = bcm_field_qualify_DstMac(unit,0,macData,macMask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstMac\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    result = bcm_field_action_add(unit, 0, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    return result;
}

int field_test_presels_setup_intersection(/* in */ int unit,
                        /* in */ int in_stage,
                        /* in */ bcm_port_t ing_port,
                        /* in */ bcm_port_t egr_port )
{

    int group_priority = 12;
    bcm_gport_t local_gport;
    bcm_field_presel_set_t psset;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent =2;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int result;
    int auxRes;
    bcm_field_stage_t stage;
    int group_id = 2; 
    bcm_field_group_t grp = 2;
    bcm_field_stage_info_t info;
    int presel_id = 7;
    int presel_flags = 0;
    int modid;

    stage = (in_stage) ? bcmFieldStageEgress : bcmFieldStageIngress;
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        if(in_stage) {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        } else {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        }
        result = bcm_field_presel_create_stage_id(unit, stage, presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_stage_id\n");
                auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
                return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_id\n");
                auxRes = bcm_field_presel_destroy(unit, presel_id);
                return result;
        }
    }
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /* Depending on stage, configure the presel stage and in/out port qualifier */	
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    if (!in_stage) {
        result = local_port_modid_get(unit, ing_port, &modid);
        if (BCM_E_NONE != result) {
            printf("Error in local_port_modid_get\n");
            return result;
        }
        result = bcm_field_qualify_SrcPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, modid, -1, ing_port, 0xffffffff);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_SrcPort\n");
            return result;
        }
    }
    else
    {
        result = local_port_modid_get(unit, egr_port, &modid);
        if (BCM_E_NONE != result) {
            printf("Error in local_port_modid_get\n");
            return result;
        }
        result = bcm_field_qualify_DstPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, modid, -1, egr_port, 0xffffffff);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_DstPort\n");
            return result;
        }
    }

    if(in_stage == 0) {
    result = bcm_field_qualify_IpType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldIpTypeIpv4Any);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_IpType\n");
            return result;
        }
    } else {
        result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeL2);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_IpType\n");
            return result;
        }
    }

    BCM_FIELD_QSET_INIT(qset);

    if(in_stage == 0) 	{
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    } else {
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    }

    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
        
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    result = bcm_field_group_create_id(unit, qset, group_priority, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create_id(unit, grp, ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create_id\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
	

    macData[0] = 0x00;
    macData[1] = 0x00;
    macData[2] = 0x00;
    macData[3] = 0x00;
    macData[4] = 0x00;
    macData[5] = 0x13;

    macMask[0] = 0xff;
    macMask[1] = 0xff;
    macMask[2] = 0xff;
    macMask[3] = 0xff;
    macMask[4] = 0xff;
    macMask[5] = 0xff;
	
    result = bcm_field_qualify_DstMac(unit,ent,macData,macMask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstMac\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    return result;
}


int field_test_presels_setup(/* in */ int unit,
                        /* in */ int in_stage,
                        /* in */ bcm_port_t ing_port,
                        /* in */ bcm_port_t egr_port )
{

    int group_priority = 10;
    bcm_gport_t local_gport;
    bcm_field_presel_set_t psset1, psset2;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_field_qset_t qset1, qset2;
    bcm_field_aset_t aset1 , aset2;
    bcm_field_entry_t ent1=0, ent2=1;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int result;
    int auxRes;
    bcm_field_stage_t stage;
    int group_id = 0; 
    bcm_field_group_t grp1 = 0;
    bcm_field_group_t grp2 = 1;
    bcm_field_stage_info_t info;
    int low_presel_id = 2;
    int high_presel_id = 4;
    int modid;
    bcm_field_stage_info_t stage_info;
    int presel_flags = 0;

    result = bcm_field_stage_info_get(unit, stage, &stage_info);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_stage_info_get\n");
        return result;
    }

    stage = (in_stage) ? bcmFieldStageEgress : bcmFieldStageIngress;

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        if(in_stage) {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        } else {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        }
        result = bcm_field_presel_create_stage_id(unit, stage, low_presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_stage_id\n");
                auxRes = bcm_field_presel_destroy(unit, low_presel_id | presel_flags);
                return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_id\n");
                auxRes = bcm_field_presel_destroy(unit, low_presel_id);
                return result;
        }
    }

    BCM_FIELD_PRESEL_INIT(psset1);
    BCM_FIELD_PRESEL_ADD(psset1, low_presel_id);

    /* Depending on stage, configure the presel stage and in/out port qualifier */	
    result = bcm_field_qualify_Stage(unit, low_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    if (!in_stage) {
        result = local_port_modid_get(unit, ing_port, &modid);
        if (BCM_E_NONE != result) {
            printf("Error in local_port_modid_get\n");
            return result;
        }
        result = bcm_field_qualify_SrcPort(unit, low_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, modid, -1, ing_port, 0xffffffff);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_SrcPort\n");
            return result;
        }
    }
    else
    {
        result = local_port_modid_get(unit, egr_port, &modid);
        if (BCM_E_NONE != result) {
            printf("Error in local_port_modid_get\n");
            return result;
        }
        result = bcm_field_qualify_DstPort(unit, low_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, modid, -1, egr_port, 0xffffffff);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_DstPort\n");
            return result;
        }
    }

    BCM_FIELD_QSET_INIT(qset1);

    if(in_stage == 0) 	{
        BCM_FIELD_QSET_ADD(qset1, bcmFieldQualifyStageIngress);
    } else {
        BCM_FIELD_QSET_ADD(qset1, bcmFieldQualifyStageEgress);
    }

    BCM_FIELD_QSET_ADD(qset1, bcmFieldQualifyDstMac);
            
    BCM_FIELD_ASET_INIT(aset1);
    BCM_FIELD_ASET_ADD(aset1, bcmFieldActionDrop);
    
    result = bcm_field_group_create_id(unit, qset1, group_priority, grp1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp1, &psset1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp1, aset1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create_id(unit, grp1, ent1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create_id\n");
        auxRes = bcm_field_entry_destroy(unit, ent1);
        auxRes = bcm_field_group_destroy(unit, grp1);
        return result;
    }

    macData[0] = 0x00;
    macData[1] = 0x00;
    macData[2] = 0x00;
    macData[3] = 0x00;
    macData[4] = 0x00;
    macData[5] = 0x11;

    macMask[0] = 0xff;
    macMask[1] = 0xff;
    macMask[2] = 0xff;
    macMask[3] = 0xff;
    macMask[4] = 0xff;
    macMask[5] = 0xff;
    
    result = bcm_field_qualify_DstMac(unit,ent1,macData,macMask);
    if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_DstMac\n");
            auxRes = bcm_field_entry_destroy(unit, ent1);
            auxRes = bcm_field_group_destroy(unit, grp1);
            return result;
    }
    result = bcm_field_action_add(unit, ent1, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, ent1);
        auxRes = bcm_field_group_destroy(unit, grp1);
        return result;
    }


    /* Create program and attach group for higher priority presel*/
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        if(in_stage) {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        } else {
            presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        }
        result = bcm_field_presel_create_stage_id(unit, stage, high_presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_stage_id\n");
                auxRes = bcm_field_presel_destroy(unit, high_presel_id | presel_flags);
                return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, high_presel_id);
        if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_id\n");
                auxRes = bcm_field_presel_destroy(unit, high_presel_id);
                return result;
        }
    }

    BCM_FIELD_PRESEL_INIT(psset2);
    BCM_FIELD_PRESEL_ADD(psset2, high_presel_id);

    /* Depending on stage, configure the presel stage and in/out port qualifier */	
    result = bcm_field_qualify_Stage(unit, high_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, stage);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    if(in_stage == 0) {
        result = bcm_field_qualify_IpType(unit, high_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldIpTypeIpv4Any);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_IpType\n");
            return result;
        }
    } else {
        result = bcm_field_qualify_ForwardingType(unit, high_presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeL2);	
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_IpType\n");
            return result;
        }
    }

    BCM_FIELD_QSET_INIT(qset2);

    if(in_stage == 0) 	{
        BCM_FIELD_QSET_ADD(qset2, bcmFieldQualifyStageIngress);
    } else {
        BCM_FIELD_QSET_ADD(qset2, bcmFieldQualifyStageEgress);
    }

    BCM_FIELD_QSET_ADD(qset2, bcmFieldQualifyDstMac);
        
    BCM_FIELD_ASET_INIT(aset2);
    BCM_FIELD_ASET_ADD(aset2, bcmFieldActionDrop);

    result = bcm_field_group_create_id(unit, qset2, group_priority+1, grp2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp2, &psset2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
  
    result = bcm_field_group_action_set(unit, grp2, aset2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create_id(unit, grp2, ent2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create_id\n");
        auxRes = bcm_field_entry_destroy(unit, ent2);
        auxRes = bcm_field_group_destroy(unit, grp2);
        return result;
    }

    macData[5] = 0x12;

	
    result = bcm_field_qualify_DstMac(unit,ent2,macData,macMask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_DstMac\n");
        auxRes = bcm_field_entry_destroy(unit, ent2);
        auxRes = bcm_field_group_destroy(unit, grp2);
        return result;
    }
    result = bcm_field_action_add(unit, ent2, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, ent2);
        auxRes = bcm_field_group_destroy(unit, grp2);
        return result;
    }

    return result;
}


int field_presels_example(/* in */ int unit, int in_stage, int single_presel,
                        /* in */ bcm_port_t ing_port,
                        /* in */ bcm_port_t egr_port )
{

    int result = 0;
    bcm_vlan_t vsi=100;

    bcm_mac_t mac;
    bcm_field_group_t group;
    bcm_vlan_port_t vp;
    bcm_vlan_t vlan = 100;
    bcm_gport_t in_gport_lif=5000;
    bcm_gport_t out_gport_lif=6000;


    port_tpid_init(ing_port,1,1);
    result = port_tpid_set(unit);
    if (result != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return result;
    }

    /*Map local Port to vlan domain*/
    result = bcm_port_class_set(unit, ing_port, bcmPortClassId, ing_port);
     if (result != BCM_E_NONE) {
      printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, ing_port, result);
      return rv;
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


    /*create vlan port (Outlif)*/
    result = system_vlan_port_create(unit,egr_port,vlan,vsi,&out_gport_lif,&vp);
    if (BCM_E_NONE != result) {
        printf("Error in system_vlan_port_create\n");
        return result;
    }
    
   /*PMF configuration */
    if (single_presel) {
        result = field_single_presels_setup(unit, in_stage, ing_port, egr_port);
        if (BCM_E_NONE != result) {
            printf("Error in field_presels_setup\n");
        }
    } else {
        result = field_test_presels_setup(unit, in_stage, ing_port, egr_port);
        if (BCM_E_NONE != result) {
            printf("Error in field_presels_setup\n");
        }
    }

    /*Add Static MAC DA to port*/
    mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
    mac[5] = 0x11;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
      return result;
    }
    mac[5] = 0x12;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
      return result;
    }

    mac[5] = 0x13;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("Error, vlan_bridge_add_l2_addr_to_gport\n");
      return result;
    }

    return result;

}

int presel_destroy(int unit , int presel_id )
{
	int result = 0;

	result = bcm_field_entry_destroy(unit,  presel_id | BCM_FIELD_QUALIFY_PRESEL);
    if (result != BCM_E_NONE) {
      printf("Error, bcm_field_entry_destroy\n");
    }

    return result;
}
