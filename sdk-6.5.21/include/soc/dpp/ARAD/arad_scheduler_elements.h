/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SCHEDULER_ELEMENTS_H_INCLUDED__

#define __ARAD_SCHEDULER_ELEMENTS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>










#define ARAD_SCH_SE_IS_CL(se_id) SOC_SAND_IS_VAL_IN_RANGE(se_id, ARAD_CL_SE_ID_MIN,ARAD_CL_SE_ID_MAX)
#define ARAD_SCH_SE_IS_HR(se_id) SOC_SAND_IS_VAL_IN_RANGE(se_id, ARAD_HR_SE_ID_MIN,ARAD_HR_SE_ID_MAX)
#define ARAD_SCH_SE_IS_FQ(se_id) SOC_SAND_IS_VAL_IN_RANGE(se_id, ARAD_FQ_SE_ID_MIN,ARAD_FQ_SE_ID_MAX)

















uint32
  arad_sch_se_state_get(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     int                core,
    SOC_SAND_IN     ARAD_SCH_SE_ID     se_ndx,
    SOC_SAND_OUT    uint8              *is_se_enabled
  );


uint32
  arad_sch_se_state_set(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     int                core,
    SOC_SAND_IN     ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN     uint8                is_se_enabled
  );


uint32
  arad_sch_se_dual_shaper_get(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     int                core,
    SOC_SAND_IN     ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN     ARAD_SCH_SE_TYPE        se_type,
    SOC_SAND_OUT    uint8                *is_dual_shaper
  );


uint32
  arad_sch_se_dual_shaper_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     ARAD_SCH_FLOW_ID          se_ndx,
    SOC_SAND_IN     uint8                   is_dual_shaper
  );


uint32
  arad_sch_se_config_get(
    SOC_SAND_IN   int          unit,
    SOC_SAND_IN   int          core,
    SOC_SAND_IN   ARAD_SCH_SE_ID    se_ndx,
    SOC_SAND_IN   ARAD_SCH_SE_TYPE  se_type,
    SOC_SAND_OUT  ARAD_SCH_SE_INFO  *se
  );


uint32
  arad_sch_se_config_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core,
    SOC_SAND_IN  ARAD_SCH_SE_INFO     *se,
    SOC_SAND_IN  uint32              nof_subflows
  );


uint32
  arad_sch_se_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_SE_INFO      *se
  );


uint32
  arad_sch_se_get_unsafe(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   int         core,
    SOC_SAND_IN   ARAD_SCH_SE_ID    se_ndx,
    SOC_SAND_OUT  ARAD_SCH_SE_INFO  *se
  );


uint32
  arad_sch_se_set_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core,
    SOC_SAND_IN  ARAD_SCH_SE_INFO      *se,
    SOC_SAND_IN  uint32              nof_subflows
  );




uint32
  arad_sch_se_id_and_type_match_verify(
    ARAD_SCH_SE_ID                   se_id,
    ARAD_SCH_SE_TYPE                 se_type
  );


uint32
  arad_sch_class_type_params_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_INFO *class_type
  );


uint32
  arad_sch_class_type_params_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *exact_class_type
  );


uint32
  arad_sch_class_type_params_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *class_type
  );


uint32
  arad_sch_class_type_params_table_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_TABLE *sct
  );


uint32
  arad_sch_class_type_params_table_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_TABLE *sct,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *exact_sct
  );


uint32
  arad_sch_class_type_params_table_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *sct
  );






#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


