/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_EGR_PRGE_INTERFACE_INCLUDED__

#define __ARAD_EGR_PRGE_INTERFACE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/TMC/tmc_api_stack.h>

#include <soc/dpp/ARAD/arad_egr_prog_editor.h>








#define ARAD_EGR_PROG_EDITOR_DATA_SRC_PKT           (0x0)
#define ARAD_EGR_PROG_EDITOR_DATA_SRC_GEN           (0x1)
#define ARAD_EGR_PROG_EDITOR_DATA_SRC_RES           (0x2)


#define ARAD_EGR_PROG_EDITOR_OFFSET_CODE_REG        (0x0)
#define ARAD_EGR_PROG_EDITOR_OFFSET_CODE_CONST      (0x1)


#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_0       (0x0)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_1       (0x1)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_2       (0x2)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_3       (0x3)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_SZ      (0x4)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0     (0x5)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128   (0x6)
#define ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192   (0x7)


#define ARAD_EGR_PROG_EDITOR_SIZE_CODE_REG          (0x0)
#define ARAD_EGR_PROG_EDITOR_SIZE_CODE_CONST        (0x1)


#define ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_0         (0x0)
#define ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_1         (0x1)
#define ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_2         (0x2)
#define ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_3         (0x3)
#define ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_SZ        (0x4)
#define ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0       (0x5)



#define ARAD_EGR_PROG_EDITOR_OP_TYPE_REG            (0x0)
#define ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST          (0x1)
#define ARAD_EGR_PROG_EDITOR_OP_TYPE_FEM            (0x2)
#define ARAD_EGR_PROG_EDITOR_OP_TYPE_DATA_ADDR      (0x3)
#define ARAD_EGR_PROG_EDITOR_OP_TYPE_ALU            (0x4)


#define ARAD_PP_PRGE_ENG_OP_REG_R0       (0)
#define ARAD_PP_PRGE_ENG_OP_REG_R1       (1)
#define ARAD_PP_PRGE_ENG_OP_REG_R2       (2)
#define ARAD_PP_PRGE_ENG_OP_REG_R3       (3)
#define ARAD_PP_PRGE_ENG_OP_REG_RSZ      (4)
#define ARAD_PP_PRGE_ENG_OP_SIZE_ADDR    (5)
#define ARAD_PP_PRGE_ENG_OP_FEM          (6)
#define ARAD_PP_PRGE_ENG_OP_VALUE        (7)
#define ARAD_PP_PRGE_ENG_OP_ALU          (8)


#define ARAD_EGR_PROG_EDITOR_ALU_CMP_NONE           (0x0)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_LT             (0x1)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_EQ             (0x2)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_GT             (0x4)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_GE             (ARAD_EGR_PROG_EDITOR_ALU_CMP_GT|ARAD_EGR_PROG_EDITOR_ALU_CMP_EQ)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_LE             (ARAD_EGR_PROG_EDITOR_ALU_CMP_LT|ARAD_EGR_PROG_EDITOR_ALU_CMP_EQ)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_NEQ            (ARAD_EGR_PROG_EDITOR_ALU_CMP_GT|ARAD_EGR_PROG_EDITOR_ALU_CMP_LT)
#define ARAD_EGR_PROG_EDITOR_ALU_CMP_ALL            (ARAD_EGR_PROG_EDITOR_ALU_CMP_GT|ARAD_EGR_PROG_EDITOR_ALU_CMP_EQ|ARAD_EGR_PROG_EDITOR_ALU_CMP_LT)


#define ARAD_PP_EG_PROG_ALU_SUB  (0x0)
#define ARAD_PP_EG_PROG_ALU_ADD  (0x1)






#define INSTR(_interface_ce_instr, _interface_alu_instr, _doc_str) \
do { \
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION instr; \
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION_clear(&instr); \
    SOC_SAND_CHECK_FUNC_RESULT((_interface_ce_instr), 370, exit); \
    SOC_SAND_CHECK_FUNC_RESULT((_interface_alu_instr), 370, exit); \
    instr.doc_str = _doc_str; \
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_mgmt_instr_add(unit, &instr), 370, exit); \
} while(0)








#define R_OFST(_reg_num, _base)         ARAD_EGR_PROG_EDITOR_OFFSET_CODE_REG, (ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_##_reg_num), (_base)

#define C_OFST(_offset)                 ARAD_EGR_PROG_EDITOR_OFFSET_CODE_CONST, 0, (_offset)

#define R_SIZE(_reg_num, _base)         ARAD_EGR_PROG_EDITOR_SIZE_CODE_REG, (ARAD_EGR_PROG_EDITOR_SIZE_SRC_REG_##_reg_num), (_base)

#define C_SIZE(_size)                   ARAD_EGR_PROG_EDITOR_SIZE_CODE_CONST, 0, (_size)






#define CE_READ(_src, _offset) \
                arad_egr_prge_interface_read_data_instr(unit, (ARAD_EGR_PROG_EDITOR_DATA_SRC_##_src), _offset, ARAD_EGR_PROG_EDITOR_LFEM_NULL, &instr)


#define CE_LREAD(_src, _offset, _lfem) \
                arad_egr_prge_interface_read_data_instr(unit, (ARAD_EGR_PROG_EDITOR_DATA_SRC_##_src), _offset, (ARAD_EGR_PROG_EDITOR_##_lfem), &instr)


#define CE_FREAD(_field_enum, _offset) \
                arad_egr_prge_interface_read_field_instr(unit, (_field_enum), (_offset), ARAD_EGR_PROG_EDITOR_LFEM_NULL, &instr)


#define CE_FLREAD(_field_enum, _offset, _lfem) \
                arad_egr_prge_interface_read_field_instr(unit, (_field_enum), (_offset), (ARAD_EGR_PROG_EDITOR_##_lfem), &instr)




#define CE_COPY(_src, _offset, _size) \
                arad_egr_prge_interface_copy_data_instr(unit, (ARAD_EGR_PROG_EDITOR_DATA_SRC_##_src), _offset, _size, ARAD_EGR_PROG_EDITOR_LFEM_NULL, &instr)


#define CE_LCOPY(_src, _offset, _size, _lfem) \
                arad_egr_prge_interface_copy_data_instr(unit, (ARAD_EGR_PROG_EDITOR_DATA_SRC_##_src), _offset, _size, (ARAD_EGR_PROG_EDITOR_##_lfem), &instr)


#define CE_FCOPY(_field_enum, _offset, _size) \
                arad_egr_prge_interface_copy_field_instr(unit, (_field_enum), (_offset), _size, ARAD_EGR_PROG_EDITOR_LFEM_NULL, &instr)


#define CE_FLCOPY(_field_enum, _offset, _size, _lfem) \
                arad_egr_prge_interface_copy_field_instr(unit, (_field_enum), (_offset), _size, (ARAD_EGR_PROG_EDITOR_##_lfem), &instr)


#define CE_NOP                      arad_egr_prge_interface_ce_nop(unit, &instr)







#define REG_OP(_reg_num)    ARAD_EGR_PROG_EDITOR_OP_TYPE_REG, (ARAD_PP_PRGE_ENG_OP_REG_R##_reg_num)

#define CNST(_val)          ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST, (_val)

#define FEM                 ARAD_EGR_PROG_EDITOR_OP_TYPE_FEM, 0

#define DATA_ADDR           ARAD_EGR_PROG_EDITOR_OP_TYPE_DATA_ADDR, (ARAD_PP_PRGE_ENG_DST_DATA_ADDR)
#define ALU                 ARAD_EGR_PROG_EDITOR_OP_TYPE_ALU, 0



#define ALU_ADD(_op1, _op2, _dst) \
                arad_egr_prge_interface_add_instr(unit, _op1, _op2, _dst, &instr)


#define ALU_SUB(_op1, _op2, _dst) \
                arad_egr_prge_interface_sub_instr(unit, _op1, _op2, _dst, &instr)


#define ALU_SET(_val, _dst) \
                arad_egr_prge_interface_set_instr(unit, (_val), _dst, &instr)


#define ALU_SET_IF(_op1, _cmp_type, _op2, _res, _dst) \
                arad_egr_prge_interface_set_if_instr(unit, _op1, (ARAD_EGR_PROG_EDITOR_ALU_CMP_##_cmp_type), _op2, _res, _dst, ARAD_PP_EG_PROG_ALU_SUB, &instr)


#define ALU_SET_COMP_SUM(_op1, _op2, _cmp_type, _res, _dst) \
                    arad_egr_prge_interface_set_if_instr(unit, _op1, (ARAD_EGR_PROG_EDITOR_ALU_CMP_##_cmp_type), _op2, _res, _dst, ARAD_PP_EG_PROG_ALU_ADD, &instr)


#define ALU_JMP_IF(_op1, _cmp_type, _op2, _branch, _jump_point) \
                arad_egr_prge_interface_jump_if_instr(unit, _op1, (ARAD_EGR_PROG_EDITOR_ALU_CMP_##_cmp_type), _op2, \
                                                (ARAD_EGR_PROG_EDITOR_##_branch), (ARAD_EGR_PROG_EDITOR_##_jump_point), &instr)


#define ALU_JMP(_addr_op, _jump_point) \
                arad_egr_prge_interface_jump_instr(unit, _addr_op, (ARAD_EGR_PROG_EDITOR_##_jump_point), &instr)


#define ALU_NOP                      arad_egr_prge_interface_alu_nop(unit, &instr)








typedef enum predefined_data_s {

    
    PrgePktFieldFheiSize_UseR2TM,
    PrgePktFieldFwdHdrOffset_UseR2TM,

    
    
    PrgeDataFieldOutLifProfile5,
    PrgeDataFieldOutLifProfile6,
    PrgeDataFieldOutLifProfile7,
    PrgeDataFieldNativeLLSize,
    PrgeDataFieldCud2,
    PrgeDataFieldLBKey,
    
    PrgeDataFieldOutLifProfile1,
    PrgeDataFieldOutLifProfile2,
    PrgeDataFieldOutLifProfile3,
    PrgeDataFieldOutLifProfile4,
    PrgeDataFieldOutLifMsb,
    
    PrgeDataFieldEesData3,
    
    PrgeDataFieldR0,
    PrgeDataFieldR1,
    PrgeDataFieldR2,
    PrgeDataFieldR3,
    PrgeDataFieldSrcSysPort,
    PrgeDataFieldDstSysPort,
    PrgeDataFieldNwkHdrSize,
    PrgeDataFieldSysHdrSize,
    PrgeDataFieldEesData1,
    PrgeDataFieldEesData2,
    PrgeDataFieldProgVar,
    PrgeDataFieldOutTmPort,
    PrgeDataFieldOutPpPort,
    PrgeDataFieldOamCounterValue,
    PrgeDataFieldOamExtId,
    PrgeDataFieldOamExtSubType,
    PrgeDataFieldTod1588,
    PrgeDataFieldTodNtp,
    PrgeDataFieldVsiVar,
    PrgeDataFieldAceVar,
    PrgeDataFieldOutLif,
    PrgeDataFieldTmPortVar,
    PrgeDataFieldPpPortVar,
    PrgeDataFieldOamId,
    PrgeDataFieldOamSubType,
    PrgeDataFieldDataMemoryEntry,

    
    PRGE_PREDEFINED_FIELDS_NOF
} predefined_data_e;










void
  ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION_clear(
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION * instr
  );


void
  arad_egr_prge_interface_field_available_by_device_init(
     int unit
  );


uint32
  arad_egr_prge_interface_read_data_instr(
     int     unit,
     uint32  data_src,
     uint32  offset_code,
     uint32  offset_src,
     int     offset_base,
     ARAD_EGR_PROG_EDITOR_LFEM lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_read_field_instr(
     int                                  unit,
     predefined_data_e                    field,
     int                                  offset,
     ARAD_EGR_PROG_EDITOR_LFEM            lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_copy_data_instr(
     int                                  unit,
     uint32                               data_src,
     uint32                               offset_code,
     uint32                               offset_src,
     int                                  offset_base,
     uint32                               size_code,
     uint32                               size_src,
     int                                  size_base,
     ARAD_EGR_PROG_EDITOR_LFEM            lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_copy_field_instr(
     int     unit,
     predefined_data_e                    field,
     int                                  offset,
     uint32                               size_code,
     uint32                               size_src,
     int                                  size_base,
     ARAD_EGR_PROG_EDITOR_LFEM            lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_ce_nop(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );



uint32
  arad_egr_prge_interface_add_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   op2_type,
    uint32   op2_val,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_sub_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   op2_type,
    uint32   op2_val,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_set_instr(
    int      unit,
    uint32   val,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_set_if_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   cmp_type,
    uint32   op2_type,
    uint32   op2_val,
    uint32   res_type,
    uint32   res_val,
    uint32   __ignored,
    uint32   dst_reg,
    uint32   alu_action,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_jump_if_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   cmp_type,
    uint32   op2_type,
    uint32   op2_val,
    ARAD_EGR_PROG_EDITOR_BRANCH branch,
    ARAD_EGR_PROG_EDITOR_JUMP_POINT branch_usage,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_jump_instr(
    int      unit,
    uint32   addr_op_type,
    uint32   addr_op_val,
    ARAD_EGR_PROG_EDITOR_JUMP_POINT branch_usage,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );


uint32
  arad_egr_prge_interface_alu_nop(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

