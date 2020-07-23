/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
*
* File: cint_pon_cos_remark_llid_dml.c  
* 
* Purpose: set mapping bwtween packet cos to sysstem tc for 1G traffic to resolve PFC_1 issue.
*          The mapping is used to make sure 1G traffic will go to HR-1 and EGQ-1, and 10G traffic will go to HR-0 and EGQ-0.
*
* Map PKT_PRI to System TC by using PMF as follows:
*     Ingress(PKT_PRI/CFI)    System TC
*     0                       4
*     1                       5
*     2                       6
*     3                       7
*     4                       4
*     5                       5
*     6                       6
*     7                       7
* 
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/pon/cint_pon_cos_remark_llid_dml.c
*     BCM> cint
*     cint> pon_dn_cos_remark(unit, 5);
 */

int packet_cos[8] = {0,1,2,3,4,5,6,7};
int packet_cos_internal_remark[8] = {4,5,6,7,4,5,6,7};
int packet_cos_internal_remark_untagged = 4;

int pon_flow_gport = 0;
int global_dn_outer_tpid = 0x9100;
int fp_group = -1;

int pon_dn_cos_remark(int unit, int serviceIdx)
{
    bcm_error_t rv = BCM_E_NONE;
    int type;
    int entries;
    int group_priority = 5;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;

    int index;

    pon_flow_gport = pon_n_1_service_uc_info[serviceIdx].flow_gport;
    print pon_flow_gport;

    /* check if fp group is created */
    if (fp_group == -1) {
	    BCM_FIELD_QSET_INIT(qset);          
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
    	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVlanFormat);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanPri);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterTpid);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        
        rv = bcm_field_group_create(unit, qset, group_priority, &fp_group);
        if (rv != BCM_E_NONE) {
		    fp_group = -1;
            printf("Error in bcm_field_group_create_id $rv\n");
            return rv;
        }

        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
        rv = bcm_field_group_action_set(unit, fp_group, aset);
        if (rv != BCM_E_NONE) {
		    fp_group = -1;
            printf("Error in bcm_field_group_action_set $rv\n");
            return rv;
        }
    }

	
	/* create field entry for tagged packet */
    for (index = 0; index < 8; index++) {
        /* The 1st entry is to do a mapping between packet cos and modified cos */
        rv = bcm_field_entry_create(unit, fp_group, &entries);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create $rv\n");
            return rv;
        }
        
        rv = bcm_field_qualify_DstPort(unit, entries, 0, 0, pon_flow_gport, 0xffffffff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_DstPort with dst_port $rv %d\n", pon_flow_gport);
            return rv;
        }

        rv = bcm_field_qualify_OuterTpid(unit, entries, global_dn_outer_tpid);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_OuterTpid with packet_tpid $rv %d\n", global_dn_outer_tpid);
            return rv;
        }
        
        rv = bcm_field_qualify_OuterVlanPri(unit, entries, packet_cos[index], 0x7);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_OuterVlanPri with packet_cos $rv %d\n", packet_cos[index]);
            return rv;
        }
        
        rv = bcm_field_action_add(unit, entries, bcmFieldActionPrioIntNew, packet_cos_internal_remark[index], 0);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR in bcm_field_action_add $rv %d\n", packet_cos_internal_remark[index]);
            return rv;
        }  
        
        rv = bcm_field_entry_install(unit, entries);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR in bcm_field_entry_install $rv\n");
            return rv;
        }
    }

    /* create field entry for untagged packet */
    rv = bcm_field_entry_create(unit, fp_group, &entries);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create $rv\n");
        return rv;
    }
        
    rv = bcm_field_qualify_DstPort(unit, entries, 0, 0, pon_flow_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_DstPort with dst_port $rv %d\n", pon_flow_gport);
        return rv;
    }

    rv = bcm_field_qualify_VlanFormat(unit, entries, 0, 0xff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_VlanFormat with untag $rv\n");
        return rv;
    }
        
    rv = bcm_field_action_add(unit, entries, bcmFieldActionPrioIntNew, packet_cos_internal_remark_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR in bcm_field_action_add $rv %d\n", packet_cos_internal_remark_untagged);
        return rv;
    }  
        
    rv = bcm_field_entry_install(unit, entries);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR in bcm_field_entry_install $rv\n");
        return rv;
    }

    return rv;
}

