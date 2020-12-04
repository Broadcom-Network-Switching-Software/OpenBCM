
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* 
 * File: cint_pon_field_vmac.c
 * Purpose: An example of how to use PMF to update user-headers with VMAC requestion. 
 *    VMAC requestion:
 *        Upstream:     
 *            User-header1[31:16] User-header1[31:24] VMAC identfier (UP).User-header1[23:16] zeros.
 *            User-header1[15:0] MAC 16MSBs
 *            User-header2[31:0] MAC 32LSBs
 *        Downstream:     
 *            User-header1[31:16]  User-header1[31:24] VMAC identfier (Down).User-header1[23:16] zeros.
 *            User-header1[15:0] MAC 16MSBs
 *            User-header2[31:0] MAC 32LSBs
 */

bcm_field_group_t vmac_group[5];
bcm_field_presel_set_t psset_up;
int presel_id_up = 0;
/* Create a pselect for VMAC upstream.
 * params:
 *    unit: device number
 *    in_pbmp: bitmap for PON port want to enable vmac function.
 */
int vmac_upstream_pselect_setup_upstream(int unit, 
                                         bcm_pbmp_t in_pbmp)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int presel_flags = 0;
    /* 
     * Set the preselector to be on the ports
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm, in_pbmp);
    for(i=0; i<256; i++)
    { 
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id_up);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_up | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id_up);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_up | presel_flags);
            return result;
        }
    }
  
    result = bcm_field_qualify_Stage(unit, presel_id_up | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up | presel_flags);
        return result;
    }
    
    result = bcm_field_qualify_InPorts(unit, presel_id_up | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up | presel_flags);
        return result;
    }
  
    /* Select on LIF-Profile[0] = 0x1 */
    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id_up | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up | presel_flags);
        return result;
    }
  
    BCM_FIELD_PRESEL_INIT(psset_up);
    BCM_FIELD_PRESEL_ADD(psset_up, presel_id_up);

    return result;
}

bcm_field_presel_set_t psset_down;
int presel_id_down = 1;
/* Create a pselect for VMAC downstream.
 * params:
 *    unit: device number
 *    in_pbmp: bitmap for NNI port want to enable vmac function.
 */
int vmac_upstream_pselect_setup_downstream(int unit, 
                                         bcm_pbmp_t in_pbmp)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int presel_flags = 0;

    /* 
     * Set the preselector to be on the ports
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm, in_pbmp);
    for(i = 0; i < 256; i++)
    { 
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id_down);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_down | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id_down);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_down | presel_flags);
            return result;
        }
    }
  
    result = bcm_field_qualify_Stage(unit, presel_id_down | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_down | presel_flags);
        return result;
    }
    
    result = bcm_field_qualify_InPorts(unit, presel_id_down | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_down | presel_flags);
        return result;
    }
  
    /* Select on LIF-Profile[0] = 0x1 */
    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id_down | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_down | presel_flags);
        return result;
    }
  
    BCM_FIELD_PRESEL_INIT(psset_down);
    BCM_FIELD_PRESEL_ADD(psset_down, presel_id_down);

    return result;
}

/* Set upstream user-header-2
 */
int vmac_field_group_set_upstream_hd2(/* in */ int unit,
                                      /* in */ int group_priority,
                                      /* out */ bcm_field_group_t *group) 
{
    int result;
    int auxRes;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[4]; /* the 4 data qualifiers */
    uint8 dq_data[2], dq_mask[2];
    uint32 dq_lsb[4] = {0, 0, 0, 4};
    uint32 dq_ndx, dq_ndx2, dq_length[4] = {8, 10, 4, 10};
    bcm_field_qualify_t dq_qualifier[4] = {bcmFieldQualifyL2DestValue,
      bcmFieldQualifyTunnelId, bcmFieldQualifyInterfaceClassProcessingPort, bcmFieldQualifyInterfaceClassProcessingPort};
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext[4];
    bcm_field_extraction_action_t extact;
    int ext_num = 4;

    if (NULL == group) 
    {
        printf("Pointer to group ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    /* 
     * The Field group is a Direct extraction group done 
     * on an action with a size higher than 19b. 
     * Due to hardware limitations, these groups are supported 
     * only under very strict constraints: 
     * - the field extractions must be stricly equal 
     * to the qualifier, i.e. all the qualifier size must be 
     * extracted. In case only part of the qualifier is required 
     * for the extraction, use data qualifiers (like here) predefined- 
     * based. 
     * - the field extractions must be have a size under 16 bits 
     * - they must have no lost bits (except the first extraction), 
     * which means they must be nibble-aligned in the HW when the 
     * HW instruction is defined 
     * - the extraction filter (whether to do or not) can be at the 
     *  most according to the LSB
     * - it is recommended to define these Field groups first, e.g. 
     * after init   
     */
    bcm_field_group_config_t_init(&grp);
    grp.group = -1;
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++) 
    {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
      data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
      data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      data_qual[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
      if (BCM_E_NONE != result) 
      {
          printf("Error in bcm_field_data_qualifier_create\n");
          for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
          {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }

      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
      if (BCM_E_NONE != result) 
      {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
          {
              auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
          }
          return result;
      }
    }

    /*
    *  This Field Group can change the User-Header-2 value
    */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionClassSourceSet);

    /*  Create the Field group */
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET |BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset_up;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create\n");
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    /*
     *  Add a single entry to the Field group.
     */
    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    bcm_field_extraction_action_t_init(&extact);
    extact.action = bcmFieldActionClassSourceSet;
    extact.bias = 0;
    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++) 
    {
        bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
        ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        ext[dq_ndx].qualifier = data_qual[dq_ndx].qual_id;
        ext[dq_ndx].lsb = 0;
        ext[dq_ndx].bits = dq_length[dq_ndx];
    }

    result = bcm_field_direct_extraction_action_add(unit,
                                              ent,
                                              extact,
                                              ext_num/* count */,
                                              ext);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
    *group = grp.group; 

    return result;
}

/* Set upstream user-header-1
 */
int vmac_field_group_set_upstream_hd1(/* in */ int unit,
                                      /* in */ int group_priority,
                                      /* out */ bcm_field_group_t *group) 
{
    int result;
    int auxRes;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[4]; /* the 4 data qualifiers */
    uint32 dq_ndx, dq_ndx2, dq_length[4] = {16, 9, 1, 6};
    uint32 dq_lsb[4] = {16, 0, 0};
    bcm_field_qualify_t dq_qualifier[4] = { bcmFieldQualifyInterfaceClassProcessingPort, bcmFieldQualifyConstantZero, bcmFieldQualifyConstantOne, bcmFieldQualifyConstantZero};
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext[4];
    bcm_field_extraction_action_t extact;
    int ext_num = 4;

    if (NULL == group) 
    {
        printf("Pointer to group ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    /* 
     * The Field group is a Direct extraction group done 
     * on an action with a size higher than 19b. 
     * Due to hardware limitations, these groups are supported 
     * only under very strict constraints: 
     * - the field extractions must be stricly equal 
     * to the qualifier, i.e. all the qualifier size must be 
     * extracted. In case only part of the qualifier is required 
     * for the extraction, use data qualifiers (like here) predefined- 
     * based. 
     * - the field extractions must be have a size under 16 bits 
     * - they must have no lost bits (except the first extraction), 
     * which means they must be nibble-aligned in the HW when the 
     * HW instruction is defined 
     * - the extraction filter (whether to do or not) can be at the 
     *  most according to the LSB
     * - it is recommended to define these Field groups first, e.g. 
     * after init   
     */
    bcm_field_group_config_t_init(&grp);
    grp.group = -1;
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++)
    {
        bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
        data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
        data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
        data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
        data_qual[dq_ndx].length = dq_length[dq_ndx]; 
        result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
        if (BCM_E_NONE != result) 
        {
            printf("Error in bcm_field_data_qualifier_create\n");
            for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
            {
                auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
            }
            return result;
        }

        /* Add the Data qualifier to the QSET */
        result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_qset_data_qualifier_add\n");
            for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
            {
                auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
            }
            return result;
        }
    }

  /*
   *  This Field Group can change the User-Header-1 value
   */
  BCM_FIELD_ASET_INIT(grp.aset);
  BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionClassDestSet);

  /*  Create the Field group */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp.mode = bcmFieldGroupModeDirectExtraction;
  grp.preselset = psset_up;
  result = bcm_field_group_config_create(unit, &grp);
  if (BCM_E_NONE != result)
  {
      printf("Error in bcm_field_group_config_create\n");
      auxRes = bcm_field_group_destroy(unit, grp.group);
      for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++) 
      {
          auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
      }
      return result;
  }

    /*
     *  Add a single entry to the Field group.
     */
    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    bcm_field_extraction_action_t_init(&extact);
    extact.action = bcmFieldActionClassDestSet;
    extact.bias = 0;
    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++) 
    {
        bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
        ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        ext[dq_ndx].qualifier = data_qual[dq_ndx].qual_id;
        ext[dq_ndx].lsb = 0;
        ext[dq_ndx].bits = dq_length[dq_ndx];
    }

    result = bcm_field_direct_extraction_action_add(unit,
                                      ent,
                                      extact,
                                      ext_num/* count */,
                                      ext);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
    *group = grp.group; 

    return result;
}

/* Set upstream user-header-0 and user-header-1
 */
int vmac_upstream_user_header(int unit, int pon_port) 
{
    int result;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_group_t group;
    bcm_field_group_t group1;
    bcm_pbmp_t in_pbmp;

    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_PORT_ADD(in_pbmp, pon_port);

    result = vmac_upstream_pselect_setup_upstream(unit, in_pbmp);
    if (BCM_E_NONE != result)
    {
        bcm_field_presel_destroy(unit, presel_id_up);
        printf("Error in vmac_upstream_pselect_setup\n");
        return result;
    }

    result = vmac_field_group_set_upstream_hd1(unit, group_priority, &group);
    if (BCM_E_NONE != result) 
    {
        bcm_field_presel_destroy(unit, presel_id_up);
        printf("Error in vmac_field_group_set_upstream_hd1\n");
        return result;
    }

    result = vmac_field_group_set_upstream_hd2(unit, group_priority, &group1);
    if (BCM_E_NONE != result) 
    {
        bcm_field_presel_destroy(unit, presel_id_up);
        printf("Error in vmac_field_group_set_upstream_hd2\n");
        return result;
    }

    printf("group:%d, group1:%d\n", group, group1);
    vmac_group[0] = group;
    vmac_group[1] = group1;
    
    return result;
}

/* Set downstream user-header-0 and user-header-1
 */
int vmac_downstream_user_header(int unit, int nni_port) 
{
    int result;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_group_t group, group1, group2;
    bcm_pbmp_t in_pbmp;

    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_PORT_ADD(in_pbmp, nni_port);

    result = vmac_upstream_pselect_setup_downstream(unit, in_pbmp);
    if (BCM_E_NONE != result)
    {
        bcm_field_presel_destroy(unit, presel_id_down);
        printf("Error in vmac_upstream_pselect_setup\n");
        return result;
    }

    result = vmac_field_group_set_downstream_hd1(unit, group_priority, &group);
    if (BCM_E_NONE != result)
    {
        bcm_field_presel_destroy(unit, presel_id_down);
        printf("Error in vmac_field_group_set_downstream_hd1\n");
        return result;
    }

    result = vmac_field_group_set_downstream_hd2(unit, group_priority, group1, group_priority, &group2);
    if (BCM_E_NONE != result) 
    {
        bcm_field_presel_destroy(unit, presel_id_down);
        printf("Error in vmac_field_group_set_downstream_hd2\n");
        return result;
    }

    printf("group:%d, group1:%d group2:%d\n", group, group1, group2);
    vmac_group[2] = group;
    vmac_group[3] = group1;
    vmac_group[4] = group2;

    return result;
}


/* Set downstream user-header-2
 */
int vmac_field_group_set_downstream_hd2(/* in */ int unit,
                                         /* in */ int group_priority_1,
                                         /* out */ bcm_field_group_t *group_1,
                                         /* in */ int group_priority_2,
                                         /* out */ bcm_field_group_t *group_2) 
{
    int result;
    int auxRes;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[2]; /* the 4 data qualifiers */
    uint32 dq_ndx, dq_ndx2, dq_length[2] = {12, 15};
    uint32 dq_lsb[2] = {0, 12};
    bcm_field_qualify_t dq_qualifier[2] = {bcmFieldQualifyL2SrcValue, bcmFieldQualifyL2SrcValue};
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext[3]; /* 5th is const for vmac down */
    bcm_field_extraction_action_t extact;
    int ext_num = 2;
    uint8 i = 0, j = 0;

    bcm_field_group_config_t grp1;
    bcm_field_entry_t cascaded_ent[32];
    bcm_field_aset_t aset1;
    bcm_field_data_qualifier_t data_qual_fg1[1]; /* the 1 data qualifiers */ 
    uint32 dq_length_fg1[1] = {7}; 
    uint32 dq_lsb_fg1[1] = {36};
    bcm_field_qualify_t dq_qualifier_fg1[1] = {bcmFieldQualifyL2DestValue};

    if (NULL == group_1 || NULL == group_2) 
    {
        printf("Pointer to group ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    bcm_field_group_config_t_init(&grp);
    bcm_field_group_config_t_init(&grp1);
    grp.group = -1;
    grp1.group = -1;

    /* 
    * Map the "5 oMAC bits" in Direct table & cascaded Field group.
    * The 2nd lookup database will be {Previous-UD-header[26:0], Cascaded-Value[4:0]} = New-UD-Header[31:0]
    * The other Database is New-UD-Header[57:32] = {"vMAC Down"in [57:50], Previous-UD-header[47:27]}
 */
    result = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth,5 /* bits in cascaded key */);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_control_set result:%d\n", result);
        return result;
    }

    /* Define the group2 QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyCascadedKeyValue);

    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++) 
    {
        bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
        data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
        data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
        data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
        data_qual[dq_ndx].length = dq_length[dq_ndx]; 
        result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_data_qualifier_create\n");
            for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
            {
                auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
            }
            return result;
        }

        /* Add the Data qualifier to the QSET */
        result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
        if (BCM_E_NONE != result) 
        {
            printf("Error in bcm_field_qset_data_qualifier_add\n");
            for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
            {
                auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
            }
            return result;
        }
    }

    /*
     *  This Field Group can change the User-Header-2 value
     */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionClassSourceSet);

    /*  Create the Field group 2 */
    grp.priority = group_priority_2;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset_down;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create\n");
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    /*
     *  Add a single entry to the Field group.
     */
    result = bcm_field_entry_create(unit, grp.group, &ent);

    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++) 
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    bcm_field_extraction_action_t_init(&extact);
    extact.action = bcmFieldActionClassSourceSet;
    extact.bias = 0;
    /* cascaded key:5 bits*/
    dq_ndx = 0;
    bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
    ext[dq_ndx].qualifier = bcmFieldQualifyCascadedKeyValue;
    ext[dq_ndx].lsb = 0;
    ext[dq_ndx].bits = 5;
    for (dq_ndx = 1; dq_ndx < (1 + ext_num); dq_ndx++) 
    {
        /* cascaded key:5 bits*/
        bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
        ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        ext[dq_ndx].qualifier = data_qual[dq_ndx-1].qual_id;
        ext[dq_ndx].lsb = 0;
        ext[dq_ndx].bits = dq_length[dq_ndx-1];
    }

    result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extact,
                                                  (ext_num + 1)/* count */,
                                                  ext);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++) 
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install - grp\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    /* qualifier Set */
    BCM_FIELD_QSET_INIT(grp1.qset);
    BCM_FIELD_QSET_ADD(grp1.qset, bcmFieldQualifyStageIngress);

    bcm_field_data_qualifier_t_init(&data_qual_fg1[0]); 
    data_qual_fg1[0].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
    data_qual_fg1[0].offset = dq_lsb_fg1[0];  
    data_qual_fg1[0].qualifier = dq_qualifier_fg1[0]; 
    data_qual_fg1[0].length = dq_length_fg1[0]; 
    result = bcm_field_data_qualifier_create(unit, &data_qual_fg1[0]);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_data_qualifier_create, result:%d, grp1\n", result);
        auxRes = bcm_field_data_qualifier_destroy(unit, data_qual_fg1[0].qual_id);
    }

    /* Add the Data qualifier to the QSET */
    result = bcm_field_qset_data_qualifier_add(unit, &grp1.qset, data_qual_fg1[0].qual_id);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_qset_data_qualifier_add, result:%d, grp1\n", result);
        auxRes = bcm_field_data_qualifier_destroy(unit, data_qual_fg1[0].qual_id);
    }

    /* Actions Set */
    BCM_FIELD_ASET_INIT(grp1.aset);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionCascadedKeyValueSet);

    /* Create FG 1  */
    grp1.priority = group_priority_1;
    grp1.preselset = psset_down;
    grp1.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp1.mode = bcmFieldGroupModeDirect;

    result = bcm_field_group_config_create(unit, &grp1);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create grp1\n");
        auxRes = bcm_field_group_destroy(unit, grp1.group);
        auxRes = bcm_field_data_qualifier_destroy(unit, data_qual_fg1[0].qual_id);
        return result;
    }
    /*
     * Have 32 entries with 1x1 mapping between the 5 bits with regular bcm_field_qualify_data API.
     * 5 oMAC bits
     *   oMAC bits[2:0] - LEM lookup result bits[38:36]
     *   oMAC bits[4:3] - LEM lookup result bits[42:41]
     */
    for (i = 0; i < 32; i++)
    {
        uint8 mask = 0x67;
        uint8 data = 0;
        result = bcm_field_entry_create(unit, grp1.group, &cascaded_ent[i]);
        if (BCM_E_NONE != result) 
        {
            printf("Error in bcm_field_entry_create - grp1 entry:%d\n", i);
            break;
        }

        if (i >= 8)
        {
            /* Insert 2 bits[5:4], bits value is 0 */
            data = (((i << 2) & 0x60) | (i & 0x7));
        }
        else
        {
            data = i;
        }
        result = bcm_field_qualify_data(unit,
                               cascaded_ent[i],
                               data_qual_fg1[0].qual_id,
                               &data,
                               &mask,
                               1);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_qualify_data - grp1 entry:%d\n", i);
            break;
        }

        result = bcm_field_action_add(unit,
                            cascaded_ent[i],
                            bcmFieldActionCascadedKeyValueSet,
                            i,
                            0x1F);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_action_add - grp1 entry:%d\n", i);
            break;
        }
    }

    if (BCM_E_NONE != result)
    {
        for (j = 0; j < i; j++)
        {
           auxRes = bcm_field_entry_remove(unit, cascaded_ent[j]);
        }
        auxRes = bcm_field_group_destroy(unit, grp1.group);
        auxRes = bcm_field_data_qualifier_destroy(unit, data_qual_fg1[0].qual_id);
        return result;
    }

    result = bcm_field_group_install(unit, grp1.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install - FG1\n");
        for (j = 0; j < i; j++)
        {
           auxRes = bcm_field_entry_remove(unit, cascaded_ent[j]);
        }
        auxRes = bcm_field_group_destroy(unit, grp1.group);
        auxRes = bcm_field_data_qualifier_destroy(unit, data_qual_fg1[0].qual_id);
        return result;
    }

    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
    *group_2 = grp.group; 
    *group_1 = grp1.group;

    return result;
}

/* Set downstream user-header-1
 */
int vmac_field_group_set_downstream_hd1(/* in */ int unit,
                                        /* in */ int group_priority,
                                        /* out */ bcm_field_group_t *group) 
{
    int result;
    int auxRes;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[5]; /* the 4 data qualifiers */
    uint32 dq_ndx, dq_ndx2, dq_length[5] = {13, 3, 8, 1, 7};
    uint32 dq_lsb[5] = {27, 40, 0, 0, 0};
    bcm_field_qualify_t dq_qualifier[5] = {bcmFieldQualifyL2SrcValue, bcmFieldQualifyL2SrcValue, bcmFieldQualifyConstantZero, bcmFieldQualifyConstantOne, bcmFieldQualifyConstantZero};
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext[5];
    bcm_field_extraction_action_t extact;
    int ext_num = 5;

    if (NULL == group)
    {
        printf("Pointer to group ID must not be NULL\n");
        return BCM_E_PARAM;
    }

    bcm_field_group_config_t_init(&grp);
    grp.group = -1;
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++)
    {
        bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
        data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
        data_qual[dq_ndx].offset = dq_lsb[dq_ndx];  
        data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
        data_qual[dq_ndx].length = dq_length[dq_ndx]; 
        result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
        if (BCM_E_NONE != result) 
        {
            printf("Error in bcm_field_data_qualifier_create\n");
            for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++) 
            {
                auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
            }
            return result;
        }

        /* Add the Data qualifier to the QSET */
        result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_qset_data_qualifier_add\n");
            for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++) 
            {
                auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
            }
            return result;
        }
    }

   /*
    *  This Field Group can change the User-Header-1 value
    */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionClassDestSet);

    /*  Create the Field group */
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset_down;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create\n");
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

   /*
    *  Add a single entry to the Field group.
    */
    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    bcm_field_extraction_action_t_init(&extact);
    extact.action = bcmFieldActionClassDestSet;
    extact.bias = 0;
    for (dq_ndx = 0; dq_ndx < ext_num; dq_ndx++)
    {
        bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
        ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
        ext[dq_ndx].qualifier = data_qual[dq_ndx].qual_id;
        ext[dq_ndx].lsb = 0;
        ext[dq_ndx].bits = dq_length[dq_ndx];
    }

    result = bcm_field_direct_extraction_action_add(unit,
                                              ent,
                                              extact,
                                              ext_num/* count */,
                                              ext);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_remove(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp.group);
        for (dq_ndx2 = 0; dq_ndx2 < ext_num; dq_ndx2++)
        {
            auxRes = bcm_field_data_qualifier_destroy(unit, data_qual[dq_ndx2].qual_id);
        }
        return result;
    }

    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
    *group = grp.group; 

    return result;
}


/* Destory all groups */
int vmac_field_set_teardown(/* in */ int unit,
                            /* in */ bcm_field_group_t *group,
                            /* in */ int group_num) 
{
    int result;
    int auxRes;
    int i;

    /* Destroy the Field groups */
    for (i = 0; i < group_num; i++)
    {
        result = bcm_field_group_destroy(unit, group[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error in bcm_field_group_destroy\n");
            return result;
        }
    }

    return result;
}




