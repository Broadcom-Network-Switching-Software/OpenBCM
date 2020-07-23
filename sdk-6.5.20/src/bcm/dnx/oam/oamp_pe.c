
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 
#include <src/bcm/dnx/oam/oamp_pe.h>
#include "src/bcm/dnx/oam/oamp_pe_infra.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <soc/dnx/swstate/auto_generated/access/oam_access.h>






#define OFST DNX_OAMP_PE_FEM_OFST
#define CNST DNX_OAMP_PE_FEM_CNST


#define DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR 0x0EC90000
#define DNX_OAMP_PE_BFD_IPV6_JR1_PROG_VAR       0x0EC90020

#define DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR 0x7F0000FF

#define DNX_OAMP_PE_1DM_PROG_VAR 0x2D10FFFC

#define DNX_OAMP_PE_BFD_VCCV_PROG_VAR 0x10000021






static shr_error_e
oamp_pe_function_default(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], ALWAYS_CMP2, LOP_NXT), DNX_OAMP_PE_INST_LABEL_DEFAULT);
        
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_copy_48_maid(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{

    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 3, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_MAID_48);
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_1B], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_2B], NEVER_NEVER, NXT_JMP));
                 
                 INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_3B], NEVER_NEVER, NXT_JMP));

                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 12, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(FDBK_FIFO,DEFAULT,INST_CONST,3B), ALU_NOP, CMN(READ, NONE, 0x1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_MAID_48_1B);
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 0x31, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 10, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(FDBK_FIFO,DEFAULT,INST_CONST,2B), ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_MAID_48_2B);
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 0x22, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 10, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(FDBK_FIFO,DEFAULT,INST_CONST,1B), ALU_NOP, CMN(READ, NONE, 0x3, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_MAID_48_3B);
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 0x13, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, READ, 10, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_default_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_DEFAULT].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DEFAULT].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_DEFAULT].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_DEFAULT].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_echo_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    int udh_header_size = 0;
    
    uint32 instr_entry_handle_id;

    
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        
        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 6, ALWAYS_NEVER, LOP_CST));

        while (udh_header_size >= 4)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -=4;
        }

       

        switch (udh_header_size)
        {
            case 0:
                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;

            case 1:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;

            case 2:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                break;


            case 3:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_echo_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr, instr_const, udp_legth_offset = 0;
    uint32 saved_prog_index;
    int udh_header_size = 0;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        
        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 9, ALWAYS_NEVER, LOP_CST));

        while (udh_header_size >= 4)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -=4;
        }

        
        switch (udh_header_size)
        {
            case 0:
                
                instr_const = 2;
                break;
            case 1:
                
                instr_const = 3;
                
                udp_legth_offset = 0x13;
                break;
            case 2:
                
                instr_const = 4;
                
                udp_legth_offset = 0x22;
                break;
            case 3:
                
                instr_const = 4;
                
                udp_legth_offset = 0x31;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));

        if (udh_header_size == 3)
        {
            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        }
        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        if (udh_header_size != 0)
        {
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, udp_legth_offset, ALWAYS_NEVER, NXT_JMP));
        }

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_maid_48_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        
        if (func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction));
        }
    
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_lmdm_flexible_da_maid_48_ccm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 6, NEVER_NEVER, LOP_CST));

        
        if (func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction));
        }
    
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_down_mep_inj_self_contained_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        

        
        
        if (func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config
                            [DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
        
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_down_mep_inj_self_contained_maid_48_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        

        
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        
        if (func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config
                            [DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
        
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr2(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        

        
        
        
        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED);

        
        INSTR(CE1_COPY(IN_FIFO, PPH_EXIST_UPDATE, VLD_B, VLD_B, FEM), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG2), CMN(READ, NONE, 0x20,  ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, OR), CE2_COPY(TMP_REG3, DEFAULT, TMP_REG3, 1B), ALU_NOP,CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x8, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(NS_16_VAR_1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x30, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_ASE_SELF_CONTAINED], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x3, NEVER_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG4), CMN(NONE, NONE, 0x2, NEVER_NEVER, NXT_JMP));

        
        
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT, INST_CONST , INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NS_16_VAR_0, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        

        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));


        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_ASE_SELF_CONTAINED);
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));


        
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT, INST_CONST , INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NS_16_VAR_0, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        

        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
        
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr1(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;
    int udh_header_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        

        
        
        
        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED);

        
        INSTR(CE1_NOP, CE2_COPY(NS_16_VAR_0, VSI_GEN_MEM_OFFSET_JR1, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, PPH_EXIST_UPDATE_JR1, VLD_B, VLD_B, FEM), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG2), CMN(READ, NONE, 0x20,  ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));


        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT_JR1, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(NS_16_VAR_1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_PPH_START_JR1], NEVER_NEVER, NXT_JMP));
           
            INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x30, NEVER_NEVER, NXT_JMP));

           
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG3), CMN(NONE, NONE, 0x3, NEVER_NEVER, NXT_JMP));

        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        
        
        
        INSTR_LABEL(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_PPH_START_JR1);
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, INST_CONST, INST_CONST, OR), CE2_COPY(NS_8, OUTLIF_VSI_LSB_SHIFT_JR1, INST_CONST, 1B), ALU_NOP, CMN(NONE, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));

        
        if(udh_header_size>1)
        {
            
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
            if(udh_header_size > 5)
            {

                
                INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size - 5, ALWAYS_NEVER, NXT_JMP));
            }
            else
            {
                
                INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size - 1, ALWAYS_NEVER, NXT_JMP));
            }
        }
        else
        {
             
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size + 3, ALWAYS_NEVER, NXT_JMP));
        }

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG1, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_ETHERNET_JR1], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_ETHERNET_JR1);
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, ALWAYS, POS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_dispatch(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

     
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr1(unit,
                                                                                            label_mode,
                                                                                            func_config,
                                                                                            params,
                                                                                            inst_labels,
                                                                                            next_instruction_index,
                                                                                            first_instruction));
    }
    else
    {
        SHR_IF_ERR_EXIT(oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_jr2(unit,
                                                                                            label_mode,
                                                                                            func_config,
                                                                                            params,
                                                                                            inst_labels,
                                                                                            next_instruction_index,
                                                                                            first_instruction));
    }

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
oamp_pe_function_up_mep_server_maid_48(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP_JR1, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        }
        else
        {
            
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        }

        INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_TC, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_1, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        if (func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
               
               INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
               INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_COPY_MAID_48].first_instruction));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}




static shr_error_e
oamp_pe_function_up_mep_server(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP_JR1, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        }
        else
        {
            
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_DP, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_0, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        }

        INSTR(CE1_COPY(TC_DP_LOCAL_PORT, ITMH_TC, INST_CONST, INST_CONST, OR), CE2_COPY(NS_16_VAR_1, DEFAULT, NONE, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_dm_jumbo_tlv(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PKT_LNTH, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG1), CMN(NONE, NONE, 12, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(PKT_LNTH, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG2), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));

        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_MPLS_COPY], NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_UP_COPY], NEVER_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x1, ALWAYS_NEVER, NXT_JMP), (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_MPLS_COPY ));

        
        
        INSTR_LABEL(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 3, ALWAYS_NEVER, NXT_JMP), (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_UP_COPY));

        
        INSTR(CE1_NOP, CE2_NOP,ALU_SUB(PRG_VAR, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_RSP], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        
        INSTR(CE1_NOP, CE2_COPY(NS_16_VAR_0, TLV_DATA_OFFSET , INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP,ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_COMMON], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR_LABEL(CE1_NOP, CE2_COPY(NS_16_VAR_0,  TLV_DATA_OFFSET_RSP, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP), (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_RSP));

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP, (DNX_OAMP_PE_INST_LABEL_JUMBO_TLV_COMMON));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            
            INSTR(CE1_NOP, CE2_NOP, ALU_ADD(TMP_REG4, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG4), CMN(NONE, NONE, 256, NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_ADD(TMP_REG4, INST_VAR, ALU_OUT, ALWAYS, NEVER, TMP_REG4), CMN(NONE, NONE, 512, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG1), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG3), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(MEM_OUT, TMP_REG2), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, VLD_B, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B), ALU_SUB(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, NPOS, TMP_REG1), CMN(NONE, NONE, 12, ALWAYS_NEVER, LOP_NXT));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0x1, ALWAYS_CMP2, LOP_NXT));

   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_bfd_single_hop_random_dip(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    
    uint8 oamp_tx_format, jr1_format;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        if(oamp_tx_format == jr1_format)
        {
            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 5, ALWAYS_NEVER, LOP_CST));
        }
        else
        {
            
            pph_ttl = 0xFF;
            pph_ttl_msb = pph_ttl >> 5;
            pph_ttl_lsb = (pph_ttl << 3) & 0xFF;


            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            
            INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
            
            INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        }

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 5, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_bfd_mplstp_vccv(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            
            INSTR(CE1_COPY(IN_FIFO, VCCV_BFD, VLD_B, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0,  ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x45, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 255, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
oamp_pe_function_lmdm_flexible_da(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 0, NEVER_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 18, NEVER_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3),CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG2, INST_VAR, NEVER, POS, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_FLEXIBLE_DA_UP_MEP], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x01, ALWAYS_NEVER, NXT_JMP));;

        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0x02, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        
        INSTR_LABEL(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_FLEXIBLE_DA_UP_MEP);
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 0x02, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));
   }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
oamp_pe_bfd_ipv6_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr, instr_const;
    int udh_header_size = 0;
    
    uint32 instr_entry_handle_id;
    uint32 saved_prog_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {

        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));
        

        
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 3, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));
        

        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        
        while (udh_header_size >= 4)
        {
            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -= 4;
        }

        switch (udh_header_size)
        {
            case 0:
                instr_const = 0;
                break;
            case 1:
                instr_const = 1;
                break;
            case 2:
                instr_const = 2;
                break;
            case 3:
                instr_const = 3;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "udh_header_size(%d) error.\n", udh_header_size);
        }
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        

        
        INSTR(CE1_COPY(FDBK_FIFO, BFD_IPV6_ECHO, INST_CONST, INST_CONST, FEM), CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3), CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 2, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG1), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 8, ALWAYS_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_SBFD], NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG2), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        switch (udh_header_size)
        {
            case 0:
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
                    ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;

            case 1:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x12, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

                break;

            case 2:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

                break;

            case 3:
            default:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
                    ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;
        }

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG4, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2),
                CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO], NEVER_NEVER, NXT_JMP),
                DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_SBFD);

            

            
            INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));


            
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT , NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT , INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO], NEVER_NEVER, NXT_JMP));

            
            
            INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

            if (udh_header_size != 0) {
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
            } else {
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            }

        switch (udh_header_size)
        {
            case 0:
                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;
            case 1:
                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x3, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;
            case 2:
                
                INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;
            case 3:
            default:
                
                INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_NO_ECHO);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;
        }

        

        

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, 0x1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_JR1_ECHO);

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG4, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_ipv6_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    
    uint32 instr_entry_handle_id;
    uint32 saved_prog_index;

    
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        

        
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 6, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, OR), CE2_COPY(IN_FIFO, DEFAULT, NONE, 2B), ALU_NOP,CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, NONE), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        


        
        INSTR(CE1_COPY(FDBK_FIFO, BFD_IPV6_ECHO, INST_CONST, INST_CONST, FEM), CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 2, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG1), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 8, ALWAYS_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_SBFD], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
            ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG4, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_DISCR_MSB], NEVER_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x1, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_SBFD);

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO], NEVER_NEVER, NXT_JMP));

        

            
            INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));


            

            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));


        
        INSTR(CE1_NOP, CE2_COPY(PROG_VAR, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG1, DEFAULT, NONE, 2B),
            ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        

        

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP),DNX_OAMP_PE_INST_LABEL_BFD_IPV6_DISCR_MSB);

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_ipv6_mpls_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    int instr_const;
    int udh_header_size = 0;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));
        
        
        


        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP,CMN(READ, NONE, 5, ALWAYS_NEVER, LOP_CST));
        
        while (udh_header_size >= 4)
        {
            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
            udh_header_size -= 4;
        }
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0xFF, NEVER_NEVER, NXT_JMP));

        switch (udh_header_size)
        {
            case 0:
                
                instr_const = 0;
                break;
            case 1:
                
                instr_const = 1;
                break;
            case 2:
                
                instr_const = 2;
                break;
            case 3:
            default:
                
                instr_const = 3;
                break;
        }
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, instr_const, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, NEVER_NEVER, LOP_CST));

        if (udh_header_size != 0) {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
        }

        switch (udh_header_size)
        {
            case 0:
                
                instr_const = 0x30;
                break;
            case 1:
                
                instr_const = 0x0;
                break;
            case 2:
                
                instr_const = 0x10;
                break;
            case 3:
            default:
                
                instr_const = 0x20;
                break;
        }

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, instr_const, NEVER_NEVER, NXT_JMP));
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 7, ALWAYS_NEVER, LOP_CST));

        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x7F, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));

        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 4, NEVER_NEVER, LOP_NXT));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2),
                CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD], NEVER_NEVER, NXT_JMP));

        switch (udh_header_size)
        {
            case 0:

                    

                    
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

                

                
                INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                
                INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));

                

                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                break;
            case 1:

                    

                    
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                

                
                INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));

                

                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x12, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;
            case 2:
                    

                    
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                

                
                INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

                

                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;
            case 3:
            default:

                    

                    
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));
                    INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                

                
                INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_NO_SBFD);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x12, ALWAYS_NEVER, NXT_JMP));

                

                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP),
                        DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_JR1_AFTER_SBFD);
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x1, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;
         }

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_ipv6_mpls_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        
        
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG3), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0x0, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 10, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 10, NEVER_NEVER, LOP_CST));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG1), CMN(NONE, NONE, 0xFF, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, NEVER_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));


        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_SET(FEM, TMP_REG4), CMN(NONE, READ, 0x22, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, NONE, INST_CONST, CONCATENATE),
            CE2_COPY(FDBK_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(NONE, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(FDBK_FIFO, DEFAULT, NONE, 2B), ALU_SET(FEM, TMP_REG3), CMN(NONE, READ, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(FDBK_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 7, ALWAYS_NEVER, LOP_CST));

        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
        
        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x7F, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG1, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(INST_VAR, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, READ, 1, ALWAYS_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 4, NEVER_NEVER, LOP_NXT));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_NO_SBFD], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG2),
                CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_AFTER_SBFD], NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_COPY(TMP_REG4, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x2, ALWAYS_NEVER, NXT_JMP),
                DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_NO_SBFD);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, CONCATENATE), CE2_COPY(TMP_REG3, DEFAULT, NONE, 2B),
            ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_MPLS_AFTER_SBFD);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_function_discr_msb_1(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    oamp_pe_func_config_t * func_config,
    oamp_pe_func_params_s * params,
    int *inst_labels,
    int *next_instruction_index,
    int *first_instruction)
{
    
    uint32 instr_entry_handle_id;

    
    int udh_header_size = 0, rem_bytes_to_discr;

    
    uint8 oamp_tx_format, jr1_format;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    
    if (*first_instruction == DNX_OAMP_PE_NOT_SET)
    {
        *first_instruction = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        
        if (oamp_tx_format == jr1_format)
        {
            rem_bytes_to_discr = udh_header_size & 0x3;
        }
        else
        {
            rem_bytes_to_discr = (2 + udh_header_size) & 0x3;
        }

        
        
        

        switch(rem_bytes_to_discr)
        {
            
            case 0:
                
                INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));
                break;

            
            case 1:
                
                INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, LOP_NXT));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;

            
            case 2:
                
                INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                
                INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x22, NEVER_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR_MSB, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, TMP_REG2, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_COPY(TMP_REG3, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;

            
            case 3:
                
                INSTR_LABEL(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1);

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 0x80, NEVER_NEVER, LOP_NXT));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

                
                INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));
                break;
        }
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_default_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    int udh_header_size = 0, discriminator_offset, dwords_to_discr;
    uint32 saved_prog_index;

    
    uint32 instr_entry_handle_id;

    
    uint8 oamp_tx_format, jr1_format;

    
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        
        oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        jr1_format = dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit);

        
        if (oamp_tx_format == jr1_format)
        {
            
            discriminator_offset = 48 + udh_header_size;
        }
        else
        {
            
            discriminator_offset = 54 + udh_header_size;

            
            pph_ttl = 0xFF;
            pph_ttl_msb = pph_ttl >> 5;
            pph_ttl_lsb = (pph_ttl << 3) & 0xFF;
        }

        
        dwords_to_discr = discriminator_offset >> 2;

        

        
        if(oamp_tx_format == jr1_format)
        {
            

            
            INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, dwords_to_discr, ALWAYS_NEVER, LOP_CST));

                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            
            INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG1), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

            
            INSTR_LABEL(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT), DNX_OAMP_PE_INST_LABEL_BFD_IPV6_NO_ECHO );

            
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG1, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

            
            INSTR(CE1_COPY(TMP_REG1, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

            
            INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, dwords_to_discr-5, ALWAYS_NEVER, LOP_CST));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }

        
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }
        

        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_down_lm_dm_add_udh_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    int udh_header_size = 0;
    uint32 saved_prog_index;

    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, LOP_CST));

        
        if(udh_header_size <= 4)
        {
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, udh_header_size, ALWAYS_NEVER, NXT_JMP));
        }
        else
        {
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x4, ALWAYS_NEVER, NXT_JMP));
            INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, (udh_header_size - 4), ALWAYS_NEVER, NXT_JMP));
        }

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT));

        

        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_up_mep_server_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_up_mep_server_maid_48_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_1dm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 8, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 3, NEVER_NEVER, LOP_NXT));
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG1, PRG_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 0, NEVER_NEVER, LOP_NXT));
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 0, NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 1, NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_SET(INST_VAR, TMP_REG4), CMN(NONE, NONE, 2, NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_1B], NEVER_NEVER, NXT_JMP));
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_2B], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG2, TMP_REG4, INST_VAR, ALWAYS, ZERO, TMP_REG1), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_3B], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV], NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_1B);
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV], NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x21, ALWAYS_NEVER, NXT_JMP));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 2, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_2B);
        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV], NEVER_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        
        
        INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_3B);
        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 1, ALWAYS_NEVER, NXT_JMP));

        
        INSTR_LABEL(CE1_COPY(PROG_VAR, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP), DNX_OAMP_PE_INST_LABEL_1DM_TIME_STAMP_TLV);
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, LOP_CST));
        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_COPY(NONE, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_ALWAYS, NXT_JMP));

        
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_dm_jumbo_tlv_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_DM_JUMBO_TLV].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_single_hop_random_dip_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_mplstp_vccv_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_lmdm_flexible_da_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    uint32 saved_prog_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        
        if (func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].first_instruction == DNX_OAMP_PE_NOT_SET)
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                             &func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].first_instruction));
        }

        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const,
                               func_config[DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA].first_instruction);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_signal_detect_ccm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

       
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 20, NEVER_NEVER, LOP_CST));

        
       INSTR(CE1_NOP, CE2_NOP, ALU_AND(PRG_VAR, NS_8, INST_VAR, ALWAYS, ZERO, TMP_REG4),CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_SIGNAL_DETECT_COPY_REST], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

       
       INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, ALWAYS_NEVER, LOP_CST));

       
       INSTR(CE1_COPY(IN_FIFO, SIGNAL_DETECT_SET, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));

       
       INSTR_LABEL(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(PKT_LNTH, BYTES_RD, NONE, NPOS, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_CMP2, LOP_NXT), DNX_OAMP_PE_INST_LABEL_SIGNAL_DETECT_COPY_REST);

        
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_signal_detect_maid_48_ccm_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;
    
    uint32 instr_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));
    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        
        INSTR(CE1_NOP, CE2_NOP, ALU_ADD(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 82, NEVER_NEVER, LOP_NXT));
        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 4B), ALU_NOP, CMN(READ, WRITE, 12, NEVER_NEVER, LOP_CST));
        
        INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, NEVER_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(PRG_VAR, NS_8, INST_VAR, ALWAYS, ZERO, TMP_REG4),CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 8, ALWAYS_NEVER, LOP_CST));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(NONE, NONE, INST_VAR, ALWAYS, ALWAYS, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_MAID_48_1B], NEVER_NEVER, NXT_JMP));

           
           INSTR(CE1_COPY(IN_FIFO, SIGNAL_DETECT_SET, INST_CONST, INST_CONST, CONCATENATE), CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 2B), ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
           
           INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        
        
        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }
    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_seamless_bfd_ipv4_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;

    
    uint32 instr_entry_handle_id;

    
    uint8 pph_ttl, pph_ttl_msb, pph_ttl_lsb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        pph_ttl = 0xFF;
        pph_ttl_msb = pph_ttl >> 5;
        pph_ttl_lsb = (pph_ttl << 3) & 0xFF;

        
        

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 4, ALWAYS_NEVER, LOP_CST));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x2, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG3), CMN(NONE, NONE, 0x10, NEVER_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_COPY(IN_FIFO, DEFAULT, INST_CONST, 1B), ALU_SET(FEM, TMP_REG2), CMN(NONE, NONE, 0x20, NEVER_NEVER, NXT_JMP));

            
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG3, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG3), CMN(NONE, NONE, pph_ttl_msb, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_OR(TMP_REG2, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, pph_ttl_lsb, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 20, NEVER_NEVER, LOP_NXT));

        
        INSTR(CE1_COPY(TMP_REG3, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(TMP_REG2, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x11, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT) );

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        

        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
oamp_pe_bfd_seamless_bfd_ipv4_jr1_program(
    int unit,
    dnx_oamp_pe_label_mode_e label_mode,
    dbal_enum_value_field_oamp_pe_program_enum_e program_id,
    int program_const,
    int *inst_labels,
    int *next_instruction_index,
    int *next_valid_program_index,
    int *prog_index,
    oamp_pe_func_config_t * func_config)
{
    int first_instr;
    uint32 saved_prog_index;

    
    uint32 instr_entry_handle_id;
    uint32 udh_header_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, program_id, &saved_prog_index));

    
    if (saved_prog_index == DNX_OAMP_PE_NOT_SET)
    {
        first_instr = *next_instruction_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTIONS, &instr_entry_handle_id));

        
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, &udh_header_size));

        
        udh_header_size = udh_header_size & 0x3;

        
        

        
        INSTR(CE1_NOP, CE2_NOP, ALU_SUB(PDU_OFFS, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG1), CMN(NONE, NONE, 20, NEVER_NEVER, LOP_NXT));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, VLD_B, FEM), CE2_NOP, ALU_SUB(TMP_REG1, BYTES_RD, NONE, NEVER, NPOS, TMP_REG1), CMN(READ, NONE, 0, ALWAYS_NEVER, LOP_NXT));

        switch (udh_header_size)
        {
            case 0:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;

            case 1:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 1, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, NEVER_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x13, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));
                break;

            case 2:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 2, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x22, ALWAYS_NEVER, NXT_JMP));
                break;

            case 3:
            default:
                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 3, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(PROG_VAR, DEFAULT, NONE, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(NONE, NONE, 4, ALWAYS_NEVER, NXT_JMP));

                
                INSTR(CE1_COPY(IN_FIFO, DEFAULT, INST_CONST, INST_CONST, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0x31, ALWAYS_NEVER, NXT_JMP));
                break;

        }

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(NS_8, INST_VAR, ALU_OUT, ALWAYS, ALWAYS, TMP_REG2), CMN(NONE, NONE, 1, NEVER_NEVER, LOP_NXT) );

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        INSTR(CE1_COPY(IN_FIFO, DEFAULT, NONE, VLD_B, FEM), CE2_NOP, ALU_NOP, CMN(READ, NONE, 0, ALWAYS_NEVER, NXT_JMP));

        

        
        INSTR(CE1_NOP, CE2_NOP, ALU_AND(TMP_REG2, TMP_REG2, INST_VAR, ALWAYS, ZERO, TMP_REG3), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DEFAULT], NEVER_NEVER, NXT_JMP));

            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
            INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);

        
        if (func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction != DNX_OAMP_PE_NOT_SET)
        {
            
            INSTR(CE1_NOP, CE2_NOP, ALU_SUB(TMP_REG3, TMP_REG3, INST_VAR, ALWAYS, ZERO, TMP_REG4), CMN(NONE, NONE, inst_labels[DNX_OAMP_PE_INST_LABEL_DISCR_MSB_1], NEVER_NEVER, NXT_JMP));
                
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
                INSTR(CE1_NOP, CE2_NOP, ALU_NOP, CMN_NOP);
        }
        else
        {
            SHR_IF_ERR_EXIT(func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].func_ptr
                            (unit, label_mode, func_config, NULL, inst_labels, next_instruction_index,
                            &func_config[DNX_OAMP_PE_FUNC_DISCR_MSB_1].first_instruction));
        }

        

        
        SET_PROGRAM_PROPERTIES(program_id, next_valid_program_index, program_const, first_instr);
    }

    *prog_index = saved_prog_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static int
dnx_oamp_pe_check_fem_soc(
    int unit,
    dnx_oamp_pe_fems_e fem_id)
{
    return 1;
}


static shr_error_e
dnx_oamp_pe_fem_init(
    int unit,
    dnx_oamp_pe_fem_config_s fem_actions[DNX_OAMP_PE_FEM_LAST])
{
    
    dnx_oamp_pe_fem_config_s fem_actions_local[DNX_OAMP_PE_FEM_LAST] = {

        

        
        
        {DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT,     {{OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}, {OFST, 19}, {CNST, 0},  {CNST, 0},  {OFST, 20}, {OFST, 21}, {OFST, 22}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_OUTLIF_VSI_LSB_SHIFT_JR1, {{OFST, 28}, {OFST, 29}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {OFST, 30}, {OFST, 31}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE,         {{OFST, 0},  {OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {OFST, 6},  {OFST, 7},  {OFST, 8},  {CNST, 1},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {OFST, 15}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_PPH_EXIST_UPDATE_JR1,     {{OFST, 0},  {OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {OFST, 6},  {OFST, 7},  {OFST, 8},  {OFST, 9},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {CNST, 1},  {OFST, 15}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_VSI_GEN_MEM_OFFSET_JR1,   {{OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {OFST, 6},  {OFST, 7},  {OFST, 8},  {OFST, 9},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_BFD_IPV6_ECHO,            {{CNST, 0},  {OFST, 17}, {OFST, 18}, {OFST, 19}, {OFST, 20}, {OFST, 21}, {OFST, 22}, {OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_ITMH_DP,                  {{CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {OFST, 26}, {OFST, 27}},   1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_ITMH_DP_JR1,              {{CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {OFST, 26}, {OFST, 27}, {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_ITMH_TC,                  {{CNST, 0},  {OFST, 28}, {OFST, 29}, {OFST, 30}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_SIGNAL_DETECT_SET,        {{OFST, 16}, {OFST, 17}, {OFST, 18}, {OFST, 19}, {OFST, 20}, {OFST, 21}, {CNST, 1},  {OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_TLV_DATA_OFFSET,          {{OFST, 21}, {OFST, 22}, {OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_TLV_DATA_OFFSET_RSP,      {{OFST, 23}, {OFST, 24}, {OFST, 25}, {OFST, 26}, {OFST, 27}, {OFST, 28}, {OFST, 29}, {OFST, 30}, {OFST, 31}, {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0},  {CNST, 0}},    1,    DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FEM_VCCV_BFD,                 {{OFST, 0},  {OFST, 1},  {OFST, 2},  {OFST, 3},  {OFST, 4},  {OFST, 5},  {CNST, 1},  {CNST, 0},  {CNST, 1},  {CNST, 0},  {OFST, 10}, {OFST, 11}, {OFST, 12}, {OFST, 13}, {OFST, 14}, {OFST, 15}},   1,    DNX_OAMP_PE_NOT_SET},
    };
    
    int ii, jj;

    SHR_FUNC_INIT_VARS(unit);
    

    
    for (ii = 0, jj = 0; ii < DNX_OAMP_PE_FEM_LAST; ii++)
    {
        if ((fem_actions_local[ii].valid) && dnx_oamp_pe_check_fem_soc(unit, ii))
        {
            fem_actions_local[ii].hw_index = jj;
            jj++;
        }
    }

    

    
    for (ii = 0; ii < DNX_OAMP_PE_FEM_LAST; ii++)
    {
        if (fem_actions_local[ii].fem_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "fem_actions_local at index %d is out of order.\n", ii);
        }
        fem_actions[ii].fem_id = fem_actions_local[ii].fem_id;
        fem_actions[ii].valid = fem_actions_local[ii].valid;
        fem_actions[ii].hw_index = fem_actions_local[ii].hw_index;
        for (jj = 0; jj < DNX_DATA_MAX_OAM_OAMP_OAMP_PE_NOF_FEM_BITS; jj++)
        {
            fem_actions[ii].bit[jj][0] = fem_actions_local[ii].bit[jj][0];
            fem_actions[ii].bit[jj][1] = fem_actions_local[ii].bit[jj][1];
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static int
dnx_oamp_pe_mep_pe_profile_soc_disable(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e profile_id)
{
    return 0;
}


static shr_error_e
dnx_oamp_pe_mep_pe_profile_init(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES])
{

    int system_headers_mode;


dnx_oamp_mep_pe_profile_s mep_pe_profiles_local[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES] = {

    

    
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT,                            0,              0,              0,              0,              0,          1,   0},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO,                           0,              0,              0,              0,              0,          1,   1},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,27,             48,             29,             50,             0,          1,   2},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED,        27,             48,             29,             50,             0,          1,   3},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48,                            0,              0,              0,              0,              0,          1,   4},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT,                        50,             0,              0,              0,              0,          1,   5},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6,                           50,             0,              0,              0,              0,          1,   6},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS,                      47,             0,              0,              0,              0,          1,   8},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP,                           0,              0,              0,              0,              0,          1,   10},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48,                   0,              0,              0,              0,              0,          1,   11},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER,                      0,              0,              29,             50,             0,          1,   12},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48,              0,              0,              29,             50,             0,          1,   13},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM,                                0,              0,              0,              0,              0,          1,   14},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48,                        0,              0,              0,              0,              0,          1,   15},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA,                   0,              0,              29,             50,             0,          1,   16},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48,           0,              0,              29,             50,             0,          1,   17},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT,                      48,             0,              0,              0,              0,          1,   18},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48,              48,             0,              0,              0,              0,          1,   19},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48,               0,              0,              29,             50,             0,          1,   20},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV,                       0,              0,              29,             50,             0,          1,   21},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4,                  50,             0,              0,              0,              0,          1,   22},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6,                  50,             0,              0,              0,              0,          1,   23},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP,           0,             0,              0,              0,              0,          1,   24},
    {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV,                    0,             0,              0,              0,              0,          1,   25}
};

    
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP].hw_index & 0xfffe) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48].hw_index & 0xfffe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Down Mep PE hardware profile value w/wo maid48 should be different by 1 lsb bit only.\n");
    }

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER].hw_index & 0xfffe) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48].hw_index & 0xfffe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "UP Mep Server PE hardware profile value w/wo maid48 should be different by 1 lsb bit only.\n");
    }

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].hw_index & 0xfffe) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].hw_index & 0xfffe))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Jumbo DM tlv PE hardware profile value w/wo maid48 should be different by 1 lsb bit only.\n");
    }

    if ((mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48].hw_index & 0xffef) !=
        (mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].hw_index & 0xffef))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "JUMBO MAID48 and not jumbo maid 48  should be different by 1 4'th bit.\n");
    }

    
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

     
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].word_0_offset = 54;
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].byte_0_offset = 0;
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].word_0_offset = 54;
        mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].byte_0_offset = 0;
    }

    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].word_0_offset = 54;
    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].byte_0_offset = 0;
    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].word_0_offset = 54;
    mep_pe_profiles_local[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].byte_0_offset = 0;

    
    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if ((mep_pe_profiles_local[ii].valid) && dnx_oamp_pe_mep_pe_profile_soc_disable(unit, ii))
        {
            mep_pe_profiles_local[ii].valid = 0;
        }
    }

    
    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if (mep_pe_profiles_local[ii].profile_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mep_pe_profile_actions at index %d is out of order.\n", ii);
        }
    }

    
    sal_memcpy(mep_pe_profiles, mep_pe_profiles_local, sizeof(mep_pe_profiles_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static int
dnx_oamp_pe_tcam_soc_enable(
    int unit,
    oamp_pe_tcam_config_s tcam_config_local[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES],
    dbal_enum_value_field_oamp_pe_prog_tcam_enum_e tcam_id,
    int udh_header_size)
{
    int system_headers_mode;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (tcam_id)
    {
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48:
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO:
            tcam_config_local[tcam_id].valid =
                (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT:
            tcam_config_local[tcam_id].valid = TRUE;
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1:
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_MAID48_CCM_UDH_JR1:
            tcam_config_local[tcam_id].valid = ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                                                && (udh_header_size != 0));
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        case DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1:
            tcam_config_local[tcam_id].valid = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE);
            break;
        default:
            break;
    }

    return tcam_config_local[tcam_id].valid;
}


static shr_error_e
dnx_oamp_pe_tcam_init(
    int unit,
    oamp_pe_tcam_config_s tcam_config[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES])
{
    
    
    oamp_pe_tcam_config_s tcam_config_local[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES] = {

        
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,                 oamp_pe_down_mep_inj_self_contained_program,          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,           0x00000000,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED},         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x3}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,         oamp_pe_down_mep_inj_self_contained_maid_48_program,  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,   0x00000001,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48}, {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM,   oamp_pe_down_mep_inj_self_contained_program,          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED,           0x00000000,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48}, {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1,                                oamp_pe_bfd_echo_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1,                          DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO},                            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1,                           oamp_pe_bfd_ipv6_mpls_jr1_program,                    1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1,                     DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR,      OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1,                                oamp_pe_bfd_ipv6_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1,                          DNX_OAMP_PE_BFD_IPV6_JR1_PROG_VAR,       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6},                            {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS,                               oamp_pe_bfd_ipv6_mpls_program,                        1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS,                         DNX_OAMP_PE_BFD_IPV6_MPLS_PROG_VAR,      OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6,                                    oamp_pe_bfd_ipv6_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6,                              DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6},                            {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48,                                     oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48},                             {0x10}, {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO,                                    oamp_pe_bfd_echo_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO,                              DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO},                            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT,                                 oamp_pe_bfd_default_program,                          1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DEFAULT,                           0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT},                         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1,                        oamp_pe_down_lm_dm_add_udh_jr1_program,               1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_ADD_UDH_JR1,                  0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x1,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP},                            {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_MAID48_CCM_UDH_JR1,                 oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48},                    {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER,                               oamp_pe_up_mep_server_program,                        1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER,                         0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48,                       oamp_pe_up_mep_server_maid_48_program,                1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER_MAID_48,                 0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48},               {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM,                                         oamp_pe_1dm_program,                                  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM,                                   DNX_OAMP_PE_1DM_PROG_VAR,                OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM},                                 {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48,                                 oamp_pe_1dm_program,                                  1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM,                                   DNX_OAMP_PE_1DM_PROG_VAR,                OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48},                         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM,                             oamp_pe_maid_48_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48},                         {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA,                            oamp_pe_lmdm_flexible_da_program,                     1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA,                      0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA},                    {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48,                    oamp_pe_lmdm_flexible_da_program,                     1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA,                      0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48},            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0x1}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM,                oamp_pe_lmdm_flexible_da_maid_48_ccm_program,         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM,          0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48},            {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM,                           oamp_pe_signal_detect_ccm_program,                    1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_CCM,                     0x00000002,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE},     {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT},                       {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM,                   oamp_pe_signal_detect_maid_48_ccm_program,            1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_MAID_48_CCM,             0x00000002,                              OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE},     {0x0}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48},               {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV,                                oamp_pe_dm_jumbo_tlv_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV,                          0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48},                {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP,                            oamp_pe_dm_jumbo_tlv_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP,                      1,                                       OAMP_PE_TCAM_KEY_MEP_RESPONSE,  0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48},                {0x1},  {DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR},          {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4,                           oamp_pe_bfd_seamless_bfd_ipv4_program,                1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4,                     7784,                                    OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6,                           oamp_pe_bfd_ipv6_program,                             1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6,                     DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1,                       oamp_pe_bfd_seamless_bfd_ipv4_jr1_program,            1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1,                 7784,                                    OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1,                       oamp_pe_bfd_ipv6_jr1_program,                         1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1,                 DNX_OAMP_PE_BFD_ECHO_UDP_DPORT_PROG_VAR, OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP}, {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6},                   {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP,                   oamp_pe_bfd_single_hop_random_dip_program,            1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP,             0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP},           {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV,                            oamp_pe_bfd_mplstp_vccv_program,                      1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV,                      DNX_OAMP_PE_BFD_VCCV_PROG_VAR,           OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS},      {0x3}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV},                    {0x0},  {DBAL_ENUM_FVAL_MSG_TYPE_OTHER},            {0x0}}},DNX_OAMP_PE_NOT_SET},
        {DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT,                                     oamp_pe_default_program,                              1,     DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT,                               0,                                       OAMP_PE_TCAM_KEY_MEP_GENERATED, 0x0,           {{{DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM},       {0xF}, {0}, {0x7F}, {DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT},                             {0x3F}, {DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR},  {0xF}}},DNX_OAMP_PE_NOT_SET}
    };
    
    int ii, jj;
    int system_headers_mode;
    int udh_header_size = 0;
    uint32 prog_var_tmp = 0;

    SHR_FUNC_INIT_VARS(unit);
    

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

     
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
         
        SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));
        prog_var_tmp = ((0x20 << 24) | (((udh_header_size << 1) | 1) << 16) | (0xc0 << 8));

         
        tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].program_const =
            prog_var_tmp |
            tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].program_const;

        tcam_config_local
            [DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].program_const =
            prog_var_tmp |
            tcam_config_local
            [DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].program_const;

        tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].program_const =
            prog_var_tmp |
            tcam_config_local[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].program_const;
    }

    for (ii = 0, jj = 0; ii < DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES; ii++)
    {
        if (dnx_oamp_pe_tcam_soc_enable(unit, tcam_config_local, ii, udh_header_size))
        {
            tcam_config_local[ii].hw_index = jj;
            jj++;
        }
    }

    
    for (ii = 0; ii < DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES; ii++)
    {
        if (tcam_config_local[ii].tcam_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tcam_config_local at index %d is out of order.\n", ii);
        }
    }

    sal_memcpy(tcam_config, tcam_config_local, sizeof(tcam_config_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_pe_func_init(
    int unit,
    oamp_pe_func_config_t func_config[DNX_OAMP_PE_FUNC_NOF])
{
    
    oamp_pe_func_config_t func_config_local[DNX_OAMP_PE_FUNC_NOF] = {
        
        {DNX_OAMP_PE_FUNC_DEFAULT,                                        oamp_pe_function_default,                                                 DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_COPY_MAID_48,                                   oamp_pe_function_copy_48_maid,                                            DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DOWN_MEP_INJ_BUILD_ITMH_EXT_PPH_SELF_CONTAINED, oamp_pe_function_down_mep_inj_build_itmh_ext_pph_self_contained_dispatch, DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DISCR_MSB_1,                                    oamp_pe_function_discr_msb_1,                                             DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_UP_MEP_SERVER,                                  oamp_pe_function_up_mep_server,                                           DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_UP_MEP_SERVER_MAID_48,                          oamp_pe_function_up_mep_server_maid_48,                                   DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_LMDM_FLEXIBLE_DA,                               oamp_pe_function_lmdm_flexible_da,                                        DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_DM_JUMBO_TLV,                                   oamp_pe_function_dm_jumbo_tlv,                                            DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_SINGLE_HOP_RANDOM_DIP,                          oamp_pe_function_bfd_single_hop_random_dip,                               DNX_OAMP_PE_NOT_SET},
        {DNX_OAMP_PE_FUNC_VCCV_TYPE_BFD,                                  oamp_pe_function_bfd_mplstp_vccv,                                         DNX_OAMP_PE_NOT_SET},
};
    
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    

    
    for (ii = 0; ii < DNX_OAMP_PE_FUNC_NOF; ii++)
    {
        if (func_config_local[ii].func_id != ii)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "func_config_local at index %d is out of order.\n", ii);
        }
    }

    sal_memcpy(func_config, func_config_local, sizeof(func_config_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_pe_init_inst_labels(
    int unit,
    int inst_labels[DNX_OAMP_PE_INST_LABEL_LAST])
{
    int inst_labels_local[DNX_OAMP_PE_INST_LABEL_LAST];
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DNX_OAMP_PE_INST_LABEL_LAST; ii++)
    {
        inst_labels_local[ii] = DNX_OAMP_PE_NOT_SET;
    }

    

    sal_memcpy(inst_labels, inst_labels_local, sizeof(inst_labels_local));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}




shr_error_e
dnx_oamp_pe_module_init(
    int unit)
{
    int ii, prog_index;
    
    int default_instruction;
    
    int max_nof_instructions;
    
    int next_instruction_index = 0;
    
    int next_valid_program_index = 0;
    
    dnx_oamp_pe_fem_config_s fem_actions[DNX_OAMP_PE_FEM_LAST];
    
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES];
    
    int inst_labels[DNX_OAMP_PE_INST_LABEL_LAST];
    
    oamp_pe_tcam_config_s tcam_config[DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES];
    oamp_pe_func_config_t func_config[DNX_OAMP_PE_FUNC_NOF];

    SHR_FUNC_INIT_VARS(unit);

    
    default_instruction = dnx_data_oam.oamp.oamp_pe_default_instruction_index_get(unit);
    max_nof_instructions = dnx_data_oam.oamp.oamp_pe_max_nof_instructions_get(unit);

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_init_inst_labels(unit, inst_labels));
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_fem_init(unit, fem_actions));
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_init(unit, mep_pe_profiles));
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_tcam_init(unit, tcam_config));
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_func_init(unit, func_config));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_set(unit, mep_pe_profiles));
    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_set(unit, mep_pe_profiles));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_fem_set(unit, fem_actions));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_instruction_config_init
                    (unit, default_instruction, max_nof_instructions, DBAL_ENUM_FVAL_DEBUG_MODE_OFF));

    

    
    for (ii = DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES - 1; ii > -1; ii--)
    {
        if (tcam_config[ii].valid)
        {
            SHR_IF_ERR_EXIT(tcam_config[ii].program_ptr
                            (unit, DNX_OAMP_PE_LABEL_MODE_ON, tcam_config[ii].program_id, tcam_config[ii].program_const,
                             inst_labels, &next_instruction_index, &next_valid_program_index, &prog_index,
                             func_config));
        }
    }
    
    next_instruction_index = 0;
    next_valid_program_index = 0;
    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_func_init(unit, func_config));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_ENUM2HW));

    
    for (ii = DBAL_NOF_ENUM_OAMP_PE_PROG_TCAM_ENUM_VALUES - 1; ii > -1; ii--)
    {
        if (tcam_config[ii].valid)
        {
            SHR_IF_ERR_EXIT(tcam_config[ii].program_ptr
                            (unit, DNX_OAMP_PE_LABEL_MODE_OFF, tcam_config[ii].program_id,
                             tcam_config[ii].program_const, inst_labels, &next_instruction_index,
                             &next_valid_program_index, &prog_index, func_config));
            if (prog_index != DNX_OAMP_PE_NOT_SET)
            {
                
                SHR_IF_ERR_EXIT(dnx_oamp_pe_programs_tcam_set(unit, tcam_config[ii], prog_index));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_pe_sw_mep_pe_profile_get(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e profile,
    uint32 *profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROFILE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE_SW, profile);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, profile_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
oamp_pe_udh_header_size_get(
    int unit,
    uint32 *udh_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_UDH, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UDH_SIZE, INST_SINGLE, udh_size));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}






