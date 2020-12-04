/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 * File: field_presel_create_stage_id.c 
 * Purpose: Example of preselection with explicit stage by the API bcm_field_presel_create_stage_id.  
 *  
 * Pre-Requirement: Peselectors management advanced mode should be enabled by SOC property.
 *
 * When configuring a program selection with Forwarding header = IPv4 and entry with specific SMAC and drop action for that field group,
 * it is expected that frame which ingress the device in that source port and have that SMAC will be dropped. 
 * Vlan Editing commands for that Inlif will be performed.
 * The stage of the field program selection is provided explicitly, in this example it is Egress stage.
 * 
 * CINT Usage: 
 *  
 *  1. Set SOC property:
 *      field_presel_mgmt_advanced_mode=1
 *   
 *  2. Run:
 *      cint cint_sand_utils_global.c
 *      cint cint_utils_l3.c
 *      cint cint_utils_l2.c
 *      cint cint_utils_vlan.c
 *      cint cint_multi_device_utils.c
 *      cint cint_ip_route.c 
 *      cint cint_field_presel_mgmt_advanced_mode.c
 *      cint cint_field_presel_create_stage_id.c
 *      cint
 *      basic_example_single_unit(int unit, int in_port, int out_port);
 *      field_presel_stage_example(int unit, int in_port, int out_port, int presel_id);
 *  
 *  3. In order to verify presel was commited correctly to HW,
 *     send IPv4oEth packet with:
 *        DMAC = 00:0c:00:02:00:00
 *        SMAC = 00:00:00:00:00:11
 *        VLAN = 100
 *     from ingress port given as in_port.
 *     Packet should be dropped.
 *  
 *  4. Destroy created group, entry and presel by running : presel_destroy_all(int unit, bcm_field_stage_t stage, int presel_id);
 *  
 *  5. In order to verify presel was removed correctly from HW,
 *     send packet with SMAC address in entry, from ingress port and vid=100.
 *     Packet should pass and be received by egress port.
 */

/* Utility to get the needed Stage offset flag for the BCM stage*/
void presel_stage_to_flag(/* in */ bcm_field_stage_t stage,
                          /* out */ int *flag) 
{
    switch(stage) 
    {
    case bcmFieldStageEgress:
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        break;
    case bcmFieldStageHash:
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
        break;
    default:
        *flag = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        break;
    }
}

/* Utility to get stage qualifier from BCM stage */
void presel_stage_to_qual(/* in */ bcm_field_stage_t stage, 
                          /* out */ bcm_field_qualify_t *qual) 
{
    switch(stage) 
    {
    case bcmFieldStageEgress:
        *qual = bcmFieldQualifyStageEgress;
        break;
    case bcmFieldStageHash:
        *qual = bcmFieldQualifyStageHash;
        break;
    default:
        *qual = bcmFieldQualifyStageIngress;
        break;
    }
}

/* Utility to get group ID, unique for presel and stage */
void presel_to_group(/* in */ bcm_field_presel_t presel,
                     /* in */ bcm_field_stage_t  stage,
                     /* out */ int *group)
{
    int eg_pmf_start_offset,
        slb_start_offset;

    eg_pmf_start_offset  = 42;
    slb_start_offset     = eg_pmf_start_offset +  16;
    switch (stage)
    {
    case bcmFieldStageEgress:
            *group = presel + eg_pmf_start_offset;
            break;
    case bcmFieldStageHash:
            *group = presel + slb_pmf_start_offset;
            break;
    default:
            *group = presel;
            break;
    }
}

/* Setup preselectors using stage or non-stage API 
 *     Used API: (with_stage) ? bcm_field_presel_create_stage_id : bcm_field_presel_create_id */
int field_presel_stage_setup(/* in */ int unit,
                             /* in */ bcm_field_stage_t stage,
                             /* in */ uint8 with_stage,
                             /* in */ int presel_id) 
{
    int result;
    int auxRes;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_stage_info_t info;
    int flags = 0;

    /* Create a presel entity */
    if(with_stage) {
        if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_stage_to_flag(stage, &flags);
            result = bcm_field_presel_create_stage_id(unit, stage, presel_id);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_stage_id\n");
                auxRes = bcm_field_presel_destroy(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | flags);
                return result;
            }
        } else {
            printf("Error: cannot use bcm_field_presel_create_stage_id without setting field_presel_mgmt_advanced_mode=1\n");
            return BCM_E_CONFIG;
        }
    } else {
        /* Negative scenario to test error codes under field_presel_mgmt_advanced_mode=1*/
        result = bcm_field_presel_create_id(unit, presel_id);
        if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            if (BCM_E_CONFIG == result) {
                printf("bcm_field_presel_create_id: error code is correct\n");
                return BCM_E_NONE;
            } else {
                printf("Error in bcm_field_presel_create_id: expected error code %s but received %s for field_presel_mgmt_advanced_mode=1\n",
                       bcm_errmsg(BCM_E_CONFIG), bcm_errmsg(result));
                return BCM_E_FAIL;
            }
        } else {
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_id\n");
                return result;
            }
        }
    }

    /* Depending on stage, configure the presel stage qualifier */	
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | flags, stage);
    if (BCM_E_NONE != result) {
       printf("Error in bcm_field_qualify_Stage\n");
       return result;
    }
    /* Presel qualifier which is allowed by all stages: ingress, Egress and SLB */	
    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | flags, bcmFieldForwardingTypeIp4Ucast);	
    if (BCM_E_NONE != result) {
       printf("Error in bcm_field_qualify_HeaderFormat\n");
       return result;
    }

    return result;
}

/* Create functional field groups attached to the given presel_id */
int field_group_stage_setup(/* in */ int unit,
                            /* in */ bcm_field_stage_t stage,
                            /* in */ int presel_id) 
{
    int result;
    int auxRes;
    bcm_field_group_t grp = presel_id;
    int group_priority = 10;
    bcm_field_presel_set_t psset;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_stage_info_t info;
    bcm_field_qualify_t stage_qual;
    int flags = 0;

    /* Get unique group ID for the preselector and stage */ 
    presel_to_group(presel_id, stage, &grp);
    presel_stage_to_qual(stage, &stage_qual);

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, stage_qual);
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
	
    result = bcm_field_qualify_SrcMac(unit,0,macData,macMask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_SrcMac\n");
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


/*
 * Main function: configure VSI, LIF and call PMF config 
 * Attention! This example uses functions declared in cint_field_presel_mgmt_advanced_mode.c, 
 * please load the Cint before calling the below example 
 */
int field_presel_stage_example(/* in */ int unit,
                               /* in */ bcm_port_t ing_port,
                               /* in */ bcm_port_t egr_port,
                               /* in */ int presel_id)
{
    int result = 0;
    bcm_vlan_t vsi = 100;
    bcm_mac_t mac;
    bcm_field_group_t group = presel_id;
    bcm_vlan_port_t vp;
    bcm_vlan_t vlan = 100;
    bcm_gport_t in_gport_lif = 5000;
    bcm_gport_t out_gport_lif = 6000;
    bcm_field_stage_t stage = bcmFieldStageEgress;
    bcm_field_presel_t *group_id;
    char *proc_name = "field_presel_stage_example";

    port_tpid_init(ing_port,1,1);
    result = port_tpid_set(unit);
    if (result != BCM_E_NONE) {
        printf("%s(): Error, port_tpid_set\n", proc_name);
        return result;
    }
    /*Map local Port to vlan domain*/
    result = bcm_port_class_set(unit, ing_port, bcmPortClassId, ing_port);
     if (result != BCM_E_NONE) {
      printf("%s(): Error, bcm_port_class_set unit %d, port %d rv %d\n", proc_name, unit, ing_port, result);
      return rv;
    }
    /*create VSI*/
    result  = bcm_vlan_create(unit, vsi);
    if (BCM_E_NONE != result) {
        printf("%s(): Error, bcm_vlan_create unit %d, vid %d, rv %d\n", proc_name, unit, vsi, result);
        return result;
    }

    /*create vlan port (Inlif)*/
    result = system_vlan_port_create(unit,ing_port,vlan,vsi,&in_gport_lif,&vp);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in system_vlan_port_create\n", proc_name, unit, vsi, result);
        return result;
    }
    /*create vlan port (Outlif)*/
    result = system_vlan_port_create(unit,egr_port,vlan,vsi,&out_gport_lif,&vp);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in system_vlan_port_create\n", proc_name);
        return result;
    }

    /*Add Static MAC DA to port*/
    mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
    mac[5] = 0x11;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("%s(): Error, vlan_bridge_add_l2_addr_to_gport\n", proc_name);
      return result;
    }
    mac[5] = 0x12;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("%s(): Error, vlan_bridge_add_l2_addr_to_gport\n", proc_name);
      return result;
    }

    mac[5] = 0x13;
    result = vlan_basic_bridge_add_l2_addr_to_gport(unit, egr_port, mac, vsi);
    if (result != BCM_E_NONE) {
      printf("%s(): Error, vlan_bridge_add_l2_addr_to_gport\n");
      return result;
    }

    /* 
     * PMF configuration
     */
    printf("%s(): Creating presel 0x%x... \n", proc_name, presel_id);
    result = field_test_presel_stage_setup(unit, stage , TRUE, presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in field_presels_setup\n", proc_name);
    }
    presel_to_group(presel_id, stage, &group);
    printf("%s(): Creating group 0x%x... \n", proc_name, group);
    result = field_test_group_stage_setup(unit, stage, presel_id);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in field_presels_setup\n", proc_name);
    }

    return result;

}

/* destroy created group, entry and presel associated with the ID and stage*/
int presel_destroy_all(/* in */ int unit,
                       /* in */ bcm_field_stage_t stage,
                       /* in */ uint8 with_stage,
                       /* in */ int presel_id)
{
    int result = 0;
    int group = presel_id;
    int flags = 0;
    char *proc_name = "presel_destroy_all";

    presel_to_group(presel_id, stage, &group);
    printf("%s(): Flushing group %d... \n", proc_name, group);
    result = bcm_field_group_flush(unit, group);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_group_flush\n");
    }
    if(with_stage) {
        presel_stage_to_flag(stage, &flags);
    }
    printf("%s(): Destroying presel 0x%x [with ID 0x%x]... \n", proc_name, presel_id, presel_id | BCM_FIELD_QUALIFY_PRESEL | flags);
    result = bcm_field_entry_destroy(unit,  presel_id | BCM_FIELD_QUALIFY_PRESEL | flags);
    if (result != BCM_E_NONE) {
        printf("%s(): Error, bcm_field_entry_destroy returned %d (%s)\n", proc_name, result, bcm_errmsg(result));
    }

    return result;
}

