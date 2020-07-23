
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_DRAM

 
#include <shared/bsl.h>

#include <sal/types.h>

 
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>


#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

 
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/DRC/drc_combo28_bist.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>

 
#include <shared/swstate/access/sw_state_access.h>



#define SWSTATE_JER_TM_ACCESS   sw_state_access[unit].dpp.soc.jericho.tm

#define DPP_DRC_COMBO28_ADDR_DQ_BIT_MAP_VALUES_REQUIRES_GENERIC_HANDLING 16


int soc_dpp_drc_combo28_util_ps2ck(int unit, int mr, int t_ck, int val, uint32 *clk)
{
    int tmp = 0x0, 
        deviation_per = 0x0,
        ps2ck = 0x0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(clk);

    if (t_ck == 0x0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: t_ck=%d is no initialized."), FUNCTION_NAME(), t_ck));
    }

    if (val == 0x0) {
        
        *clk = 0x0;
    } else if (val & SOC_SAND_BIT(SOC_DPP_DRC_COMBO28_VAL_IS_IN_CLOCKS_BIT)) {
        
        *clk = val & SOC_SAND_RBIT(SOC_DPP_DRC_COMBO28_VAL_IS_IN_CLOCKS_BIT);
    } else {
        
        ps2ck = SOC_SAND_DIV_ROUND_UP(val, t_ck);
        tmp = ps2ck * t_ck;
        deviation_per = ((tmp - val) * 10000000) / val;

        if ((deviation_per >= 0 ) && (deviation_per < 1000) && ((mr == 0x0) || (deviation_per != 0))) {
            ps2ck = ps2ck + 1;
        }

        *clk = ps2ck;
    }

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): mr=%d, t_ck=%d, val=%d,0x%x, ps2ck=%d, tmp=%d, deviation_per=%d, *clk=%d.\n"),
                          FUNCTION_NAME(), mr, t_ck, val, val, ps2ck, tmp, deviation_per, *clk));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_wr_recovery_opcode(int unit, uint32 dram_type, uint32 c_mr_wr, uint32 *wr_recovery_op)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(wr_recovery_op);

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
        switch (c_mr_wr) {   
        case 10:
            *wr_recovery_op = 0;
            break;
        case 11:
        case 12:
            *wr_recovery_op = 1;
            break;
        case 13:
        case 14:
            *wr_recovery_op = 2;
            break;
        case 15:
        case 16:
            *wr_recovery_op = 3;
            break;
        case 17:
        case 18:
            *wr_recovery_op = 4;
            break;
        case 19:
        case 20:
            *wr_recovery_op = 5;
            break;
        case 21:
        case 22:
        case 23:
        case 24:
            *wr_recovery_op = 6;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: write recovery c_mr_wr=%d is is out of bound."), FUNCTION_NAME(), c_mr_wr));
        }
    break;
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        if ((c_mr_wr >= 4) && (c_mr_wr <= 35)) {
            *wr_recovery_op = c_mr_wr - 4;
            break;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: write recovery c_mr_wr=%d is is out of range."), FUNCTION_NAME(), c_mr_wr));
        }
        
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_mr_wr=%d, *wr_recovery_op=%d.\n"),FUNCTION_NAME(), c_mr_wr, *wr_recovery_op));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_cas_latency_calc(int unit, uint32 dram_type, uint32 c_cas_latency, uint32 *cas_latency_calc)
{
    
    int ddr4_cas_latency_translation_arr[33] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6, 
                                                  7, 13,  8, 14,  9, 15, 10, 12, 11, 16, 17, 18, 19, 20, 21, 22, 23 };
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cas_latency_calc);

    *cas_latency_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
        if ((c_cas_latency < 9) || (c_cas_latency > 32)) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: cas latency is out of bound. c_cas_latency=%d"), FUNCTION_NAME(), c_cas_latency));
        }

        *cas_latency_calc = ddr4_cas_latency_translation_arr[c_cas_latency];
        break;
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        if ((c_cas_latency <=36) && (c_cas_latency >= 5)) {
            *cas_latency_calc = c_cas_latency - 5;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: cas latency is out of bound. c_cas_latency=%d"), FUNCTION_NAME(), c_cas_latency));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): c_cas_latency=%d, *cas_latency_calc=%d.\n"),FUNCTION_NAME(), c_cas_latency, *cas_latency_calc));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_wr_latency_calc(int unit, uint32 dram_type, uint32 c_wr_latency, uint32 *wr_latency_calc)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(wr_latency_calc);

    *wr_latency_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
        if ((c_wr_latency >=9) && (c_wr_latency <= 12)) {
            *wr_latency_calc = c_wr_latency - 9;
        } else if ((c_wr_latency == 14) || (c_wr_latency == 16) || (c_wr_latency == 18)) {
            *wr_latency_calc = (c_wr_latency / 2) - 3;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Write latency is out of bound. c_wr_latency=%d"), FUNCTION_NAME(), c_wr_latency));
        }
        break;
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        if (c_wr_latency <= 7) {
            *wr_latency_calc = c_wr_latency;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Write latency is out of bound. c_wr_latency=%d"), FUNCTION_NAME(), c_wr_latency));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_wr_latency=%d, *wr_latency_calc=%d.\n"),FUNCTION_NAME(), c_wr_latency, *wr_latency_calc));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_ccd_l_calc(int unit, uint32 dram_type, uint32 c_ccd_l, uint32 *ccd_l_calc)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ccd_l_calc);

    *ccd_l_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
        if ((c_ccd_l >=4) && (c_ccd_l <= 12)) {
            *ccd_l_calc = c_ccd_l - 4;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: CCD_L is out of bound. c_ccd_l=%d"), FUNCTION_NAME(), c_ccd_l));
        }
        break;
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        switch (c_ccd_l) {   
        case 2:
            *ccd_l_calc = 0;
            break;
        case 3:
            *ccd_l_calc = 3;
            break;
        case 4:
            *ccd_l_calc = 2;
            break;
         default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: CCD_L is out of bound. c_ccd_l=%d"), FUNCTION_NAME(), c_ccd_l));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_ccd_l=%d, *ccd_l_calc=%d.\n"),FUNCTION_NAME(), c_ccd_l, *ccd_l_calc));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_cmd_par_latency_calc(int unit, uint32 dram_type, uint32 cmd_par_latency, uint32 *cmd_par_latency_calc)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cmd_par_latency_calc);

    *cmd_par_latency_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
        switch (cmd_par_latency) {   
        case 0:
            *cmd_par_latency_calc = 0;
            break;
        case 4:
            *cmd_par_latency_calc = 1;
            break;
        case 5:
            *cmd_par_latency_calc = 2;
            break;
        case 6:
            *cmd_par_latency_calc = 3;
            break;
        case 8:
            *cmd_par_latency_calc = 4;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: command_parity_latency_calc support only values of 0,4,5,6,8 was called with cmd_par_latency=%d"), 
                                                FUNCTION_NAME(), cmd_par_latency));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): cmd_par_latency=%d, *cmd_par_latency_calc=%d.\n"),FUNCTION_NAME(), cmd_par_latency, *cmd_par_latency_calc));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_crc_rd_latency_calc(int unit, uint32 dram_type, uint32 c_crc_rd_latency, uint32 *crc_rd_latency_calc)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(crc_rd_latency_calc);

    *crc_rd_latency_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        if (c_crc_rd_latency <= 4) {
            *crc_rd_latency_calc = c_crc_rd_latency;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: CRCRL is out of bound. c_crc_rd_latency=%d"), FUNCTION_NAME(), c_crc_rd_latency));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_crc_rd_latency=%d, *crc_rd_latency_calc=%d.\n"),FUNCTION_NAME(), c_crc_rd_latency, *crc_rd_latency_calc));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_crc_wr_latency_calc(int unit, uint32 dram_type, uint32 c_crc_wr_latency, uint32 *crc_wr_latency_calc)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(crc_wr_latency_calc);

    *crc_wr_latency_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        if ((c_crc_wr_latency >= 7) && (c_crc_wr_latency <= 14)) {
            *crc_wr_latency_calc = c_crc_wr_latency - 7;
        } else if (c_crc_wr_latency == 0) {
            *crc_wr_latency_calc = c_crc_wr_latency;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: CRCWL is out of bound. c_crc_wr_latency=%d"), FUNCTION_NAME(), c_crc_wr_latency));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_crc_wr_latency=%d, *crc_wr_latency_calc=%d.\n"),FUNCTION_NAME(), c_crc_wr_latency, *crc_wr_latency_calc));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_util_ras_calc(int unit, uint32 dram_type, uint32 c_mr_ras, uint32 *ras_calc)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ras_calc);

    *ras_calc = 0x0;

    switch (dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        if (c_mr_ras <= 63) {
            *ras_calc = c_mr_ras;
        } else {
            if (!SOC_IS_DNX_TEST_DEVICE(unit)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: DDRtRAS for MR is out of bound. c_mr_ras=%d"), FUNCTION_NAME(), c_mr_ras));
            } else {
                LOG_WARN(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Warning, DDRtRAS for MR is out of bound. c_mr_ras=%d\n"), FUNCTION_NAME(), c_mr_ras));
                *ras_calc = c_mr_ras;
            }
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), dram_type));
    }   
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_mr_ras=%d, *ras_calc=%d.\n"),FUNCTION_NAME(), c_mr_ras, *ras_calc));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_info_struct_convert(int unit, soc_dpp_drc_combo28_info_t *drc_info, combo28_shmoo_dram_info_t *shmoo_info)
{
    SOCDNX_INIT_FUNC_DEFS;

    shmoo_info->ctl_type                = SHMOO_COMBO28_CTL_TYPE_1;
    shmoo_info->dram_type               = drc_info->dram_type;

    SHR_BITCOPY_RANGE(&shmoo_info->dram_bitmap, 0, drc_info->dram_bitmap, 0, SOC_DPP_DEFS_GET(unit,hw_dram_interfaces_max));
    shmoo_info->num_columns             = drc_info->nof_columns;
    shmoo_info->num_rows                = drc_info->nof_rows;
    shmoo_info->num_banks               = drc_info->nof_banks;
    shmoo_info->data_rate_mbps          = drc_info->data_rate_mbps;
    shmoo_info->ref_clk_mhz             = drc_info->ref_clk_mhz;
    shmoo_info->refi                    = drc_info->dram_int_param.c_refi;
    shmoo_info->command_parity_latency  = drc_info->cmd_par_latency;
    shmoo_info->sim_system_mode         = drc_info->sim_system_mode;
    sal_memcpy(shmoo_info->zq_cal_array, drc_info->zq_calib_map, sizeof(uint8) * SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));
    SHR_BITCOPY_RANGE(&shmoo_info->ref_clk_bitmap, 0, drc_info->ref_clk_bitmap, 0, SOC_DPP_DEFS_GET(unit,hw_dram_interfaces_max));

    sal_memcpy(shmoo_info->dq_swap, drc_info->dram_dq_swap, sizeof(uint32)*SHMOO_COMBO28_MAX_INTERFACES*SHMOO_COMBO28_BYTES_PER_INTERFACE*SHMOO_COMBO28_BYTE);

    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_dprc_out_of_reset(int unit, int drc_ndx)
{
    uint32 reg_val;
    static soc_reg_t DRC_DPRC_ENABLERSl[] = {DRCA_DPRC_ENABLERSr, DRCB_DPRC_ENABLERSr, DRCC_DPRC_ENABLERSr, 
                                             DRCD_DPRC_ENABLERSr, DRCE_DPRC_ENABLERSr, DRCF_DPRC_ENABLERSr, 
                                             DRCG_DPRC_ENABLERSr, DRCH_DPRC_ENABLERSr};

    static soc_reg_t DRC_DDR_PHY_PLL_RESETl[] = {DRCA_DDR_PHY_PLL_RESETr, DRCB_DDR_PHY_PLL_RESETr, DRCC_DDR_PHY_PLL_RESETr,
                                                 DRCD_DDR_PHY_PLL_RESETr, DRCE_DDR_PHY_PLL_RESETr, DRCF_DDR_PHY_PLL_RESETr,
                                                 DRCG_DDR_PHY_PLL_RESETr, DRCH_DDR_PHY_PLL_RESETr};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Aligner out of reset.\n"),FUNCTION_NAME(), drc_ndx));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, DPRC_ALIGN_SOFT_INITf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, DPRC_ALIGN_SOFT_INITf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, DPRC out of reset.\n"),FUNCTION_NAME(), drc_ndx));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, DPRC_PRC_SOFT_INITf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, DPRC_PRC_SOFT_INITf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, DDR-PHY out of reset.\n"),FUNCTION_NAME(), drc_ndx));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, DDR_PHY_RSTNf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, DDR_PHY_RSTNf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Enable DDR-PHY write phase FIFO.\n"),FUNCTION_NAME(), drc_ndx));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, WRITE_FIFO_ENABLEf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, WRITE_FIFO_ENABLEf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    if(SOC_IS_QUX(unit))
    {
        LOG_VERBOSE(BSL_LS_SOC_DRAM,
                    (BSL_META_U(unit,
                                "%s(): drc_ndx=%d, MMU portion on dram clock out of reset.\n"),FUNCTION_NAME(), drc_ndx));
        soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, MMU_FAST_CLK_DIV_RSTNf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
        soc_reg_field_set(unit, DRCA_DPRC_ENABLERSr, &reg_val, MMU_RSTNf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DPRC_ENABLERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Disable IDDQ.\n"),FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DDR_PHY_PLL_RESETl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_DDR_PHY_PLL_RESETr, &reg_val, IDDQ_ENABLEf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DDR_PHY_PLL_RESETl[drc_ndx], REG_PORT_ANY, 0, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}   



int soc_dpp_drc_combo28_init_dprc_init(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    if (SOC_IS_SINAI(unit))
    {
        rv = soc_combo16_shmoo_phy_cfg_pll(unit, drc_ndx);
    }
    else 
    {
        rv = soc_combo28_shmoo_phy_cfg_pll(unit, drc_ndx); 
    }
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_IF_ERR_EXIT(rv); 
    }
    
    
    rv = soc_dpp_drc_combo28_dprc_out_of_reset(unit, drc_ndx); 
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_drc_clam_shell_cfg(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int 
        rv = SOC_E_NONE,
        mr_ndx;
    uint32
        field32_val,
        reg32_val,
        shadow_mr_clam_shell_swap[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX] = {0x0, 0x2, 0x1, 0x3, 0x4, 0x6, 0x5, 0x7, 0x8, 0xff, 0xff, 0xff, 0xc, 0xff, 0xd, 0xf};
    static soc_reg_t 
        DRC_CLAM_SHELLl[] = {DRCA_CLAM_SHELLr, DRCB_CLAM_SHELLr, DRCC_CLAM_SHELLr, 
                             DRCD_CLAM_SHELLr, DRCE_CLAM_SHELLr, DRCF_CLAM_SHELLr, 
                             DRCG_CLAM_SHELLr, DRCH_CLAM_SHELLr},
        DRC_AUXS_MUXl[] = {DRCA_AUXS_MUXr, DRCB_AUXS_MUXr, DRCC_AUXS_MUXr,
                           DRCD_AUXS_MUXr, DRCE_AUXS_MUXr, DRCF_AUXS_MUXr,
                           DRCG_AUXS_MUXr, DRCH_AUXS_MUXr},
        DRC_SHADOW_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
            {{DRCA_SHADOW_DRAM_MR_0r, DRCB_SHADOW_DRAM_MR_0r, DRCC_SHADOW_DRAM_MR_0r, DRCD_SHADOW_DRAM_MR_0r, DRCE_SHADOW_DRAM_MR_0r, DRCF_SHADOW_DRAM_MR_0r, DRCG_SHADOW_DRAM_MR_0r, DRCH_SHADOW_DRAM_MR_0r},
             {DRCA_SHADOW_DRAM_MR_1r, DRCB_SHADOW_DRAM_MR_1r, DRCC_SHADOW_DRAM_MR_1r, DRCD_SHADOW_DRAM_MR_1r, DRCE_SHADOW_DRAM_MR_1r, DRCF_SHADOW_DRAM_MR_1r, DRCG_SHADOW_DRAM_MR_1r, DRCH_SHADOW_DRAM_MR_1r},
             {DRCA_SHADOW_DRAM_MR_2r, DRCB_SHADOW_DRAM_MR_2r, DRCC_SHADOW_DRAM_MR_2r, DRCD_SHADOW_DRAM_MR_2r, DRCE_SHADOW_DRAM_MR_2r, DRCF_SHADOW_DRAM_MR_2r, DRCG_SHADOW_DRAM_MR_2r, DRCH_SHADOW_DRAM_MR_2r},
             {DRCA_SHADOW_DRAM_MR_3r, DRCB_SHADOW_DRAM_MR_3r, DRCC_SHADOW_DRAM_MR_3r, DRCD_SHADOW_DRAM_MR_3r, DRCE_SHADOW_DRAM_MR_3r, DRCF_SHADOW_DRAM_MR_3r, DRCG_SHADOW_DRAM_MR_3r, DRCH_SHADOW_DRAM_MR_3r},
             {DRCA_SHADOW_DRAM_MR_4r, DRCB_SHADOW_DRAM_MR_4r, DRCC_SHADOW_DRAM_MR_4r, DRCD_SHADOW_DRAM_MR_4r, DRCE_SHADOW_DRAM_MR_4r, DRCF_SHADOW_DRAM_MR_4r, DRCG_SHADOW_DRAM_MR_4r, DRCH_SHADOW_DRAM_MR_4r},
             {DRCA_SHADOW_DRAM_MR_5r, DRCB_SHADOW_DRAM_MR_5r, DRCC_SHADOW_DRAM_MR_5r, DRCD_SHADOW_DRAM_MR_5r, DRCE_SHADOW_DRAM_MR_5r, DRCF_SHADOW_DRAM_MR_5r, DRCG_SHADOW_DRAM_MR_5r, DRCH_SHADOW_DRAM_MR_5r},
             {DRCA_SHADOW_DRAM_MR_6r, DRCB_SHADOW_DRAM_MR_6r, DRCC_SHADOW_DRAM_MR_6r, DRCD_SHADOW_DRAM_MR_6r, DRCE_SHADOW_DRAM_MR_6r, DRCF_SHADOW_DRAM_MR_6r, DRCG_SHADOW_DRAM_MR_6r, DRCH_SHADOW_DRAM_MR_6r},
             {DRCA_SHADOW_DRAM_MR_7r, DRCB_SHADOW_DRAM_MR_7r, DRCC_SHADOW_DRAM_MR_7r, DRCD_SHADOW_DRAM_MR_7r, DRCE_SHADOW_DRAM_MR_7r, DRCF_SHADOW_DRAM_MR_7r, DRCG_SHADOW_DRAM_MR_7r, DRCH_SHADOW_DRAM_MR_7r},
             {DRCA_SHADOW_DRAM_MR_8r, DRCB_SHADOW_DRAM_MR_8r, DRCC_SHADOW_DRAM_MR_8r, DRCD_SHADOW_DRAM_MR_8r, DRCE_SHADOW_DRAM_MR_8r, DRCF_SHADOW_DRAM_MR_8r, DRCG_SHADOW_DRAM_MR_8r, DRCH_SHADOW_DRAM_MR_8r},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_12r, DRCB_SHADOW_DRAM_MR_12r, DRCC_SHADOW_DRAM_MR_12r, DRCD_SHADOW_DRAM_MR_12r, DRCE_SHADOW_DRAM_MR_12r, DRCF_SHADOW_DRAM_MR_12r, DRCG_SHADOW_DRAM_MR_12r, DRCH_SHADOW_DRAM_MR_12r},
             {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_14r, DRCB_SHADOW_DRAM_MR_14r, DRCC_SHADOW_DRAM_MR_14r, DRCD_SHADOW_DRAM_MR_14r, DRCE_SHADOW_DRAM_MR_14r, DRCF_SHADOW_DRAM_MR_14r, DRCG_SHADOW_DRAM_MR_14r, DRCH_SHADOW_DRAM_MR_14r},
             {DRCA_SHADOW_DRAM_MR_15r, DRCB_SHADOW_DRAM_MR_15r, DRCC_SHADOW_DRAM_MR_15r, DRCD_SHADOW_DRAM_MR_15r, DRCE_SHADOW_DRAM_MR_15r, DRCF_SHADOW_DRAM_MR_15r, DRCG_SHADOW_DRAM_MR_15r, DRCH_SHADOW_DRAM_MR_15r}};
     static soc_field_t 
         SHADOW_DRAM_MR_ADDRl[] = {SHADOW_DRAM_MR_0_ADDRf,  SHADOW_DRAM_MR_1_ADDRf, SHADOW_DRAM_MR_2_ADDRf, SHADOW_DRAM_MR_3_ADDRf, 
                                   SHADOW_DRAM_MR_4_ADDRf,  SHADOW_DRAM_MR_5_ADDRf, SHADOW_DRAM_MR_6_ADDRf, SHADOW_DRAM_MR_7_ADDRf, 
                                   SHADOW_DRAM_MR_8_ADDRf,  INVALIDf,               INVALIDf,               INVALIDf, 
                                   SHADOW_DRAM_MR_12_ADDRf, INVALIDf,               SHADOW_DRAM_MR_14_ADDRf, SHADOW_DRAM_MR_15_ADDRf},
         SHADOW_DRAM_MR_BANKl[] = {SHADOW_DRAM_MR_0_BANKf,  SHADOW_DRAM_MR_1_BANKf, SHADOW_DRAM_MR_2_BANKf, SHADOW_DRAM_MR_3_BANKf, 
                                   SHADOW_DRAM_MR_4_BANKf,  SHADOW_DRAM_MR_5_BANKf, SHADOW_DRAM_MR_6_BANKf, SHADOW_DRAM_MR_7_BANKf, 
                                   SHADOW_DRAM_MR_8_BANKf,  INVALIDf,               INVALIDf,               INVALIDf, 
                                   SHADOW_DRAM_MR_12_BANKf, INVALIDf,               SHADOW_DRAM_MR_14_BANKf, SHADOW_DRAM_MR_15_BANKf};

    SOCDNX_INIT_FUNC_DEFS;

    if (drc_info->dram_clam_shell_mode[drc_ndx] == DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DISABLED) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_DISABLED, (_BSL_SOCDNX_MSG("Error in %s: Clam shell is disabled for drc_ndx=%d."),FUNCTION_NAME(), drc_ndx));   
    }

    
    for (mr_ndx = 0; mr_ndx < SOC_DPP_DRC_COMBO28_MRS_NUM_MAX; mr_ndx++) {

        if ((mr_ndx == 9) || (mr_ndx == 10) || (mr_ndx == 11) || (mr_ndx == 13)) {
            continue;
        }

        rv = soc_dpp_drc_combo28_util_calm_shell_bits_swap(unit, drc_info->mr[drc_ndx][mr_ndx], &field32_val);
        SOCDNX_IF_ERR_EXIT(rv);
        reg32_val = 0;
        soc_reg_field_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], &reg32_val, SHADOW_DRAM_MR_ADDRl[mr_ndx], field32_val);
        field32_val = shadow_mr_clam_shell_swap[mr_ndx];
        soc_reg_field_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], &reg32_val, SHADOW_DRAM_MR_BANKl[mr_ndx], field32_val);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, reg32_val));
    }

    
    reg32_val = 0x0;
    field32_val = 12; 
    soc_reg_field_set(unit, DRCA_AUXS_MUXr, &reg32_val, AUX_0_MUXf, field32_val);
    field32_val = 13; 
    soc_reg_field_set(unit, DRCA_AUXS_MUXr, &reg32_val, AUX_1_MUXf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AUXS_MUXl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_CLAM_SHELLl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0x1;
    soc_reg_field_set(unit, DRCA_CLAM_SHELLr, &reg32_val, CLAM_SHELL_MODEf, field32_val);
    field32_val = drc_info->dram_clam_shell_mode[drc_ndx] == DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_0 ? 1 : 0;
    soc_reg_field_set(unit, DRCA_CLAM_SHELLr, &reg32_val, USE_CLAM_SHELL_DDR_0f, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_CLAM_SHELLl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_drc_addr_bank_swap(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info)
{
    int bit, swap_counter = 0;
    soc_reg_above_64_val_t reg_above64_val;

    static soc_reg_t drc_addr_bank_map[] = { DRCA_ADDR_BANK_MAPr, DRCB_ADDR_BANK_MAPr, DRCC_ADDR_BANK_MAPr, DRCD_ADDR_BANK_MAPr,
                                             DRCE_ADDR_BANK_MAPr, DRCF_ADDR_BANK_MAPr, DRCG_ADDR_BANK_MAPr, DRCH_ADDR_BANK_MAPr };
    static soc_field_t addr_bank_map[] = { BANK_0_MAPf, BANK_1_MAPf, BANK_2_MAPf, BANK_3_MAPf, ADDR_0_MAPf, ADDR_1_MAPf,  ADDR_2_MAPf,  ADDR_3_MAPf,  ADDR_4_MAPf, 
                                           ADDR_5_MAPf, ADDR_6_MAPf, ADDR_7_MAPf, ADDR_8_MAPf, ADDR_9_MAPf, ADDR_10_MAPf, ADDR_11_MAPf, ADDR_12_MAPf, ADDR_13_MAPf };
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, drc_addr_bank_map[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

    for (bit = 0; bit < SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE ; ++bit) {
        
        if (drc_info->dram_addr_bank_swap[drc_ndx][bit] != bit)  {
            soc_reg_above_64_field32_set(unit, DRCA_ADDR_BANK_MAPr, reg_above64_val, addr_bank_map[bit], drc_info->dram_addr_bank_swap[drc_ndx][bit]);
            ++swap_counter;
            LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): addr_bank_swap[drc_ndx=%d][bit=%d]=%d.\n"), FUNCTION_NAME(), drc_ndx, bit, drc_info->dram_addr_bank_swap[drc_ndx][bit]));
        }
    }

    
    if (swap_counter > 0) {
        
        soc_reg_above_64_field32_set(unit, DRCA_ADDR_BANK_MAPr, reg_above64_val, ADDR_MAP_ENf, 0x1); 

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, drc_addr_bank_map[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_drc_dq_swap(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int         current_addr_bank_index;
    int         byte;
    int         bit;
    int         current_addr_dq_byte;
    int         current_swap;
    int         position_delta_caused_by_swap;
    uint32      reg32_val;
    uint32      field32_val;
    
    uint32      addr_dq_bit_map_val[DPP_DRC_COMBO28_ADDR_DQ_BIT_MAP_VALUES_REQUIRES_GENERIC_HANDLING] = {9, 11, 13, 15, 25, 27, 29, 31, 30, 10, 8 , 28, 12, 26, 24, 14};
    soc_reg_above_64_val_t
        reg_above64_val,
        field_above64_val;
    static soc_reg_t 
        DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[] = {DRCA_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, DRCB_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, DRCC_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, 
                                              DRCD_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, DRCE_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, DRCF_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, 
                                              DRCG_GDDR_5_BIST_ADT_ADDR_DQ_MAPr, DRCH_GDDR_5_BIST_ADT_ADDR_DQ_MAPr},
        DRC_DQ_BYTE_MAPl[SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
            {{DRCA_DQ_BYTE_0_MAPr, DRCB_DQ_BYTE_0_MAPr, DRCC_DQ_BYTE_0_MAPr, DRCD_DQ_BYTE_0_MAPr, DRCE_DQ_BYTE_0_MAPr, DRCF_DQ_BYTE_0_MAPr, DRCG_DQ_BYTE_0_MAPr, DRCH_DQ_BYTE_0_MAPr},
             {DRCA_DQ_BYTE_1_MAPr, DRCB_DQ_BYTE_1_MAPr, DRCC_DQ_BYTE_1_MAPr, DRCD_DQ_BYTE_1_MAPr, DRCE_DQ_BYTE_1_MAPr, DRCF_DQ_BYTE_1_MAPr, DRCG_DQ_BYTE_1_MAPr, DRCH_DQ_BYTE_1_MAPr},
             {DRCA_DQ_BYTE_2_MAPr, DRCB_DQ_BYTE_2_MAPr, DRCC_DQ_BYTE_2_MAPr, DRCD_DQ_BYTE_2_MAPr, DRCE_DQ_BYTE_2_MAPr, DRCF_DQ_BYTE_2_MAPr, DRCG_DQ_BYTE_2_MAPr, DRCH_DQ_BYTE_2_MAPr},
             {DRCA_DQ_BYTE_3_MAPr, DRCB_DQ_BYTE_3_MAPr, DRCC_DQ_BYTE_3_MAPr, DRCD_DQ_BYTE_3_MAPr, DRCE_DQ_BYTE_3_MAPr, DRCF_DQ_BYTE_3_MAPr, DRCG_DQ_BYTE_3_MAPr, DRCH_DQ_BYTE_3_MAPr}};
    static soc_field_t 
        DQ_BIT_MAPl[SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES][SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE] = 
                          { {DQ_0_BYTE_0_MAPf, DQ_1_BYTE_0_MAPf, DQ_2_BYTE_0_MAPf, DQ_3_BYTE_0_MAPf, DQ_4_BYTE_0_MAPf, DQ_5_BYTE_0_MAPf, DQ_6_BYTE_0_MAPf, DQ_7_BYTE_0_MAPf},
                            {DQ_0_BYTE_1_MAPf, DQ_1_BYTE_1_MAPf, DQ_2_BYTE_1_MAPf, DQ_3_BYTE_1_MAPf, DQ_4_BYTE_1_MAPf, DQ_5_BYTE_1_MAPf, DQ_6_BYTE_1_MAPf, DQ_7_BYTE_1_MAPf},
                            {DQ_0_BYTE_2_MAPf, DQ_1_BYTE_2_MAPf, DQ_2_BYTE_2_MAPf, DQ_3_BYTE_2_MAPf, DQ_4_BYTE_2_MAPf, DQ_5_BYTE_2_MAPf, DQ_6_BYTE_2_MAPf, DQ_7_BYTE_2_MAPf},
                            {DQ_0_BYTE_3_MAPf, DQ_1_BYTE_3_MAPf, DQ_2_BYTE_3_MAPf, DQ_3_BYTE_3_MAPf, DQ_4_BYTE_3_MAPf, DQ_5_BYTE_3_MAPf, DQ_6_BYTE_3_MAPf, DQ_7_BYTE_3_MAPf} },
        DRC_GDDR_5_BIST_ADT_ADDR_DQ_BIT_MAPl[] = {ADDR_0f, ADDR_1f, ADDR_2f, ADDR_3f, ADDR_4f, ADDR_5f, ADDR_6f, ADDR_7f, ADDR_8f, ADDR_9f, ADDR_10f, ADDR_11f,
                                                  BANK_0f, BANK_1f, BANK_2f, BANK_3f};

    SOCDNX_INIT_FUNC_DEFS;

    for (byte = 0; byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; byte++) 
    {
        for (bit = 0; bit < SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE ; bit++) 
        {
            if (drc_info->dram_dq_swap[drc_ndx][byte][bit] != bit) 
            {
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DQ_BYTE_MAPl[byte][drc_ndx], REG_PORT_ANY, 0, &reg32_val));
                soc_reg_field_set(unit, DRC_DQ_BYTE_MAPl[byte][0], &reg32_val, DQ_BIT_MAPl[byte][bit], drc_info->dram_dq_swap[drc_ndx][byte][bit]);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DQ_BYTE_MAPl[byte][drc_ndx], REG_PORT_ANY, 0, reg32_val));

                LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): dq_swap[drc_ndx=%d][byte=%d][bit=%d]=%d, reg32_val=0x%x.\n"),
                                                             FUNCTION_NAME(), drc_ndx,    byte,    bit, drc_info->dram_dq_swap[drc_ndx][byte][bit], reg32_val));
            }
        }
    }

    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    
    
    for (current_addr_bank_index = 0; current_addr_bank_index < DPP_DRC_COMBO28_ADDR_DQ_BIT_MAP_VALUES_REQUIRES_GENERIC_HANDLING; current_addr_bank_index++) 
    { 
        
        current_addr_dq_byte = addr_dq_bit_map_val[current_addr_bank_index] / SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE;
        current_swap = drc_info->dram_dq_swap_bytes[drc_ndx][current_addr_dq_byte];
        position_delta_caused_by_swap = (current_swap - current_addr_dq_byte) * SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE;
        field32_val = addr_dq_bit_map_val[current_addr_bank_index] + position_delta_caused_by_swap;
        if (field32_val > 31) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Error in %s: calculated field value:%d, exceeded allowed values (0-31)."), FUNCTION_NAME(), field32_val));
        }
        soc_reg_above_64_field32_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], reg_above64_val, DRC_GDDR_5_BIST_ADT_ADDR_DQ_BIT_MAPl[current_addr_bank_index], field32_val); 
    }

    
    if (drc_info->dram_dq_swap_bytes[drc_ndx][0] == 2 || drc_info->dram_dq_swap_bytes[drc_ndx][0] == 3) 
    {
        soc_reg_above_64_field32_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], reg_above64_val, ADDR_12f, 2);
        soc_reg_above_64_field32_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], reg_above64_val, ADDR_13f, 0);
    } else {
        soc_reg_above_64_field32_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], reg_above64_val, ADDR_12f, 0);
        soc_reg_above_64_field32_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], reg_above64_val, ADDR_13f, 2);
    }

    
    soc_reg_above_64_field32_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], reg_above64_val, DQ_TO_ADDR_MAP_ENf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_BIST_ADT_ADDR_DQ_MAPl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_drc_soft_init(int unit, int drc_ndx, int init) 
{
    uint32 
        reg_val,
        field_val;
    static soc_reg_t DRC_INITIALIZATION_CONTROLLl[] = {DRCA_INITIALIZATION_CONTROLLr, DRCB_INITIALIZATION_CONTROLLr, DRCC_INITIALIZATION_CONTROLLr, 
                                                       DRCD_INITIALIZATION_CONTROLLr, DRCE_INITIALIZATION_CONTROLLr, DRCF_INITIALIZATION_CONTROLLr, 
                                                       DRCG_INITIALIZATION_CONTROLLr, DRCH_INITIALIZATION_CONTROLLr};

    SOCDNX_INIT_FUNC_DEFS;

    field_val = (init == 0) ? 0x1 : 0x0;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, init=%d, field_val=%d.\n"), FUNCTION_NAME(), init, field_val, drc_ndx));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DDR_RSTNf, field_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_sw_state_init(int unit)
{
    uint8 is_allocated;
    
    SOCDNX_INIT_FUNC_DEFS;    
    
    
    SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_rd_crc_interrupt_state.is_allocated(unit, &is_allocated));
    if (!is_allocated)
    {
        SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_rd_crc_interrupt_state.alloc(unit, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)));
    }

    
    SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_wr_crc_interrupt_state.is_allocated(unit, &is_allocated));
    if (!is_allocated)
    {
        SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_wr_crc_interrupt_state.alloc(unit, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)));
    }    

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_info_prepare(int unit, soc_dpp_drc_combo28_info_t *drc_info)
{
    int 
        vco_prd;
    uint32
        t_ck,
        tmp_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    vco_prd = 1000000 / drc_info->data_rate_mbps;
    t_ck = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? ((vco_prd * 2)) : ((vco_prd * 4));
    drc_info->dram_int_param.t_ck = t_ck;

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_ref, &tmp_val));
    drc_info->dram_int_param.c_refi = SOC_SAND_DIV_ROUND_UP(tmp_val, 32);

    
    drc_info->dram_int_param.dqs_prem_1_ck = 1;

    
    switch (drc_info->dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
        drc_info->dram_int_param.c_pcb = 6;
        break;
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
         SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, 3900 , &(drc_info->dram_int_param.c_pcb)));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), drc_info->dram_type));
    }
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): c_pcb = 0x%x\n"), FUNCTION_NAME(), drc_info->dram_int_param.c_pcb));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_ras,    &(drc_info->dram_int_param.c_ras)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 1, t_ck, drc_info->dram_param.t_ras,    &(drc_info->dram_int_param.c_mr_ras)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rp,    &(drc_info->dram_int_param.c_rp)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_wr,    &(drc_info->dram_int_param.c_wr)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 1, t_ck, drc_info->dram_param.t_wr,    &(drc_info->dram_int_param.c_mr_wr)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rtp_l, &(drc_info->dram_int_param.c_rtp_l)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_wtr_l, &(drc_info->dram_int_param.c_wtr_l)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_wtr_s, &(drc_info->dram_int_param.c_wtr_s)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_ccd_l, &(drc_info->dram_int_param.c_ccd_l)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_ccd_s, &(drc_info->dram_int_param.c_ccd_s)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_crc_rd_latency, &(drc_info->dram_int_param.c_crc_rd_latency)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_crc_wr_latency, &(drc_info->dram_int_param.c_crc_wr_latency)));

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_al,    &(drc_info->dram_int_param.c_al)));

    
    drc_info->dram_int_param.c_wl = drc_info->dram_param.c_wr_latency + drc_info->dram_int_param.c_al;

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rfc,   &(drc_info->dram_int_param.c_rfc_f)));

    
    drc_info->dram_int_param.bl = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 4 : 2;

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_wr_recovery_opcode(unit, drc_info->dram_type, drc_info->dram_int_param.c_mr_wr, &(drc_info->dram_int_param.wr_recovery_op)));

    
    drc_info->dram_int_param.burst_chop_en = 0x0;

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_drc_general_register_set(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info, uint32 write_crc) 
{
    uint32
        reg32_val,
        cmd_par_latency_calc,
        field32_val;
    uint64
        reg64_val,
        field64_val;
    static soc_reg_t
        DRC_INITIALIZATION_CONTROLLl[] = {DRCA_INITIALIZATION_CONTROLLr, DRCB_INITIALIZATION_CONTROLLr, DRCC_INITIALIZATION_CONTROLLr, 
                                                       DRCD_INITIALIZATION_CONTROLLr, DRCE_INITIALIZATION_CONTROLLr, DRCF_INITIALIZATION_CONTROLLr, 
                                                       DRCG_INITIALIZATION_CONTROLLr, DRCH_INITIALIZATION_CONTROLLr},
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr},
        DRC_GENERAL_CONFIGURATIONSl[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr, DRCC_GENERAL_CONFIGURATIONSr,
                                         DRCD_GENERAL_CONFIGURATIONSr, DRCE_GENERAL_CONFIGURATIONSr, DRCF_GENERAL_CONFIGURATIONSr,
                                         DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr},
        DRC_DDR_PHY_UPDATE_ENABLEl[] = {DRCA_DDR_PHY_UPDATE_ENABLEr, DRCB_DDR_PHY_UPDATE_ENABLEr, DRCC_DDR_PHY_UPDATE_ENABLEr, DRCD_DDR_PHY_UPDATE_ENABLEr,
                                        DRCE_DDR_PHY_UPDATE_ENABLEr, DRCF_DDR_PHY_UPDATE_ENABLEr, DRCG_DDR_PHY_UPDATE_ENABLEr, DRCH_DDR_PHY_UPDATE_ENABLEr},
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr,
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},                                                                           
        DRC_INIT_SEQUENCE_REGISTERl[] = {DRCA_INIT_SEQUENCE_REGISTERr, DRCB_INIT_SEQUENCE_REGISTERr, DRCC_INIT_SEQUENCE_REGISTERr,
                                         DRCD_INIT_SEQUENCE_REGISTERr, DRCE_INIT_SEQUENCE_REGISTERr, DRCF_INIT_SEQUENCE_REGISTERr,
                                         DRCG_INIT_SEQUENCE_REGISTERr, DRCH_INIT_SEQUENCE_REGISTERr},
        DRC_WRITE_READ_RATESl[] = {DRCA_WRITE_READ_RATESr, DRCB_WRITE_READ_RATESr, DRCC_WRITE_READ_RATESr,
                                   DRCD_WRITE_READ_RATESr, DRCE_WRITE_READ_RATESr, DRCF_WRITE_READ_RATESr,
                                   DRCG_WRITE_READ_RATESr, DRCH_WRITE_READ_RATESr},
        DRC_DRAM_ERROR_RECOVERYl[] = {DRCA_DRAM_ERROR_RECOVERYr, DRCB_DRAM_ERROR_RECOVERYr, DRCC_DRAM_ERROR_RECOVERYr,
                                      DRCD_DRAM_ERROR_RECOVERYr, DRCE_DRAM_ERROR_RECOVERYr, DRCF_DRAM_ERROR_RECOVERYr,
                                      DRCG_DRAM_ERROR_RECOVERYr, DRCH_DRAM_ERROR_RECOVERYr},
        DRC_ODT_CONFIGURATION_REGISTERl[] = {DRCA_ODT_CONFIGURATION_REGISTERr, DRCB_ODT_CONFIGURATION_REGISTERr, DRCC_ODT_CONFIGURATION_REGISTERr,
                                             DRCD_ODT_CONFIGURATION_REGISTERr, DRCE_ODT_CONFIGURATION_REGISTERr, DRCF_ODT_CONFIGURATION_REGISTERr,
                                             DRCG_ODT_CONFIGURATION_REGISTERr, DRCH_ODT_CONFIGURATION_REGISTERr},
        DRC_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGl[] = {DRCA_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, DRCB_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, DRCC_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr,
                                                     DRCD_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, DRCE_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, DRCF_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr,
                                                     DRCG_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, DRCH_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0;
    soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg32_val, ADDR_CMD_ODT_EQ_ZQf, field32_val);
    soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg32_val, AUTO_DRAM_INITf, field32_val);
    soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg32_val, WCK_CTRL_DURING_INIT_ENf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg32_val));
   
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    field32_val = drc_info->gear_down_mode;
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, GEAR_DOWN_MODEf, field32_val);
    if (((drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) && (drc_info->gear_down_mode == 0x0)) ||
        ((SOC_IS_SINAI(unit)) && (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5))                ) {
        field32_val = 1;
    } else {
        field32_val = 0;
    }
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DRC_IN_HALF_DRAM_CMD_FREQ_MODEf, field32_val);
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, UI_2_FSM_ENf, field32_val);
    if (SOC_IS_SINAI(unit))
    {
        field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 0 ;
        soc_reg64_field32_set(unit,DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DATA_X_8_MODEf, field32_val);
    }
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 0 : 1; 
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DRAM_TYPEf, field32_val);
    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 2 : 0;
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, BANK_GROUP_TOPOf, field32_val);
    field32_val = drc_info->dram_int_param.burst_chop_en;
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, BURST_CHOP_ENf, field32_val);
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, BURST_CHOP_RELAX_TIMING_ENf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_GENERAL_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 2 : 0;
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, AP_BIT_POSf, field32_val);
    field32_val = 12 + 21* SOC_SAND_DIV_ROUND_DOWN(drc_info->data_rate_mbps, (drc_info->device_core_freq * 4));
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, REFRESH_DELAY_PRDf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 0;
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, STOP_MMU_PIPE_INSIDE_DPRCf, field32_val);
    field32_val = 2;
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, REFRESH_BURST_SIZEf, field32_val);

    field32_val = 16;
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, SOC_IS_ARDON(unit) ? ITEM_0_4f : CQF_THRESHOLDf, field32_val);

    switch (drc_info->nof_columns) {   
    case 1024:
        field32_val = 2;
        break;
    case 512:
        field32_val = 1;
        break;
    case 256:
        field32_val = 0;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: nof_columns=%d is not supported."), FUNCTION_NAME(), drc_info->nof_columns));
    }
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, NUM_COLSf, field32_val);
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 0 : 1;
    soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg32_val, ABI_BUS_WIDTHf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_GENERAL_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DDR_PHY_UPDATE_ENABLEl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = drc_info->dram_int_param.c_rfc_f - 15;
    soc_reg_field_set(unit, DRCA_DDR_PHY_UPDATE_ENABLEr, &reg32_val, WAIT_AFT_REFRESH_TO_DISABLE_UPDATE_PRDf, field32_val);
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 0;
    soc_reg_field_set(unit, DRCA_DDR_PHY_UPDATE_ENABLEr, &reg32_val, ENABLE_PHY_VDL_UPDATEf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DDR_PHY_UPDATE_ENABLEl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    COMPILER_64_SET(field64_val, 0x0, write_crc);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, WRITE_CRCf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, drc_info->read_crc);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, READ_CRCf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, ((drc_info->read_crc == 0x0) ? 0x0 : drc_info->dram_param.c_cas_latency + drc_info->dram_int_param.c_crc_rd_latency + 1));
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, EDC_RD_LATTENCYf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, ((write_crc == 0x0) ? 0x0 : drc_info->dram_param.c_wr_latency + drc_info->dram_int_param.c_crc_wr_latency + 1));
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, EDC_WR_LATTENCYf, field64_val);
    COMPILER_64_SET(field64_val, 0x0,  drc_info->dram_int_param.dqs_prem_1_ck);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQS_PREAMBLE_1_CKf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, 20);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, ALERT_N_PW_PARITTY_THf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, drc_info->abi);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, ABIf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, drc_info->write_dbi);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, WRITE_DBIf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, drc_info->read_dbi);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, READ_DBIf, field64_val);
    if(drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
        cmd_par_latency_calc = 0;
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_cmd_par_latency_calc(unit, drc_info->dram_type, drc_info->cmd_par_latency, &cmd_par_latency_calc));
    }
    COMPILER_64_SET(field64_val, 0x0, cmd_par_latency_calc);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, COMMAND_PARITY_LATTENCYf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, 0x1);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, ADDR_OEB_POST_CYCLEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, ADDR_OEB_POST_VALUEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, ADDR_OEB_PRE_CYCLEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, ADDR_OEB_PRE_VALUEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQ_OEB_POST_CYCLEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQ_OEB_POST_VALUEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQ_OEB_PRE_CYCLEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQ_OEB_PRE_VALUEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQS_OEB_POST_CYCLEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQS_OEB_POST_VALUEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQS_OEB_PRE_CYCLEf, field64_val);
    soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, DQS_OEB_PRE_VALUEf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INIT_SEQUENCE_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = drc_info->dram_param.init_wait_period;
    soc_reg_field_set(unit, DRCA_INIT_SEQUENCE_REGISTERr, &reg32_val, INIT_WAIT_PRDf, field32_val);
    field32_val = 2;
    soc_reg_field_set(unit, DRCA_INIT_SEQUENCE_REGISTERr, &reg32_val, GEAR_DOWN_SET_UP_PRDf, field32_val);
    soc_reg_field_set(unit, DRCA_INIT_SEQUENCE_REGISTERr, &reg32_val, GEAR_DOWN_HOLD_PRDf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INIT_SEQUENCE_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    COMPILER_64_SET(field64_val, 0x0, ((drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? (drc_info->dram_int_param.c_wl + drc_info->cmd_par_latency) : drc_info->dram_int_param.c_wl));
    soc_reg64_field_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, WR_LATENCYf, field64_val);
    field32_val=0;
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, DEFAULT_ADDR_VALf, field32_val);
    COMPILER_64_SET(field64_val, 0x0, 0x1);
    soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, FORCE_ADDR_OEBf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DRAM_ERROR_RECOVERYl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0;
    soc_reg_field_set(unit, DRCA_DRAM_ERROR_RECOVERYr, &reg32_val, RETRANSMIT_UPPON_WR_CRC_ERRf, field32_val);
    soc_reg_field_set(unit, DRCA_DRAM_ERROR_RECOVERYr, &reg32_val, RETRANSMIT_UPPON_RD_CRC_ERRf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DRAM_ERROR_RECOVERYl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_WRITE_READ_RATESl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = drc_info->dram_int_param.c_ccd_s;
    soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_RD_DIFF_BG_PRDf, field32_val);
    field32_val = drc_info->dram_int_param.c_ccd_l;
    soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_RD_SAME_BG_PRDf, field32_val);
    field32_val = (write_crc && drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? SOC_SAND_MAX(5 ,drc_info->dram_int_param.c_ccd_s) : drc_info->dram_int_param.c_ccd_s;
    soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_WR_DIFF_BG_PRDf, field32_val);
    field32_val = (write_crc && drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? SOC_SAND_MAX(5 ,drc_info->dram_int_param.c_ccd_l) : drc_info->dram_int_param.c_ccd_l;
    soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_WR_SAME_BG_PRDf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_WRITE_READ_RATESl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_ODT_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0;
    soc_reg_field_set(unit, DRCA_ODT_CONFIGURATION_REGISTERr, &reg32_val, DYN_ODT_START_DELAYf, field32_val);
    soc_reg_field_set(unit, DRCA_ODT_CONFIGURATION_REGISTERr, &reg32_val, DYN_ODT_LENGTHf, field32_val);
    soc_reg_field_set(unit, DRCA_ODT_CONFIGURATION_REGISTERr, &reg32_val, DDR_3_ZQ_CALIB_GEN_PRDf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_ODT_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0;
    soc_reg_field_set(unit, DRCA_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, &reg32_val, EVENT_TRIGGER_ON_PARITY_PAD_ENf, field32_val);
    soc_reg_field_set(unit, DRCA_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGr, &reg32_val, EVENT_TRIGGER_ON_PARITY_PAD_OP_CODEf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_EVENT_TRIGGER_ON_PARITY_PAD_CONFIGl[drc_ndx], REG_PORT_ANY, 0, reg32_val));
 

exit:
    SOCDNX_FUNC_RETURN;;
}

int soc_dpp_drc_combo28_drc_ac_op_register_set(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info, uint32 write_crc) 
{
    uint32
        t_ck = drc_info->dram_int_param.t_ck,
        reg32_val,
        field32_val,
        tmp_val;
    uint64
        reg64_val,
        field64_val;
    static soc_reg_t
        DRC_AC_OPERATING_CONDITIONS_1l[] = {DRCA_AC_OPERATING_CONDITIONS_1r, DRCB_AC_OPERATING_CONDITIONS_1r, DRCC_AC_OPERATING_CONDITIONS_1r,
                                            DRCD_AC_OPERATING_CONDITIONS_1r, DRCE_AC_OPERATING_CONDITIONS_1r, DRCF_AC_OPERATING_CONDITIONS_1r,
                                            DRCG_AC_OPERATING_CONDITIONS_1r, DRCH_AC_OPERATING_CONDITIONS_1r},
        DRC_AC_OPERATING_CONDITIONS_2l[] = {DRCA_AC_OPERATING_CONDITIONS_2r, DRCB_AC_OPERATING_CONDITIONS_2r, DRCC_AC_OPERATING_CONDITIONS_2r,
                                            DRCD_AC_OPERATING_CONDITIONS_2r, DRCE_AC_OPERATING_CONDITIONS_2r, DRCF_AC_OPERATING_CONDITIONS_2r,
                                            DRCG_AC_OPERATING_CONDITIONS_2r, DRCH_AC_OPERATING_CONDITIONS_2r},
        DRC_AC_OPERATING_CONDITIONS_3l[] = {DRCA_AC_OPERATING_CONDITIONS_3r, DRCB_AC_OPERATING_CONDITIONS_3r, DRCC_AC_OPERATING_CONDITIONS_3r,
                                            DRCD_AC_OPERATING_CONDITIONS_3r, DRCE_AC_OPERATING_CONDITIONS_3r, DRCF_AC_OPERATING_CONDITIONS_3r,
                                            DRCG_AC_OPERATING_CONDITIONS_3r, DRCH_AC_OPERATING_CONDITIONS_3r},
        DRC_AC_OPERATING_CONDITIONS_4l[] = {DRCA_AC_OPERATING_CONDITIONS_4r, DRCB_AC_OPERATING_CONDITIONS_4r, DRCC_AC_OPERATING_CONDITIONS_4r,
                                            DRCD_AC_OPERATING_CONDITIONS_4r, DRCE_AC_OPERATING_CONDITIONS_4r, DRCF_AC_OPERATING_CONDITIONS_4r,
                                            DRCG_AC_OPERATING_CONDITIONS_4r, DRCH_AC_OPERATING_CONDITIONS_4r},
        DRC_AC_OPERATING_CONDITIONS_5l[] = {DRCA_AC_OPERATING_CONDITIONS_5r, DRCB_AC_OPERATING_CONDITIONS_5r, DRCC_AC_OPERATING_CONDITIONS_5r,
                                            DRCD_AC_OPERATING_CONDITIONS_5r, DRCE_AC_OPERATING_CONDITIONS_5r, DRCF_AC_OPERATING_CONDITIONS_5r,
                                            DRCG_AC_OPERATING_CONDITIONS_5r, DRCH_AC_OPERATING_CONDITIONS_5r};
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_1l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rc, &field32_val));
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg32_val, DD_RT_RCf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rst, &tmp_val));
    field32_val = SOC_SAND_DIV_ROUND_UP(tmp_val, 32) + 20;
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg32_val, DD_RT_RSTf, field32_val);
    field32_val = 0x4;
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg32_val, DD_RT_DLLf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_1l[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rcd_rd, &field32_val));
    COMPILER_64_SET(field64_val, 0x0, field32_val);
    soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RCDRf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rcd_wr, &field32_val));
    COMPILER_64_SET(field64_val, 0x0, field32_val);
    soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RCDWf, field64_val);
    COMPILER_64_SET(field64_val, 0x0, drc_info->dram_int_param.c_rfc_f);
    soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RFCf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rrd_s, &field32_val));
    COMPILER_64_SET(field64_val, 0x0, field32_val);
    soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RRD_DIFF_BGf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_rrd_l, &field32_val));
    COMPILER_64_SET(field64_val, 0x0, field32_val);
    soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RRD_SAME_BGf, field64_val);
    SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_3l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = drc_info->dram_int_param.c_ras - drc_info->dram_int_param.c_rtp_l;
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_RASRDPRDf, field32_val);
    if ( drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4 ) {
        switch (drc_info->dram_int_param.wr_recovery_op) {
        case 0:
            tmp_val = 10;
            break;
        case 1:
            tmp_val = 12;
            break;
        case 2:
            tmp_val = 14;
            break;
        case 3:
            tmp_val = 16;
            break;
        case 4:
            tmp_val = 18;
            break;
        case 5:
            tmp_val = 20;
            break;
        case 6:
            tmp_val = 24;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: wr_recovery_op is out of bound."), FUNCTION_NAME()));
        }
        field32_val = drc_info->dram_int_param.c_wl + drc_info->dram_int_param.bl + drc_info->dram_int_param.c_rp + write_crc + tmp_val;
    } else {
        field32_val = drc_info->dram_int_param.c_wl + drc_info->dram_int_param.bl + drc_info->dram_int_param.c_rp + drc_info->dram_int_param.c_wr;
    }
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_WRAPPRDf, field32_val);
    tmp_val = drc_info->dram_int_param.bl + drc_info->dram_int_param.c_wl + drc_info->dram_int_param.c_wr;
    if (drc_info->dram_int_param.c_ras <= tmp_val) {
        tmp_val = 0x0;
    } else {
        tmp_val = drc_info->dram_int_param.c_ras - tmp_val;
        if ( drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5 ) {
            field32_val = tmp_val + 1;
        } else {
            field32_val = tmp_val - ((write_crc) ? 1:0);
        }
    }    
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_RASWRPRDf, field32_val);
    tmp_val = drc_info->dram_int_param.c_al + drc_info->dram_param.c_cas_latency + drc_info->dram_int_param.bl;
    field32_val = SOC_SAND_MAX(tmp_val, (drc_info->dram_int_param.c_rtp_l + drc_info->dram_int_param.c_rp + ((drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1:0)));
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_RDAPPRDf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_3l[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    field32_val = drc_info->dram_param.c_cas_latency  + drc_info->dram_int_param.bl  - drc_info->dram_int_param.c_wl +  drc_info->dram_int_param.c_pcb + (write_crc && (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4));
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, CNT_RD_WR_PRDf, field32_val);
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, DD_RT_REFIf, drc_info->dram_int_param.c_refi);
    tmp_val = drc_info->dram_int_param.c_wl + drc_info->dram_int_param.bl + drc_info->dram_int_param.c_wtr_s;
    field32_val = tmp_val + ((write_crc) && (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ?  1:0);
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, CNT_WR_RD_DIFF_BG_PRDf, field32_val);
    tmp_val = drc_info->dram_int_param.c_wl + drc_info->dram_int_param.bl + drc_info->dram_int_param.c_wtr_l;
    field32_val = tmp_val + ((write_crc) && (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ?  1:0);
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, CNT_WR_RD_SAME_BG_PRDf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_5l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_faw, &field32_val));
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_5r, &reg32_val, DD_RT_FAWf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_32aw, &field32_val));
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_5r, &reg32_val, DD_RT_32_AWf, field32_val);
    if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) {
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_zqcs, &field32_val));
    } else {
        field32_val = 0x0;
    }
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_5r, &reg32_val, DD_RT_ZQCSf, field32_val);
     if ( drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4 ) {
         SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_util_ps2ck(unit, 0, t_ck, drc_info->dram_param.t_crc_alert, &tmp_val));
         field32_val = 9  + ((drc_info->dram_int_param.bl + write_crc + tmp_val) / 2) + 3 ;
     } else {
         field32_val = 0; 
     }
    soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_5r, &reg32_val, CNT_WR_DATA_ALERT_PRDf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_5l[drc_ndx], REG_PORT_ANY, 0, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;;
}

int soc_dpp_drc_combo28_drc_mr_register_set(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int mr_ndx;
    uint32 reg32_val;
    static soc_reg_t
        DRC_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
        {{DRCA_DRAM_MR_0r, DRCB_DRAM_MR_0r, DRCC_DRAM_MR_0r, DRCD_DRAM_MR_0r, DRCE_DRAM_MR_0r, DRCF_DRAM_MR_0r, DRCG_DRAM_MR_0r, DRCH_DRAM_MR_0r},
         {DRCA_DRAM_MR_1r, DRCB_DRAM_MR_1r, DRCC_DRAM_MR_1r, DRCD_DRAM_MR_1r, DRCE_DRAM_MR_1r, DRCF_DRAM_MR_1r, DRCG_DRAM_MR_1r, DRCH_DRAM_MR_1r},
         {DRCA_DRAM_MR_2r, DRCB_DRAM_MR_2r, DRCC_DRAM_MR_2r, DRCD_DRAM_MR_2r, DRCE_DRAM_MR_2r, DRCF_DRAM_MR_2r, DRCG_DRAM_MR_2r, DRCH_DRAM_MR_2r},
         {DRCA_DRAM_MR_3r, DRCB_DRAM_MR_3r, DRCC_DRAM_MR_3r, DRCD_DRAM_MR_3r, DRCE_DRAM_MR_3r, DRCF_DRAM_MR_3r, DRCG_DRAM_MR_3r, DRCH_DRAM_MR_3r},
         {DRCA_DRAM_MR_4r, DRCB_DRAM_MR_4r, DRCC_DRAM_MR_4r, DRCD_DRAM_MR_4r, DRCE_DRAM_MR_4r, DRCF_DRAM_MR_4r, DRCG_DRAM_MR_4r, DRCH_DRAM_MR_4r},
         {DRCA_DRAM_MR_5r, DRCB_DRAM_MR_5r, DRCC_DRAM_MR_5r, DRCD_DRAM_MR_5r, DRCE_DRAM_MR_5r, DRCF_DRAM_MR_5r, DRCG_DRAM_MR_5r, DRCH_DRAM_MR_5r},
         {DRCA_DRAM_MR_6r, DRCB_DRAM_MR_6r, DRCC_DRAM_MR_6r, DRCD_DRAM_MR_6r, DRCE_DRAM_MR_6r, DRCF_DRAM_MR_6r, DRCG_DRAM_MR_6r, DRCH_DRAM_MR_6r},
         {DRCA_DRAM_MR_7r, DRCB_DRAM_MR_7r, DRCC_DRAM_MR_7r, DRCD_DRAM_MR_7r, DRCE_DRAM_MR_7r, DRCF_DRAM_MR_7r, DRCG_DRAM_MR_7r, DRCH_DRAM_MR_7r},
         {DRCA_DRAM_MR_8r, DRCB_DRAM_MR_8r, DRCC_DRAM_MR_8r, DRCD_DRAM_MR_8r, DRCE_DRAM_MR_8r, DRCF_DRAM_MR_8r, DRCG_DRAM_MR_8r, DRCH_DRAM_MR_8r},
         {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {DRCA_DRAM_MR_12r, DRCB_DRAM_MR_12r, DRCC_DRAM_MR_12r, DRCD_DRAM_MR_12r, DRCE_DRAM_MR_12r, DRCF_DRAM_MR_12r, DRCG_DRAM_MR_12r, DRCH_DRAM_MR_12r},
         {INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
         {DRCA_DRAM_MR_14r, DRCB_DRAM_MR_14r, DRCC_DRAM_MR_14r, DRCD_DRAM_MR_14r, DRCE_DRAM_MR_14r, DRCF_DRAM_MR_14r, DRCG_DRAM_MR_14r, DRCH_DRAM_MR_14r},
         {DRCA_DRAM_MR_15r, DRCB_DRAM_MR_15r, DRCC_DRAM_MR_15r, DRCD_DRAM_MR_15r, DRCE_DRAM_MR_15r, DRCF_DRAM_MR_15r, DRCG_DRAM_MR_15r, DRCH_DRAM_MR_15r}};

    SOCDNX_INIT_FUNC_DEFS;

    for (mr_ndx = 0; mr_ndx < SOC_DPP_DRC_COMBO28_MRS_NUM_MAX; mr_ndx++) {

        if ((mr_ndx == 9) || (mr_ndx == 10) || (mr_ndx == 11) || (mr_ndx == 13)) {
            continue;
        }
        reg32_val = drc_info->mr[drc_ndx][mr_ndx];
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;;
}


int soc_dpp_drc_combo28_drc_bist_timing(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info) 
{
    soc_reg_above_64_val_t
        reg_above64_val,
        field_above64_val;
    uint32 field_val;
    static soc_reg_t 
        DRC_GDDR_5_SPECIAL_CMD_TIMINGl[] = {DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, DRCB_GDDR_5_SPECIAL_CMD_TIMINGr, DRCC_GDDR_5_SPECIAL_CMD_TIMINGr,
                                            DRCD_GDDR_5_SPECIAL_CMD_TIMINGr, DRCE_GDDR_5_SPECIAL_CMD_TIMINGr, DRCF_GDDR_5_SPECIAL_CMD_TIMINGr,
                                            DRCG_GDDR_5_SPECIAL_CMD_TIMINGr, DRCH_GDDR_5_SPECIAL_CMD_TIMINGr};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));
    field_val= drc_info->dram_int_param.c_al + drc_info->dram_param.c_cas_latency + drc_info->dram_int_param.bl - drc_info->dram_int_param.c_wl + drc_info->dram_int_param.c_pcb;
    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    SHR_BITCOPY_RANGE(field_above64_val, 0, &field_val, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, RDTR_2_WRTR_PRDf, field_above64_val);
    SOC_REG_ABOVE_64_CLEAR(field_above64_val);
    field_val= drc_info->dram_int_param.bl + drc_info->dram_int_param.c_wl + drc_info->dram_int_param.c_wtr_s;
    SHR_BITCOPY_RANGE(field_above64_val, 0, &field_val, 0, 32);
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, WRTR_2_RDTR_PRDf, field_above64_val);
    field_above64_val[0]=6;
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, LTLTR_PRDf, field_above64_val);
    field_above64_val[0]=8;
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, LTL_7_TR_PRDf, field_above64_val);  
    field_above64_val[0]=2;
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, RDTR_2_RDTR_PRDf, field_above64_val); 
    field_above64_val[0]=3;
    soc_reg_above_64_field_set(unit, DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, reg_above64_val, WRTR_2_WRTR_PRDf, field_above64_val);         
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGl[drc_ndx], REG_PORT_ANY, 0, reg_above64_val));

exit:
    SOCDNX_FUNC_RETURN;
}
int soc_dpp_drc_combo28_mr_register_info_prepare(int unit, soc_dpp_drc_combo28_info_t *drc_info)
{
    int 
        rv = SOC_E_NONE;
    uint32
        cas_latency_calc,
        mrs_brst_lng,
        ods,
        output_driver_impedance,
        wr_latency_calc,
        ccd_l_calc,
        cmd_par_latency_calc,
        crc_rd_latency_calc,
        crc_wr_latency_calc,
        ras_calc,
        write_crc;
    int drc_ndx;
    uint32 max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_dpp_drc_combo28_util_cas_latency_calc(unit, drc_info->dram_type, drc_info->dram_param.c_cas_latency, &cas_latency_calc);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dpp_drc_combo28_util_wr_latency_calc(unit, drc_info->dram_type, drc_info->dram_param.c_wr_latency, &wr_latency_calc);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dpp_drc_combo28_util_ccd_l_calc(unit, drc_info->dram_type, drc_info->dram_int_param.c_ccd_l, &ccd_l_calc);
    SOCDNX_IF_ERR_EXIT(rv);

    if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
        write_crc = drc_info->write_crc;
    } else {
        write_crc = 0x0;
    }

    SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, drc_ndx) {
        switch (drc_info->dram_type) {   
        case SHMOO_COMBO28_DRAM_TYPE_DDR4:

            mrs_brst_lng = (drc_info->dram_int_param.burst_chop_en == 0) ? 0 : 1;
            output_driver_impedance = soc_property_suffix_num_get(unit, drc_ndx, spn_CUSTOM_FEATURE, "ext_ram_ddr4_odi_dram", 34);
            switch (output_driver_impedance) {
                case 34:
                    ods = 0x0;
                    break;
                case 48:
                    ods = 0x1;
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: given output driver impedance %d is not supported."), FUNCTION_NAME(), output_driver_impedance));
            }

            rv = soc_dpp_drc_combo28_util_cmd_par_latency_calc(unit, drc_info->dram_type, drc_info->cmd_par_latency, &cmd_par_latency_calc);
            SOCDNX_IF_ERR_EXIT(rv);

            
            drc_info->mr[drc_ndx][0]  = 0x0;
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(cas_latency_calc,4,4), 12, 12);  
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(drc_info->dram_int_param.wr_recovery_op, 11, 9);         
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(0x1, 8, 8);                                              
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(0x0, 7, 7);                                              
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(cas_latency_calc,3,1), 6, 4);    
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(0x0, 3, 3);                                              
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(cas_latency_calc,0,0), 2, 2);    
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(mrs_brst_lng, 1, 0);                                     

            
            drc_info->mr[drc_ndx][1]  = 0x0;
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x0, 12, 12);                         
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x0, 11, 11);                         
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x0, 10, 8);                          
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x0, 7, 7);                           
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(drc_info->dram_int_param.c_al, 4, 3); 
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(ods, 2, 1);                           
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x1, 0, 0);                           

            
            drc_info->mr[drc_ndx][2]  = 0x0;
            drc_info->mr[drc_ndx][2] |= SOC_SAND_SET_BITS_RANGE(write_crc, 12, 12); 
            drc_info->mr[drc_ndx][2] |= SOC_SAND_SET_BITS_RANGE(0x0, 10, 9);                                 
            drc_info->mr[drc_ndx][2] |= SOC_SAND_SET_BITS_RANGE(0x0, 7, 6);                                  
            drc_info->mr[drc_ndx][2] |= SOC_SAND_SET_BITS_RANGE(wr_latency_calc, 5, 3);                      

            
            drc_info->mr[drc_ndx][3]  = 0x0; 
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(0x1, 12, 12);               
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(0x0, 8, 6);                      
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(0x1, 5, 5); 
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(drc_info->gear_down_mode, 3, 3); 

            
            drc_info->mr[drc_ndx][4]  = 0x0;
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(drc_info->dram_int_param.dqs_prem_1_ck), 12, 12); 
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(drc_info->dram_int_param.dqs_prem_1_ck), 11, 11); 

            
            drc_info->mr[drc_ndx][5]  = 0x0;
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(drc_info->read_dbi, 12, 12);  
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(drc_info->write_dbi, 11, 11); 
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(0x1, 8, 6);                   
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(cmd_par_latency_calc, 2, 0);  

            
            drc_info->mr[drc_ndx][6]  = 0x0;
            drc_info->mr[drc_ndx][6] |= SOC_SAND_SET_BITS_RANGE(ccd_l_calc, 12, 10); 
            drc_info->mr[drc_ndx][6] |= SOC_SAND_SET_BITS_RANGE(0x10, 5, 0);

            
            drc_info->mr[drc_ndx][7]  = 0x0;
            drc_info->mr[drc_ndx][8]  = 0x0;
            drc_info->mr[drc_ndx][12]  = 0x0;
            drc_info->mr[drc_ndx][14]  = 0x0;
            drc_info->mr[drc_ndx][15]  = 0x0;
          
            break;
        case SHMOO_COMBO28_DRAM_TYPE_GDDR5:

            rv = soc_dpp_drc_combo28_util_crc_rd_latency_calc(unit, drc_info->dram_type, drc_info->dram_int_param.c_crc_rd_latency, &crc_rd_latency_calc);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_dpp_drc_combo28_util_crc_wr_latency_calc(unit, drc_info->dram_type, drc_info->dram_int_param.c_crc_wr_latency, &crc_wr_latency_calc);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_dpp_drc_combo28_util_ras_calc(unit, drc_info->dram_type, drc_info->dram_int_param.c_mr_ras, &ras_calc);
            SOCDNX_IF_ERR_EXIT(rv);

            
            drc_info->mr[drc_ndx][0]  = 0x0;
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(drc_info->dram_int_param.wr_recovery_op, 11, 8); 
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(cas_latency_calc, 6, 3);                         
            drc_info->mr[drc_ndx][0] |= SOC_SAND_SET_BITS_RANGE(wr_latency_calc, 2, 0);                          

            
            drc_info->mr[drc_ndx][1]  = 0x0;
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0, 11, 11);                                            
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(drc_info->abi), 10, 10);     
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(drc_info->write_dbi), 9, 9); 
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(drc_info->read_dbi), 8, 8);  
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x0, 7, 7);                                            
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL(drc_info->dram_auto_calibration_update_disable), 6, 6);  
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x1, 5, 4);                                              
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x1, 3, 2);                                              
            drc_info->mr[drc_ndx][1] |= SOC_SAND_SET_BITS_RANGE(0x0, 1, 0);                                              

            
            drc_info->mr[drc_ndx][2]  = 0x0;

            
            drc_info->mr[drc_ndx][3]  = 0x0;
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(ccd_l_calc, 1, 0), 11, 10); 
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(0x1, 9, 8);                                         
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(0, 5, 5);                                           
            drc_info->mr[drc_ndx][3] |= SOC_SAND_SET_BITS_RANGE(0, 4, 4);                                           

            
            drc_info->mr[drc_ndx][4]  = 0x0;
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(0, 11, 11);                                                             
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(write_crc), 10, 10); 
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_NUM2BOOL_INVERSE(drc_info->read_crc), 9, 9);    
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(crc_rd_latency_calc, 8, 7);                                             
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(crc_wr_latency_calc, 6, 4);                                             
            drc_info->mr[drc_ndx][4] |= SOC_SAND_SET_BITS_RANGE(0xf, 3, 0);                                                             

            
            drc_info->mr[drc_ndx][5]  = 0x0;
            if(drc_info->t_ras_enable){
                drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(ras_calc, 11, 6);  
            }
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(0, 2, 2);          
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(0, 1, 1);          
            drc_info->mr[drc_ndx][5] |= SOC_SAND_SET_BITS_RANGE(0, 0, 0);          

            
            drc_info->mr[drc_ndx][6]  = 0x0;
            drc_info->mr[drc_ndx][6] |= SOC_SAND_SET_BITS_RANGE(5, 11, 8); 
            drc_info->mr[drc_ndx][6] |= SOC_SAND_SET_BITS_RANGE(5, 7, 4);  
            drc_info->mr[drc_ndx][6] |= SOC_SAND_SET_BITS_RANGE(1, 3, 3);  
            drc_info->mr[drc_ndx][6] |= SOC_SAND_SET_BITS_RANGE(0, 0, 0);  

            
            drc_info->mr[drc_ndx][7]  = 0x0;
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(0, 11, 10); 
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(0, 9, 8);   
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(0, 7, 7);   
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(0, 6, 6);   
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(1, 5, 5);   
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(0, 4, 4);   
            drc_info->mr[drc_ndx][7] |= SOC_SAND_SET_BITS_RANGE(0, 3, 3);   

            
            drc_info->mr[drc_ndx][8]  = 0x0;
            drc_info->mr[drc_ndx][8] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(drc_info->dram_int_param.wr_recovery_op ,4,4), 1, 1); 
            drc_info->mr[drc_ndx][8] |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(cas_latency_calc,4,4), 0, 0);                         

            
            drc_info->mr[drc_ndx][12]  = 0x0;

            
            drc_info->mr[drc_ndx][14]  = 0x0;

            
            drc_info->mr[drc_ndx][15]  = 0x0;
            drc_info->mr[drc_ndx][15] |= SOC_SAND_SET_BITS_RANGE(0, 10, 10); 
            drc_info->mr[drc_ndx][15] |= SOC_SAND_SET_BITS_RANGE(0, 9, 9);   
            drc_info->mr[drc_ndx][15] |= SOC_SAND_SET_BITS_RANGE(0, 8, 8);   
            
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), drc_info->dram_type));
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_drc_register_set(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int 
        rv = SOC_E_NONE;
    uint32 
        reg_val,
        write_crc;
    static soc_reg_t 
        DRC_INITIALIZATION_CONTROLLl[] = {DRCA_INITIALIZATION_CONTROLLr, DRCB_INITIALIZATION_CONTROLLr, DRCC_INITIALIZATION_CONTROLLr, 
                                          DRCD_INITIALIZATION_CONTROLLr, DRCE_INITIALIZATION_CONTROLLr, DRCF_INITIALIZATION_CONTROLLr, 
                                          DRCG_INITIALIZATION_CONTROLLr, DRCH_INITIALIZATION_CONTROLLr};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_INITIALIZATION_CONTROLLr, &reg_val, DDR_RSTNf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INITIALIZATION_CONTROLLl[drc_ndx], REG_PORT_ANY, 0, reg_val));
    if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
        write_crc = drc_info->write_crc;
    } else {
        write_crc = 0x0;
    }

    
    rv = soc_dpp_drc_combo28_drc_general_register_set(unit, drc_ndx, drc_info, write_crc);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_dpp_drc_combo28_drc_ac_op_register_set(unit, drc_ndx, drc_info, write_crc);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_dpp_drc_combo28_drc_mr_register_set(unit, drc_ndx, drc_info);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_dpp_drc_combo28_drc_bist_timing(unit, drc_ndx, drc_info);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;;
}

int soc_dpp_src_combo28_gear_down_mode(int unit, int drc_ndx){
    uint32
        reg32_val,
        field32_val;
    uint64
        reg64_val,
        field64_val;
    static soc_reg_t
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr},
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr,
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},                                                                         
        DRC_WRITE_READ_RATESl[] = {DRCA_WRITE_READ_RATESr, DRCB_WRITE_READ_RATESr, DRCC_WRITE_READ_RATESr,
                                   DRCD_WRITE_READ_RATESr, DRCE_WRITE_READ_RATESr, DRCF_WRITE_READ_RATESr,
                                   DRCG_WRITE_READ_RATESr, DRCH_WRITE_READ_RATESr},
        DRC_AC_OPERATING_CONDITIONS_1l[] = {DRCA_AC_OPERATING_CONDITIONS_1r, DRCB_AC_OPERATING_CONDITIONS_1r, DRCC_AC_OPERATING_CONDITIONS_1r,
                                            DRCD_AC_OPERATING_CONDITIONS_1r, DRCE_AC_OPERATING_CONDITIONS_1r, DRCF_AC_OPERATING_CONDITIONS_1r,
                                            DRCG_AC_OPERATING_CONDITIONS_1r, DRCH_AC_OPERATING_CONDITIONS_1r},
        DRC_AC_OPERATING_CONDITIONS_2l[] = {DRCA_AC_OPERATING_CONDITIONS_2r, DRCB_AC_OPERATING_CONDITIONS_2r, DRCC_AC_OPERATING_CONDITIONS_2r,
                                            DRCD_AC_OPERATING_CONDITIONS_2r, DRCE_AC_OPERATING_CONDITIONS_2r, DRCF_AC_OPERATING_CONDITIONS_2r,
                                            DRCG_AC_OPERATING_CONDITIONS_2r, DRCH_AC_OPERATING_CONDITIONS_2r},
        DRC_AC_OPERATING_CONDITIONS_3l[] = {DRCA_AC_OPERATING_CONDITIONS_3r, DRCB_AC_OPERATING_CONDITIONS_3r, DRCC_AC_OPERATING_CONDITIONS_3r,
                                            DRCD_AC_OPERATING_CONDITIONS_3r, DRCE_AC_OPERATING_CONDITIONS_3r, DRCF_AC_OPERATING_CONDITIONS_3r,
                                            DRCG_AC_OPERATING_CONDITIONS_3r, DRCH_AC_OPERATING_CONDITIONS_3r},
        DRC_AC_OPERATING_CONDITIONS_4l[] = {DRCA_AC_OPERATING_CONDITIONS_4r, DRCB_AC_OPERATING_CONDITIONS_4r, DRCC_AC_OPERATING_CONDITIONS_4r,
                                            DRCD_AC_OPERATING_CONDITIONS_4r, DRCE_AC_OPERATING_CONDITIONS_4r, DRCF_AC_OPERATING_CONDITIONS_4r,
                                            DRCG_AC_OPERATING_CONDITIONS_4r, DRCH_AC_OPERATING_CONDITIONS_4r},
        DRC_AC_OPERATING_CONDITIONS_5l[] = {DRCA_AC_OPERATING_CONDITIONS_5r, DRCB_AC_OPERATING_CONDITIONS_5r, DRCC_AC_OPERATING_CONDITIONS_5r,
                                            DRCD_AC_OPERATING_CONDITIONS_5r, DRCE_AC_OPERATING_CONDITIONS_5r, DRCF_AC_OPERATING_CONDITIONS_5r,
                                            DRCG_AC_OPERATING_CONDITIONS_5r, DRCH_AC_OPERATING_CONDITIONS_5r};

    SOCDNX_INIT_FUNC_DEFS;

     SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
     field64_val = soc_reg64_field_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx],reg64_val, READ_DBIf);
     
     COMPILER_64_ADD_32(field64_val, 1);
     COMPILER_64_SHR(field64_val,    1);
     soc_reg64_field_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, READ_DBIf, field64_val);
     SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));    

     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_1l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_1r, reg32_val, DD_RT_DLLf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg32_val, DD_RT_DLLf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_1l[drc_ndx], REG_PORT_ANY, 0, reg32_val));

     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_1l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_1r, reg32_val, DD_RT_RCf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg32_val, DD_RT_RCf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_1l[drc_ndx], REG_PORT_ANY, 0, reg32_val));       
            
     SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
     field64_val = soc_reg64_field_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx],reg64_val, DD_RT_RRD_DIFF_BGf);
     
     COMPILER_64_ADD_32(field64_val, 1);
     COMPILER_64_SHR(field64_val,    1);
     soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RRD_DIFF_BGf, field64_val);

     field64_val = soc_reg64_field_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx],reg64_val, DD_RT_RRD_SAME_BGf);
     
     COMPILER_64_ADD_32(field64_val, 1);
     COMPILER_64_SHR(field64_val,    1);
     soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RRD_SAME_BGf, field64_val);

     field64_val = soc_reg64_field_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx],reg64_val, DD_RT_RFCf);
     
     COMPILER_64_ADD_32(field64_val, 1);
     COMPILER_64_SHR(field64_val,    1);
     soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RFCf, field64_val);

     field64_val = soc_reg64_field_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx],reg64_val, DD_RT_RCDRf);
     
     COMPILER_64_ADD_32(field64_val, 1);
     COMPILER_64_SHR(field64_val,    1);
     soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RCDRf, field64_val);

     field64_val = soc_reg64_field_get(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx],reg64_val, DD_RT_RCDWf);
     
     COMPILER_64_ADD_32(field64_val, 1);
     COMPILER_64_SHR(field64_val,    1);
     soc_reg64_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg64_val, DD_RT_RCDWf, field64_val);
     SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_AC_OPERATING_CONDITIONS_2l[drc_ndx], REG_PORT_ANY, 0, reg64_val)); 
                   
     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_3l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg32_val, CNT_RASRDPRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_RASRDPRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg32_val, CNT_RASWRPRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_RASWRPRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg32_val, CNT_RDAPPRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_RDAPPRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg32_val, CNT_WRAPPRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg32_val, CNT_WRAPPRDf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_3l[drc_ndx], REG_PORT_ANY, 0, reg32_val));
      
     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg32_val, DD_RT_REFIf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, DD_RT_REFIf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg32_val, CNT_WR_RD_DIFF_BG_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, CNT_WR_RD_DIFF_BG_PRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg32_val, CNT_WR_RD_SAME_BG_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, CNT_WR_RD_SAME_BG_PRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg32_val, CNT_RD_WR_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg32_val, CNT_RD_WR_PRDf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_4l[drc_ndx], REG_PORT_ANY, 0, reg32_val)); 
            
     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_AC_OPERATING_CONDITIONS_5l[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_5r, reg32_val, DD_RT_FAWf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_5r, &reg32_val, DD_RT_FAWf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_5r, reg32_val, DD_RT_ZQCSf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_5r, &reg32_val, DD_RT_ZQCSf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_AC_OPERATING_CONDITIONS_5l[drc_ndx], REG_PORT_ANY, 0, reg32_val));

     SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_WRITE_READ_RATESl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
     field32_val = soc_reg_field_get(unit, DRCA_WRITE_READ_RATESr, reg32_val, CNT_WR_DIFF_BG_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_WR_DIFF_BG_PRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_WRITE_READ_RATESr, reg32_val, CNT_WR_SAME_BG_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_WR_SAME_BG_PRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_WRITE_READ_RATESr, reg32_val, CNT_RD_DIFF_BG_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_RD_DIFF_BG_PRDf, field32_val);

     field32_val = soc_reg_field_get(unit, DRCA_WRITE_READ_RATESr, reg32_val, CNT_RD_SAME_BG_PRDf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg32_val, CNT_RD_SAME_BG_PRDf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_WRITE_READ_RATESl[drc_ndx], REG_PORT_ANY, 0, reg32_val));
    
     SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
     field32_val = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, WR_LATENCYf);
     field32_val = SOC_SAND_DIV_ROUND_UP(field32_val, 2);
     soc_reg64_field32_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg64_val, WR_LATENCYf, field32_val);
     SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo16_init_dynamic_calibration(int unit, int drc_ndx)
{
    static soc_reg_t 
        DRC_GENERAL_CONFIGURATIONSr[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr, DRCC_GENERAL_CONFIGURATIONSr, DRCD_GENERAL_CONFIGURATIONSr, 
                                         DRCE_GENERAL_CONFIGURATIONSr, DRCF_GENERAL_CONFIGURATIONSr, DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr },
        DRC_GDDR_5_SPECIAL_CMD_TIMINGr[] = {DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, DRCB_GDDR_5_SPECIAL_CMD_TIMINGr, DRCC_GDDR_5_SPECIAL_CMD_TIMINGr, DRCD_GDDR_5_SPECIAL_CMD_TIMINGr, 
                                           DRCE_GDDR_5_SPECIAL_CMD_TIMINGr, DRCF_GDDR_5_SPECIAL_CMD_TIMINGr, DRCG_GDDR_5_SPECIAL_CMD_TIMINGr, DRCH_GDDR_5_SPECIAL_CMD_TIMINGr },
        DRC_PHY_CALIBRATION_CTRLr[] = {DRCA_PHY_CALIBRATION_CTRLr, DRCB_PHY_CALIBRATION_CTRLr, DRCC_PHY_CALIBRATION_CTRLr, DRCD_PHY_CALIBRATION_CTRLr, 
                                       DRCE_PHY_CALIBRATION_CTRLr, DRCF_PHY_CALIBRATION_CTRLr, DRCG_PHY_CALIBRATION_CTRLr, DRCH_PHY_CALIBRATION_CTRLr },
        DRC_PHY_CALIBRATION_CFGr[] = {DRCA_PHY_CALIBRATION_CFGr, DRCB_PHY_CALIBRATION_CFGr, DRCC_PHY_CALIBRATION_CFGr, DRCD_PHY_CALIBRATION_CFGr, 
                                      DRCE_PHY_CALIBRATION_CFGr, DRCF_PHY_CALIBRATION_CFGr, DRCG_PHY_CALIBRATION_CFGr, DRCH_PHY_CALIBRATION_CFGr };

    soc_reg_above_64_val_t reg_above_64;
    soc_dpp_drc_combo28_info_t *drc_info;
    combo16_step_size_t step_size;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_GENERAL_CONFIGURATIONSr[drc_ndx], REG_PORT_ANY, STOP_MMU_PIPE_INSIDE_DPRCf, 1));

    
    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
    SOCDNX_IF_ERR_EXIT( soc_combo16_calculate_step_size(unit, drc_ndx, &step_size));
    drc_info->ui_size = step_size.size1000UI / 1000;

    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGr[drc_ndx], REG_PORT_ANY, 0, REFRESH_PRDf, 10));
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_GDDR_5_BIST_MODEf, 1);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_OVRD_MODEf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_STARTf, 10);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_OVRD_VDL_2_BIST_ENf, 10);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_BIST_EN_AFTER_FINISHEDf, 64);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_BEFORE_WRITEf, 10);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_REF_TRf, 12);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_PRE_REF_PRDf,2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_init_dynamic_calibration(int unit, int drc_ndx)
{
    static soc_reg_t 
        DRC_GENERAL_CONFIGURATIONSr[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr, DRCC_GENERAL_CONFIGURATIONSr, DRCD_GENERAL_CONFIGURATIONSr, 
                                         DRCE_GENERAL_CONFIGURATIONSr, DRCF_GENERAL_CONFIGURATIONSr, DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr },
        DRC_GDDR_5_SPECIAL_CMD_TIMINGr[] = {DRCA_GDDR_5_SPECIAL_CMD_TIMINGr, DRCB_GDDR_5_SPECIAL_CMD_TIMINGr, DRCC_GDDR_5_SPECIAL_CMD_TIMINGr, DRCD_GDDR_5_SPECIAL_CMD_TIMINGr, 
                                           DRCE_GDDR_5_SPECIAL_CMD_TIMINGr, DRCF_GDDR_5_SPECIAL_CMD_TIMINGr, DRCG_GDDR_5_SPECIAL_CMD_TIMINGr, DRCH_GDDR_5_SPECIAL_CMD_TIMINGr },
        DRC_PHY_CALIBRATION_CTRLr[] = {DRCA_PHY_CALIBRATION_CTRLr, DRCB_PHY_CALIBRATION_CTRLr, DRCC_PHY_CALIBRATION_CTRLr, DRCD_PHY_CALIBRATION_CTRLr, 
                                       DRCE_PHY_CALIBRATION_CTRLr, DRCF_PHY_CALIBRATION_CTRLr, DRCG_PHY_CALIBRATION_CTRLr, DRCH_PHY_CALIBRATION_CTRLr },
        DRC_PHY_CALIBRATION_CFGr[] = {DRCA_PHY_CALIBRATION_CFGr, DRCB_PHY_CALIBRATION_CFGr, DRCC_PHY_CALIBRATION_CFGr, DRCD_PHY_CALIBRATION_CFGr, 
                                      DRCE_PHY_CALIBRATION_CFGr, DRCF_PHY_CALIBRATION_CFGr, DRCG_PHY_CALIBRATION_CFGr, DRCH_PHY_CALIBRATION_CFGr };

    soc_reg_above_64_val_t reg_above_64;
    soc_dpp_drc_combo28_info_t *drc_info;
    combo28_step_size_t step_size;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_GENERAL_CONFIGURATIONSr[drc_ndx], REG_PORT_ANY, STOP_MMU_PIPE_INSIDE_DPRCf, 1));

    
    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
    SOCDNX_IF_ERR_EXIT( soc_combo28_calculate_step_size(unit, drc_ndx, &step_size));
    drc_info->ui_size = step_size.size1000UI / 1000;

    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_GDDR_5_SPECIAL_CMD_TIMINGr[drc_ndx], REG_PORT_ANY, 0, REFRESH_PRDf, 10));
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_GDDR_5_BIST_MODEf, 1);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_OVRD_MODEf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_STARTf, 10);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_OVRD_VDL_2_BIST_ENf, 10);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_BIST_EN_AFTER_FINISHEDf, 64);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_WAIT_BEFORE_WRITEf, 10);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_REF_TRf, 12);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], reg_above_64, CALIB_PRE_REF_PRDf,2);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_PHY_CALIBRATION_CFGr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_init_drc_init(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        if (SOC_IS_SINAI(unit))
        {
            rv = soc_combo16_shmoo_phy_init(unit, drc_ndx);
        }
        else
        {
            rv = soc_combo28_shmoo_phy_init(unit, drc_ndx); 
        }
        SOCDNX_IF_ERR_EXIT(rv); 
    }
    
    
    if (drc_info->dram_clam_shell_mode[drc_ndx] != DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DISABLED) {
        rv = soc_dpp_drc_combo28_drc_clam_shell_cfg(unit, drc_ndx, drc_info); 
        SOCDNX_IF_ERR_EXIT(rv);   
    }

    
    rv = soc_dpp_drc_combo28_drc_dq_swap(unit, drc_ndx, drc_info); 
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    rv = soc_dpp_drc_combo28_drc_addr_bank_swap(unit, drc_ndx, drc_info); 
    SOCDNX_IF_ERR_EXIT(rv);
            
    
    if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
        if (SOC_IS_SINAI(unit))
        {
            soc_dpp_drc_combo16_init_dynamic_calibration(unit, drc_ndx);
        }
        else
        {
            soc_dpp_drc_combo28_init_dynamic_calibration(unit, drc_ndx);
        }
    }

    
    rv = soc_dpp_drc_combo28_drc_soft_init(unit, drc_ndx, 1); 
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    rv = soc_dpp_drc_combo28_drc_register_set(unit, drc_ndx, drc_info); 
    SOCDNX_IF_ERR_EXIT(rv);

    if (1 == drc_info->gear_down_mode) {
        rv = soc_dpp_src_combo28_gear_down_mode(unit, drc_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    rv = soc_dpp_drc_combo28_drc_soft_init(unit, drc_ndx, 0); 
    SOCDNX_IF_ERR_EXIT(rv);
    
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_init_check_bist_result(int unit, int dram_ndx, combo28_bist_err_cnt_t *combo28_bist_err_cnt, soc_dpp_drc_combo28_info_t *drc_info)
{
    int is_gddr5;
    int is_crc;

    SOCDNX_INIT_FUNC_DEFS; 

    SOCDNX_NULL_CHECK(combo28_bist_err_cnt);

    is_gddr5 = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5);
    is_crc   = ((drc_info->write_crc == 1) && (drc_info->read_crc == 1));

    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "\nDRAM index: %u \nResults:\n"), dram_ndx));
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Mapping of bits with errors : 0x%x \n"), combo28_bist_err_cnt->bist_err_occur));
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Number of errors occoured (bist full mask): %d\n"), combo28_bist_err_cnt->bist_full_err_cnt)); 
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Number of errors occoured (bist single mask): %d\n"), combo28_bist_err_cnt->bist_single_err_cnt)); 
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Number of global errors: %d\n"), combo28_bist_err_cnt->bist_global_err_cnt));
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Number of DBI errors: %d\n"), combo28_bist_err_cnt->bist_dbi_global_err_cnt));
    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Mapping of DBI bits with errors : 0x%x \n"), combo28_bist_err_cnt->bist_dbi_err_occur));
    if ((is_gddr5 == 1) && (is_crc == 1))
    {
        LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Number of EDC errors: %d\n"), combo28_bist_err_cnt->bist_edc_global_err_cnt));
        
        LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Mapping of EDC bits with errors : 0x%x \n"), combo28_bist_err_cnt->bist_edc_err_occur));
    }

    
    if ((combo28_bist_err_cnt->bist_err_occur != 0x0)          || (combo28_bist_err_cnt->bist_full_err_cnt != 0x0)   || 
        (combo28_bist_err_cnt->bist_single_err_cnt != 0x0)     || (combo28_bist_err_cnt->bist_global_err_cnt != 0x0) ||
        (combo28_bist_err_cnt->bist_dbi_global_err_cnt != 0x0) || (combo28_bist_err_cnt->bist_dbi_err_occur != 0x0)  || 
        ((is_gddr5 == 1) && (is_crc == 1) && ((combo28_bist_err_cnt->bist_edc_global_err_cnt != 0x0) || (combo28_bist_err_cnt->bist_edc_err_occur != 0x0))))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Error in %s: Error in one of the counters, increase to Verbose and re-run to see prints"), FUNCTION_NAME()));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_init_run_bist_unsafe(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info)
{
    combo28_bist_err_cnt_t combo28_bist_err_cnt;
    SOC_TMC_DRAM_BIST_INFO run_info;

    SOCDNX_INIT_FUNC_DEFS;    

    
    run_info.arad_bist_flags = SOC_TMC_DRAM_BIST_FLAGS_ALL_ADDRESS | SOC_TMC_DRAM_BIST_FLAGS_CONS_ADDR_8_BANKS;
    run_info.bist_start_address = 0x0;
    run_info.bist_end_address = (((drc_info->nof_columns) / 8) * (drc_info->nof_banks) * (drc_info->nof_rows)) - 1;
    run_info.bist_num_actions =  run_info.bist_end_address + 1;
    run_info.pattern_mode = SOC_TMC_DRAM_BIST_DATA_PATTERN_ADDR_MODE;

    
    
    run_info.write_weight = 1;
    run_info.read_weight = 0;
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_bist_test_start(unit, drc_ndx, &run_info, NULL));
    
    run_info.write_weight = 0;
    run_info.read_weight = 1;
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_bist_test_start(unit, drc_ndx, &run_info, NULL));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt(unit, drc_ndx, &combo28_bist_err_cnt));
    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_init_check_bist_result(unit, drc_ndx, &combo28_bist_err_cnt, drc_info));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_init_gddr5_cdr_mechanism(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info)
{
    uint32 field32_val;
    uint64 reg64_val;
    static soc_reg_t 
        DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[] = {DRCA_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCB_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCC_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCD_PERIODIC_PHY_CDR_MONITOR_CFGr,
                                               DRCE_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCF_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCG_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCH_PERIODIC_PHY_CDR_MONITOR_CFGr };
    SOCDNX_INIT_FUNC_DEFS;

    if (drc_info->dram_type != SHMOO_COMBO28_DRAM_TYPE_GDDR5) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Error in %s: dram_type != SHMOO_COMBO28_DRAM_TYPE_GDDR5, dram_type == %d"), FUNCTION_NAME(), drc_info->dram_type));
    }

    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_cdr_ctl(unit, drc_ndx, 1, 1, 1));

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    
    field32_val = 0x10;
    soc_reg64_field32_set(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], &reg64_val, PHY_CDR_GEN_PRDf, field32_val);
    
    field32_val = 0xf;
    soc_reg64_field32_set(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], &reg64_val, PHY_CDR_MONITOR_INTERRUP_THf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], REG_PORT_ANY, 0, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_init_tune_ddr(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info)
{
    int rv = SOC_E_NONE,
        shmoo_type = -1;
    uint32 shmoo_flags = 0x0;


    SOCDNX_INIT_FUNC_DEFS;    


#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        if (drc_info->auto_tune == 0x0)
        {
            LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Restore Configuration. dram ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
            rv = soc_dpp_drc_combo28_shmoo_cfg_get(unit, drc_ndx, &(drc_info->shmoo_config_param[drc_ndx]));
            if (rv == SOC_E_NOT_FOUND) 
            {
                LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Configuration was not found, Shmoo dram ndx=%d. Do nothing\n"), FUNCTION_NAME(), drc_ndx));
                if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_dynamic_calibration_read_direction", 0) ||
                    soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_dynamic_calibration_write_direction", 0)){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("%s(): The dynamic calibration should be prevented and disable if configuration isn't found and shmoo doesn't happen!\n"), FUNCTION_NAME()));
                }
            }
            else
            {
                if (SOC_IS_SINAI(unit))
                {
                    rv = soc_combo16_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RESTORE, (combo16_shmoo_config_param_t*)&(drc_info->shmoo_config_param[drc_ndx]));
                }
                else
                {
                    rv = soc_combo28_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RESTORE, &(drc_info->shmoo_config_param[drc_ndx]));
                }
                SOCDNX_IF_ERR_EXIT(rv);

                rv = soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(unit, drc_info, drc_ndx);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        } else if (drc_info->auto_tune == 0x1)
        {
            LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Shmoo dram ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
            if (SOC_IS_SINAI(unit))
            {
                rv = soc_combo16_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RUN_AND_SAVE, (combo16_shmoo_config_param_t*)&(drc_info->shmoo_config_param[drc_ndx])); 
            }
            else
            {
                rv = soc_combo28_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RUN_AND_SAVE, &(drc_info->shmoo_config_param[drc_ndx])); 
            }
            SOCDNX_IF_ERR_EXIT(rv);            

            LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Save Shmoo Configurations dram ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
            rv = soc_dpp_drc_combo28_shmoo_cfg_set(unit, drc_ndx, &(drc_info->shmoo_config_param[drc_ndx]));
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(unit, drc_info, drc_ndx);
            SOCDNX_IF_ERR_EXIT(rv);
        } else if (drc_info->auto_tune == 0x2) 
        {
            LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Restore Configuration. dram ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
            rv = soc_dpp_drc_combo28_shmoo_cfg_get(unit, drc_ndx, &(drc_info->shmoo_config_param[drc_ndx]));
            if (rv == SOC_E_NOT_FOUND) 
            {
                LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Configuration was not found, Shmoo dram ndx=%d.\n"), FUNCTION_NAME(), drc_ndx));
                if (SOC_IS_SINAI(unit))
                {
                    rv = soc_combo16_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RUN_AND_SAVE, (combo16_shmoo_config_param_t*)&(drc_info->shmoo_config_param[drc_ndx])); 
                }
                else
                {
                    rv = soc_combo28_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RUN_AND_SAVE, &(drc_info->shmoo_config_param[drc_ndx])); 
                }
                SOCDNX_IF_ERR_EXIT(rv);            

                LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Save Shmoo Configurations dram ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
                rv = soc_dpp_drc_combo28_shmoo_cfg_set(unit, drc_ndx, &(drc_info->shmoo_config_param[drc_ndx]));
                SOCDNX_IF_ERR_EXIT(rv);
            }
            else
            {
                if (SOC_IS_SINAI(unit))
                {
                    rv = soc_combo16_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RESTORE, (combo16_shmoo_config_param_t*)&(drc_info->shmoo_config_param[drc_ndx])); 
                }
                else
                {
                    rv = soc_combo28_shmoo_ctl(unit, drc_ndx, shmoo_type, shmoo_flags, SHMOO_COMBO28_ACTION_RESTORE, &(drc_info->shmoo_config_param[drc_ndx])); 
                }
                SOCDNX_IF_ERR_EXIT(rv);
            }

            rv = soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(unit, drc_info, drc_ndx);
            SOCDNX_IF_ERR_EXIT(rv);
        } else if (drc_info->auto_tune == 0x3) 
        {
            LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Skip Shmoo. dram ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_dynamic_calibration_read_direction", 0) ||
                soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_dynamic_calibration_write_direction", 0)){
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("%s(): The dynamic calibration should be prevented and disable if shmoo doesn't happen!\n"), FUNCTION_NAME()));
            }
        }
    }    
    

    
    if (drc_info->bist_enable == 1) {
        rv = soc_dpp_drc_combo28_init_run_bist_unsafe(unit, drc_ndx, drc_info);
        SOCDNX_IF_ERR_EXIT(rv);
    }
exit:
    SOCDNX_FUNC_RETURN;
}  


