/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Confgure ETPP LIF MTU trap and see packet terminated
 *
 * soc properties:
 *pmf_vsi_profile_full_range =1;   
 *custom_feature_mtu_advanced_mode=1;
 * 
 * how to run the test: 
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
    cint ../../../../src/examples/dpp/cint_field_qual_vsi_profile_full_range.c
    cint
    field_qual_vsi_profile__start_run(0,NULL);
    exit;
 * 
 * 
 * Traffic: 
 
 * Packet flow:
 
 */

struct qual_vsi_prof_info_s{
    int    vlan;
    uint32 vsi_profile; 
    int group_priority;
};

qual_vsi_prof_info_s qual_info_g ={
        1, /*vlan*/
        9, /*vsi_profile*/
        162 /*group_priority*/
};


/*****************************************************************************
* Function:  field_qual_vsi_profile_init
* Purpose:   
* Params:
* unit - 
* Return:    (int)
 */
int field_qual_vsi_profile_init(int unit)
{
    
    if (soc_property_get(unit, spn_PMF_VSI_PROFILE_FULL_RANGE, 0) == FALSE ||
        soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,"mtu_advanced_mode", 0) == FALSE)
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
    
}

/*****************************************************************************
* Function:  field_qual_vsi_profile_struct_get
* Purpose:   
* Params:
* unit  - 
* param - 
 */
void field_qual_vsi_profile_struct_get(int unit, qual_vsi_prof_info_s *param)
{
     sal_memcpy(param,&qual_info_g, sizeof(qual_info_g));
     return;
}


/*****************************************************************************
* Function:  field_qual_vsi_profile__start_run
* Purpose:   
* Params:
* unit      - 
* vlan - 
* vsi_profile- 
* Return:    (int)
 */
int field_qual_vsi_profile__start_run_with_params(int unit,int vlan,int vsi_profile)
{
    qual_vsi_prof_info_s qual_info;
    field_qual_vsi_profile_struct_get(unit,&qual_info);
    qual_info.vlan = vlan;
    qual_info.vsi_profile = vsi_profile;
    return field_qual_vsi_profile__start_run(unit,&qual_info);
}


/*****************************************************************************
* Function:  field_qual_vsi_profile__start_run
* Purpose:   
* Params:
* unit      - 
* qual_info_p - 
* Return:    (int)
 */
 int field_qual_vsi_profile__start_run(int unit,qual_vsi_prof_info_s *qual_info_p)
 {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    qual_vsi_prof_info_s qual_info;
    int vsi_prof_2_bit_msb;
    int vsi_prof_2_bit_lsb;
    int auxRes;
    int group_priority;

    rv = field_qual_vsi_profile_init(unit);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    if(qual_info_p == NULL)
    {
        field_qual_vsi_profile_struct_get(unit,&qual_info);
    }
    else
    {
        sal_memcpy(&qual_info,qual_info_p, sizeof(qual_info));
    }

    vsi_prof_2_bit_msb = qual_info.vsi_profile >> 2;
    vsi_prof_2_bit_lsb = qual_info.vsi_profile & 3;

    /*Create VLAN*/
    vlan__init_vlan(unit,qual_info.vlan);

    /* Set the VSI profile MSB*/
    vlan__control_vlan_vsi_profile_set(unit,qual_info.vlan,vsi_prof_2_bit_msb);

    /* Set the VSI profile LSB*/
    l2__vpn_to_vsi_profile_set(unit,qual_info.vlan,vsi_prof_2_bit_lsb);


    group_priority = qual_info.group_priority;
    /*Configure PMF*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassL2);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    rv = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != rv) {
        return rv;
        }
    rv = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
    }
    
    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
    }

    rv = bcm_field_qualify_InterfaceClassL2(unit, ent, qual_info.vsi_profile, 0xF);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        /* auxRes = bcm_field_stat_destroy(unit, statId);*/
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
     }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit,ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
        }
    
    rv = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
     }
    
    return rv;
    
 }

