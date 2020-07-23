/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario:  Configure Field Processor Program Selection Preselector for header format, MPLS any label any L2/L3
 *
 * 
 * how to run the test: 
    cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c 
    cint ../../../../src/examples/dpp/cint_field_mpls_x_presel.c
    cint
    field_mpls_x_presel__start_run(0,NULL);
    exit;
 */

struct mpls_x_info_s{
    bcm_field_presel_t presel_id;
    int in_port;
    int group_priority;  
};

mpls_x_info_s mpls_x_info_g ={
          2, /*Presel ID*/
        200, /*In port*/
        162 /*group_priority*/    
};


/*****************************************************************************
* Function:  field_mpls_x_presel_init
* Purpose:   
* Params:
* unit - 
* Return:    (int)
 */
int field_mpls_x_presel_init(int unit)
{

    return BCM_E_NONE;
}    

/*****************************************************************************
* Function:  field_mpls_x_presel_struct_get
* Purpose:   
* Params:
* unit  - 
* param - 
 */
void field_mpls_x_presel_struct_get(int unit, mpls_x_info_s *param)
{
     sal_memcpy(param,&mpls_x_info_g, sizeof(mpls_x_info_g));
     return;
}

/*****************************************************************************
* Function:  field_mpls_x_presel__start_run_with_port
* Purpose:   
* Params:
* unit      - 
* in_port - 
* Return:    (int)
 */

int field_mpls_x_presel__start_run_with_params
                (int unit,int in_port)
{
    mpls_x_info_s mpls_x_info;
    field_mpls_x_presel_struct_get(unit,&mpls_x_info);
    mpls_x_info.in_port = in_port; 
    return field_mpls_x_presel__start_run(unit,&mpls_x_info);
}

/*****************************************************************************
* Function:  field_mpls_x_presel__start_run
* Purpose:   
* Params:
* unit      - 
* mpls_x_info_p - 
* Return:    (int)
 */
 int  field_mpls_x_presel__start_run(int unit,mpls_x_info_s *mpls_x_info_p)
 {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    mpls_x_info_s mpls_x_info;
    int group_priority;
    int presel;
    bcm_field_presel_t presel_id;
    bcm_field_presel_set_t psst;
    bcm_gport_t in_gport;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;

    rv = field_mpls_x_presel_init(unit);
    if (BCM_E_NONE != rv) 
    { 
        return rv;
    }
    
    if(mpls_x_info_p == NULL)
    {
        field_mpls_x_presel_struct_get(unit,&mpls_x_info);
    }
    else
    {
        sal_memcpy(&mpls_x_info,mpls_x_info_p, sizeof(mpls_x_info));
    }

    /*Copnfigure presel ID */
    presel_id = mpls_x_info.presel_id;
    
    /*Configure Group priority*/
    group_priority = mpls_x_info.group_priority;

    /* Create a presel entity for MPLS with three labels */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            return rv;
        }
    }
    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }     
    rv = bcm_field_qualify_HeaderFormat(unit, presel_id | presel_flags, bcmFieldHeaderFormatMplsAnyLabelAnyL2L3);
    if (rv != BCM_E_NONE) 
    { 
        return rv;
    }
    BCM_FIELD_PRESEL_INIT(psst);
    BCM_FIELD_PRESEL_ADD(psst, presel_id);
    
    /*Configure PMF*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    rv = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    
    rv = bcm_field_group_presel_set(unit, grp, &psst);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    
    rv = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    
    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    BCM_GPORT_LOCAL_SET(in_gport,mpls_x_info.in_port);
    rv = bcm_field_qualify_SrcPort(unit, ent, 0,0,in_gport, 0xffffffff);
    if (BCM_E_NONE != rv) 
    { 
        return rv;
    }     
    
    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    
    rv = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != rv) 
    {
        return rv;
    }
    
    return rv;
    
 }

