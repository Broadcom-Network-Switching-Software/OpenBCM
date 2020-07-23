#include <shared/bsl.h>

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



#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_scheduler_element_converts.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_reg_access.h>


























SOC_SAND_RET
  arad_sch_INTERNAL_CLASS_TYPE_to_CLASS_TYPE_convert(
    SOC_SAND_IN     ARAD_SCH_SCT_TBL_DATA     *internal_class_type,
    SOC_SAND_OUT    ARAD_SCH_SE_CL_CLASS_INFO *class_type
  )
{
  ARAD_INIT_ERR_DEFS;

  ARAD_ERR_IF_NULL(class_type, 10);
  ARAD_ERR_IF_NULL(internal_class_type, 20);

  switch (internal_class_type->clconfig)
  {
  case 15:
    class_type->mode = ARAD_SCH_CL_MODE_1;
    break;
  case 3:
    class_type->mode = ARAD_SCH_CL_MODE_2;
    break;
  case 8:
    class_type->mode = ARAD_SCH_CL_MODE_3;
    break;
  case 1:
    class_type->mode = ARAD_SCH_CL_MODE_4;
    break;
  case 0:
    class_type->mode = ARAD_SCH_CL_MODE_5;
    break;
  default:
    ARAD_SET_ERR_AND_EXIT(ARAD_SCH_CLCONFIG_OUT_OF_RANGE_ERR);
  }
  class_type->weight[0] = internal_class_type->af0_inv_weight;
  class_type->weight[1] = internal_class_type->af1_inv_weight;
  class_type->weight[2] = internal_class_type->af2_inv_weight;
  class_type->weight[3] = internal_class_type->af3_inv_weight;

  class_type->weight_mode = internal_class_type->wfqmode;

  switch(internal_class_type->enh_clen)
  {
  case 0:
    class_type->enhanced_mode = ARAD_CL_ENHANCED_MODE_DISABLED;
    break;
  case 1:
    if (internal_class_type->enh_clsphigh)
    {
      class_type->enhanced_mode = ARAD_CL_ENHANCED_MODE_ENABLED_HP;
    }
    else
    {
      class_type->enhanced_mode = ARAD_CL_ENHANCED_MODE_ENABLED_LP;
    }
    break;
  default:
    ARAD_SET_ERR_AND_EXIT(ARAD_SCH_ENH_MODE_OUT_OF_RANGE_ERR);
  }

exit:
  ARAD_RETURN;
}


SOC_SAND_RET
  arad_sch_CLASS_TYPE_to_INTERNAL_CLASS_TYPE_convert(
    SOC_SAND_IN     int                 unit,
    SOC_SAND_IN   ARAD_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT  ARAD_SCH_SCT_TBL_DATA     *internal_class_type
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  ARAD_INIT_ERR_DEFS;

  ARAD_ERR_IF_NULL(class_type, 10);
  ARAD_ERR_IF_NULL(internal_class_type, 20);

  ret = soc_sand_os_memset(
    internal_class_type,
    0x0,
    sizeof(ARAD_SCH_SCT_TBL_DATA)
  );
  ARAD_EXIT_IF_ERR(ret, 25);

 
 


  switch (class_type->mode)
  {
  case ARAD_SCH_CL_MODE_1:
    internal_class_type->clconfig = 0xf;
    break;
  case ARAD_SCH_CL_MODE_2:
    internal_class_type->clconfig = 0x3;
    break;
  case ARAD_SCH_CL_MODE_3:
    internal_class_type->clconfig = 0x8;
    break;
  case ARAD_SCH_CL_MODE_4:
    internal_class_type->clconfig = 0x1;
    break;
  case ARAD_SCH_CL_MODE_5:
    internal_class_type->clconfig = 0x0;
    break;
  default:
    ARAD_SET_ERR_AND_EXIT(ARAD_SCH_CLCONFIG_OUT_OF_RANGE_ERR);
  }

  internal_class_type->af0_inv_weight = class_type->weight[0];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af0_inv_weight, 0x3FF);

  internal_class_type->af1_inv_weight = class_type->weight[1];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af1_inv_weight, 0x3FF);

  internal_class_type->af2_inv_weight = class_type->weight[2];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af2_inv_weight, 0x3FF);

  internal_class_type->af3_inv_weight = class_type->weight[3];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af3_inv_weight, 0x3FF);

  internal_class_type->wfqmode = class_type->weight_mode;

  
  if(
     (class_type->mode == ARAD_SCH_CL_MODE_1) ||
     (class_type->mode == ARAD_SCH_CL_MODE_2)
    )
  {
    if(class_type->weight_mode == ARAD_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW)
    {
      internal_class_type->wfqmode = ARAD_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW;
    }
  }

  
  if(class_type->mode == ARAD_SCH_CL_MODE_1)
  {
    internal_class_type->wfqmode = ARAD_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW;
  }

  switch(class_type->enhanced_mode)
  {
  case ARAD_CL_ENHANCED_MODE_DISABLED:
    internal_class_type->enh_clen = 0;
    internal_class_type->enh_clsphigh = 0;
    break;
  case ARAD_CL_ENHANCED_MODE_ENABLED_HP:
    internal_class_type->enh_clen = 1;
    internal_class_type->enh_clsphigh = 1;
    break;
  case ARAD_CL_ENHANCED_MODE_ENABLED_LP:
    internal_class_type->enh_clen = 1;
    internal_class_type->enh_clsphigh = 0;
    break;
  default:
    ARAD_SET_ERR_AND_EXIT(ARAD_SCH_ENH_MODE_OUT_OF_RANGE_ERR);
    break;
  }

exit:
  ARAD_RETURN;
}





#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
