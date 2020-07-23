/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
int program_set(int unit_loop)
{
  int rtn_code = 0;
  if(soc_property_get(unit_loop, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      rtn_code += bcm_field_presel_create_stage_id(unit_loop, bcmFieldStageIngress, 1);
      rtn_code += bcm_field_qualify_Stage(unit_loop, 1 | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS, bcmFieldStageIngress);
      rtn_code = bcm_field_qualify_HeaderFormat(unit_loop, 1 | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS, bcmFieldHeaderFormatIp4);
      if(BCM_E_NONE != rtn_code)
      {
          printf("Error in ipv4 program init\n");
      }

      rtn_code = bcm_field_presel_create_stage_id(unit_loop, bcmFieldStageIngress, 2);
      rtn_code += bcm_field_qualify_Stage(unit_loop, 2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
      rtn_code += bcm_field_qualify_HeaderFormat(unit_loop, 2 | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS, bcmFieldHeaderFormatIp4);
      rtn_code += bcm_field_qualify_VlanTranslationHit(unit_loop, 2 | BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS, 0x1, 0x1);
      if(BCM_E_NONE != rtn_code)
      {
          printf("Error ipv4 fec extend program init!\n");
      }
  } else {
      rtn_code += bcm_field_presel_create_id(unit_loop, 1);
      rtn_code += bcm_field_qualify_Stage(unit_loop, 1 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
      rtn_code = bcm_field_qualify_HeaderFormat(unit_loop, 1 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp4);
      if(BCM_E_NONE != rtn_code)
      {
          printf("Error in ipv4 program init\n");
      }

      rtn_code = bcm_field_presel_create_id(unit_loop, 2);
      rtn_code += bcm_field_qualify_Stage(unit_loop, 2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
      rtn_code += bcm_field_qualify_HeaderFormat(unit_loop, 2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp4);
      rtn_code += bcm_field_qualify_VlanTranslationHit(unit_loop, 2 | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
      if(BCM_E_NONE != rtn_code)
      {
          printf("Error ipv4 fec extend program init!\n");
      }
  }
  return rtn_code;
}




int roo_set(int unit)
{

  int grp1 = 10;
  int grp2 = 20;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  int group_priority = 0;
  int result = 0;
  bcm_field_entry_t ent;
  bcm_field_extraction_field_t ext[5];
  bcm_field_extraction_action_t extact;

  uint32 dq_ndx, fec_dq_length[2] = {12,4};
  uint32 fec_dq_offset[2] = {0,16};
  uint32 fec_nof_data_quals = 2;
  bcm_field_data_qualifier_t fec_data_qual[2];
  bcm_field_qualify_t fec_dq_qualifier[2] = {bcmFieldQualifyDstL3Egress, bcmFieldQualifyTrillEgressRbridge};
  bcm_field_presel_set_t presel;

  BCM_FIELD_PRESEL_INIT(presel);
  BCM_FIELD_PRESEL_ADD(presel, 2);


      /*First group to create FEC*/
      for (dq_ndx = 0; dq_ndx < fec_nof_data_quals; dq_ndx++) {
            bcm_field_data_qualifier_t_init(&fec_data_qual[dq_ndx]);
            fec_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
            fec_data_qual[dq_ndx].offset = fec_dq_offset[dq_ndx];   
            fec_data_qual[dq_ndx].qualifier = fec_dq_qualifier[dq_ndx];
            fec_data_qual[dq_ndx].length = fec_dq_length[dq_ndx];
            bcm_field_data_qualifier_create(unit, &fec_data_qual[dq_ndx]);   
      }
        
      BCM_FIELD_QSET_INIT(qset);
      BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        
      bcm_field_qset_data_qualifier_add(unit, &qset, fec_data_qual[0].qual_id);
      bcm_field_qset_data_qualifier_add(unit, &qset, fec_data_qual[1].qual_id);
        
      BCM_FIELD_ASET_INIT(aset);
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
      group_priority= 110 - grp1;   
      bcm_field_group_create_mode_id(unit, qset, group_priority,  bcmFieldGroupModeDirectExtraction, grp1);

      bcm_field_group_presel_set(unit, grp1, &presel);     
      bcm_field_group_action_set(unit, grp1, aset);
      bcm_field_entry_create(unit, grp1, &ent);
	  printf("group1 ent = %d\n", ent);
  

      /*Construct new FEC destination*/
      bcm_field_extraction_action_t_init(&extact);
      extact.action = bcmFieldActionRedirect;
      extact.bias = 0;

      bcm_field_extraction_field_t_init(&(ext[0]));
      ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext[0].qualifier = fec_data_qual[0].qual_id;
      ext[0].lsb = 0;
      ext[0].bits = 12;

      bcm_field_extraction_field_t_init(&(ext[1]));
      ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext[1].qualifier = fec_data_qual[1].qual_id;
      ext[1].lsb = 0;
      ext[1].bits = 4;

      bcm_field_extraction_field_t_init(&(ext[2]));
      ext[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
      ext[2].value = 0;
      ext[2].bits = 1;
                    
      bcm_field_extraction_field_t_init(&(ext[3]));
      ext[3].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
      ext[3].value = 1;
      ext[3].bits = 1;

      bcm_field_extraction_field_t_init(&(ext[4]));
      ext[4].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
      ext[4].value = 0;
      ext[4].bits = 1;
      bcm_field_direct_extraction_action_add(unit,
                                                   ent,
                                                   extact,
                                                   5 /* count */,
                                                   ext);

      result=bcm_field_group_install(unit, grp1);  
	  return result;

}

int cint_field_remove_de_main(int unit)
{
   int result = 0;

   if(BCM_E_NONE != program_set(unit))
   {
      printf("Error in program_set!\n");
   }
   if(BCM_E_NONE != roo_set(unit))
   {
      printf("Error in roo_set!\n");
   }
   return result;   
}

