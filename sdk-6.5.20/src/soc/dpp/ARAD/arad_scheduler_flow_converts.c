#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/ARAD/arad_scheduler_flow_converts.h>
#include <soc/dpp/ARAD/arad_scheduler_flows.h>
#include <soc/dpp/ARAD/arad_scheduler_elements.h>

#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>






#define ARAD_SCH_SUB_FLOW_COS_HR_EF1           0xfc
#define ARAD_SCH_SUB_FLOW_COS_HR_EF2           0xfd
#define ARAD_SCH_SUB_FLOW_COS_HR_EF3           0xfe
#define ARAD_SCH_SUB_FLOW_COS_HR_BE            0xff
#define ARAD_SCH_SUB_FLOW_COS_HR_EF4           0x30
#define ARAD_SCH_SUB_FLOW_COS_HR_EF5           0x70
#define ARAD_SCH_SUB_FLOW_COS_HR_EF6           0xb0
#define ARAD_SCH_SUB_FLOW_COS_HR_EF7           0xf0
#define ARAD_SCH_SUB_FLOW_COS_HR_EF8           0xe4
#define ARAD_SCH_SUB_FLOW_COS_HR_EF9           0xe1


#define ARAD_SCH_SUB_FLOW_COS_CL_SP_ENCHANCED  0xfb
#define ARAD_SCH_SUB_FLOW_COS_CL_SP1           0xfc
#define ARAD_SCH_SUB_FLOW_COS_CL_SP2           0xfd
#define ARAD_SCH_SUB_FLOW_COS_CL_SP3           0xfe
#define ARAD_SCH_SUB_FLOW_COS_CL_SP4           0xff

#define ARAD_SCH_SUB_FLOW_COS_CL_MIN_AF        1
#define ARAD_SCH_SUB_FLOW_COS_CL_MAX_AF        0xfa

#define ARAD_SCH_FDM_COS_HR_MAN_MS_BIT         4
#define ARAD_SCH_FDM_COS_HR_MAN_LS_BIT         0

#define ARAD_SCH_FDM_COS_HR_EXP_MS_BIT         7
#define ARAD_SCH_FDM_COS_HR_EXP_LS_BIT         5

#define ARAD_SCH_SHPR_CLKS_PER_TOKEN           12






#define IS_AF_WEIGHT(w) (((w) >= ARAD_SCH_SUB_FLOW_COS_CL_MIN_AF) && \
((w) <= ARAD_SCH_SUB_FLOW_COS_CL_MAX_AF))


















uint32
  arad_sch_from_internal_HR_weight_convert(
    SOC_SAND_IN     uint32                    internal_weight,
    SOC_SAND_OUT    uint32*                   weight
  )
{
  uint32
    mnt,
    exp;
  uint32
    weight_denominator;
  ARAD_REG_FIELD
    cos_hr_man,
    cos_hr_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SCH_FROM_INTERNAL_HR_WEIGHT_CONVERT);

  cos_hr_man.lsb = ARAD_SCH_FDM_COS_HR_MAN_LS_BIT;
  cos_hr_man.msb = ARAD_SCH_FDM_COS_HR_MAN_MS_BIT;

  cos_hr_exp.lsb = ARAD_SCH_FDM_COS_HR_EXP_LS_BIT;
  cos_hr_exp.msb = ARAD_SCH_FDM_COS_HR_EXP_MS_BIT;

  
  SOC_SAND_CHECK_NULL_INPUT(weight);

  mnt = SOC_SAND_GET_FLD_FROM_PLACE(internal_weight,
    ARAD_FLD_SHIFT_OLD(cos_hr_man),
    ARAD_FLD_MASK_OLD(cos_hr_man));
  exp = SOC_SAND_GET_FLD_FROM_PLACE(internal_weight,
    ARAD_FLD_SHIFT_OLD(cos_hr_exp),
    ARAD_FLD_MASK_OLD(cos_hr_exp));

  soc_sand_mnt_binary_fraction_exp_to_abs_val(
    ARAD_FLD_NOF_BITS_OLD(cos_hr_man),
    ARAD_FLD_NOF_BITS_OLD(cos_hr_exp),
    ARAD_SCH_FLOW_HR_MAX_WEIGHT,
    mnt,
    exp,
    weight,
    &weight_denominator
    );

  *weight /= weight_denominator;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_from_internal_HR_weight_convert()",0,0);
}



uint32
  arad_sch_to_internal_HR_weight_convert(
    SOC_SAND_IN     uint32                    weight,
    SOC_SAND_OUT    uint32*                   internal_weight
  )
{
  uint32
    mnt,
    exp;
  ARAD_REG_FIELD
    cos_hr_man,
    cos_hr_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SCH_TO_INTERNAL_HR_WEIGHT_CONVERT);
  
  SOC_SAND_CHECK_NULL_INPUT(internal_weight);

  cos_hr_man.lsb = ARAD_SCH_FDM_COS_HR_MAN_LS_BIT;
  cos_hr_man.msb = ARAD_SCH_FDM_COS_HR_MAN_MS_BIT;

  cos_hr_exp.lsb = ARAD_SCH_FDM_COS_HR_EXP_LS_BIT;
  cos_hr_exp.msb = ARAD_SCH_FDM_COS_HR_EXP_MS_BIT;

  soc_sand_abs_val_to_mnt_binary_fraction_exp(
    weight,
    1,
    ARAD_FLD_NOF_BITS_OLD(cos_hr_man),
    ARAD_FLD_NOF_BITS_OLD(cos_hr_exp),
    ARAD_SCH_FLOW_HR_MAX_WEIGHT,
    &mnt,
    &exp
  );

  SOC_SAND_LIMIT_FROM_ABOVE(mnt, ARAD_FLD_MAX_OLD(cos_hr_man));
  SOC_SAND_LIMIT_FROM_ABOVE(exp, ARAD_FLD_MAX_OLD(cos_hr_exp));

  *internal_weight = 0;

  *internal_weight |= SOC_SAND_SET_FLD_IN_PLACE(mnt,
    ARAD_FLD_SHIFT_OLD(cos_hr_man),
    ARAD_FLD_MASK_OLD(cos_hr_man));
  *internal_weight |= SOC_SAND_SET_FLD_IN_PLACE(exp,
    ARAD_FLD_SHIFT_OLD(cos_hr_exp),
    ARAD_FLD_MASK_OLD(cos_hr_exp));

    
  if(*internal_weight == ARAD_SCH_SUB_FLOW_COS_HR_EF1)
  {
    *internal_weight = 0xCE; 
  }
  else if(*internal_weight == ARAD_SCH_SUB_FLOW_COS_HR_EF2)
  {
    *internal_weight = 0xCF; 
  }
  else if(*internal_weight == ARAD_SCH_SUB_FLOW_COS_HR_EF3)
  {
    *internal_weight = 0xCF; 
  }
  else if(*internal_weight >= ARAD_SCH_SUB_FLOW_COS_HR_BE)
  {
    *internal_weight = 0xD0; 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_to_internal_HR_weight_convert()",0,0);
}

uint32
  arad_sch_from_internal_HR_subflow_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     ARAD_SCH_SE_HR*        hr_properties,
    SOC_SAND_IN     ARAD_SCH_INTERNAL_SUB_FLOW_DESC* internal_sub_flow,
    SOC_SAND_OUT    ARAD_SCH_SUBFLOW*           sub_flow
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FROM_INTERNAL_HR_SUBFLOW_CONVERT);
  SOC_SAND_CHECK_NULL_INPUT(hr_properties);
  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);

  if (ARAD_SCH_SUB_FLOW_COS_HR_EF1 == internal_sub_flow->cos)
  {
    sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_CLASS_EF1;
  }
  else if (ARAD_SCH_SUB_FLOW_COS_HR_EF2 == internal_sub_flow->cos)
  {
    sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_CLASS_EF2;
  }
  else if (ARAD_SCH_SUB_FLOW_COS_HR_EF3 == internal_sub_flow->cos)
  {
    sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_CLASS_EF3;
  }
  else if (ARAD_SCH_SUB_FLOW_COS_HR_BE == internal_sub_flow->cos)
  {
    switch (hr_properties->mode)
    {
    case ARAD_SCH_HR_MODE_SINGLE_WFQ:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_SINGLE_CLASS_BE1;
      break;
    case ARAD_SCH_HR_MODE_DUAL_WFQ:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_DUAL_CLASS_BE2;
      break;
    case ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE2;
      break;
    default:
      break;
    }
  }
  else if ((ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ == hr_properties->mode) &&
           (internal_sub_flow->hr_sel_dual == FALSE) &&
           ((ARAD_SCH_SUB_FLOW_COS_HR_EF4 == internal_sub_flow->cos) ||
            (ARAD_SCH_SUB_FLOW_COS_HR_EF5 == internal_sub_flow->cos) ||
            (ARAD_SCH_SUB_FLOW_COS_HR_EF6 == internal_sub_flow->cos) ||
            (ARAD_SCH_SUB_FLOW_COS_HR_EF7 == internal_sub_flow->cos) ||
            (ARAD_SCH_SUB_FLOW_COS_HR_EF8 == internal_sub_flow->cos) ||
            (ARAD_SCH_SUB_FLOW_COS_HR_EF9 == internal_sub_flow->cos)
           )
          )
  {
    switch (internal_sub_flow->cos)
    {
    case ARAD_SCH_SUB_FLOW_COS_HR_EF4:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF1;
      break;
    case ARAD_SCH_SUB_FLOW_COS_HR_EF5:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF2;
      break;
    case ARAD_SCH_SUB_FLOW_COS_HR_EF6:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF3;
      break;
    case ARAD_SCH_SUB_FLOW_COS_HR_EF7:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF4;
      break;
    case ARAD_SCH_SUB_FLOW_COS_HR_EF8:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF5;
      break;
    case ARAD_SCH_SUB_FLOW_COS_HR_EF9:
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF6;
      break;
    default:
      break;
    }
  }
  else
  {
    if (ARAD_SCH_HR_MODE_SINGLE_WFQ == hr_properties->mode)
    {
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ;
    }
    else if (ARAD_SCH_HR_MODE_DUAL_WFQ == hr_properties->mode)
    {
      sub_flow->credit_source.se_info.hr.sp_class = internal_sub_flow->hr_sel_dual ?
      ARAD_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ : ARAD_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ;
    }
    else if (ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ == hr_properties->mode)
    {
      sub_flow->credit_source.se_info.hr.sp_class = ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ;
    }
    arad_sch_from_internal_HR_weight_convert(
      internal_sub_flow->cos,
      &sub_flow->credit_source.se_info.hr.weight
    );
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_from_internal_HR_subflow_convert()",0,0);
}

uint32
  arad_sch_to_internal_HR_subflow_convert(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     ARAD_SCH_SE_HR*                  hr_properties,
    SOC_SAND_IN     ARAD_SCH_SUBFLOW*                sub_flow,
    SOC_SAND_OUT    ARAD_SCH_INTERNAL_SUB_FLOW_DESC* internal_sub_flow
  )
{
  uint32
    val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FROM_INTERNAL_HR_SUBFLOW_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(hr_properties);
  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);
  
  if (ARAD_SCH_FLOW_HR_CLASS_EF1 == sub_flow->credit_source.se_info.hr.sp_class)
  {
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF1;
  }
  else if (ARAD_SCH_FLOW_HR_CLASS_EF2 == sub_flow->credit_source.se_info.hr.sp_class)
  {
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF2;
  }
  else if (ARAD_SCH_FLOW_HR_CLASS_EF3 == sub_flow->credit_source.se_info.hr.sp_class)
  {
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF3;
  }
  else if (
    ((ARAD_SCH_FLOW_HR_SINGLE_CLASS_BE1 ==
    sub_flow->credit_source.se_info.hr.sp_class) &&
    (ARAD_SCH_HR_MODE_SINGLE_WFQ == hr_properties->mode)) ||
    ((ARAD_SCH_FLOW_HR_DUAL_CLASS_BE2 ==
    sub_flow->credit_source.se_info.hr.sp_class) &&
    (ARAD_SCH_HR_MODE_DUAL_WFQ == hr_properties->mode))   ||
    ((ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE2 ==
    sub_flow->credit_source.se_info.hr.sp_class) &&
    (ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ == hr_properties->mode))
    )
  {
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_BE;
  }
  else if ((ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ == hr_properties->mode) &&
    ((ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF1 ==
    sub_flow->credit_source.se_info.hr.sp_class) ||
    (ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF2 ==
    sub_flow->credit_source.se_info.hr.sp_class) ||
    (ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF3 ==
    sub_flow->credit_source.se_info.hr.sp_class) ||
    (ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF4 ==
    sub_flow->credit_source.se_info.hr.sp_class) ||
    (ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF5 ==
    sub_flow->credit_source.se_info.hr.sp_class) ||
    (ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF6 ==
    sub_flow->credit_source.se_info.hr.sp_class))
    )
  {
    switch (sub_flow->credit_source.se_info.hr.sp_class)
    {
    case ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF1:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF4;
      break;
    case ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF2:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF5;
      break;
    case ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF3:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF6;
      break;
    case ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF4:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF7;
      break;
    case ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF5:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF8;
      break;
    case ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF6:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF9;
      break;
    default:
      break;
    }
  }
  else if (
    
    ((ARAD_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ ==
    sub_flow->credit_source.se_info.hr.sp_class) &&
    (ARAD_SCH_HR_MODE_SINGLE_WFQ == hr_properties->mode)) ||
    
    (((ARAD_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ ==
    sub_flow->credit_source.se_info.hr.sp_class) ||
    (ARAD_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ ==
    sub_flow->credit_source.se_info.hr.sp_class)) &&
    (ARAD_SCH_HR_MODE_DUAL_WFQ == hr_properties->mode)) ||
    
    ((ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ ==
    sub_flow->credit_source.se_info.hr.sp_class) &&
    (ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ == hr_properties->mode))
    )
  {
    if (ARAD_SCH_HR_MODE_DUAL_WFQ == hr_properties->mode)
    {
      internal_sub_flow->hr_sel_dual =
        (ARAD_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ ==
        sub_flow->credit_source.se_info.hr.sp_class) ? 1 : 0;
    }

    if(ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ == hr_properties->mode)
    {
      internal_sub_flow->hr_sel_dual =
        (ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ ==
        sub_flow->credit_source.se_info.hr.sp_class) ? 1 : 0;
    }

    arad_sch_to_internal_HR_weight_convert(
      sub_flow->credit_source.se_info.hr.weight,
      &val
    );
    internal_sub_flow->cos = val;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_HR_AND_SCHEDULER_MODE_MISMATCH_ERR, 20, exit);
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_to_internal_HR_subflow_convert()",0,0);
}

uint32
  arad_sch_from_internal_CL_weight_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     ARAD_SCH_CL_CLASS_MODE       mode,
    SOC_SAND_IN     uint32                     internal_weight,
    SOC_SAND_OUT    uint32*                     weight
  )
{
  uint32
    i_w,
    max_weight,
    weight_denominator,
    res;
  ARAD_SCH_FLOW_IPF_CONFIG_MODE
    config_mode;
  soc_field_info_t
    cos_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FROM_INTERNAL_CL_WEIGHT_CONVERT);
  
  SOC_SAND_CHECK_NULL_INPUT(weight);

  res = arad_sch_flow_ipf_config_mode_get_unsafe(
          unit,
          &config_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (config_mode == ARAD_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL)
  {
    i_w = (mode == ARAD_SCH_CL_MODE_5) ?internal_weight:internal_weight/4;
  }
  else if(config_mode == ARAD_SCH_FLOW_IPF_CONFIG_MODE_INVERSE)
  {
    max_weight = (mode == ARAD_SCH_CL_MODE_5) ?
    ARAD_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5:ARAD_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4;

    ARAD_TBL_REF(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, COSf, &cos_fld);
 

    soc_sand_mnt_binary_fraction_exp_to_abs_val(
      ARAD_FLD_NOF_BITS(cos_fld),
      0,
      max_weight,
      internal_weight,
      0,
      &i_w,
      &weight_denominator
      );
    i_w /= weight_denominator;
  }
  else{
      SOC_SAND_SET_ERROR_CODE(ARAD_FLD_OUT_OF_RANGE, 40, exit);
  }

  *weight = i_w;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_from_internal_CL_weight_convert()",0,0);
}

uint32
  arad_sch_to_internal_CL_weight_convert(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     ARAD_SCH_CL_CLASS_MODE     mode,
    SOC_SAND_IN     uint32                    weight,
    SOC_SAND_OUT    uint32*                  internal_weight
  )
{
  uint32
    max_weight,
    demi_exp,
    weight_calc,
    i_w,
    res;
  ARAD_SCH_FLOW_IPF_CONFIG_MODE
    config_mode;
  soc_field_info_t
    cos_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_TO_INTERNAL_CL_WEIGHT_CONVERT);
  
  SOC_SAND_CHECK_NULL_INPUT(internal_weight);

  res = arad_sch_flow_ipf_config_mode_get_unsafe(
          unit,
          &config_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (config_mode == ARAD_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL)
  {
    i_w = weight;
  }
  else if(config_mode == ARAD_SCH_FLOW_IPF_CONFIG_MODE_INVERSE)
  {
 
    ARAD_TBL_REF(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, COSf, &cos_fld);

    weight_calc = (mode == ARAD_SCH_CL_MODE_3) || (mode == ARAD_SCH_CL_MODE_4) ? (weight * 4) : (weight);
    max_weight = ARAD_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5;

    soc_sand_abs_val_to_mnt_binary_fraction_exp(
      weight_calc,
      1,
      ARAD_FLD_NOF_BITS(cos_fld),
      0,
      max_weight,
      &i_w,
      &demi_exp
      );
  }
  else{
      SOC_SAND_SET_ERROR_CODE(ARAD_FLD_OUT_OF_RANGE, 40, exit);
  }
  
  i_w = (mode == ARAD_SCH_CL_MODE_3) || (mode == ARAD_SCH_CL_MODE_4) ? (i_w << 2) : i_w;

  SOC_SAND_LIMIT_VAL(i_w, ARAD_SCH_SUB_FLOW_COS_CL_MIN_AF, ARAD_SCH_SUB_FLOW_COS_CL_MAX_AF);

  
  i_w = (mode == ARAD_SCH_CL_MODE_3) || (mode == ARAD_SCH_CL_MODE_4) ? ((i_w >> 2) << 2):i_w;

  *internal_weight = i_w;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_to_internal_CL_weight_convert()",0,0);
}

uint32
  arad_sch_from_internal_CL_subflow_convert(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_SE_CL*            cl_properties,
    SOC_SAND_IN     ARAD_SCH_INTERNAL_SUB_FLOW_DESC* internal_sub_flow,
    SOC_SAND_OUT    ARAD_SCH_SUBFLOW*          sub_flow
  )
{
  uint32
    res;
  ARAD_SCH_SE_CL_CLASS_INFO
    class_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FROM_INTERNAL_CL_SUBFLOW_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(cl_properties);
  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);

  class_type.id = cl_properties->id;
  res = arad_sch_class_type_params_get_unsafe(
    unit, core,
    class_type.id,
    &class_type
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  
  if (ARAD_SCH_SUB_FLOW_COS_CL_SP_ENCHANCED == internal_sub_flow->cos)
  {
    if (class_type.enhanced_mode == ARAD_CL_ENHANCED_MODE_ENABLED_HP)
    {
      sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED;
    }
    else if (class_type.enhanced_mode == ARAD_CL_ENHANCED_MODE_ENABLED_LP)
    {
      sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_AND_SE_TYPE_MISMATCH_ERR, 10, exit);
    }
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP1 == internal_sub_flow->cos) &&
    ((ARAD_SCH_CL_MODE_1 == class_type.mode)        ||
    ((ARAD_SCH_CL_MODE_2 == class_type.mode) &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode))             ||
    (ARAD_SCH_CL_MODE_4 == class_type.mode))
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP1;
  }
  
  else if (
    ((ARAD_SCH_SUB_FLOW_COS_CL_SP2 == internal_sub_flow->cos) &&
    ((ARAD_SCH_CL_MODE_1 == class_type.mode)        ||
    ((ARAD_SCH_CL_MODE_2 == class_type.mode) &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode))))
    ||
    ((ARAD_SCH_SUB_FLOW_COS_CL_SP4 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_3 == class_type.mode))
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP2;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP3 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_1 == class_type.mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP3;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP4 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_1 == class_type.mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP4;
  }
  
  else if (
    IS_AF_WEIGHT(internal_sub_flow->cos)                       &&
    ARAD_SCH_IS_INDEPENDENT_WFQ_MODE(class_type.weight_mode)            &&
    ((ARAD_SCH_CL_MODE_3 == class_type.mode) ||
    (ARAD_SCH_CL_MODE_5 == class_type.mode))
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ;
    arad_sch_from_internal_CL_weight_convert(
      unit,
      class_type.mode,
      internal_sub_flow->cos,
      &sub_flow->credit_source.se_info.cl.weight
    );
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP1 == internal_sub_flow->cos) &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)             &&
    ((ARAD_SCH_CL_MODE_3 == class_type.mode) ||
    (ARAD_SCH_CL_MODE_5 == class_type.mode))
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP2 == internal_sub_flow->cos) &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)             &&
    ((ARAD_SCH_CL_MODE_3 == class_type.mode) ||
    (ARAD_SCH_CL_MODE_5 == class_type.mode))
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP3 == internal_sub_flow->cos) &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)             &&
    ((ARAD_SCH_CL_MODE_3 == class_type.mode) ||
    (ARAD_SCH_CL_MODE_5 == class_type.mode))
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP4 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_5 == class_type.mode)      &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4;
  }
  
  else if (
    IS_AF_WEIGHT(internal_sub_flow->cos)                     &&
    (ARAD_SCH_CL_MODE_4 == class_type.mode)      &&
    ARAD_SCH_IS_INDEPENDENT_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ;
    arad_sch_from_internal_CL_weight_convert(
      unit,
      class_type.mode,
      internal_sub_flow->cos,
      &sub_flow->credit_source.se_info.cl.weight
    );
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP2 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_4 == class_type.mode)      &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP3 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_4 == class_type.mode)      &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP4 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_4 == class_type.mode)      &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP3 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_2 == class_type.mode)      &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1;
  }
  
  else if (
    (ARAD_SCH_SUB_FLOW_COS_CL_SP4 == internal_sub_flow->cos) &&
    (ARAD_SCH_CL_MODE_2 == class_type.mode)      &&
    ARAD_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)
    )
  {
    sub_flow->credit_source.se_info.cl.sp_class = ARAD_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2;
  }
  
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_SE_TYPE_SE_CONFIG_MISMATCH_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_from_internal_CL_subflow_convert()",0,0);
}

uint32
  arad_sch_to_internal_CL_subflow_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     int                     core,
    SOC_SAND_IN     ARAD_SCH_SE_CL*        cl_properties,
    SOC_SAND_IN     ARAD_SCH_SUBFLOW*           sub_flow,
    SOC_SAND_OUT    ARAD_SCH_INTERNAL_SUB_FLOW_DESC* internal_sub_flow
  )
{
  uint32
    res;
  ARAD_SCH_SE_CL_CLASS_INFO
    class_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_TO_INTERNAL_CL_SUBFLOW_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(cl_properties);
  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);


  class_type.id = cl_properties->id;
  res = arad_sch_class_type_params_get_unsafe(
    unit, core,
    class_type.id,
    &class_type
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch (sub_flow->credit_source.se_info.cl.sp_class)
  {
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP1:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1:
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_CL_SP1;
    break;

  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP2:
    internal_sub_flow->cos = (ARAD_SCH_CL_MODE_3 == class_type.mode) ?
ARAD_SCH_SUB_FLOW_COS_CL_SP4: ARAD_SCH_SUB_FLOW_COS_CL_SP2;
    break;

  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1:
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_CL_SP2;
    break;

  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP3:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1:
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_CL_SP3;
    break;

  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP4:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2:
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_CL_SP4;
    break;

  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ:
    arad_sch_to_internal_CL_weight_convert(
      unit,
      class_type.mode,
      sub_flow->credit_source.se_info.cl.weight,
      &internal_sub_flow->cos
    );
    break;
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED:
  case ARAD_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED:
    if (class_type.enhanced_mode == ARAD_CL_ENHANCED_MODE_DISABLED)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_SCH_SE_TYPE_SE_CONFIG_MISMATCH_ERR, 20, exit);
    }

    
    internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_CL_SP_ENCHANCED;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_FLOW_HR_CLASS_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_to_internal_CL_subflow_convert()",0,0);
}


uint32
  arad_sch_from_internal_subflow_shaper_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_OUT    ARAD_SCH_SUBFLOW           *sub_flow
  )
{
  uint32
    res,
    max_rate_in_kbits_per_sec,
    shaper_weight,
    shaper_weight_denominator,
    credit_worth;
  soc_field_info_t
    peak_rate_man_fld, 
	peak_rate_exp_fld;
  SOC_SAND_U64
    intermediate_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_TO_INTERNAL_CL_SUBFLOW_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);

 
 

  
  ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
  ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,10,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;

  max_rate_in_kbits_per_sec = ((credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
    arad_chip_mega_ticks_per_sec_get(unit)) / ARAD_SCH_SHPR_CLKS_PER_TOKEN;
  max_rate_in_kbits_per_sec *= 1000;

  if (
      (internal_sub_flow->peak_rate_man == ARAD_FLD_MAX(peak_rate_man_fld)) &&
      (internal_sub_flow->peak_rate_exp == 0)
     )
  {
    sub_flow->shaper.max_rate = ARAD_SCH_SUB_FLOW_SHAPE_NO_LIMIT;
  }
  else
  {
    soc_sand_mnt_binary_fraction_exp_to_abs_val(ARAD_FLD_NOF_BITS(peak_rate_man_fld),
      ARAD_FLD_NOF_BITS(peak_rate_exp_fld),
      max_rate_in_kbits_per_sec,
      internal_sub_flow->peak_rate_man,
      internal_sub_flow->peak_rate_exp,
      &shaper_weight,
      &shaper_weight_denominator
      );
    if(!shaper_weight)
    {
      sub_flow->shaper.max_rate = max_rate_in_kbits_per_sec;
    }
    else
    {
      soc_sand_u64_multiply_longs(
        max_rate_in_kbits_per_sec,
        shaper_weight_denominator,
        &intermediate_val
      );
      soc_sand_u64_devide_u64_long(
        &intermediate_val,
        shaper_weight,
        &intermediate_val
      );

      if(intermediate_val.arr[1] != 0)
      {
        intermediate_val.arr[1] = 0;
        intermediate_val.arr[0] = 0xFFFFFFFF;
      }
      sub_flow->shaper.max_rate = intermediate_val.arr[0];
    }
  }

  sub_flow->shaper.max_burst = credit_worth * internal_sub_flow->max_burst;

  
  switch(internal_sub_flow->slow_rate_index) {
    case 0x0:
      sub_flow->slow_rate_ndx = ARAD_SCH_SLOW_RATE_NDX_1;
      break;
    case 0x1:
      sub_flow->slow_rate_ndx = ARAD_SCH_SLOW_RATE_NDX_2;
      break;
    default:
      sub_flow->slow_rate_ndx = ARAD_SCH_SLOW_RATE_NDX_1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_from_internal_subflow_shaper_convert()",0,0);
}


STATIC
  uint8
    arad_sch_weight1_is_bigger(
      SOC_SAND_IN uint32 weight1,
      SOC_SAND_IN uint32 denominator1,
      SOC_SAND_IN uint32 weight2,
      SOC_SAND_IN uint32 denominator2
    )
{
  SOC_SAND_U64
    val64_1,
    val64_2;
  uint32
    is_bigger;
  
  soc_sand_u64_multiply_longs(
    weight1,
    1000,
    &val64_1
    );
  soc_sand_u64_devide_u64_long(
    &val64_1,
    denominator1,
    &val64_1
    );

  soc_sand_u64_multiply_longs(
    weight2,
    1000,
    &val64_2
    );
  soc_sand_u64_devide_u64_long(
    &val64_2,
    denominator2,
    &val64_2
    );

  is_bigger = soc_sand_u64_is_bigger(&val64_1, &val64_2);
  return SOC_SAND_NUM2BOOL(is_bigger);
}

uint32
  arad_sch_to_internal_subflow_shaper_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     ARAD_SCH_SUBFLOW           *sub_flow,
    SOC_SAND_OUT    ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_IN     uint32                     round_up
  )
{
  uint32
    res,
    max_rate_in_kbits_per_sec,
    shaper_weight_numerator,
    shaper_weight_denominator,
    exact_shaper_weight,
    exact_shaper_weight_denominator,
    credit_worth,
    peak_rate_man,
    peak_rate_exp;
  soc_field_info_t
    peak_rate_man_fld, 
	peak_rate_exp_fld,
    max_burst_fld;
  SOC_SAND_U64
    intermediate_numerator;
                                                    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_TO_INTERNAL_SUBFLOW_SHAPER_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);

  
  ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
  ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);
  ARAD_TBL_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, MAX_BURST_EVENf, &max_burst_fld);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(
    res,10,exit,ARAD_GET_ERR_TEXT_001,MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_get, (unit, &credit_worth))) ;

  max_rate_in_kbits_per_sec = (
    (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) * arad_chip_mega_ticks_per_sec_get(unit))
    / ARAD_SCH_SHPR_CLKS_PER_TOKEN;
  max_rate_in_kbits_per_sec *= 1000;

  if (
       (0 == sub_flow->shaper.max_rate) ||
       (ARAD_SCH_SUB_FLOW_SHAPE_NO_LIMIT == sub_flow->shaper.max_rate)
     )
  {
    internal_sub_flow->peak_rate_man = ARAD_FLD_MAX(peak_rate_man_fld);
    internal_sub_flow->peak_rate_exp = 0;
  }
  else
  {
    shaper_weight_denominator = 64;
    soc_sand_u64_multiply_longs(
      max_rate_in_kbits_per_sec,
      shaper_weight_denominator,
      &intermediate_numerator
    );
    soc_sand_u64_devide_u64_long(
      &intermediate_numerator,
      sub_flow->shaper.max_rate,
      &intermediate_numerator
    );

    if(intermediate_numerator.arr[1] != 0)
    {
      intermediate_numerator.arr[1] = 0;
      intermediate_numerator.arr[0] = 0xFFFFFFFF;
    }
    shaper_weight_numerator = intermediate_numerator.arr[0];
    
    soc_sand_abs_val_to_mnt_binary_fraction_exp(
      shaper_weight_numerator,
      shaper_weight_denominator,
      ARAD_FLD_NOF_BITS(peak_rate_man_fld),
      ARAD_FLD_NOF_BITS(peak_rate_exp_fld),
      max_rate_in_kbits_per_sec,
      &peak_rate_man,
      &peak_rate_exp
    );

    internal_sub_flow->peak_rate_man = peak_rate_man;
    internal_sub_flow->peak_rate_exp = peak_rate_exp;

    soc_sand_mnt_binary_fraction_exp_to_abs_val(
      ARAD_FLD_NOF_BITS(peak_rate_man_fld),
      ARAD_FLD_NOF_BITS(peak_rate_exp_fld),
      max_rate_in_kbits_per_sec,
      peak_rate_man,
      peak_rate_exp,
      &exact_shaper_weight,
      &exact_shaper_weight_denominator
      );

    if(round_up)
    {
      if (arad_sch_weight1_is_bigger(exact_shaper_weight, exact_shaper_weight_denominator, shaper_weight_numerator, shaper_weight_denominator))
      {
        if (internal_sub_flow->peak_rate_man == ARAD_FLD_MAX(peak_rate_man_fld))
        {
          if (internal_sub_flow->peak_rate_exp > 0)
          {
            internal_sub_flow->peak_rate_man = SOC_SAND_DIV_ROUND_UP(internal_sub_flow->peak_rate_man,2);
            internal_sub_flow->peak_rate_exp--;
          }
        }
        else
        {
          internal_sub_flow->peak_rate_man++;
        }
      }
    }

    
    SOC_SAND_LIMIT_FROM_ABOVE(internal_sub_flow->peak_rate_man, ARAD_FLD_MAX(peak_rate_man_fld));
    SOC_SAND_LIMIT_FROM_ABOVE(internal_sub_flow->peak_rate_exp, ARAD_FLD_MAX(peak_rate_exp_fld));

      
    if( ((internal_sub_flow->peak_rate_man & 0x1) == 1) &&
      (internal_sub_flow->peak_rate_exp == 15)
      )
    {
      internal_sub_flow->peak_rate_man = SOC_SAND_DIV_ROUND_UP(internal_sub_flow->peak_rate_man,2);
      internal_sub_flow->peak_rate_exp--;
    }
  }

  internal_sub_flow->max_burst = SOC_SAND_DIV_ROUND_UP(sub_flow->shaper.max_burst,credit_worth);
  SOC_SAND_LIMIT_FROM_ABOVE(internal_sub_flow->max_burst, ARAD_FLD_MAX(max_burst_fld));

  internal_sub_flow->max_burst_update = 1;

  
  switch(sub_flow->slow_rate_ndx) {
    case ARAD_SCH_SLOW_RATE_NDX_1:
      internal_sub_flow->slow_rate_index = 0x0;
      break;
    case ARAD_SCH_SLOW_RATE_NDX_2:
      internal_sub_flow->slow_rate_index = 0x1;
      break;
    default:
      internal_sub_flow->slow_rate_index = 0x0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_to_internal_subflow_shaper_convert()",0,0);
}

uint32
  arad_sch_INTERNAL_SUB_FLOW_to_SUB_FLOW_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     int                     core,
    SOC_SAND_IN     ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_OUT    ARAD_SCH_SUBFLOW           *sub_flow
  )
{
  uint32
    res;
  ARAD_SCH_SE_INFO
    se;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_INTERNAL_SUB_FLOW_TO_SUB_FLOW_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);

  
  se.id = sub_flow->credit_source.id = internal_sub_flow->sch_number;

  res = arad_sch_se_get_unsafe(
    unit, core,
    se.id,
    &se
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_sch_from_internal_subflow_shaper_convert(
          unit,
          internal_sub_flow,
          sub_flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  switch (se.type)
  {
  case ARAD_SCH_SE_TYPE_HR:
    sub_flow->credit_source.se_type = ARAD_SCH_SE_TYPE_HR;
    res = arad_sch_from_internal_HR_subflow_convert(
            unit,
            &se.type_info.hr,
            internal_sub_flow,
            sub_flow
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;

  case ARAD_SCH_SE_TYPE_CL:
    sub_flow->credit_source.se_type = ARAD_SCH_SE_TYPE_CL;
    res = arad_sch_from_internal_CL_subflow_convert(
      unit, core,
      &se.type_info.cl,
      internal_sub_flow,
      sub_flow);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;

  case ARAD_SCH_SE_TYPE_FQ:
    sub_flow->credit_source.se_type = ARAD_SCH_SE_TYPE_FQ;
    break;

  default:
    sub_flow->credit_source.se_type = ARAD_SCH_SE_TYPE_NONE;
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_sch_INTERNAL_SUB_FLOW_to_SUB_FLOW_convert()",0,0);
}

uint32
  arad_sch_SUB_FLOW_to_INTERNAL_SUB_FLOW_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     int                     core,
    SOC_SAND_IN     ARAD_SCH_SUBFLOW           *sub_flow,
    SOC_SAND_OUT    ARAD_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow
  )
{
  uint32
    res;
  ARAD_SCH_SE_INFO
    se;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SUB_FLOW_TO_INTERNAL_SUB_FLOW_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(internal_sub_flow);
  SOC_SAND_CHECK_NULL_INPUT(sub_flow);
  if (!sub_flow->update_bw_only) {
    
    res = arad_sch_se_get_unsafe(
          unit, core,
          sub_flow->credit_source.id,
          &se
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    internal_sub_flow->sch_number = sub_flow->credit_source.id;
  }
  
  res = arad_sch_to_internal_subflow_shaper_convert(
          unit,
          sub_flow,
          internal_sub_flow,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (!sub_flow->update_bw_only) {
    
    internal_sub_flow->hr_sel_dual = 0;
    switch (se.type)
    {
    case ARAD_SCH_SE_TYPE_HR:
      res = arad_sch_to_internal_HR_subflow_convert(
              unit,
              &se.type_info.hr,
              sub_flow,
              internal_sub_flow
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      break;
  
    case ARAD_SCH_SE_TYPE_CL:
      res = arad_sch_to_internal_CL_subflow_convert(
              unit, core,
              &se.type_info.cl,
              sub_flow,
              internal_sub_flow
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      break;
  
    case ARAD_SCH_SE_TYPE_FQ:
      internal_sub_flow->cos = ARAD_SCH_SUB_FLOW_COS_HR_EF1;
      break;
  
    default:
      break;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_SUB_FLOW_to_INTERNAL_SUB_FLOW_convert", 0, 0);
}

uint32
  arad_sch_INTERNAL_HR_MODE_to_HR_MODE_convert(
    SOC_SAND_IN     uint32                internal_hr_mode,
    SOC_SAND_OUT    ARAD_SCH_SE_HR_MODE  *hr_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SCH_INTERNAL_HR_MODE_TO_HR_MODE_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(hr_mode);

  switch (internal_hr_mode)
  {
  case 0x0:
    *hr_mode = ARAD_SCH_HR_MODE_SINGLE_WFQ;
    break;

  case 0x1:
    *hr_mode = ARAD_SCH_HR_MODE_DUAL_WFQ;
    break;

  case 0x3:
    *hr_mode = ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ;
    break;

  default:
    *hr_mode = ARAD_SCH_HR_MODE_NONE;
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_HR_MODE_OUT_OF_RANGE_ERR, 10, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_INTERNAL_HR_MODE_to_HR_MODE_convert", internal_hr_mode, 0);
}

uint32
  arad_sch_HR_MODE_to_INTERNAL_HR_MODE_convert(
    SOC_SAND_IN    ARAD_SCH_SE_HR_MODE  hr_mode,
    SOC_SAND_OUT   uint32               *internal_hr_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_SCH_INTERNAL_HR_MODE_TO_HR_MODE_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(internal_hr_mode);

  switch (hr_mode)
  {
  case ARAD_SCH_HR_MODE_SINGLE_WFQ:
    *internal_hr_mode = 0x0;
    break;

  case ARAD_SCH_HR_MODE_DUAL_WFQ:
    *internal_hr_mode = 0x1;
    break;

  case ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ:
    *internal_hr_mode = 0x3;
    break;

  default:
    *internal_hr_mode = 0x0;
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_HR_MODE_OUT_OF_RANGE_ERR, 10, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_HR_MODE_to_INTERNAL_HR_MODE_convert", 0, 0);
}





#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

