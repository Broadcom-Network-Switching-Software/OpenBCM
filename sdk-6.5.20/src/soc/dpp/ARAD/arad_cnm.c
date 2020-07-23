/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/mcm/memregs.h>

#include <soc/mem.h>

#if defined(BCM_88650_A0)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_CNM


#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_cnm.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>


















uint32
  arad_cnm_cp_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_CNM_CP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val=0, i,
    sampling_mode = 0x0;
    soc_field_t field1, field2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_SET_UNSAFE);

  if (!SOC_IS_QAX(unit))
  {
      field1 = CNM_DUNE_HEADER_FORMATf;
      field2 = CNM_NON_DUNE_FORMAT_MODEf;
      if (SOC_IS_JERICHO(unit)) {
         field1 = CNM_FULL_MODE_ENABLEf;
         field2 = CNM_NON_FULL_MODE_FORMATf;
      }

     if(info->pkt_gen_mode == SOC_TMC_CNM_GEN_MODE_DUNE_PP)
     {
         SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, field1,  0x1));
     }
     else
     {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, field1,  0x0));

        switch(info->pkt_gen_mode)
        {
          case SOC_TMC_CNM_GEN_MODE_EXT_PP:
            fld_val = 0x0;
            break;
          case SOC_TMC_CNM_GEN_MODE_SAMPLING:
            fld_val = 0x1;
            sampling_mode = 0x1;
            break;
          case SOC_TMC_CNM_GEN_MODE_HIGIG:
            fld_val = 0x2;
            break;
          default:
          SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CNM_GEN_MODE, 30, exit);
        }

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, field2,  fld_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, CNM_SMP_MODE_ENf,  sampling_mode));
     }  
  }  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, CNM_ENABLEf,  info->is_cp_enabled));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_EXTERNAL_PP_SAMPLING_DATA_SIZEr, REG_PORT_ANY, 0, CNM_NON_DUNE_SAMP_DATA_SIZEf,  128));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_Q_SIGf,  3));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMH_LB_EXTr, REG_PORT_ANY, 0, CNM_LB_EXT_USE_ORIG_KEYf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMH_DSP_EXTr, REG_PORT_ANY, 0, CNM_DSP_EXT_USE_ORIG_DSPf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMH_STACK_EXTr, REG_PORT_ANY, 0, CNM_STACK_EXT_USE_ORIG_BMAPf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PPH_VSIr, REG_PORT_ANY, 0, CNM_PPH_VSI_USE_ORIG_VSIf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PPH_INLIFr, REG_PORT_ANY, 0, CNM_PPH_INLIF_USE_ORIG_INLIFf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ACTION_PROFILE_3r, SOC_CORE_ALL, 0, ACTION_PROFILE_CNM_INTERCEPTf,  0x8));

  for(i=0 ; i<256 ; i++) {
      res = WRITE_IPT_ITM_TO_OTM_MAPm(unit, MEM_BLOCK_ANY, i, &i);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 200+i, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_set_unsafe()",info->pkt_gen_mode,0);
}

uint32
  arad_cnm_cp_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT ARAD_CNM_CP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
    soc_field_t field1, field2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_GET_UNSAFE);

   if (!SOC_IS_QAX(unit))
   {
      field1 = CNM_DUNE_HEADER_FORMATf;
      field2 = CNM_NON_DUNE_FORMAT_MODEf;
      if (SOC_IS_JERICHO(unit)) {
         field1 = CNM_FULL_MODE_ENABLEf;
         field2 = CNM_NON_FULL_MODE_FORMATf;
       }

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, field1, &fld_val));

      if(fld_val == 0x1)
      {
         info->pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_DUNE_PP;
      }
      else
      {
         SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, field2, &fld_val));
         switch(fld_val)
         {
           case 0x0:
              info->pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_EXT_PP;
              break;
           case 0x1:
              info->pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_SAMPLING;
              break;
           case 0x2:
              info->pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_HIGIG;
              break;
           default:
              info->pkt_gen_mode = SOC_TMC_CNM_NOF_GEN_MODES;
         }
      }
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, CNM_ENABLEf, &fld_val));
  info->is_cp_enabled = (fld_val ? 1 : 0);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_get_unsafe()",0,0);
}

uint32
  arad_cnm_cp_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_CNM_CP_INFO                *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_SET_VERIFY);

  if( info->pkt_gen_mode != SOC_TMC_CNM_GEN_MODE_EXT_PP &&
      info->pkt_gen_mode != SOC_TMC_CNM_GEN_MODE_SAMPLING &&
      info->pkt_gen_mode != SOC_TMC_CNM_GEN_MODE_DUNE_PP &&
      info->pkt_gen_mode != SOC_TMC_CNM_GEN_MODE_HIGIG)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CNM_GEN_MODE, 10, exit);
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_set_verify()",0,0);
}

uint32
  arad_cnm_queue_mapping_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_Q_MAPPING_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_QUEUE_MAPPING_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CP_QUEUES_RANGE_0r, REG_PORT_ANY, 0, CP_QNUM_LOWf,  info->cp_queue_low));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CP_QUEUES_RANGE_1r, REG_PORT_ANY, 0, CP_QNUM_HIGHf,  info->cp_queue_high));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, CP_EIGHT_QS_SET_MODEf,  (info->q_set == SOC_TMC_CNM_Q_SET_8_CPS)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, CP_QNUM_ODD_SELf,  info->odd_queues_select));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_queue_mapping_set_unsafe()",0,0);
}

uint32
  arad_cnm_queue_mapping_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_Q_MAPPING_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_QUEUE_MAPPING_GET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CP_QUEUES_RANGE_0r, REG_PORT_ANY, 0, CP_QNUM_LOWf, &info->cp_queue_low));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CP_QUEUES_RANGE_1r, REG_PORT_ANY, 0, CP_QNUM_HIGHf, &info->cp_queue_high));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, CP_EIGHT_QS_SET_MODEf, &info->q_set));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, CP_QNUM_ODD_SELf, (uint32*)&info->odd_queues_select));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_queue_mapping_get_unsafe()",0,0);
}

uint32
  arad_cnm_queue_mapping_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_Q_MAPPING_INFO             *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_QUEUE_MAPPING_SET_VERIFY);

  if(info->cp_queue_high > info->cp_queue_low)
  {
    if((info->cp_queue_high - info->cp_queue_low + 1) > ARAD_CNM_MAX_CP_QUEUES)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CP_QUEUE_RANGE, 10, exit);
    }
  }

  if( info->q_set != SOC_TMC_CNM_Q_SET_8_CPS &&
      info->q_set != SOC_TMC_CNM_Q_SET_4_CPS)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CP_QUEUE_SET, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_queue_mapping_set_verify()",0,0);
}



uint32
  arad_cnm_cpq_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  ARAD_CNM_CPQ_INFO                   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_IQM_CNM_DS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CPQ_SET_UNSAFE);

  entry_offset = cpq_ndx;

  res = arad_iqm_cnm_ds_tbl_get_unsafe(
            unit,
            entry_offset,
            &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  data.cp_enable_1 = info->is_cp_enabled;
  data.cp_profile_1 = info->profile;
  data.cp_id = cpq_ndx;

  res = arad_iqm_cnm_ds_tbl_set_unsafe(
            unit,
            entry_offset,
            &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cpq_set_unsafe()",0,0);
}

uint32
  arad_cnm_cpq_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_OUT ARAD_CNM_CPQ_INFO                   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  ARAD_IQM_CNM_DS_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CPQ_GET_UNSAFE);

  
  entry_offset = cpq_ndx;
  
  res = arad_iqm_cnm_ds_tbl_get_unsafe(
            unit,
            entry_offset,
            &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->is_cp_enabled = data.cp_enable_1;
  info->profile = data.cp_profile_1;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cpq_get_unsafe()",0,0);
}

uint32
  arad_cnm_cpq_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  ARAD_CNM_CPQ_INFO                   *info
  )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CPQ_VERIFY);

  
  if(cpq_ndx >= ARAD_CNM_MAX_CP_QUEUES)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CP_QUEUE_INDEX, 10, exit);
  }

  
  if(info->profile >= ARAD_CNM_NOF_PROFILES)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CP_PROFILE_INDEX, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cpq_verify()",0,0);
}

uint32
  arad_cnm_cp_profile_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_IN  ARAD_CNM_CP_PROFILE_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    data;
  uint32
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_PROFILE_SET_UNSAFE);

  data.cp_qeq = info->q_eq;
  data.cp_w = info->cpw_power;
  data.cp_fb_max_val = info->max_neg_fb_value;
  data.cp_quant_div= info->quant_div;
  data.cp_fixed_sample_base = (info->is_sampling_th_random == 0 ? 1 : 0);

  for(i = 0; i < ARAD_CNM_NOF_SAMPLING_BASES; i++)
  {
    data.cp_sample_base[i] = info->sampling_base[i];
  }

  res = arad_iqm_cnm_profile_tbl_set_unsafe(
            unit,
            profile_ndx,
            &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_profile_set_unsafe()",0,0);
}

uint32
  arad_cnm_cp_profile_get_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_OUT ARAD_CNM_CP_PROFILE_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    data;
  uint32
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_PROFILE_GET_UNSAFE);

  res = arad_iqm_cnm_profile_tbl_get_unsafe(
            unit,
            profile_ndx,
            &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->q_eq = data.cp_qeq;
  info->cpw_power = data.cp_w;
  info->max_neg_fb_value = data.cp_fb_max_val;
  info->quant_div = data.cp_quant_div;
  info->is_sampling_th_random = (data.cp_fixed_sample_base == 0 ? 1 : 0);

  for(i = 0; i < ARAD_CNM_NOF_SAMPLING_BASES; i++)
  {
    info->sampling_base[i] = data.cp_sample_base[i];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_profile_get_unsafe()",0,0);
}

uint32
  arad_cnm_cp_profile_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_IN  ARAD_CNM_CP_PROFILE_INFO       *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_PROFILE_VERIFY);

  
  if(profile_ndx >= ARAD_CNM_NOF_PROFILES)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNM_ILLEGAL_CP_PROFILE_INDEX, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_profile_verify()",0,0);
}

uint32
  arad_cnm_cp_packet_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_CNM_CP_PACKET_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK,
     mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  uint64
    mac_sa_address;
  soc_reg_above_64_val_t 
    reg_val,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_PACKET_SET_UNSAFE);
  COMPILER_64_ZERO(mac_sa_address);

  if(info->ftmh_src_port == -1)
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_USE_ORIG_SRC_SYS_PORTf,  0x1));
  }
  else
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_USE_ORIG_SRC_SYS_PORTf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_SRC_SYS_PORTf,  info->ftmh_src_port));
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_TRAFFIC_CLASSf,  info->ftmh_tc));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_DPf,  info->ftmh_dp));
    

  SOC_REG_ABOVE_64_CLEAR(fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_EGQ_ETHERNET_TYPESr(unit, REG_PORT_ANY, reg_val));
  fld_val[0] = info->ether_type;
  soc_reg_above_64_field_set(unit, EGQ_ETHERNET_TYPESr, reg_val, ETH_TYPE_2f, fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_EGQ_ETHERNET_TYPESr(unit, REG_PORT_ANY, reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_CNM_GENERAL_CONFIGr, SOC_CORE_ALL, 0, CNM_ETHERNET_TYPE_CODEf,  9)); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_ETHER_TYPEr, REG_PORT_ANY, 0, CNM_ETHER_TYPEf,  info->ether_type)); 
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_CNM_GENERAL_CONFIGr, SOC_CORE_ALL, 0, CNM_USE_VLAN_PRIORITY_BITSf,  1));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PDUr, REG_PORT_ANY, 0, CNM_PDU_VERSIONf,  info->pdu_version));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PDUr, REG_PORT_ANY, 0, CNM_PDU_RESERVED_Vf,  info->pdu_reserved_v));
  
  
     
  res = soc_sand_pp_mac_address_struct_to_long(
            &info->mac_sa,
            mac_in_longs
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  COMPILER_64_SET(mac_sa_address, mac_in_longs[1], mac_in_longs[0]);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IPT_CNM_MAC_SAr, REG_PORT_ANY, 0, CNM_MAC_SAf,  mac_sa_address));

  
  if(info->ftmh_otm_port == -1)
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMH_2r, REG_PORT_ANY, 0, CNM_USE_OTM_PORT_TYPEf,  0x1));
  }
  else
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMH_2r, REG_PORT_ANY, 0, CNM_USE_OTM_PORT_TYPEf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_FTMH_2r, REG_PORT_ANY, 0, CNM_FTMH_OTM_PORTf,  info->ftmh_otm_port));
  }

  
  if(info->has_vlan_config) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ORIG_VLAN_PRIORITYf,  info->vlan_priority));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ORIG_VLAN_CFIf,  info->vlan_cfi));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ORIG_VLAN_IDf,  info->vlan_id));

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PPHFHEIr, REG_PORT_ANY, 0, CNMP_PH_EDIT_PCPf,  info->vlan_priority));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PPHFHEIr, REG_PORT_ANY, 0, CNMP_PH_EDIT_DEIf,  info->vlan_cfi));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PPHFHEIr, REG_PORT_ANY, 0, CNM_PPH_EDIT_VIDf,  info->vlan_id));
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_packet_set_unsafe()",0,0);
}

uint32
  arad_cnm_cp_packet_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT ARAD_CNM_CP_PACKET_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  uint64
    mac_sa_address;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_PACKET_SET_UNSAFE);
  COMPILER_64_ZERO(mac_sa_address);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_USE_ORIG_SRC_SYS_PORTf, &fld_val));
  if(fld_val == 0x1)
  {
    
    info->ftmh_src_port = -1;
  }
  else
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_SRC_SYS_PORTf, &info->ftmh_src_port));
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_FTMHr, REG_PORT_ANY, 0, CNM_FTMH_TRAFFIC_CLASSf, &info->ftmh_tc));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_ETHER_TYPEr, REG_PORT_ANY, 0, CNM_ETHER_TYPEf, &info->ether_type));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_PDUr, REG_PORT_ANY, 0, CNM_PDU_VERSIONf, &info->pdu_version));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_PDUr, REG_PORT_ANY, 0, CNM_PDU_RESERVED_Vf, &info->pdu_reserved_v));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, IPT_CNM_MAC_SAr, REG_PORT_ANY, 0, CNM_MAC_SAf, &mac_sa_address));

  COMPILER_64_TO_32_LO(mac_in_longs[0], mac_sa_address );
  COMPILER_64_TO_32_HI(mac_in_longs[1], mac_sa_address );

  
  
     
  res = soc_sand_pp_mac_address_long_to_struct(
            mac_in_longs,
            &info->mac_sa
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
 
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_FTMH_2r, REG_PORT_ANY, 0, CNM_USE_OTM_PORT_TYPEf, &fld_val));
  if(fld_val == 0x1)
  {
    
    info->ftmh_otm_port = -1;
  }
  else
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_FTMH_2r, REG_PORT_ANY, 0, CNM_FTMH_OTM_PORTf, &info->ftmh_otm_port));
  }

  
  info->has_vlan_config = 1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ORIG_VLAN_PRIORITYf, &info->vlan_priority));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ORIG_VLAN_CFIf, &info->vlan_cfi));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ORIG_VLAN_IDf, &info->vlan_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_packet_set_unsafe()",0,0);
}

uint32
  arad_cnm_cp_options_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_CP_OPTIONS                 *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_OPTIONS_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, EN_CNM_ING_REPf,  info->gen_if_replications));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_GEN_IF_NO_CN_TAGf,  info->gen_if_no_cn_tag));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_DISABLE_CN_TAGf,  info->disable_cn_tag_if_exist_in_dram));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ADD_CFG_CN_TAGf,  info->add_cn_tag));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_GEN_IF_NO_PPHf,  info->gen_if_no_pph));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PDU_CN_TAGr, REG_PORT_ANY, 0, CNM_CN_TAG_ETHER_TYPEf,  info->cn_tag_ethertype));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_PDU_CN_TAGr, REG_PORT_ANY, 0, CNM_CN_TAG_FLOW_IDf,  info->cn_tag_flow_id));

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_CALC_FABRIC_CRC_DISf,  info->disable_fabric_crc_calculation));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_REMOVE_DRAM_CRC_DISf,  info->disable_crc_removal));
    if(SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, FIELD_29_29f,  info->enable_cnm_ocb));

    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_options_set_unsafe()",0,0);
}

uint32
  arad_cnm_cp_options_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_CP_OPTIONS                 *info
  )
{
  uint32
    res = SOC_SAND_OK,
    field_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNM_CP_OPTIONS_GET_UNSAFE);
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CP_CONFIGURATIONr, SOC_CORE_ALL, 0, EN_CNM_ING_REPf, &field_val));
  info->gen_if_replications = field_val;
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_GEN_IF_NO_CN_TAGf, &field_val));
  info->gen_if_no_cn_tag = field_val;
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_DISABLE_CN_TAGf, &field_val));
  info->disable_cn_tag_if_exist_in_dram = field_val;
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_ADD_CFG_CN_TAGf, &field_val));
  info->add_cn_tag = field_val;
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,IPT_CNM_CONTORLr,REG_PORT_ANY,0, CNM_GEN_IF_NO_PPHf,&field_val));
  info->gen_if_no_pph = field_val;
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,IPT_CNM_PDU_CN_TAGr,REG_PORT_ANY,0, CNM_CN_TAG_ETHER_TYPEf,&field_val));
  info->cn_tag_ethertype = field_val;
  field_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,IPT_CNM_PDU_CN_TAGr,REG_PORT_ANY,0, CNM_CN_TAG_FLOW_IDf,&field_val));
  info->cn_tag_flow_id = field_val;
  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    field_val = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_CALC_FABRIC_CRC_DISf, &field_val));
    info->disable_fabric_crc_calculation = field_val;
    field_val = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_CNM_CONTORLr, REG_PORT_ANY, 0, CNM_REMOVE_DRAM_CRC_DISf, &field_val));
    info->disable_crc_removal = field_val ;
    if(SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        field_val = 0;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, FIELD_29_29f, &field_val));
        info->enable_cnm_ocb = field_val;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnm_cp_options_get_unsafe()",0,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

