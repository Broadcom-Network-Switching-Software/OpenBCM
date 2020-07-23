/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON DIAG C
 */
 
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/drv.h>
#include <soc/dnxc/error.h>
#include <soc/dnxf/cmn/dnxf_drv.h> 
#include <soc/dnxf/cmn/dnxf_diag.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/dnxf/ramon/ramon_diag.h>

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

const soc_dnxf_diag_flag_str_t soc_ramon_diag_flag_str_interrupts_names[] = 
{
    {0, "ECI_GLOBAL"},
    {1, "OCCG"},
    {2, "AVS_TOP"},
    {3, "DCML0"},
    {4, "DCML1"},
    {5, "DCML2"},
    {6, "DCML3"},
    {7, ""},
    {8, "DCML4"},
    {9, "DCML5"},
    {10, "DCML6"},
    {11, "DCML7"},
    {12, "FMAC0"},
    {13, "FMAC1"},
    {14, "FMAC2"},
    {15, "FMAC3"},
    {16, "FMAC4"},
    {17, "FMAC5"},
    {18, "FMAC6"},
    {19, "FMAC7"},
    {20, "FMAC8"},
    {21, "FMAC9"},
    {22, "FMAC10"},
    {23, "FMAC11"},
    {24, "FMAC12"},
    {25, "FMAC13"},
    {26, "FMAC14"},
    {27, "FMAC15"},
    {28, "FMAC16"},
    {29, "FMAC17"},
    {30, "FMAC18"},
    {31, "FMAC19"},
    {32, "FMAC20"},
    {33, "FMAC21"},
    {34, "FMAC22"},
    {35, "FMAC23"},
    {36, "FMAC24"},
    {37, "FMAC25"},
    {38, "FMAC26"},
    {39, "FMAC27"},
    {40, "FMAC28"},
    {41, "FMAC29"},
    {42, "FMAC30"},
    {43, "FMAC31"},
    {44, "FMAC32"},
    {45, "FMAC33"},
    {46, "FMAC34"},
    {47, "FMAC35"},
    {48, "FMAC36"},
    {49, "FMAC37"},
    {50, "FMAC38"},
    {51, "FMAC39"},
    {52, "FMAC40"},
    {53, "FMAC41"},
    {54, "FMAC42"},
    {55, "FMAC43"},
    {56, "FMAC44"},
    {57, "FMAC45"},
    {58, "FMAC46"},
    {59, "FMAC47"},
    {60, "FSRD0"},
    {61, "FSRD1"},
    {62, "FSRD2"},
    {63, "FSRD3"},
    {64, "FSRD4"},
    {65, "FSRD5"},
    {66, "FSRD6"},
    {67, "FSRD7"},
    {68, "FSRD8"},
    {69, "FSRD9"},
    {70, "FSRD10"},
    {71, "FSRD11"},
    {72, "FSRD12"},
    {73, "FSRD13"},
    {74, "FSRD14"},
    {75, "FSRD15"},
    {76, "FSRD16"},
    {77, "FSRD17"},
    {78, "FSRD18"},
    {79, "FSRD19"},
    {80, "FSRD20"},
    {81, "FSRD21"},
    {82, "FSRD22"},
    {83, "FSRD23"},
    {84, ""},
    {85, ""},
    {86, "RTP"},
    {87, "MCT"},
    {88, "QRH0"},
    {89, "QRH1"},
    {90, "QRH2"},
    {91, "QRH3"},
    {92, "QRH4"},
    {93, "QRH5"},
    {94, "QRH6"},
    {95, "QRH7"},
    {96, "DCH0"},
    {97, "DCH1"},
    {98, "DCH2"},
    {99, "DCH3"},
    {100, "DCH4"},
    {101, "DCH5"},
    {102, "DCH6"},
    {103, "DCH7"},
    {104, "CCH0"},
    {105, "CCH1"},
    {106, "CCH2"},
    {107, "CCH3"},
    {108, "CCH4"},
    {109, "CCH5"},
    {110, "CCH6"},
    {111, "CCH7"},
    {112, "LCM0"},
    {113, "LCM1"},
    {114, "LCM2"},
    {115, "LCM3"},
    {116, "LCM4"},
    {117, "LCM5"},
    {118, "LCM6"},
    {119, "LCM7"},
    {-1,  NULL}
};

shr_error_e
soc_ramon_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dnxf_diag_flag_str_t **intr_names)
{
    SHR_FUNC_INIT_VARS(unit);

    *intr_names = soc_ramon_diag_flag_str_interrupts_names;

    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_diag_cell_pipe_counter_get(int unit, int pipe, uint64 * counter)
{
    int i;
    uint32 reg_val32;
    uint64 sum;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_SET(sum, 0, 0);

    switch (pipe) {
    case 0: 
        for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
        {
            SHR_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_0r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_0r, reg_val32, DCH_TOTAL_CELL_CNT_P_0f));
        }
        break;

    case 1: 
        for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
        {
            SHR_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_1r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_1r, reg_val32, DCH_TOTAL_CELL_CNT_P_1f));
        }
        break;

    case 2: 
        for (i = 0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
        {
            SHR_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_2r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_2r, reg_val32, DCH_TOTAL_CELL_CNT_P_2f));
        }
        break;

    case -1: 
        for (i = 0 ; i <dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
        {
            SHR_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_0r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_0r, reg_val32, DCH_TOTAL_CELL_CNT_P_0f));

            SHR_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_1r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_1r, reg_val32, DCH_TOTAL_CELL_CNT_P_1f));

            SHR_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_2r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_2r, reg_val32, DCH_TOTAL_CELL_CNT_P_2f));
        }
        break;

    default:
        SHR_ERR_EXIT(_SHR_E_PARAM, "pipe %d does not exist\n", pipe);
    }

    COMPILER_64_SET(*counter, COMPILER_64_HI(sum), COMPILER_64_LO(sum));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e 
soc_ramon_counters_get_info(int unit,soc_dnxf_counters_info_t* fe_counters_info)
{
    int nof_pipes = 0;
    uint32 block_idx, pipe_idx, priority_idx, dtm_block_idx, dch_block_idx;
    uint32 block_idx_start, block_idx_end, fe13_offset;
    uint32 max_fifo_cells_lr, max_fifo_cells_nlr, max_fifo_cells, temp_max_fifo_cells_cnt;
    uint32 reg_val;
    uint64 reg_val64;

    soc_dnxf_dch_counters_info_t * current_dch_blocks_info;
    soc_dnxf_dtm_counters_info_t * current_dtm_blocks_info;
    soc_dnxf_dtl_counters_info_t * current_dtl_blocks_info;

    soc_reg_t   dch_total_in_per_pipe_reg_list[3] = {DCH_DCH_TOTAL_CELL_CNT_P_0r,  DCH_DCH_TOTAL_CELL_CNT_P_1r, DCH_DCH_TOTAL_CELL_CNT_P_2r};
    soc_field_t dch_total_in_per_pipe_field_list[3] = {DCH_TOTAL_CELL_CNT_P_0f,  DCH_TOTAL_CELL_CNT_P_1f, DCH_TOTAL_CELL_CNT_P_2f};

    soc_reg_t   dch_total_out_per_pipe_reg_list[3] = {DCH_DCH_TOTAL_OUT_CELL_CNT_P_0r,  DCH_DCH_TOTAL_OUT_CELL_CNT_P_1r, DCH_DCH_TOTAL_OUT_CELL_CNT_P_2r};
    soc_field_t dch_total_out_per_pipe_field_list[3] = {DCH_TOTAL_OUT_CELL_CNT_P_0f,  DCH_TOTAL_OUT_CELL_CNT_P_1f, DCH_TOTAL_OUT_CELL_CNT_P_2f};

    soc_reg_t   dch_ifm_occupancy_per_pipe_reg_list[3] = {DCH_IFM_MAX_OCCUP_P_0r,  DCH_IFM_MAX_OCCUP_P_1r, DCH_IFM_MAX_OCCUP_P_2r};
    soc_field_t dch_ifm_occupancy_per_pipe_field_list[9] = {MAX_IFMA_STAT_P_0f,  MAX_IFMB_STAT_P_0f, MAX_IFMC_STAT_P_0f,
                                                            MAX_IFMA_STAT_P_1f,  MAX_IFMB_STAT_P_1f, MAX_IFMC_STAT_P_1f,
                                                            MAX_IFMA_STAT_P_2f,  MAX_IFMB_STAT_P_2f, MAX_IFMC_STAT_P_2f};

    soc_reg_t   dtm_total_in_per_pipe_reg_list[3] = {LCM_DTM_TOTAL_CELL_CNT_DCH_P_0r,  LCM_DTM_TOTAL_CELL_CNT_DCH_P_1r, LCM_DTM_TOTAL_CELL_CNT_DCH_P_2r};
    soc_field_t dtm_total_in_per_pipe_field_list[3] = {DTM_TOTAL_CELL_CNT_DCH_NP_0f,  DTM_TOTAL_CELL_CNT_DCH_NP_1f, DTM_TOTAL_CELL_CNT_DCH_NP_2f};

    soc_reg_t   dtm_max_fifo_cells_per_pipe_reg_list[3] = {LCM_DTM_FRAG_CNT_INFO_P_0_DCHr,  LCM_DTM_FRAG_CNT_INFO_P_1_DCHr, LCM_DTM_FRAG_CNT_INFO_P_2_DCHr};
    soc_field_t dtm_max_fifo_cells_per_pipe_field_list[3] = {DTM_FRAG_CNT_MAX_P_0_DCH_Nf,  DTM_FRAG_CNT_MAX_P_1_DCH_Nf, DTM_FRAG_CNT_MAX_P_2_DCH_Nf};

    soc_reg_t   dtl_total_dropped_nlr_cells_per_pipe_reg_list[3] = {LCM_DTL_DROPPED_IP_PR_P_0_CNTr,  LCM_DTL_DROPPED_IP_PR_P_1_CNTr, LCM_DTL_DROPPED_IP_PR_P_2_CNTr};
    soc_field_t dtl_total_dropped_nlr_cells_per_pipe_field_list[3] = {DTL_DROPPED_IP_PR_NP_0_CNTf,  DTL_DROPPED_IP_PR_NP_1_CNTf, DTL_DROPPED_IP_PR_NP_2_CNTf};

    soc_reg_t   dtl_total_dropped_lr_cells_per_pipe_reg_list[3] = {LCM_DTL_LCLRT_DROPPED_IP_PR_P_0_CNTr, LCM_DTL_LCLRT_DROPPED_IP_PR_P_1_CNTr, LCM_DTL_LCLRT_DROPPED_IP_PR_P_2_CNTr};
    soc_field_t dtl_total_dropped_lr_cells_per_pipe_field_list[3] = {DTL_LCLRT_DROPPED_IP_PR_NP_0_CNTf,  DTL_LCLRT_DROPPED_IP_PR_NP_1_CNTf, DTL_LCLRT_DROPPED_IP_PR_NP_2_CNTf};

    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    fe_counters_info -> nof_pipes = nof_pipes;
    
    for (pipe_idx = 0 ; pipe_idx < nof_pipes ; pipe_idx ++) {
    
        for (fe13_offset = 0; fe13_offset <= 1; fe13_offset++)
        {
            if (fe13_offset == 0)
            {   
                current_dch_blocks_info = &fe_counters_info->dch_fe1_counters_info[pipe_idx];
                block_idx_start = 0;
                block_idx_end = dnxf_data_device.blocks.nof_instances_dch_get(unit)/2;
            } else {
                
                current_dch_blocks_info = &fe_counters_info->dch_fe3_counters_info[pipe_idx];
                block_idx_start = dnxf_data_device.blocks.nof_instances_dch_get(unit)/2;
                block_idx_end = dnxf_data_device.blocks.nof_instances_dch_get(unit);
            }

             max_fifo_cells = 0;
             temp_max_fifo_cells_cnt = 0;
            
            for (block_idx = block_idx_start; block_idx < block_idx_end; block_idx++)
            {
                
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dch_total_in_per_pipe_reg_list[pipe_idx], block_idx, 0, &reg_val));
                current_dch_blocks_info->dch_total_in += soc_reg_field_get(unit, dch_total_in_per_pipe_reg_list[pipe_idx], reg_val, dch_total_in_per_pipe_field_list[pipe_idx]);

                
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dch_total_out_per_pipe_reg_list[pipe_idx], block_idx, 0, &reg_val));
                current_dch_blocks_info->dch_total_out += soc_reg_field_get(unit, dch_total_out_per_pipe_reg_list[pipe_idx], reg_val, dch_total_out_per_pipe_field_list[pipe_idx]);

                
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCH_FIFO_DISCARD_CNT_Pr, block_idx, pipe_idx, &reg_val));
                current_dch_blocks_info->dch_fifo_discard += soc_reg_field_get(unit, DCH_FIFO_DISCARD_CNT_Pr, reg_val, FIFO_DISCARD_CNT_P_Nf);

                
                SHR_IF_ERR_EXIT(READ_DCH_UNREACH_DEST_CNTr(unit, block_idx, &reg_val));
                current_dch_blocks_info->dch_unreach_discard += soc_reg64_field32_get(unit, DCH_UNREACH_DEST_CNTr, reg_val, UNREACH_DEST_CNTf);

                
                SHR_IF_ERR_EXIT(soc_reg_get(unit, dch_ifm_occupancy_per_pipe_reg_list[pipe_idx], block_idx, 0, &reg_val64));
                temp_max_fifo_cells_cnt = soc_reg64_field32_get(unit, dch_ifm_occupancy_per_pipe_reg_list[pipe_idx], reg_val64, dch_ifm_occupancy_per_pipe_field_list[pipe_idx*3 + 0]);
                max_fifo_cells = (temp_max_fifo_cells_cnt > max_fifo_cells) ? temp_max_fifo_cells_cnt : max_fifo_cells;
                temp_max_fifo_cells_cnt = soc_reg64_field32_get(unit, dch_ifm_occupancy_per_pipe_reg_list[pipe_idx], reg_val64, dch_ifm_occupancy_per_pipe_field_list[pipe_idx*3 + 1]);
                max_fifo_cells = (temp_max_fifo_cells_cnt > max_fifo_cells) ? temp_max_fifo_cells_cnt : max_fifo_cells;
                temp_max_fifo_cells_cnt = soc_reg64_field32_get(unit, dch_ifm_occupancy_per_pipe_reg_list[pipe_idx], reg_val64, dch_ifm_occupancy_per_pipe_field_list[pipe_idx*3 + 2]);
                max_fifo_cells = (temp_max_fifo_cells_cnt > max_fifo_cells) ? temp_max_fifo_cells_cnt : max_fifo_cells;
            }
            current_dch_blocks_info->dch_max_fifo_cells = max_fifo_cells;
        }

    

    
        
        current_dtm_blocks_info = &fe_counters_info->dtm_fe1_counters_info[pipe_idx];

        temp_max_fifo_cells_cnt = 0;
        
        for (dtm_block_idx = dnxf_data_device.blocks.nof_instances_dcml_get(unit)/2; dtm_block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); dtm_block_idx++ )
        {
            
            for (dch_block_idx = 0; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_block_idx++ )
            {
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtm_total_in_per_pipe_reg_list[pipe_idx], dtm_block_idx, dch_block_idx, &reg_val));
                current_dtm_blocks_info->dtm_total_in_nlr += soc_reg_field_get(unit, dtm_total_in_per_pipe_reg_list[pipe_idx], reg_val, dtm_total_in_per_pipe_field_list[pipe_idx]);
            }

            
            for (dch_block_idx = 0; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_block_idx++)
            {
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtm_max_fifo_cells_per_pipe_reg_list[pipe_idx], dtm_block_idx, dch_block_idx, &reg_val));
                temp_max_fifo_cells_cnt = soc_reg_field_get(unit, dtm_max_fifo_cells_per_pipe_reg_list[pipe_idx], reg_val, dtm_max_fifo_cells_per_pipe_field_list[pipe_idx]);
                max_fifo_cells = (temp_max_fifo_cells_cnt > max_fifo_cells) ? temp_max_fifo_cells_cnt : max_fifo_cells;
            }

            

            
            SHR_IF_ERR_EXIT(READ_LCM_DTM_TOTAL_OUT_CELL_CNT_Pr(unit, dtm_block_idx, pipe_idx, &reg_val));
            current_dtm_blocks_info->dtm_total_out_nlr += soc_reg_field_get(unit, LCM_DTM_TOTAL_OUT_CELL_CNT_Pr, reg_val, DTM_TOTAL_OUT_CELL_CNT_P_Nf);

        }
        current_dtm_blocks_info->dtm_max_fifo_cells_nlr = max_fifo_cells;
         

        
        current_dtm_blocks_info = &fe_counters_info->dtm_fe3_counters_info[pipe_idx];
        temp_max_fifo_cells_cnt = 0;
        current_dtm_blocks_info->dtm_max_fifo_cells_lr = 0;
        current_dtm_blocks_info->dtm_max_fifo_cells_nlr = 0;

        for (dtm_block_idx = 0; dtm_block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit)/2; dtm_block_idx++ )
        {
            for (dch_block_idx = 0; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit)/2; dch_block_idx++ )
            {
                
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtm_total_in_per_pipe_reg_list[pipe_idx], dtm_block_idx, dch_block_idx, &reg_val));
                current_dtm_blocks_info->dtm_total_in_lr += soc_reg_field_get(unit, dtm_total_in_per_pipe_reg_list[pipe_idx], reg_val, dtm_total_in_per_pipe_field_list[pipe_idx]);
            }

            for (dch_block_idx = dnxf_data_device.blocks.nof_instances_dch_get(unit)/2; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_block_idx++ )
            {
                
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtm_total_in_per_pipe_reg_list[pipe_idx], dtm_block_idx, dch_block_idx, &reg_val));
                current_dtm_blocks_info->dtm_total_in_nlr += soc_reg_field_get(unit, dtm_total_in_per_pipe_reg_list[pipe_idx], reg_val, dtm_total_in_per_pipe_field_list[pipe_idx]);
            }
            
            for (dch_block_idx = 0; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit)/2; dch_block_idx++ )
            {
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtm_max_fifo_cells_per_pipe_reg_list[pipe_idx], dtm_block_idx, dch_block_idx, &reg_val));
                temp_max_fifo_cells_cnt = soc_reg_field_get(unit, dtm_max_fifo_cells_per_pipe_reg_list[pipe_idx], reg_val, dtm_max_fifo_cells_per_pipe_field_list[pipe_idx]);
                current_dtm_blocks_info->dtm_max_fifo_cells_lr = (temp_max_fifo_cells_cnt > current_dtm_blocks_info->dtm_max_fifo_cells_lr) ? temp_max_fifo_cells_cnt : current_dtm_blocks_info->dtm_max_fifo_cells_lr;
            }

            
            for (dch_block_idx = dnxf_data_device.blocks.nof_instances_dch_get(unit)/2; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_block_idx++ )
            {
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtm_max_fifo_cells_per_pipe_reg_list[pipe_idx], dtm_block_idx, dch_block_idx, &reg_val));
                temp_max_fifo_cells_cnt = soc_reg_field_get(unit, dtm_max_fifo_cells_per_pipe_reg_list[pipe_idx], reg_val, dtm_max_fifo_cells_per_pipe_field_list[pipe_idx]);
                current_dtm_blocks_info->dtm_max_fifo_cells_nlr = (temp_max_fifo_cells_cnt > current_dtm_blocks_info->dtm_max_fifo_cells_nlr) ? temp_max_fifo_cells_cnt : current_dtm_blocks_info->dtm_max_fifo_cells_nlr;
            }

            
            SHR_IF_ERR_EXIT(READ_LCM_DTM_LCLRT_TOTAL_OUT_CELL_CNT_Pr(unit, dtm_block_idx, pipe_idx, &reg_val));
            current_dtm_blocks_info->dtm_total_out_lr += soc_reg_field_get(unit, LCM_DTM_LCLRT_TOTAL_OUT_CELL_CNT_Pr, reg_val, DTM_LCLRT_TOTAL_OUT_CELL_CNT_P_Nf);

            
            SHR_IF_ERR_EXIT(READ_LCM_DTM_TOTAL_OUT_CELL_CNT_Pr(unit, dtm_block_idx, pipe_idx, &reg_val));
            current_dtm_blocks_info->dtm_total_out_nlr += soc_reg_field_get(unit, LCM_DTM_TOTAL_OUT_CELL_CNT_Pr, reg_val, DTM_TOTAL_OUT_CELL_CNT_P_Nf);
        }

        

    

    
        for (fe13_offset = 0; fe13_offset <= 1; fe13_offset++)
        {
            if (fe13_offset == 0)
            {   
                current_dtl_blocks_info = &fe_counters_info->dtl_fe1_counters_info[pipe_idx];
                block_idx_start = dnxf_data_device.blocks.nof_instances_dcml_get(unit)/2;
                block_idx_end = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            } else {
                
                current_dtl_blocks_info = &fe_counters_info->dtl_fe3_counters_info[pipe_idx];
                block_idx_start = 0;
                block_idx_end = dnxf_data_device.blocks.nof_instances_dcml_get(unit)/2;
            }

            max_fifo_cells_nlr = 0;
            max_fifo_cells_lr = 0;
            for (block_idx = block_idx_start; block_idx < block_idx_end; block_idx++)
            {
                
                
                SHR_IF_ERR_EXIT(READ_LCM_DTL_TOTAL_IN_CELL_P_CNTr(unit, block_idx , pipe_idx, &reg_val));
                current_dtl_blocks_info->dtl_total_in_nlr += soc_reg_field_get(unit, LCM_DTL_TOTAL_IN_CELL_P_CNTr, reg_val, DTL_TOTAL_IN_CELL_P_N_CNTf);
                
                SHR_IF_ERR_EXIT(READ_LCM_DTL_LCLRT_TOTAL_IN_CELL_P_CNTr(unit, block_idx , pipe_idx, &reg_val));
                current_dtl_blocks_info->dtl_total_in_lr += soc_reg_field_get(unit, LCM_DTL_LCLRT_TOTAL_IN_CELL_P_CNTr, reg_val, DTL_LCLRT_TOTAL_IN_CELL_P_N_CNTf);

                

                
                for (priority_idx = 0 ; priority_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); priority_idx++)
                {
                    
                    SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtl_total_dropped_nlr_cells_per_pipe_reg_list[pipe_idx], block_idx, priority_idx, &reg_val));
                    current_dtl_blocks_info->dtl_total_dropped_nlr_cells[priority_idx] += soc_reg_field_get(unit, dtl_total_dropped_nlr_cells_per_pipe_reg_list[pipe_idx], reg_val, dtl_total_dropped_nlr_cells_per_pipe_field_list[pipe_idx]);

                    
                    SHR_IF_ERR_EXIT(soc_reg32_get(unit, dtl_total_dropped_lr_cells_per_pipe_reg_list[pipe_idx], block_idx, priority_idx, &reg_val));
                    current_dtl_blocks_info->dtl_total_dropped_lr_cells[priority_idx] += soc_reg_field_get(unit, dtl_total_dropped_lr_cells_per_pipe_reg_list[pipe_idx], reg_val, dtl_total_dropped_lr_cells_per_pipe_field_list[pipe_idx]);
                }
                

                
                temp_max_fifo_cells_cnt = 0;
                SHR_IF_ERR_EXIT(READ_LCM_DTL_FRAG_MAX_LEVEL_NLR_Pr(unit, block_idx , pipe_idx, &reg_val));
                temp_max_fifo_cells_cnt = soc_reg_field_get(unit, LCM_DTL_FRAG_MAX_LEVEL_NLR_Pr, reg_val,  DTL_FRAG_MAX_LEVEL_NLR_P_Nf);

                max_fifo_cells_nlr = temp_max_fifo_cells_cnt > max_fifo_cells_nlr  ? temp_max_fifo_cells_cnt : max_fifo_cells_nlr;

                temp_max_fifo_cells_cnt = 0;
                SHR_IF_ERR_EXIT(READ_LCM_DTL_FRAG_MAX_LEVEL_LR_Pr(unit, block_idx , pipe_idx, &reg_val));
                temp_max_fifo_cells_cnt = soc_reg_field_get(unit, LCM_DTL_FRAG_MAX_LEVEL_LR_Pr, reg_val, DTL_FRAG_MAX_LEVEL_LR_P_Nf);

                max_fifo_cells_lr = temp_max_fifo_cells_cnt > max_fifo_cells_lr  ? temp_max_fifo_cells_cnt : max_fifo_cells_lr;

                

                
                SHR_IF_ERR_EXIT(READ_DCML_TOTAL_OUT_CELL_P_CNTr(unit, block_idx , pipe_idx, &reg_val));
                current_dtl_blocks_info->dtl_total_out += soc_reg_field_get(unit, DCML_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNTf);

                
            }
            current_dtl_blocks_info->dtl_max_fifo_cells_nlr = max_fifo_cells_nlr;
            current_dtl_blocks_info->dtl_max_fifo_cells_lr = max_fifo_cells_lr;
        }
    
    }


exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_queues_get_info(int unit, soc_dnxf_queues_info_t* fe_queues_info)
{
    int nof_pipes, pipe_idx, block_idx, dfl_bank_idx, link_group_idx, dch_fifo_idx, correct_dch_fifo_idx, sub_bank_idx;
    int dch_block_link_offset, link_group_offset;
    uint64 reg_val64;
    uint32 reg_val, nof_links_in_dch_link_group;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 nof_fifo;
    soc_dnxf_queues_dch_link_group_t* current_dch_link_group;
    soc_dnxf_queues_dtl_info_t*       current_dtl_block;

    soc_reg_t   dch_per_pipe_reg_list[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {DCH_IFM_MAX_OCCUP_P_0r,  DCH_IFM_MAX_OCCUP_P_1r, DCH_IFM_MAX_OCCUP_P_2r};

    soc_field_t dch_stat_per_link_group_field_list[9] = {MAX_IFMA_STAT_P_0f,  MAX_IFMB_STAT_P_0f, MAX_IFMC_STAT_P_0f,
                                                         MAX_IFMA_STAT_P_1f,  MAX_IFMB_STAT_P_1f, MAX_IFMC_STAT_P_1f,
                                                         MAX_IFMA_STAT_P_2f,  MAX_IFMB_STAT_P_2f, MAX_IFMC_STAT_P_2f};

    soc_field_t dch_nof_link_per_link_group_field_list[9] = {MAX_IFMA_FIF_NUM_P_0f,  MAX_IFMB_FIF_NUM_P_0f, MAX_IFMC_FIF_NUM_P_0f,
                                                             MAX_IFMA_FIF_NUM_P_1f,  MAX_IFMB_FIF_NUM_P_1f, MAX_IFMC_FIF_NUM_P_1f,
                                                             MAX_IFMA_FIF_NUM_P_2f,  MAX_IFMB_FIF_NUM_P_2f, MAX_IFMC_FIF_NUM_P_2f};


    soc_field_t dtm_per_dtm_fifo_field_list[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_DTM_FIFOS] = {FIELD_0_12f, FIELD_16_28f, FIELD_32_44f, FIELD_48_60f,
                                                                                 FIELD_64_76f, FIELD_80_92f, FIELD_96_108f, FIELD_112_124f}; 

    int dtm_with_swapped_dch_fifos[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_DTM_FIFOS] = {0, 0, 1, 1, 1, 1, 0, 0};
    int dtm_swapped_dch_fifo_order[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_DTM_FIFOS] = {2, 3, 0, 1, 6, 7, 4, 5};

    soc_field_t dfl_per_dfl_sub_bank__field_list[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_DFL_SUB_BANKS] = {BANK_N_SUB_BANK_0_FILL_MIN_WTRKf, BANK_N_SUB_BANK_1_FILL_MIN_WTRKf};

    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    fe_queues_info->nof_pipes = nof_pipes;
    
    
    nof_links_in_dch_link_group = dnxf_data_device.blocks.nof_links_in_dch_get(unit) / dnxf_data_device.blocks.nof_dch_link_groups_get(unit);

    
    for (pipe_idx = 0;  pipe_idx < nof_pipes ; pipe_idx++)
    {
        for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); block_idx++)
        {
            dch_block_link_offset = block_idx * dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
            
            SHR_IF_ERR_EXIT(soc_reg_get(unit, dch_per_pipe_reg_list[pipe_idx], block_idx, 0, &reg_val64));
            for (link_group_idx = 0; link_group_idx < dnxf_data_device.blocks.nof_dch_link_groups_get(unit); link_group_idx++)
            {
                current_dch_link_group = &fe_queues_info->dch_queues_info[pipe_idx].nof_dch[block_idx].dch_link_group[link_group_idx];
                link_group_offset = link_group_idx * nof_links_in_dch_link_group;

                current_dch_link_group->max_occupancy_value = soc_reg64_field32_get(unit, dch_per_pipe_reg_list[pipe_idx], reg_val64, dch_stat_per_link_group_field_list[pipe_idx*3 + link_group_idx]);
                nof_fifo = soc_reg64_field32_get(unit, dch_per_pipe_reg_list[pipe_idx], reg_val64, dch_nof_link_per_link_group_field_list[pipe_idx*3 + link_group_idx]);
                current_dch_link_group->most_occupied_link =    dch_block_link_offset + link_group_offset + nof_fifo;
                current_dch_link_group->link_group_first_link = dch_block_link_offset + link_group_offset;
                current_dch_link_group->link_group_last_link =  dch_block_link_offset + link_group_offset + nof_links_in_dch_link_group - 1;
            }
        }
    }
    

    
    
    for (pipe_idx = 0;  pipe_idx < nof_pipes ; pipe_idx++)
    {
        for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
        {
            SHR_IF_ERR_EXIT(READ_LCM_LCM_REGISTER_9r(unit, block_idx, pipe_idx, reg_above_64_val));
            
            for (dch_fifo_idx = 0; dch_fifo_idx < dnxf_data_device.blocks.nof_dtm_fifos_get(unit); dch_fifo_idx++)
            {
                
                
                if (dtm_with_swapped_dch_fifos[block_idx])
                {
                    
                    correct_dch_fifo_idx = dtm_swapped_dch_fifo_order[dch_fifo_idx];
                } else
                {
                    
                    correct_dch_fifo_idx = dch_fifo_idx;
                }
                fe_queues_info->dtm_queues_info[pipe_idx].nof_dtm[block_idx].fifo_max_occupancy_value[dch_fifo_idx] = soc_reg_above_64_field32_get(unit, LCM_LCM_REGISTER_9r, reg_above_64_val, dtm_per_dtm_fifo_field_list[correct_dch_fifo_idx]);
            }
        }
    }
    

     
    
    for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
    {
        for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
        {
            SHR_IF_ERR_EXIT(READ_LCM_DTL_FRAG_MAX_LEVEL_NLR_Pr(unit, block_idx, pipe_idx, &reg_val));
            current_dtl_block = &fe_queues_info->dtl_queues_info_nlr[pipe_idx].nof_dtl[block_idx];

            current_dtl_block->max_occupancy_value = soc_reg_field_get(unit, LCM_DTL_FRAG_MAX_LEVEL_NLR_Pr, reg_val, DTL_FRAG_MAX_LEVEL_NLR_P_Nf);
            nof_fifo = soc_reg_field_get(unit, LCM_DTL_FRAG_MAX_LEVEL_NLR_Pr, reg_val, DTL_FRAG_MAX_LEVEL_NLR_NUM_P_Nf);;
            current_dtl_block->most_occupied_link = block_idx*dnxf_data_device.blocks.nof_links_in_dcq_get(unit) + nof_fifo;
        }
    }

    
    for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
    {
        for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
        {
            SHR_IF_ERR_EXIT(READ_LCM_DTL_FRAG_MAX_LEVEL_LR_Pr(unit, block_idx, pipe_idx, &reg_val));
            current_dtl_block = &fe_queues_info->dtl_queues_info_lr[pipe_idx].nof_dtl[block_idx];

            current_dtl_block->max_occupancy_value = soc_reg_field_get(unit, LCM_DTL_FRAG_MAX_LEVEL_LR_Pr, reg_val, DTL_FRAG_MAX_LEVEL_LR_P_Nf);
            nof_fifo = soc_reg_field_get(unit, LCM_DTL_FRAG_MAX_LEVEL_LR_Pr, reg_val, DTL_FRAG_MAX_LEVEL_LR_NUM_P_Nf);;
            current_dtl_block->most_occupied_link = block_idx*dnxf_data_device.blocks.nof_links_in_dcq_get(unit) + nof_fifo;
        }
    }
    

    
    for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++) {
        for (dfl_bank_idx = 0; dfl_bank_idx < dnxf_data_fabric.congestion.nof_dfl_banks_get(unit); dfl_bank_idx++) {
            SHR_IF_ERR_EXIT(READ_DCML_FBC_BANK_STATr(unit, block_idx, dfl_bank_idx, reg_above_64_val));
            for (sub_bank_idx = 0; sub_bank_idx < dnxf_data_fabric.congestion.nof_dfl_sub_banks_get(unit); sub_bank_idx++)
            {
                fe_queues_info->dfl_queues_info[block_idx].nof_dfl[dfl_bank_idx].sub_bank_min_free_entries_value[sub_bank_idx] = soc_reg_above_64_field32_get(unit, DCML_FBC_BANK_STATr, reg_above_64_val, dfl_per_dfl_sub_bank__field_list[sub_bank_idx]);
            }
        }
    }
    

exit:
    SHR_FUNC_EXIT;
}

int
soc_ramon_diag_mesh_topology_get(int unit, soc_dnxc_fabric_mesh_topology_diag_t *mesh_topology_diag)
{
    soc_reg_t reg_list[] = {MESH_TOPOLOGY_REG_1A0r,  MESH_TOPOLOGY_REG_01A1r, MESH_TOPOLOGY_REG_01A2r, MESH_TOPOLOGY_REG_01A3r, MESH_TOPOLOGY_REG_01A4r, MESH_TOPOLOGY_REG_01A5r, MESH_TOPOLOGY_REG_01A6r, MESH_TOPOLOGY_REG_01A7r, MESH_TOPOLOGY_REG_01A8r, MESH_TOPOLOGY_REG_01A9r, MESH_TOPOLOGY_REG_01AAr, MESH_TOPOLOGY_REG_01ABr, MESH_TOPOLOGY_REG_01ACr, MESH_TOPOLOGY_REG_01ADr, MESH_TOPOLOGY_REG_01AEr, MESH_TOPOLOGY_REG_01AFr,
                            MESH_TOPOLOGY_REG_01B0r, MESH_TOPOLOGY_REG_01B1r, MESH_TOPOLOGY_REG_01B2r, MESH_TOPOLOGY_REG_01B3r, MESH_TOPOLOGY_REG_01B4r, MESH_TOPOLOGY_REG_01B5r, MESH_TOPOLOGY_REG_01B6r, MESH_TOPOLOGY_REG_01B7r, MESH_TOPOLOGY_REG_01B8r, MESH_TOPOLOGY_REG_01B9r, MESH_TOPOLOGY_REG_01BAr, MESH_TOPOLOGY_REG_01BBr, MESH_TOPOLOGY_REG_01BCr, MESH_TOPOLOGY_REG_01BDr, MESH_TOPOLOGY_REG_01BEr, MESH_TOPOLOGY_REG_01BFr,
                            MESH_TOPOLOGY_REG_01C0r, MESH_TOPOLOGY_REG_01C1r, MESH_TOPOLOGY_REG_01C2r, MESH_TOPOLOGY_REG_01C3r, MESH_TOPOLOGY_REG_01C4r, MESH_TOPOLOGY_REG_01C5r, MESH_TOPOLOGY_REG_01C6r, MESH_TOPOLOGY_REG_01C7r, MESH_TOPOLOGY_REG_01C8r, MESH_TOPOLOGY_REG_01C9r, MESH_TOPOLOGY_REG_01CAr, MESH_TOPOLOGY_REG_01CBr, MESH_TOPOLOGY_REG_01CCr, MESH_TOPOLOGY_REG_01CDr, MESH_TOPOLOGY_REG_01CEr, MESH_TOPOLOGY_REG_01CFr};

    uint32 reg32_val;
    uint64 reg64_val;
    int i, valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_0000r(unit, &reg32_val));
    mesh_topology_diag->indication_1 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_0000r, reg32_val, FIELD_1_1f);
    mesh_topology_diag->indication_2 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_0000r, reg32_val, FIELD_2_2f);

    for (i = 0; i < sizeof(reg_list) / sizeof(soc_reg_t); i++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg_list[i], REG_PORT_ANY, 0, &reg32_val));
        valid = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_15_15f);
        if (valid) {
            mesh_topology_diag->type2_list[i*2] = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_0_14f);
        } else {
            mesh_topology_diag->type2_list[i*2] = -1;
        }

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg_list[i], REG_PORT_ANY, 0, &reg32_val));
        valid = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_31_31f);
        if (valid) {
            mesh_topology_diag->type2_list[i*2 + 1] = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_16_30f);
        } else {
            mesh_topology_diag->type2_list[i*2 + 1] = -1;
        }
    }
    mesh_topology_diag->link_list_count = i*2;

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr(unit, &reg32_val));
    mesh_topology_diag->rx_control_cells_type1 = soc_reg_field_get(unit, MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr, reg32_val, RCV_CTL_1f);
    mesh_topology_diag->rx_control_cells_type2 = soc_reg_field_get(unit, MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr, reg32_val, RCV_CTL_2f);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01D1r(unit, &reg32_val));
    mesh_topology_diag->indication_3 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_01D1r, reg32_val, FIELD_4_4f);
    mesh_topology_diag->indication_4 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_01D1r, reg32_val, FIELD_5_5f);
    mesh_topology_diag->control_cells_type3 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_01D1r, reg32_val, FIELD_7_20f);

    
    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01D4r(unit, &reg64_val));
    mesh_topology_diag->status_1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01D4r, reg64_val, FIELD_0_14f);
    mesh_topology_diag->status_1_id1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01D4r, reg64_val, FIELD_15_22f);
    mesh_topology_diag->status_1_id2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01D4r, reg64_val, FIELD_23_33f);

    
    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01D5r(unit, &reg64_val));
    mesh_topology_diag->status_2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01D5r, reg64_val, FIELD_0_14f);
    mesh_topology_diag->status_2_id1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01D5r, reg64_val, FIELD_15_22f);
    mesh_topology_diag->status_2_id2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01D5r, reg64_val, FIELD_23_33f);

exit:
    SHR_FUNC_EXIT;
}

const soc_reg_t temp_regs[] = {
    ECI_PVT_MON_A_THERMAL_DATAr,
    ECI_PVT_MON_B_THERMAL_DATAr,
    ECI_PVT_MON_C_THERMAL_DATAr,
    ECI_PVT_MON_D_THERMAL_DATAr
};
const soc_field_t curr_temp_fields[] = {
     THERMAL_DATA_Af,
     THERMAL_DATA_Bf,
     THERMAL_DATA_Cf,
     THERMAL_DATA_Df };
const soc_field_t peak_temp_fields[] ={
     PEAK_THERMAL_DATA_Af,
     PEAK_THERMAL_DATA_Bf,
     PEAK_THERMAL_DATA_Cf,
     PEAK_THERMAL_DATA_Df };
int 
soc_ramon_temperature_monitor_regs_get(int unit, const soc_reg_t **temp_reg_ptr, const soc_field_t **currr_temp_field_ptr, const soc_field_t **peak_temp_field_ptr)
{

    SHR_FUNC_INIT_VARS(unit);

    *temp_reg_ptr = temp_regs;
    *currr_temp_field_ptr = curr_temp_fields;
    *peak_temp_field_ptr = peak_temp_fields;

    SHR_FUNC_EXIT;
}
