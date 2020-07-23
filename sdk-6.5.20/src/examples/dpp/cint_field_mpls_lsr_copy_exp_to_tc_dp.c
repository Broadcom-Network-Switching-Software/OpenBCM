/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_mpls_lsr_copy_exp_to_tc_dp.c
 * Purpose: Example for utilizing the field APIs to map the forwarding label EXP + RIF.CoS-Profile to TC and DP. 
 * 
 * This example illustrates how to map the CoS-Profile+Forwarding-Label.EXP to TC,DP in an MPLS LSR application (pipe model),
 * using Petra-B (not relevant to Arad, which has native support for this)
 *
 * CINT usage:
 *  *   Create a RIF which will perform LSR, using lsr_tc_tp_create_rif_with_vrf_as_cos. The vrf parameter is the RIF CoS-Profile.
 *  *   Use lsr_tc_dp_map_setup to create a field group with appropriate qualifiers and actions (for more details on the field APIs, see for example cint_fid_demo.c)
 *  *   Perform any frame testing desired.
 *  *   Invoke lsr_tc_dp_map_teardown(unit, group).
 *
 *  Modify lsr_tc_dp_map_setup to fit your needs, adding entries as you like.
 */

int lsr_tc_dp_map_setup(/* in */ int unit,
                    /* in */ int group_priority,
                    /* out */ bcm_field_group_t *group) {
    int result;
    int auxRes;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    uint32 vrfData;
    uint32 vrfMask;
    uint8 expData;
    uint8 expMask;
    bcm_field_stat_t stats[2];
    int statId;
    
    /* 
     * Build the qualifier set for the field group which will include entries to map {CoS-Profile X Forwarding-Label.Exp}
     * to TC and DP. Since Petra-B can't access RIF.CoS-Proflie from the PMF, we use the RIF.VRF[3:0] as the CoS-Profile. 
     * This is OK because there is no use for the VRF on RIFs functioning as an LSR. 
     */ 
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMpls);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsForwardingLabelExp);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    
    /*
     *  Build the action set for the group.
     *	We want to update the TC and DP.	
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
    


  /*
   *  Create the group and set its actions.
   */
  result = bcm_field_group_create(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
    print bcm_field_show(unit,"");
    return result;
  }

    result = bcm_field_group_action_set(unit, *group, aset);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }
    
    /*
     *  Add an entry to the group (could add many entries).
     */
    result = bcm_field_entry_create(unit, *group, &ent);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }
	vrfData = 3;
	vrfMask = 0xf; /* only bits [3:0] are taken as the cos-profile */
    /* The VRF is used as the RIF cos-profile, and the EXP is taken from the forwarding label */
    result = bcm_field_qualify_Vrf(unit, ent, vrfData, vrfMask);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }
	expData = 2;
	expMask = 0x7; /* EXP field is 3 bits wide */
    result = bcm_field_qualify_MplsForwardingLabelExp(unit, ent, expData, expMask);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }
    
    /*
     *  Once we match the frames, map them to TC 3 + DP yellow
     */
    result = bcm_field_action_add(unit, ent, bcmFieldActionDropPrecedence, BCM_FIELD_COLOR_YELLOW, 0);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }  

    result = bcm_field_action_add(unit, ent, bcmFieldActionPrioIntNew, 3, 0);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }  
    
    /*
     *  Commit the entire group to hardware.  We could do individual entries,
     *  but since we just created the group here and want the entry/entries all
     *  in hardware at this point, it's a bit quicker to commit the whole group.
     */
    result = bcm_field_group_install(unit, *group);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, *group);
        return result;
    }
    
    return result;
}

/* this function sets up the MPLS RIF, with the CoS-Profile entered as the VRF */
int lsr_tc_tp_create_rif_with_vrf_as_cos(int unit, int rif, int vrf, int *intf) {

	/* the VSI mymac for the MPLS RIF */
	bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34};

	bcm_port_config_t c;
    bcm_pbmp_t p,u;
    int rc;
	bcm_l3_intf_t l3if, l3if_ori;
    bcm_l3_ingress_t l3_ing_if;
        
	/* create a vlan, add all ports */
    bcm_port_config_get(unit, &c);
    BCM_PBMP_ASSIGN(p, c.e);
    rc = bcm_vlan_create(unit, rif);
    if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
        printf ("bcm_vlan_create failed: %d \n", rc);
        return rc;
    }
    if(rc == BCM_E_EXISTS) {
        printf ("vlan %d aleady exist \n", vlan);
    }
    
    rc = bcm_vlan_port_add(unit, rif, p, u);
        if (rc != BCM_E_NONE) {
        return rc;
    }

    /* before creating L3 INTF, check whether L3 INTF already exists*/
    bcm_l3_intf_t_init(&l3if_ori);
    l3if_ori.l3a_intf_id = rif;
    rc = bcm_l3_intf_get(unit, &l3if_ori);
    if (rc == BCM_E_NONE) {
        /* if L3 INTF already exists, replace it*/
        l3if.l3a_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
        l3if.l3a_intf_id = rif;
    }    

    sal_memcpy(l3if.l3a_mac_addr, mac_l3_ingress, 6);
    l3if.l3a_vid = rif;
    l3if.l3a_vrf = vrf;
    
    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE) {
        return rc;
    }
	*intf = l3if.l3a_intf_id;

	return rc;
}

int lsr_tc_dp_map_teardown(/* in */ int unit,
                       /* in */ bcm_field_group_t group) {
    int result;
    int auxRes;
    bcm_field_entry_t ent[128];
    int entCount;
    int entIndex;
    int statId;
    
    do {
        /* get a bunch of entries in this group */
        result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
        if (BCM_E_NONE != result) {
            return result;
        }
        for (entIndex = 0; entIndex < entCount; entIndex++) {
            /* remove each entry from hardware and destroy it */
            result = bcm_field_entry_remove(unit, ent[entIndex]);
            if (BCM_E_NONE != result) {
                return result;
            }
            result = bcm_field_entry_destroy(unit, ent[entIndex]);
            if (BCM_E_NONE != result) {
                return result;
            }
        }
    /* all as long as there were entries to remove & destroy */
    } while (entCount > 0);
    
    /* destroy the group */  
    result = bcm_field_group_destroy(unit, group);
    if (BCM_E_NONE != result) {
                return result;
            }
    
    return result;
}

