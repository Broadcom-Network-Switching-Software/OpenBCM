/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SCHEDULER_ELEMENT_CONVERTS_H_INCLUDED__

#define __ARAD_SCHEDULER_ELEMENT_CONVERTS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>


























SOC_SAND_RET
  arad_sch_INTERNAL_CLASS_TYPE_to_CLASS_TYPE_convert(
    SOC_SAND_IN     ARAD_SCH_SCT_TBL_DATA     *internal_class_type,
    SOC_SAND_OUT    ARAD_SCH_SE_CL_CLASS_INFO *class_type
  );
SOC_SAND_RET
  arad_sch_CLASS_TYPE_to_INTERNAL_CLASS_TYPE_convert(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   ARAD_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT  ARAD_SCH_SCT_TBL_DATA     *internal_class_type
  );

uint32
  arad_sch_INTERNAL_HR_MODE_to_HR_MODE_convert(
    SOC_SAND_IN     uint32                internal_hr_mode,
    SOC_SAND_OUT    ARAD_SCH_SE_HR_MODE  *hr_mode
  );

uint32
  arad_sch_HR_MODE_to_INTERNAL_HR_MODE_convert(
    SOC_SAND_IN    ARAD_SCH_SE_HR_MODE  hr_mode,
    SOC_SAND_OUT   uint32               *internal_hr_mode
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


