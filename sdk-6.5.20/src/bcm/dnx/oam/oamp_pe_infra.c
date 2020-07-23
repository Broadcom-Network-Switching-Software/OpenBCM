
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 
#include <src/bcm/dnx/oam/oamp_pe_infra.h>

extern shr_error_e dbal_fields_field_parent_encode_custom_val(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e parent_field_id,
    uint32 encode_mask_val,
    uint32 *field_mask_val,
    uint32 *parent_field_full_mask);










shr_error_e
dnx_oamp_pe_ce_copy(
    int unit,
    uint32 entry_handle_id,
    dnx_oamp_pe_ce_path_e path,
    dbal_enum_value_field_mux_src_e src,
    dnx_oamp_pe_fems_e fem,
    dbal_enum_value_field_buff_size_src_e shift,
    dbal_enum_value_field_buff_size_src_e buff1_size,
    dbal_enum_value_field_buff2_size_e buff2_size,
    dbal_enum_value_field_merge_inst_e merge)
{
    dbal_fields_e mux_field, fem_field, shift_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((merge == DBAL_ENUM_FVAL_MERGE_INST_NONE) && (path == CE_PATH_1))
    {
        SHR_EXIT();
    }

    if (path == CE_PATH_1)
    {
        mux_field = DBAL_FIELD_MUX1_SRC;
        fem_field = DBAL_FIELD_FEM1_SEL;
        shift_field = DBAL_FIELD_SHIFT1_SRC;
    }
    else
    {
        mux_field = DBAL_FIELD_MUX2_SRC;
        fem_field = DBAL_FIELD_FEM2_SEL;
        shift_field = DBAL_FIELD_SHIFT2_SRC;

        if (merge != DBAL_ENUM_FVAL_MERGE_INST_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "MERGE action not supported for path 2, merge must be set to DBAL_ENUM_FVAL_MERGE_INST_NONE\n");
        }
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, mux_field, INST_SINGLE, src);
    dbal_entry_value_field32_set(unit, entry_handle_id, fem_field, INST_SINGLE, fem);
    dbal_entry_value_field32_set(unit, entry_handle_id, shift_field, INST_SINGLE, shift);

    if (path == CE_PATH_1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFF_SIZE_SRC, INST_SINGLE, buff1_size);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MERGE_INST, INST_SINGLE, merge);
    }
    else if (path == CE_PATH_2)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFF2_SIZE, INST_SINGLE, buff2_size);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_alu(
    int unit,
    uint32 entry_handle_id,
    dbal_enum_value_field_op_sel_e op1,
    dbal_enum_value_field_op_sel_e op2,
    dbal_enum_value_field_alu_act_e alu_action,
    dbal_enum_value_field_op3_sel_e op3,
    dbal_enum_value_field_cmp_act_e cmp1,
    dbal_enum_value_field_cmp_act_e cmp2,
    dbal_enum_value_field_alu_dst_e alu_dst)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OP1_SEL, INST_SINGLE, op1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OP2_SEL, INST_SINGLE, op2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALU_ACT, INST_SINGLE, alu_action);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OP3_SEL, INST_SINGLE, op3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMP1_ACT, INST_SINGLE, cmp1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CMP2_ACT, INST_SINGLE, cmp2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALU_DST, INST_SINGLE, alu_dst);

    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_common(
    int unit,
    uint32 entry_handle_id,
    dnx_oamp_pe_fifo_action_e fifo,
    dnx_oamp_pe_fdbk_fifo_action_e fdbk_fifo,
    int common,
    dnx_oamp_pe_buff_eop_action_e buff_eop,
    dbal_enum_value_field_inst_src_e next_inst)
{
    int fdbk_fifo_read, fdbk_fifo_write;
    dbal_enum_value_field_buff_wr_e buff_wr;
    dbal_enum_value_field_buf_eop_e eop_wr;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    switch (fdbk_fifo)
    {
        case DNX_OAMP_PE_FDBK_FIFO_NONE:
            fdbk_fifo_read = 0;
            fdbk_fifo_write = 0;
            break;
        case DNX_OAMP_PE_FDBK_FIFO_READ:
            fdbk_fifo_read = 1;
            fdbk_fifo_write = 0;
            break;
        case DNX_OAMP_PE_FDBK_FIFO_WRITE:
            fdbk_fifo_read = 0;
            fdbk_fifo_write = 1;
            break;
        case DNX_OAMP_PE_FDBK_FIFO_R_W:
            fdbk_fifo_read = 1;
            fdbk_fifo_write = 1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid fdbk_fifo (%d) provided\n", fdbk_fifo);
    }

    
    switch (buff_eop)
    {
        case DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_NEVER:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_NEVER;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_ALWAYS:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_NEVER;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_ALWAYS;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_CMP1:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_NEVER;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_NEVER_CMP2:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_NEVER;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_NEVER:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ALWAYS;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_ALWAYS:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ALWAYS;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_ALWAYS;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_CMP1:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ALWAYS;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_ALWAYS_CMP2:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ALWAYS;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_NEVER:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_ALWAYS:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_ALWAYS;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_CMP1:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP1_CMP2:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_NEVER:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_ALWAYS:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_ALWAYS;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_CMP1:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1;
            break;
        case DNX_OAMP_PE_BUFF_EOP_ACTION_CMP2_CMP2:
            buff_wr = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2;
            eop_wr = DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid buff_eop (%d) provided\n", buff_eop);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_FIFO_RD, INST_SINGLE, fifo);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FDBK_FF_RD_BIT, INST_SINGLE, fdbk_fifo_read);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FDBK_FF_WR_BIT, INST_SINGLE, fdbk_fifo_write);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INST_CONST, INST_SINGLE, common);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFF_WR, INST_SINGLE, buff_wr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUF_EOP, INST_SINGLE, eop_wr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INST_SRC, INST_SINGLE, next_inst);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_programs_properties_set(
    int unit,
    int prog_index,
    int const_value,
    int first_instr)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, prog_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, const_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROGRAM_ADRESS, INST_SINGLE, first_instr);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oamp_pe_tcam_key_set(
    int unit,
    oamp_pe_tcam_config_s tcam_config,
    uint32 *entry_handle_id)
{
    uint32 complete_key_val = 0;
    uint32 encoded_complete_key_val = 0;
    uint32 complete_key_mask_val = 0;
    uint32 encoded_complete_key_mask_val = 0;

    dbal_fields_e child_field_id;
    uint32 mep_pe_profile = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (tcam_config.key_type)
    {
        case OAMP_PE_TCAM_KEY_MEP_GENERATED:
        case OAMP_PE_TCAM_KEY_MEP_RESPONSE:
        case OAMP_PE_TCAM_KEY_TXO:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_MEP_GENERATED, DBAL_FIELD_OAMP_MEP_TYPE,
                             tcam_config.key.mep_generated.mep_type, tcam_config.key.mep_generated.type_mask,
                             &complete_key_val, &complete_key_mask_val));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_MEP_GENERATED, DBAL_FIELD_MEP_PROFILE,
                             tcam_config.key.mep_generated.mep_profile, tcam_config.key.mep_generated.profile_mask,
                             &complete_key_val, &complete_key_mask_val));

            SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                            (unit,
                             (dbal_enum_value_field_mep_pe_profile_sw_e) tcam_config.key.
                             mep_generated.mep_pe_profile[0], &mep_pe_profile));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_MEP_GENERATED, DBAL_FIELD_MEP_PE_PROFILE, &mep_pe_profile,
                             tcam_config.key.mep_generated.pe_profile_mask, &complete_key_val, &complete_key_mask_val));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_MEP_GENERATED, DBAL_FIELD_MSG_TYPE,
                             tcam_config.key.mep_generated.msg_type, tcam_config.key.mep_generated.msg_mask,
                             &complete_key_val, &complete_key_mask_val));

            child_field_id = tcam_config.key_type == OAMP_PE_TCAM_KEY_MEP_GENERATED ? DBAL_FIELD_MEP_GENERATED :
                tcam_config.key_type == OAMP_PE_TCAM_KEY_MEP_RESPONSE ? DBAL_FIELD_MEP_RESPONSE : DBAL_FIELD_TXO_PACKET;

            SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_OAMP_PE_PROG_TCAM, child_field_id,
                                                                 &complete_key_val, &encoded_complete_key_val));

            SHR_IF_ERR_EXIT(dbal_fields_field_parent_encode_custom_val
                            (unit, child_field_id, DBAL_FIELD_OAMP_PE_PROG_TCAM, tcam_config.key_type_mask,
                             &complete_key_mask_val, &encoded_complete_key_mask_val));

            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_OAMP_PE_PROG_TCAM,
                                              encoded_complete_key_val, encoded_complete_key_mask_val);
            break;
        case OAMP_PE_TCAM_KEY_PUNT:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_PUNT_PACKET, DBAL_FIELD_PUNT_PE_PROFILE,
                             &tcam_config.key.punt.punt_profile, &tcam_config.key.punt.profile_mask, &complete_key_val,
                             &complete_key_mask_val));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_PUNT_PACKET, DBAL_FIELD_PUNT_TRAP_CODE, &tcam_config.key.punt.trap_code,
                             &tcam_config.key.punt.trap_mask, &complete_key_val, &complete_key_mask_val));

            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_PUNT_PACKET, complete_key_val,
                                              complete_key_mask_val);
            break;
        case OAMP_PE_TCAM_KEY_EVENT:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_EVENT_PACKET, DBAL_FIELD_EVENT_PACKET_PE_SEL,
                             &tcam_config.key.event.event_pe_sel, &tcam_config.key.event.event_mask, &complete_key_val,
                             &complete_key_mask_val));

            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_EVENT_PACKET, complete_key_val,
                                              complete_key_mask_val);
            break;
        case OAMP_PE_TCAM_KEY_SAT:
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_SAT_PACKET, DBAL_FIELD_SAT_PE_KEY_MSB, &tcam_config.key.sat.sat_pe_key,
                             &tcam_config.key.sat.sat_pe_mask, &complete_key_val, &complete_key_mask_val));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                            (unit, DBAL_FIELD_SAT_PACKET, DBAL_FIELD_GENERATOR_INDEX,
                             &tcam_config.key.sat.generator_index, &tcam_config.key.sat.generator_mask,
                             &complete_key_val, &complete_key_mask_val));

            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_SAT_PACKET, complete_key_val,
                                              complete_key_mask_val);
            break;
        case OAMP_PE_TCAM_KEY_DMA_1:
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_DMA_PACKET_1,
                                              tcam_config.key.dma.data, tcam_config.key.dma.data_mask);
            break;
        case OAMP_PE_TCAM_KEY_DMA_2:
            dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_DMA_PACKET_2,
                                              tcam_config.key.dma.data, tcam_config.key.dma.data_mask);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "TCAM key type (%d) not supported\n", tcam_config.key_type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_programs_tcam_set(
    int unit,
    oamp_pe_tcam_config_s tcam_config,
    int prog_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_TCAM, &entry_handle_id));

    
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, tcam_config.hw_index));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_tcam_key_set(unit, tcam_config, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM, INST_SINGLE, (uint32) prog_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, tcam_config.valid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(dnx_oamp_pe_tcam_sw_set(unit, tcam_config.hw_index, tcam_config.tcam_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_mep_pe_profile_set(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES])
{
    int ii;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PE_PROFILES, &entry_handle_id));

    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if (mep_pe_profiles[ii].valid)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, mep_pe_profiles[ii].hw_index);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTE_0_OFFSET, INST_SINGLE,
                                         mep_pe_profiles[ii].byte_0_offset);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BYTE_1_OFFSET, INST_SINGLE,
                                         mep_pe_profiles[ii].byte_1_offset);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WORD_0_OFFSET, INST_SINGLE,
                                         mep_pe_profiles[ii].word_0_offset);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WORD_1_OFFSET, INST_SINGLE,
                                         mep_pe_profiles[ii].word_1_offset);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_INSERT, INST_SINGLE,
                                         mep_pe_profiles[ii].mep_insert);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_fem_set(
    int unit,
    dnx_oamp_pe_fem_config_s fem_actions[DNX_OAMP_PE_FEM_LAST])
{
    int ii, bit_index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_FEMS, &entry_handle_id));

    for (ii = 0; ii < DNX_OAMP_PE_FEM_LAST; ii++)
    {
        if (fem_actions[ii].valid)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PE_FEM_INDEX, fem_actions[ii].hw_index);
            for (bit_index = 0; bit_index < DNX_DATA_MAX_OAM_OAMP_OAMP_PE_NOF_FEM_BITS; bit_index++)
            {
                if (fem_actions[ii].bit[bit_index][0] == DNX_OAMP_PE_FEM_OFST)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEM_OFFSET, bit_index,
                                                 fem_actions[ii].bit[bit_index][1]);
                }
                else
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEM_CONST, bit_index,
                                                 fem_actions[ii].bit[bit_index][1]);
                }
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_mep_pe_profile_sw_set(
    int unit,
    dnx_oamp_mep_pe_profile_s mep_pe_profiles[DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES])
{
    int ii;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROFILE_SW, &entry_handle_id));

    for (ii = 0; ii < DBAL_NOF_ENUM_MEP_PE_PROFILE_SW_VALUES; ii++)
    {
        if (mep_pe_profiles[ii].valid)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE_SW, ii);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE,
                                         mep_pe_profiles[ii].hw_index);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_mep_pe_profile_sw_get(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e mep_pe_profile_sw,
    uint32 *mep_pe_profile_hw)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROFILE_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE_SW, mep_pe_profile_sw);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, INST_SINGLE, mep_pe_profile_hw));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_program_sw_hw2enum_set(
    int unit,
    int hw_index,
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_HW2ENUM, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM, hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, INST_SINGLE, prog_enum);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_program_sw_hw2enum_get(
    int unit,
    int hw_index,
    uint32 *prog_enum)
{
    uint32 entry_handle_id;
    uint32 valid;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_HW2ENUM, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM, hw_index);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, INST_SINGLE, prog_enum));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &valid));
    if (!valid)
    {
        *prog_enum = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_INVALID;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_program_sw_enum2hw_set(
    int unit,
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum,
    int hw_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_ENUM2HW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, prog_enum);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM, INST_SINGLE, hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_program_sw_enum2hw_get(
    int unit,
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum,
    uint32 *hw_index)
{
    uint32 entry_handle_id;
    uint32 valid;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAM_SW_ENUM2HW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_ENUM, prog_enum);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM, INST_SINGLE, hw_index));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &valid));
    if (!valid)
    {
        *hw_index = DNX_OAMP_PE_NOT_SET;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_tcam_sw_set(
    int unit,
    int hw_index,
    dbal_enum_value_field_oamp_pe_prog_tcam_enum_e tcam_enum)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_TCAM_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROG_TCAM_ID, hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROG_TCAM_ENUM, INST_SINGLE, tcam_enum);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_tcam_sw_get(
    int unit,
    int hw_index,
    uint32 *tcam_enum)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_TCAM_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROG_TCAM_ID, hw_index);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROG_TCAM_ENUM, INST_SINGLE, tcam_enum));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_oamp_pe_instruction_config_init(
    int unit,
    int instruction,
    int max_nof_instructions,
    dbal_enum_value_field_debug_mode_e debug_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_INSTRUCTION_GENERAL, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_INST_ADDR, INST_SINGLE, instruction);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_INST, INST_SINGLE, max_nof_instructions);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEBUG_MODE, INST_SINGLE, debug_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



 

 
