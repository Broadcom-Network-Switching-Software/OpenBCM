/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * 
 * **************************************************************************************************************************************************
 * This CINT script is an example for followed usage
 *    Use PMF to redirect PPPoE protocol packets to CPU
 *        -- pppoe_protocol_fg()
 
 *    Use PMF to drop packet fail to PPPoE anti-spoofing 
 *        -- pppoe_protocol_pppoe_anti_spoofing_fg()
 
 *    Use PMF to drop packet fail to SIP4 anti-spoofing
 *        -- pppoe_sipv4_anti_spoofing_fg()
                                                  
 */
 
int pppoe_protocol_fg(int unit, int uni_port)
{
    int rv = 0, i = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry[3];
    bcm_field_entry_t eth_entry;
    bcm_field_data_qualifier_t data_qual;
    uint8 data[3][2] = {{0x80, 0x21},{0x80, 0x57}, {0xC0, 0x21}}; /* PPPoE header protocols */
    uint8 mask[2] = {0xff, 0xff};
    int sys_gport = 0;
    int dest_port  = 15;
    bcm_field_data_qualifier_t data_qual_ethType;                 /* EthType 0x8863: PPPoE protocol header */
    uint8 ethType_data[2] = {0x88, 0x63};                
    uint8 ethType_mask[2] = {0xff, 0xff};

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);

    bcm_field_data_qualifier_t_init(&data_qual_ethType);
    data_qual_ethType.offset_base = bcmFieldDataOffsetBaseL2Header;
    data_qual_ethType.offset = 16;                               /* Offset to L2 Ethertype */
    data_qual_ethType.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qual_ethType);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE ethernet Type\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual_ethType.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE ethernet Type\n");
        return rv;
    }

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header;
    data_qual.offset = 24;                                       /* Offset to PPPoE protocol */
    data_qual.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE protocol ID\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE protocol ID\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    grp.group = -1;
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_config_create for PPPoE protocol ID\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);            
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_action_set for PPPoE protocol ID\n");
        return rv;
    }

    /* PPPoE control packet: ethernet type = 0x8863 */
    rv = bcm_field_entry_create(unit, grp.group, &eth_entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_create for PPPoE control\n");
        return rv;
    }
    
    rv = bcm_field_qualify_InPort(unit, eth_entry, uni_port, 0xffffffff);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_InPort for PPPoE control\n");
        return rv;
    }

    rv = bcm_field_qualify_data(unit, eth_entry, data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_EtherType for PPPoE control packet\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, dest_port);
    rv = bcm_field_action_add(unit, eth_entry, bcmFieldActionRedirect, 0 , sys_gport);         /* configure discovery action redirect to port 15 */
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_action_add for PPPoE control\n");
        return rv;
    }
    
    rv = bcm_field_entry_install(unit, eth_entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_install for PPPoE protocol ID\n");
        return rv;
    }


    /* PPPoE session stage protocols - 0x8864 + 0x8021/8057/C021  */
    for (i = 0; i < 3; i++)
    {
        rv = bcm_field_entry_create(unit, grp.group, &entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_create for PPPoE protocol ID\n");
            return rv;
        }

        rv = bcm_field_qualify_InPort(unit, entry[i], uni_port, 0xffffffff);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_InPort for PPPoE protocol ID\n");
            return rv;
        }

        ethType_data[1] = 0x64;
        rv = bcm_field_qualify_data(unit, entry[i], data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_EtherType for PPPoE protocol packet\n");
            return rv;
        }

        rv = bcm_field_qualify_data(unit, entry[i], data_qual.qual_id, data[i], mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_data for PPPoE protocol ID\n");
            return rv;
        }

        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, dest_port);
        rv = bcm_field_action_add(unit, entry[i], bcmFieldActionRedirect, 0 , sys_gport);         /* configure session stage action redirect to port 15 */
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_action_add for PPPoE protocol ID\n");
            return rv;
        }

        rv = bcm_field_entry_install(unit, entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_install for PPPoE protocol ID\n");
            return rv;
        }
    }

    return rv;
}

int pppoe_protocol_pppoe_anti_spoofing_fg(int unit, int uni_port)
{
    int rv = 0, i = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry[2];
    bcm_field_data_qualifier_t data_qual;
    uint8 data[2][2] = {{0x00, 0x21}, {0x00, 0x57}};  /* allow only IPv4/IPv6 next protocol */
    uint8 mask[2] = {0xff, 0xff};
    bcm_field_data_qualifier_t data_qual_ethType;
    uint8 ethType_data[2] = {0x88, 0x64};             /* Allow only PPPoE EtherType */
    uint8 ethType_mask[2] = {0xff, 0xff};

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL2SrcHit);

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
    data_qual.offset = 2;
    data_qual.length = 2;
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE data packet\n");
        return rv;
    }

    /* Ethernet Type 0x8864 */
    bcm_field_data_qualifier_t_init(&data_qual_ethType);
    data_qual_ethType.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
    data_qual_ethType.offset = 10;
    data_qual_ethType.length = 2;
    data_qual_ethType.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
    rv = bcm_field_data_qualifier_create(unit, &data_qual_ethType);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE Ethernet Type\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual_ethType.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE Ethernet Type\n");
        return rv;
    }


    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    grp.group = -1;
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_config_create for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);            
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_action_set for PPPoE data packet\n");
        return rv;
    }

    /* PPPoE data packet  */
    for (i = 0; i < 2; i++)
    {
        rv = bcm_field_entry_create(unit, grp.group, &entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_create for PPPoE data packet\n");
            return rv;
        }

        rv = bcm_field_qualify_InPort(unit, entry[i], uni_port, 0xffffffff);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_InPort for PPPoE data packet\n");
            return rv;
        }

        rv = bcm_field_qualify_data(unit, entry[i], data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_data for PPPoE data packet\n");
            return rv;
        }

        rv = bcm_field_qualify_data(unit, entry[i], data_qual.qual_id, data[i], mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_data for PPPoE data packet\n");
            return rv;
        }

        rv = bcm_field_qualify_L2SrcHit(unit, entry[i], 0, 1);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_L2DestHit for PPPoE data packet\n");
            return rv;
        }

        rv = bcm_field_action_add(unit, entry[i], bcmFieldActionDrop, 0 , 0);         
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_action_add for PPPoE data packet\n");
            return rv;
        }

        rv = bcm_field_entry_install(unit, entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_install for PPPoE data packet\n");
            return rv;
        }
    }

    print grp.group;
    return rv;

}

int pppoe_data_sipv6_anti_spoofing_fg(int unit, int uni_port)
{
    int rv = 0, i = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry[1];
    bcm_field_data_qualifier_t data_qual;
    uint8 data[2] = {0x00, 0x57};
    uint8 mask[2] = {0xff, 0xff};
    bcm_field_data_qualifier_t data_qual_ethType;
    uint8 ethType_data[2] = {0x88, 0x64};
    uint8 ethType_mask[2] = {0xff, 0xff};

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyL3SrcRouteHit);

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
    data_qual.offset = 2;
    data_qual.length = 2;
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE data packet\n");
        return rv;
    }

    /* Ethernet Type */
    bcm_field_data_qualifier_t_init(&data_qual_ethType);
    data_qual_ethType.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
    data_qual_ethType.offset = 10;
    data_qual_ethType.length = 2;
    data_qual_ethType.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
    rv = bcm_field_data_qualifier_create(unit, &data_qual_ethType);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE Ethernet Type\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual_ethType.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE Ethernet Type\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    grp.group = -1;
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_config_create for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);            
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_action_set for PPPoE data packet\n");
        return rv;
    }

    /* PPPoE data packet  */
    i = 0;
    rv = bcm_field_entry_create(unit, grp.group, &entry[i]);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_create for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, entry[i], uni_port, 0xffffffff);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_InPort for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_qualify_data(unit, entry[i], data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_data for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_qualify_data(unit, entry[i], data_qual.qual_id, data[i], mask, 2);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_data for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_qualify_L3SrcRouteHit(unit, entry[i], 0, 1);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_L2DestHit for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_action_add(unit, entry[i], bcmFieldActionDrop, 0 , 0);         
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_action_add for PPPoE data packet\n");
        return rv;
    }

    rv = bcm_field_entry_install(unit, entry[i]);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_install for PPPoE data packet\n");
        return rv;
    }

    return rv;

}


bcm_field_presel_set_t psset_up_static;
int presel_id_up_static = 0;
/* Create a pselect for IP anti-spoofing static mode in upstream.
 * params:
 *    unit: device number
 *    in_pbmp: bitmap for PON port want to enable IP spoofing static mode.
 */
int pppoe_sipv4_anti_spoofing_static_pselect_set(/* in */int unit, 
                                                           /* in */bcm_pbmp_t in_pbmp)
{
    int result;
    int auxRes;
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int i;
    int port_num = get_local_port_number(unit);
    int presel_flags = 0;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id_up_static);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id_up_static);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id_up_static);
            return result;
        }
    }

    /* 
     * Set the preselector to be on the ports
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_ASSIGN(pbm, in_pbmp);
    for(i=0; i<port_num; i++)
    { 
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }

    result = bcm_field_qualify_Stage(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_Stage\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
        return result;
    }
    
    result = bcm_field_qualify_InPorts(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_InPorts\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static | presel_flags);
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id_up_static | BCM_FIELD_QUALIFY_PRESEL , bcmFieldForwardingTypeIp4Ucast);	
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType bcmFieldForwardingTypeIp4Ucast\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id_up_static);
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset_up_static);
    BCM_FIELD_PRESEL_ADD(psset_up_static, presel_id_up_static);

    return result;
}

bcm_field_group_config_t grp_tcam, grp_de, grp_cascaded;
bcm_field_entry_t ent_tcam, ent_de, ent_cascaded[2];
int is_cascaded = 1;

/* ***************************************************************** */
/* 
 * pon_anti_spoofing_compare_fg_set_a(): 
 *  
 * 1. Create the first field group for compare in the FP: 
 *    TCAM Database of 80 bits.    
 * 2. Set the QSET of to be user defined.    
 */
/* ***************************************************************** */
int pppoe_sipv4_anti_spoofing_compare_fg_set_a(int unit,bcm_field_presel_set_t psset,int group_priority) 
{
  int result;
  bcm_field_aset_t aset, aset_cascaded;
  bcm_field_data_qualifier_t data_qual;
  int index = 0;
  bcm_field_data_qualifier_t dq_pppoe_protocol;
  uint8 pppoe_data_protocol[2]={0x00, 0x21};
  uint8 pppoe_data_protocol_mask[2]={0xff, 0xff};
  bcm_field_data_qualifier_t data_qual_ethType;
  uint8 ethType_data[2] = {0x88, 0x64};
  uint8 ethType_mask[2] = {0xff, 0xff};

  /* 
   * Define the first Field Group (LSB bits of key 7).
   */
  bcm_field_group_config_t_init(&grp_tcam);
  grp_tcam.group = (-1);
  grp_tcam.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
      BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
      BCM_FIELD_GROUP_CREATE_IS_EQUAL;
  grp_tcam.mode = bcmFieldGroupModeAuto;
  grp_tcam.priority = group_priority;
  grp_tcam.preselset = psset;

  /* 
   * Define the QSET. 
   * No need to use IsEqual as qualifier for this field group.
   */
  BCM_FIELD_QSET_INIT(grp_tcam.qset);
  BCM_FIELD_QSET_ADD(grp_tcam.qset, bcmFieldQualifyStageIngress);

  if (is_cascaded) 
  {
      /* Set cascaded field size */
      result = bcm_field_control_set(unit,
                                     bcmFieldControlCascadedKeyWidth,
                                     16 /* bits in cascaded key */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_control_set\n");
          return result;
      }

      /* Add cascaded qualifier to field group */
      BCM_FIELD_QSET_ADD(grp_tcam.qset, bcmFieldQualifyCascadedKeyValue);

     /* Pad with 4 zeros so that next field will start at bit 20
         * This must be done before creating hte next field, since 
         * both are located in the key according to the order of 
         * their creation.
         */
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 0;  
      data_qual.qualifier = bcmFieldQualifyConstantZero; 
      data_qual.length = 4; 
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /* 1st compared key: IN_LIF
 */
  bcm_field_data_qualifier_t_init(&data_qual); 
  data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
      BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;  
  data_qual.offset = 0;  
  data_qual.qualifier = bcmFieldQualifyInVPort; 
  data_qual.length = 16; 
  data_qual.stage = bcmFieldStageIngress;
  result = bcm_field_data_qualifier_create(unit, &data_qual);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }

  result = bcm_field_qset_data_qualifier_add(unit, &grp_tcam.qset, data_qual.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }

  /*
   *  Define the ASET - use counter 0.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

  /*  Create the Field group with type TCAM */
  result = bcm_field_group_config_create(unit, &grp_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create for grp_tcam.group %d\n", grp_tcam.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_tcam.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam.group);
    return result;
  }

  if (is_cascaded) 
  {
      /* Create initial field group if cascaded */
      bcm_field_group_config_t_init(&grp_cascaded);
      grp_cascaded.group = (-1);
      grp_cascaded.flags |= (BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET);
      grp_cascaded.mode = bcmFieldGroupModeAuto;
      grp_cascaded.priority = BCM_FIELD_GROUP_PRIO_ANY;
      grp_cascaded.preselset = psset;

      bcm_field_data_qualifier_t_init(&dq_pppoe_protocol);
      dq_pppoe_protocol.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
      dq_pppoe_protocol.offset = 2;
      dq_pppoe_protocol.length = 2;
      dq_pppoe_protocol.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
      result = bcm_field_data_qualifier_create(unit, &dq_pppoe_protocol);
      if (BCM_E_NONE != result)
      {
          printf("Error, bcm_field_data_qualifier_create for PPPoE data packet\n");
          return result;
      }

      /* Ethernet Type */
      bcm_field_data_qualifier_t_init(&data_qual_ethType);
      data_qual_ethType.offset_base = bcmFieldDataOffsetBaseForwardingHeader;
      data_qual_ethType.offset = 10;
      data_qual_ethType.length = 2;
      data_qual_ethType.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
      result = bcm_field_data_qualifier_create(unit, &data_qual_ethType);
      if (BCM_E_NONE != result)
      {
          printf("Error, bcm_field_data_qualifier_create for PPPoE Ethernet Type\n");
          return result;
      }

      BCM_FIELD_QSET_INIT(grp_cascaded.qset);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyStageIngress);
      BCM_FIELD_QSET_ADD(grp_cascaded.qset, bcmFieldQualifyAppType);
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &(grp_cascaded.qset), dq_pppoe_protocol.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_cascaded.qset, data_qual_ethType.qual_id);
      if (BCM_E_NONE != result)
      {
          printf("Error, bcm_field_qset_data_qualifier_add for PPPoE Ethernet Type\n");
          return result;
      }

      result = bcm_field_group_config_create(unit, &grp_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_config_create for grp_cascaded.group %d\n", grp_cascaded.group);
          return result;
      }

      BCM_FIELD_ASET_INIT(aset_cascaded);
      BCM_FIELD_ASET_ADD(aset_cascaded, bcmFieldActionCascadedKeyValueSet);

      result = bcm_field_group_action_set(unit, grp_cascaded.group, aset_cascaded);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_action_set for group %d\n", grp_cascaded.group);
          return result;
      }
  }

  result = bcm_field_entry_create(unit, grp_tcam.group, &ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  if (is_cascaded) 
  {
      result = bcm_field_qualify_CascadedKeyValue(unit, ent_tcam, 1, 1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_CascadedKeyValue\n");
          return result;
      }
  }

  result = bcm_field_entry_install(unit, ent_tcam);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }

  if (is_cascaded) 
  {
      index = 0;
      result = bcm_field_entry_create(unit, grp_cascaded.group, &(ent_cascaded[index]));
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

	  result = bcm_field_qualify_AppType(unit, ent_cascaded[index], bcmFieldAppTypeIp4SrcBind);
	  if (BCM_E_NONE != result) {
		  printf("Error in bcm_field_qualify_InPort\n");
		  return result;
	  }
      result = bcm_field_qualify_data(unit,
                                      ent_cascaded[index],
                                      dq_pppoe_protocol.qual_id,
                                      pppoe_data_protocol,
                                      pppoe_data_protocol_mask,
                                      2 /* length here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data\n");
          return result;
      }

      result = bcm_field_qualify_data(unit, ent_cascaded[index], data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
      if (BCM_E_NONE != result)
      {
          printf("Error, bcm_field_qualify_data for PPPoE data packet\n");
          return result;
      }

      result = bcm_field_action_add(unit, ent_cascaded[index], bcmFieldActionCascadedKeyValueSet, 1, 1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_action_add\n");
          return result;
      }

      result = bcm_field_entry_install(unit, ent_cascaded[index]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_install\n");
          return result;
      }  
  }
 
  return result;

} /* compare_field_group_set_a */


/* ***************************************************************** */
/* 
 * pon_anti_spoofing_compare_fg_set_b(): 
 *  
 * 1. Create Second field group for compare in the FP: 
 *    Direct Extraction Database of 80 bits. 
 * 2. Set the QSET of to be user defined + bcmFieldQualifyIsEqualValue.
 * 3. Set the action of the Direct Extraction database to 
 *    be according to result of the compare operation
 *    (located in the 5 MSB bits of Key 7). 
 */
/* ***************************************************************** */
int pppoe_sipv4_anti_spoofing_compare_fg_set_b(int unit,bcm_field_presel_set_t psset,int group_priority)
{
  int result;
  int dq_idx;
  uint8 data_mask;
  bcm_field_aset_t aset;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t is_equal[2];
  bcm_field_data_qualifier_t data_qual;
  
  /* 
   * Define the second Field Group (MSB bits of key 7).
   */
  bcm_field_group_config_t_init(&grp_de);
  grp_de.group = (-1);
  grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
      BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |
      BCM_FIELD_GROUP_CREATE_IS_EQUAL;
  grp_de.mode = bcmFieldGroupModeDirectExtraction;
  grp_de.priority = group_priority;
  grp_de.preselset = psset;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp_de.qset);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyIsEqualValue);

 /* If cascaded the compare key is as follows:
   * 80b LSB : < 20b = 0, 20b = InLIF,      20b = PPPoE data packet & bcmFieldAppTypeIp4SrcBind > 
   * 80b MSB : < 20b = 0, 20b = LPM result, 20b = 0> 
   */
  if (is_cascaded) 
  {
    for (dq_idx = 0; dq_idx < 2; dq_idx++) 
    {
       bcm_field_data_qualifier_t_init(&data_qual); 
       data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
           BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
       data_qual.offset = 0;
       if (dq_idx == 0) {
           data_qual.qualifier = bcmFieldQualifyConstantZero; 
           data_qual.length = 10; 
       } else {
           data_qual.qualifier = bcmFieldQualifyConstantZero; 
           data_qual.length = 10; 
       }
       data_qual.stage = bcmFieldStageIngress;
       result = bcm_field_data_qualifier_create(unit, &data_qual);
       if (BCM_E_NONE != result) {
           printf("Error in bcm_field_data_qualifier_create\n");
           return result;
       }

       result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual.qual_id);
       if (BCM_E_NONE != result) {
           printf("Error in bcm_field_qset_data_qualifier_add\n");
           return result;
       }
    }
  }

  for (dq_idx = 0; dq_idx < 2; dq_idx++) 
  {
      bcm_field_data_qualifier_t_init(&data_qual); 
      data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | 
          BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual.offset = 0;
      if (dq_idx == 0) {
          data_qual.qualifier = bcmFieldQualifyL3SrcRouteValue; 
          data_qual.length = 15; 
      } else {
          data_qual.qualifier = bcmFieldQualifyConstantZero; 
          data_qual.length = 5; 
      }
      data_qual.stage = bcmFieldStageIngress;
      result = bcm_field_data_qualifier_create(unit, &data_qual);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }

      result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual.qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }
 
  /*
   *  Define the ASET.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

  /*  Create the Field group with type Direct Extraction */
  result = bcm_field_group_config_create(unit, &grp_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create for group %d\n", grp_de.group);
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_de.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_de.group);
      return result;
  }

  /* 
   * Create the Direct Extraction entry:
   *  1. create the entry
   *  2. Construct the action:
   *     bit 0 = action valid 
   *     bits 3:0 = IsEqual
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* 
   * IsEqualValue Qualifier: 
   *    IsEqualValue qualifier has 4 bits, each indicated 
   *    match of 20 bits in the compare key. The LSB bit 
   *    corresponds to the LSB 20 compared bits, and so on. 
   * In this example:
   *    Look at different bits in
   *    order to use compare result for Drop  
   */
  data_mask = is_cascaded ? 3 : 1;
  result = bcm_field_qualify_IsEqualValue(unit, ent_de, 0, data_mask);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_IsEqualValue\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&is_equal[0]);
  bcm_field_extraction_field_t_init(&is_equal[1]);

  extract.action = bcmFieldActionDrop;
  extract.bias = 0;

  /* First extraction structure indicates action is valid */
  is_equal[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
  is_equal[0].bits  = 1;
  is_equal[0].value = 1;
  is_equal[0].qualifier = bcmFieldQualifyConstantOne;

  if (!is_device_or_above(unit,JERICHO_PLUS)) {
      /* second extraction structure indicates to use IsEqual qualifier */
      is_equal[1].flags = 0;
      is_equal[1].bits  = 4;
      is_equal[1].lsb   = 0;
      is_equal[1].qualifier = bcmFieldQualifyIsEqualValue;
  }

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent_de,
                                                  extract,
                                                  is_device_or_above(unit,JERICHO_PLUS)? 1 : 2,
                                                  &is_equal);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  } 
  
  return result;

}


int pppoe_sipv4_anti_spoofing_fg(int unit, int uni_port)
{
    int result;
    int group_priority_a = BCM_FIELD_GROUP_PRIO_ANY,
        group_priority_b = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_group_t 
        group_a = 2,
        group_b = 3;
    bcm_pbmp_t in_pbmp;
    
    BCM_PBMP_CLEAR(in_pbmp);
    BCM_PBMP_PORT_ADD(in_pbmp, uni_port);
    
    result = pppoe_sipv4_anti_spoofing_static_pselect_set(unit, in_pbmp);
    if (BCM_E_NONE != result)
    {
        bcm_field_presel_destroy(unit, presel_id_up_static);
        printf("Error in pppoe_sipv4_anti_spoofing_static_pselect_set\n");
        return result;
    }
    
    /* Create LSB field group  for IPv4 static in upstream */
    result = pppoe_sipv4_anti_spoofing_compare_fg_set_a(
                  unit, 
                  psset_up_static,
                  group_priority_a
              );
    if (BCM_E_NONE != result) {
        printf("Error in pppoe_sipv4_anti_spoofing_compare_fg_set_a for IPv4 static in upstream\n");
        return result;
    }
    /* Create MSB field group for IPv4 static in upstream */
    result = pppoe_sipv4_anti_spoofing_compare_fg_set_b(
                  unit, 
                  psset_up_static,
                  group_priority_b
             );
    if (BCM_E_NONE != result) {
        printf("Error in pppoe_sipv4_anti_spoofing_compare_fg_set_b for IPv4 static in upstream\n");
        return result;
    }
    
    return result;


}

/*
pppoe_vxlan_example()
1).pppoe control packet redriect to vxlan tunnel.
2).pppoe oontrol packet Over vxlan redriect to pppoe port.

config add  bcm886xx_ip4_tunnel_termination_mode.0=0
config add  bcm886xx_vxlan_enable.0=1
config add  bcm886xx_ether_ip_enable.0=0
config add  bcm886xx_vxlan_tunnel_lookup_mode.0=2
config add  tm_port_header_type_in_41=ETH
config add  tm_port_header_type_out_41=ETH
config add  ucode_port_41=RCY.1:core_1.41
config add  PPPoE_mode=1
tr 141

cint    ../../../../src/./examples/sand/utility/cint_sand_utils_global.c
cint    ../../../../src/./examples/sand/utility/cint_sand_utils_global.c
cint    ../../../../src/./examples/sand/utility/cint_sand_utils_vlan.c
cint    ../../../../src/./examples/sand/utility/cint_sand_utils_vxlan.c
cint    ../../../../src/./examples/sand/utility/cint_sand_utils_l3.c
cint    ../../../../src/./examples/dpp/cint_port_tpid.c
cint    ../../../../src/./examples/dpp/cint_advanced_vlan_translation_mode.c
cint    ../../../../src/./examples/sand/cint_ip_route_basic.c
cint    ../../../../src/./examples/sand/cint_sand_ip_tunnel.c
cint    ../../../../src/./examples/sand/cint_sand_vxlan.c
cint    ../../../../src/./examples/dpp/cint_pmf_2pass_snoop.c
cint    ../../../../src/./examples/dpp/cint_field_pppoe.c

cint
pppoe_vxlan_example(0,200,201,202,41);
exit;

vlxan -> pppoe
pppoe o vxlan packet will Redirect to PPPoE, 0x8863 Or (0x8864 + 0x8021/0x8057/0xc021)
tx 1  PtchSRCport=201 DATA="000c000200000000070001008100000a080045000057000000008011d955a00f1011a10f1011555555550043000008000000001388000011000000110000070001008100001191000012886311005555002e00214508002e000040004006b8bd6fffff017fffff033344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5f5f5f5f5f5f5f"
tx 1  PtchSRCport=201 DATA="000c000200000000070001008100000a080045000057000000008011d955a00f1011a10f1011555555550043000008000000001388000011000000110000070001008100001191000012886411005555002e8021ff08002e000040004006b8bd6fffff017fffff023344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5"
tx 1  PtchSRCport=201 DATA="000c000200000000070001008100000a080045000057000000008011d955a00f1011a10f1011555555550043000008000000001388000011000000110000070001008100001191000012886411005555002e8057ff08002e000040004006b8bd6fffff017fffff023344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5"
tx 1  PtchSRCport=201 DATA="000c000200000000070001008100000a080045000057000000008011d955a00f1011a10f1011555555550043000008000000001388000011000000110000070001008100001191000012886411005555002eC021ff08002e000040004006b8bd6fffff017fffff023344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5"

pppoe -> vxlan
PPPoE Session control 0x8863 Or (0x8864 + 0x8021/0x8057/0xc021) -- Packet will be Redirect to vlxan tunnle
tx 1  PtchSRCport=202 DATA="0000070001000011000000118100001191000012886311005555002e00214508002e000040004006b8bd6fffff017fffff033344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5f5f5f5f5f5f5f"
tx 1  PtchSRCport=202 DATA="0000070001000011000000118100001191000012886411005555002e8021ff08002e000040004006b8bd6fffff017fffff023344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5"
tx 1  PtchSRCport=202 DATA="0000070001000011000000118100001191000012886411005555002e8057ff08002e000040004006b8bd6fffff017fffff023344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5"
tx 1  PtchSRCport=202 DATA="0000070001000011000000118100001191000012886411005555002eC021ff08002e000040004006b8bd6fffff017fffff023344556600a1a2a300b1b2b3500f01f57eb00000f5f5f5f5f5f5f5f5f5"
*/
int pppoe_vxlan_example(int unit, int access_port, int provider_port, int pppoe_port, int rcy_port)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = vxlan_example(unit, access_port, provider_port,0);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_example \n");
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_EGRESS_DIST);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    rv = vxlan_to_pppoe(unit, provider_port,rcy_port);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_to_pppoe \n");
        return rv;
    }
    rv = pppoe_to_vxlan(unit, pppoe_port, ip_tunnel_basic_info.encap_fec_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, pppoe_to_vxlan\n");
        return rv;
    }
    return rv;
}

/*
 * Use PMF to redirect vxlan protocol packets to recycle port.
 */
int vxlan_to_pppoe(int unit, int vxlan_port, int rcy_port)
{
    int rv = 0, i = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry[3];
    bcm_field_entry_t eth_entry;
    bcm_field_data_qualifier_t data_qual;
    uint8 data[3][2] = {{0x80, 0x21},{0x80, 0x57}, {0xC0, 0x21}}; /* PPPoE header protocols */
    uint8 mask[2] = {0xff, 0xff};
    bcm_port_t port;
    bcm_field_data_qualifier_t data_qual_ethType;                 /* EthType 0x8863: PPPoE protocol header */
    uint8 ethType_data[2] = {0x88, 0x63};
    uint8 ethType_mask[2] = {0xff, 0xff};

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);

    bcm_field_data_qualifier_t_init(&data_qual_ethType);
    data_qual_ethType.offset_base = bcmFieldDataOffsetBaseSecondHeader;
    data_qual_ethType.offset = 20;                               /* Offset to L2 Ethertype */
    data_qual_ethType.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qual_ethType);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE ethernet Type\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual_ethType.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE ethernet Type\n");
        return rv;
    }

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.offset_base = bcmFieldDataOffsetBaseSecondHeader;
    data_qual.offset = 28;                                       /* Offset to PPPoE protocol */
    data_qual.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE protocol ID\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE protocol ID\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    grp.group = -1;
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_config_create for PPPoE protocol ID\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_action_set for PPPoE protocol ID\n");
        return rv;
    }

    /* PPPoE control packet: ethernet type = 0x8863 */
    rv = bcm_field_entry_create(unit, grp.group, &eth_entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_create for PPPoE control\n");
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, eth_entry, vxlan_port, 0xffffffff);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_InPort for PPPoE control\n");
        return rv;
    }

    rv = bcm_field_qualify_data(unit, eth_entry, data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_EtherType for PPPoE control packet\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(port, rcy_port);
    rv = bcm_field_action_add(unit, eth_entry, bcmFieldActionRedirect, 0 , port);         /* configure discovery action redirect to port 15 */
    if (rv != BCM_E_NONE)
    {
        printf("ERROR in bcm_field_action_add $rv %d\n", eth_entry);
        return rv;
    }

    rv = bcm_field_entry_install(unit, eth_entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_install for PPPoE protocol ID\n");
        return rv;
    }


    /* PPPoE session stage protocols - 0x8864 + 0x8021/8057/C021  */
    for (i = 0; i < 3; i++)
    {
        rv = bcm_field_entry_create(unit, grp.group, &entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_create for PPPoE protocol ID\n");
            return rv;
        }

        ethType_data[1] = 0x64;
        rv = bcm_field_qualify_data(unit, entry[i], data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_EtherType for PPPoE protocol packet\n");
            return rv;
        }

        rv = bcm_field_qualify_data(unit, entry[i], data_qual.qual_id, data[i], mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_data for PPPoE protocol ID\n");
            return rv;
        }

        BCM_GPORT_LOCAL_SET(port, rcy_port);
        rv = bcm_field_action_add(unit, entry[i], bcmFieldActionRedirect, 0 , port);         /* configure discovery action redirect to port 15 */
        if (rv != BCM_E_NONE)
        {
            printf("ERROR in bcm_field_action_add $rv %d\n", entry[i]);
            return rv;
        }

        rv = bcm_field_entry_install(unit, entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_install for PPPoE protocol ID\n");
            return rv;
        }
    }
    return rv;
}


/*
 *  Use PMF to redirect PPPoE protocol packet to vlxan tunnle.
 */

int pppoe_to_vxlan(int unit, int pppoe_port, int vxlan_encap_fec_id)
{
    int rv = 0, i = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry[3];
    bcm_field_entry_t eth_entry;
    bcm_field_data_qualifier_t data_qual;
    uint8 data[3][2] = {{0x80, 0x21},{0x80, 0x57}, {0xC0, 0x21}}; /* PPPoE header protocols */
    uint8 mask[2] = {0xff, 0xff};
    bcm_gport_t fec_dest;
    bcm_field_data_qualifier_t data_qual_ethType;                 /* EthType 0x8863: PPPoE protocol header */
    uint8 ethType_data[2] = {0x88, 0x63};
    uint8 ethType_mask[2] = {0xff, 0xff};

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInPort);

    bcm_field_data_qualifier_t_init(&data_qual_ethType);
    data_qual_ethType.offset_base = bcmFieldDataOffsetBaseL2Header;
    data_qual_ethType.offset = 20;                               /* Offset to L2 Ethertype */
    data_qual_ethType.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qual_ethType);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE ethernet Type\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual_ethType.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE ethernet Type\n");
        return rv;
    }

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header;
    data_qual.offset = 28;                                       /* Offset to PPPoE protocol */
    data_qual.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_data_qualifier_create for PPPoE protocol ID\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qset_data_qualifier_add for PPPoE protocol ID\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    grp.group = -1;
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_config_create for PPPoE protocol ID\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_group_action_set for PPPoE protocol ID\n");
        return rv;
    }

    /* PPPoE control packet: ethernet type = 0x8863 */
    rv = bcm_field_entry_create(unit, grp.group, &eth_entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_create for PPPoE control\n");
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, eth_entry, pppoe_port, 0xffffffff);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_InPort for PPPoE control\n");
        return rv;
    }

    rv = bcm_field_qualify_data(unit, eth_entry, data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_qualify_EtherType for PPPoE control packet\n");
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(fec_dest, vxlan_encap_fec_id & 0xFFFF);
    rv = bcm_field_action_add(unit, eth_entry, bcmFieldActionRedirect, 0 , fec_dest);         /* configure discovery action redirect to port 15 */
    if (rv != BCM_E_NONE)
    {
        printf("ERROR in bcm_field_action_add $rv %d\n", eth_entry);
        return rv;
    }

    rv = bcm_field_entry_install(unit, eth_entry);
    if (BCM_E_NONE != rv)
    {
        printf("Error, bcm_field_entry_install for PPPoE protocol ID\n");
        return rv;
    }

    /* PPPoE session stage protocols - 0x8864 + 0x8021/8057/C021  */
    for (i = 0; i < 3; i++)
    {
        rv = bcm_field_entry_create(unit, grp.group, &entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_create for PPPoE protocol ID\n");
            return rv;
        }

        rv = bcm_field_qualify_InPort(unit, entry[i], pppoe_port, 0xffffffff);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_InPort for PPPoE protocol ID\n");
            return rv;
        }

        ethType_data[1] = 0x64;
        rv = bcm_field_qualify_data(unit, entry[i], data_qual_ethType.qual_id, ethType_data, ethType_mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_EtherType for PPPoE protocol packet\n");
            return rv;
        }

        rv = bcm_field_qualify_data(unit, entry[i], data_qual.qual_id, data[i], mask, 2);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_qualify_data for PPPoE protocol ID\n");
            return rv;
        }

        BCM_GPORT_FORWARD_PORT_SET(fec_dest, vxlan_encap_fec_id & 0xFFFF);
        rv = bcm_field_action_add(unit, entry[i], bcmFieldActionRedirect, 0 , fec_dest);         /* configure discovery action redirect to port 15 */
        if (rv != BCM_E_NONE)
        {
            printf("ERROR in bcm_field_action_add $rv %d\n", entry[i]);
            return rv;
        }

        rv = bcm_field_entry_install(unit, entry[i]);
        if (BCM_E_NONE != rv)
        {
            printf("Error, bcm_field_entry_install for PPPoE protocol ID\n");
            return rv;
        }
    }
    return rv;
}

