/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* Use first cycle's action to lookup in second cycle (cascaded) 
 * Two databases are created - the first is a regolar TCAM and the second is a Direct Table DB.
 * When a packet arrives, its tunnel ID (first 10 bits) is used to address the action table (direct table - second database).
 * The action holds a part of the key to be searched in the second cycle (in the TCAM - first database), 
 * along with other important firlds. 
 * In this CINT we create some entries for the Ditect Table database in order to allocate some bank for it to use.
 * In practice, these entries are not needed at this point, however without it there would be no bank.
 */
int mact_llid_limit(int unit) 
{
    bcm_field_group_config_t grp1;
    bcm_field_group_config_t grp2;
    bcm_field_aset_t aset1;
    bcm_field_aset_t aset2;
    bcm_field_entry_t ent[16];
    bcm_field_entry_t dt_ent[16];
    bcm_field_entry_t ent_drop[16];
    int result;
    int auxRes;
    int index;
    int group_priority1 = 5;
    int group_priority2 = 6;
    bcm_field_data_qualifier_t dqsma;
    bcm_field_data_qualifier_t dqsma_dt;
    uint8 dqMask[2], dqData[2];

    bcm_field_group_config_t_init(&grp1);
    bcm_field_group_config_t_init(&grp2);

    grp1.group = -1;
    grp2.group = -1;
 
    for (index = 0; index < 16; index++)
    {
        ent[index] = -1;
        dt_ent[index] = -1;
        ent_drop[index] = -1;
    }
 
    /*
     *  Set the cascaded key length to 16 bits
     */
    result = bcm_field_control_set(unit,
                                   bcmFieldControlCascadedKeyWidth,
                                   16 /* bits in cascaded key */);

    /* Create a Data qualifier for 16b including the Tunnel Id */
    if (BCM_E_NONE == result) 
   	{
        bcm_field_data_qualifier_t_init(&dqsma); 
        dqsma.offset_base = bcmFieldDataOffsetBasePacketStart;
        dqsma.offset = 2; /* 2 bytes to skip */
        dqsma.length = 2; /* 16 bits */
        result = bcm_field_data_qualifier_create(unit, &dqsma);
    }
 
    /*
     *  On PetraB / Arad, the second group in a cascade must be created first.
     *  This specific requirement may be masked in the future, but for right
     *  now, it is obligatory even at the BCM layer.
     */
    if (BCM_E_NONE == result)
    {
        BCM_FIELD_QSET_INIT(grp2.qset);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyCascadedKeyValue);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyL2SrcHit);
        BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyStageIngress);
        result = bcm_field_qset_data_qualifier_add(unit,
                                                   &(grp2.qset),
                                                   dqsma.qual_id);

        grp2.priority = group_priority2;
    }                            
    if (BCM_E_NONE == result)
    {
        BCM_FIELD_ASET_INIT(aset2);
        BCM_FIELD_ASET_ADD(aset2, bcmFieldActionDrop);
        BCM_FIELD_ASET_ADD(aset2, bcmFieldActionDoNotLearn);
        result = bcm_field_group_config_create(unit, &grp2);
    }
    if (BCM_E_NONE == result)
    {
        result = bcm_field_group_action_set(unit, grp2.group, aset2);
    }

    /*
     *  Now create the initial group 
     */
    if (BCM_E_NONE == result) 
    {
        bcm_field_data_qualifier_t_init(&dqsma_dt); 
        dqsma_dt.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES|BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;
        dqsma_dt.offset = 0; 
        dqsma_dt.length = 10; 
        dqsma_dt.qualifier = bcmFieldQualifyTunnelId; 
        result = bcm_field_data_qualifier_create(unit, &dqsma_dt);
    }
    if (BCM_E_NONE == result)
    {
        BCM_FIELD_QSET_INIT(grp1.qset);
        BCM_FIELD_QSET_ADD(grp1.qset, bcmFieldQualifyStageIngress);
        result = bcm_field_qset_data_qualifier_add(unit,
                                                   &(grp1.qset),
                                                   dqsma_dt.qual_id);

        grp1.priority = group_priority1;
    }
    if (BCM_E_NONE == result)
    {
        BCM_FIELD_ASET_INIT(aset1);
        BCM_FIELD_ASET_ADD(aset1, bcmFieldActionCascadedKeyValueSet);
        
    	grp1.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    	grp1.mode = bcmFieldGroupModeDirect;
        result = bcm_field_group_config_create(unit, &grp1);
    }

    if (BCM_E_NONE == result)
	{
        result = bcm_field_group_action_set(unit, grp1.group, aset1);
    }
	
    /* Add entries to the second group: when to drop */
    for (index = 0; index < 6; index++)
    {
        if (BCM_E_NONE == result)
        {
		    result = bcm_field_entry_create(unit, grp2.group, &(ent[index]));
		}
        if (BCM_E_NONE == result)
        {
            /* Bit = index is up */
            result = bcm_field_qualify_CascadedKeyValue(unit, ent[index], (1 << index), (1 << index));
        }
        if (BCM_E_NONE == result)
        {
            /* InPort = index >> 1 for the 3 first bits */
            result = bcm_field_qualify_InPort(unit, ent[index], (index >> 1), (~0));
        }
        if (BCM_E_NONE == result)
        {
            /* SA lookup not found to drop */
            result = bcm_field_qualify_L2SrcHit(unit, ent[index], 0, 0x1);
        }
        if (BCM_E_NONE == result)
        {
            /* Bit 10 of Tunnel Id up if needed */
            dqData[1] = 0x00;
            dqData[0] = 0x04 * (index % 2);
            dqMask[1] = 0x00;
            dqMask[0] = 0x04;
            result = bcm_field_qualify_data(unit,
                                            ent[index],
                                            dqsma.qual_id,
                                            &(dqData[0]),
                                            &(dqMask[0]),
                                            2 /* len here always in bytes */);
        }
        if (BCM_E_NONE == result)
        {
            result = bcm_field_action_add(unit, ent[index], bcmFieldActionDrop, 0, 0);
        }
        if (BCM_E_NONE == result)
        {
            result = bcm_field_action_add(unit, ent[index], bcmFieldActionDoNotLearn, 0, 0);
        }
        /* Drop until learnt - packet are dropped until the SA is learnt */
        if (BCM_E_NONE == result)
        {
		    result = bcm_field_entry_create(unit, grp2.group, &(ent_drop[index]));
        }
        if (BCM_E_NONE == result)
        {
            /* SA lookup not found to drop */
            result = bcm_field_qualify_L2SrcHit(unit, ent_drop[index], 0, 0x1);
        }
        if (BCM_E_NONE == result)
        {
            result = bcm_field_action_add(unit, ent_drop[index], bcmFieldActionDrop, 0, 0);
        }
    }
 
    if (BCM_E_NONE == result)
    {
        result = bcm_field_group_install(unit, grp2.group);
    }
 
    /*
     *  The ARM add entries to the 'first' group 
     */
    /*for (index = 0; index < 1; ++index) 
    {
        if (BCM_E_NONE == result)
        {
            result = bcm_field_entry_create(unit, grp1.group, &(dt_ent[index]));
        }
        if (BCM_E_NONE == result)
        {
            dqData[0] = 0x0;
            dqMask[0] = 0xFF;
            dqData[1] = 0x0;
            dqMask[1] = 0x0;
            result = bcm_field_qualify_data(unit,
                                      dt_ent[index],
                                      dqsma_dt.qual_id,
                                      &(dqData[0]),
                                      &(dqMask[0]),
                                      1);
        }
        if (BCM_E_NONE == result)
        {
            result = bcm_field_action_add(unit, dt_ent[index], bcmFieldActionCascadedKeyValueSet, 0, 0);
        }
    }

    if (BCM_E_NONE == result)
    {
        result = bcm_field_group_install(unit, grp1.group);
    }*/
    return result;
}
