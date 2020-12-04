/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 DIAG C
 */
 
#include <shared/bsl.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h> 
#include <soc/dfe/cmn/dfe_diag.h>
 
#include <soc/dfe/fe3200/fe3200_diag.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG

const soc_dfe_diag_flag_str_t soc_fe3200_diag_flag_str_interrupts_names[] = 
{
    {0, "ECI_GLOBAL"},
    {1, "OCCG"},
    {2, "DCH0"},
    {3, "DCH1"},
    {4, "DCH2"},
    {5, "DCH3"},
    {6, "DCL0"},
    {7, ""},
    {8, "DCL1"},
    {9, "DCL2"},
    {10, "DCL3"},
    {11, "DCM0"},
    {12, "DCM1"},
    {13, "DCM2"},
    {14, "DCM3"},
    {15, "DCMC"},
    {16, "CCS0"},
    {17, "CCS1"},
    {18, "CCS2"},
    {19, "CCS3"},
    {20, "RTP"},
    {21, "MESH_TOPOLOGY"},
    {22, "FMAC0"},
    {23, "FMAC1"},
    {24, "FMAC2"},
    {25, "FMAC3"},
    {26, "FMAC4"},
    {27, "FMAC5"},
    {28, "FMAC6"},
    {29, "FMAC7"},
    {30, "FMAC8"},
    {31, "FMAC9"},
    {32, "FMAC10"},
    {33, "FMAC11"},
    {34, "FMAC12"},
    {35, "FMAC13"},
    {36, "FMAC14"},
    {37, "FMAC15"},
    {38, "FMAC16"},
    {39, "FMAC17"},
    {40, "FMAC18"},
    {41, "FMAC19"},
    {42, "FMAC20"},
    {43, "FMAC21"},
    {44, "FMAC22"},
    {45, "FMAC23"},
    {46, "FMAC24"},
    {47, "FMAC25"},
    {48, "FMAC26"},
    {49, "FMAC27"},
    {50, "FMAC28"},
    {51, "FMAC29"},
    {52, "FMAC30"},
    {53, "FMAC31"},
    {54, "FMAC32"},
    {55, "FMAC33"},
    {56, "FMAC34"},
    {57, "FMAC35"},
    {58, "FSRD0"},
    {59, "FSRD1"},
    {60, "FSRD2"},
    {61, "FSRD3"},
    {62, "FSRD4"},
    {63, "FSRD5"},
    {64, "FSRD6"},
    {65, "FSRD7"},
    {66, "FSRD8"},
    {67, "FSRD9"},
    {68, "FSRD10"},
    {69, "FSRD11"},
    {-1,  NULL}
};

soc_error_t
soc_fe3200_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dfe_diag_flag_str_t **intr_names)
{
    SOCDNX_INIT_FUNC_DEFS;

    *intr_names = soc_fe3200_diag_flag_str_interrupts_names;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_diag_cell_pipe_counter_get(int unit, int pipe, uint64 * counter)
{
    int i;
    uint32 reg_val32;
    uint64 sum;

    SOCDNX_INIT_FUNC_DEFS

    COMPILER_64_SET(sum, 0, 0);

    switch (pipe) {
    case 0: 
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_0r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_0r, reg_val32, DCH_TOTAL_CELL_CNT_P_0f));
        }
        break;

    case 1: 
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_1r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_1r, reg_val32, DCH_TOTAL_CELL_CNT_P_1f));
        }
        break;

    case 2: 
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_2r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_2r, reg_val32, DCH_TOTAL_CELL_CNT_P_2f));
        }
        break;

    case -1: 
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_0r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_0r, reg_val32, DCH_TOTAL_CELL_CNT_P_0f));

            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_1r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_1r, reg_val32, DCH_TOTAL_CELL_CNT_P_1f));

            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_2r(unit, i, &reg_val32));
            COMPILER_64_ADD_32(sum, soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_2r, reg_val32, DCH_TOTAL_CELL_CNT_P_2f));
        }
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pipe %d does not exist\n"), pipe));
    }

    COMPILER_64_SET(*counter, COMPILER_64_HI(sum), COMPILER_64_LO(sum));

exit:
    SOCDNX_FUNC_RETURN
}

soc_error_t 
soc_fe3200_counters_get_info(int unit,soc_dfe_counters_info_t* fe_counters_info)
{
    int nof_pipes, curr_max =0, temp, i , j , k;
    uint32 reg_val;
    uint64 reg_val64;

    SOCDNX_INIT_FUNC_DEFS

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    fe_counters_info -> nof_pipes = nof_pipes;


    

    
    curr_max = 0;

    for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_0r(unit, i , &reg_val));
        fe_counters_info->dch_counters_info[0].dch_total_in.value += soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_0r, reg_val, DCH_TOTAL_CELL_CNT_P_0f);
        fe_counters_info->dch_counters_info[0].dch_total_in.overflow += soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_0r, reg_val, DCH_TOTAL_CELL_CNT_OP_0f);

        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_OUT_CELL_CNT_P_0r(unit, i , &reg_val));
        fe_counters_info->dch_counters_info[0].dch_total_out.value += soc_reg_field_get(unit, DCH_DCH_TOTAL_OUT_CELL_CNT_P_0r, reg_val, DCH_TOTAL_OUT_CELL_CNT_P_0f);
        fe_counters_info->dch_counters_info[0].dch_total_out.overflow += soc_reg_field_get(unit, DCH_DCH_TOTAL_OUT_CELL_CNT_P_0r, reg_val, DCH_TOTAL_OUT_CELL_CNT_OP_0f);
    
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, i, 0, &reg_val));
        fe_counters_info->dch_counters_info[0].dch_fifo_discard.value += soc_reg_field_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, reg_val, FIFO_DISCARD_CNT_P_Nf);
        fe_counters_info->dch_counters_info[0].dch_fifo_discard.overflow += soc_reg_field_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, reg_val, FIFO_DISCARD_CNT_O_P_Nf);

        SOCDNX_IF_ERR_EXIT(READ_DCH_UN_REACH_DESTr(unit, i, &reg_val64));
        fe_counters_info->dch_counters_info[0].sum_dch_unreach_discard.value += soc_reg64_field32_get(unit, DCH_UN_REACH_DESTr, reg_val64, UNREACH_DEST_CNTf);
        fe_counters_info->dch_counters_info[0].sum_dch_unreach_discard.overflow += soc_reg64_field32_get(unit, DCH_UN_REACH_DESTr, reg_val64, UNREACH_DEST_CNT_Of);

        SOCDNX_IF_ERR_EXIT(READ_DCH_IFM_MAX_OCCUP_P_0r(unit, i, &reg_val64));
        temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMA_STAT_P_0f);
        curr_max = (temp>curr_max)? temp:curr_max;
        temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMB_STAT_P_0f);
        curr_max = (temp>curr_max)? temp:curr_max;
        temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMC_STAT_P_0f);
        curr_max = (temp>curr_max)? temp:curr_max;
        temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMD_STAT_P_0f);
        curr_max = (temp>curr_max)? temp:curr_max;
    }
    
    fe_counters_info->dch_counters_info[0].dch_max_q_size = curr_max;
    
    
    if (nof_pipes >= 2)
    {
        curr_max = 0;

        for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_1r(unit, i, &reg_val));
            fe_counters_info->dch_counters_info[1].dch_total_in.value += soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_1r, reg_val, DCH_TOTAL_CELL_CNT_P_1f);
            fe_counters_info->dch_counters_info[1].dch_total_in.overflow += soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_1r, reg_val, DCH_TOTAL_CELL_CNT_OP_1f);

            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_OUT_CELL_CNT_P_1r(unit, i , &reg_val));
            fe_counters_info->dch_counters_info[1].dch_total_out.value += soc_reg_field_get(unit, DCH_DCH_TOTAL_OUT_CELL_CNT_P_1r, reg_val, DCH_TOTAL_OUT_CELL_CNT_P_1f);
            fe_counters_info->dch_counters_info[1].dch_total_out.overflow += soc_reg_field_get(unit, DCH_DCH_TOTAL_OUT_CELL_CNT_P_1r, reg_val, DCH_TOTAL_OUT_CELL_CNT_OP_1f);

            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, i, 1, &reg_val));
            fe_counters_info->dch_counters_info[1].dch_fifo_discard.value += soc_reg_field_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, reg_val, FIFO_DISCARD_CNT_P_Nf);
            fe_counters_info->dch_counters_info[1].dch_fifo_discard.overflow += soc_reg_field_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, reg_val, FIFO_DISCARD_CNT_O_P_Nf);

            SOCDNX_IF_ERR_EXIT(READ_DCH_UN_REACH_DESTr(unit, i, &reg_val64));
            fe_counters_info->dch_counters_info[1].sum_dch_unreach_discard.value += soc_reg64_field32_get(unit, DCH_UN_REACH_DESTr, reg_val64, UNREACH_DEST_CNTf);
            fe_counters_info->dch_counters_info[1].sum_dch_unreach_discard.overflow += soc_reg64_field32_get(unit, DCH_UN_REACH_DESTr, reg_val64, UNREACH_DEST_CNT_Of);

            SOCDNX_IF_ERR_EXIT(READ_DCH_IFM_MAX_OCCUP_P_1r(unit, i, &reg_val64));
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMA_STAT_P_1f);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMB_STAT_P_1f);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMC_STAT_P_1f);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMD_STAT_P_1f);
            curr_max = (temp>curr_max)? temp:curr_max;

        }
        fe_counters_info->dch_counters_info[1].dch_max_q_size = curr_max;
    }

    
    if (nof_pipes == 3)
    {
        curr_max = 0;
        for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++ )
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_CELL_CNT_P_2r(unit, i, &reg_val));
            fe_counters_info->dch_counters_info[2].dch_total_in.value += soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_2r, reg_val, DCH_TOTAL_CELL_CNT_P_2f);
            fe_counters_info->dch_counters_info[2].dch_total_in.overflow += soc_reg_field_get(unit, DCH_DCH_TOTAL_CELL_CNT_P_2r, reg_val, DCH_TOTAL_CELL_CNT_OP_2f);

            SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_TOTAL_OUT_CELL_CNT_P_2r(unit, i , &reg_val));
            fe_counters_info->dch_counters_info[2].dch_total_out.value += soc_reg_field_get(unit, DCH_DCH_TOTAL_OUT_CELL_CNT_P_2r, reg_val, DCH_TOTAL_OUT_CELL_CNT_P_2f);
            fe_counters_info->dch_counters_info[2].dch_total_out.overflow += soc_reg_field_get(unit, DCH_DCH_TOTAL_OUT_CELL_CNT_P_2r, reg_val, DCH_TOTAL_OUT_CELL_CNT_OP_2f);

            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, i, 2, &reg_val));
            fe_counters_info->dch_counters_info[2].dch_fifo_discard.value += soc_reg_field_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, reg_val, FIFO_DISCARD_CNT_P_Nf);
            fe_counters_info->dch_counters_info[2].dch_fifo_discard.overflow += soc_reg_field_get(unit, DCH_FIFO_DISCARD_COUNTER_Pr, reg_val, FIFO_DISCARD_CNT_O_P_Nf);

            SOCDNX_IF_ERR_EXIT(READ_DCH_UN_REACH_DESTr(unit, i, &reg_val64));
            fe_counters_info->dch_counters_info[2].sum_dch_unreach_discard.value += soc_reg64_field32_get(unit, DCH_UN_REACH_DESTr, reg_val64, UNREACH_DEST_CNTf);
            fe_counters_info->dch_counters_info[2].sum_dch_unreach_discard.overflow += soc_reg64_field32_get(unit, DCH_UN_REACH_DESTr, reg_val64, UNREACH_DEST_CNT_Of);

            SOCDNX_IF_ERR_EXIT(READ_DCH_IFM_MAX_OCCUP_P_2r(unit, i, &reg_val64));
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMA_STAT_P_2f);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMB_STAT_P_2f);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMC_STAT_P_2f);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMD_STAT_P_2f);
            curr_max = (temp>curr_max)? temp:curr_max;
        }
        fe_counters_info->dch_counters_info[2].dch_max_q_size = curr_max;
    }

    

    for (j = 0 ; j < nof_pipes ; j ++)
    {
        curr_max = 0;
        for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dcm); i++ )
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_TOTAL_IN_CELL_CNT_PABr(unit, i, j, &reg_val));
            fe_counters_info->dcm_counters_info[j].dcm_tot.value += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PABr, reg_val, DCHA_TOTAL_IN_CELL_CNT_P_Nf);
            fe_counters_info->dcm_counters_info[j].dcm_tot.overflow += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PABr, reg_val, DCHA_TOTAL_IN_CELL_CNT_P_NOf);
            fe_counters_info->dcm_counters_info[j].dcm_tot.value += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PABr, reg_val, DCHB_TOTAL_IN_CELL_CNT_P_Nf);
            fe_counters_info->dcm_counters_info[j].dcm_tot.overflow += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PABr, reg_val, DCHB_TOTAL_IN_CELL_CNT_P_NOf);

            SOCDNX_IF_ERR_EXIT(READ_DCM_TOTAL_IN_CELL_CNT_PCDr(unit, i, j, &reg_val));
            fe_counters_info->dcm_counters_info[j].dcm_tot.value += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PCDr, reg_val, DCHC_TOTAL_IN_CELL_CNT_P_Nf);
            fe_counters_info->dcm_counters_info[j].dcm_tot.overflow += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PCDr, reg_val, DCHC_TOTAL_IN_CELL_CNT_P_NOf);
            fe_counters_info->dcm_counters_info[j].dcm_tot.value += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PCDr, reg_val, DCHD_TOTAL_IN_CELL_CNT_P_Nf);
            fe_counters_info->dcm_counters_info[j].dcm_tot.overflow += soc_reg_field_get(unit, DCM_TOTAL_IN_CELL_CNT_PCDr, reg_val, DCHD_TOTAL_IN_CELL_CNT_P_NOf);

            SOCDNX_IF_ERR_EXIT(READ_DCM_DROP_TOTAL_P_CNTr(unit, i, j, &reg_val));
            fe_counters_info->dcm_counters_info[j].dcm_drop.value += soc_reg_field_get(unit, DCM_DROP_TOTAL_P_CNTr, reg_val, DROP_TOTAL_P_N_CNTf);
            fe_counters_info->dcm_counters_info[j].dcm_drop.overflow += soc_reg_field_get(unit, DCM_DROP_TOTAL_P_CNTr, reg_val, DROP_TOTAL_P_N_CNT_Of);

            for (k = 0; k < soc_dfe_fabric_priority_nof; k++) {
                switch (j) {
                case 0:
                    SOCDNX_IF_ERR_EXIT(READ_DCM_DROP_IP_PRI_P_0_CNTr(unit, i, k, &reg_val));
                    fe_counters_info->dcm_counters_info[j].dcm_drop.value += soc_reg_field_get(unit, DCM_DROP_IP_PRI_P_0_CNTr, reg_val, DROP_IP_PRI_NP_0_CNTf);
                    fe_counters_info->dcm_counters_info[j].dcm_drop.overflow += soc_reg_field_get(unit, DCM_DROP_IP_PRI_P_0_CNTr, reg_val, DROP_IP_PRI_NP_0_CNT_Of);
                    break;
                case 1:
                    SOCDNX_IF_ERR_EXIT(READ_DCM_DROP_IP_PRI_P_1_CNTr(unit, i, k, &reg_val));
                    fe_counters_info->dcm_counters_info[j].dcm_drop.value += soc_reg_field_get(unit, DCM_DROP_IP_PRI_P_1_CNTr, reg_val, DROP_IP_PRI_NP_1_CNTf);
                    fe_counters_info->dcm_counters_info[j].dcm_drop.overflow += soc_reg_field_get(unit, DCM_DROP_IP_PRI_P_1_CNTr, reg_val, DROP_IP_PRI_NP_1_CNT_Of);
                    break;
                case 2:
                    SOCDNX_IF_ERR_EXIT(READ_DCM_DROP_IP_PRI_P_2_CNTr(unit, i, k, &reg_val));
                    fe_counters_info->dcm_counters_info[j].dcm_drop.value += soc_reg_field_get(unit, DCM_DROP_IP_PRI_P_2_CNTr, reg_val, DROP_IP_PRI_NP_2_CNTf);
                    fe_counters_info->dcm_counters_info[j].dcm_drop.overflow += soc_reg_field_get(unit, DCM_DROP_IP_PRI_P_2_CNTr, reg_val, DROP_IP_PRI_NP_2_CNT_Of);
                    break;
                }
            }

            SOCDNX_IF_ERR_EXIT(READ_DCM_MC_LP_CELL_DRP_CNT_Pr(unit, i, j, &reg_val));
            fe_counters_info->dcm_counters_info[j].dcm_drop.value += soc_reg_field_get(unit, DCM_MC_LP_CELL_DRP_CNT_Pr, reg_val, MC_LP_CELL_DRP_CNT_P_Nf);
            fe_counters_info->dcm_counters_info[j].dcm_drop.overflow += soc_reg_field_get(unit, DCM_MC_LP_CELL_DRP_CNT_Pr, reg_val, MC_LP_CELL_DRP_CNT_P_NOf);

            SOCDNX_IF_ERR_EXIT(READ_DCM_DCM_MAX_OCUP_Pr(unit, i , j, &reg_val64));
            temp = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHA_DCM_MAX_OCUP_P_Nf);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHB_DCM_MAX_OCUP_P_Nf);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHC_DCM_MAX_OCUP_P_Nf);
            curr_max = (temp>curr_max)? temp:curr_max;
            temp = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHD_DCM_MAX_OCUP_P_Nf);
            curr_max = (temp>curr_max)? temp:curr_max;
        }
        fe_counters_info->dcm_counters_info[j].dcm_max = curr_max;
    }

    
    curr_max = 0;

    for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dcl) ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_TOTAL_IN_CELL_P_CNTr(unit, i , 0, &reg_val));
        fe_counters_info->dcl_counters_info[0].dcl_tot_in.value += soc_reg_field_get(unit, DCL_TOTAL_IN_CELL_P_CNTr, reg_val, TOTAL_IN_CELL_P_N_CNTf);
        fe_counters_info->dcl_counters_info[0].dcl_tot_in.overflow += soc_reg_field_get(unit, DCL_TOTAL_IN_CELL_P_CNTr, reg_val, TOTAL_IN_CELL_P_N_CNT_Of);

        SOCDNX_IF_ERR_EXIT(READ_DCL_TOTAL_OUT_CELL_P_CNTr(unit, i, 0, &reg_val));
        fe_counters_info->dcl_counters_info[0].dcl_tot_out.value += soc_reg_field_get(unit, DCL_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNTf);
        fe_counters_info->dcl_counters_info[0].dcl_tot_out.overflow += soc_reg_field_get(unit, DCL_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNT_Of);

        for (j = 0; j < 4; j++) 
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_DROPPED_IP_PR_P_0_CNTr(unit, i , j, &reg_val));
            fe_counters_info->dcl_counters_info[0].dcl_dropped.value += soc_reg_field_get(unit, DCL_DROPPED_IP_PR_P_0_CNTr, reg_val, DROPPED_IP_PR_NP_0_CNTf);
            fe_counters_info->dcl_counters_info[0].dcl_dropped.overflow += soc_reg_field_get(unit, DCL_DROPPED_IP_PR_P_0_CNTr, reg_val, DROPPED_IP_PR_NP_0_CNT_Of);

        }

        SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCCUPANCY_FIFO_Pr(unit, i, 0, &reg_val));
        temp = soc_reg_field_get(unit, DCL_MAX_OCCUPANCY_FIFO_Pr, reg_val, MAX_OCCUP_P_Nf);
        curr_max = (temp>curr_max)? temp:curr_max;
    }
    fe_counters_info->dcl_counters_info[0].dcl_max = curr_max;

    
    if (nof_pipes >= 2)
    {
        curr_max = 0;

        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); i++ )
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TOTAL_IN_CELL_P_CNTr(unit, i, 1, &reg_val));
            fe_counters_info->dcl_counters_info[1].dcl_tot_in.value += soc_reg_field_get(unit, DCL_TOTAL_IN_CELL_P_CNTr, reg_val, TOTAL_IN_CELL_P_N_CNTf);
            fe_counters_info->dcl_counters_info[1].dcl_tot_in.overflow += soc_reg_field_get(unit, DCL_TOTAL_IN_CELL_P_CNTr, reg_val, TOTAL_IN_CELL_P_N_CNT_Of);

            SOCDNX_IF_ERR_EXIT(READ_DCL_TOTAL_OUT_CELL_P_CNTr(unit, i, 1, &reg_val));
            fe_counters_info->dcl_counters_info[1].dcl_tot_out.value += soc_reg_field_get(unit, DCL_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNTf);
            fe_counters_info->dcl_counters_info[1].dcl_tot_out.overflow += soc_reg_field_get(unit, DCL_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNT_Of);

            for (j = 0 ; j < 4; j++)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_DROPPED_IP_PR_P_1_CNTr(unit, i, j, &reg_val));
                fe_counters_info->dcl_counters_info[1].dcl_dropped.value += soc_reg_field_get(unit, DCL_DROPPED_IP_PR_P_1_CNTr, reg_val, DROPPED_IP_PR_NP_1_CNTf);
                fe_counters_info->dcl_counters_info[1].dcl_dropped.overflow += soc_reg_field_get(unit, DCL_DROPPED_IP_PR_P_1_CNTr, reg_val, DROPPED_IP_PR_NP_1_CNT_Of);
            }

            SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCCUPANCY_FIFO_Pr(unit, i, 1, &reg_val));
            temp = soc_reg_field_get(unit, DCL_MAX_OCCUPANCY_FIFO_Pr, reg_val, MAX_OCCUP_P_Nf);
            curr_max = (temp>curr_max)? temp:curr_max;
        }

        fe_counters_info->dcl_counters_info[1].dcl_max = curr_max;
    }

    
    if (nof_pipes == 3)
    {
        curr_max = 0;
        
        for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); i++ )
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_TOTAL_IN_CELL_P_CNTr(unit, i, 2, &reg_val));
            fe_counters_info->dcl_counters_info[2].dcl_tot_in.value += soc_reg_field_get(unit, DCL_TOTAL_IN_CELL_P_CNTr, reg_val, TOTAL_IN_CELL_P_N_CNTf);
            fe_counters_info->dcl_counters_info[2].dcl_tot_in.overflow += soc_reg_field_get(unit, DCL_TOTAL_IN_CELL_P_CNTr, reg_val, TOTAL_IN_CELL_P_N_CNT_Of);

            SOCDNX_IF_ERR_EXIT(READ_DCL_TOTAL_OUT_CELL_P_CNTr(unit, i, 2, &reg_val));
            fe_counters_info->dcl_counters_info[2].dcl_tot_out.value += soc_reg_field_get(unit, DCL_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNTf);
            fe_counters_info->dcl_counters_info[2].dcl_tot_out.overflow += soc_reg_field_get(unit, DCL_TOTAL_OUT_CELL_P_CNTr, reg_val, TOTAL_OUT_CELL_P_N_CNT_Of);

            for (j = 0 ; j < 4 ; j++)
            {
                SOCDNX_IF_ERR_EXIT(READ_DCL_DROPPED_IP_PR_P_2_CNTr(unit, i, j, &reg_val));
                fe_counters_info->dcl_counters_info[2].dcl_dropped.value += soc_reg_field_get(unit, DCL_DROPPED_IP_PR_P_2_CNTr, reg_val, DROPPED_IP_PR_NP_2_CNTf);
                fe_counters_info->dcl_counters_info[2].dcl_dropped.overflow += soc_reg_field_get(unit, DCL_DROPPED_IP_PR_P_2_CNTr, reg_val, DROPPED_IP_PR_NP_2_CNT_Of);

            }

            SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCCUPANCY_FIFO_Pr(unit, i, 2, &reg_val));
            temp = soc_reg_field_get(unit, DCL_MAX_OCCUPANCY_FIFO_Pr, reg_val, MAX_OCCUP_P_Nf);
            curr_max = (temp>curr_max)? temp:curr_max;
        }

        fe_counters_info->dcl_counters_info[2].dcl_max = curr_max;
    }

     

    for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++)
    {        
        SOCDNX_IF_ERR_EXIT(READ_DCH_DCM_FLOW_CNTROL_0_CNT_P_0r(unit, i, &reg_val));
        fe_counters_info->dch_dcm_fc[0].value += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_0_CNT_P_0r, reg_val, DCM_FLOW_CNTROL_0_CNT_P_0f);
        fe_counters_info->dch_dcm_fc[0].overflow += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_0_CNT_P_0r, reg_val, DCM_FLOW_CNTROL_0_CNT_P_0_OVERFLOWf);

        SOCDNX_IF_ERR_EXIT(READ_DCH_DCM_FLOW_CNTROL_1_CNT_P_0r(unit, i, &reg_val));
        fe_counters_info->dch_dcm_fc[0].value += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_1_CNT_P_0r, reg_val, DCM_FLOW_CNTROL_1_CNT_P_0f);
        fe_counters_info->dch_dcm_fc[0].overflow += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_1_CNT_P_0r, reg_val, DCM_FLOW_CNTROL_1_CNT_P_0_OVERFLOWf);

    }

    

    if (nof_pipes >= 2)
    {
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCM_FLOW_CNTROL_0_CNT_P_1r(unit, i, &reg_val));
            fe_counters_info->dch_dcm_fc[1].value += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_0_CNT_P_1r, reg_val, DCM_FLOW_CNTROL_0_CNT_P_1f);
            fe_counters_info->dch_dcm_fc[1].overflow += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_0_CNT_P_1r, reg_val, DCM_FLOW_CNTROL_0_CNT_P_1_OVERFLOWf);

            SOCDNX_IF_ERR_EXIT(READ_DCH_DCM_FLOW_CNTROL_1_CNT_P_1r(unit, i, &reg_val));
            fe_counters_info->dch_dcm_fc[1].value += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_1_CNT_P_1r, reg_val, DCM_FLOW_CNTROL_1_CNT_P_1f);
            fe_counters_info->dch_dcm_fc[1].overflow += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_1_CNT_P_1r, reg_val, DCM_FLOW_CNTROL_1_CNT_P_1_OVERFLOWf);
        }
    }

    

    if (nof_pipes == 3)
    {
        for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_DCM_FLOW_CNTROL_0_CNT_P_2r(unit, i, &reg_val));
            fe_counters_info->dch_dcm_fc[2].value += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_0_CNT_P_2r, reg_val, DCM_FLOW_CNTROL_0_CNT_P_2f);
            fe_counters_info->dch_dcm_fc[2].overflow += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_0_CNT_P_2r, reg_val, DCM_FLOW_CNTROL_0_CNT_P_2_OVERFLOWf);

            SOCDNX_IF_ERR_EXIT(READ_DCH_DCM_FLOW_CNTROL_1_CNT_P_2r(unit, i, &reg_val));
            fe_counters_info->dch_dcm_fc[2].value += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_1_CNT_P_2r, reg_val, DCM_FLOW_CNTROL_1_CNT_P_2f);
            fe_counters_info->dch_dcm_fc[2].overflow += soc_reg_field_get(unit, DCH_DCM_FLOW_CNTROL_1_CNT_P_2r, reg_val, DCM_FLOW_CNTROL_1_CNT_P_2_OVERFLOWf);
        }
    }

    
    for (j = 0; j < nof_pipes ; j++)
    {
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dcm) ; i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_DCL_FLOW_CONTROL_CNT_Pr(unit, i, j, &reg_val));
            fe_counters_info->dcm_dcl_fc[j].value += soc_reg_field_get(unit, DCM_DCL_FLOW_CONTROL_CNT_Pr, reg_val, DCL_FLOW_CONTROL_CNT_P_Nf);
            fe_counters_info->dcm_dcl_fc[j].overflow += soc_reg_field_get(unit, DCM_DCL_FLOW_CONTROL_CNT_Pr, reg_val, DCL_FLOW_CONTROL_CNT_P_N_OVERFLOWf);
        }
    }

    

    for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_ccs) ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_TOTAL_CELLS_CNTr(unit, i, &reg_val));
        fe_counters_info->ccs_counters_info.ccs_total_control.value += soc_reg_field_get(unit, CCS_TOTAL_CELLS_CNTr, reg_val, TOTAL_CELLS_CNTf);
        fe_counters_info->ccs_counters_info.ccs_total_control.overflow += soc_reg_field_get(unit, CCS_TOTAL_CELLS_CNTr, reg_val, TOTAL_CELLS_CNT_OVERFLOWf);

        SOCDNX_IF_ERR_EXIT(READ_CCS_FLOW_STATUS_CELLS_CNTr(unit, i, &reg_val));
        fe_counters_info->ccs_counters_info.ccs_flow_stat.value += soc_reg_field_get(unit, CCS_FLOW_STATUS_CELLS_CNTr, reg_val, FLOW_STATUS_CELLS_CNTf);
        fe_counters_info->ccs_counters_info.ccs_flow_stat.overflow += soc_reg_field_get(unit, CCS_FLOW_STATUS_CELLS_CNTr, reg_val, FLOW_STATUS_CELLS_CNT_OVERFLOWf);

        SOCDNX_IF_ERR_EXIT(READ_CCS_CREDIT_CELLS_CNTr(unit, i, &reg_val));
        fe_counters_info->ccs_counters_info.ccs_credits.value += soc_reg_field_get(unit, CCS_CREDIT_CELLS_CNTr, reg_val, CREDIT_CELLS_CNTf);
        fe_counters_info->ccs_counters_info.ccs_credits.overflow += soc_reg_field_get(unit, CCS_CREDIT_CELLS_CNTr, reg_val, CREDIT_CELLS_CNT_OVERFLOWf);

        SOCDNX_IF_ERR_EXIT(READ_CCS_REACHABILITY_CELLS_CNTr(unit, i, &reg_val));
        fe_counters_info->ccs_counters_info.ccs_reachability.value += soc_reg_field_get(unit, CCS_REACHABILITY_CELLS_CNTr, reg_val, REACHABILITY_CELLS_CNTf);
        fe_counters_info->ccs_counters_info.ccs_reachability.overflow += soc_reg_field_get(unit, CCS_REACHABILITY_CELLS_CNTr, reg_val, REACHABILITY_CELLS_CNT_OVERFLOWf);

        SOCDNX_IF_ERR_EXIT(READ_CCS_UNREACHABLE_DESTINATION_CELLS_CNTr(unit, i, &reg_val));
        fe_counters_info->ccs_counters_info.ccs_unreach_dest.value += soc_reg_field_get(unit, CCS_UNREACHABLE_DESTINATION_CELLS_CNTr, reg_val, UNREACHABLE_DESTINATION_CELLS_CNTf);
        fe_counters_info->ccs_counters_info.ccs_unreach_dest.overflow += soc_reg_field_get(unit, CCS_UNREACHABLE_DESTINATION_CELLS_CNTr, reg_val, UNREACHABLE_DESTINATION_CELLS_CNT_OVERFLOWf);

    }

exit:
    SOCDNX_FUNC_RETURN
}

soc_error_t
soc_fe3200_queues_get_info(int unit, soc_dfe_queues_info_t* fe_queues_info)
{
    int nof_pipes, i, j;
    uint64 reg_val64;
    uint32 nof_fifo, reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    fe_queues_info->nof_pipes = nof_pipes;

    
    
    for (i = 0; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
    {
        
        
        SOCDNX_IF_ERR_EXIT(READ_DCH_IFM_MAX_OCCUP_P_0r(unit, i, &reg_val64));

        fe_queues_info->dch_queues_info[0].nof_dch[i].value_a = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMA_STAT_P_0f);
        nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMA_FIF_NUM_P_0f);
        fe_queues_info->dch_queues_info[0].nof_dch[i].nof_link_a = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + nof_fifo;

        fe_queues_info->dch_queues_info[0].nof_dch[i].value_b = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMB_STAT_P_0f);
        nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMB_FIF_NUM_P_0f);
        fe_queues_info->dch_queues_info[0].nof_dch[i].nof_link_b = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + SOC_DFE_DEFS_GET(unit, nof_links_in_dcq)/3 + nof_fifo; 

        fe_queues_info->dch_queues_info[0].nof_dch[i].value_c = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMC_STAT_P_0f);
        nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_0r, reg_val64, MAX_IFMC_FIF_NUM_P_0f);
        fe_queues_info->dch_queues_info[0].nof_dch[i].nof_link_c = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + 2*(SOC_DFE_DEFS_GET(unit,nof_links_in_dcq)/3) + nof_fifo; 
    }

    
    if (nof_pipes >= 2)
    {
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_IFM_MAX_OCCUP_P_1r(unit, i, &reg_val64));

            fe_queues_info->dch_queues_info[1].nof_dch[i].value_a = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMA_STAT_P_1f);
            nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMA_FIF_NUM_P_1f);
            fe_queues_info->dch_queues_info[1].nof_dch[i].nof_link_a = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + nof_fifo;
            
            fe_queues_info->dch_queues_info[1].nof_dch[i].value_b = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMB_STAT_P_1f) ;
            nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMB_FIF_NUM_P_1f);
            fe_queues_info->dch_queues_info[1].nof_dch[i].nof_link_b = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + SOC_DFE_DEFS_GET(unit, nof_links_in_dcq)/3 + nof_fifo; 

            fe_queues_info->dch_queues_info[1].nof_dch[i].value_c = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMC_STAT_P_1f);
            nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_1r, reg_val64, MAX_IFMC_FIF_NUM_P_1f);
            fe_queues_info->dch_queues_info[1].nof_dch[i].nof_link_c = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + 2*(SOC_DFE_DEFS_GET(unit,nof_links_in_dcq)/3) + nof_fifo; 
        }
    }
    
    if (nof_pipes == 3)
    {
        for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCH_IFM_MAX_OCCUP_P_2r(unit, i , &reg_val64));

            fe_queues_info->dch_queues_info[2].nof_dch[i].value_a = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMA_STAT_P_2f);
            nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMA_FIF_NUM_P_2f);
            fe_queues_info->dch_queues_info[2].nof_dch[i].nof_link_a = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + nof_fifo;

            fe_queues_info->dch_queues_info[2].nof_dch[i].value_b = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMB_STAT_P_2f);
            nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMB_FIF_NUM_P_2f);
            fe_queues_info->dch_queues_info[2].nof_dch[i].nof_link_b = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + SOC_DFE_DEFS_GET(unit, nof_links_in_dcq)/3 + nof_fifo; 

            fe_queues_info->dch_queues_info[2].nof_dch[i].value_c = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMC_STAT_P_2f);
            nof_fifo = soc_reg64_field32_get(unit, DCH_IFM_MAX_OCCUP_P_2r, reg_val64, MAX_IFMC_FIF_NUM_P_2f);
            fe_queues_info->dch_queues_info[2].nof_dch[i].nof_link_c = i*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + 2*(SOC_DFE_DEFS_GET(unit,nof_links_in_dcq)/3) + nof_fifo; 
        }
    }

    
    
    for (i = 0;  i < nof_pipes ; i++)
    {
        for (j = 0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dcm); j++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCM_DCM_MAX_OCUP_Pr(unit, j, i, &reg_val64));

            fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[0] = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHA_DCM_MAX_OCUP_P_Nf);
            fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[1] = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHB_DCM_MAX_OCUP_P_Nf);
            fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[2] = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHC_DCM_MAX_OCUP_P_Nf);
            fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[3] = soc_reg64_field32_get(unit, DCM_DCM_MAX_OCUP_Pr, reg_val64, DCHD_DCM_MAX_OCUP_P_Nf);
        }
    }

    

    for (i = 0; i < nof_pipes ; i++)
    {
        for (j = 0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); j++)
        {
            SOCDNX_IF_ERR_EXIT(READ_DCL_MAX_OCCUPANCY_FIFO_Pr(unit, j, i, &reg_val));

            fe_queues_info->dcl_queues_info[i].nof_dcl[j].value = soc_reg_field_get(unit, DCL_MAX_OCCUPANCY_FIFO_Pr, reg_val, MAX_OCCUP_P_Nf);
            nof_fifo = soc_reg_field_get(unit, DCL_MAX_OCCUPANCY_FIFO_Pr, reg_val, MAX_OCCUP_FIFO_P_Nf);
            fe_queues_info->dcl_queues_info[i].nof_dcl[j].nof_link = j*SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + nof_fifo;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_diag_mesh_topology_get(int unit, soc_dcmn_fabric_mesh_topology_diag_t *mesh_topology_diag)
{
    soc_reg_t reg_list[] = {MESH_TOPOLOGY_REG_1A0r,  MESH_TOPOLOGY_REG_01A1r, MESH_TOPOLOGY_REG_01A2r, MESH_TOPOLOGY_REG_01A3r, MESH_TOPOLOGY_REG_01A4r, MESH_TOPOLOGY_REG_01A5r, MESH_TOPOLOGY_REG_01A6r, MESH_TOPOLOGY_REG_01A7r, MESH_TOPOLOGY_REG_01A8r, MESH_TOPOLOGY_REG_01A9r, MESH_TOPOLOGY_REG_01AAr, MESH_TOPOLOGY_REG_01ABr, MESH_TOPOLOGY_REG_01ACr, MESH_TOPOLOGY_REG_01ADr, MESH_TOPOLOGY_REG_01AEr, MESH_TOPOLOGY_REG_01AFr,
                            MESH_TOPOLOGY_REG_01B0r, MESH_TOPOLOGY_REG_01B1r, MESH_TOPOLOGY_REG_01B2r, MESH_TOPOLOGY_REG_01B3r, MESH_TOPOLOGY_REG_01B4r, MESH_TOPOLOGY_REG_01B5r, MESH_TOPOLOGY_REG_01B6r, MESH_TOPOLOGY_REG_01B7r, MESH_TOPOLOGY_REG_01B8r, MESH_TOPOLOGY_REG_01B9r, MESH_TOPOLOGY_REG_01BAr, MESH_TOPOLOGY_REG_01BBr, MESH_TOPOLOGY_REG_01BCr, MESH_TOPOLOGY_REG_01BDr, MESH_TOPOLOGY_REG_01BEr, MESH_TOPOLOGY_REG_01BFr,
                            MESH_TOPOLOGY_REG_01C0r, MESH_TOPOLOGY_REG_01C1r, MESH_TOPOLOGY_REG_01C2r, MESH_TOPOLOGY_REG_01C3r};
    uint32 reg32_val;
    uint64 reg64_val;
    int i, valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_0000r(unit, &reg32_val));
    mesh_topology_diag->indication_1 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_0000r, reg32_val, FIELD_1_1f);
    mesh_topology_diag->indication_2 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_0000r, reg32_val, FIELD_2_2f);

    for (i = 0; i < sizeof(reg_list) / sizeof(soc_reg_t); i++) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg_list[i], REG_PORT_ANY, 0, &reg32_val));
        valid = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_15_15f);
        if (valid) {
            mesh_topology_diag->type2_list[i*2] = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_0_14f);
        } else {
            mesh_topology_diag->type2_list[i*2] = -1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg_list[i], REG_PORT_ANY, 0, &reg32_val));
        valid = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_31_31f);
        if (valid) {
            mesh_topology_diag->type2_list[i*2 + 1] = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_16_30f);
        } else {
            mesh_topology_diag->type2_list[i*2 + 1] = -1;
        }
    }
    mesh_topology_diag->link_list_count = i*2;

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr(unit, &reg32_val));
    mesh_topology_diag->rx_control_cells_type1 = soc_reg_field_get(unit, MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr, reg32_val, RCV_CTL_1f);
    mesh_topology_diag->rx_control_cells_type2 = soc_reg_field_get(unit, MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr, reg32_val, RCV_CTL_2f);

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01C5r(unit, &reg32_val));
    mesh_topology_diag->indication_3 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_01C5r, reg32_val, FIELD_4_4f);
    mesh_topology_diag->indication_4 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_01C5r, reg32_val, FIELD_5_5f);
    mesh_topology_diag->control_cells_type3 = soc_reg_field_get(unit, MESH_TOPOLOGY_REG_01C5r, reg32_val, FIELD_7_20f);

    
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01C8r(unit, &reg64_val));
    mesh_topology_diag->status_1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01C8r, reg64_val, FIELD_0_14f);
    mesh_topology_diag->status_1_id1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01C8r, reg64_val, FIELD_15_22f);
    mesh_topology_diag->status_1_id2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01C8r, reg64_val, FIELD_23_33f);

    
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01C9r(unit, &reg64_val));
    mesh_topology_diag->status_2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01C9r, reg64_val, FIELD_0_14f);
    mesh_topology_diag->status_2_id1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01C9r, reg64_val, FIELD_15_22f);
    mesh_topology_diag->status_2_id2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01C9r, reg64_val, FIELD_23_33f);

exit:
    SOCDNX_FUNC_RETURN;
}
