
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_INIT_INCLUDED__

#define __ARAD_PP_INIT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>













typedef enum
{
  
  ARAD_PP_INIT_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_INIT_FIRST,
  ARAD_PP_INIT_GET_ERRS_PTR,
  

  ARAD_PP_MGMT_FUNCTIONAL_INIT,
  ARAD_PP_MGMT_HW_SET_DEFAULTS,
  ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1,
  ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2,
  ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE,
  ARAD_PP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY,
  ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE,
  ARAD_PP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY,

  
  ARAD_PP_INIT_PROCEDURE_DESC_LAST
} ARAD_PP_INIT_PROCEDURE_DESC;

typedef enum
{
  
  

  
  ARAD_PP_INIT_ERR_LAST
} ARAD_PP_INIT_ERR;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint32 cpu_sys_port;

}ARAD_PP_INIT_PHASE1_CONF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  int unused;
}ARAD_PP_INIT_PHASE2_CONF;











uint32
  arad_pp_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE1_CONF    *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  arad_pp_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE1_CONF    *hw_adjust
  );


uint32
  arad_pp_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE2_CONF    *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  arad_pp_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE2_CONF    *hw_adjust
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_init_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_init_get_errs_ptr(void);



uint32
  arad_pp_mgmt_init_sequence_phase1(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE1_CONF    *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  );


uint32
  arad_pp_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_INIT_PHASE2_CONF    *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  );

void
  ARAD_PP_INIT_PHASE1_CONF_clear(
    SOC_SAND_OUT ARAD_PP_INIT_PHASE1_CONF *info
  );
  
void
  ARAD_PP_INIT_PHASE2_CONF_clear(
    SOC_SAND_OUT ARAD_PP_INIT_PHASE2_CONF *info
  );
  


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



