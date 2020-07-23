/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DDR4 Memory Support
 */
#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <soc/memtune.h>
#include <soc/iproc.h>


#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifndef BCM_SJC16_SUPPORT
#define BCM_SJC16_SUPPORT
#endif

#ifdef BCM_SJC16_SUPPORT
#include <soc/shmoo_sjc16.h>
#include <soc/phy/phy_sjc16.h>
#include <soc/phy/ydc_ddr_bist.h>
#endif

#ifdef BCM_HELIX5_SUPPORT

static sjc16_shmoo_dram_info_t shmoo_dram_info =
{
    /*.ctl_type =*/ 0,
    /*.dram_type =*/ 0,
    /*.dram_bitmap =*/ 0x00000000,
    /*.interface_bitwidth =*/ 0,
    /*.split_bus =*/ 0,
    /*.num_columns =*/ -1,
    /*.num_rows =*/ -1,
    /*.num_banks =*/ -1,
    /*.num_bank_groups =*/ -1,
    /*.data_rate_mbps =*/ -1,
    /*.ref_clk_mhz =*/ -1,
    /*.refi =*/ 0,
    /*.command_parity_latency =*/ 0,
    /*.sim_system_mode =*/ 0
};

const uint32 shmoo_order_sjc16_ddr4[SHMOO_SJC16_DDR4_ECC_32P4_SEQUENCE_COUNT] =
{
    SHMOO_SJC16_RD_EN_FISH,
    SHMOO_SJC16_RD_EXTENDED,
    SHMOO_SJC16_WR_EXTENDED,
    SHMOO_SJC16_ADDR_CTRL_EXTENDED,
    SHMOO_SJC16_RD_EN_FISH_ECC,
    SHMOO_SJC16_RD_EXTENDED_ECC,
    SHMOO_SJC16_WR_EXTENDED_ECC
};

uint32
_shmoo_sjc16_check_dram(int phy_ndx)
{
    return ((shmoo_dram_info.dram_bitmap >> phy_ndx) & 0x1);
}

STATIC int
_initialize_bist(int unit, int phy_ndx, int bit, sjc16_shmoo_container_t *scPtr)
{
    ydc_ddr_bist_info_t bist_info;

    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            bist_info.write_weight = 0;
            bist_info.read_weight = 1;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 1;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 0;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_RD_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_WR_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_ADDR_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_CTRL_EXTENDED:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            bist_info.write_weight = 0;
            bist_info.read_weight = 1;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 1;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 0;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            bist_info.write_weight = 255;
            bist_info.read_weight = 255;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 51510;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x0FFFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            bist_info.addr_prbs_mode = 0;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    return soc_ydc_ddr_bist_config_set(unit, phy_ndx, &bist_info);
}

STATIC int
_run_bist(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr, sjc16_shmoo_error_array_t *seaPtr)
{
    ydc_ddr_bist_err_cnt_t be;

    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));
            break;
        case SHMOO_SJC16_RD_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info.interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_SJC16_WR_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info.interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info.interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_SJC16_ADDR_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info.interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_SJC16_CTRL_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(shmoo_dram_info.interface_bitwidth == 16)
            {
                (*seaPtr)[0] = (((be.bist_err_occur) >> 16) | (be.bist_err_occur)) & 0xFFFF;
            }
            else
            {
                (*seaPtr)[0] = be.bist_err_occur;
            }
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            /* iproc16_clear_ecc_syndrome(); */
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));
            break;
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            /* iproc16_clear_ecc_syndrome(); */
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(be.bist_err_occur /* | iproc16_read_ecc_syndrome() */)
            {
                (*seaPtr)[0] = 0xFFFFFFFF;
            }
            else
            {
                (*seaPtr)[0] = 0x00000000;
            }
            break;
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            /* iproc16_clear_ecc_syndrome(); */
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(be.bist_err_occur /* | iproc16_read_ecc_syndrome() */)
            {
                (*seaPtr)[0] = 0xFFFFFFFF;
            }
            else
            {
                (*seaPtr)[0] = 0x00000000;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int
_calculate_ui_position(uint32 linearPos, sjc16_shmoo_container_t *scPtr, sjc16_ui_position_t *upPtr)
{
    uint32 ui;
    uint32 position;

    position = linearPos;

    for(ui = 0; ui < SHMOO_SJC16_MAX_EFFECTIVE_UI_COUNT; ui++)
    {
        if(linearPos <= (*scPtr).endUI[ui])
        {
            if(ui != 0)
            {
                position = linearPos - ((*scPtr).endUI[ui - 1] + 1);
            }
            break;
        }
    }

    if(ui == SHMOO_SJC16_MAX_EFFECTIVE_UI_COUNT)
    {
        ui--;
        position = linearPos - ((*scPtr).endUI[ui] + 1);
    }

    (*upPtr).ui = ui;
    (*upPtr).position = position;

    return SOC_E_NONE;
}

STATIC int
_calculate_addr_ui_position(uint32 linearPos, sjc16_shmoo_container_t *scPtr, sjc16_ui_position_t *upPtr)
{
    uint32 ui;
    uint32 position;

    position = linearPos;

    for(ui = 1; ui < SHMOO_SJC16_MAX_EFFECTIVE_UI_COUNT; ui += 2)
    {
        if(linearPos <= (*scPtr).endUI[ui])
        {
            if(ui != 1)
            {
                position = linearPos - ((*scPtr).endUI[ui - 2] + 1);
            }
            break;
        }
    }

    if(ui >= SHMOO_SJC16_MAX_EFFECTIVE_UI_COUNT)
    {
        ui -= 2;
        position = linearPos - ((*scPtr).endUI[ui] + 1);
    }

    (*upPtr).ui = ui >> 1;
    (*upPtr).position = position;

    return SOC_E_NONE;
}

STATIC int
_write_mr(uint32 mr_ndx, uint32 mr_data, uint32 mr_mask)
{
#if defined(CONFIG_HELIX5)
    int unit = 0;
    uint32 val;
    uint32 data;
    uint32 reg_mr_field_lsb;
    uint32 reg_mr_single_field_lsb;
    uint32 reg_mr_field_mask;

    reg_mr_single_field_lsb = 0;
    reg_mr_field_mask = 0x1FFFF;

    switch(mr_ndx)
    {
        case 0:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_46r(unit, &val));
            reg_mr_field_lsb = 8;
            break;
        case 1:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_47r(unit, &val));
            reg_mr_field_lsb = 0;
            break;
        case 2:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_48r(unit, &val));
            reg_mr_field_lsb = 0;
            break;
        case 3:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_50r(unit, &val));
            reg_mr_field_lsb = 0;
            break;
        case 4:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_51r(unit, &val));
            reg_mr_field_lsb = 0;
            break;
        case 5:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_52r(unit, &val));
            reg_mr_field_lsb = 0;
            break;
        case 6:
            SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_53r(unit, &val));
            reg_mr_field_lsb = 0;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported MR register: %d\n"), mr_ndx));
            return SOC_E_FAIL;
    }

    data = (val >> reg_mr_field_lsb) & reg_mr_field_mask;
    data &= ~mr_mask;
    data |= (mr_data & mr_mask);
    val &= ~(reg_mr_field_mask << reg_mr_field_lsb);
    val |= data << reg_mr_field_lsb;

    switch(mr_ndx)
    {
        case 0:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_46r(unit, val));
            break;
        case 1:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_47r(unit, val));
            break;
        case 2:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_48r(unit, val));
            break;
        case 3:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_50r(unit, val));
            break;
        case 4:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_51r(unit, val));
            break;
        case 5:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_52r(unit, val));
            break;
        case 6:
            SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_53r(unit, val));
            break;
    }

    SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_49r(unit, &val));
    val &= ~(reg_mr_field_mask << reg_mr_single_field_lsb);
    val |= data << reg_mr_single_field_lsb;
    SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_49r(unit, val));

    val = 0x01800000 | mr_ndx;
    SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_37r(unit, val));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    val |= 0x02000000;
    SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_37r(unit, val));
    sal_usleep(SHMOO_SJC16_LONG_SLEEP);

    val &= ~(0x02000000);
    SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_37r(unit, val));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
#endif /* CONFIG_HELIX5 */
    return SOC_E_NONE;
}

STATIC int
_reset_dram(sjc16_shmoo_container_t *scPtr)
{
#if defined(CONFIG_HELIX5)
    uint32 i, iMemC, iBist, iPhy;
    int unit = 0;
    uint32 reg_addr;
    uint32 data, reset_ctrl_data, read_fifo_ctrl_data;

    i = (SHMOO_SJC16_RESULT2D_X)*(SHMOO_SJC16_RESULT2D_Y) - 1;
    reg_addr = 0x1023B000;
    iMemC = i;
    soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i]);
    (*scPtr).result2D[i--] &= 0xFFFFFFFE;

    for(reg_addr = 0x1023B004; reg_addr < 0x1023B29C; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    iBist = i;
    for(reg_addr = 0x1023BC00; reg_addr < 0x1023BDF0; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    iPhy = i;
    for(reg_addr = 0x1023C000; reg_addr < 0x1023C144; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023C400; reg_addr < 0x1023C544; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023C800; reg_addr < 0x1023C944; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023CC00; reg_addr < 0x1023CD44; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023D000; reg_addr < 0x1023D144; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023F000; reg_addr < 0x1023F00C; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023F400; reg_addr < 0x1023F428; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023F800; reg_addr < 0x1023F824; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023FC00; reg_addr < 0x1023FC48; reg_addr+=4)
    {
        soc_iproc_getreg(unit, reg_addr, &(*scPtr).result2D[i--]);
    }

    SOC_IF_ERROR_RETURN(WRITE_DDR_S1_IDM_RESET_CONTROLr(unit, 1));
    SOC_IF_ERROR_RETURN(WRITE_DDR_S0_IDM_RESET_CONTROLr(unit, 1));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(WRITE_DDR_S1_IDM_RESET_CONTROLr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_DDR_S0_IDM_RESET_CONTROLr(unit, 0));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    reset_ctrl_data = 0;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, 0));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    read_fifo_ctrl_data = 0;
    DDR_PHY_SET_FIELD(read_fifo_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, READ_FIFO_CTRL, W2R_MIN_DELAY_2, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_READ_FIFO_CTRLr(unit, read_fifo_ctrl_data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, READ_FIFO_RESET_N, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(read_fifo_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, READ_FIFO_CTRL, ENABLE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_READ_FIFO_CTRLr(unit, read_fifo_ctrl_data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    i = iPhy;
    for(reg_addr = 0x1023C000; reg_addr < 0x1023C144; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023C400; reg_addr < 0x1023C544; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023C800; reg_addr < 0x1023C944; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023CC00; reg_addr < 0x1023CD44; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023D000; reg_addr < 0x1023D144; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023F000; reg_addr < 0x1023F00C; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023F400; reg_addr < 0x1023F428; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023F800; reg_addr < 0x1023F824; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    for(reg_addr = 0x1023FC00; reg_addr < 0x1023FC48; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    i = iMemC;

    for(reg_addr = 0x1023B000; reg_addr < 0x1023B29C; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_00r(unit, &data));
    data |= 0x01;
    SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_00r(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_89r(unit, &data));
    while(!(data & (0x800))) {
        SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_89r(unit, &data));
    }

    SOC_IF_ERROR_RETURN(READ_DDR_DENALI_CTL_91r(unit, &data));
    data |= 0x800;
    SOC_IF_ERROR_RETURN(WRITE_DDR_DENALI_CTL_91r(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    i = iBist;
    for(reg_addr = 0x1023BC00; reg_addr < 0x1023BDF0; reg_addr+=4)
    {
        soc_iproc_setreg(unit, reg_addr, (*scPtr).result2D[i--]);
    }

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
#endif

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_rd_en_fish(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 d;
    uint32 data;

    uint32 jump = 3;
    uint32 wp0_pre = 0, wp1_pre = 0, wp2_pre = 0, wp3_pre = 0;
    uint32 wp0_post = 0, wp1_post = 0, wp2_post = 0, wp3_post = 0;
    uint32 sp0_pre = 0xFF, sp1_pre = 0xFF, sp2_pre = 0xFF, sp3_pre = 0xFF;
    uint32 sp0_post = 0xFF, sp1_post = 0xFF, sp2_post = 0xFF, sp3_post = 0xFF;
    uint32 fish0 = 1, fish1 = 1, fish2 = 1, fish3 = 1;

    (*scPtr).engageUIshift = 0;
    (*scPtr).shmooType = SHMOO_SJC16_RD_EN_FISH;

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), phy_ndx));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EN_FISH\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Searching.......: RD_2G_DELAY\n")));

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(d = 0; d < 32; d++)
    {
        if(fish0)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_DDR4_REN_FIFO_CONFIGr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_FIFO_CONFIG, RD_2G_DELAY, d);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_REN_FIFO_CONFIGr(unit, data));
        }
        if(fish1)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_DDR4_REN_FIFO_CONFIGr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, DDR4_REN_FIFO_CONFIG, RD_2G_DELAY, d);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_REN_FIFO_CONFIGr(unit, data));
        }
        if(fish2)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_DDR4_REN_FIFO_CONFIGr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, DDR4_REN_FIFO_CONFIG, RD_2G_DELAY, d);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_REN_FIFO_CONFIGr(unit, data));
        }
        if(fish3)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_DDR4_REN_FIFO_CONFIGr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, DDR4_REN_FIFO_CONFIG, RD_2G_DELAY, d);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_REN_FIFO_CONFIGr(unit, data));
        }

        for(x = 0; x < (((*scPtr).endUI[3] + 1) >> jump); x++)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, x << jump);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));


            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));
            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_STATUS_DDR4_RDATA_FIFOr(unit, &wp0_pre));
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_STATUS_DDR4_RDATA_FIFOr(unit, &wp1_pre));
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_STATUS_DDR4_RDATA_FIFOr(unit, &wp2_pre));
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_STATUS_DDR4_RDATA_FIFOr(unit, &wp3_pre));

            wp0_pre &= 0xF;
            wp1_pre &= 0xF;
            wp2_pre &= 0xF;
            wp3_pre &= 0xF;

            _run_bist(unit, phy_ndx, scPtr, NULL);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_STATUS_DDR4_RDATA_FIFOr(unit, &wp0_post));
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_STATUS_DDR4_RDATA_FIFOr(unit, &wp1_post));
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_STATUS_DDR4_RDATA_FIFOr(unit, &wp2_post));
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_STATUS_DDR4_RDATA_FIFOr(unit, &wp3_post));

            wp0_post &= 0xF;
            wp1_post &= 0xF;
            wp2_post &= 0xF;
            wp3_post &= 0xF;

            if(wp0_post < wp0_pre)
            {
                wp0_post += 0x10;
            }
            if(wp1_post < wp1_pre)
            {
                wp1_post += 0x10;
            }
            if(wp2_post < wp2_pre)
            {
                wp2_post += 0x10;
            }
            if(wp3_post < wp3_pre)
            {
                wp3_post += 0x10;
            }

            if(fish0)
            {
                sp0_pre = wp0_pre;
                sp0_post = wp0_post;

                if(wp0_post - wp0_pre == 4)
                {
                    fish0 = 0;
                }
            }
            if(fish1)
            {
                sp1_pre = wp1_pre;
                sp1_post = wp1_post;

                if(wp1_post - wp1_pre == 4)
                {
                    fish1 = 0;
                }
            }
            if(fish2)
            {
                sp2_pre = wp2_pre;
                sp2_post = wp2_post;

                if(wp2_post - wp2_pre == 4)
                {
                    fish2 = 0;
                }
            }
            if(fish3)
            {
                sp3_pre = wp3_pre;
                sp3_post = wp3_post;

                if(wp3_post - wp3_pre == 4)
                {
                    fish3 = 0;
                }
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Delay %02u........: %01u %01u %01u %01u     PRE: %02u %02u %02u %02u     POST: %02u %02u %02u %02u\n"),
            d, fish3, fish2, fish1, fish0, sp3_pre, sp2_pre, sp1_pre, sp0_pre, sp3_post, sp2_post, sp1_post, sp0_post));

        if(!(fish0 || fish1 || fish2 || fish3))
        {
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_rd_en_fish_ecc(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 d;
    uint32 data;
    uint32 jump = 3;
    uint32 wp4_pre = 0;
    uint32 wp4_post = 0;
    uint32 fish4 = 1;

    (*scPtr).engageUIshift = 0;
    (*scPtr).shmooType = SHMOO_SJC16_RD_EN_FISH;

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), phy_ndx));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EN_FISH_ECC\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Searching.......: RD_2G_DELAY\n")));

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(d = 0; d < 32; d++)
    {
        if(fish4)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_DDR4_REN_FIFO_CONFIGr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_FIFO_CONFIG, RD_2G_DELAY, d);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_REN_FIFO_CONFIGr(unit, data));
        }

        for(x = 0; x < (((*scPtr).endUI[3] + 1) >> jump); x++)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, x << jump);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_STATUS_DDR4_RDATA_FIFOr(unit, &wp4_pre));

            wp4_pre &= 0xF;

            _run_bist(unit, phy_ndx, scPtr, NULL);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_STATUS_DDR4_RDATA_FIFOr(unit, &wp4_post));

            wp4_post &= 0xF;

            if(wp4_post < wp4_pre)
            {
                wp4_post += 0x10;
            }

            if(wp4_post - wp4_pre == 4)
            {
                fish4 = 0;
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Delay %02u........: %01u     PRE: %02u     POST: %02u\n"),
            d, fish4, wp4_pre, wp4_post));

        if(!fish4)
        {
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_wr_extended(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui;
    uint32 position;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 sizeY;
    uint32 xStart;
    int MRfield;
    uint32 data;
    sjc16_shmoo_error_array_t sea;

    yCapMin = 8;
    yCapMax = 71;
    xStart = 0;

    sizeY = yCapMax - yCapMin;
    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[2] + 1;
    (*scPtr).sizeY = sizeY;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_SJC16_WR_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        if(y > SHMOO_SJC16_DDR4_VREF_RANGE_CROSSOVER)
        {   /* Vref Range 1 */
            MRfield = y - 23;
        }
        else
        {   /* Vref Range 2 */
            MRfield = y | 0x40;
        }
        _write_mr(6, MRfield, 0x7F);

        position = 0;
        ui = 0;

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, &data));
            if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_SJC16_LAST_EFFECTIVE_UI))
            {
                ui++;
                position = 0;
            }
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, UI_SHIFT, ui);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, position);
            position++;

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_wr_extended_ecc(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui;
    uint32 position;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 sizeY;
    uint32 xStart;
    uint32 data;
    sjc16_shmoo_error_array_t sea;

    yCapMin = 0;
    yCapMax = 1;
    sizeY = 1;
    xStart = 0;

    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[3] + 1;
    (*scPtr).sizeY = sizeY;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_SJC16_WR_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        position = 0;
        ui = 0;

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, &data));

            if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_SJC16_LAST_EFFECTIVE_UI))
            {
                ui++;
                position = 0;
            }
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, WRITE_MAX_VDL_DATA, UI_SHIFT, ui);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, position);
            position++;

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_rd_extended(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 sizeY;
    uint32 xStart;
    uint32 data;
    sjc16_shmoo_error_array_t sea;

    xStart = 0;

    yCapMin = 42;
    yCapMax = 105;
    (*scPtr).sizeX = (*scPtr).endUI[2] + 1;

    if((*scPtr).debugMode & SHMOO_SJC16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
    {
        yCapMin = 11;
        yCapMax = 126;
    }

    sizeY = yCapMax - yCapMin;
    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = sizeY;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_SJC16_RD_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, &data));

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, VREF_DAC_CONFIG, DATA, y << 1);

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));

        for(x = (*scPtr).calibStart; x < (*scPtr).sizeX; x++)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, x);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));


            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_rd_extended_ecc(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 sizeY;
    uint32 xStart;
    uint32 data;
    sjc16_shmoo_error_array_t sea;

    sizeY = SHMOO_SJC16_MAX_VREF_RANGE;
    xStart = 0;

    yCapMin = 58;
    yCapMax = 89;
    (*scPtr).sizeX = (*scPtr).endUI[3] + 1;

    if((*scPtr).debugMode & SHMOO_SJC16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
    {
        yCapMin = 11;
        yCapMax = 126;
    }

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = sizeY;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_SJC16_RD_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, &data));

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, VREF_DAC_CONFIG, DATA, y << 1);

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, data));

        for(x = (*scPtr).calibStart; x < (*scPtr).sizeX; x++)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, x);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));

            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_addr_ctrl_extended(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui, addr_ui;
    uint32 position;
    uint32 data;
    uint32 prevResult;
    sjc16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[8] + 1;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_SJC16_ADDR_CTRL_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    ui = 0;
    addr_ui = 0;
    position = 0;
    prevResult = 1;

    for(x = (*scPtr).calibStart; x < (*scPtr).sizeX; x++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, &data));
        if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_SJC16_LAST_EFFECTIVE_UI))
        {
            ui++;
            addr_ui = ui >> 1;
            if((addr_ui << 1) == ui)
            {
                position = 0;
            }
        }
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, UI_SHIFT, addr_ui);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, position);
        position++;

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        if(prevResult)
        {
        #if defined(CONFIG_HELIX5)
            _reset_dram(scPtr);
        #endif
        }

        _run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_addr_extended(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui, addr_ui;
    uint32 position;
    uint32 data;
    uint32 prevResult;
    sjc16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[8] + 1;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_SJC16_ADDR_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    ui = 0;
    addr_ui = 0;
    position = 0;
    prevResult = 1;

    for(x = (*scPtr).calibStart; x < (*scPtr).sizeX; x++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, &data));
        if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_SJC16_LAST_EFFECTIVE_UI))
        {
            ui++;
            addr_ui = ui >> 1;
            if((addr_ui << 1) == ui)
            {
                position = 0;
            }
        }
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, UI_SHIFT, addr_ui);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, position);
        position++;

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        if(prevResult)
        {
        #if defined(CONFIG_HELIX5)
            _reset_dram(scPtr);
        #endif
        }

        _run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "x: %03u     Addr UI: %u     Addr Step: %03u     Result: 0x%08X\n"),
            x, addr_ui, position, sea[0]));
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_ctrl_extended(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui, addr_ui;
    uint32 position;
    uint32 data;
    uint32 prevResult;
    sjc16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[8] + 1;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_SJC16_CTRL_EXTENDED;

    _initialize_bist(unit, phy_ndx, -1, scPtr);

    ui = 0 ;
    addr_ui = 0;
    position = 0;
    prevResult = 1;

    for(x = (*scPtr).calibStart; x < (*scPtr).sizeX; x++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, &data));
        if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_SJC16_LAST_EFFECTIVE_UI))
        {
            ui++;
            addr_ui = ui >> 1;
            if((addr_ui << 1) == ui)
            {
                position = 0;
            }
        }
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, UI_SHIFT, addr_ui);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, position);
        position++;

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        if(prevResult)
        {
        #if defined(CONFIG_HELIX5)
            _reset_dram(scPtr);
        #endif
        }

        _run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "x: %03u     Addr UI: %u     Addr Step: %03u     Result: 0x%08X\n"),
            x, addr_ui, position, sea[0]));
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_do(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            return _shmoo_sjc16_rd_en_fish(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_RD_EXTENDED:
            return _shmoo_sjc16_rd_extended(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_WR_EXTENDED:
            return _shmoo_sjc16_wr_extended(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            return _shmoo_sjc16_addr_ctrl_extended(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_ADDR_EXTENDED:
            return _shmoo_sjc16_addr_extended(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_CTRL_EXTENDED:
            if(!SHMOO_SJC16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _shmoo_sjc16_ctrl_extended(unit, phy_ndx, scPtr);
            }
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            return _shmoo_sjc16_rd_en_fish_ecc(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            return _shmoo_sjc16_rd_extended_ecc(unit, phy_ndx, scPtr);
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            return _shmoo_sjc16_wr_extended_ecc(unit, phy_ndx, scPtr);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_calib_2D(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr, uint32 calibMode, uint32 calibPos)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 calibStart;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 i;
    uint32 iter;
    uint32 shiftAmount;
    uint32 dataMask;
    int32 passStart;
    int32 failStart;
    int32 passStartSeen;
    int32 failStartSeen;
    int32 passLength;
    int32 maxPassStart;
    int32 maxPassLength;
    int32 maxMidPointX;
    uint32 maxPassLengthArray[SHMOO_SJC16_WORD];
    uint32 xSum[SHMOO_SJC16_WORD], ySum[SHMOO_SJC16_WORD], passCount[SHMOO_SJC16_WORD];

    xStart = 0;
    sizeX = (*scPtr).sizeX;
    calibStart = (*scPtr).calibStart;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;

    switch(calibMode)
    {
        case SHMOO_SJC16_BIT:
            iter = shmoo_dram_info.interface_bitwidth;
            shiftAmount = 0;
            dataMask = 0x1;
            break;
        case SHMOO_SJC16_BYTE:
            iter = shmoo_dram_info.interface_bitwidth >> 3;
            shiftAmount = 3;
            dataMask = 0xFF;
            break;
        case SHMOO_SJC16_HALFWORD:
            iter = shmoo_dram_info.interface_bitwidth >> 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            break;
        case SHMOO_SJC16_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported 2D calibration mode: %02u\n"), calibMode));
            return SOC_E_FAIL;
    }

    for(i = 0; i < iter; i++)
    {
        (*scPtr).resultData[i] = 0;
        maxPassLengthArray[i] = 0;
        xSum[i] = 0;
        ySum[i] = 0;
        passCount[i] = 0;
    }

    for(y = yCapMin; y < yCapMax; y++)
    {
        for(i = 0; i < iter; i++)
        {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            passStartSeen = -1;
            failStartSeen = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPointX = -2;
            for(x = calibStart; x < sizeX; x++)
            {
                if(((*scPtr).result2D[xStart + x] >> (i << shiftAmount)) & dataMask)
                {   /* FAIL */
                    if(failStart < 0) {
                        failStart = x;
                        if(maxPassLength < passLength)
                        {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                        if((failStartSeen < 0) && (maxPassLength > 0))
                        {
                            failStartSeen = x;
                        }
                    }
                }
                else
                {   /* PASS */
                    xSum[i] += x;
                    ySum[i] += y;
                    passCount[i]++;
                    if(passStart < 0)
                    {
                        passStart = x;
                        passLength = 1;
                        failStart = -1;
                        if((passStartSeen < 0) && (passLength < x))
                        {
                            passStartSeen = x;
                        }
                    }
                    else
                    {
                        passLength++;
                    }

                    if(x == sizeX - 1)
                    {
                        if(maxPassLength < passLength)
                        {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                }
            }

            switch (calibPos) {
                case SHMOO_SJC16_CALIB_FAIL_START:
                case SHMOO_SJC16_CALIB_RISING_EDGE:
                    if(failStartSeen > 0)
                    {
                        maxMidPointX = failStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_SJC16_CALIB_PASS_START:
                case SHMOO_SJC16_CALIB_FALLING_EDGE:
                    if(passStartSeen > 0)
                    {
                        maxMidPointX = passStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_SJC16_CALIB_CENTER_PASS:
                    if((maxPassLength > 0) && (maxPassLengthArray[i] < maxPassLength))
                    {
                        maxMidPointX = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                        maxPassLengthArray[i] = maxPassLength;
                    }
                    break;
                case SHMOO_SJC16_CALIB_VDL_ZERO:
                    maxMidPointX = 0;
                    (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    break;
                case SHMOO_SJC16_CALIB_XY_AVERAGE:
                    if (passCount[i]) {
                        (*scPtr).resultData[i] = ((ySum[i] / passCount[i]) << 16) | ((xSum[i] / passCount[i]) & 0xFFFF);
                    } else {
                        (*scPtr).resultData[i] =  ((ySum[i] << 16) | (xSum[i] & 0xFFFF));
                    }
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration position: %02u\n"), calibPos));
                    return SOC_E_FAIL;
            }
        }
        xStart += sizeX;
    }

    (*scPtr).calibMode = calibMode;
    (*scPtr).calibPos = calibPos;

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_calib_2D(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_BYTE, SHMOO_SJC16_CALIB_CENTER_PASS);
        case SHMOO_SJC16_RD_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_BYTE, SHMOO_SJC16_CALIB_XY_AVERAGE);
        case SHMOO_SJC16_WR_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_BYTE, SHMOO_SJC16_CALIB_XY_AVERAGE);
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD, SHMOO_SJC16_CALIB_CENTER_PASS);
        case SHMOO_SJC16_ADDR_EXTENDED:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD, SHMOO_SJC16_CALIB_CENTER_PASS);
        case SHMOO_SJC16_CTRL_EXTENDED:
            if(!SHMOO_SJC16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD, SHMOO_SJC16_CALIB_CENTER_PASS);
            }
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD, SHMOO_SJC16_CALIB_CENTER_PASS);
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD, SHMOO_SJC16_CALIB_CENTER_PASS);
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            return _calib_2D(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD, SHMOO_SJC16_CALIB_CENTER_PASS);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_set_new_step(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    uint32 calibMode;
    uint32 engageUIshift;
    uint32 MRfield0, MRfield1, MRfield2, MRfield3;
    uint32 data;
    sjc16_ui_position_t up;

    calibMode = (*scPtr).calibMode;
    engageUIshift = (*scPtr).engageUIshift;

    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            break;
        case SHMOO_SJC16_RD_EXTENDED:

            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_SJC16_BYTE:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[1] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[2] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[3] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                case SHMOO_SJC16_HALFWORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));


                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[1] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                case SHMOO_SJC16_WORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            break;
        case SHMOO_SJC16_WR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_SJC16_BYTE:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, &data));

                    if(engageUIshift)
                    {
                        _calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));

                        _calculate_ui_position((*scPtr).resultData[1] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));

                        _calculate_ui_position((*scPtr).resultData[2] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));

                        _calculate_ui_position((*scPtr).resultData[3] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));

                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));

                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));

                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
                    }

                    MRfield0 = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    MRfield1 = ((*scPtr).resultData[1] >> 16) & 0xFFFF;
                    MRfield2 = ((*scPtr).resultData[2] >> 16) & 0xFFFF;
                    MRfield3 = ((*scPtr).resultData[3] >> 16) & 0xFFFF;

                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_MACRO_RESERVED_REGr(unit, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_MACRO_RESERVED_REGr(unit, MRfield1));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_MACRO_RESERVED_REGr(unit, MRfield2));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_MACRO_RESERVED_REGr(unit, MRfield3));

                    MRfield0 = (MRfield0 + MRfield1 + MRfield2 + MRfield3) >> 2;

                    if(MRfield0 > SHMOO_SJC16_DDR4_VREF_RANGE_CROSSOVER)
                    {   /* Vref Range 1 */
                        MRfield0 = MRfield0 - 23;
                    }
                    else
                    {   /* Vref Range 2 */
                        MRfield0 = MRfield0 | 0x40;
                    }
                    _write_mr(6, MRfield0, 0x7F);

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                case SHMOO_SJC16_HALFWORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, &data));

                    if(engageUIshift)
                    {
                        _calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));

                        _calculate_ui_position((*scPtr).resultData[1] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));

                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
                    }

                    MRfield0 = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    MRfield2 = ((*scPtr).resultData[1] >> 16) & 0xFFFF;

                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_MACRO_RESERVED_REGr(unit, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_MACRO_RESERVED_REGr(unit, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_MACRO_RESERVED_REGr(unit, MRfield2));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_MACRO_RESERVED_REGr(unit, MRfield2));

                    MRfield0 = (MRfield0 + MRfield2) >> 1;

                    if(MRfield0 > SHMOO_SJC16_DDR4_VREF_RANGE_CROSSOVER)
                    {   /* Vref Range 1 */
                        MRfield0 = MRfield0 - 23;
                    }
                    else
                    {   /* Vref Range 2 */
                        MRfield0 = MRfield0 | 0x40;
                    }
                    _write_mr(6, MRfield0, 0x7F);

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                case SHMOO_SJC16_WORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, &data));
                    if(engageUIshift)
                    {
                        _calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
                    }

                    MRfield0 = ((*scPtr).resultData[0] >> 16) & 0xFFFF;

                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_MACRO_RESERVED_REGr(unit, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_MACRO_RESERVED_REGr(unit, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_MACRO_RESERVED_REGr(unit, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_MACRO_RESERVED_REGr(unit, MRfield0));

                    if(MRfield0 > SHMOO_SJC16_DDR4_VREF_RANGE_CROSSOVER)
                    {   /* Vref Range 1 */
                        MRfield0 = MRfield0 - 23;
                    }
                    else
                    {   /* Vref Range 2 */
                        MRfield0 = MRfield0 | 0x40;
                    }
                    _write_mr(6, MRfield0, 0x7F);

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                       (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                case SHMOO_SJC16_BYTE:
                case SHMOO_SJC16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_SJC16_WORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, &data));

                    if(engageUIshift)
                    {
                        _calculate_addr_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, up.position);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));

                    #if defined(CONFIG_HELIX5)
                        _reset_dram(scPtr);
                    #endif

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_SJC16_ADDR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                case SHMOO_SJC16_BYTE:
                case SHMOO_SJC16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_SJC16_WORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, &data));

                    if(engageUIshift)
                    {
                        _calculate_addr_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, up.position);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));

                    #if defined(CONFIG_HELIX5)
                        _reset_dram(scPtr);
                    #endif

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_SJC16_CTRL_EXTENDED:
            if(SHMOO_SJC16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
            }
            else
            {
                switch(calibMode)
                {
                    case SHMOO_SJC16_BIT:
                    case SHMOO_SJC16_BYTE:
                    case SHMOO_SJC16_HALFWORD:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                            (*scPtr).shmooType, calibMode));
                        return SOC_E_FAIL;
                    case SHMOO_SJC16_WORD:
                        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, &data));
                        if(engageUIshift)
                        {
                            _calculate_addr_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_CTRL, UI_SHIFT, up.ui);
                            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_CTRL, MAX_VDL_STEP, up.position);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));

                        #if defined(CONFIG_HELIX5)
                            _reset_dram(scPtr);
                        #endif

                        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                            (*scPtr).shmooType, calibMode));
                        return SOC_E_FAIL;
                }
            }
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            break;
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                case SHMOO_SJC16_BYTE:
                case SHMOO_SJC16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_SJC16_WORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, &data));

                    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, VREF_DAC_CONFIG, DATA, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, data));

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            break;
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                case SHMOO_SJC16_BYTE:
                case SHMOO_SJC16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_SJC16_WORD:
                    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, &data));
                    if(engageUIshift)
                    {
                        _calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, WRITE_MAX_VDL_DATA, UI_SHIFT, up.ui);
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, up.position);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, data));
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, data));
                    }

                    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    data = 0;

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    return SOC_E_NONE;
}

STATIC int
_plot(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr, uint32 plotMode)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 yJump;
    uint32 i;
    uint32 ui;
    uint32 iter;
    uint32 shiftAmount;
    uint32 dataMask;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 engageUIshift;
    uint32 step1000;
    uint32 size1000UI;
    uint32 calibShiftAmount;
    uint32 maxMidPointX;
    uint32 maxMidPointY;
    char str0[SHMOO_SJC16_STRING_LENGTH];
    char str1[SHMOO_SJC16_STRING_LENGTH];
    char str2[SHMOO_SJC16_STRING_LENGTH];
    char pass_low[2];
    char fail_high[2];
    char outOfSearch[2];

    outOfSearch[0] = ' ';
    outOfSearch[1] = 0;

    sizeX = (*scPtr).sizeX;
    sizeY = (*scPtr).sizeY;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;
    yJump = (*scPtr).yJump;
    calibMode = (*scPtr).calibMode;
    calibPos = (*scPtr).calibPos;
    calibStart = (*scPtr).calibStart;
    engageUIshift = (*scPtr).engageUIshift;
    step1000 = (*scPtr).step1000;
    size1000UI = (*scPtr).size1000UI;

    switch (calibPos) {
        case SHMOO_SJC16_CALIB_RISING_EDGE:
        case SHMOO_SJC16_CALIB_FALLING_EDGE:
            pass_low[0] = '_';
            pass_low[1] = 0;
            fail_high[0] = '|';
            fail_high[1] = 0;
            break;
        case SHMOO_SJC16_CALIB_CENTER_PASS:
        case SHMOO_SJC16_CALIB_PASS_START:
        case SHMOO_SJC16_CALIB_FAIL_START:
        case SHMOO_SJC16_CALIB_VDL_ZERO:
        case SHMOO_SJC16_CALIB_XY_AVERAGE:
            pass_low[0] = '+';
            pass_low[1] = 0;
            fail_high[0] = '-';
            fail_high[1] = 0;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration position: %02u\n"), calibPos));
            return SOC_E_FAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));

    switch(plotMode)
    {
        case SHMOO_SJC16_BIT:
            iter = shmoo_dram_info.interface_bitwidth;
            shiftAmount = 0;
            dataMask = 0x1;
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_BYTE:
                    calibShiftAmount = 3;
                    break;
                case SHMOO_SJC16_HALFWORD:
                    calibShiftAmount = 4;
                    break;
                case SHMOO_SJC16_WORD:
                    calibShiftAmount = 5;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_SJC16_BYTE:
            iter = shmoo_dram_info.interface_bitwidth >> 3;
            shiftAmount = 3;
            dataMask = 0xFF;
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from byte mode to bit mode\n")));
                    iter = shmoo_dram_info.interface_bitwidth;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_BYTE:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_HALFWORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_SJC16_WORD:
                    calibShiftAmount = 2;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_SJC16_HALFWORD:
            iter = shmoo_dram_info.interface_bitwidth >> 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to byte mode\n")));
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_HALFWORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_WORD:
                    calibShiftAmount = 1;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_SJC16_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to byte mode\n")));
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to halfword mode\n")));
                    iter = 2;
                    shiftAmount = 4;
                    dataMask = 0xFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_SJC16_WORD:
                    calibShiftAmount = 0;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02u\n"), plotMode));
            return SOC_E_FAIL;
    }
    /*
    if(engageUIshift)
    { */
        ui = 0;

        for(x = 0; x < sizeX; x++)
        {
            if((ui < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT) && (x > (*scPtr).endUI[ui]))
            {
                str0[x] = ' ';
                str1[x] = ' ';
                str2[x] = ' ';
                ui++;
            }
            else
            {
                str0[x] = '0' + (x / 100);
                str1[x] = '0' + ((x % 100) / 10);
                str2[x] = '0' + (x % 10);
            }
        }
/*    }
    else
    {
        for(x = 0; x < sizeX; x++)
        {
            str0[x] = '0' + (x / 100);
            str1[x] = '0' + ((x % 100) / 10);
            str2[x] = '0' + (x % 10);
        }
    } */

    str0[x] = 0;
    str1[x] = 0;
    str2[x] = 0;

    for(i = 0; i < iter; i++)
    {
        xStart = 0;
        maxMidPointX = (*scPtr).resultData[i >> calibShiftAmount] & 0xFFFF;
        maxMidPointY = ((*scPtr).resultData[i >> calibShiftAmount] >> 16) & 0xFFFF;

        if((sizeY > 1) || (i == 0))
        {
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), phy_ndx));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** VDL step size...: %3u.%03u ps\n"),
                        (step1000 / 1000), (step1000 % 1000)));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** UI size.........: %3u.%03u steps\n"),
                        (size1000UI / 1000), (size1000UI % 1000)));

            switch((*scPtr).shmooType)
            {
                case SHMOO_SJC16_RD_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EXTENDED\n")));
                    break;
                case SHMOO_SJC16_WR_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_EXTENDED\n")));
                    break;
                case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: ADDR_CTRL_EXTENDED\n")));
                    break;
                case SHMOO_SJC16_ADDR_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: ADDR_EXTENDED\n")));
                    break;
                case SHMOO_SJC16_CTRL_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: CTRL_EXTENDED\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Quick Shmoo.....: Off\n")));
                    break;
                case SHMOO_SJC16_RD_EXTENDED_ECC:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EXTENDED_ECC\n")));
                    break;
                case SHMOO_SJC16_WR_EXTENDED_ECC:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_EXTENDED_ECC\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
                    return SOC_E_FAIL;
            }

            if(engageUIshift)
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** UI shift........: On\n")));
            }
            else
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** UI shift........: Off or N/A\n")));
            }
        }

        if(sizeY > 1)
        {
            switch(calibMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Bit-wise\n")));
                    break;
                case SHMOO_SJC16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Byte-wise\n")));
                    break;
                case SHMOO_SJC16_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Halfword-wise\n")));
                    break;
                case SHMOO_SJC16_WORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Word-wise\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }

            switch(plotMode)
            {
                case SHMOO_SJC16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Bit-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Bit.............: %03u\n"), i));
                    break;
                case SHMOO_SJC16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Byte-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Byte............: %03u\n"), i));
                    break;
                case SHMOO_SJC16_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Halfword-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Halfword........: %03u\n"), i));
                    break;
                case SHMOO_SJC16_WORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Word-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Word............: %03u\n"), i));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02u\n"), plotMode));
                    return SOC_E_FAIL;
            }

            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "    * Center X........: %03u\n"), maxMidPointX));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "    * Center Y........: %03u\n"), maxMidPointY));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str0));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str1));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str2));

            for(y = yCapMin; y < yCapMax; y++)
            {

                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  %03u "), y << yJump));

                for(x = 0; x < calibStart; x++)
                {
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), outOfSearch));
                }

                for(x = calibStart; x < sizeX; x++)
                {
                    if(((*scPtr).result2D[xStart + x] >> (i << shiftAmount)) & dataMask)
                    {   /* FAIL - RISING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR FAIL */
                            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                        }
                        else
                        {   /* FAIL - RISING EDGE */
                            if((calibPos == SHMOO_SJC16_CALIB_RISING_EDGE) || (calibPos == SHMOO_SJC16_CALIB_FAIL_START))
                            {   /* RISING EDGE */
                                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                            }
                            else
                            {   /* FAIL */
                                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                            }
                        }
                    }
                    else
                    {   /* PASS - MIDPOINT - FALLING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR PASS */
                            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), pass_low));
                        }
                        else
                        {   /* POTENTIAL MIDPOINT - FALLING EDGE */
                            if(y == maxMidPointY)
                            {   /* MID POINT - FALLING EDGE */
                                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                            }
                            else
                            {   /* PASS */
                                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), pass_low));
                            }
                        }
                    }
                }
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
                xStart += sizeX;
            }
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        }
        else
        {
            if(i == 0)
            {
                switch(calibMode)
                {
                    case SHMOO_SJC16_BIT:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_SJC16_BYTE:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_SJC16_HALFWORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_SJC16_WORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                        return SOC_E_FAIL;
                }

                switch(plotMode)
                {
                    case SHMOO_SJC16_BIT:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_SJC16_BYTE:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_SJC16_HALFWORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_SJC16_WORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02u\n"), plotMode));
                        return SOC_E_FAIL;
                }
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str0));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str1));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str2));
            }

            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  %03u "), i));

            for(x = 0; x < calibStart; x++)
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), outOfSearch));
            }

            for(x = calibStart; x < sizeX; x++)
            {
                if(((*scPtr).result2D[x] >> (i << shiftAmount)) & dataMask)
                {   /* FAIL - RISING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR FAIL */
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                    }
                    else
                    {   /* FAIL - RISING EDGE */
                        if((calibPos == SHMOO_SJC16_CALIB_RISING_EDGE) || (calibPos == SHMOO_SJC16_CALIB_FAIL_START))
                        {   /* RISING EDGE */
                            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                        }
                        else
                        {   /* FAIL */
                            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                        }
                    }
                }
                else
                {   /* PASS - MIDPOINT - FALLING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR PASS */
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), pass_low));
                    }
                    else
                    {   /* MID POINT - FALLING EDGE */
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_plot(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            break;
        case SHMOO_SJC16_RD_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_BYTE);
        case SHMOO_SJC16_WR_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_BYTE);
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD);
        case SHMOO_SJC16_ADDR_EXTENDED:
            return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD);
        case SHMOO_SJC16_CTRL_EXTENDED:
            if(!SHMOO_SJC16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD);
            }
            else
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), phy_ndx));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: CTRL_EXTENDED\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Quick Shmoo.....: On\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot............: Off\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Copying.........: VDL_STEP\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** From............: AD00 - AD09\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** To..............: AD10 - WE_N\n")));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
            }
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            break;
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD);
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            return _plot(unit, phy_ndx, scPtr, SHMOO_SJC16_WORD);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

int
_sjc16_calculate_step_size(int unit, int phy_ndx, sjc16_step_size_t *ssPtr)
{
    uint32 data;
    uint32 obs_interval, ro_vdl_step1, ro_vdl_step2, ucount1, ucount2;
    uint32 reset_ctrl_data;
    uint32 fld_dcount;
    uint32 fld_ro_overflow;
    uint32 timeout;

    if(shmoo_dram_info.sim_system_mode)
    {
        (*ssPtr).step1000 = 1953;
        (*ssPtr).size1000UI = 128000;

        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

    data = 0;
    obs_interval = 2000;
    ro_vdl_step1 = 128;
    ro_vdl_step2 = 256;
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, OBS_INTERVAL, obs_interval);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, SEL_FC_REFCLK, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &reset_ctrl_data));
    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_FC_RESET_N, 0);
    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_RO_RESET_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, START_OBS, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, EN_NLDL_CLKOUT_BAR, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, RO_VDL_STEP, ro_vdl_step1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, EN_NLDL_CLKOUT_BAR, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_FC_RESET_N, 1);
    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_RO_RESET_N, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, START_OBS, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    timeout = 2000;
    do
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_STATUS_FREQ_CNTR_DCOUNTr(unit, &data));

        fld_dcount = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, STATUS_FREQ_CNTR_DCOUNT, DCOUNT);
        if(!fld_dcount)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr(unit, &data));
            fld_ro_overflow = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, STATUS_FREQ_CNTR_UCOUNT, RO_OVERFLOW);
            if(fld_ro_overflow & 0x4)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL step size computation rollover during first pass\n")));
                return SOC_E_FAIL;
            }
            ucount1 = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, STATUS_FREQ_CNTR_UCOUNT, RO_UCOUNT);
            break;
        }

        if(timeout == 0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Timed out waiting for first pass of VDL step size computation\n")));
            return SOC_E_TIMEOUT;
        }

        timeout--;
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
    }
    while(TRUE);

    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_FC_RESET_N, 0);
    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_RO_RESET_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, START_OBS, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, EN_NLDL_CLKOUT_BAR, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, RO_VDL_STEP, ro_vdl_step2);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, EN_NLDL_CLKOUT_BAR, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_FC_RESET_N, 1);
    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_RO_RESET_N, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, START_OBS, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    timeout = 2000;
    do
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_STATUS_FREQ_CNTR_DCOUNTr(unit, &data));

        fld_dcount = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, STATUS_FREQ_CNTR_DCOUNT, DCOUNT);
        if(!fld_dcount)
        {
            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr(unit, &data));
            fld_ro_overflow = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, STATUS_FREQ_CNTR_UCOUNT, RO_OVERFLOW);
            if(fld_ro_overflow & 0x4)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL step size computation rollover during second pass\n")));
                return SOC_E_FAIL;
            }
            ucount2 = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, STATUS_FREQ_CNTR_UCOUNT, RO_UCOUNT);
            break;
        }

        if(timeout == 0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Timed out waiting for second pass of VDL step size computation\n")));
            return SOC_E_TIMEOUT;
        }

        timeout--;
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
    }
    while(TRUE);

    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_FC_RESET_N, 0);
    DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, FREQ_CNTR_RO_RESET_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, START_OBS, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, FREQ_CNTR_CONFIG, EN_NLDL_CLKOUT_BAR, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    /* BEGIN: Adjustment */
    (*ssPtr).step1000 = (((((((1140000000 << (0x1 & (shmoo_dram_info.dram_type != SHMOO_SJC16_DRAM_TYPE_DDR4))) / (ro_vdl_step2 - ro_vdl_step1)) << 3) / shmoo_dram_info.data_rate_mbps) * obs_interval) / ucount2) * (ucount1 - ucount2)) / ucount1;
    /* END:   Adjustment */
    (*ssPtr).size1000UI = (((1000000000 / shmoo_dram_info.data_rate_mbps) * 1000) / ((*ssPtr).step1000)) >> (0x1 & (shmoo_dram_info.dram_type == SHMOO_SJC16_DRAM_TYPE_DDR4));

    return SOC_E_NONE;
}

int
_sjc16_zq_calibration(int unit, int phy_ndx)
{
    uint32 data;
    uint32 setting;
    uint32 error, status;
    uint32 timeout;
    uint32 fld_comp_done;
    uint32 temp1, temp2;
    uint32 dq_pd, dq_nd, dq_pterm, dq_nterm;
    uint32 aq_pd, aq_nd, aq_pterm, aq_nterm;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, &data));

    setting = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, SHARED_VREF_DAC_CONFIG, DATA);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, SHARED_VREF_DAC_CONFIG, DATA, 0x42);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_RESCAL_INIT_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, RESCAL_INIT_CONFIG, COMP_INIT_FDEPTH, 0xB);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, RESCAL_INIT_CONFIG, PNCOMP_INIT_DIFF, 0x1F);     /* TEMPORARY */
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_RESCAL_INIT_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RESCAL_RESET_N, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_RESCAL_OPERATION_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, RESCAL_OPERATION_CONFIG, COMP_EN, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_RESCAL_OPERATION_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    error = 0;
    status = 0;
    timeout = 2000;
    do
    {
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_STATUS_RESCALr(unit, &data));

        fld_comp_done = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_COMMON, STATUS_RESCAL, COMP_DONE);
        if(fld_comp_done)
        {
            status = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_COMMON, STATUS_RESCAL, COMP_ERROR);
            error = status & 0x1C;
            if(error)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: ZQ calibration error - Manual IO programming may be required for correct operation\n")));
                error |= 0x10020;
            }
            break;
        }

        if(timeout == 0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Timed out waiting for ZQ calibration\n")));
            return SOC_E_TIMEOUT;
        }

        timeout--;
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
    }
    while(TRUE);

    temp1 = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_COMMON, STATUS_RESCAL, PCOMP_CODE_2CORE);
    temp2 = DDR_PHY_GET_FIELD(data, SJC16_DDR4_PHY_COMMON, STATUS_RESCAL, NCOMP_CODE_2CORE);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_RESCAL_OPERATION_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, RESCAL_OPERATION_CONFIG, COMP_EN, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_RESCAL_OPERATION_CONFIGr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RESCAL_RESET_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, SHARED_VREF_DAC_CONFIG, DATA, setting);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, data));

    dq_pd = temp1;
    dq_nd = temp2;
    dq_pterm = temp1;
    dq_nterm = temp2;
    aq_pd = temp1;
    aq_nd = temp2;
    aq_pterm = temp1;
    aq_nterm = temp2;

    if((dq_pd | dq_nd | dq_pterm | dq_nterm | aq_pd | aq_pterm | aq_nterm) & 0xFFFFFFE0)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: ZQ calibration out of bounds - Manual IO programming may be required for correct operation\n")));
        error |= 0x40000;
    }

    if(error)
    {
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Status Code......................: 0x%02X\n"), status));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Pdrive.................: 0x%02X\n"), 0xFF & dq_pd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Ndrive.................: 0x%02X\n"), 0xFF & dq_nd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Ptermination...........: 0x%02X\n"), 0xFF & dq_pterm));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Ntermination (RFU).....: 0x%02X\n"), 0xFF & dq_nterm));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Pdrive........................: 0x%02X\n"), 0xFF & aq_pd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Ndrive........................: 0x%02X\n"), 0xFF & aq_nd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Ptermination..................: 0x%02X\n"), 0xFF & aq_pterm));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Ntermination (RFU)............: 0x%02X\n"), 0xFF & aq_nterm));

        if(error & 0x40000)
        {
            dq_pd &= 0x1F;
            dq_nd &= 0x1F;
            dq_pterm &= 0x1F;
            dq_nterm &= 0x1F;
            aq_pd &= 0x1F;
            aq_nd &= 0x1F;
            aq_pterm &= 0x1F;
            aq_nterm &= 0x1F;
        }

        /* return SOC_E_FAIL; */
    }
    else
    {
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Status Code......................: 0x%02X\n"), status));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Pdrive.................: 0x%02X\n"), 0xFF & dq_pd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Ndrive.................: 0x%02X\n"), 0xFF & dq_nd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Ptermination...........: 0x%02X\n"), 0xFF & dq_pterm));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DQ/DQS/CK Ntermination (RFU).....: 0x%02X\n"), 0xFF & dq_nterm));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Pdrive........................: 0x%02X\n"), 0xFF & aq_pd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Ndrive........................: 0x%02X\n"), 0xFF & aq_nd));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Ptermination..................: 0x%02X\n"), 0xFF & aq_pterm));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "AQ Ntermination (RFU)............: 0x%02X\n"), 0xFF & aq_nterm));
    }

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_IO_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, PD, dq_pd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, PTERM, dq_pterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, ND, dq_nd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, NTERM, dq_nterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, PULL_UP_OFF_B, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, RX_BIAS, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, PEAK, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, DEM, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, IO_CONFIG, RXENB, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_IO_CONFIGr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_IO_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, PD, aq_pd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, PTERM, aq_pterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, ND, aq_nd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, NTERM, aq_nterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, PULL_UP_OFF_B, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, RX_BIAS, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, PEAK, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, DEM, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, RXENB, 0x1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, IO_CONFIG, RXENB_ALERT_N, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_IO_CONFIGr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_SUPPLEMENT_IO_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, PD, aq_pd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, PTERM, aq_pterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, ND, aq_nd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, NTERM, aq_nterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, PULL_UP_OFF_B, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, RX_BIAS, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, PEAK, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, DEM, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, RXENB_ALERT_N_2, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, RXENB_ALERT_N_3, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_SUPPLEMENT, IO_CONFIG, RXENB_ALERT_N_4, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_SUPPLEMENT_IO_CONFIGr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_DATA_IO_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, PD, dq_pd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, PTERM, dq_pterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, ND, dq_nd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, NTERM, dq_nterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, PULL_UP_OFF_B, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, RX_BIAS, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, PEAK, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, DEM, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DATA_IO_CONFIG, RXENB, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DATA_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DATA_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DATA_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DATA_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DATA_IO_CONFIGr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_DQS_IO_CONFIGr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, PD, dq_pd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, PTERM, dq_pterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, ND, dq_nd);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, NTERM, dq_nterm);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, PULL_UP_OFF_B, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, RX_BIAS, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, PEAK, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, DEM, 0x0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DQS_IO_CONFIG, RXENB, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DQS_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DQS_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DQS_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DQS_IO_CONFIGr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DQS_IO_CONFIGr(unit, data));
    sal_usleep(SHMOO_SJC16_LONG_SLEEP);

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_entry(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential entry
     * Mode 1: Single entry
     */

    uint32 i;
    uint32 data, temp;
    sjc16_step_size_t ss;

    (*scPtr).calibStart = 0;

    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            data = 0;
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_3, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_2, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_1, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_0, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_PLUS_1, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_PLUS_2, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_REN_STRETCH_CONFIGr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_REN_STRETCH_CONFIGr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_REN_STRETCH_CONFIGr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_REN_STRETCH_CONFIGr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_RD_EXTENDED:
            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_WR_EXTENDED:
            _write_mr(6, 0x80, 0x80);
            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_ADDR_EXTENDED:
            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_CTRL_EXTENDED:
            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            /* iproc16_enable_ecc() */;

            data = 0;
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_MINUS_3, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_MINUS_2, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_MINUS_1, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_0, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_PLUS_1, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_PLUS_2, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_REN_STRETCH_CONFIGr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            _sjc16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_SJC16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_SJC16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_SJC16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_exit(int unit, int phy_ndx, sjc16_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential exit
     * Mode 1: Single exit
     */

    uint32 data;

    switch ((*scPtr).shmooType) {
        case SHMOO_SJC16_RD_EN_FISH:
            data = 0;
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_3, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_2, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_1, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_0, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_PLUS_1, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_PLUS_2, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_REN_STRETCH_CONFIGr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_REN_STRETCH_CONFIGr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_REN_STRETCH_CONFIGr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_REN_STRETCH_CONFIGr(unit, data));
            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
            break;
        case SHMOO_SJC16_RD_EXTENDED:
            break;
        case SHMOO_SJC16_WR_EXTENDED:
            _write_mr(6, 0x00, 0x80);
            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
            break;
        case SHMOO_SJC16_ADDR_CTRL_EXTENDED:
            break;
        case SHMOO_SJC16_ADDR_EXTENDED:
            break;
        case SHMOO_SJC16_CTRL_EXTENDED:
            break;
        case SHMOO_SJC16_RD_EN_FISH_ECC:
            data = 0;
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_MINUS_3, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_MINUS_2, 0);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_MINUS_1, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_0, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_PLUS_1, 1);
            DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, DDR4_REN_STRETCH_CONFIG, A_PLUS_2, 0);
            SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_REN_STRETCH_CONFIGr(unit, data));
            sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
            break;
        case SHMOO_SJC16_RD_EXTENDED_ECC:
            break;
        case SHMOO_SJC16_WR_EXTENDED_ECC:
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_save(int unit, int phy_ndx, sjc16_shmoo_config_param_t *config_param)
{
    uint32 data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[0][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_dbi[0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_edc[0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_data[0] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DQSr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_dqs[0] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[0][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_dbi[0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_edc[0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_dqsp[0] = data;
    /* READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQSDNr(unit, &data)); */
    (*config_param).dq_byte_rd_max_vdl_dqsn[0] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_DDR4_REN_FIFO_CONFIGr(unit, &data));
    (*config_param).dq_byte_ren_fifo_config[0] = data;
    /* READ_DDR4_PHY_DQ_BYTE0_EDCEN_FIFO_CONFIGr(unit, &data)); */
    (*config_param).dq_byte_edcen_fifo_config[0] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_DDR4_READ_MAX_VDL_FSMr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_fsm[0] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, &data));
    (*config_param).dq_byte_vref_dac_config[0] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).dq_byte_macro_reserved_reg[0] = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[1][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_dbi[1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_edc[1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_data[1] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DQSr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_dqs[1] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[1][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_dbi[1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_edc[1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_dqsp[1] = data;
    /* READ_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQSDNr(unit, &data)); */
    (*config_param).dq_byte_rd_max_vdl_dqsn[1] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_DDR4_REN_FIFO_CONFIGr(unit, &data));
    (*config_param).dq_byte_ren_fifo_config[1] = data;
    /* READ_DDR4_PHY_DQ_BYTE1_EDCEN_FIFO_CONFIGr(unit, &data)); */
    (*config_param).dq_byte_edcen_fifo_config[1] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_DDR4_READ_MAX_VDL_FSMr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_fsm[1] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, &data));
    (*config_param).dq_byte_vref_dac_config[1] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE1_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).dq_byte_macro_reserved_reg[1] = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[2][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_dbi[2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_edc[2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_data[2] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DQSr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_dqs[2] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[2][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_dbi[2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_edc[2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_dqsp[2] = data;
    /* 	READ_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQSNr(unit, &data)); */
    (*config_param).dq_byte_rd_max_vdl_dqsn[2] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_DDR4_REN_FIFO_CONFIGr(unit, &data));
    (*config_param).dq_byte_ren_fifo_config[2] = data;
    /* 	READ_DDR4_PHY_DQ_BYTE2_EDCEN_FIFO_CONFIGr(unit, &data)); */
    (*config_param).dq_byte_edcen_fifo_config[2] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_DDR4_READ_MAX_VDL_FSMr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_fsm[2] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, &data));
    (*config_param).dq_byte_vref_dac_config[2] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE2_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).dq_byte_macro_reserved_reg[2] = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[3][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_dbi[3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_edc[3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_data[3] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DQSr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_dqs[3] = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[3][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_dbi[3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_edc[3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_dqsp[3] = data;
    /* READ_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQSNr(unit, &data)); */
    (*config_param).dq_byte_rd_max_vdl_dqsn[3] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_DDR4_REN_FIFO_CONFIGr(unit, &data));
    (*config_param).dq_byte_ren_fifo_config[3] = data;
    /* READ_DDR4_PHY_DQ_BYTE3_EDCEN_FIFO_CONFIGr(unit, &data)); */
    (*config_param).dq_byte_edcen_fifo_config[3] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_DDR4_READ_MAX_VDL_FSMr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_fsm[3] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, &data));
    (*config_param).dq_byte_vref_dac_config[3] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE3_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).dq_byte_macro_reserved_reg[3] = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_bit[4][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_dbi[4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_wr_min_vdl_edc[4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_data[4] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DQSr(unit, &data));
    (*config_param).dq_byte_wr_max_vdl_dqs[4] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT0r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][0] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT1r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][1] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT2r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][2] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT3r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][3] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT4r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT5r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][5] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT6r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][6] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT7r(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_bit[4][7] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_DBIr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_dbi[4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_EDCr(unit, &data));
    (*config_param).dq_byte_rd_min_vdl_edc[4] = (uint8) data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_dqsp[4] = data;
    /* READ_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQSNr(unit, &data)); */
    (*config_param).dq_byte_rd_max_vdl_dqsn[4] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_DDR4_REN_FIFO_CONFIGr(unit, &data));
    (*config_param).dq_byte_ren_fifo_config[4] = data;
    /* READ_DDR4_PHY_DQ_BYTE4_EDCEN_FIFO_CONFIGr(unit, &data)); */
    (*config_param).dq_byte_edcen_fifo_config[4] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_DDR4_READ_MAX_VDL_FSMr(unit, &data));
    (*config_param).dq_byte_rd_max_vdl_fsm[4] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, &data));
    (*config_param).dq_byte_vref_dac_config[4] = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE4_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).dq_byte_macro_reserved_reg[4] = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, &data));
    (*config_param).aq_l_max_vdl_addr = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, &data));
    (*config_param).aq_l_max_vdl_ctrl = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).aq_l_macro_reserved_reg = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, &data));
    (*config_param).aq_u_max_vdl_addr = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, &data));
    (*config_param).aq_u_max_vdl_ctrl = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_U_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).aq_u_macro_reserved_reg = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_MACRO_RESERVED_REGr(unit, &data));
    (*config_param).common_macro_reserved_reg = data;

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_READ_CLOCK_CONFIGr(unit, &data));
    (*config_param).control_regs_read_clock_config = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_INPUT_SHIFT_CTRLr(unit, &data));
    (*config_param).control_regs_input_shift_ctrl = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
    (*config_param).control_regs_ren_fifo_central_init = data;
    /* READ_DDR4_PHY_CONTROL_REGS_EDCEN_FIFO_CENTRAL_INITIALIZERr(unit, &data)); */
    (*config_param).control_regs_edcen_fifo_central_init = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, &data));
    (*config_param).control_regs_shared_vref_dac_config = data;
    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESERVED_REGr(unit, &data));
    (*config_param).control_regs_reserved_reg = data;

    return SOC_E_NONE;
}

STATIC int
_shmoo_sjc16_restore(int unit, int phy_ndx, sjc16_shmoo_config_param_t *config_param)
{
    uint32 data;
    uint32 MRfield0, MRfield1, MRfield2, MRfield3;

    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[0][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_dbi[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_edc[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_data[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_dqs[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DQSr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[0][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_dbi[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_edc[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_rd_max_vdl_dqsp[0];
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

    /* data = (*config_param).dq_byte_rd_max_vdl_dqsn[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQSDNr(unit, data)); */

    data = (*config_param).dq_byte_ren_fifo_config[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_REN_FIFO_CONFIGr(unit, data));
    /* data = (*config_param).dq_byte_edcen_fifo_config[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_EDCEN_FIFO_CONFIGr(unit, data)); */

    data = (*config_param).dq_byte_rd_max_vdl_fsm[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_READ_MAX_VDL_FSMr(unit, data));
    data = (*config_param).dq_byte_vref_dac_config[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
    data = (*config_param).dq_byte_macro_reserved_reg[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_MACRO_RESERVED_REGr(unit, data));

    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[1][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_dbi[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_edc[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_data[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_dqs[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DQSr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[1][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_dbi[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_edc[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_rd_max_vdl_dqsp[1];
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE1, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    /* data = (*config_param).dq_byte_rd_max_vdl_dqsn[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQSNr(unit, data)); */
    data = (*config_param).dq_byte_ren_fifo_config[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_REN_FIFO_CONFIGr(unit, data));
    /* data = (*config_param).dq_byte_edcen_fifo_config[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_EDCEN_FIFO_CONFIGr(unit, data)); */
    data = (*config_param).dq_byte_rd_max_vdl_fsm[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_READ_MAX_VDL_FSMr(unit, data));
    data = (*config_param).dq_byte_vref_dac_config[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));
    data = (*config_param).dq_byte_macro_reserved_reg[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_MACRO_RESERVED_REGr(unit, data));

    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[2][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_dbi[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_edc[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_data[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_dqs[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DQSr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[2][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_dbi[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_edc[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_rd_max_vdl_dqsp[2];
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE2, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    /* data = (*config_param).dq_byte_rd_max_vdl_dqsn[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQSNr(unit, data)); */
    data = (*config_param).dq_byte_ren_fifo_config[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_REN_FIFO_CONFIGr(unit, data));
    /* data = (*config_param).dq_byte_edcen_fifo_config[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_DDR4_PHY_DQ_BYTE2_EDCEN_FIFO_CONFIGr(unit, data)); */
    data = (*config_param).dq_byte_rd_max_vdl_fsm[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_READ_MAX_VDL_FSMr(unit, data));
    data = (*config_param).dq_byte_vref_dac_config[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
    data = (*config_param).dq_byte_macro_reserved_reg[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_MACRO_RESERVED_REGr(unit, data));

    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[3][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_dbi[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_edc[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_data[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_dqs[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DQSr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[3][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_dbi[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_edc[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_rd_max_vdl_dqsp[3];
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE3, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    /* data = (*config_param).dq_byte_rd_max_vdl_dqsn[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQSNr(unit, data)); */
    data = (*config_param).dq_byte_ren_fifo_config[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_REN_FIFO_CONFIGr(unit, data));
    /* data = (*config_param).dq_byte_edcen_fifo_config[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_EDCEN_FIFO_CONFIGr(unit, data)); */
    data = (*config_param).dq_byte_rd_max_vdl_fsm[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_READ_MAX_VDL_FSMr(unit, data));
    data = (*config_param).dq_byte_vref_dac_config[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));
    data = (*config_param).dq_byte_macro_reserved_reg[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_MACRO_RESERVED_REGr(unit, data));

    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_bit[4][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_dbi[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_wr_min_vdl_edc[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_data[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, data));
    data = (*config_param).dq_byte_wr_max_vdl_dqs[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DQSr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT0r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT1r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT2r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT3r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT4r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT5r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT6r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_bit[4][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT7r(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_dbi[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_DBIr(unit, data));
    data = (uint32) (*config_param).dq_byte_rd_min_vdl_edc[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_EDCr(unit, data));
    data = (*config_param).dq_byte_rd_max_vdl_dqsp[4];
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE4, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
    /* data = (*config_param).dq_byte_rd_max_vdl_dqsn[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQSNr(unit, data)); */
    data = (*config_param).dq_byte_ren_fifo_config[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_REN_FIFO_CONFIGr(unit, data));
    /* data = (*config_param).dq_byte_edcen_fifo_config[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_EDCEN_FIFO_CONFIGr(unit, data)); */
    data = (*config_param).dq_byte_rd_max_vdl_fsm[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_READ_MAX_VDL_FSMr(unit, data));
    data = (*config_param).dq_byte_vref_dac_config[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, data));
    data = (*config_param).dq_byte_macro_reserved_reg[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_MACRO_RESERVED_REGr(unit, data));

    data = (*config_param).aq_l_max_vdl_addr;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
    data = (*config_param).aq_l_max_vdl_ctrl;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
    data = (*config_param).aq_l_macro_reserved_reg;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MACRO_RESERVED_REGr(unit, data));
    data = (*config_param).aq_u_max_vdl_addr;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));
    data = (*config_param).aq_u_max_vdl_ctrl;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));
    data = (*config_param).aq_u_macro_reserved_reg;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MACRO_RESERVED_REGr(unit, data));

    data = (*config_param).common_macro_reserved_reg;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_MACRO_RESERVED_REGr(unit, data));

    data = (*config_param).control_regs_read_clock_config;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_READ_CLOCK_CONFIGr(unit, data));
    data = (*config_param).control_regs_input_shift_ctrl;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_INPUT_SHIFT_CTRLr(unit, data));
    data = (*config_param).control_regs_ren_fifo_central_init;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));
    /* data = (*config_param).control_regs_edcen_fifo_central_init;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_EDCEN_FIFO_CENTRAL_INITIALIZERr(unit, data)); */
    data = (*config_param).control_regs_shared_vref_dac_config;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, data));
    data = (*config_param).control_regs_reserved_reg;
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESERVED_REGr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, data));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
    DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

    sal_usleep(SHMOO_SJC16_DEEP_SLEEP);

    MRfield0 = (*config_param).dq_byte_macro_reserved_reg[0];
    MRfield1 = (*config_param).dq_byte_macro_reserved_reg[1];
    MRfield2 = (*config_param).dq_byte_macro_reserved_reg[2];
    MRfield3 = (*config_param).dq_byte_macro_reserved_reg[3];

    MRfield0 = (MRfield0 + MRfield1 + MRfield2 + MRfield3) >> 2;

    if(MRfield0 > SHMOO_SJC16_DDR4_VREF_RANGE_CROSSOVER)
    {   /* Vref Range 1 */
        MRfield0 = MRfield0 - 23;
    }
    else
    {   /* Vref Range 2 */
        MRfield0 = MRfield0 | 0x40;
    }
    _write_mr(6, MRfield0, 0x7F);

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_sjc16_shmoo_ctl
 * Purpose:
 *      Perform shmoo (PHY calibration) on specific DRC index.
 * Parameters:
 *      unit                - unit number
 *      phy_ndx             - DRC index to perform shmoo on.
 *      shmoo_type          - Selects shmoo sub-section to be performs (-1 for full shmoo)
 *      stat                - RFU
 *      plot                - Plot shmoo results when not equal to 0
 *      action              - Save/restore functionality
 *      *config_param       - PHY configuration saved/restored
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 */

int
soc_sjc16_shmoo_ctl(int unit, int phy_ndx, int shmoo_type, int stat, int plot, int action, sjc16_shmoo_config_param_t *config_param)
{
    sjc16_shmoo_container_t *scPtr = NULL;
    uint32 dramType;
    uint32 ctlType;
    uint32 i;
    int ndx, ndxEnd;
    const uint32 *seqPtr;
    uint32 seqCount;

    dramType = shmoo_dram_info.dram_type;
    ctlType = shmoo_dram_info.ctl_type;

    if(!stat)
    {
        scPtr = sal_alloc(sizeof(sjc16_shmoo_container_t), "SJC16 Shmoo Container");
        if(scPtr == NULL)
        {
            return SOC_E_MEMORY;
        }
        sal_memset(scPtr, 0, sizeof(sjc16_shmoo_container_t));

        if(phy_ndx != SHMOO_SJC16_INTERFACE_RSVP)
        {
            ndx = phy_ndx;
            ndxEnd = phy_ndx + 1;
        }
        else
        {
            ndx = 0;
            ndxEnd = SHMOO_SJC16_MAX_INTERFACES;
        }

        for(; ndx < ndxEnd; ndx++)
        {
            if(!_shmoo_sjc16_check_dram(ndx)) {
                continue;
            }

            if(action == SHMOO_SJC16_ACTION_RESTORE)
            {
                switch(ctlType)
                {
                    case SHMOO_SJC16_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_SJC16_CTL_TYPE_1:
                        _shmoo_sjc16_restore(unit, phy_ndx, config_param);
                        break;
                    default:
                        if(scPtr != NULL)
                        {
                            sal_free(scPtr);
                            scPtr = NULL;
                        }

                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02u\n"), ctlType));

                        return SOC_E_FAIL;
                }
            }
            else if((action == SHMOO_SJC16_ACTION_RUN) || (action == SHMOO_SJC16_ACTION_RUN_AND_SAVE))
            {
                switch(ctlType)
                {
                    case SHMOO_SJC16_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_SJC16_CTL_TYPE_1:
                        switch(dramType)
                        {
                            case SHMOO_SJC16_DRAM_TYPE_DDR4:
                                seqPtr = &shmoo_order_sjc16_ddr4[0];
                                if(shmoo_dram_info.ecc_32p4)
                                {
                                    seqCount = SHMOO_SJC16_DDR4_ECC_32P4_SEQUENCE_COUNT;
                                }
                                else
                                {
                                    seqCount = SHMOO_SJC16_DDR4_SEQUENCE_COUNT;
                                }
                                break;
                            default:
                                if(scPtr != NULL)
                                {
                                    sal_free(scPtr);
                                    scPtr = NULL;
                                }
                                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported dram type: %02u\n"), dramType));
                                return SOC_E_FAIL;
                        }

                        (*scPtr).dramType = dramType;
                        (*scPtr).ctlType = ctlType;

                        if(shmoo_type != SHMOO_SJC16_SHMOO_RSVP)
                        {
                            (*scPtr).shmooType = shmoo_type;
                            _shmoo_sjc16_entry(unit, ndx, scPtr, SHMOO_SJC16_SINGLE);
                            _shmoo_sjc16_do(unit, ndx, scPtr);
                            _shmoo_sjc16_calib_2D(unit, ndx, scPtr);
                            _shmoo_sjc16_set_new_step(unit, ndx, scPtr);
                            if(plot)
                            {
                                _shmoo_sjc16_plot(unit, ndx, scPtr);
                            }
                            _shmoo_sjc16_exit(unit, ndx, scPtr, SHMOO_SJC16_SINGLE);
                        }
                        else
                        {
                            for(i = 0; i < seqCount; i++)
                            {
                                (*scPtr).shmooType = seqPtr[i];
                                _shmoo_sjc16_entry(unit, ndx, scPtr, SHMOO_SJC16_SEQUENTIAL);
                                _shmoo_sjc16_do(unit, ndx, scPtr);
                                _shmoo_sjc16_calib_2D(unit, ndx, scPtr);
                                _shmoo_sjc16_set_new_step(unit, ndx, scPtr);
                                if(plot)
                                {
                                    _shmoo_sjc16_plot(unit, ndx, scPtr);
                                }
                                _shmoo_sjc16_exit(unit, ndx, scPtr, SHMOO_SJC16_SEQUENTIAL);
                            }
                        }

                        break;
                    default:
                        if(scPtr != NULL)
                        {
                            sal_free(scPtr);
                            scPtr = NULL;
                        }

                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02u\n"), ctlType));

                        return SOC_E_FAIL;
                }
            }

            if((action == SHMOO_SJC16_ACTION_RUN_AND_SAVE) || (action == SHMOO_SJC16_ACTION_SAVE))
            {
                _shmoo_sjc16_save(unit, phy_ndx, config_param);
            }
        }

        if(scPtr != NULL)
        {
            sal_free(scPtr);
            scPtr = NULL;
        }

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DDR Tuning Complete\n")));
    }
    else
    {
        /* Report only */
        switch(ctlType)
        {
            case SHMOO_SJC16_CTL_TYPE_RSVP:
                break;
            case SHMOO_SJC16_CTL_TYPE_1:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02u\n"), ctlType));
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/* Set Dram Parameters/Info to Shmoo driver */
int
soc_sjc16_shmoo_dram_info_set(int unit, sjc16_shmoo_dram_info_t *sdi)
{
#if(!SHMOO_SJC16_PHY_CONSTANT_CONFIG)
    shmoo_dram_info.ctl_type = (*sdi).ctl_type;
    shmoo_dram_info.dram_type = (*sdi).dram_type;
    shmoo_dram_info.dram_bitmap = (*sdi).dram_bitmap;
    shmoo_dram_info.interface_bitwidth = (*sdi).interface_bitwidth;
    shmoo_dram_info.ecc_32p4 = (*sdi).ecc_32p4;
    shmoo_dram_info.num_columns = (*sdi).num_columns;
    shmoo_dram_info.num_rows = (*sdi).num_rows;
    shmoo_dram_info.num_banks = (*sdi).num_banks;
    shmoo_dram_info.num_bank_groups = (*sdi).num_bank_groups;
    shmoo_dram_info.data_rate_mbps = (*sdi).data_rate_mbps;
    shmoo_dram_info.ref_clk_mhz = (*sdi).ref_clk_mhz;
    shmoo_dram_info.refi = (*sdi).refi;
    shmoo_dram_info.command_parity_latency = (*sdi).command_parity_latency;
    shmoo_dram_info.sim_system_mode = (*sdi).sim_system_mode;
#endif
    return SOC_E_NONE;
}

/* Configure PHY PLL and wait for lock */
int
_soc_sjc16_shmoo_phy_cfg_pll(int unit, int phy_ndx)
{
    uint32 val;
    int fref, fref_post, fref_eff_int, fref_eff_frac, freq_vco, data_rate;
    int pdiv10, ndiv_int10;
    uint32 frequency_doubler_mode;
    uint32 iso_in, ldo_ctrl, ssc_limit, ssc_mode, ssc_step, pll_ctrl, en_ctrl;
    uint32 pdiv, ndiv_int, ndiv_frac, mdiv, fref_eff_info, vco_sel;
    uint32 timeout;

    data_rate = shmoo_dram_info.data_rate_mbps;
    fref = shmoo_dram_info.ref_clk_mhz;

    iso_in = 0;
    ldo_ctrl = 0x0000;
    ssc_limit = 0x000000;
    ssc_mode = 0;
    ssc_step = 0x0000;
    pll_ctrl = 0x00000000;

    if(shmoo_dram_info.ref_clk_mhz != 100)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Unsupported reference flock frequency: %4d MHz\n"), shmoo_dram_info.ref_clk_mhz));
        return SOC_E_FAIL;
    }

    fref_eff_int = 33;
    fref_eff_frac = 333;
    freq_vco = data_rate << 1;

    frequency_doubler_mode = 0;                                                     /* Frequency doubler OFF */
    en_ctrl = frequency_doubler_mode;

    fref_post = fref << frequency_doubler_mode;
    pdiv10 = (10000 * fref_post) / ((1000 * fref_eff_int) + fref_eff_frac);

    if((pdiv10 % 10) >= 5)
    {
        pdiv = (pdiv10 / 10) + 1;
    }
    else
    {
        pdiv = (pdiv10 / 10);
    }

    ndiv_int10 = (10000 * freq_vco) / ((1000 * fref_eff_int) + fref_eff_frac);

    if((ndiv_int10 % 10) >= 5)
    {
        ndiv_int = (ndiv_int10 / 10) + 1;
    }
    else
    {
        ndiv_int = (ndiv_int10 / 10);
    }

    ndiv_frac = 0;                                                         /* ndiv fraction OFF */
    mdiv = 1;
    fref_eff_info = fref_eff_int;

    if(freq_vco > 5500)
    {
        vco_sel = 1;
    }
    else
    {
        vco_sel = 0;
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "PHY PLL Configuration\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Fref...........: %4d\n"), fref));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "PDIV...........: %4d\n"), pdiv));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "NDIV...........: %4d\n"), ndiv_int));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Fvco...........: %4d\n"), freq_vco));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Data rate......: %4d\n"), data_rate));

#if defined(CONFIG_HELIX5)
    SOC_IF_ERROR_RETURN(READ_DDR_APB_PWR_RSTr(unit, &val));
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_PWR_ONf, 1);
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_PWR_ON_LDOf, 1);
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_RESETBf, 0);
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_POST_RESETBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_PWR_RSTr(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_DBG_1r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_DBG_1r, &val, I_PLL_CTRLf, pll_ctrl);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_DBG_1r(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_DBG_2r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_DBG_2r, &val, I_LDO_CTRLf, ldo_ctrl);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_DBG_2r(unit, val));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR_APB_PWR_RSTr(unit, &val));
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_ISO_INf, iso_in);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_PWR_RSTr(unit, val));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR_APB_LP_FREQ_2r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_LP_FREQ_2r, &val, I_NDIV_FRACf, ndiv_frac);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_LP_FREQ_2r(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_LP_FREQ_1r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_LP_FREQ_1r, &val, I_NDIV_INTf, ndiv_int);
    soc_reg_field_set(unit, DDR_APB_LP_FREQ_1r, &val, I_PDIVf, pdiv);
    soc_reg_field_set(unit, DDR_APB_LP_FREQ_1r, &val, I_MDIV_2TO0f, (mdiv & 0x7));
    soc_reg_field_set(unit, DDR_APB_LP_FREQ_1r, &val, I_FREFEFF_INFOf, fref_eff_info);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_LP_FREQ_1r(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_PWR_RSTr(unit, &val));
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_MDIV_3f, (mdiv >> 3));
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_PWR_RSTr(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_SSC_2r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_SSC_2r, &val, I_SSC_LIMITf, ssc_limit);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_SSC_2r(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_SSC_1r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_SSC_1r, &val, I_SSC_MODEf, ssc_mode);
    soc_reg_field_set(unit, DDR_APB_SSC_1r, &val, I_SSC_STEPf, ssc_step);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_SSC_1r(unit, val));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR_APB_LP_VCO_CTRL_2r(unit, &val));
    soc_reg_field_set(unit, DDR_APB_LP_VCO_CTRL_2r, &val, I_VCO_SELf, vco_sel);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_LP_VCO_CTRL_2r(unit, val));

    SOC_IF_ERROR_RETURN(READ_DDR_APB_CLK_ROUTEr(unit, &val));
    soc_reg_field_set(unit, DDR_APB_CLK_ROUTEr, &val, I_EN_CTRLf, en_ctrl);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_CLK_ROUTEr(unit, val));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(READ_DDR_APB_PWR_RSTr(unit, &val));
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_RESETBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_PWR_RSTr(unit, val));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    timeout = 20000;
    do
    {
        SOC_IF_ERROR_RETURN(READ_DDR_APB_PWR_RSTr(unit, &val));
        if(soc_reg_field_get(unit, DDR_APB_PWR_RSTr, val, O_LOCKf))
        {
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Lock status....: Locked\n")));
            break;
        }

        if(timeout == 0)
        {
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Lock status....: Timed out\n")));
            return SOC_E_TIMEOUT;
        }

        timeout--;
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);
    }
    while(TRUE);

    SOC_IF_ERROR_RETURN(READ_DDR_APB_PWR_RSTr(unit, &val));
    soc_reg_field_set(unit, DDR_APB_PWR_RSTr, &val, I_POST_RESETBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_DDR_APB_PWR_RSTr(unit, val));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(WRITE_DDR_S1_IDM_RESET_CONTROLr(unit, 1));
    SOC_IF_ERROR_RETURN(WRITE_DDR_S0_IDM_RESET_CONTROLr(unit, 1));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

    SOC_IF_ERROR_RETURN(WRITE_DDR_S1_IDM_RESET_CONTROLr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_DDR_S0_IDM_RESET_CONTROLr(unit, 0));

    sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

#endif

    return SOC_E_NONE;
}

int
soc_sjc16_shmoo_phy_init(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data, reset_ctrl_data, read_fifo_ctrl_data;
    sjc16_step_size_t ss;
    uint32 step1000;
    uint32 size1000UI;
    uint32 setting;
    uint32 ck_ui, addr_ctrl_ui, dqs_ui, data_ui;

    if(phy_ndx != SHMOO_SJC16_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_SJC16_MAX_INTERFACES;
    }

    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_sjc16_check_dram(ndx))
        {
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization (PHY index: %02d)\n"), ndx));

/*A01*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A01. Configure PHY PLL\n")));
        _soc_sjc16_shmoo_phy_cfg_pll(unit, ndx);
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A02*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A02. Reset all PHY logic\n")));
        reset_ctrl_data = 0;
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A03*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A03. Enable Read FIFO\n")));
        read_fifo_ctrl_data = 0;
        DDR_PHY_SET_FIELD(read_fifo_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, READ_FIFO_CTRL, W2R_MIN_DELAY_2, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_READ_FIFO_CTRLr(unit, read_fifo_ctrl_data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, READ_FIFO_RESET_N, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(read_fifo_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, READ_FIFO_CTRL, ENABLE, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_READ_FIFO_CTRLr(unit, read_fifo_ctrl_data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A04*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A04. Configure input shift control\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, INPUT_SHIFT_CTRL, DATAPATH_SHIFT_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, INPUT_SHIFT_CTRL, DATAPATH_ADDITIONAL_LATENCY, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, INPUT_SHIFT_CTRL, ADDRPATH_SHIFT_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, INPUT_SHIFT_CTRL, ADDRPATH_ADDITIONAL_LATENCY, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, INPUT_SHIFT_CTRL, RCMD_SHIFT_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, INPUT_SHIFT_CTRL, RCMD_ADDITIONAL_LATENCY, 5);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_INPUT_SHIFT_CTRLr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A05*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A05. Configure power regulation\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, PWRDN, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_LDO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_LDO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_LDO_W_CONFIGr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, BIAS_CTRL, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, BYPASS, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, CTRL, 0x00);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, REF_EXT, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, REF_SEL_EXT, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_LDO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_LDO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_LDO_W_CONFIGr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, LDO_CONFIG, PWRDN, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_LDO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_LDO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_LDO_W_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_LDO_R_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_LDO_W_CONFIGr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A06*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A06. Configure reference voltage\n")));
/*R06*/ data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, SHARED_VREF_DAC_CONFIG, CTRL, 0x0001);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, SHARED_VREF_DAC_CONFIG, DATA, 0x92);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, SHARED_VREF_DAC_CONFIG, CTRL, 0x0000);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_VREF_DAC_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_VREF_DAC_CONFIGr(unit, data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A07*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A07. Release VDL resets\n")));
        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, VDL_1G_RESET_N, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));

        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A08*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A08. Compute VDL step size\n")));
        _sjc16_calculate_step_size(unit, ndx, &ss);

        step1000 = ss.step1000;
        size1000UI = ss.size1000UI;
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL step size........: %3d.%03d ps\n"),
                        (step1000 / 1000), (step1000 % 1000)));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     UI size..............: %3d.%03d steps\n"),
                        (size1000UI / 1000), (size1000UI % 1000)));

/*A09*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A09. Configure read clock\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, READ_CLOCK_CONFIG, FREE_RUNNING_MODE, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_READ_CLOCK_CONFIGr(unit, data));


        data = 0;
        setting = size1000UI / 1000;                                                        /* 50% placement on 2G clock */
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_READ_MAX_VDL_FSM, MAX_VDL_STEP, setting);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_READ_MAX_VDL_FSMr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_READ_MAX_VDL_FSMr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_READ_MAX_VDL_FSMr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_READ_MAX_VDL_FSMr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_READ_MAX_VDL_FSMr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_3, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_2, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_MINUS_1, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_0, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_PLUS_1, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_STRETCH_CONFIG, A_PLUS_2, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_REN_STRETCH_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_REN_STRETCH_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_REN_STRETCH_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_REN_STRETCH_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_REN_STRETCH_CONFIGr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A10*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A10. Configure UI shifts\n")));
        ck_ui = SHMOO_SJC16_DDR4_INIT_CK_UI_SHIFT;
        addr_ctrl_ui = SHMOO_SJC16_DDR4_INIT_ADDR_CTRL_UI_SHIFT;
        dqs_ui = SHMOO_SJC16_DDR4_INIT_DQS_UI_SHIFT;
        data_ui = SHMOO_SJC16_DDR4_INIT_DATA_UI_SHIFT;

        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, MAX_VDL_CK, UI_SHIFT, ck_ui);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_MAX_VDL_CKr(unit, data));


        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, UI_SHIFT, addr_ctrl_ui);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DQS, UI_SHIFT, dqs_ui);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DQSr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, UI_SHIFT, data_ui);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A11*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A11. Configure VDLs\n")));
        data = SHMOO_SJC16_DDR4_INIT_WRITE_MIN_VDL_POS;
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_DBIr(unit, data));

    /*    DDR_PHY_REG_WRITE(unit, SHMOO_SJC16_PHY_REG_BASE, 0, SJC16_DDR4_PHY_DQ_BYTE0_WRITE_MIN_VDL_EDC, data)); */
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_DBIr(unit, data));

    /*    DDR_PHY_REG_WRITE(unit, SHMOO_SJC16_PHY_REG_BASE, 0, SJC16_DDR4_PHY_DQ_BYTE1_WRITE_MIN_VDL_EDC, data)); */
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_DBIr(unit, data));

    /*    DDR_PHY_REG_WRITE(unit, SHMOO_SJC16_PHY_REG_BASE, 0, SJC16_DDR4_PHY_DQ_BYTE2_WRITE_MIN_VDL_EDC, data)); */
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MIN_VDL_DBIr(unit, data));

    /*    DDR_PHY_REG_WRITE(unit, SHMOO_SJC16_PHY_REG_BASE, 0, SJC16_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_EDC, data)); */
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MIN_VDL_DBIr(unit, data));


        data = SHMOO_SJC16_DDR4_INIT_READ_MIN_VDL_POS;

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_DBIr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, data));

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_DBIr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, data));

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_DBIr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, data));

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_DBIr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, data));

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT3r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT4r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT5r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT6r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_BIT7r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_DBIr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MIN_VDL_EDCr(unit, data));

        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DQSr(unit, &data));
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DQS, MAX_VDL_STEP, 0);
           SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DQSr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DQSr(unit, data));

        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, &data));
        setting = (size1000UI * 3) / 20000;                                                                 /* 15% adjustment */
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, WRITE_MAX_VDL_DATA, MAX_VDL_STEP, setting);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_WRITE_MAX_VDL_DATAr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, &data));
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MAX_VDL_STEP, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, MASTER_MODE, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, READ_MAX_VDL_DQS_MASTER_CTRL, FORCE_UPDATE, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_READ_MAX_VDL_DQS_MASTER_CTRLr(unit, data));

        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, &data)); /* ADDR/CTRL should be in a working position */
        setting = size1000UI / 1250;                                                                        /* 80% adjustment */
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_AQ, L_MAX_VDL_ADDR, MAX_VDL_STEP, setting);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_ADDRr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_L_MAX_VDL_CTRLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_AQ_U_MAX_VDL_CTRLr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A12*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A12. ZQ calibration\n")));
 /*R12*/ if(shmoo_dram_info.sim_system_mode)
        {
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Skipped for emulation\n")));

            goto SHMOO_SJC16_ZQ_CALIBRATION_END;
        }

        _sjc16_zq_calibration(unit, ndx);

        SHMOO_SJC16_ZQ_CALIBRATION_END:

/*A13*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A13. Enable CK\n")));
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_COMMON_CK_CONFIGr(unit, &data));

        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_COMMON, CK_CONFIG, CK_ENABLE, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_COMMON_CK_CONFIGr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A14*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A14. Configure FIFOs\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_RD2_2G_SELECT, 0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_RD2_2G_SELECT, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_RD2_2G_SELECT, 2);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_RD2_2G_SELECT, 3);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_RD2_2G_SELECT, 4);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_FIFO_CONFIG, RD_2G_DELAY, 6);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_REN_FIFO_CONFIG, RD2_2G_DELAY, SHMOO_SJC16_DDR4_INIT_REN_RD2_2G_DELAY);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_REN_FIFO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_REN_FIFO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_REN_FIFO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_REN_FIFO_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_REN_FIFO_CONFIGr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A15*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A15. Program DRAM parameters into PHY\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_DRAM_PARAMETER_CONFIG, T_PHY_WRDATA, 0x0);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_DRAM_PARAMETER_CONFIG, BURST_LENGTH, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_DRAM_PARAMETER_CONFIGr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_OE_CONFIG, OE_POST_CYCLES, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_DQ_BYTE0, DDR4_OE_CONFIG, OE_PRE_CYCLES, 0);

        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE0_DDR4_OE_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE1_DDR4_OE_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE2_DDR4_OE_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE3_DDR4_OE_CONFIGr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_DQ_BYTE4_DDR4_OE_CONFIGr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A16*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A16. Set active PHY interfaces\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, ACTIVE_INTERFACES_CONFIG, ACTIVE_ADDRESS, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, ACTIVE_INTERFACES_CONFIG, ACTIVE_BYTES_BITMAP, 0x0F);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_ACTIVE_INTERFACES_CONFIGr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A17*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A17. Release PHY resets\n")));
        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DRC_1G_RESET_N, 1);
        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_1G_RESET_N, 1);
        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, PHY_2G_RESET_N, 1);
        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, DDR4_GLUE_RESET_N, 1);
        DDR_PHY_SET_FIELD(reset_ctrl_data, SJC16_DDR4_PHY_CONTROL_REGS, RESET_CTRL, RCMD_FIFO_RESET_N, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_RESET_CTRLr(unit, reset_ctrl_data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

/*A18*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A18. Enable FIFOs\n")));
        SOC_IF_ERROR_RETURN(READ_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, &data));
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE0_WRITE_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE1_WRITE_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE2_WRITE_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE3_WRITE_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, SJC16_DDR4_PHY_CONTROL_REGS, DDR4_REN_FIFO_CENTRAL_INITIALIZER, DQ_BYTE4_WRITE_ENABLE, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR4_PHY_CONTROL_REGS_DDR4_REN_FIFO_CENTRAL_INITIALIZERr(unit, data));
        sal_usleep(SHMOO_SJC16_SHORT_SLEEP);

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization complete (PHY index: %02d)\n"), ndx));
    }

    return SOC_E_NONE;
}

#endif
