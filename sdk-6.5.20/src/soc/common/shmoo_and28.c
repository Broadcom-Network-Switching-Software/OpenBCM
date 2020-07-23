/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * 28nm PHY Support
 */
#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <soc/memtune.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_AND28_SUPPORT
#include <soc/shmoo_and28.h>
#include <soc/phy/ddr34.h>

static uint32 SHMOO_AND28_PHY_TYPE = SHMOO_AND28_PHY_TYPE_RSVP;
static uint32 DDR34_PHY_REGS_TABLE(DDR34_PHY_MAX_REG_INDEX);
static uint32 DDR34_PHY_FIELDS_TABLE(DDR34_PHY_MAX_FIELD_INDEX, 0);

static and28_shmoo_dram_info_t shmoo_dram_info =
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

soc_and28_phy_reg_read_t soc_and28_phy_reg_read = NULL;
soc_and28_phy_reg_write_t soc_and28_phy_reg_write = NULL;
soc_and28_phy_reg_modify_t soc_and28_phy_reg_modify = NULL;
static _shmoo_and28_drc_bist_conf_set_t _shmoo_and28_drc_bist_conf_set = NULL;
static _shmoo_and28_drc_bist_err_cnt_t _shmoo_and28_drc_bist_err_cnt = NULL;
static _shmoo_and28_drc_dram_init_t _shmoo_and28_drc_dram_init = NULL;

const uint32 shmoo_order_and28_ddr3[SHMOO_AND28_DDR3_SEQUENCE_COUNT] =
{
    SHMOO_AND28_RD_EN,
    SHMOO_AND28_RD_EXTENDED,
    SHMOO_AND28_WR_EXTENDED,
    SHMOO_AND28_ADDR_EXTENDED,
    SHMOO_AND28_CTRL_EXTENDED
};

const uint32 shmoo_order_and28_ddr3l[SHMOO_AND28_DDR3L_SEQUENCE_COUNT] =
{
    SHMOO_AND28_RD_EN,
    SHMOO_AND28_RD_EXTENDED,
    SHMOO_AND28_WR_EXTENDED,
    SHMOO_AND28_ADDR_EXTENDED,
    SHMOO_AND28_CTRL_EXTENDED
};

const uint32 shmoo_order_and28_ddr4[SHMOO_AND28_DDR4_SEQUENCE_COUNT] =
{
    SHMOO_AND28_RD_EN,
    SHMOO_AND28_RD_EXTENDED,
    SHMOO_AND28_WR_EXTENDED,
    SHMOO_AND28_ADDR_EXTENDED,
    SHMOO_AND28_CTRL_EXTENDED
};

uint32
_shmoo_and28_validate_config(void)
{
    return  (
                (shmoo_dram_info.ctl_type != 0)
                && (shmoo_dram_info.dram_type != 0)
            /*    && (shmoo_dram_info.dram_bitmap != 0) */
                && (shmoo_dram_info.interface_bitwidth != 0)
            /*    && (shmoo_dram_info.split_bus != 0) */
                && (shmoo_dram_info.num_columns != -1)
                && (shmoo_dram_info.num_rows != -1)
                && (shmoo_dram_info.num_banks != -1)
                && (shmoo_dram_info.num_bank_groups != -1)
                && (shmoo_dram_info.data_rate_mbps != -1)
                && (shmoo_dram_info.ref_clk_mhz != -1)
            /*    && (shmoo_dram_info.refi != 0) */
            /*    && (shmoo_dram_info.command_parity_latency != 0) */
            /*    && (shmoo_dram_info.sim_system_mode != 0) */
            );
}

uint32
_shmoo_and28_validate_cbi(void)
{
    return  (
                (soc_and28_phy_reg_read != NULL)
                && (soc_and28_phy_reg_write != NULL)
                && (soc_and28_phy_reg_modify != NULL)
                && (_shmoo_and28_drc_bist_conf_set != NULL)
                && (_shmoo_and28_drc_bist_err_cnt != NULL)
                && (_shmoo_and28_drc_dram_init != NULL)
            );
}

uint32
_shmoo_and28_check_dram(int phy_ndx)
{
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        return ((shmoo_dram_info.dram_bitmap >> phy_ndx) & 0x1);
    }
    else
    {
        return ((shmoo_dram_info.dram_bitmap >> (phy_ndx << 1)) & 0x3);
    }
}

STATIC int
_and28_initialize_bist(int unit, int phy_ndx, int bit, and28_shmoo_container_t *scPtr, and28_bist_info_t *biPtr)
{   
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            (*biPtr).write_weight = 255;
            (*biPtr).read_weight = 255;
            (*biPtr).bist_timer_us = 0;
            (*biPtr).bist_num_actions = 51510;
            (*biPtr).bist_start_address = 0x00000000;
            (*biPtr).bist_end_address = 0x00FFFFFF;
            (*biPtr).mpr_mode = 0;
            (*biPtr).prbs_mode = 1;
            break;
        case SHMOO_AND28_RD_EXTENDED:
            (*biPtr).write_weight = 255;
            (*biPtr).read_weight = 255;
            (*biPtr).bist_timer_us = 0;
            (*biPtr).bist_num_actions = 51510;
            (*biPtr).bist_start_address = 0x00000000;
            (*biPtr).bist_end_address = 0x00FFFFFF;
            (*biPtr).mpr_mode = 0;
            (*biPtr).prbs_mode = 1;
            break;
        case SHMOO_AND28_WR_EXTENDED:
            (*biPtr).write_weight = 255;
            (*biPtr).read_weight = 255;
            (*biPtr).bist_timer_us = 0;
            (*biPtr).bist_num_actions = 51510;
            (*biPtr).bist_start_address = 0x00000000;
            (*biPtr).bist_end_address = 0x00FFFFFF;
            (*biPtr).mpr_mode = 0;
            (*biPtr).prbs_mode = 1;
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            (*biPtr).write_weight = 255;
            (*biPtr).read_weight = 255;
            (*biPtr).bist_timer_us = 0;
            (*biPtr).bist_num_actions = 51510;
            (*biPtr).bist_start_address = 0x00000000;
            (*biPtr).bist_end_address = 0x00FFFFFF;
            (*biPtr).mpr_mode = 0;
            (*biPtr).prbs_mode = 1;
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            (*biPtr).write_weight = 255;
            (*biPtr).read_weight = 255;
            (*biPtr).bist_timer_us = 0;
            (*biPtr).bist_num_actions = 51510;
            (*biPtr).bist_start_address = 0x00000000;
            (*biPtr).bist_end_address = 0x00FFFFFF;
            (*biPtr).mpr_mode = 0;
            (*biPtr).prbs_mode = 1;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_and28_run_bist(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, and28_bist_info_t *biPtr, and28_shmoo_error_array_t *seaPtr)
{
    and28_bist_err_cnt_t be;
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_conf_set(unit, phy_ndx, (*biPtr)));
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_err_cnt(unit, phy_ndx, &be));
            
            (*seaPtr)[0] = be.bist_err_occur;
            break;
        case SHMOO_AND28_RD_EXTENDED:
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_conf_set(unit, phy_ndx, (*biPtr)));
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_err_cnt(unit, phy_ndx, &be));
            
            (*seaPtr)[0] = be.bist_err_occur;
            break;
        case SHMOO_AND28_WR_EXTENDED:
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_conf_set(unit, phy_ndx, (*biPtr)));
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_err_cnt(unit, phy_ndx, &be));
            
            (*seaPtr)[0] = be.bist_err_occur;
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_conf_set(unit, phy_ndx, (*biPtr)));
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_err_cnt(unit, phy_ndx, &be));
            
            (*seaPtr)[0] = be.bist_err_occur;
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_conf_set(unit, phy_ndx, (*biPtr)));
            SOC_IF_ERROR_RETURN(_shmoo_and28_drc_bist_err_cnt(unit, phy_ndx, &be));
            
            (*seaPtr)[0] = be.bist_err_occur;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_rd_en(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 jump;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data;
    and28_bist_info_t bi;
    and28_shmoo_error_array_t sea;

    yCapMin = 0;
    yCapMax = (*scPtr).sizeY;
    jump = (*scPtr).yJump;
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_AND28_RD_EN;
    
    _and28_initialize_bist(unit, phy_ndx, -1, scPtr, &bi);
    
    for(y = yCapMin; y < yCapMax; y++)
    {
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, FORCE, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, VDL_STEP, y << jump);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
        }
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
        }
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
            }
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
            }
        }

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, x);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            }
            
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
            }

            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            
            _and28_run_bist(unit, phy_ndx, scPtr, &bi, &sea);
            
            (*scPtr).result2D[x + xStart] = sea[0];
        }
        
        xStart += (*scPtr).sizeX;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_wr_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and28_bist_info_t bi;
    and28_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND28_WR_EXTENDED;
    
    _and28_initialize_bist(unit, phy_ndx, -1, scPtr, &bi);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, x);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
        }
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
        }
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
            }
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
            }
        }
        
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
        }

        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        _and28_run_bist(unit, phy_ndx, scPtr, &bi, &sea);
        
        (*scPtr).result2D[x] = sea[0];
    }
    
    return SOC_E_NONE;
}

int
_shmoo_and28_rd_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data, temp;
    uint32 rd_dqs_pos0 = 0, rd_dqs_pos1 = 0, rd_en_pos0 = 0, rd_en_pos1 = 0, rd_dqs_delta0 = 0, rd_dqs_delta1 = 0;
    uint32 rd_dq_fail_count0 = 0, rd_dq_fail_count1 = 0;
    uint32 rd_dqs_pos2 = 0, rd_dqs_pos3 = 0, rd_en_pos2 = 0, rd_en_pos3 = 0, rd_dqs_delta2 = 0, rd_dqs_delta3 = 0;
    uint32 rd_dq_fail_count2 = 0, rd_dq_fail_count3 = 0;
    and28_bist_info_t bi;
    and28_shmoo_error_array_t sea;

    if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
    {
        yCapMin = 28;
        yCapMax = 61;
    }
    else
    {
        yCapMin = 16;
        yCapMax = 49;
    }
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = SHMOO_AND28_MAX_VREF_RANGE;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).yJump = 0;
    (*scPtr).shmooType = SHMOO_AND28_RD_EXTENDED;
    
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
    rd_dqs_pos0 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP);
    
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
    rd_dqs_pos1 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, VDL_STEP);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
        rd_dqs_pos2 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP);
        
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
        rd_dqs_pos3 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, VDL_STEP);
    }
    
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
    rd_en_pos0 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
    
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
    rd_en_pos1 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
        rd_en_pos2 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
        
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
        rd_en_pos3 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
    }
    
    _and28_initialize_bist(unit, phy_ndx, -1, scPtr, &bi);
    
    x = 0;
    data = 0;
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, x);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
    }
    
    rd_dqs_delta0 = x - rd_dqs_pos0;
    rd_dqs_delta1 = x - rd_dqs_pos1;
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        rd_dqs_delta2 = x - rd_dqs_pos2;
        rd_dqs_delta3 = x - rd_dqs_pos3;
    }
    
    temp = rd_en_pos0 + rd_dqs_delta0;
    data = 0;
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
    if(temp & 0x80000000)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
    
    temp = rd_en_pos1 + rd_dqs_delta1;
    data = 0;
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
    if(temp & 0x80000000)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        temp = rd_en_pos2 + rd_dqs_delta2;
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
        if(temp & 0x80000000)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
        }
        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
        }
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
        
        temp = rd_en_pos3 + rd_dqs_delta3;
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
        if(temp & 0x80000000)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
        }
        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
        }
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
    }
    
    rd_dq_fail_count0 = 0;
    rd_dq_fail_count1 = 0;
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        rd_dq_fail_count2 = 0;
        rd_dq_fail_count3 = 0;
    }
    
    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, FORCE, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, VDL_STEP, x);
        if(rd_dq_fail_count0 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
        {
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
            }
        }
        if(rd_dq_fail_count1 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
        {
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
            }
        }
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            if(rd_dq_fail_count2 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
                if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                {
                    WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
                }
            }
            if(rd_dq_fail_count3 <= SHMOO_AND28_RD_DQ_FAIL_CAP)
            {
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
                if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                {
                    WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
                }
            }
        }
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
        }
        
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        _and28_run_bist(unit, phy_ndx, scPtr, &bi, &sea);

        if((sea[0] & 0x000000FF) == 0x000000FF)
        {
            rd_dq_fail_count0++;
        }
        if((sea[0] & 0x0000FF00) == 0x0000FF00)
        {
            rd_dq_fail_count1++;
        }
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            if((sea[0] & 0x00FF0000) == 0x00FF0000)
            {
                rd_dq_fail_count2++;
            }
            if((sea[0] & 0xFF000000) == 0xFF000000)
            {
                rd_dq_fail_count3++;
            }
        }
        
        if((rd_dq_fail_count0 > SHMOO_AND28_RD_DQ_FAIL_CAP) && (rd_dq_fail_count1 > SHMOO_AND28_RD_DQ_FAIL_CAP))
        {
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                if((rd_dq_fail_count2 > SHMOO_AND28_RD_DQ_FAIL_CAP) && (rd_dq_fail_count3 > SHMOO_AND28_RD_DQ_FAIL_CAP))
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    for(y = yCapMin; y < yCapMax; y++)
    {
        READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &data);
        /* DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, y); */
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, y);
        WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
        
        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, x);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
            }
            
            rd_dqs_delta0 = x - rd_dqs_pos0;
            rd_dqs_delta1 = x - rd_dqs_pos1;
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                rd_dqs_delta2 = x - rd_dqs_pos2;
                rd_dqs_delta3 = x - rd_dqs_pos3;
            }
            
            temp = rd_en_pos0 + rd_dqs_delta0;
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            if(temp & 0x80000000)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            
            temp = rd_en_pos1 + rd_dqs_delta1;
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            if(temp & 0x80000000)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                temp = rd_en_pos2 + rd_dqs_delta2;
                data = 0;
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                if(temp & 0x80000000)
                {
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                }
                else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                {
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                }
                else
                {
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                }
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                
                temp = rd_en_pos3 + rd_dqs_delta3;
                data = 0;
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                if(temp & 0x80000000)
                {
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                }
                else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                {
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                }
                else
                {
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                }
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            }
            
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
            }
            
            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            
            _and28_run_bist(unit, phy_ndx, scPtr, &bi, &sea);
            
            (*scPtr).result2D[x + xStart] = sea[0];
        }
        
        xStart += (*scPtr).sizeX;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_addr_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and28_bist_info_t bi;
    and28_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND28_ADDR_EXTENDED;
    
    _and28_initialize_bist(unit, phy_ndx, -1, scPtr, &bi);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, x);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, phy_ndx, data);
        
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
        }

        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        _and28_run_bist(unit, phy_ndx, scPtr, &bi, &sea);
        
        (*scPtr).result2D[x] = sea[0];
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_ctrl_extended(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and28_bist_info_t bi;
    and28_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND28_CTRL_EXTENDED;
    
    _and28_initialize_bist(unit, phy_ndx, -1, scPtr, &bi);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD10, FORCE, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD10, VDL_STEP, x);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, phy_ndx, data);
        
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
        }
        
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        _and28_run_bist(unit, phy_ndx, scPtr, &bi, &sea);
        
        (*scPtr).result2D[x] = sea[0];
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_do(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            return _shmoo_and28_rd_en(unit, phy_ndx, scPtr);
        case SHMOO_AND28_RD_EXTENDED:
            return _shmoo_and28_rd_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND28_WR_EXTENDED:
            return _shmoo_and28_wr_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND28_ADDR_EXTENDED:
            return _shmoo_and28_addr_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND28_CTRL_EXTENDED:
            if(!SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _shmoo_and28_ctrl_extended(unit, phy_ndx, scPtr);
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_and28_calib_2D(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 calibMode, uint32 calibPos)
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
    uint32 maxPassLengthArray[SHMOO_AND28_WORD];

    xStart = 0;
    sizeX = (*scPtr).sizeX;
    calibStart = (*scPtr).calibStart;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;

    switch(calibMode)
    {
        case SHMOO_AND28_BIT:
            if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                iter = shmoo_dram_info.interface_bitwidth << 1;
            }
            else
            {
                iter = shmoo_dram_info.interface_bitwidth;
            }
            shiftAmount = 0;
            dataMask = 0x1;
            break;
        case SHMOO_AND28_BYTE:
            if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                iter = shmoo_dram_info.interface_bitwidth >> 2;
            }
            else
            {
                iter = shmoo_dram_info.interface_bitwidth >> 3;
            }
            shiftAmount = 3;
            dataMask = 0xFF;
            break;
        case SHMOO_AND28_HALFWORD:
            if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                iter = shmoo_dram_info.interface_bitwidth >> 3;
            }
            else
            {
                iter = shmoo_dram_info.interface_bitwidth >> 4;
            }
            shiftAmount = 4;
            dataMask = 0xFFFF;
            break;
        case SHMOO_AND28_WORD:
            iter = 1;
            shiftAmount = 5;
            if(shmoo_dram_info.split_bus && !(_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                dataMask = 0x0000FFFF;
            }
            else
            {
                dataMask = 0xFFFFFFFF;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported 2D calibration mode: %02lu\n"), (long unsigned int)calibMode));
            return SOC_E_FAIL;
    }

    for(i = 0; i < iter; i++)
    {
        (*scPtr).resultData[i] = 0;
        maxPassLengthArray[i] = 0;
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
            
            switch(calibPos)
            {
                case SHMOO_AND28_CALIB_FAIL_START:
                case SHMOO_AND28_CALIB_RISING_EDGE:
                    if(failStartSeen > 0)
                    {
                        maxMidPointX = failStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_AND28_CALIB_PASS_START:
                case SHMOO_AND28_CALIB_FALLING_EDGE:
                    if(passStartSeen > 0)
                    {
                        maxMidPointX = passStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_AND28_CALIB_CENTER_PASS:
                    if((maxPassLength > 0) && (maxPassLengthArray[i] < maxPassLength))
                    {
                        maxMidPointX = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                        maxPassLengthArray[i] = maxPassLength;
                    }
                    break;
                case SHMOO_AND28_CALIB_VDL_ZERO:
                    maxMidPointX = 0;
                    (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration position: %02lu\n"), (long unsigned int)calibPos));
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
_shmoo_and28_calib_2D(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            return _and28_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_RD_EXTENDED:
            return _and28_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_WR_EXTENDED:
            return _and28_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_ADDR_EXTENDED:
            return _and28_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_WORD, SHMOO_AND28_CALIB_CENTER_PASS);
        case SHMOO_AND28_CTRL_EXTENDED:
            if(!SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _and28_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND28_WORD, SHMOO_AND28_CALIB_CENTER_PASS);
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_set_new_step(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    uint32 calibMode;
/*    uint32 engageUIshift; */
    uint32 val, yVal;
    uint32 data, temp;
    uint32 rd_dqs_pos0 = 0, rd_dqs_pos1 = 0, rd_en_pos0 = 0, rd_en_pos1 = 0, rd_dqs_delta0 = 0, rd_dqs_delta1 = 0;
    uint32 rd_dqs_pos2 = 0, rd_dqs_pos3 = 0, rd_en_pos2 = 0, rd_en_pos3 = 0, rd_dqs_delta2 = 0, rd_dqs_delta3 = 0;
    
    calibMode = (*scPtr).calibMode;
/*    engageUIshift = (*scPtr).engageUIshift; */
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                        
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    }
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_HALFWORD:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    }
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_WORD:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    }
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_RD_EXTENDED:
            READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
            rd_dqs_pos0 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP);
            
            READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
            rd_dqs_pos1 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, VDL_STEP);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
                rd_dqs_pos2 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP);
                
                READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
                rd_dqs_pos3 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, VDL_STEP);
            }
            
            READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
            rd_en_pos0 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
            
            READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
            rd_en_pos1 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
                rd_en_pos2 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
                
                READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
                rd_en_pos3 = DDR34_GET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
            }
            
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                    
                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    val = (*scPtr).resultData[1] & 0xFFFF;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                    
                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        val = (*scPtr).resultData[2] & 0xFFFF;
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                        
                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                        
                        val = (*scPtr).resultData[3] & 0xFFFF;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                        
                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    }
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)
                                + (((*scPtr).resultData[2] >> 16) & 0xFFFF) + (((*scPtr).resultData[3] >> 16) & 0xFFFF)) >> 2;
                    }
                    else
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    }
                    READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &data);
                    /* DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, yVal); */
                    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_HALFWORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                    
                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        val = (*scPtr).resultData[1] & 0xFFFF;
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                        
                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                        
                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    }
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    }
                    else
                    {
                        yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    }
                    READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &data);
                    /* DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, yVal); */
                    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_WORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                    
                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                    if(temp & 0x80000000)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, val);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                        
                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                        
                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
                        if(temp & 0x80000000)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND28_MAX_VDL_LENGTH)
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                    }
                    
                    yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &data);
                    /* DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, yVal); */
                    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_WR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                    }
                    
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                    }
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                        {
                            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                        }
                        
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_WR_DQ0, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                        {
                            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                        }
                    }
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_HALFWORD:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                    }
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                        {
                            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                        {
                            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                        }
                    }
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                case SHMOO_AND28_WORD:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                    }
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                    }
                    
                    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                        {
                            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                        }
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                        {
                            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                        }
                    }
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND28_BYTE:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND28_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND28_WORD:
                    data = 0;
                    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
                    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, phy_ndx, data);
                    
                    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            if(SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, phy_ndx, &data);
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, phy_ndx, data);
                
                sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            }
            else
            {
                switch(calibMode)
                {
                    case SHMOO_AND28_BIT:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                        return SOC_E_FAIL;
                    case SHMOO_AND28_BYTE:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                        return SOC_E_FAIL;
                    case SHMOO_AND28_HALFWORD:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                        return SOC_E_FAIL;
                    case SHMOO_AND28_WORD:
                        data = 0;
                        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD10, FORCE, 1);
                        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD10, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, phy_ndx, data);
                        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, phy_ndx, data);
                        
                        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02lu / %02lu\n"), (long unsigned int)(*scPtr).shmooType, (long unsigned int)calibMode));
                        return SOC_E_FAIL;
                }
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    
    data = 0;
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
    WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
    }

    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
    return SOC_E_NONE;
}

STATIC int
_and28_plot(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 plotMode)
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
    char str0[SHMOO_AND28_STRING_LENGTH];
    char str1[SHMOO_AND28_STRING_LENGTH];
    char str2[SHMOO_AND28_STRING_LENGTH];
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

    switch(calibPos)
    {
        case SHMOO_AND28_CALIB_RISING_EDGE:
        case SHMOO_AND28_CALIB_FALLING_EDGE:
            pass_low[0] = '_';
            pass_low[1] = 0;
            fail_high[0] = '|';
            fail_high[1] = 0;
            break;
        case SHMOO_AND28_CALIB_CENTER_PASS:
        case SHMOO_AND28_CALIB_PASS_START:
        case SHMOO_AND28_CALIB_FAIL_START:
        case SHMOO_AND28_CALIB_VDL_ZERO:
            pass_low[0] = '+';
            pass_low[1] = 0;
            fail_high[0] = '-';
            fail_high[1] = 0;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration position: %02lu\n"), (long unsigned int)calibPos));
            return SOC_E_FAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));

    switch(plotMode)
    {
        case SHMOO_AND28_BIT:
            if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                iter = shmoo_dram_info.interface_bitwidth << 1;
            }
            else
            {
                iter = shmoo_dram_info.interface_bitwidth;
            }
            shiftAmount = 0;
            dataMask = 0x1;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    calibShiftAmount = 3;
                    break;
                case SHMOO_AND28_HALFWORD:
                    calibShiftAmount = 4;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 5;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02lu\n"), (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_BYTE:
            if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                iter = shmoo_dram_info.interface_bitwidth >> 2;
            }
            else
            {
                iter = shmoo_dram_info.interface_bitwidth >> 3;
            }
            shiftAmount = 3;
            dataMask = 0xFF;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from byte mode to bit mode\n")));
                    if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        iter = shmoo_dram_info.interface_bitwidth << 1;
                    }
                    else
                    {
                        iter = shmoo_dram_info.interface_bitwidth;
                    }
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_HALFWORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 2;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02lu\n"), (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_HALFWORD:
            if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                iter = shmoo_dram_info.interface_bitwidth >> 3;
            }
            else
            {
                iter = shmoo_dram_info.interface_bitwidth >> 4;
            }
            shiftAmount = 4;
            dataMask = 0xFFFF;
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to bit mode\n")));
                    if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        iter = shmoo_dram_info.interface_bitwidth << 1;
                    }
                    else
                    {
                        iter = shmoo_dram_info.interface_bitwidth;
                    }
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to byte mode\n")));
                    if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        iter = shmoo_dram_info.interface_bitwidth >> 2;
                    }
                    else
                    {
                        iter = shmoo_dram_info.interface_bitwidth >> 3;
                    }
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_HALFWORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 1;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02lu\n"), (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND28_WORD:
            iter = 1;
            shiftAmount = 5;
            if(shmoo_dram_info.split_bus && !(_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                dataMask = 0x0000FFFF;
            }
            else
            {
                dataMask = 0xFFFFFFFF;
            }
            switch(calibMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to bit mode\n")));
                    if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        iter = shmoo_dram_info.interface_bitwidth << 1;
                    }
                    else
                    {
                        iter = shmoo_dram_info.interface_bitwidth;
                    }
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to byte mode\n")));
                    if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        iter = shmoo_dram_info.interface_bitwidth >> 2;
                    }
                    else
                    {
                        iter = shmoo_dram_info.interface_bitwidth >> 3;
                    }
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to halfword mode\n")));
                    if(shmoo_dram_info.split_bus && (_shmoo_and28_check_dram(phy_ndx) > 1))
                    {
                        iter = shmoo_dram_info.interface_bitwidth >> 3;
                    }
                    else
                    {
                        iter = shmoo_dram_info.interface_bitwidth >> 4;
                    }
                    shiftAmount = 4;
                    dataMask = 0xFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND28_WORD:
                    calibShiftAmount = 0;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02lu\n"), (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02lu\n"), (long unsigned int)plotMode));
            return SOC_E_FAIL;
    }
/*    
    if(engageUIshift)
    { */
        ui = 0;
        
        for(x = 0; x < sizeX; x++)
        {
            if((ui < SHMOO_AND28_MAX_VISIBLE_UI_COUNT) && (x > (*scPtr).endUI[ui]))
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
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** VDL step size...: %3lu.%03lu ps\n"), (long unsigned int)(step1000 / 1000), (long unsigned int)(step1000 % 1000)));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** UI size.........: %3lu.%03lu steps\n"), (long unsigned int)(size1000UI / 1000), (long unsigned int)(size1000UI % 1000)));
            
            switch((*scPtr).shmooType)
            {
                case SHMOO_AND28_RD_EN:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EN\n")));
                    break;
                case SHMOO_AND28_RD_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EXTENDED\n")));
                    break;
                case SHMOO_AND28_WR_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_EXTENDED\n")));
                    break;
                case SHMOO_AND28_ADDR_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: ADDR_EXTENDED\n")));
                    break;
                case SHMOO_AND28_CTRL_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: CTRL_EXTENDED\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Quick Shmoo.....: Off\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
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
                case SHMOO_AND28_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Bit-wise\n")));
                    break;
                case SHMOO_AND28_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Byte-wise\n")));
                    break;
                case SHMOO_AND28_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Halfword-wise\n")));
                    break;
                case SHMOO_AND28_WORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Word-wise\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02lu\n"), (long unsigned int)calibMode));
                    return SOC_E_FAIL;
            }
            
            switch(plotMode)
            {
                case SHMOO_AND28_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Bit-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Bit.............: %03lu\n"), (long unsigned int)i));
                    break;
                case SHMOO_AND28_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Byte-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Byte............: %03lu\n"), (long unsigned int)i));
                    break;
                case SHMOO_AND28_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Halfword-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Halfword........: %03lu\n"), (long unsigned int)i));
                    break;
                case SHMOO_AND28_WORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Word-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Word............: %03lu\n"), (long unsigned int)i));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02lu\n"), (long unsigned int)plotMode));
                    return SOC_E_FAIL;
            }

            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "    * Center X........: %03lu\n"), (long unsigned int)maxMidPointX));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "    * Center Y........: %03lu\n"), (long unsigned int)maxMidPointY));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str0));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str1));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str2));

            for(y = yCapMin; y < yCapMax; y++)
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  %03lu "), (long unsigned int)(y << yJump)));
                
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
                            if((calibPos == SHMOO_AND28_CALIB_RISING_EDGE) || (calibPos == SHMOO_AND28_CALIB_FAIL_START))
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
                    case SHMOO_AND28_BIT:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_AND28_BYTE:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_AND28_HALFWORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_AND28_WORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02lu\n"), (long unsigned int)calibMode));
                        return SOC_E_FAIL;
                }
                
                switch(plotMode)
                {
                    case SHMOO_AND28_BIT:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_AND28_BYTE:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_AND28_HALFWORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_AND28_WORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02lu\n"), (long unsigned int)plotMode));
                        return SOC_E_FAIL;
                }
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str0));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str1));
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str2));
            }
            
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  %03lu "), (long unsigned int)i));
            
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
                        if((calibPos == SHMOO_AND28_CALIB_RISING_EDGE) || (calibPos == SHMOO_AND28_CALIB_FAIL_START))
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
_shmoo_and28_plot(int unit, int phy_ndx, and28_shmoo_container_t *scPtr)
{
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            return _and28_plot(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE);
        case SHMOO_AND28_RD_EXTENDED:
            return _and28_plot(unit, phy_ndx, scPtr, SHMOO_AND28_BYTE);
        case SHMOO_AND28_WR_EXTENDED:
            return _and28_plot(unit, phy_ndx, scPtr, SHMOO_AND28_BIT);
        case SHMOO_AND28_ADDR_EXTENDED:
            return _and28_plot(unit, phy_ndx, scPtr, SHMOO_AND28_WORD);
        case SHMOO_AND28_CTRL_EXTENDED:
            if(!SHMOO_AND28_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _and28_plot(unit, phy_ndx, scPtr, SHMOO_AND28_WORD);
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
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

int
_and28_calculate_step_size(int unit, int phy_ndx, and28_step_size_t *ssPtr)
{
    uint32 data;
    uint32 timeout;
    
    if(shmoo_dram_info.sim_system_mode)
    {
        (*ssPtr).step1000 = 8000;
        (*ssPtr).size1000UI = 66988;
        
        return SOC_E_NONE;
    }
    
    data = 0;
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CALIBRATE, CALIB_ONCE, 1);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CALIBRATE, UPDATE_REGS, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CALIBRATE, UPDATE_FAST, 0);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    timeout = 2000;
    do
    {
        READ_DDR34_PHY_CONTROL_REGS_VDL_CALIB_STATUS1r(unit, phy_ndx, &data);
        
        if(DDR34_GET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CALIB_STATUS1, CALIB_IDLE))
        {
        /*    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration complete.\n"))); */
            break;
        }
        
        if (timeout == 0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration failed!!! (Timeout)\n")));
            return SOC_E_TIMEOUT;
        }
        
        timeout--;
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    }
    while(TRUE);
    
    if(DDR34_GET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CALIB_STATUS1, CALIB_LOCK_4B) == 0)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration failed!!! (No lock)\n")));
        return SOC_E_FAIL;
    }
    
    (*ssPtr).size1000UI = DDR34_GET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CALIB_STATUS1, CALIB_TOTAL_STEPS) * 500;
    (*ssPtr).step1000 = ((1000000000 / shmoo_dram_info.data_rate_mbps) * 1000) / ((*ssPtr).size1000UI);
    
    data = 0;
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    return SOC_E_NONE;
}

int
_and28_zq_calibration(int unit, int phy_ndx)
{
    int i;
    uint32 data;
    uint32 vref_dac;
    uint32 p_drive, n_drive;
    uint32 p_term, n_term;
    uint32 p_idle, n_idle;
    uint32 aq_p_drive, aq_n_drive;
    
    READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &vref_dac);
    if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
    {
        data = vref_dac;
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 32);
        WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    }
    
    p_drive = 16;
    n_drive = 16;
    
    READ_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, &data);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_IDDQ, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, 31);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    for(i = 0; i < SHMOO_AND28_MAX_ZQ_CAL_RANGE; i++)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, i);
        WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        READ_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, &data);
        if(DDR34_GET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_STATUS))
        {
            p_drive = i;
            break;
        }
    }
    
    if(i == SHMOO_AND28_MAX_ZQ_CAL_RANGE)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error (P) - Manual IO programming required for correct operation\n")));
        /* return SOC_E_FAIL; */
    }
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 0);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, 31);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 0);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    for(i = 0; i < SHMOO_AND28_MAX_ZQ_CAL_RANGE; i++)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, i);
        WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        READ_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, &data);
        if(DDR34_GET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_STATUS))
        {
            n_drive = i;
            break;
        }
    }
    
    if(i == SHMOO_AND28_MAX_ZQ_CAL_RANGE)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error (N) - Manual IO programming required for correct operation\n")));
        /* return SOC_E_FAIL; */
    }
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_IDDQ, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_P, 0);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    WRITE_DDR34_PHY_CONTROL_REGS_ZQ_CALr(unit, phy_ndx, data);
    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
    {
        aq_p_drive = p_drive;
        aq_n_drive = n_drive;
        
        if(p_drive < 6)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error (P offset) - Manual IO programming required for correct operation\n")));
            /* return SOC_E_FAIL; */
        }
        else
        {
            p_drive -= 6;
        }
        
        p_term = p_drive;
        n_term = 0;
        
        p_idle = p_drive;
        n_idle = 0;
    }
    else
    {
        aq_p_drive = p_drive;
        aq_n_drive = n_drive;
        
        p_term = 6;
        n_term = 6;
        
        p_idle = 0;
        n_idle = 0;
    }
    
    data = 0;
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_IDLE_STRENGTH, p_idle);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_IDLE_STRENGTH, n_idle);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_TERM_STRENGTH, p_term);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_TERM_STRENGTH, n_term);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_STRENGTH, aq_p_drive);
    DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_STRENGTH, aq_n_drive);
    WRITE_DDR34_PHY_CONTROL_REGS_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
    {
        WRITE_DDR34_PHY_CONTROL_REGS_DRIVE_PAD_CTL_2Tr(unit, phy_ndx, data);
    }
    
    data = 0;
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_IDLE_STRENGTH, p_idle);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_IDLE_STRENGTH, n_idle);
    }
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);
    WRITE_DDR34_PHY_BYTE_LANE_0_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        WRITE_DDR34_PHY_BYTE_LANE_2_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    }
    
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
    {
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);
        WRITE_DDR34_PHY_BYTE_LANE_0_DQSP_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_0_DQSN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_DQSP_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_DQSN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_DQSP_DRIVE_PAD_CTLr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_2_DQSN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_DQSP_DRIVE_PAD_CTLr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_DQSN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        }
        
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ALERT_DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);
        WRITE_DDR34_PHY_BYTE_LANE_0_ALERT_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_ALERT_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_ALERT_DRIVE_PAD_CTLr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_ALERT_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        }
    }
    
    data = 0;
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, EDC_RD_EN_PD_STRENGTH, p_drive);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, EDC_RD_EN_ND_STRENGTH, n_drive);
    }
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, BL_RD_EN_PD_STRENGTH, p_drive);
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DRIVE_PAD_CTL, BL_RD_EN_ND_STRENGTH, n_drive);
    WRITE_DDR34_PHY_BYTE_LANE_0_RD_EN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_RD_EN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        WRITE_DDR34_PHY_BYTE_LANE_2_RD_EN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_RD_EN_DRIVE_PAD_CTLr(unit, phy_ndx, data);
    }

    if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
    {
        WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, vref_dac);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    }
    
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     P drive..........: 0x%02lX\n"), (long unsigned int) p_drive));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     N drive..........: 0x%02lX\n"), (long unsigned int) n_drive));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     P termination....: 0x%02lX\n"), (long unsigned int) p_term));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     N termination....: 0x%02lX\n"), (long unsigned int) n_term));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     P idle...........: 0x%02lX\n"), (long unsigned int) p_idle));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     N idle...........: 0x%02lX\n"), (long unsigned int) n_idle));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     AQ P drive.......: 0x%02lX\n"), (long unsigned int) aq_p_drive));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     AQ N drive.......: 0x%02lX\n"), (long unsigned int) aq_n_drive));
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_entry(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential entry
     * Mode 1: Single entry
     */
    
    uint32 i;
    uint32 data, temp;
    and28_step_size_t ss;

    (*scPtr).calibStart = 0;
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
    /*A04*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "R04. Configure reference voltage\n")));
    /*R04*/ READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &data);
            if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 44);
            }
            else
            {
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 32);
            }
            WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            
    /*A08*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "R08. ZQ calibration\n")));
    /*R08*/ if(shmoo_dram_info.sim_system_mode)
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Skipped for emulation\n")));
                
                goto SHMOO_AND28_RD_EN_ZQ_CALIBRATION_END;
            }
            
            _and28_zq_calibration(unit, phy_ndx);
            
            SHMOO_AND28_RD_EN_ZQ_CALIBRATION_END:
        
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 3) / 1000;      /* 300% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }
            
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            (*scPtr).yJump = 2;
            temp = temp >> (*scPtr).yJump;
            if(temp > SHMOO_AND28_MAX_VREF_RANGE)
            {
                (*scPtr).sizeY = SHMOO_AND28_MAX_VREF_RANGE;
            }
            else
            {
                (*scPtr).sizeY = temp;
            }
            
            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1); 
            temp = (*scPtr).size1000UI / 1000;                                             /* 100% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, temp);
            }
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, phy_ndx, data);
        
            data = 0;
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_CONTROL, MODE, 1);
            }
            else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_CONTROL, MODE, 0);
            }
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_CONTROL, RD_EN_MODE, 0);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_CONTROL, RD_DATA_DLY, SHMOO_AND28_RD_DATA_DLY_INIT);
            WRITE_DDR34_PHY_BYTE_LANE_0_READ_CONTROLr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_READ_CONTROLr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_READ_CONTROLr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, phy_ndx, data);
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, ENABLE_CS1, 1);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS1_CYCLES, SHMOO_AND28_RD_EN_CYC_INIT - 1);
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS0_CYCLES, SHMOO_AND28_RD_EN_CYC_INIT - 1);
            }
            else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS1_CYCLES, SHMOO_AND28_RD_EN_CYC_INIT);
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS0_CYCLES, SHMOO_AND28_RD_EN_CYC_INIT);
            }
            WRITE_DDR34_PHY_BYTE_LANE_0_RD_EN_DLY_CYCr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_RD_EN_DLY_CYCr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_RD_EN_DLY_CYCr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, phy_ndx, data);
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND28_RD_EN_VDL_INIT);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0P, VDL_STEP, SHMOO_AND28_RD_DQ_VDL_INIT);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
            }
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
            }
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
                if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                {
                    WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
                }
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
                if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                {
                    WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCPr(unit, phy_ndx, data);
                    WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EDCNr(unit, phy_ndx, data);
                }
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, FORCE, 1);
            temp = (((*scPtr).size1000UI * 3) / 4000) + SHMOO_AND28_RD_DQS_VDL_OFFSET;       /* 75% + Offset */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP, VDL_STEP, temp);
            }
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_CHAN_DLY_CYC, FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_CHAN_DLY_CYC, CYCLES, SHMOO_AND28_WR_CYC_INIT);
            WRITE_DDR34_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
            }
        
            data = 0;
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, FORCE, 1);
            temp = (*scPtr).size1000UI / 2000;
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0, VDL_STEP, temp);
            }
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
            }
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
            }
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                {
                    WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                }
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
                if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
                {
                    WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
                }
            }
        
            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
            break;
        case SHMOO_AND28_RD_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }
            
            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND28_WR_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }
            
            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }
            
            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            _and28_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_AND28_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND28_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }
            
            for(i = 0; i < SHMOO_AND28_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_exit(int unit, int phy_ndx, and28_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential exit
     * Mode 1: Single exit
     */
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_AND28_RD_EN:
            break;
        case SHMOO_AND28_RD_EXTENDED:
            break;
        case SHMOO_AND28_WR_EXTENDED:
            break;
        case SHMOO_AND28_ADDR_EXTENDED:
            break;
        case SHMOO_AND28_CTRL_EXTENDED:
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (long unsigned int)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_save(int unit, int phy_ndx, and28_shmoo_config_param_t *config_param)
{
    uint32 data;
    
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[0] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[1] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[2] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[3] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[4] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[5] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[6] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[7] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[8] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[9] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[10] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[11] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[12] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[13] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[14] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, phy_ndx, &data);
    (*config_param).control_regs_ad[15] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, phy_ndx, &data);
    (*config_param).control_regs_ba[0] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, phy_ndx, &data);
    (*config_param).control_regs_ba[1] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, phy_ndx, &data);
    (*config_param).control_regs_ba[2] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, phy_ndx, &data);
    (*config_param).control_regs_aux[0] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, phy_ndx, &data);
    (*config_param).control_regs_aux[1] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, phy_ndx, &data);
    (*config_param).control_regs_aux[2] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, phy_ndx, &data);
    (*config_param).control_regs_cs[0] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, phy_ndx, &data);
    (*config_param).control_regs_cs[1] = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, phy_ndx, &data);
    (*config_param).control_regs_par = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, phy_ndx, &data);
    (*config_param).control_regs_ras_n = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, phy_ndx, &data);
    (*config_param).control_regs_cas_n = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, phy_ndx, &data);
    (*config_param).control_regs_cke = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, phy_ndx, &data);
    (*config_param).control_regs_rst_n = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, phy_ndx, &data);
    (*config_param).control_regs_odt = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, phy_ndx, &data);
    (*config_param).control_regs_we_n = (uint16) data;
    READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, &data);
    (*config_param).control_regs_vref_dac_control = data;
    
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, &data);
    }
    else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
    {
        READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQSr(unit, phy_ndx, &data);
    }
    (*config_param).wr_vdl_dqsp[0] = (uint16) data;
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dqsn[0] = (uint16) data;
    }
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][2] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][3] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][4] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][5] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][6] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[0][7] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dm[0] = (uint16) data;
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, &data);
        (*config_param).wr_vdl_edc[0] = (uint16) data;
    }
    READ_DDR34_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYCr(unit, phy_ndx, &data);
    (*config_param).wr_chan_dly_cyc[0] = (uint8) data;
    
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqsp[0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqsn[0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][2] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][3] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][4] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][5] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][6] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[0][7] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMPr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dmp[0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][2] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][3] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][4] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][5] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][6] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[0][7] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMNr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dmn[0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
    (*config_param).rd_en_vdl_cs[0][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, &data);
    (*config_param).rd_en_vdl_cs[0][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_RD_EN_DLY_CYCr(unit, phy_ndx, &data);
    (*config_param).rd_en_dly_cyc[0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_0_READ_CONTROLr(unit, phy_ndx, &data);
    (*config_param).rd_control[0] = (uint8) data;
    
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, &data);
    }
    else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
    {
        READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQSr(unit, phy_ndx, &data);
    }
    (*config_param).wr_vdl_dqsp[1] = (uint16) data;
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dqsn[1] = (uint16) data;
    }
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][2] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][3] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][4] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][5] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][6] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dq[1][7] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, &data);
    (*config_param).wr_vdl_dm[1] = (uint16) data;
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, &data);
        (*config_param).wr_vdl_edc[1] = (uint16) data;
    }
    READ_DDR34_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYCr(unit, phy_ndx, &data);
    (*config_param).wr_chan_dly_cyc[1] = (uint8) data;
    
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqsp[1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqsn[1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][2] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][3] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][4] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][5] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][6] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqp[1][7] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMPr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dmp[1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][2] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][3] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][4] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][5] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][6] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dqn[1][7] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMNr(unit, phy_ndx, &data);
    (*config_param).rd_vdl_dmn[1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
    (*config_param).rd_en_vdl_cs[1][0] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, &data);
    (*config_param).rd_en_vdl_cs[1][1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_RD_EN_DLY_CYCr(unit, phy_ndx, &data);
    (*config_param).rd_en_dly_cyc[1] = (uint16) data;
    READ_DDR34_PHY_BYTE_LANE_1_READ_CONTROLr(unit, phy_ndx, &data);
    (*config_param).rd_control[1] = (uint8) data;
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, &data);
        }
        else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
        {
            READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQSr(unit, phy_ndx, &data);
        }
        (*config_param).wr_vdl_dqsp[2] = (uint16) data;
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, &data);
            (*config_param).wr_vdl_dqsn[2] = (uint16) data;
        }
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][4] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][5] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][6] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[2][7] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dm[2] = (uint16) data;
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, &data);
            (*config_param).wr_vdl_edc[2] = (uint16) data;
        }
        READ_DDR34_PHY_BYTE_LANE_2_WR_CHAN_DLY_CYCr(unit, phy_ndx, &data);
        (*config_param).wr_chan_dly_cyc[2] = (uint8) data;
        
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqsp[2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqsn[2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][4] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][5] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][6] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[2][7] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMPr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dmp[2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][4] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][5] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][6] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[2][7] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMNr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dmn[2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
        (*config_param).rd_en_vdl_cs[2][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, &data);
        (*config_param).rd_en_vdl_cs[2][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_RD_EN_DLY_CYCr(unit, phy_ndx, &data);
        (*config_param).rd_en_dly_cyc[2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_2_READ_CONTROLr(unit, phy_ndx, &data);
        (*config_param).rd_control[2] = (uint8) data;
        
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, &data);
        }
        else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
        {
            READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQSr(unit, phy_ndx, &data);
        }
        (*config_param).wr_vdl_dqsp[3] = (uint16) data;
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, &data);
            (*config_param).wr_vdl_dqsn[3] = (uint16) data;
        }
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][4] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][5] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][6] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dq[3][7] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, &data);
        (*config_param).wr_vdl_dm[3] = (uint16) data;
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, &data);
            (*config_param).wr_vdl_edc[3] = (uint16) data;
        }
        READ_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DLY_CYCr(unit, phy_ndx, &data);
        (*config_param).wr_chan_dly_cyc[3] = (uint8) data;
        
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqsp[3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqsn[3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][4] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][5] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][6] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqp[3][7] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMPr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dmp[3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][2] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][4] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][5] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][6] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dqn[3][7] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMNr(unit, phy_ndx, &data);
        (*config_param).rd_vdl_dmn[3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, &data);
        (*config_param).rd_en_vdl_cs[3][0] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, &data);
        (*config_param).rd_en_vdl_cs[3][1] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, phy_ndx, &data);
        (*config_param).rd_en_dly_cyc[3] = (uint16) data;
        READ_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, phy_ndx, &data);
        (*config_param).rd_control[3] = (uint8) data;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_and28_restore(int unit, int phy_ndx, and28_shmoo_config_param_t *config_param)
{
    uint32 data;
   
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[0]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[1]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[2]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[3]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[4]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[5]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[6]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[7]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[8]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[9]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[10]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[11]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[12]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[13]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[14]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ad[15]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ba[0]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ba[1]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ba[2]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_aux[0]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_aux[1]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_aux[2]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cs[0]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cs[1]);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_par);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_ras_n);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cas_n);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_cke);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_rst_n);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_odt);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, phy_ndx, data);
    data = SET_ADDR_VDL_FORCE((uint32) (*config_param).control_regs_we_n);
    WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, phy_ndx, data);
    data = SET_VREF_DAC_CONTROL((*config_param).control_regs_vref_dac_control);
    WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, phy_ndx, data);
    
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[0]);
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, data);
    }
    else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
    {
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQSr(unit, phy_ndx, data);
    }
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[0]);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, data);
    }
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][1]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][2]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][3]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][4]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][5]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][6]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[0][7]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[0]);
        WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
    }
    data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
    
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][1]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][2]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][3]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][4]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][5]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][6]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[0][7]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][1]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][2]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][3]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][4]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][5]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][6]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[0][7]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[0][0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[0][1]);
    WRITE_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
    data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_RD_EN_DLY_CYCr(unit, phy_ndx, data);
    data = SET_RD_CONTROL((uint32) (*config_param).rd_control[0]);
    WRITE_DDR34_PHY_BYTE_LANE_0_READ_CONTROLr(unit, phy_ndx, data);
    
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[1]);
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, data);
    }
    else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
    {
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQSr(unit, phy_ndx, data);
    }
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[1]);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, data);
    }
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][0]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][2]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][3]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][4]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][5]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][6]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[1][7]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
    data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
    if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
    {
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[1]);
        WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
    }
    data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
    
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][0]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][2]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][3]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][4]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][5]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][6]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[1][7]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][0]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][2]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][3]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][4]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][5]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][6]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[1][7]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[1][0]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
    data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[1][1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
    data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_RD_EN_DLY_CYCr(unit, phy_ndx, data);
    data = SET_RD_CONTROL((uint32) (*config_param).rd_control[1]);
    WRITE_DDR34_PHY_BYTE_LANE_1_READ_CONTROLr(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[2]);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, data);
        }
        else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQSr(unit, phy_ndx, data);
        }
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[2]);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, data);
        }
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][0]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][1]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][3]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][4]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][5]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][6]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[2][7]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[2]);
            WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
        }
        data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
        
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][0]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][1]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][3]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][4]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][5]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][6]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[2][7]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][0]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][1]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][3]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][4]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][5]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][6]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[2][7]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[2][0]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[2][1]);
        WRITE_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
        data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_RD_EN_DLY_CYCr(unit, phy_ndx, data);
        data = SET_RD_CONTROL((uint32) (*config_param).rd_control[2]);
        WRITE_DDR34_PHY_BYTE_LANE_2_READ_CONTROLr(unit, phy_ndx, data);
        
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsp[3]);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_Pr(unit, phy_ndx, data);
        }
        else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
        {
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQSr(unit, phy_ndx, data);
        }
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dqsn[3]);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_Nr(unit, phy_ndx, data);
        }
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][0]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][1]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][2]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][4]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][5]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][6]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dq[3][7]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7r(unit, phy_ndx, data);
        data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_dm[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DMr(unit, phy_ndx, data);
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            data = SET_WR_VDL_FORCE((uint32) (*config_param).wr_vdl_edc[3]);
            WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_EDCr(unit, phy_ndx, data);
        }
        data = SET_WR_CHAN_DLY_CYC_FORCE((uint32) (*config_param).wr_chan_dly_cyc[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DLY_CYCr(unit, phy_ndx, data);
        
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsp[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSPr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqsn[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSNr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][0]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][1]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][2]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][4]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][5]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][6]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqp[3][7]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Pr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmp[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMPr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][0]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][1]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][2]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][4]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][5]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][6]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dqn[3][7]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7Nr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_vdl_dmn[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DMNr(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[3][0]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, phy_ndx, data);
        data = SET_RD_VDL_FORCE((uint32) (*config_param).rd_en_vdl_cs[3][1]);
        WRITE_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, phy_ndx, data);
        data = SET_RD_EN_DLY_CYC_FORCE((uint32) (*config_param).rd_en_dly_cyc[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, phy_ndx, data);
        data = SET_RD_CONTROL((uint32) (*config_param).rd_control[3]);
        WRITE_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, phy_ndx, data);
    }

    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    data = 0;
    DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
    WRITE_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, phy_ndx, data);
    WRITE_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, phy_ndx, data);
    
    if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
    {
        WRITE_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, phy_ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, phy_ndx, data);
    }

    sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_and28_shmoo_ctl
 * Purpose:
 *      Perform shmoo (PHY calibration) on specific DRC index.
 * Parameters:
 *      unit                - unit number
 *      phy_ndx             - DRC index to perform shmoo on.
 *      shmoo_type          - Selects shmoo sub-section to be performs (-1 for full shmoo)
 *      flags               -   SHMOO_AND28_CTL_FLAGS_STAT_BIT            
 *									0: Runs normal shmoo functions
 *									1: Doesn't run shmoo. Only prints statistics. (Nothing is printed at the moment)
 * 								SHMOO_AND28_CTL_FLAGS_PLOT_BIT            
 *									0: No shmoo plots are printed during shmoo
 *									1: Shmoo plots are printed during shmoo
 * 								SHMOO_AND28_CTL_FLAGS_EXT_VREF_RANGE_BIT (Not implemented at the moment)
 *									0: Shmoo runs with a regular range for Vref sweep in *EXTENDED shmoos
 *									1: Shmoo runs with an extended range for Vref sweep in *EXTENDED shmoos
 *      action              - Save/restore functionality
 *      *config_param       - PHY configuration saved/restored
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 */

int
soc_and28_shmoo_ctl(int unit, int phy_ndx, int shmoo_type, uint32 flags, int action, and28_shmoo_config_param_t *config_param)
{
    and28_shmoo_container_t *scPtr = NULL;
    uint32 dramType;
    uint32 ctlType;
    uint32 i;
    int ndx, ndxEnd;
	uint32 stat, plot;
    const uint32 *seqPtr;
    uint32 seqCount;

    dramType = shmoo_dram_info.dram_type;
    ctlType = shmoo_dram_info.ctl_type;
    
    stat = flags & SHMOO_AND28_CTL_FLAGS_STAT_BIT;
    plot = flags & SHMOO_AND28_CTL_FLAGS_PLOT_BIT;
    
    if(!stat)
    {
        scPtr = sal_alloc(sizeof(and28_shmoo_container_t), "AND28 Shmoo Container");
        if(scPtr == NULL)
        {
            return SOC_E_MEMORY;
        }
        sal_memset(scPtr, 0, sizeof(and28_shmoo_container_t));
        
        (*scPtr).debugMode = flags & SHMOO_AND28_DEBUG_MODE_FLAG_BITS;
        
        if(phy_ndx != SHMOO_AND28_INTERFACE_RSVP)
        {
            ndx = phy_ndx;
            ndxEnd = phy_ndx + 1;
        }
        else
        {
            ndx = 0;
            ndxEnd = SHMOO_AND28_MAX_INTERFACES;
        }
        
        for(; ndx < ndxEnd; ndx++)
        {
            if(!_shmoo_and28_check_dram(ndx)) {
                continue;
            }
            
            (*scPtr).restore = 0;
            
            if(action == SHMOO_AND28_ACTION_RESTORE)
            {
                (*scPtr).restore = 1;
                switch(ctlType)
                {
                    case SHMOO_AND28_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_AND28_CTL_TYPE_1:
                        _shmoo_and28_restore(unit, phy_ndx, config_param);
                        break;
                    default:
                        if(scPtr != NULL)
                        {
                            sal_free(scPtr);
                        }
                        
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02lu\n"), (long unsigned int)ctlType));
                        return SOC_E_FAIL;
                }
            }
            else if((action == SHMOO_AND28_ACTION_RUN) || (action == SHMOO_AND28_ACTION_RUN_AND_SAVE))
            {
                switch(ctlType)
                {
                    case SHMOO_AND28_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_AND28_CTL_TYPE_1:
                        switch(dramType)
                        {
                            case SHMOO_AND28_DRAM_TYPE_DDR3:
                                seqPtr = &shmoo_order_and28_ddr3[0];
                                seqCount = SHMOO_AND28_DDR3_SEQUENCE_COUNT;
                                break;
                            case SHMOO_AND28_DRAM_TYPE_DDR3L:
                                seqPtr = &shmoo_order_and28_ddr3l[0];
                                seqCount = SHMOO_AND28_DDR3L_SEQUENCE_COUNT;
                                break;
                            case SHMOO_AND28_DRAM_TYPE_DDR4:
                                seqPtr = &shmoo_order_and28_ddr4[0];
                                seqCount = SHMOO_AND28_DDR4_SEQUENCE_COUNT;
                                break;
                            default:
                                if(scPtr != NULL)
                                {
                                    sal_free(scPtr);
                                }
                                
                                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported dram type: %02lu\n"), (long unsigned int)dramType));
                                return SOC_E_FAIL;
                        }
                        
                        (*scPtr).dramType = dramType;
                        (*scPtr).ctlType = ctlType;
                        
                        if(shmoo_type != SHMOO_AND28_SHMOO_RSVP)
                        {
                            (*scPtr).shmooType = shmoo_type;
                            _shmoo_and28_entry(unit, ndx, scPtr, SHMOO_AND28_SINGLE);
                            _shmoo_and28_do(unit, ndx, scPtr);
                            _shmoo_and28_calib_2D(unit, ndx, scPtr);
                            _shmoo_and28_set_new_step(unit, ndx, scPtr);
                            if(plot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO))
                            {
                                _shmoo_and28_plot(unit, ndx, scPtr);
                            }
                            _shmoo_and28_exit(unit, ndx, scPtr, SHMOO_AND28_SINGLE);
                        }
                        else
                        {
                            for(i = 0; i < seqCount; i++)
                            {
                                (*scPtr).shmooType = seqPtr[i];
                                _shmoo_and28_entry(unit, ndx, scPtr, SHMOO_AND28_SEQUENTIAL);
                                _shmoo_and28_do(unit, ndx, scPtr);
                                _shmoo_and28_calib_2D(unit, ndx, scPtr);
                                _shmoo_and28_set_new_step(unit, ndx, scPtr);
                                if(plot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO))
                                {
                                    _shmoo_and28_plot(unit, ndx, scPtr);
                                }
                                _shmoo_and28_exit(unit, ndx, scPtr, SHMOO_AND28_SEQUENTIAL);
                            }
                        }
                        
                        break;
                    default:
                        if(scPtr != NULL)
                        {
                            sal_free(scPtr);
                        }
                        
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02lu\n"), (long unsigned int)ctlType));
                        return SOC_E_FAIL;
                }
            }
            
            if((action == SHMOO_AND28_ACTION_RUN_AND_SAVE) || (action == SHMOO_AND28_ACTION_SAVE))
            {
                _shmoo_and28_save(unit, phy_ndx, config_param);
            }
        }
        
        if(scPtr != NULL)
        {
            sal_free(scPtr);
        }
        
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DDR Tuning Complete\n")));
    }
    else
    {
        /* Report only */
        switch(ctlType)
        {
            case SHMOO_AND28_CTL_TYPE_RSVP:
                break;
            case SHMOO_AND28_CTL_TYPE_1:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02lu\n"), (long unsigned int)ctlType));
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/* Set Dram Parameters/Info to Shmoo driver */ 
int
soc_and28_shmoo_dram_info_set(int unit, CONST and28_shmoo_dram_info_t *sdi)
{
    shmoo_dram_info.ctl_type = sdi->ctl_type;
    shmoo_dram_info.dram_type = sdi->dram_type;
    shmoo_dram_info.dram_bitmap = sdi->dram_bitmap;
    shmoo_dram_info.interface_bitwidth = sdi->interface_bitwidth;
    shmoo_dram_info.split_bus = sdi->split_bus;
    shmoo_dram_info.num_columns = sdi->num_columns;
    shmoo_dram_info.num_rows = sdi->num_rows;
    shmoo_dram_info.num_banks = sdi->num_banks;
    shmoo_dram_info.num_bank_groups = sdi->num_bank_groups;
    shmoo_dram_info.data_rate_mbps = sdi->data_rate_mbps;
    shmoo_dram_info.ref_clk_mhz = sdi->ref_clk_mhz;
    shmoo_dram_info.refi = sdi->refi;
    shmoo_dram_info.command_parity_latency = sdi->command_parity_latency;
    shmoo_dram_info.sim_system_mode = sdi->sim_system_mode;
    
    return SOC_E_NONE;
}

/* Configure PHY PLL and wait for lock */
int
_and28_phy_cfg_pll(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data;
    uint32 timeout;
    uint32 pll_config;
    uint32 pll_control2;
    uint32 pll_dividers;
    uint32 pll_frac_divider;
    
    if(shmoo_dram_info.ref_clk_mhz != 50)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Unsupported reference flock frequency: %4d MHz\n"), shmoo_dram_info.ref_clk_mhz));
        return SOC_E_FAIL;
    }
    
    pll_config = 0x00000000;
    switch(shmoo_dram_info.data_rate_mbps)
    {
        case 800:
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                pll_config = 0x018D0012;
            }
            else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
            {
                pll_config = 0x01890012;
            }
            pll_control2 = 0x94000000;
            pll_dividers = 0x004030C0;
            pll_frac_divider = 0x00000000;
            break;
        case 1066:
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                pll_config = 0x018D0012;
            }
            else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
            {
                pll_config = 0x01890012;
            }
            pll_control2 = 0x94000000;
            pll_dividers = 0x003030BF;
            pll_frac_divider = 0x000E147A;
            break;
        case 1333:
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                pll_config = 0x018D0012;
            }
            else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
            {
                pll_config = 0x01890012;
            }
            pll_control2 = 0x94000000;
            pll_dividers = 0x002030A0;
            pll_frac_divider = 0x000147AE;
            break;
        case 1600:
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                pll_config = 0x018D0012;
            }
            else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
            {
                pll_config = 0x01890012;
            }
            pll_control2 = 0x94000000;
            pll_dividers = 0x002030C0;
            pll_frac_divider = 0x00000000;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Unsupported data rate: %4d Mbps\n"), shmoo_dram_info.data_rate_mbps));
            return SOC_E_FAIL;
    }
    
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PHY PLL Configuration\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Fref.............: %4d MHz\n"), shmoo_dram_info.ref_clk_mhz));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Data rate........: %4d Mbps\n"), shmoo_dram_info.data_rate_mbps));

    if(phy_ndx != SHMOO_AND28_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_AND28_MAX_INTERFACES;
    }
    
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_and28_check_dram(ndx))
        {
            continue;
        }
        
        WRITE_DDR34_PHY_CONTROL_REGS_PLL_CONFIGr(unit, phy_ndx, pll_config);
        WRITE_DDR34_PHY_CONTROL_REGS_PLL_CONTROL2r(unit, phy_ndx, pll_control2);
        WRITE_DDR34_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit, phy_ndx, pll_dividers);
        WRITE_DDR34_PHY_CONTROL_REGS_PLL_FRAC_DIVIDERr(unit, phy_ndx, pll_frac_divider);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        READ_DDR34_PHY_CONTROL_REGS_PLL_CONFIGr(unit, phy_ndx, &data);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, PLL_CONFIG, RESET, 0);
        WRITE_DDR34_PHY_CONTROL_REGS_PLL_CONFIGr(unit, phy_ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        timeout = 2000;
        do
        {
            READ_DDR34_PHY_CONTROL_REGS_PLL_STATUSr(unit, phy_ndx, &data);
            
            if(DDR34_GET_FIELD(data, DDR34_PHY_CONTROL_REGS, PLL_STATUS, LOCK))
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PLL locked.\n")));
                break;
            }
            
            if (timeout == 0)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PLL not locked!!! (Timeout)\n")));
                return SOC_E_TIMEOUT;
            }
            
            timeout--;
            sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        }
        while(TRUE);
        
        READ_DDR34_PHY_CONTROL_REGS_PLL_CONFIGr(unit, phy_ndx, &data);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, PLL_CONFIG, RESET_POST_DIV, 0);
        WRITE_DDR34_PHY_CONTROL_REGS_PLL_CONFIGr(unit, phy_ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
    }

    return SOC_E_NONE;
}

int
soc_and28_shmoo_phy_init(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data;
    uint32 dfi_ctrl;
    uint32 dram_config;
    uint32 dram_timing1;
    uint32 dram_timing2;
    uint32 dram_timing3;
    uint32 dram_timing4;
    uint32 step1000, size1000UI, sizeUI;
    and28_step_size_t ss;
    
    if(phy_ndx != SHMOO_AND28_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_AND28_MAX_INTERFACES;
    }
    
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_and28_check_dram(ndx))
        {
            continue;
        }
        
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization (PHY index: %02d)\n"), ndx));
        
/*A00*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A00. Detect PHY type\n")));
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_RSVP)
        {
            DDR34_REG_READ(unit, ndx, 0x00, 0x00000000, &data);
            SHMOO_AND28_PHY_TYPE = (data >> 8) & 0xFF;
            
            switch(SHMOO_AND28_PHY_TYPE)
            {
                case SHMOO_AND28_PHY_TYPE_E0:
                    INIT_DDR34_E0_PHY_REGS_TABLE();
                    INIT_DDR34_E0_PHY_FIELDS_TABLE();
                    break;
                case SHMOO_AND28_PHY_TYPE_F0:
                    INIT_DDR34_F0_PHY_REGS_TABLE();
                    INIT_DDR34_F0_PHY_FIELDS_TABLE();
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported PHY type: %02X\n\n"), SHMOO_AND28_PHY_TYPE));
                    return SOC_E_FAIL;
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PHY type.........: %02X\n"), SHMOO_AND28_PHY_TYPE));
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A01*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A01. Acquire PHY control\n")));
        dfi_ctrl = 0;
        DDR34_SET_FIELD(dfi_ctrl, DDR34_PHY_CONTROL_REGS, DFI_CNTRL, ASSERT_REQ, 1);
        DDR34_SET_FIELD(dfi_ctrl, DDR34_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CS0, 1);
        DDR34_SET_FIELD(dfi_ctrl, DDR34_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CS1, 1);
        DDR34_SET_FIELD(dfi_ctrl, DDR34_PHY_CONTROL_REGS, DFI_CNTRL, DFI_CKE, 0);
        WRITE_DDR34_PHY_CONTROL_REGS_DFI_CNTRLr(unit, ndx, dfi_ctrl);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A02*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A02. Configure timing parameters\n")));
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            switch(shmoo_dram_info.data_rate_mbps)
            {
                case 800:
                    dram_timing1 = 0x0F040606;
                    dram_timing2 = 0x04060506;
                    dram_timing3 = 0x00044068;
                    dram_timing4 = 0x00000000;
                    break;
                case 1066:
                    dram_timing1 = 0x14040707;
                    dram_timing2 = 0x04080607;
                    dram_timing3 = 0x0004408C;
                    dram_timing4 = 0x00000000;
                    break;
                case 1333:
                    dram_timing1 = 0x18050909;
                    dram_timing2 = 0x050A0709;
                    dram_timing3 = 0x000450B0;
                    dram_timing4 = 0x00000000;
                    break;
                case 1600:
                    if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
                    {
                        dram_timing1 = 0x1C000C0C;
                        switch(shmoo_dram_info.num_bank_groups) 
                        {
                            case 2:     dram_timing1 |= 0x00060000; break;
                            case 4:     dram_timing1 |= 0x00050000; break;
                            default:
                                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported number of bank groups: %d\n"), shmoo_dram_info.num_bank_groups));
                                return SOC_E_FAIL;
                        }
                        dram_timing2 = 0x060C090A;
                        dram_timing3 = 0x003460D0;
                        dram_timing4 = 0x00000000;
                    }
                    else
                    {
                        dram_timing1 = 0x1C060B0B;
                        dram_timing2 = 0x060C080B;
                        dram_timing3 = 0x000460D0;
                        dram_timing4 = 0x00000000;
                    }
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported data rate: %4d Mbps\n"), shmoo_dram_info.data_rate_mbps));
                    return SOC_E_FAIL;
            }
        }
        else
        {
            switch(shmoo_dram_info.data_rate_mbps)
            {
                case 800:
                    dram_timing1 = 0x0F040606;
                    dram_timing2 = 0x04060506;
                    dram_timing3 = 0x00044068;
                    dram_timing4 = 0x00000000;
                    break;
                case 1066:
                    dram_timing1 = 0x14040707;
                    dram_timing2 = 0x04080607;
                    dram_timing3 = 0x0004408C;
                    dram_timing4 = 0x00000000;
                    break;
                case 1333:
                    dram_timing1 = 0x18040909;
                    dram_timing2 = 0x050A0709;
                    dram_timing3 = 0x000450B0;
                    dram_timing4 = 0x00000000;
                    break;
                case 1600:
                    if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
                    {
                        dram_timing1 = 0x1C000C0C;
                        switch(shmoo_dram_info.num_bank_groups) 
                        {
                            case 2:     dram_timing1 |= 0x00060000; break;
                            case 4:     dram_timing1 |= 0x00050000; break;
                            default:
                                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported number of bank groups: %d\n"), shmoo_dram_info.num_bank_groups));
                                return SOC_E_FAIL;
                        }
                        dram_timing2 = 0x060C090A;
                        dram_timing3 = 0x003460D0;
                        dram_timing4 = 0x00000000;
                    }
                    else
                    {
                        dram_timing1 = 0x1C050B0B;
                        dram_timing2 = 0x060C080B;
                        dram_timing3 = 0x000460D0;
                        dram_timing4 = 0x00000000;
                    }
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported data rate: %4d Mbps\n"), shmoo_dram_info.data_rate_mbps));
                    return SOC_E_FAIL;
            }
        }
        
        dram_config = 0x00001000;
        if(shmoo_dram_info.split_bus)
        {
            dram_config |= 0x04000000;
            if(shmoo_dram_info.interface_bitwidth <= 8)
            {
                dram_config |= 0x02000000;
            }
        }
        else
        {
            if(shmoo_dram_info.interface_bitwidth <= 16)
            {
                dram_config |= 0x02000000;
            }
            if(shmoo_dram_info.interface_bitwidth <= 8)
            {
                dram_config |= 0x01000000;
            }
        }
        switch(shmoo_dram_info.dram_type) 
        {
            case SHMOO_AND28_DRAM_TYPE_DDR3:    dram_config |= 0x00000000; break;
            case SHMOO_AND28_DRAM_TYPE_DDR3L:   dram_config |= 0x00000000; break;
            case SHMOO_AND28_DRAM_TYPE_DDR4:    dram_config |= 0x00000001; break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported dram type: %d\n"), shmoo_dram_info.dram_type));
                return SOC_E_FAIL;
        }
        switch(shmoo_dram_info.num_rows) 
        {
            case 4096:  dram_config |= 0x00000000; break;
            case 8192:  dram_config |= 0x00000010; break;
            case 16384: dram_config |= 0x00000020; break;
            case 32768: dram_config |= 0x00000030; break;
            case 65536: dram_config |= 0x00000040; break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported number of rows: %d\n"), shmoo_dram_info.num_rows));
                return SOC_E_FAIL;
        }
        switch(shmoo_dram_info.num_columns) 
        {
            case 512:   dram_config |= 0x00000000; break;
            case 1024:  dram_config |= 0x00000100; break;
            case 2048:  dram_config |= 0x00000200; break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported number of columns: %d\n"), shmoo_dram_info.num_columns));
                return SOC_E_FAIL;
        }
        switch(shmoo_dram_info.num_banks) 
        {
            case 4:     dram_config |= 0x00000000; break;
            case 8:     dram_config |= 0x00000400; break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported number of banks: %d\n"), shmoo_dram_info.num_banks));
                return SOC_E_FAIL;
        }
        switch(shmoo_dram_info.num_bank_groups) 
        {
            case 2:     dram_config |= 0x00000000; break;
            case 4:     dram_config |= 0x00001000; break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported number of bank groups: %d\n"), shmoo_dram_info.num_bank_groups));
                return SOC_E_FAIL;
        }
        
        WRITE_DDR34_PHY_CONTROL_REGS_DRAM_CONFIGr(unit, ndx, dram_config);
        WRITE_DDR34_PHY_CONTROL_REGS_DRAM_TIMING1r(unit, ndx, dram_timing1);
        WRITE_DDR34_PHY_CONTROL_REGS_DRAM_TIMING2r(unit, ndx, dram_timing2);
        WRITE_DDR34_PHY_CONTROL_REGS_DRAM_TIMING3r(unit, ndx, dram_timing3);
        WRITE_DDR34_PHY_CONTROL_REGS_DRAM_TIMING4r(unit, ndx, dram_timing4);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A03*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A03. Configure PHY PLL\n")));
        _and28_phy_cfg_pll(unit, ndx);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A04*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A04. Configure reference voltage\n")));
/*R04*/ READ_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ndx, &data);
        if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 44);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC0, 32);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, DAC1, 32);
        }
        WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN0, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN1, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN2, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VREF_DAC_CONTROL, PDN3, 0);
        WRITE_DDR34_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A05*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A05. Compute VDL step size\n")));
        _and28_calculate_step_size(unit, ndx, &ss);
        
        step1000 = ss.step1000;
        size1000UI = ss.size1000UI;
        sizeUI = size1000UI / 1000;
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration complete.\n")));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL step size....: %3lu.%03lu ps\n"), (long unsigned int)(step1000 / 1000), (long unsigned int)(step1000 % 1000)));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     UI size..........: %3lu.%03lu steps\n"), (long unsigned int)sizeUI, (long unsigned int)(size1000UI % 1000)));
        
/*A06*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A06. Configure ADDR/CTRL VDLs\n")));
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, FORCE, 1);
        if(sizeUI > SHMOO_AND28_MAX_VDL_LENGTH)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, SHMOO_AND28_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, sizeUI);
        }
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, ndx, data);
        WRITE_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, ndx, data);
        
/*A07*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A07. Disable Virtual VTT\n")));
        READ_DDR34_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROLr(unit, ndx, &data);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CTL_IDLE, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CS_IDLE, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CKE_IDLE, 0);
        WRITE_DDR34_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROLr(unit, ndx, data);
        
/*A08*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A08. ZQ calibration\n")));
/*R08*/ if(shmoo_dram_info.sim_system_mode)
        {
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Skipped for emulation\n")));
            
            goto SHMOO_AND28_ZQ_CALIBRATION_END;
        }
        
        _and28_zq_calibration(unit, ndx);
        
        SHMOO_AND28_ZQ_CALIBRATION_END:
        
/*A09*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A09. Configure Static Pad Control\n")));
        READ_DDR34_PHY_CONTROL_REGS_STATIC_PAD_CTLr(unit, ndx, &data);
        if(shmoo_dram_info.split_bus)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_CLK1, 0);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_CLK1, 1);
        }
        if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, AUTO_OEB, 0);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, AUTO_OEB, 1);
        }
        if(shmoo_dram_info.split_bus)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_CS1, 0);
        }
        else
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_CS1, 1);
        }
        WRITE_DDR34_PHY_CONTROL_REGS_STATIC_PAD_CTLr(unit, ndx, data);
        
        if(shmoo_dram_info.dram_type == SHMOO_AND28_DRAM_TYPE_DDR4)
        {
            READ_DDR34_PHY_BYTE_LANE_0_STATIC_PAD_CTLr(unit, ndx, &data);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, STATIC_PAD_CTL, DM_INVERT, 0);
            WRITE_DDR34_PHY_BYTE_LANE_0_STATIC_PAD_CTLr(unit, ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_1_STATIC_PAD_CTLr(unit, ndx, data);
            
            if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
            {
                WRITE_DDR34_PHY_BYTE_LANE_2_STATIC_PAD_CTLr(unit, ndx, data);
                WRITE_DDR34_PHY_BYTE_LANE_3_STATIC_PAD_CTLr(unit, ndx, data);
            }
            
            READ_DDR34_PHY_CONTROL_REGS_ABI_PAR_CNTRLr(unit, ndx, &data);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, ABI_PAR_CNTRL, PAR_ENABLE, 1);
            WRITE_DDR34_PHY_CONTROL_REGS_ABI_PAR_CNTRLr(unit, ndx, data);
        }
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A10*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A10. Configure ODT\n")));
        data = 0;
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT_FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT_FORCE_VALUE, 1);
        }
        else if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
        {
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT0_FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT0_FORCE_VALUE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT1_FORCE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT1_FORCE_VALUE, 1);
        }
        WRITE_DDR34_PHY_CONTROL_REGS_WRITE_ODT_CNTRLr(unit, ndx, data);
        
        data = 0;
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_ENABLE, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_DELAY, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_POST_LENGTH, 2);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_PRE_LENGTH, 4);
        WRITE_DDR34_PHY_BYTE_LANE_0_ODT_CONTROLr(unit, ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_ODT_CONTROLr(unit, ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_ODT_CONTROLr(unit, ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_ODT_CONTROLr(unit, ndx, data);
        }
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A11*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A11. Configure Write Pre-/Post-amble\n")));
        READ_DDR34_PHY_BYTE_LANE_0_WR_PREAMBLE_MODEr(unit, ndx, &data);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQ_POSTAM_BITS, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQ_PREAM_BITS, 1);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS, 0xE);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS_POSTAM_BITS, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS_PREAM_BITS, 2);
        WRITE_DDR34_PHY_BYTE_LANE_0_WR_PREAMBLE_MODEr(unit, ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_WR_PREAMBLE_MODEr(unit, ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth == 32) || (_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            WRITE_DDR34_PHY_BYTE_LANE_2_WR_PREAMBLE_MODEr(unit, ndx, data);
            WRITE_DDR34_PHY_BYTE_LANE_3_WR_PREAMBLE_MODEr(unit, ndx, data);
        }
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
/*A12*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A12. Configure Auto Idle\n")));
        READ_DDR34_PHY_BYTE_LANE_0_IDLE_PAD_CONTROLr(unit, ndx, &data);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, AUTO_DQ_RXENB_MODE, 0);
        DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, AUTO_DQ_IDDQ_MODE, 0);
        WRITE_DDR34_PHY_BYTE_LANE_0_IDLE_PAD_CONTROLr(unit, ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_1_IDLE_PAD_CONTROLr(unit, ndx, data);
        
        if((shmoo_dram_info.interface_bitwidth != 32) && !(_shmoo_and28_check_dram(phy_ndx) > 1))
        {
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, IDLE, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, RXENB, 1);
            DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, IDDQ, 1);
        
            if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_E0)
            {
                DDR34_SET_FIELD(data, DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, IO_IDLE_ENABLE, 0x7FF);
            }
        }
        WRITE_DDR34_PHY_BYTE_LANE_2_IDLE_PAD_CONTROLr(unit, ndx, data);
        WRITE_DDR34_PHY_BYTE_LANE_3_IDLE_PAD_CONTROLr(unit, ndx, data);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        if(SHMOO_AND28_PHY_TYPE == SHMOO_AND28_PHY_TYPE_F0)
        {
            WRITE_DDR34_PHY_CONTROL_REGS_STANDBY_CONTROLr(unit, ndx, 0);
        }

/*A13*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A13. Release PHY control\n")));
        WRITE_DDR34_PHY_CONTROL_REGS_STANDBY_CONTROLr(unit, ndx, 0);
        DDR34_SET_FIELD(dfi_ctrl, DDR34_PHY_CONTROL_REGS, DFI_CNTRL, ASSERT_REQ, 0);
        WRITE_DDR34_PHY_CONTROL_REGS_DFI_CNTRLr(unit, ndx, dfi_ctrl);
        sal_usleep(SHMOO_AND28_SHORT_SLEEP);
        
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization complete (PHY index: %02d)\n"), ndx));
    }
    
    return SOC_E_NONE;
}

/* Register Device interface CB to shmoo */
int
soc_and28_shmoo_interface_cb_register(int unit, and28_shmoo_cbi_t shmoo_cbi)
{
    soc_and28_phy_reg_read = shmoo_cbi.and28_phy_reg_read;
    soc_and28_phy_reg_write = shmoo_cbi.and28_phy_reg_write;
    soc_and28_phy_reg_modify = shmoo_cbi.and28_phy_reg_modify;
    _shmoo_and28_drc_bist_conf_set = shmoo_cbi.and28_drc_bist_conf_set;
    _shmoo_and28_drc_bist_err_cnt = shmoo_cbi.and28_drc_bist_err_cnt;
    _shmoo_and28_drc_dram_init = shmoo_cbi.and28_drc_dram_init;

    if(!_shmoo_and28_validate_cbi())
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Null callback function detected during CBI registration\n")));
        return SOC_E_FAIL;
    }
    
    return SOC_E_NONE;
}

#endif /* BCM_AND28_SUPPORT */
