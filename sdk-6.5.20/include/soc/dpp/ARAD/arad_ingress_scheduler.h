/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_INGRESS_SCHEDULER_INCLUDED__

#define __ARAD_INGRESS_SCHEDULER_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_ingress_scheduler.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>





#define ARAD_ING_SCH_MAX_DELAY_VAL   0xFFFF
#define ARAD_ING_SCH_MIN_DELAY_VAL   4

#define ARAD_ING_SCH_MAX_CAL_VAL     0xFFFF
#define ARAD_ING_SCH_MIN_CAL_VAL     0
#define ARAD_ING_SCH_FIRST_CAL_VAL   1024

#define ARAD_ING_SCH_NUM_OF_CONTEXTS ARAD_ING_SCH_MESH_LAST

#define ARAD_ING_SCH_MAX_RATE_MIN 15
#define ARAD_ING_SCH_MAX_RATE_MAX 120000000











typedef struct {
    soc_reg_t   reg;
    soc_field_t field;
    int         index;
} reg_field;












soc_error_t
  arad_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_ingress_scheduler_mesh_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *exact_mesh_info
  );


uint32
  arad_ingress_scheduler_mesh_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *exact_mesh_info
  );


uint32
  arad_ingress_scheduler_mesh_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *mesh_info
  );


uint32
  arad_ingress_scheduler_clos_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
  );


uint32
  arad_ingress_scheduler_clos_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
  );


uint32
  arad_ingress_scheduler_clos_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *clos_info
  );



uint32
  arad_ingress_scheduler_clos_weights_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info
     );


uint32
   arad_ingress_scheduler_clos_global_shapers_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
     SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
    );


uint32
  arad_ingress_scheduler_shaper_values_set(
       SOC_SAND_IN  int                  unit,
       SOC_SAND_IN  int                     is_delay_2_clocks,
       SOC_SAND_IN  ARAD_ING_SCH_SHAPER     *shaper,
       SOC_SAND_IN  reg_field               *max_credit_fld,
       SOC_SAND_IN  reg_field               *delay_fld,
       SOC_SAND_IN  reg_field               *cal_fld,
       SOC_SAND_OUT uint32                  *exact_max_rate
      );



uint32
  arad_ingress_scheduler_rate_to_delay_cal_form(
     SOC_SAND_IN  int            unit,
     SOC_SAND_IN  uint32         max_rate,
     SOC_SAND_IN  int            is_delay_2_clocks,
     SOC_SAND_OUT uint32*        shaper_delay_2_clocks,
     SOC_SAND_OUT uint32*        shaper_cal,
     SOC_SAND_OUT uint32*        exact_max_rate
     );

uint32
  arad_ingress_scheduler_shaper_values_get(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 is_delay_2_clocks,
    SOC_SAND_IN   reg_field              *max_credit_fld,
    SOC_SAND_IN   reg_field              *delay_fld,
    SOC_SAND_IN   reg_field              *cal_fld,
    SOC_SAND_OUT  ARAD_ING_SCH_SHAPER    *shaper
  );


uint32
  arad_ingress_scheduler_delay_cal_to_max_rate_form(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          shaper_delay,
    SOC_SAND_IN  uint32          shaper_cal,
    SOC_SAND_OUT uint32          *max_rate
  );



uint32
  arad_ingress_scheduler_mesh_reg_flds_db_get(
    SOC_SAND_IN  int       unit,
    reg_field          wfq_weights[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    reg_field          shaper_max_crdts[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    reg_field          shaper_delays[ARAD_ING_SCH_NUM_OF_CONTEXTS],
    reg_field          shaper_cals[ARAD_ING_SCH_NUM_OF_CONTEXTS]
  );


uint32
   arad_ingress_scheduler_clos_weights_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
    );


uint32
   arad_ingress_scheduler_clos_global_shapers_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
    );


uint32
  arad_ingress_scheduler_exact_cal_value(
    SOC_SAND_IN uint32  cal_value,
    SOC_SAND_IN uint32  max_rate,
    SOC_SAND_IN uint32  exact_rate_in_kbits_per_sec,
    SOC_SAND_OUT uint32 *exact_cal_value_long
  );



uint8
  arad_ingress_scheduler_conversion_test(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  );


uint32
  arad_ingress_scheduler_clos_hp_shapers_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
    );


uint32
  arad_ingress_scheduler_clos_lp_shapers_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *exact_clos_info
    );


uint32
  arad_ingress_scheduler_clos_hp_shapers_get(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
  );


uint32
  arad_ingress_scheduler_clos_lp_shapers_get(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_OUT  ARAD_ING_SCH_CLOS_INFO   *clos_info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
