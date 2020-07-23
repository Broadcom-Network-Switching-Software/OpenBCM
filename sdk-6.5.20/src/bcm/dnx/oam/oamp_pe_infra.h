
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _OAMP_PE_INFRA_H_INCLUDED__

#define _OAMP_PE_INFRA_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif


#include <shared/shrextend/shrextend_error.h>
#include "include/bcm/oam.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dbal/dbal.h>




#define DNX_OAMP_PE_FEM_CNST 1

#define DNX_OAMP_PE_FEM_OFST 0

#define DNX_OAMP_PE_NOT_SET 0xFFFFFFFF


typedef enum
{
    DNX_OAMP_PE_LABEL_MODE_OFF,
    DNX_OAMP_PE_LABEL_MODE_ON
} dnx_oamp_pe_label_mode_e;


typedef enum
{
    CE_PATH_1,
    CE_PATH_2
} dnx_oamp_pe_ce_path_e;


typedef enum
{
    
    DNX_OAMP_PE_FIFO_NONE,
    
    DNX_OAMP_PE_FIFO_READ
} dnx_oamp_pe_fifo_action_e;


typedef enum
{
    
    DNX_OAMP_PE_FDBK_FIFO_NONE,
    
    DNX_OAMP_PE_FDBK_FIFO_READ,
    
    DNX_OAMP_PE_FDBK_FIFO_WRITE,
    
    DNX_OAMP_PE_FDBK_FIFO_R_W
} dnx_oamp_pe_fdbk_fifo_action_e;


typedef enum
{
    DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_NEVER,
    DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_ALWAYS,
    DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_CMP1,
    DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_CMP2,
    DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_NEVER,
    DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_ALWAYS,
    DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_CMP1,
    DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_CMP2,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_NEVER,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_ALWAYS,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_CMP1,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_CMP2,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_NEVER,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_ALWAYS,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_CMP1,
    DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_CMP2
} dnx_oamp_pe_buff_eop_action_e;


typedef enum
{
    DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT,
    DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT_JR1,
    DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE,
    DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE_JR1,
    DNX_OAMP_PE_FEM_VSI_GEN_MEM_OFFSET_JR1,
    DNX_OAMP_PE_FEM_BFD_IPV6_ECHO,
    DNX_OAMP_PE_FEM_ITMH_DP,
    DNX_OAMP_PE_FEM_ITMH_DP_JR1,
    DNX_OAMP_PE_FEM_ITMH_TC,
    DNX_OAMP_PE_FEM_SIGNAL_DETECT_SET,
    DNX_OAMP_PE_FEM_TLV_DATA_OFFSET,
    DNX_OAMP_PE_FEM_TLV_DATA_OFFSET_RSP,
    DNX_OAMP_PE_FEM_VCCV_BFD,
    
    DNX_OAMP_PE_FEM_LAST,
    
    DNX_OAMP_PE_FEM_DEFAULT = 31
} dnx_oamp_pe_fems_e;


typedef enum
{
    DNX_OAMP_PE_INST_LABEL_DEFAULT,
    DNX_OAMP_PE_INST_LABEL_MAID_48_1B,
    DNX_OAMP_PE_INST_LABEL_MAID_48_2B,
    DNX_OAMP_PE_INST_LABEL_MAID_48_3B,
    DNX_OAMP_PE_INST_LABEL_MAID_48,
    DNX_OAMP_PE_INST_LABEL_PPH_START_JR1,
    DNX_OAMP_PE_INST_LABEL_ETHERNET_JR1,
    DNX_OAMP_PE_INST_LABEL_ASE_SELF_CONTAINED,
    DNX_OAMP_PE_INST_LABEL_ASE_SHORT_OFFL,
    DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED,
    DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SHORT_OFFL,
    DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_SBFD,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_SBFD,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_DISCR_MSB,
    DNX_OAMP_PE_INST_LABEL_1DM_1B,
    DNX_OAMP_PE_INST_LABEL_1DM_2B,
    DNX_OAMP_PE_INST_LABEL_1DM_3B,
    DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV,
    DNX_OAMP_PE_INST_LABEL_FLEXIBLE_DA_UP_MEP,
    DNX_OAMP_PE_INST_LABEL_SIGNAL_DETECT_COPY_REST,
    DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_MPLS_COPY,
    DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_UP_COPY,
    DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_DOWN_COPY,
    DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_RSP,
    DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_COMMON,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_NO_SBFD,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_AFTER_SBFD,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD,
    DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD,
    
    DNX_OAMP_PE_INST_LABEL_LAST
} dnx_oamp_pe_instruction_labels_e;


typedef enum
{
    DNX_OAMP_PE_FUNC_DEFAULT,
    DNX_OAMP_PE_FUNC_COPY_MAID_48,
    DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED,
    DNX_OAMP_PE_FUNC_DISCR_MSB_1,
    DNX_OAMP_PE_FUNC_UP_MEP_SERVER,
    DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48,
    DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA,
    DNX_OAMP_PE_FUNC_DM_JUMBO_TLV,
    DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP,
    DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD,
    
    DNX_OAMP_PE_FUNC_NOF
} dnx_oamp_pe_func_name_e;


typedef struct
{
    int dummy;
} oamp_pe_func_params_s;

typedef struct oamp_pe_func_config_s oamp_pe_func_config_t;


typedef shr_error_e(
    *oamp_pe_func_ptr) (
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction);

struct oamp_pe_func_config_s
{
    dnx_oamp_pe_func_name_e func_id;
    oamp_pe_func_ptr func_ptr;
    int first_instruction;
};

typedef struct
{
    dnx_oamp_pe_fems_e fem_id;
    
    int bit[DNX_DATA_MAX_OAM_OAMP_OAMP_PE_NOF_FEM_BITS][2];
    int valid;
    int hw_index;
} dnx_oamp_pe_fem_config_s;

typedef struct
{
    dbal_enum_value_field_mep_pe_profile_sw_e profile_id;
    int byte_0_offset;
    int byte_1_offset;
    int word_0_offset;
    int word_1_offset;
    int mep_insert;
    int valid;
    int hw_index;
} dnx_oamp_mep_pe_profile_s;


typedef enum
{
    OAMP_PE_TCAM_KEY_MEP_GENERATED,
    OAMP_PE_TCAM_KEY_MEP_RESPONSE,
    OAMP_PE_TCAM_KEY_PUNT,
    OAMP_PE_TCAM_KEY_EVENT,
    OAMP_PE_TCAM_KEY_SAT,
    OAMP_PE_TCAM_KEY_TXO,
    OAMP_PE_TCAM_KEY_DMA_1,
    OAMP_PE_TCAM_KEY_DMA_2
} oamp_pe_tcam_key_types_e;


typedef shr_error_e(
    *oamp_pe_programs_ptr) (
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config);


typedef struct
{
    dbal_enum_value_result_type_oamp_mep_db_e mep_type[1];
    uint32 type_mask[1];
    
    uint32 mep_profile[1];
    uint32 profile_mask[1];
    uint32 mep_pe_profile[1];
    uint32 pe_profile_mask[1];
    dbal_enum_value_field_msg_type_e msg_type[1];
    uint32 msg_mask[1];
} dnx_oamp_pe_mep_generated_s;


typedef union
{
    dnx_oamp_pe_mep_generated_s mep_generated;
    dnx_oamp_pe_mep_generated_s mep_response;
    dnx_oamp_pe_mep_generated_s txo;
    struct
    {
        uint32 punt_profile;
        uint32 profile_mask;
        uint32 trap_code;
        uint32 trap_mask;
    } punt;
    struct
    {
        uint32 event_pe_sel;
        uint32 event_mask;
    } event;
    struct
    {
        uint32 sat_pe_key;
        uint32 sat_pe_mask;
        uint32 generator_index;
        uint32 generator_mask;
    } sat;
    struct
    {
        uint32 data;
        uint32 data_mask;
    } dma;
} dnx_oamp_pe_tcam_key_struct_u;


typedef struct
{
    dbal_enum_value_field_oamp_pe_prog_tcam_enum_e tcam_id;
    oamp_pe_programs_ptr program_ptr;
    int valid;
    dbal_enum_value_field_oamp_pe_program_enum_e program_id;
    int program_const;
    oamp_pe_tcam_key_types_e key_type;
    uint32 key_type_mask;
    dnx_oamp_pe_tcam_key_struct_u key;
    int hw_index;
} oamp_pe_tcam_config_s;


shr_error_e dnx_oamp_pe_ce_copy(
    int unit,
    uint32 entry_handle_id,
    dnx_oamp_pe_ce_path_e path,
    dbal_enum_value_field_mux_src_e src,
    dnx_oamp_pe_fems_e fem,
    dbal_enum_value_field_buff_size_src_e shift,
    dbal_enum_value_field_buff_size_src_e buff1_size,
    dbal_enum_value_field_buff2_size_e buff2_size,
    dbal_enum_value_field_merge_inst_e merge);


shr_error_e dnx_oamp_pe_alu(
    int unit,
    uint32 entry_handle_id,
    dbal_enum_value_field_op_sel_e op1,
    dbal_enum_value_field_op_sel_e op2,
    dbal_enum_value_field_alu_act_e alu_action,
    dbal_enum_value_field_op3_sel_e op3,
    dbal_enum_value_field_cmp_act_e cmp1,
    dbal_enum_value_field_cmp_act_e cmp2,
    dbal_enum_value_field_alu_dst_e alu_dst);


shr_error_e dnx_oamp_pe_common(
    int unit,
    uint32 entry_handle_id,
    dnx_oamp_pe_fifo_action_e fifo,
    dnx_oamp_pe_fdbk_fifo_action_e fdbk_fifo,
    int common,
    dnx_oamp_pe_buff_eop_action_e buff_eop,
    dbal_enum_value_field_inst_src_e next_inst);

#define SET_PROGRAM_PROPERTIES(prog_enum, _prog_index_ptr, _prog_const, _first_instr)   \
do { \
    if (label_mode == DNX_OAMP_PE_LABEL_MODE_OFF) \
    { \
        SHR_IF_ERR_EXIT(dnx_oamp_pe_programs_properties_set(unit, *_prog_index_ptr, _prog_const, _first_instr)); \
         \
        SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_hw2enum_set(unit, *_prog_index_ptr, prog_enum)); \
    } \
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_set(unit, prog_enum, *_prog_index_ptr )); \
    saved_prog_index = *_prog_index_ptr; \
     \
    (*_prog_index_ptr)++; \
} while (0)


#define INSTR(_interface_ce_instr1, _interface_ce_instr2, _interface_alu_instr, _common) \
do { \
    if (label_mode == DNX_OAMP_PE_LABEL_MODE_OFF)\
    { \
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, instr_entry_handle_id)); \
        dbal_entry_key_field32_set(unit, instr_entry_handle_id, DBAL_FIELD_OAMP_PE_INSTRUCT_INDEX, *next_instruction_index); \
        SHR_IF_ERR_EXIT(_interface_ce_instr1); \
        SHR_IF_ERR_EXIT(_interface_ce_instr2); \
        SHR_IF_ERR_EXIT(_interface_alu_instr); \
        SHR_IF_ERR_EXIT(_common); \
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, instr_entry_handle_id, DBAL_COMMIT)); \
    } \
    (*next_instruction_index)++; \
} while(0)


#define INSTR_LABEL(_interface_ce_instr1, _interface_ce_instr2, _interface_alu_instr, _common, label) \
do { \
    if (label_mode == DNX_OAMP_PE_LABEL_MODE_ON)\
    { \
        if (inst_labels[label] == DNX_OAMP_PE_NOT_SET) \
        { \
            inst_labels[label] = *next_instruction_index; \
        } \
        (*next_instruction_index)++; \
    } \
    else \
    { \
        INSTR(_interface_ce_instr1, _interface_ce_instr2, _interface_alu_instr, _common); \
    } \
} while(0)


#define CE1_COPY(src, fem, shift, buff_size, merge)    \
    dnx_oamp_pe_ce_copy(unit, instr_entry_handle_id, CE_PATH_1, DBAL_ENUM_FVAL_MUX_SRC_##src, DNX_OAMP_PE_FEM_##fem, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_##shift, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_##buff_size, DBAL_ENUM_FVAL_BUFF2_SIZE_1B, DBAL_ENUM_FVAL_MERGE_INST_##merge)

#define CE2_COPY(src, fem, shift, buff_size)    \
    dnx_oamp_pe_ce_copy(unit, instr_entry_handle_id, CE_PATH_2, DBAL_ENUM_FVAL_MUX_SRC_##src, DNX_OAMP_PE_FEM_##fem, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_##shift, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE, DBAL_ENUM_FVAL_BUFF2_SIZE_##buff_size, DBAL_ENUM_FVAL_MERGE_INST_NONE)

#define CE1_NOP  \
    dnx_oamp_pe_ce_copy(unit, instr_entry_handle_id, CE_PATH_1, DBAL_ENUM_FVAL_MUX_SRC_NONE, DNX_OAMP_PE_FEM_DEFAULT, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE, DBAL_ENUM_FVAL_BUFF2_SIZE_1B, DBAL_ENUM_FVAL_MERGE_INST_NONE)
#define CE2_NOP  \
    dnx_oamp_pe_ce_copy(unit, instr_entry_handle_id, CE_PATH_2, DBAL_ENUM_FVAL_MUX_SRC_NONE, DNX_OAMP_PE_FEM_DEFAULT, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE, DBAL_ENUM_FVAL_BUFF2_SIZE_1B, DBAL_ENUM_FVAL_MERGE_INST_NONE)


#define ALU(op1, op2, alu_action, op3, cmp1, cmp2, alu_dst)  \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_##op1, DBAL_ENUM_FVAL_OP_SEL_##op2, DBAL_ENUM_FVAL_ALU_ACT_##alu_action, DBAL_ENUM_FVAL_OP3_SEL_##op3, DBAL_ENUM_FVAL_CMP_ACT_##cmp1, DBAL_ENUM_FVAL_CMP_ACT_##cmp2, DBAL_ENUM_FVAL_ALU_DST_##alu_dst)

#define ALU_SUB(op1, op2, op3, cmp1, cmp2, alu_dst)  \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_##op1, DBAL_ENUM_FVAL_OP_SEL_##op2, DBAL_ENUM_FVAL_ALU_ACT_SUB, DBAL_ENUM_FVAL_OP3_SEL_##op3, DBAL_ENUM_FVAL_CMP_ACT_##cmp1, DBAL_ENUM_FVAL_CMP_ACT_##cmp2, DBAL_ENUM_FVAL_ALU_DST_##alu_dst)

#define ALU_ADD(op1, op2, op3, cmp1, cmp2, alu_dst)  \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_##op1, DBAL_ENUM_FVAL_OP_SEL_##op2, DBAL_ENUM_FVAL_ALU_ACT_ADD, DBAL_ENUM_FVAL_OP3_SEL_##op3, DBAL_ENUM_FVAL_CMP_ACT_##cmp1, DBAL_ENUM_FVAL_CMP_ACT_##cmp2, DBAL_ENUM_FVAL_ALU_DST_##alu_dst)

#define ALU_AND(op1, op2, op3, cmp1, cmp2, alu_dst)  \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_##op1, DBAL_ENUM_FVAL_OP_SEL_##op2, DBAL_ENUM_FVAL_ALU_ACT_AND, DBAL_ENUM_FVAL_OP3_SEL_##op3, DBAL_ENUM_FVAL_CMP_ACT_##cmp1, DBAL_ENUM_FVAL_CMP_ACT_##cmp2, DBAL_ENUM_FVAL_ALU_DST_##alu_dst)

#define ALU_OR(op1, op2, op3, cmp1, cmp2, alu_dst)  \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_##op1, DBAL_ENUM_FVAL_OP_SEL_##op2, DBAL_ENUM_FVAL_ALU_ACT_OR, DBAL_ENUM_FVAL_OP3_SEL_##op3, DBAL_ENUM_FVAL_CMP_ACT_##cmp1, DBAL_ENUM_FVAL_CMP_ACT_##cmp2, DBAL_ENUM_FVAL_ALU_DST_##alu_dst)

#define ALU_SET(op3, alu_dst) \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_NONE, DBAL_ENUM_FVAL_OP_SEL_NONE, DBAL_ENUM_FVAL_ALU_ACT_ADD, DBAL_ENUM_FVAL_OP3_SEL_##op3, DBAL_ENUM_FVAL_CMP_ACT_ALWAYS, DBAL_ENUM_FVAL_CMP_ACT_NEVER, DBAL_ENUM_FVAL_ALU_DST_##alu_dst)

#define ALU_NOP \
    dnx_oamp_pe_alu(unit, instr_entry_handle_id, DBAL_ENUM_FVAL_OP_SEL_NONE, DBAL_ENUM_FVAL_OP_SEL_NONE, DBAL_ENUM_FVAL_ALU_ACT_ADD, DBAL_ENUM_FVAL_OP3_SEL_NONE, DBAL_ENUM_FVAL_CMP_ACT_NEVER, DBAL_ENUM_FVAL_CMP_ACT_NEVER, DBAL_ENUM_FVAL_ALU_DST_TMP_REG1)


#define CMN(fifo, fdbk_fifo, common, buff_eop, next_inst)   \
    dnx_oamp_pe_common(unit, instr_entry_handle_id, DNX_OAMP_PE_FIFO_##fifo, DNX_OAMP_PE_FDBK_FIFO_##fdbk_fifo, common, DNX_OAMP_PE_BUFF_EOP_ACTION_##buff_eop, DBAL_ENUM_FVAL_INST_SRC_##next_inst)


#define CMN_NOP \
    dnx_oamp_pe_common(unit, instr_entry_handle_id, DNX_OAMP_PE_FIFO_NONE, DNX_OAMP_PE_FDBK_FIFO_NONE, 0, DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_NEVER, DBAL_ENUM_FVAL_INST_SRC_NXT_JMP)


shr_error_e dnx_oamp_pe_programs_properties_set(
    int unit,
    int prog_index,
    int const_value,
    int first_instr);


shr_error_e dnx_oamp_pe_programs_tcam_set(
    int unit,
    oamp_pe_tcam_config_s tcam_config,
    int prog_index);


shr_error_e dnx_oamp_pe_mep_pe_profile_set(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES]);


shr_error_e dnx_oamp_pe_fem_set(
    int unit,
    dnx_oamp_pe_fem_config_s fem_actions[DNX_OAMP_PE_FEM_LAST]);


shr_error_e dnx_oamp_pe_mep_pe_profile_sw_set(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES]);


shr_error_e dnx_oamp_pe_mep_pe_profile_sw_get(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e mep_pe_profile_sw,
    uint32 *mep_pe_profile_hw);


shr_error_e dnx_oamp_pe_program_sw_hw2enum_set(
    int unit,
    int hw_index,
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum);


shr_error_e dnx_oamp_pe_program_sw_hw2enum_get(
    int unit,
    int hw_index,
    uint32 *prog_enum);


shr_error_e dnx_oamp_pe_program_sw_enum2hw_set(
    int unit,
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum,
    int hw_index);


shr_error_e dnx_oamp_pe_program_sw_enum2hw_get(
    int unit,
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum,
    uint32 *hw_index);


shr_error_e dnx_oamp_pe_tcam_sw_set(
    int unit,
    int hw_index,
    dbal_enum_value_field_oamp_pe_prog_tcam_enum_e tcam_enum);


shr_error_e dnx_oamp_pe_tcam_sw_get(
    int unit,
    int hw_index,
    uint32 *tcam_enum);


shr_error_e dnx_oamp_pe_instruction_config_init(
    int unit,
    int instruction,
    int max_nof_instructions,
    dbal_enum_value_field_debug_mode_e debug_mode);


#endif 
