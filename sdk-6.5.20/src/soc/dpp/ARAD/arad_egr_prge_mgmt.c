/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS






#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_stack.h>

#include <soc/dpp/ARAD/arad_egr_prge_mgmt.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_egr_prge_interface.h>

#include <soc/dpp/ARAD/arad_sw_db.h>








#define ARAD_EGR_PRGE_MGMT_MAX_NOF_LFEMS_PER_PROGRAM 4




#define ARAD_EGR_PRGE_MGMT_MAX_NOF_PERMUTATIONS_PER_PROGRAM 16*4 





#define _ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R0           0
#define _ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R1           1
#define _ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R2           2
#define _ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R3           3
#define _ARAD_EGR_PRGE_MGMT_DEP_OFFSET_RSZ          4
#define _ARAD_EGR_PRGE_MGMT_DEP_OFFSET_CAT_SIZE     5


#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_R0             _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R0)
#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_R1             _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R1)
#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_R2             _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R2)
#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_R3             _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_ARAD_EGR_PRGE_MGMT_DEP_OFFSET_R3)
#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_RSZ            _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_ARAD_EGR_PRGE_MGMT_DEP_OFFSET_RSZ)
#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_CAT_SIZE       _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_ARAD_EGR_PRGE_MGMT_DEP_OFFSET_CAT_SIZE)





#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

#define _ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(_unit, _program) \
            (mgmt_programs[(_unit)][(_program)].management_type == ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_MANAGED)

#define _ARAD_EGR_PRGE_MGMT_IS_PROGRAM_LOADED_ONLY(_unit, _program) \
            (mgmt_programs[(_unit)][(_program)].management_type == ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_LOAD_ONLY)

#define _ARAD_EGR_PRGE_MGMT_IS_PROGRAM_OLD_INTERFACE(_unit, _program) \
            (mgmt_programs[(_unit)][(_program)].management_type == ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED)

#define _ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTR_IS_NOP(p_ce_interface_instr) (((p_ce_interface_instr)->src_select) == _ARAD_EGR_PRGE_MGMT_CE_DATA_SRC_NOP)
#define _ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTR_IS_NOP(p_alu_interface_instr) (((p_alu_interface_instr)->cmp_action) == ARAD_EGR_PROG_EDITOR_ALU_CMP_NONE)

#define _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET(_offset) (1<<(_offset))

#define _ARAD_EGR_PRGE_MGMT_MEM_LSB_FROM_ENTRY_GET(_unit, _entry) \
            (((_entry)>>1) & (SOC_IS_JERICHO(_unit) ? 0x3 : 0x1 ))

#define _ARAD_EGR_PRGE_MGMT_IS_INSTR_MEM_LSB_IN_BMP(_unit, _program, _instr) \
            (mgmt_programs[(_unit)][(_program)].graph[(_instr)].ce.anchor_ce_instr_mem_bmp & \
                (1 << _ARAD_EGR_PRGE_MGMT_MEM_LSB_FROM_ENTRY_GET((_unit), mgmt_programs[(_unit)][(_program)].graph[(_instr)].selected_entry)))

#define _ARAD_EGR_PRGE_MGMT_INSTR_USE_LFEM(_unit, _program, _instr) \
            (!(mgmt_programs[(_unit)][(_program)].graph[(_instr)].ce.is_nop)) \
             && (mgmt_programs[(_unit)][(_program)].graph[(_instr)].ce.ce_ifc.lfem < ARAD_EGR_PROG_EDITOR_LFEM_NULL)


#define _ARAD_EGR_PRGE_MGMT_CE_NOP_INSTR_LOAD(_unit, _instr_tbl)    \
do {                                                                \
    (_instr_tbl).src_select  = 0;                                   \
    (_instr_tbl).offset_base = 0;                                   \
    (_instr_tbl).offset_src  = 0;                                   \
    (_instr_tbl).size_src  = ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0; \
    (_instr_tbl).size_base   = 0;                                   \
    (_instr_tbl).fem_select  = ARAD_PP_EG_PROG_NOF_FEM(_unit);      \
} while (0)

#define _ARAD_EGR_PRGE_INTERFACE_ALU_NOP_INSTR_LOAD(_unit, _instr_tbl)    \
do {                                                                \
    (_instr_tbl).op_1_field_select  = 0;                            \
    (_instr_tbl).op_value           = 0;                            \
    (_instr_tbl).op_2_field_select  = 0;                            \
    (_instr_tbl).alu_action         = ARAD_PP_EG_PROG_ALU_SUB;      \
    (_instr_tbl).op_3_field_select  = 0;                            \
    (_instr_tbl).cmp_action         = 0;                            \
    (_instr_tbl).dst_select         = 0;                            \
} while (0)


#define _ARAD_EGR_PRGE_MGMT_LOG(_unit, _text) \
                LOG_VERBOSE(BSL_LS_SOC_EGRESS, (BSL_META_U((_unit),"        ++ %d: PRGE MGMT - " _text),(_unit)))

#define _ARAD_EGR_PRGE_MGMT_LOG_PARAM(_unit, _text, _param) \
                LOG_VERBOSE(BSL_LS_SOC_EGRESS, (BSL_META_U((_unit),"        ++ %d: PRGE MGMT - " _text),(_unit),(_param)))





typedef struct {
    uint32 prog_index;
    lfem_maps_shadow_t  *program_permutations; 
    uint8 valid_permutation_number; 
} single_program_permutations_t;


typedef struct {

    ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION ce_ifc;

    uint8 is_nop;

    uint8 size_change;

    
    uint8  reg_dep_instr[8]; 
    int    last_reg_dep;
    int    reg_immediate_dep; 
    int    size_dep;

    
    int anchor_ce_instr_mem_bmp; 
    int anchor_instr_entry_range_min;
    int anchor_instr_entry_range_max;

} _ARAD_EGR_PRGE_MGMT_CE_INSTRUCTION;


typedef struct {

    ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION alu_ifc;

    uint8  is_nop;

    uint32 dep_flags;
    uint32 cond_dep_flags;

    
    uint8  reg_dep_instr[8]; 
    int    last_reg_dep;
    int    reg_immediate_dep; 
    int    lfem_dep;

    
    int anchor_instr_entry_range_min;
    int anchor_instr_entry_range_max;

} _ARAD_EGR_PRGE_MGMT_ALU_INSTRUCTION;


typedef struct {

    int selected_entry;
    char *doc;

    _ARAD_EGR_PRGE_MGMT_CE_INSTRUCTION ce;
    _ARAD_EGR_PRGE_MGMT_ALU_INSTRUCTION alu;

} _ARAD_EGR_PRGE_MGMT_INSTRUCTION;



typedef enum {

    ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED = 0,  
    ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_LOAD_ONLY,  
    ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_MANAGED,  

    
    ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOF
} ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE;


typedef struct {

    
    ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE management_type;

    
    uint32 instructions_nof;

    
    ARAD_EGR_PROG_EDITOR_JUMP_POINT jump_points[ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF];
    uint8 nof_jump_points;

    
    _ARAD_EGR_PRGE_MGMT_INSTRUCTION graph[ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS];

} _ARAD_EGR_PRGE_MGMT_PROGRAM;



typedef struct {

    
    ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE management_type;

    
    uint32 instructions_nof;

    
    uint8 jump_out;

    
    uint8 branch_uses[ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF];

    
    ARAD_EGR_PROG_EDITOR_JUMP_POINT jump_points[ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF];
    uint8 nof_jump_points;

    
    _ARAD_EGR_PRGE_MGMT_INSTRUCTION graph[ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS];

} _ARAD_EGR_PRGE_MGMT_BRANCH;







STATIC ARAD_EGR_PROG_EDITOR_PROGRAMS current_program[SOC_MAX_NUM_DEVICES];


STATIC _ARAD_EGR_PRGE_MGMT_PROGRAM mgmt_programs[SOC_MAX_NUM_DEVICES][ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS];




STATIC ARAD_EGR_PROG_EDITOR_BRANCH current_branch[SOC_MAX_NUM_DEVICES];


STATIC int mgmt_branch_usage[SOC_MAX_NUM_DEVICES][ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF][2];


STATIC _ARAD_EGR_PRGE_MGMT_BRANCH mgmt_branches[SOC_MAX_NUM_DEVICES][ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF];







void _ARAD_EGR_PRGE_MGMT_CE_INSTRUCTION_init(_ARAD_EGR_PRGE_MGMT_CE_INSTRUCTION * p_ce_instr) {
    p_ce_instr->last_reg_dep                    = -1;
    p_ce_instr->reg_immediate_dep               = -1;
    p_ce_instr->size_dep                        = -1;
    p_ce_instr->anchor_instr_entry_range_min    = -1;
    p_ce_instr->anchor_instr_entry_range_max    = -1;
    p_ce_instr->anchor_ce_instr_mem_bmp         = 0;
}


void _ARAD_EGR_PRGE_MGMT_ALU_INSTRUCTION_init(_ARAD_EGR_PRGE_MGMT_ALU_INSTRUCTION * p_alu_instr) {
    p_alu_instr->last_reg_dep                   = -1;
    p_alu_instr->reg_immediate_dep              = -1;
    p_alu_instr->lfem_dep                       = -1;
    p_alu_instr->anchor_instr_entry_range_min   = -1;
    p_alu_instr->anchor_instr_entry_range_max   = -1;
}


void _ARAD_EGR_PRGE_MGMT_INSTRUCTION_clear(_ARAD_EGR_PRGE_MGMT_INSTRUCTION* p_instr) {
    sal_memset(p_instr, 0, sizeof(_ARAD_EGR_PRGE_MGMT_INSTRUCTION));
    p_instr->selected_entry                 = -1;
    _ARAD_EGR_PRGE_MGMT_CE_INSTRUCTION_init(&(p_instr->ce));
    _ARAD_EGR_PRGE_MGMT_ALU_INSTRUCTION_init(&(p_instr->alu));
}


void _ARAD_EGR_PRGE_MGMT_PROGRAM_init(_ARAD_EGR_PRGE_MGMT_PROGRAM *p_program) {
    uint32 j;

    p_program->management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED;

    p_program->instructions_nof = 0;
    p_program->nof_jump_points = 0;

    for (j = 0; j < ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS; j++) {
        _ARAD_EGR_PRGE_MGMT_INSTRUCTION_clear(&(p_program->graph[j]));
    }
}


void _ARAD_EGR_PRGE_MGMT_BRANCH_init(_ARAD_EGR_PRGE_MGMT_BRANCH *p_branch) {
    uint32 j;

    p_branch->management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED;

    p_branch->instructions_nof = 0;
    p_branch->jump_out = 0;
    p_branch->nof_jump_points = 0;

    for (j = 0; j < ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF; j++) {
        p_branch->branch_uses[j] = 0;
    }

    for (j = 0; j < ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS; j++) {
        _ARAD_EGR_PRGE_MGMT_INSTRUCTION_clear(&(p_branch->graph[j]));
    }
}



uint32
  arad_egr_prge_mgmt_init(
     int     unit
  ){

    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    current_program[unit] = ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS;
    current_branch[unit] = ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF;

    for (i = 0; i < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; i++) {
        _ARAD_EGR_PRGE_MGMT_PROGRAM_init(&(mgmt_programs[unit][i]));
    }

    for (i = 0; i < ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF; i++) {
        _ARAD_EGR_PRGE_MGMT_BRANCH_init(&(mgmt_branches[unit][i]));
    }

    for (i = 0; i < ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF; i++) {
        
        mgmt_branch_usage[unit][i][0] = ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS;
        mgmt_branch_usage[unit][i][1] = -1;
    }

    arad_egr_prge_interface_field_available_by_device_init(unit);

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_init()",0,0);
}


uint32
  arad_egr_prge_mgmt_build_program_start(
     int     unit,
     ARAD_EGR_PROG_EDITOR_PROGRAMS   program
  ){

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (current_program[unit] != ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) {
        
        char *next_name, *cur_name;
        arad_egr_prog_editor_prog_name_get_by_id(current_program[unit], &cur_name);
        arad_egr_prog_editor_prog_name_get_by_id(program, &next_name);

        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - Can't start %s, Previus program(%s) hasn't ended yet"),
                                next_name, cur_name));
    }

    if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(unit, program)) {
        
        char *next_name;
        arad_egr_prog_editor_prog_name_get_by_id(program, &next_name);

        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - Program %s already loaded"),
                                next_name));
    }

    
    current_program[unit] = program;
    mgmt_programs[unit][program].instructions_nof = 0;
    mgmt_programs[unit][program].management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_build_program_start()",0,0);
}


uint32
  _arad_egr_prge_mgmt_ce_reg_dependency_get(
     int     unit,
     ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION    *ce_interface_instr,
     uint32  *dep_res
  ){
    uint32 result = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (ce_interface_instr->offset_src < ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0) {
        
        result |= _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((ce_interface_instr->offset_src - ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_0));
    }

    if (ce_interface_instr->size_src < ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0) {
        
        result |= _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((ce_interface_instr->size_src - ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_0));
    }

    

    *dep_res = result;

    SOC_SAND_EXIT_NO_ERROR;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_ce_instr_add()",0,0);
}



uint32
  _arad_egr_prge_mgmt_alu_reg_dependency_get(
     int     unit,
     ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION    *alu_interface_instr,
     uint32  *dep_res
  ){
    uint32 result = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (alu_interface_instr->op1 <= ARAD_PP_PRGE_ENG_OP_REG_RSZ) {
        
        result |= _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((alu_interface_instr->op1 - ARAD_PP_PRGE_ENG_OP_REG_R0));
    }

    if (alu_interface_instr->op2 <= ARAD_PP_PRGE_ENG_OP_REG_RSZ) {
        
        result |= _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((alu_interface_instr->op2 - ARAD_PP_PRGE_ENG_OP_REG_R0));
    }

    if (alu_interface_instr->op3 <= ARAD_PP_PRGE_ENG_OP_REG_RSZ) {
        
        result |= _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((alu_interface_instr->op3 - ARAD_PP_PRGE_ENG_OP_REG_R0));
    }

    *dep_res = result;

    SOC_SAND_EXIT_NO_ERROR;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_ce_instr_add()",0,0);
}


uint32
  _arad_egr_prge_mgmt_prog_ce_instr_add(
     int     unit,
     ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION    *ce_interface_instr
  ){

    uint32 res;
    ARAD_EGR_PROG_EDITOR_PROGRAMS prog = current_program[unit];
    uint32 next_instr = mgmt_programs[unit][prog].instructions_nof;

    uint32 reg_dep_vector;

    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (_ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTR_IS_NOP(ce_interface_instr)) {
        
        mgmt_programs[unit][prog].graph[next_instr].ce.is_nop = 1;
        SOC_SAND_EXIT_NO_ERROR;
    }

    
    sal_memcpy(&(mgmt_programs[unit][prog].graph[next_instr].ce.ce_ifc), ce_interface_instr,
               sizeof(ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION));

    if (next_instr > 0) { 
        if ((ce_interface_instr->size_src != ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0)
            || (ce_interface_instr->size_base)) {
            
            for (i = next_instr - 1; i >= 0; i--) {
                if (mgmt_programs[unit][prog].graph[i].ce.size_change) {
                    mgmt_programs[unit][prog].graph[next_instr].ce.size_dep = i;
                    break;
                }
            }
        }

        res = _arad_egr_prge_mgmt_ce_reg_dependency_get(unit, ce_interface_instr, &reg_dep_vector);
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

        if (reg_dep_vector & mgmt_programs[unit][prog].graph[next_instr - 1].alu.dep_flags) {
            
            mgmt_programs[unit][prog].graph[next_instr].ce.reg_immediate_dep = next_instr - 1;
        }

        
        for (i = next_instr - 2; (i >= 0) && reg_dep_vector; i--) {
            if (reg_dep_vector & mgmt_programs[unit][prog].graph[i].alu.dep_flags) {
                
                mgmt_programs[unit][prog].graph[next_instr].ce
                    .reg_dep_instr[ ++mgmt_programs[unit][prog].graph[next_instr].ce.last_reg_dep ] = i;
                reg_dep_vector &= ~(mgmt_programs[unit][prog].graph[i].alu.dep_flags);
            }
            else if (reg_dep_vector & mgmt_programs[unit][prog].graph[i].alu.cond_dep_flags) {
                
                mgmt_programs[unit][prog].graph[next_instr].ce
                    .reg_dep_instr[ ++mgmt_programs[unit][prog].graph[next_instr].ce.last_reg_dep ] = i;
            }
        }

        


    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_ce_instr_add()",0,0);
}


uint32
  _arad_egr_prge_mgmt_prog_alu_instr_add(
     int     unit,
     ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION    *alu_interface_instr
  ){

    uint32 res;
    ARAD_EGR_PROG_EDITOR_PROGRAMS prog = current_program[unit];
    uint32 next_instr = mgmt_programs[unit][prog].instructions_nof;

    uint32 reg_dep_vector;

    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (_ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTR_IS_NOP(alu_interface_instr)) {
        
        mgmt_programs[unit][prog].graph[next_instr].alu.is_nop = 1;
        SOC_SAND_EXIT_NO_ERROR;
    }

    
    sal_memcpy(&(mgmt_programs[unit][prog].graph[next_instr].alu.alu_ifc), alu_interface_instr,
               sizeof(ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION));

    
    if ((alu_interface_instr->op1 == ARAD_PP_PRGE_ENG_OP_FEM)
        || (alu_interface_instr->op2 == ARAD_PP_PRGE_ENG_OP_FEM)) {
        if (next_instr < 2) {
            char *prog_name;
            arad_egr_prog_editor_prog_name_get_by_id(prog, &prog_name);
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - LFEM as ALU input in instruction %d (Program: %s)"),
                                    next_instr , prog_name));
        }
        mgmt_programs[unit][prog].graph[next_instr].alu.lfem_dep = next_instr-2;
    }

    if (next_instr > 0) { 

        res = _arad_egr_prge_mgmt_alu_reg_dependency_get(unit, alu_interface_instr, &reg_dep_vector);
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

        if (reg_dep_vector & mgmt_programs[unit][prog].graph[next_instr - 1].alu.dep_flags) {
            
            mgmt_programs[unit][prog].graph[next_instr].alu.reg_immediate_dep = next_instr - 1;
        }

        
        for (i = next_instr - 2; (i >= 0) && reg_dep_vector; i--) {
            if (reg_dep_vector & mgmt_programs[unit][prog].graph[i].alu.dep_flags) {
                
                mgmt_programs[unit][prog].graph[next_instr]
                    .alu.reg_dep_instr[ ++mgmt_programs[unit][prog].graph[next_instr].alu.last_reg_dep ] = i;
                reg_dep_vector &= ~(mgmt_programs[unit][prog].graph[i].alu.dep_flags);
            }
            else if (reg_dep_vector & mgmt_programs[unit][prog].graph[i].alu.cond_dep_flags) {
                
                mgmt_programs[unit][prog].graph[next_instr]
                    .alu.reg_dep_instr[ ++mgmt_programs[unit][prog].graph[next_instr].alu.last_reg_dep ] = i;
            }
        }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_ce_instr_add()",0,0);
}


uint32
  _arad_egr_prge_mgmt_dep_flags_set(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION    *ifc_instr
  ){

    const ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION    *ce_interface_instr = &(ifc_instr->ce_interface_instruction);
    const ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION   *alu_interface_instr = &(ifc_instr->alu_interface_instruction);

    ARAD_EGR_PROG_EDITOR_PROGRAMS prog = current_program[unit];
    uint32 next_instr = mgmt_programs[unit][prog].instructions_nof;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if ((!_ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTR_IS_NOP(alu_interface_instr))
        && (alu_interface_instr->dst_select <= ARAD_PP_PRGE_ENG_OP_REG_RSZ)) {
        
        if (alu_interface_instr->cmp_action == ARAD_EGR_PROG_EDITOR_ALU_CMP_ALL) {
            mgmt_programs[unit][prog].graph[next_instr].alu.dep_flags |=
                _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((alu_interface_instr->dst_select - ARAD_PP_PRGE_ENG_OP_REG_R0));
        }
        else if (alu_interface_instr->cmp_action != ARAD_EGR_PROG_EDITOR_ALU_CMP_NONE) {
            mgmt_programs[unit][prog].graph[next_instr].alu.cond_dep_flags
                |= _ARAD_EGR_PRGE_MGMT_DEP_FLAG_FROM_OFFSET((alu_interface_instr->dst_select - ARAD_PP_PRGE_ENG_OP_REG_R0));
        }
    }

    if ((!_ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTR_IS_NOP(ce_interface_instr))
        && ((ce_interface_instr->size_src != ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0)
            || (ce_interface_instr->size_base != 0))){
        mgmt_programs[unit][prog].graph[next_instr].ce.size_change = 1;
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_dep_flags_set()",0,0);
}


uint32
  _arad_egr_prge_mgmt_prog_instr_add(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION    *ifc_instr
  ){
    uint32 res;

    ARAD_EGR_PROG_EDITOR_PROGRAMS prog = current_program[unit];
    uint32 next_instr = mgmt_programs[unit][prog].instructions_nof;

    ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION    *ce_interface_instr = &(ifc_instr->ce_interface_instruction);
    ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION   *alu_interface_instr = &(ifc_instr->alu_interface_instruction);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (prog == ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - Can't add instruction - no program is started")));
    }
    if (mgmt_programs[unit][current_program[unit]].instructions_nof >= ARAD_PP_EG_PROG_NOF_INSTR(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Can't add instruction - too many instructions in the program")));
    }

    
    res = _arad_egr_prge_mgmt_prog_ce_instr_add(unit, ce_interface_instr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
    res = _arad_egr_prge_mgmt_prog_alu_instr_add(unit, alu_interface_instr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

    
    mgmt_programs[unit][prog].graph[next_instr].doc = ifc_instr->doc_str;

    
    res = _arad_egr_prge_mgmt_dep_flags_set(unit, ifc_instr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

    
    mgmt_programs[unit][prog].graph[next_instr].selected_entry = mgmt_programs[unit][current_program[unit]].instructions_nof++;

    
    if (mgmt_programs[unit][current_program[unit]].instructions_nof == ARAD_PP_EG_PROG_NOF_INSTR(unit)) {
        mgmt_programs[unit][current_program[unit]].management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_LOAD_ONLY;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_prog_instr_add()",0,0);
}


uint32
  _arad_egr_prge_mgmt_branch_instr_add(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION    *ifc_instr
  ){

    ARAD_EGR_PROG_EDITOR_BRANCH branch = current_branch[unit];
    uint32 next_instr = mgmt_branches[unit][branch].instructions_nof;

    ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION    *ce_interface_instr = &(ifc_instr->ce_interface_instruction);
    ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION   *alu_interface_instr = &(ifc_instr->alu_interface_instruction);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (mgmt_branches[unit][branch].instructions_nof >= (ARAD_PP_EG_PROG_NOF_INSTR(unit)-4)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Can't add instruction - too many instructions in the branch")));
    }

    
    
    
    if (_ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTR_IS_NOP(ce_interface_instr)) {
        
        mgmt_branches[unit][branch].graph[next_instr].ce.is_nop = 1;
    }
    else {
        
        sal_memcpy(&(mgmt_branches[unit][branch].graph[next_instr].ce.ce_ifc), ce_interface_instr,
                   sizeof(ARAD_EGR_PRGE_MGMT_CE_INTERFACE_INSTRUCTION));
    }
    
    if (_ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTR_IS_NOP(alu_interface_instr)) {
        
        mgmt_branches[unit][branch].graph[next_instr].alu.is_nop = 1;
    }
    else {
        
        sal_memcpy(&(mgmt_branches[unit][branch].graph[next_instr].alu.alu_ifc), alu_interface_instr,
                   sizeof(ARAD_EGR_PRGE_MGMT_ALU_INTERFACE_INSTRUCTION));
    }

    
    mgmt_branches[unit][branch].graph[next_instr].doc = ifc_instr->doc_str;


    
    mgmt_branches[unit][branch].graph[next_instr].selected_entry = mgmt_branches[unit][branch].instructions_nof++;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_branch_instr_add()",0,0);
}


uint32
  arad_egr_prge_mgmt_instr_add(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION    *ifc_instr
  ){

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (current_program[unit] < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) {
        SOC_SAND_CHECK_FUNC_RESULT(_arad_egr_prge_mgmt_prog_instr_add(unit,ifc_instr), 370, exit);
    }
    else if (current_branch[unit] < ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF) {
        SOC_SAND_CHECK_FUNC_RESULT(_arad_egr_prge_mgmt_branch_instr_add(unit,ifc_instr), 370, exit);
    }
    else {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Not in program or branch. can't add instruction.")));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_instr_add()",0,0);
}



uint32
  arad_egr_prge_mgmt_build_program_end(
     int     unit
  ){

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (current_program[unit] == ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - Can't end a program - no program is started")));
    }

    
    if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_OLD_INTERFACE(unit, current_program[unit])) {
        
        mgmt_programs[unit][current_program[unit]].management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_MANAGED;
    }
    current_program[unit] = ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_build_program_end()",0,0);
}


uint32
  arad_egr_prge_mgmt_program_jump_point_add(
     int     unit,
     ARAD_EGR_PROG_EDITOR_JUMP_POINT jump_point
  ){
    int
        current_instr;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if ((current_program[unit] == ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS)
        && (current_branch[unit] == ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - Not in a program - can't add jump point")));
    }
    if (jump_point >= ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internal error - Undefined jump point ID")));
    }

    
    if (current_branch[unit] == ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF) {
        
        _ARAD_EGR_PRGE_MGMT_PROGRAM *program = &mgmt_programs[unit][current_program[unit]];

        current_instr = program->instructions_nof;

        if (program->nof_jump_points < ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF) {
            
            program->jump_points[program->nof_jump_points++] = jump_point;
        }
        else {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Too many jumps from a single program")));
        }

         
        program->management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_LOAD_ONLY;
    }
    else {
        
        _ARAD_EGR_PRGE_MGMT_BRANCH *branch = &mgmt_branches[unit][current_branch[unit]];

        current_instr = branch->instructions_nof;

        if (branch->nof_jump_points < ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF) {
            
            branch->jump_points[branch->nof_jump_points] = jump_point;
        }
        else {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Too many jumps from a single branch")));
        }
    }

    if (current_instr < mgmt_branch_usage[unit][jump_point][0]) {
        mgmt_branch_usage[unit][jump_point][0] = current_instr;
    }
    if (current_instr > mgmt_branch_usage[unit][jump_point][1]) {
        mgmt_branch_usage[unit][jump_point][1] = current_instr;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_program_branch_usage_add()",jump_point,0);
}


uint32
  arad_egr_prge_mgmt_current_program_load_only_set(
     int     unit
  ){
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (current_program[unit] < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) {
        
        mgmt_programs[unit][current_program[unit]].management_type = ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_LOAD_ONLY;
    }
    else if (current_branch[unit] < ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF){
        
        
        SOC_SAND_EXIT_NO_ERROR;
    }
    else {
        
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Error - not in a branch or a program")));
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_current_program_load_only_set()",0,0);
}


uint32
  arad_egr_prge_mgmt_build_branch_start(
     int     unit,
     ARAD_EGR_PROG_EDITOR_BRANCH    branch
  ){

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (current_branch[unit] != ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Branch build error - branch already started")));
    }
    if (mgmt_branches[unit][branch].management_type != ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Branch build error - branch already loaded")));
    }

    
    current_branch[unit] = branch;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_build_branch_start()",branch,0);
}

uint32
  arad_egr_prge_mgmt_branch_usage_add(
     int     unit,
     ARAD_EGR_PROG_EDITOR_JUMP_POINT    jump_point 
  ){
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (current_branch[unit] == ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Branch error - No branch started to add usage to")));
    }
    if (jump_point >= ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Branch error - Invalid usage code")));
    }

    
    mgmt_branches[unit][current_branch[unit]].branch_uses[jump_point] = 1;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_branch_usage_add()",jump_point,0);
}


uint32
  arad_egr_prge_mgmt_build_branch_end(
     int     unit
  ){
    uint32 i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (current_branch[unit] == ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Branch error - No branch started")));
    }

    for (i = 0; i < ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF; i++) {
        
        if (mgmt_branches[unit][current_branch[unit]].branch_uses[i]) {
            
            mgmt_branches[unit][current_branch[unit]].management_type =  ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_LOAD_ONLY; 
            current_branch[unit] = ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF;
            SOC_SAND_EXIT_NO_ERROR;
        }
    }

    
    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Branch error - No jump point defined for the branch. Branch cannot be loaded.")));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_build_branch_end()",0,0);
}


uint32
  _arad_egr_prge_mgmt_program_init(
     int     unit,
     ARAD_EGR_PROG_EDITOR_PROGRAMS   program
  ){
    int instr;
    uint32 instr_nof = mgmt_programs[unit][program].instructions_nof;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (instr = 0; instr < instr_nof; instr++) {
        
        mgmt_programs[unit][program].graph[instr].ce.anchor_ce_instr_mem_bmp = 0;
        
        mgmt_programs[unit][program].graph[instr].selected_entry = instr;
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_program_init()",0,0);
}


uint32
  _arad_egr_prge_mgmt_lfem_buffer_find(
     int     unit,
     ARAD_EGR_PROG_EDITOR_LFEM   lfem,
     lfem_maps_shadow_t          lfem_shadow,
     uint32                     *buffers,
     uint32                     *num_res
  ) {
    uint32 i,j;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(lfem_shadow);
    SOC_SAND_CHECK_NULL_INPUT(buffers);
    SOC_SAND_CHECK_NULL_INPUT(num_res);

    if (lfem >= ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Internal error - Invalid LFEM")));
    }

    *num_res = 0;

    for (i = 0; i < ARAD_PP_EG_PROG_NOF_LFEM_TABLES(unit); i++) {
        for (j = 0; j < ARAD_PP_EG_PROG_NOF_FEM(unit); j++) {
            if (lfem == lfem_shadow[i][j]) {
                buffers[*num_res] = i;
                (*num_res)++;
                break;
            }
        }
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_lfem_buffer_find()",0,0);
}


uint32
  _arad_egr_prge_mgmt_lfem_option_add(
    int     unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
    uint32  instruction,
    uint32  mem_lsb
  ){

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    mgmt_programs[unit][program].graph[instruction].ce.anchor_ce_instr_mem_bmp |= (1<<mem_lsb);

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_prge_mgmt_lfem_option_add()",0,0);
}


uint32
  _arad_egr_prge_mgmt_program_anchors_update(
    int     unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
    lfem_maps_shadow_t          lfem_shadow
  ){
    uint32 res;
    int instr;

    uint32 buffers[ARAD_EGR_PRGE_MAX_NOF_LFEM_TABLES];
    uint32 num_res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (instr = 0; instr < mgmt_programs[unit][program].instructions_nof; instr++) {
        if (_ARAD_EGR_PRGE_MGMT_INSTR_USE_LFEM(unit, program, instr)) {
            
            res = _arad_egr_prge_mgmt_lfem_buffer_find(unit, mgmt_programs[unit][program].graph[instr].ce.ce_ifc.lfem,
                                                       lfem_shadow, buffers, &num_res);
            SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

            if (num_res == 0) { 
                SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Required LFEM wasn't loaded")));
            }

            while (num_res) {
                num_res--;
                res = _arad_egr_prge_mgmt_lfem_option_add(unit, program, instr, buffers[num_res]);
                SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
            }
        }
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_program_anchors_update()",0,0);
}


uint32
  _arad_egr_prge_mgmt_program_entries_by_lfems_adjust(
    int     unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
    uint8                           error_on_fail
  ){
    int instr;
    int i, highest_to_move;
    int instr_added=0;

    uint32 instr_nof = mgmt_programs[unit][program].instructions_nof;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (instr = 0; instr < instr_nof; instr++) {
        if (_ARAD_EGR_PRGE_MGMT_INSTR_USE_LFEM(unit, program, instr)) {

            highest_to_move = instr;

            if (!_ARAD_EGR_PRGE_MGMT_IS_INSTR_MEM_LSB_IN_BMP(unit, program, instr)) {
                
                while ((mgmt_programs[unit][program].graph[highest_to_move].alu.lfem_dep != -1)
                       || (mgmt_programs[unit][program].graph[highest_to_move+1].alu.lfem_dep != -1)
                       || (mgmt_programs[unit][program].graph[highest_to_move].ce.reg_immediate_dep != -1)
                       || (mgmt_programs[unit][program].graph[highest_to_move].alu.reg_immediate_dep != -1)) {
                    highest_to_move--;
                }

                while (!_ARAD_EGR_PRGE_MGMT_IS_INSTR_MEM_LSB_IN_BMP(unit, program, instr)) {
                    

                    if (instr_nof + instr_added < ARAD_PP_EG_PROG_NOF_INSTR(unit)) {

                        
                        for (i = highest_to_move; i < instr_nof; i++) {
                            mgmt_programs[unit][program].graph[i].selected_entry++;
                        }
                        instr_added++;
                    } else {
                        if (error_on_fail) {
                            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Can't deploy with current lfem arrangement")));
                        }
                        else {
                            return SOC_SAND_ERR;
                        }
                    }
                }
            }

            if (highest_to_move < instr) {
                
                instr = highest_to_move-1;
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_program_entries_by_anchors_adjust()", 0, 0);
}


uint32
  _arad_egr_pge_mgmt_null_instruction_load(
     int     unit,
     int     mem,
     int     entry
  ){
    uint32 res;
    ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA
        prge_instruction_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    _ARAD_EGR_PRGE_MGMT_CE_NOP_INSTR_LOAD(unit, prge_instruction_tbl_data);
    _ARAD_EGR_PRGE_INTERFACE_ALU_NOP_INSTR_LOAD(unit, prge_instruction_tbl_data);
    res = arad_pp_epni_prge_instruction_tbl_set_unsafe(unit, mem, entry, &prge_instruction_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_pge_mgmt_null_instruction_load()",mem,entry);
}


uint32
  _arad_egr_prge_mgmt_instruction_load(
     int     unit,
     ARAD_EGR_PROG_EDITOR_PROGRAMS       *program_list,
     uint8                               num_programs,
     _ARAD_EGR_PRGE_MGMT_INSTRUCTION     *instruction,
     int     mem,
     int     entry
  ){
    uint32 res;
    uint8 i;

    ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA
        prge_instruction_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (num_programs == 0) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE internale error - Loaded instruction for no program")));
    }

    
    if (!(instruction->ce.is_nop)) {
        prge_instruction_tbl_data.src_select  = instruction->ce.ce_ifc.src_select;
        prge_instruction_tbl_data.offset_base = instruction->ce.ce_ifc.offset_base;
        prge_instruction_tbl_data.offset_src  = instruction->ce.ce_ifc.offset_src;
        prge_instruction_tbl_data.size_src    = instruction->ce.ce_ifc.size_src;
        prge_instruction_tbl_data.size_base   = instruction->ce.ce_ifc.size_base;
        for (i = 0; i < num_programs; i++) {
            res = arad_egr_prog_editor_supply_lfem_idx(unit, instruction->ce.ce_ifc.lfem, mem, program_list[i],
                                                       &(prge_instruction_tbl_data.fem_select));
            SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
        }
    }
    else {
        _ARAD_EGR_PRGE_MGMT_CE_NOP_INSTR_LOAD(unit, prge_instruction_tbl_data);
    }

    
    if (!(instruction->alu.is_nop)) {
        prge_instruction_tbl_data.op_1_field_select     = instruction->alu.alu_ifc.op1;
        prge_instruction_tbl_data.op_2_field_select     = instruction->alu.alu_ifc.op2;
        prge_instruction_tbl_data.op_3_field_select     = instruction->alu.alu_ifc.op3;
        prge_instruction_tbl_data.alu_action            = instruction->alu.alu_ifc.alu_action;
        prge_instruction_tbl_data.cmp_action            = instruction->alu.alu_ifc.cmp_action;
        prge_instruction_tbl_data.dst_select            = instruction->alu.alu_ifc.dst_select;
        
        if (instruction->alu.alu_ifc.dst_select != ARAD_PP_PRGE_ENG_OP_SIZE_ADDR) {
            
            prge_instruction_tbl_data.op_value              = instruction->alu.alu_ifc.op_value;
        }
        else {
            
            if (instruction->alu.alu_ifc.op3 == ARAD_PP_PRGE_ENG_OP_VALUE) {
                
                prge_instruction_tbl_data.op_value = ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ADDR((instruction->alu.alu_ifc.op_value), (entry & 0x1));
            }
            else {
                
                prge_instruction_tbl_data.op_value = (entry & 0x1) ? 1 : 0;
            }
        }
    }
    else {
        _ARAD_EGR_PRGE_INTERFACE_ALU_NOP_INSTR_LOAD(unit, prge_instruction_tbl_data);
    }

    
    res = arad_pp_epni_prge_instruction_tbl_set_unsafe(unit, mem, entry, &prge_instruction_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_egr_pge_mgmt_null_instruction_load()",mem,entry);
}


uint32
  _arad_egr_prge_mgmt_program_load(
    int     unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program
  ){
    uint32 res;
    int mem;
    int entry, entry_even, entry_odd;
    int next_instr_entry = 0;
    uint32 program_pointer;

    int instr;
    uint32 instr_nof = mgmt_programs[unit][program].instructions_nof;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    mem = EPNI_PRGE_INSTRUCTION_0m - 1;
    res = sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.programs.program_pointer.get(unit, program, &program_pointer);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    entry_odd = _ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ENTRY_ODD(program_pointer);
    entry_even = _ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ENTRY_EVEN(program_pointer);
    entry = entry_odd;

    for (instr = 0; instr < instr_nof; instr++ ) {
        if (mgmt_programs[unit][program].graph[instr].ce.is_nop
            && mgmt_programs[unit][program].graph[instr].alu.is_nop) {
            
            continue;
        }
        
        while (next_instr_entry < mgmt_programs[unit][program].graph[instr].selected_entry) {
            mem   = (entry == entry_odd) ? (mem + 1) : mem;
            entry = (entry == entry_even) ? entry_odd : entry_even;
            res = _arad_egr_pge_mgmt_null_instruction_load(unit, mem, entry);
            SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
            next_instr_entry++;
        }
        
        mem   = (entry == entry_odd) ? (mem + 1) : mem;
        entry = (entry == entry_even) ? entry_odd : entry_even;
        res = _arad_egr_prge_mgmt_instruction_load(unit, &program, 1 ,
                                                   &mgmt_programs[unit][program].graph[instr], mem, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
        next_instr_entry++;
    }

    mem   = (entry == entry_odd) ? (mem + 1) : mem;
    entry = (entry == entry_even) ? entry_odd : entry_even;
    SOC_SAND_IF_ERR_EXIT( arad_egr_prog_editor_cat_nops(unit, mem, entry, entry_even, entry_odd) );

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_program_load()",0,0);
}


uint32
  arad_egr_prge_mgmt_program_deploy(
    int     unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
    lfem_maps_shadow_t          lfem_shadow,
    uint8 do_load
  ){
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = _arad_egr_prge_mgmt_program_init(unit, program);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

    
    res = _arad_egr_prge_mgmt_program_anchors_update(unit, program, lfem_shadow);
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

    
    res = _arad_egr_prge_mgmt_program_entries_by_lfems_adjust(unit, program, do_load);
    if (do_load) {
        
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

        res = _arad_egr_prge_mgmt_program_load(unit, program);
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
    }
    else {
        
        return res;
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_program_deploy()",0,0);
}


uint32
  arad_egr_prge_mgmt_pre_processing_program_scan(
    int     unit,
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program,
    ARAD_EGR_PROG_EDITOR_LFEM       lfem_buffer[],
    uint8                           *num_lfems
  ){

    uint32 instr,i;
    ARAD_EGR_PROG_EDITOR_LFEM lfem;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(lfem_buffer);
    SOC_SAND_CHECK_NULL_INPUT(num_lfems);

    if (!_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(unit,program)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE management error - program isn't managed")));
    }

    *num_lfems = 0;

    for (instr = 0; instr < mgmt_programs[unit][program].instructions_nof; instr++ ) {
        if (_ARAD_EGR_PRGE_MGMT_INSTR_USE_LFEM(unit, program, instr)) {
            
            lfem = mgmt_programs[unit][program].graph[instr].ce.ce_ifc.lfem;
            for (i = 0; (i < *num_lfems) && (lfem_buffer[i] != lfem); i++)
                ;
            if (i == *num_lfems) { 
                if (*num_lfems >= ARAD_EGR_PRGE_MGMT_MAX_NOF_LFEMS_PER_PROGRAM) {
                    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE management error - program has too many different lfems to manage")));
                }
                lfem_buffer[*num_lfems] = lfem;
                (*num_lfems)++;
            }
        }
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_pre_processing_program_scan()",0,0);
}









int
    arad_egr_prge_mgmt_sort_program_according_to_permutation_number(
       SOC_SAND_IN    int                             unit,
       single_program_permutations_t                    *all_program_permutations)
{
    uint8 program_ndx, program_ndx_2, program_ndx_min, permutation_num_min;
    uint8 prog_id;
    lfem_maps_shadow_t *permutations_temp;

    SOCDNX_INIT_FUNC_DEFS;

    for (program_ndx = 0; program_ndx < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; program_ndx++) {
        program_ndx_min = program_ndx;
        permutation_num_min = all_program_permutations[program_ndx].valid_permutation_number;

        
        for (program_ndx_2 = program_ndx+1; program_ndx_2 < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; program_ndx_2++) {
            if (all_program_permutations[program_ndx_2].valid_permutation_number < permutation_num_min) {
                program_ndx_min = program_ndx_2;
                permutation_num_min = all_program_permutations[program_ndx_2].valid_permutation_number;
            }
        }

        if (program_ndx_min != program_ndx) {
            
            permutations_temp = all_program_permutations[program_ndx_min].program_permutations;
            prog_id = all_program_permutations[program_ndx_min].prog_index;
            all_program_permutations[program_ndx_min].program_permutations = all_program_permutations[program_ndx].program_permutations;
            all_program_permutations[program_ndx_min].valid_permutation_number = all_program_permutations[program_ndx].valid_permutation_number;
            all_program_permutations[program_ndx_min].prog_index = all_program_permutations[program_ndx].prog_index;
            all_program_permutations[program_ndx].program_permutations = permutations_temp;
            all_program_permutations[program_ndx].valid_permutation_number = permutation_num_min;
            all_program_permutations[program_ndx].prog_index = prog_id; 
        }
    }

    SOC_EXIT;

exit:
    SOCDNX_FUNC_RETURN;
}


int
    arad_egr_prge_mgmt_lfem_shadow_init(
       SOC_SAND_IN    int                             unit,
       lfem_maps_shadow_t lfem_maps
    ) {

    uint8 lfem_ndx, lfem_table_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    
    
    for (lfem_ndx = 0; lfem_ndx < ARAD_EGR_PRGE_MAX_NOF_LFEMS; lfem_ndx++) {
        for (lfem_table_ndx = 0; lfem_table_ndx < ARAD_EGR_PRGE_MAX_NOF_LFEM_TABLES; lfem_table_ndx++) {
            lfem_maps[lfem_table_ndx][lfem_ndx] = ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS;
        }
    }

    SOC_EXIT;

exit:
    SOCDNX_FUNC_RETURN;
}


int
    arad_egr_prge_mgmt_copy_shadow_internal(
       SOC_SAND_IN    int                             unit,
       lfem_maps_shadow_t lfem_maps_src,
       lfem_maps_shadow_t lfem_maps_dst,
       uint8                print_error)
{
    uint32 rv = SOC_SAND_OK;
    uint8 lfem_ndx_src, lfem_ndx_dst, lfem_table_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    lfem_ndx_dst = 0;
    
    
    for (lfem_ndx_src = 0; lfem_ndx_src < ARAD_EGR_PRGE_MAX_NOF_LFEMS; lfem_ndx_src++) {
        for (lfem_table_ndx = 0; lfem_table_ndx < ARAD_EGR_PRGE_MAX_NOF_LFEM_TABLES; lfem_table_ndx++) {
            if (lfem_maps_src[lfem_table_ndx][lfem_ndx_src] != ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS) {
                
                
                lfem_ndx_dst = 0;
                while (lfem_ndx_dst < ARAD_PP_EG_PROG_NOF_FEM(unit)) {
                    if (lfem_maps_dst[lfem_table_ndx][lfem_ndx_dst] == ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS) {
                        
                        lfem_maps_dst[lfem_table_ndx][lfem_ndx_dst] = lfem_maps_src[lfem_table_ndx][lfem_ndx_src];
                        break;
                    }
                    else if (lfem_maps_dst[lfem_table_ndx][lfem_ndx_dst] == lfem_maps_src[lfem_table_ndx][lfem_ndx_src]) {
                        
                        break;
                    }
                    lfem_ndx_dst++;
                }

                
                if (lfem_ndx_dst == ARAD_PP_EG_PROG_NOF_FEM(unit)) {
                    if (print_error) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Error - Not enough place in lfem_maps_dst for all the LFEMs")));
                    }
                    else {
                        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                    }
                }
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
    arad_egr_prge_mgmt_copy_shadow(
       SOC_SAND_IN    int                             unit,
       lfem_maps_shadow_t lfem_maps_src,
       lfem_maps_shadow_t lfem_maps_dst)
{
    uint32 rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_egr_prge_mgmt_copy_shadow_internal(unit, lfem_maps_src, lfem_maps_dst, TRUE );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


int
    arad_egr_prge_mgmt_add_permutation_according_to_lfem_shadow(
       SOC_SAND_IN    int                             unit,
       lfem_maps_shadow_t lfem_maps_shadow,
       single_program_permutations_t *permutation_struct)
{
    uint32 rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = arad_egr_prge_mgmt_copy_shadow(unit, lfem_maps_shadow, permutation_struct->program_permutations[permutation_struct->valid_permutation_number]);
    SOCDNX_IF_ERR_EXIT(rv);

    permutation_struct->valid_permutation_number++;

exit:
    SOCDNX_FUNC_RETURN;
}

int
    arad_egr_prge_mgmt_lfem_add_to_shadow(
       int                              unit,
       ARAD_EGR_PROG_EDITOR_LFEM        lfem,
       lfem_maps_shadow_t               lfem_shadow,
       uint8                            lfem_buffer_ndx
    ){

    uint32  i;

    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < ARAD_PP_EG_PROG_NOF_FEM(unit); i++) {
        if (lfem_shadow[lfem_buffer_ndx][i] == lfem) {
            return SOC_SAND_OK;
        }
        else if (lfem_shadow[lfem_buffer_ndx][i] == ARAD_EGR_PROG_EDITOR_LFEM_NOF_LFEMS) {
            lfem_shadow[lfem_buffer_ndx][i] = lfem;
            return SOC_SAND_OK;
        }
    }

    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Error - No room in LFEM buffer")));

    return SOC_SAND_ERR;

exit:
    SOCDNX_FUNC_RETURN;
}


int
    arad_egr_prge_mgmt_pre_processing_program_lfem_permutation_struct_build(int unit,
                                                                              int program_ndx,
                                                                              ARAD_EGR_PROG_EDITOR_LFEM lfems_usage_per_program[],
                                                                              uint8 num_lfems_usage_per_program,
                                                                              uint8 is_duplicate,
                                                                              single_program_permutations_t *permutation_struct)
{
    lfem_maps_shadow_t lfem_maps_shadow_dummy;
    uint8 one_permutation_succcess = 0;
    uint8 lfem_ndx, lfem_ndx_to_duplicate, lfem_table_ndx, lfem_id;
    uint16 permutation;
    uint8 nof_permutations;

    uint32 rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    nof_permutations = SOC_IS_JERICHO(unit) ? ((1 << num_lfems_usage_per_program*2) -1)  :
        ((1 << num_lfems_usage_per_program) -1) ;

    
    
    for (permutation = 0; permutation <= nof_permutations; permutation++) {

        
        rv = arad_egr_prge_mgmt_lfem_shadow_init(unit, lfem_maps_shadow_dummy);
        SOCDNX_IF_ERR_EXIT(rv);

        
        for (lfem_ndx_to_duplicate = 0; lfem_ndx_to_duplicate <= (is_duplicate?4:0); lfem_ndx_to_duplicate++) {

            
            for (lfem_ndx = 0; lfem_ndx < num_lfems_usage_per_program; lfem_ndx++)  {
                if (SOC_IS_JERICHO(unit)) {
                    lfem_table_ndx = (permutation >> (lfem_ndx*2)) & 3; 
                }
                else {
                    lfem_table_ndx = (permutation >> lfem_ndx) & 1; 
                }
                lfem_id = lfems_usage_per_program[lfem_ndx];

                rv = arad_egr_prge_mgmt_lfem_add_to_shadow(unit, lfem_id, lfem_maps_shadow_dummy, lfem_table_ndx);
                SOCDNX_IF_ERR_EXIT(rv);
                if (is_duplicate && (lfem_ndx_to_duplicate==lfem_ndx)) {
                    if (SOC_IS_JERICHO(unit)) {
                        
                        lfem_table_ndx = (lfem_table_ndx==3) ? 0 : 1 + lfem_table_ndx;
                    }
                    else {
                        
                        lfem_table_ndx = 1 - lfem_table_ndx;

                    }
                    rv = arad_egr_prge_mgmt_lfem_add_to_shadow(unit, lfem_id, lfem_maps_shadow_dummy, lfem_table_ndx);
                    SOCDNX_IF_ERR_EXIT(rv); 
                }
            }

            
            rv = arad_egr_prge_mgmt_program_deploy(unit, program_ndx, lfem_maps_shadow_dummy, FALSE);

            if (rv == SOC_SAND_OK) {
                one_permutation_succcess = 1;
                
                rv = arad_egr_prge_mgmt_add_permutation_according_to_lfem_shadow(unit, lfem_maps_shadow_dummy, permutation_struct);
                SOCDNX_IF_ERR_EXIT(rv);

                if (permutation_struct->valid_permutation_number == ARAD_EGR_PRGE_MGMT_MAX_NOF_PERMUTATIONS_PER_PROGRAM) {
                    
                    return SOC_SAND_OK;
                }
            }
            
        }
    }

    if (one_permutation_succcess) {
        return SOC_SAND_OK;
    }
    else {
        return SOC_SAND_ERR;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_egr_prge_mgmt_pre_processing(
     SOC_SAND_IN    int                             unit,
     single_program_permutations_t                    *all_program_permutations
  )
{
    
    ARAD_EGR_PROG_EDITOR_LFEM lfems_usage_per_program[ARAD_EGR_PRGE_MGMT_MAX_NOF_LFEMS_PER_PROGRAM];
    uint8 program_ndx, num_lfems_usage_per_program;

    char* prog_name=NULL;

    uint32 rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    _ARAD_EGR_PRGE_MGMT_LOG(unit,"Pre processing - start\n");

    

    
    for (program_ndx = 0; program_ndx < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; program_ndx++) {
        
        if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(unit, program_ndx)) { 
            _ARAD_EGR_PRGE_MGMT_LOG_PARAM(unit,"Pre processing - scanning program: %d\n", program_ndx);

            
            rv = arad_egr_prge_mgmt_pre_processing_program_scan(unit, program_ndx, lfems_usage_per_program, &num_lfems_usage_per_program);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = arad_egr_prge_mgmt_pre_processing_program_lfem_permutation_struct_build(unit, program_ndx, lfems_usage_per_program,
                                                                                         num_lfems_usage_per_program, 0, &all_program_permutations[program_ndx]);

            
            if (rv != SOC_SAND_OK) {
                
                rv = arad_egr_prge_mgmt_pre_processing_program_lfem_permutation_struct_build(unit, program_ndx, lfems_usage_per_program,
                                                                                             num_lfems_usage_per_program, 1, &all_program_permutations[program_ndx]);
                if (rv != SOC_SAND_OK) {
                    arad_egr_prog_editor_prog_name_get_by_id(program_ndx, &prog_name);
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Error - PRGE Pre-Processing failed to find LFEMs for program %s.\n\r"), prog_name));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_egr_prge_mgmt_programs_allocate_recursively(
     SOC_SAND_IN    int                             unit,
     lfem_maps_shadow_t                             lfem_maps_shadow,
     single_program_permutations_t                  *all_program_permutations,
     uint8                                          current_program
  )
{
    lfem_maps_shadow_t lfem_maps_shadow_dummy;
    uint8 permutation;
    char* prog_name;

    uint32 rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    
    if ((current_program >= ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) || (all_program_permutations[current_program].valid_permutation_number == 0)) {
        return SOC_SAND_OK;
    }

    arad_egr_prog_editor_prog_name_get_by_id(all_program_permutations[current_program].prog_index, &prog_name);
    _ARAD_EGR_PRGE_MGMT_LOG_PARAM(unit,"Allocating program: %s\n", prog_name);

    
    rv = arad_egr_prge_mgmt_lfem_shadow_init(unit, lfem_maps_shadow_dummy);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egr_prge_mgmt_copy_shadow(unit, lfem_maps_shadow, lfem_maps_shadow_dummy);
    SOCDNX_IF_ERR_EXIT(rv);

    
    for (permutation = 0; permutation < all_program_permutations[current_program].valid_permutation_number; permutation++) {
        
        rv = arad_egr_prge_mgmt_copy_shadow_internal(unit, all_program_permutations[current_program].program_permutations[permutation], lfem_maps_shadow_dummy,
                                                     FALSE );

        
        if (rv != SOC_SAND_NO_ERR) {
            
            rv = arad_egr_prge_mgmt_lfem_shadow_init(unit, lfem_maps_shadow_dummy);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = arad_egr_prge_mgmt_copy_shadow(unit, lfem_maps_shadow, lfem_maps_shadow_dummy);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else {
            
            rv = arad_egr_prge_mgmt_programs_allocate_recursively(unit, lfem_maps_shadow_dummy, all_program_permutations, current_program+1);

            if (rv != SOC_SAND_NO_ERR) {
                
                rv = arad_egr_prge_mgmt_lfem_shadow_init(unit, lfem_maps_shadow_dummy);
                SOCDNX_IF_ERR_EXIT(rv);

                rv = arad_egr_prge_mgmt_copy_shadow(unit, lfem_maps_shadow, lfem_maps_shadow_dummy);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            else {
                

                
                rv = arad_egr_prge_mgmt_copy_shadow(unit, lfem_maps_shadow_dummy, lfem_maps_shadow);
                SOCDNX_IF_ERR_EXIT(rv);

                return SOC_SAND_NO_ERR;
            }
        }
    }

    
    return SOC_SAND_ERR;

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_egr_prge_mgmt_program_allocation(
     SOC_SAND_IN    int                             unit,
     single_program_permutations_t                  *all_program_permutations,
     lfem_maps_shadow_t                             lfem_maps_shadow
  )
{
    uint8 current_program;

    uint32 rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = arad_egr_prge_mgmt_sort_program_according_to_permutation_number(unit, all_program_permutations);
    SOCDNX_IF_ERR_EXIT(rv);

    
    for (current_program = 0; (current_program < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) &&
                                (all_program_permutations[current_program].valid_permutation_number<1) ;
          current_program++ ) {
        
    }

   if (current_program < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS) {
        
        rv = arad_egr_prge_mgmt_programs_allocate_recursively(unit, lfem_maps_shadow, all_program_permutations, current_program);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  _arad_egr_prge_mgmt_branch_usage_min_max_instr_find(
     int        unit,
     ARAD_EGR_PROG_EDITOR_BRANCH branch,
     int        *min_jump_instr,
     int        *max_jump_instr
  ){

    ARAD_EGR_PROG_EDITOR_JUMP_POINT
        usage;

    SOCDNX_INIT_FUNC_DEFS;

    *min_jump_instr = ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS;
    *max_jump_instr = -1;

    for (usage = 0; usage < ARAD_EGR_PROG_EDITOR_JUMP_POINT_IDS_NOF; usage++ ) {
        if (mgmt_branches[unit][branch].branch_uses[usage]) {
            *min_jump_instr = MIN(*min_jump_instr, mgmt_branch_usage[unit][usage][0]);
            *max_jump_instr = MAX(*max_jump_instr, mgmt_branch_usage[unit][usage][1]);
        }
    }

    if ((*min_jump_instr == ARAD_EGR_PRGE_MAX_NOF_PROG_INSTRUCTIONS)
        || (*max_jump_instr == -1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Error - No branch usage mentioned that define this branch's instructions")));
    }

    
    *min_jump_instr += 4;
    *max_jump_instr += 4;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  _arad_egr_prge_mgmt_programs_jumping_to_branch_find(
     int unit, ARAD_EGR_PROG_EDITOR_BRANCH branch,
     ARAD_EGR_PROG_EDITOR_PROGRAMS jumped_from_programs[],
     uint8 *num_results) {

    ARAD_EGR_PROG_EDITOR_PROGRAMS
        program;

    uint32
        i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(jumped_from_programs);
    SOCDNX_NULL_CHECK(num_results);

    *num_results = 0;

    for (program = 0; program < ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; program++) {
        if (!_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_OLD_INTERFACE(unit, program)) {
            
            _ARAD_EGR_PRGE_MGMT_PROGRAM *p_program = &mgmt_programs[unit][program];
            for (i = 0; i < p_program->nof_jump_points; i++) {
                if (mgmt_branches[unit][branch].branch_uses[p_program->jump_points[i]]) {
                    
                    jumped_from_programs[(*num_results)++] = program;
                }
            }
        }
    }

    if (*num_results == 0) {
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("Error - No programs jump to the loaded branch")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  _arad_egr_prge_mgmt_branch_load(int unit, ARAD_EGR_PROG_EDITOR_BRANCH branch) {

    uint32 res;

    int
        min_jump_instr, max_jump_instr;
    int
        instr;
    int
        mem, entry_odd, entry_even, entry;

    
    ARAD_EGR_PROG_EDITOR_PROGRAMS jumped_from_programs[ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS];
    uint8 nof_program_jumps; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = _arad_egr_prge_mgmt_branch_usage_min_max_instr_find(unit, branch, &min_jump_instr, &max_jump_instr);
    if (res != SOC_E_NONE) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Could not load branch.\n\r")));
    }
    
    if (max_jump_instr + mgmt_branches[unit][branch].instructions_nof > ARAD_PP_EG_PROG_NOF_INSTR(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - After jump, not enough instructions left for the branch.\n\r")));
    }
    res = _arad_egr_prge_mgmt_programs_jumping_to_branch_find(unit, branch, jumped_from_programs, &nof_program_jumps);
    if ((res != SOC_E_NONE) || (nof_program_jumps == 0)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Loaded branch jump points aren't used by any program.\n\r")));
    }

    entry_odd = _ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ENTRY_ODD(branch_entries[unit][branch]);
    entry_even = _ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ENTRY_EVEN(branch_entries[unit][branch]);

    mem = EPNI_PRGE_INSTRUCTION_0m + (min_jump_instr >> 1) - ((min_jump_instr & 0x1) ? 0 : 1);
    entry = (min_jump_instr & 0x1) ? entry_even : entry_odd ;

    
    for ( ; min_jump_instr < max_jump_instr; min_jump_instr++ ) {
        mem   = (entry == entry_odd) ? (mem + 1) : mem;
        entry = (entry == entry_even) ? entry_odd : entry_even;
        res = _arad_egr_pge_mgmt_null_instruction_load(unit, mem, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
    }

    
    for (instr = 0; instr < mgmt_branches[unit][branch].instructions_nof; instr++ ) {
        mem   = (entry == entry_odd) ? (mem + 1) : mem;
        entry = (entry == entry_even) ? entry_odd : entry_even;
        res = _arad_egr_prge_mgmt_instruction_load(unit, jumped_from_programs, nof_program_jumps,
                                                   &mgmt_branches[unit][branch].graph[instr], mem, entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
    }

    if (!mgmt_branches[unit][branch].jump_out) {
        mem   = (entry == entry_odd) ? (mem + 1) : mem;
        entry = (entry == entry_even) ? entry_odd : entry_even;
        SOC_SAND_IF_ERR_EXIT( arad_egr_prog_editor_cat_nops(unit, mem, entry, entry_even, entry_odd) );
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_mgmt_program_deploy()",0,0);
}


uint32
  arad_egr_prge_mgmt_branches_load(
     int     unit
  ){
    uint32 rv = SOC_SAND_OK;
    ARAD_EGR_PROG_EDITOR_BRANCH branch;

    SOCDNX_INIT_FUNC_DEFS;

    for (branch = 0; branch < ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF; branch++) {
        if ((mgmt_branches[unit][branch].management_type > ARAD_EGR_PRGE_MGMT_MANAGEMENT_TYPE_NOT_MANAGED)
            && (branch_entries[unit][branch] > -1)) {
            
            rv = _arad_egr_prge_mgmt_branch_load(unit, branch);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_egr_prge_mgmt_program_management_main(
     int    unit
  )
{
    single_program_permutations_t        all_program_permutations[ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS];

    uint8 prog_ndx, permutation;
    lfem_maps_shadow_t                             allocated_lfem_maps_shadow;
    uint32 rv = SOC_SAND_OK;
    char *prog_name;

    uint8 management_enabled;

    SOCDNX_INIT_FUNC_DEFS;

    _ARAD_EGR_PRGE_MGMT_LOG(unit,"Starting PRGE Management\n");

    management_enabled = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "prge_management_enable", 1);
    if (!management_enabled) {
        _ARAD_EGR_PRGE_MGMT_LOG(unit,"Programs manipulation is disabled!\n");
    }

    
    for (prog_ndx = 0; prog_ndx<ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; prog_ndx++) {
        if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(unit, prog_ndx) && management_enabled) {
            all_program_permutations[prog_ndx].prog_index = prog_ndx;
            all_program_permutations[prog_ndx].program_permutations = NULL;
            SOCDNX_ALLOC(all_program_permutations[prog_ndx].program_permutations, lfem_maps_shadow_t, ARAD_EGR_PRGE_MGMT_MAX_NOF_PERMUTATIONS_PER_PROGRAM, "program_permutations");

            for (permutation=0; permutation < ARAD_EGR_PRGE_MGMT_MAX_NOF_PERMUTATIONS_PER_PROGRAM; permutation++) {
                rv = arad_egr_prge_mgmt_lfem_shadow_init(unit, all_program_permutations[prog_ndx].program_permutations[permutation]);
                SOCDNX_IF_ERR_EXIT(rv);
            }

            
            all_program_permutations[prog_ndx].valid_permutation_number = 0;
            arad_egr_prog_editor_prog_name_get_by_id(prog_ndx, &prog_name);
            _ARAD_EGR_PRGE_MGMT_LOG_PARAM(unit,"Allocating (with management) program: %s\n", prog_name);

        }
        else {
            all_program_permutations[prog_ndx].prog_index = ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS;
            all_program_permutations[prog_ndx].valid_permutation_number = 0;
            all_program_permutations[prog_ndx].program_permutations = NULL;
            if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_LOADED_ONLY(unit, prog_ndx)
                || ((!management_enabled) && (!_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_OLD_INTERFACE(unit, prog_ndx)))) {
                
                rv = _arad_egr_prge_mgmt_program_load(unit, prog_ndx);
                SOCDNX_IF_ERR_EXIT(rv);

                arad_egr_prog_editor_prog_name_get_by_id(prog_ndx, &prog_name);
                _ARAD_EGR_PRGE_MGMT_LOG_PARAM(unit,"Loading (without management) program: %s\n", prog_name);

            }
        }
    }

    _ARAD_EGR_PRGE_MGMT_LOG(unit,"Allocate buffer for program permutation success\n");

    
    
    rv = arad_egr_prge_mgmt_branches_load(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    _ARAD_EGR_PRGE_MGMT_LOG(unit,"Branches loading success\n");

    if (management_enabled) {
        
        rv = arad_egr_prge_mgmt_pre_processing(unit, all_program_permutations);
        SOCDNX_IF_ERR_EXIT(rv);

        _ARAD_EGR_PRGE_MGMT_LOG(unit,"Pre processing success\n");

        
        rv = arad_egr_prge_mgmt_lfem_shadow_init(unit, allocated_lfem_maps_shadow);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prge_mgmt_copy_shadow(unit, lfem_maps_shadow[unit], allocated_lfem_maps_shadow);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = arad_egr_prge_mgmt_program_allocation(unit, all_program_permutations, allocated_lfem_maps_shadow);
        SOCDNX_IF_ERR_EXIT(rv);

        _ARAD_EGR_PRGE_MGMT_LOG(unit,"Program allocation success\n");

        rv = arad_egr_prge_mgmt_copy_shadow(unit, allocated_lfem_maps_shadow, lfem_maps_shadow[unit]);
        SOCDNX_IF_ERR_EXIT(rv);

        _ARAD_EGR_PRGE_MGMT_LOG(unit,"Copy to LFEMs shadow buffer success\n");

        for (prog_ndx = 0; prog_ndx<ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; prog_ndx++) {
            if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(unit, prog_ndx)) {
                rv = arad_egr_prge_mgmt_program_deploy(unit, prog_ndx, lfem_maps_shadow[unit], TRUE );
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }

        
        for (prog_ndx = 0; prog_ndx<ARAD_EGR_PROG_EDITOR_PROG_NOF_PROGS; prog_ndx++) {
            if (all_program_permutations[prog_ndx].program_permutations) {
                SOCDNX_FREE(all_program_permutations[prog_ndx].program_permutations);
            }
        }
    }

    _ARAD_EGR_PRGE_MGMT_LOG(unit,"Managed programs loaded to HW successfully\n");

exit:
    SOCDNX_FUNC_RETURN;
}



void print_instr(_ARAD_EGR_PRGE_MGMT_INSTRUCTION* instr){
    int i;

    bsl_printf("\tselected_entry: %d\n",instr->selected_entry);

    if (!instr->ce.is_nop) {
        bsl_printf("\tCE fields\n");
        bsl_printf("\t\tRegister dependencies: "); for (i = 0; i <= instr->ce.last_reg_dep; i++) {bsl_printf(" %d",instr->ce.reg_dep_instr[i]);} bsl_printf("\n");
        bsl_printf("\t\tSize dependency: %d\n",instr->ce.size_dep);
        bsl_printf("\t\tImmediate register dependency: %d\n",instr->ce.reg_immediate_dep);
        bsl_printf("\t\tThis program copies byte to the header: %d\n",instr->ce.size_change);
    }
    else {
        bsl_printf("\t-CE NOP-\n");
    }

    if (!instr->alu.is_nop) {
        bsl_printf("\tALU fields\n");
        bsl_printf("\t\tRegister dependencies: "); for (i = 0; i <= instr->alu.last_reg_dep; i++) {bsl_printf(" %d",instr->alu.reg_dep_instr[i]);} bsl_printf("\n");
        bsl_printf("\t\tLFEM dependency: %d\n",instr->alu.lfem_dep);
        bsl_printf("\t\tImmediate Register dependency: %d\n",instr->alu.reg_immediate_dep);
        bsl_printf("\t\tRegister change bitmap: 0x%04x\n",instr->alu.dep_flags);
        bsl_printf("\t\tRegister conditional change bitmap: 0x%04x\n",instr->alu.cond_dep_flags);
    }
    else {
        bsl_printf("\t-ALU NOP-\n");
    }

}


void arad_egr_prge_mgmt_branch_print(int unit, uint32 branch){
    char *branch_name;
    int instr, instr_start, nop_start;
    uint32 res;

    arad_egr_prog_editor_branch_name_get_by_id(branch, &branch_name);

    res = _arad_egr_prge_mgmt_branch_usage_min_max_instr_find(unit, branch, &nop_start, &instr_start);
    if (res != SOC_E_NONE) {
        bsl_printf("PRGE error - Could find where branch %s is loaded.\n", branch_name);
        return;
    }

    bsl_printf("\nBranch %s:\n"
               "--------------------------------------------------\n", branch_name);

    for ( ; nop_start < instr_start; nop_start++) {
        bsl_printf("Entry %02d - NOP (Added dynamically by management)\n", nop_start);
    }

    for (instr = 0; instr < mgmt_branches[unit][branch].instructions_nof; instr++) {

        bsl_printf("Entry %02d - Instruction %02d: %s\n", instr + instr_start, instr,
                   mgmt_branches[unit][branch].graph[instr].doc);

    }

}


void arad_egr_prge_mgmt_program_print(int unit, uint32 program, char* prog_name, uint8 print_full_graph){
    int instr, entry, branch;
    int i;
    uint32 instr_nof = mgmt_programs[unit][program].instructions_nof;

    bsl_printf("\nProgram %s:\n"
             "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", prog_name);
    if (print_full_graph) {
        for (instr = 0; instr < instr_nof; instr++) {
            bsl_printf("Instruction %d\n", instr);
            print_instr(&mgmt_programs[unit][program].graph[instr]);
        }
    }
    else {
        for (entry = 0, instr = 0; instr < instr_nof; instr++, entry++) {
            for ( ; entry < mgmt_programs[unit][program].graph[instr].selected_entry; entry++) {
                bsl_printf("Entry %02d - NOP (Added dynamically by management)\n", entry);
            }
            bsl_printf("Entry %02d - Instruction %02d: %s\n", entry, instr,
                       mgmt_programs[unit][program].graph[instr].doc);
        }

        for (branch = 0; branch < ARAD_EGR_PROG_EDITOR_BRANCH_IDS_NOF; branch++ ) {
            for (i = 0; i < mgmt_programs[unit][program].nof_jump_points; i++) {
                if (mgmt_branches[unit][branch].branch_uses[ mgmt_programs[unit][program].jump_points[i] ]) {
                    
                    arad_egr_prge_mgmt_branch_print(unit, branch);
                    break; 
                }
            }
        }
    }
}


int
  arad_egr_prge_mgmt_diag_print(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_PRGE_MGMT_DIAG_PARAMS *key_params
   ){

    uint32 res;

    char *prog_name = key_params->deploy?key_params->deploy:key_params->graph;
    ARAD_EGR_PROG_EDITOR_PROGRAMS prog;

    res = arad_egr_prog_editor_program_by_name_find(unit, prog_name, &prog);
    if (res != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_EGRESS,
            (BSL_META_U(unit,"Error - Program %s not found"), prog_name));
        return(-1);
    }

    if (key_params->deploy == NULL) {
        
        if (!_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_MANAGED(unit, prog)) {
            LOG_ERROR(BSL_LS_SOC_EGRESS,
                      (BSL_META_U(unit, "Error - No graph for program %s"), prog_name));
            return (-1);
        }
    }
    else if (_ARAD_EGR_PRGE_MGMT_IS_PROGRAM_OLD_INTERFACE(unit, prog)) {
        
        LOG_ERROR(BSL_LS_SOC_EGRESS,
                  (BSL_META_U(unit, "Error - program %s uses the old interface"), prog_name));
        return (-1);
    }


    arad_egr_prge_mgmt_program_print(unit, prog, prog_name, (key_params->deploy == NULL));

    return(0);

}

#if ARAD_DEBUG_IS_LVL1

#endif 









#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

#undef _ERR_MSG_MODULE_NAME
