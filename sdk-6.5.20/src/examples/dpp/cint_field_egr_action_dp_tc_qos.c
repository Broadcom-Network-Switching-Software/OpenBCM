/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Configure egress PMF change DP TC and qos and see it changed
 *
 * 
 * how to run the test: 
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
    cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
    cint ../../../../src/examples/dpp/utility/cint_utils_qos.c
    cint ../../../../src/examples/dpp/cint_ip_route.c
    cint ../../../../src/examples/dpp/cint_field_egr_action_dp_tc_qos.c
    cint
    field_egr_action_dp_tc_qos__start_run(0,NULL);
    exit;
 * 
 * 
 * Traffic: 
 
 * Packet flow:
 
 */

struct action_dp_tc_qos_info_s{
    int in_port;
    int out_port;
    int group_priority;
    int int_pri;                /* Internal priority */
    bcm_color_t color;          /* Color */
    int tc_for_pmf;
    int dp_for_pmf;
    int qos_profile_id_for_pmf;
};

action_dp_tc_qos_info_s action_info_g ={
        200, /*In port*/
        201, /*Out Port*/
        162, /*group_priority*/
         1, /* Internal priority */
         bcmColorRed, /* Color */
         2, /*tc_for_pmf*/
         1,  /*dp_for_pmf*/
         2 /*qos_profile_id_for_pmf*/
        
};


/*****************************************************************************
* Function:  field_egr_action_dp_tc_qos_init
* Purpose:   
* Params:
* unit - 
* Return:    (int)
 */
int field_egr_action_dp_tc_qos_init(int unit)
{
    
    if(is_device_or_above(unit, JERICHO) == FALSE)
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}    

/*****************************************************************************
* Function:  field_egr_action_dp_tc_qos_struct_get
* Purpose:   
* Params:
* unit  - 
* param - 
 */
void field_egr_action_dp_tc_qos_struct_get(int unit, action_dp_tc_qos_info_s *param)
{
     sal_memcpy(param,&action_info_g, sizeof(action_info_g));
     return;
}

/*****************************************************************************
* Function:  field_egr_action_dp_tc_qos__start_run_with_port
* Purpose:   
* Params:
* unit      - 
* in_port - 
* out_port - 
* Return:    (int)
 */

int field_egr_action_dp_tc_qos__start_run_with_params
                (int unit,int in_port, int out_port,int new_dp,int new_tc,int new_qos)
{
    action_dp_tc_qos_info_s action_info;
    field_egr_action_dp_tc_qos_struct_get(unit,&action_info);
    action_info.in_port = in_port;
    action_info.out_port = out_port;
    action_info.tc_for_pmf = new_tc;
    action_info.dp_for_pmf = new_dp;
    action_info.qos_profile_id_for_pmf = new_qos;
    return field_egr_action_dp_tc_qos__start_run(unit,&action_info);
}

/*****************************************************************************
* Function:  field_egr_action_dp_tc_qos__start_run
* Purpose:   
* Params:
* unit      - 
* action_info_p - 
* Return:    (int)
 */
 int  field_egr_action_dp_tc_qos__start_run(int unit,action_dp_tc_qos_info_s *action_info_p)
 {
    int rv = BCM_E_NONE;
    int auxRes;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    action_dp_tc_qos_info_s action_info;
    bcm_gport_t system_port;
    int group_priority;


    rv = field_egr_action_dp_tc_qos_init(unit);
    if (BCM_E_NONE != rv) 
    {
        printf("Error in field_action_dp_tc_qos_init\n");
        return rv;
    }
    
    if(action_info_p == NULL)
    {
        field_egr_action_dp_tc_qos_struct_get(unit,&action_info);
    }
    else
    {
        sal_memcpy(&action_info,action_info_p, sizeof(action_info));
    }

    /*Configure Group priority*/
    group_priority = action_info.group_priority;
    
    /*Configure PMF*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionQosMapIdNew);

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

    /*
     * Retrieve the Source-System-Port and match it
     */
    rv = bcm_stk_gport_sysport_get (unit, action_info.in_port, &system_port);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_stk_gport_sysport_get\n");
        return rv;
    }
    rv = bcm_field_qualify_SrcPort(unit, ent, 0,0,system_port, 0xffffffff);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
    }
    printf("Qual bcmFieldQualifySrcPort was set with value %d\n",action_info.in_port);
    
    rv = bcm_field_action_add(unit, ent, bcmFieldActionPrioIntNew, action_info.tc_for_pmf, 0);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit,ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
    }
    printf("Action bcmFieldActionPrioIntNew was set with value %d\n",action_info.tc_for_pmf);
    
    rv = bcm_field_action_add(unit, ent, bcmFieldActionDropPrecedence, action_info.dp_for_pmf, 0);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit,ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
    }
    printf("Action bcmFieldActionDropPrecedence was set with value %d\n", action_info.dp_for_pmf);
    
    rv = bcm_field_action_add(unit, ent, bcmFieldActionQosMapIdNew,action_info.qos_profile_id_for_pmf , 0);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit,ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
    }
    printf("Action bcmFieldActionQosMapIdNew was set with id %d\n", action_info.qos_profile_id_for_pmf);
    
    rv = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != rv) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return rv;
     }
    
    return rv;
    
 }

