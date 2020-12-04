/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_EGR_PRGE_MGMT_INCLUDED__

#define __ARAD_EGR_PRGE_MGMT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/TMC/tmc_api_stack.h>

#include <soc/dpp/ARAD/arad_egr_prog_editor.h>








#define _ARAD_EGR_PRGE_MGMT_CE_DATA_SRC_NOP (255)










typedef struct {

   uint8 src_select;

   int   offset_base;
   uint8 offset_src;

   int   size_base;
   uint8 size_src;

   ARAD_EGR_PROG_EDITOR_LFEM lfem;

} ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION;


typedef struct {


    int op_value;

    uint8 op1;
    uint8 alu_action;
    uint8 op2;

    uint8 cmp_action;
    uint8 op3;

    uint8 dst_select;

} ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION;


typedef struct {
    
    ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION ce_interface_instruction;

    
    ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION alu_interface_instruction;

    
    char *doc_str;

} ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION;




typedef struct {
    char* graph;
    char* deploy;
} ARAD_PP_PRGE_MGMT_DIAG_PARAMS;










uint32
  arad_egr_prge_mgmt_init(
     int     unit
  );


uint32
  arad_egr_prge_mgmt_build_program_start(
     int     unit,
     ARAD_EGR_PROG_EDITOR_PROGRAMS   program
  );


uint32
  arad_egr_prge_mgmt_instr_add(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION    *ifc_instr
  );


uint32
  arad_egr_prge_mgmt_build_program_end(
     int     unit
  );


uint32
  arad_egr_prge_mgmt_program_jump_point_add(
     int     unit,
     ARAD_EGR_PROG_EDITOR_JUMP_POINT jump_point
  );


uint32
  arad_egr_prge_mgmt_current_program_load_only_set(
     int     unit
  );


uint32
  arad_egr_prge_mgmt_build_branch_start(
     int     unit,
     ARAD_EGR_PROG_EDITOR_BRANCH    branch
  );


uint32
  arad_egr_prge_mgmt_branch_usage_add(
     int     unit,
     ARAD_EGR_PROG_EDITOR_JUMP_POINT    jump_point
  );


uint32
  arad_egr_prge_mgmt_build_branch_end(
     int     unit
  );


int
  arad_egr_prge_mgmt_program_management_main(
     int     unit
  );


uint32
  arad_egr_prge_mgmt_branches_load(
     int     unit
  );



int
  arad_egr_prge_mgmt_diag_print(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_PRGE_MGMT_DIAG_PARAMS *key_params
   );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

