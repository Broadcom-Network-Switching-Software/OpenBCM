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

#ifdef BCM_AND16_SUPPORT
#define SOC_DDR4_SUPPORT
#define CHIP_IS_HURRICANE4 1
#define CONFIG_HURRICANE4  1
#include <soc/shmoo_and16.h>
#include <soc/phy/phy_and16_k1.h>
#include <soc/phy/ydc_ddr_bist.h>

static and16_shmoo_dram_info_t shmoo_dram_info;

const uint32 shmoo_order_and16_ddr3[SHMOO_AND16_DDR3_ECC_32P4_SEQUENCE_COUNT] =
{
    SHMOO_AND16_RD_EN,
    SHMOO_AND16_RD_EXTENDED,
    SHMOO_AND16_WR_EXTENDED,
    SHMOO_AND16_ADDR_EXTENDED,
    SHMOO_AND16_CTRL_EXTENDED,
    SHMOO_AND16_RD_EN_ECC,
    SHMOO_AND16_RD_EXTENDED_ECC,
    SHMOO_AND16_WR_EXTENDED_ECC
};

const uint32 shmoo_order_and16_ddr3l[SHMOO_AND16_DDR3L_ECC_32P4_SEQUENCE_COUNT] =
{
    SHMOO_AND16_RD_EN,
    SHMOO_AND16_RD_EXTENDED,
    SHMOO_AND16_WR_EXTENDED,
    SHMOO_AND16_ADDR_EXTENDED,
    SHMOO_AND16_CTRL_EXTENDED,
    SHMOO_AND16_RD_EN_ECC,
    SHMOO_AND16_RD_EXTENDED_ECC,
    SHMOO_AND16_WR_EXTENDED_ECC
};

const uint32 shmoo_order_and16_ddr4[SHMOO_AND16_DDR4_ECC_32P4_SEQUENCE_COUNT] =
{
    SHMOO_AND16_RD_EN,
    SHMOO_AND16_RD_EXTENDED,
    SHMOO_AND16_WR_EXTENDED,
    SHMOO_AND16_ADDR_EXTENDED,
    SHMOO_AND16_CTRL_EXTENDED,
    SHMOO_AND16_RD_EN_ECC,
    SHMOO_AND16_RD_EXTENDED_ECC,
    SHMOO_AND16_WR_EXTENDED_ECC
};

uint32
_shmoo_and16_check_dram(int phy_ndx)
{
    return ((shmoo_dram_info.dram_bitmap >> phy_ndx) & 0x1);
}

STATIC int
_shmoo_and16_initialize_bist(int unit, int phy_ndx, int bit, and16_shmoo_container_t *scPtr)
{
    ydc_ddr_bist_info_t bist_info;

    sal_memset(&bist_info, 0, sizeof(ydc_ddr_bist_info_t));

    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
        case SHMOO_AND16_RD_EXTENDED:
        case SHMOO_AND16_WR_EXTENDED:
        case SHMOO_AND16_ADDR_EXTENDED:
        case SHMOO_AND16_CTRL_EXTENDED:
        case SHMOO_AND16_RD_EN_ECC:
        case SHMOO_AND16_RD_EXTENDED_ECC:
        case SHMOO_AND16_WR_EXTENDED_ECC:
            bist_info.write_weight = 64;
            bist_info.read_weight = 64;
            bist_info.bist_timer_us = 0;
            bist_info.bist_num_actions = 65536;
            bist_info.bist_start_address = 0x00000000;
            bist_info.bist_end_address = 0x00FFFFFF;
            bist_info.mpr_mode = 0;
            bist_info.prbs_mode = 1;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"),(unsigned long) (*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    return soc_ydc_ddr_bist_config_set(unit, phy_ndx, &bist_info);
}

STATIC int
_shmoo_and16_run_bist(int unit, int phy_ndx, and16_shmoo_container_t *scPtr, and16_shmoo_error_array_t *seaPtr)
{
    ydc_ddr_bist_err_cnt_t be;

    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
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
        case SHMOO_AND16_RD_EXTENDED:
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
        case SHMOO_AND16_WR_EXTENDED:
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
        case SHMOO_AND16_ADDR_EXTENDED:
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
        case SHMOO_AND16_CTRL_EXTENDED:
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
        case SHMOO_AND16_RD_EN_ECC:
            iproc16_clear_ecc_syndrome(unit);
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(be.bist_err_occur || iproc16_read_ecc_syndrome(unit))
            {
                (*seaPtr)[0] = 0xFFFFFFFF;
            }
            else
            {
                (*seaPtr)[0] = 0x00000000;
            }
            break;
        case SHMOO_AND16_RD_EXTENDED_ECC:
            iproc16_clear_ecc_syndrome(unit);
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(be.bist_err_occur || iproc16_read_ecc_syndrome(unit))
            {
                (*seaPtr)[0] = 0xFFFFFFFF;
            }
            else
            {
                (*seaPtr)[0] = 0x00000000;
            }
            break;
        case SHMOO_AND16_WR_EXTENDED_ECC:
            iproc16_clear_ecc_syndrome(unit);
            SOC_IF_ERROR_RETURN(soc_ydc_ddr_bist_run(unit, phy_ndx, &be));

            if(be.bist_err_occur || iproc16_read_ecc_syndrome(unit))
            {
                (*seaPtr)[0] = 0xFFFFFFFF;
            }
            else
            {
                (*seaPtr)[0] = 0x00000000;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"),(unsigned long) (*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_rd_en(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 jump;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data;
    and16_shmoo_error_array_t sea;

    yCapMin = 0;
    yCapMax = (*scPtr).sizeY;
    jump = (*scPtr).yJump;
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_AND16_RD_EN;

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0_CS0, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0_CS0, VDL_STEP, y << jump);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DM_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DM_CS0r(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DM_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DM_CS0r(unit, data));
        }
        #endif

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, x);

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
            }
            #endif

            sal_usleep(SHMOO_AND16_SHORT_SLEEP);

            _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_rd_en_ecc(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
#if (defined(PHY_AND16_K1))
    uint32 x;
    uint32 y;
    uint32 jump;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data;
    and16_shmoo_error_array_t sea;

    yCapMin = 0;
    yCapMax = (*scPtr).sizeY;
    jump = (*scPtr).yJump;
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_AND16_RD_EN_ECC;

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    for(y = yCapMin; y < yCapMax; y++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQ0_CS0, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQ0_CS0, VDL_STEP, y << jump);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DM_CS0r(unit, data));

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, x);

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
            }
            #endif

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));

            sal_usleep(SHMOO_AND16_SHORT_SLEEP);

            _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }
#endif

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_wr_extended(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND16_WR_EXTENDED;

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, x);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
        }
        #endif

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
        }
        #endif

        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_wr_extended_ecc(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
#if (defined(PHY_AND16_K1))
    uint32 x;
    uint32 data;
    and16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND16_WR_EXTENDED_ECC;

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, x);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
        }
        #endif

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));

        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }
#endif

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_rd_extended(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data, temp;
    uint32 rd_dqs_pos0, rd_dqs_pos1, rd_en_pos0, rd_en_pos1, rd_dqs_delta0, rd_dqs_delta1;
    uint32 rd_dq_fail_count0, rd_dq_fail_count1;
    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        uint32 rd_dqs_pos2 = 0, rd_dqs_pos3 = 0, rd_en_pos2 = 0, rd_en_pos3 = 0, rd_dqs_delta2 = 0, rd_dqs_delta3 = 0;
        uint32 rd_dq_fail_count2, rd_dq_fail_count3;
    #endif
    and16_shmoo_error_array_t sea;

    if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
    {
        yCapMin = 163;
        yCapMax = 196;
    }
    else
    {
        yCapMin = 112;
        yCapMax = 145;
    }
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = SHMOO_AND16_MAX_VREF_RANGE;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).yJump = 0;
    (*scPtr).shmooType = SHMOO_AND16_RD_EXTENDED;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
    rd_dqs_pos0 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
    rd_dqs_pos1 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
        rd_dqs_pos2 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
        rd_dqs_pos3 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);
    }
    #endif

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, &data));
    rd_en_pos0 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, &data));
    rd_en_pos1 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, &data));
        rd_en_pos2 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, &data));
        rd_en_pos3 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
    }
    #endif

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    x = 0;
    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, x);

    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
    }
    #endif

    rd_dqs_delta0 = x - rd_dqs_pos0;
    rd_dqs_delta1 = x - rd_dqs_pos1;

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        rd_dqs_delta2 = x - rd_dqs_pos2;
        rd_dqs_delta3 = x - rd_dqs_pos3;
    }
    #endif

    temp = rd_en_pos0 + rd_dqs_delta0;
    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
    if(temp & 0x80000000)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));

    temp = rd_en_pos1 + rd_dqs_delta1;
    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
    if(temp & 0x80000000)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        temp = rd_en_pos2 + rd_dqs_delta2;
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
        if(temp & 0x80000000)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
        }
        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));

        temp = rd_en_pos3 + rd_dqs_delta3;
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
        if(temp & 0x80000000)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
        }
        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
    }
    #endif

    rd_dq_fail_count0 = 0;
    rd_dq_fail_count1 = 0;
    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        rd_dq_fail_count2 = 0;
        rd_dq_fail_count3 = 0;
    }
    #endif

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0_CS0, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0_CS0, VDL_STEP, x);
        if(rd_dq_fail_count0 <= SHMOO_AND16_RD_DQ_FAIL_CAP)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DM_CS0r(unit, data));
        }
        if(rd_dq_fail_count1 <= SHMOO_AND16_RD_DQ_FAIL_CAP)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DM_CS0r(unit, data));
        }
        
        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            if(rd_dq_fail_count2 <= SHMOO_AND16_RD_DQ_FAIL_CAP)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DM_CS0r(unit, data));
            }
            if(rd_dq_fail_count3 <= SHMOO_AND16_RD_DQ_FAIL_CAP)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DM_CS0r(unit, data));
            }
        }
        #endif

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
        }
        #endif

        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

        if((sea[0] & 0x000000FF) == 0x000000FF)
        {
            rd_dq_fail_count0++;
        }
        if((sea[0] & 0x0000FF00) == 0x0000FF00)
        {
            rd_dq_fail_count1++;
        }

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
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
        #endif

        if((rd_dq_fail_count0 > SHMOO_AND16_RD_DQ_FAIL_CAP) && (rd_dq_fail_count1 > SHMOO_AND16_RD_DQ_FAIL_CAP))
        {
            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                if((rd_dq_fail_count2 > SHMOO_AND16_RD_DQ_FAIL_CAP) && (rd_dq_fail_count3 > SHMOO_AND16_RD_DQ_FAIL_CAP))
                {
                    break;
                }
            }
            else
            {
                break;
            }
            #else
                break;
            #endif
        }
    }

    for(y = yCapMin; y < yCapMax; y++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, y);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, y);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));
        
        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, x);

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
            }
            #endif

            rd_dqs_delta0 = x - rd_dqs_pos0;
            rd_dqs_delta1 = x - rd_dqs_pos1;

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                rd_dqs_delta2 = x - rd_dqs_pos2;
                rd_dqs_delta3 = x - rd_dqs_pos3;
            }
            #endif

            temp = rd_en_pos0 + rd_dqs_delta0;
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
            if(temp & 0x80000000)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));

            temp = rd_en_pos1 + rd_dqs_delta1;
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
            if(temp & 0x80000000)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                temp = rd_en_pos2 + rd_dqs_delta2;
                data = 0;
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                if(temp & 0x80000000)
                {
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                }
                else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                {
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                }
                else
                {
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                }
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
                
                temp = rd_en_pos3 + rd_dqs_delta3;
                data = 0;
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                if(temp & 0x80000000)
                {
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                }
                else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                {
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                }
                else
                {
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                }
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
            }
            #endif

            sal_usleep(SHMOO_AND16_SHORT_SLEEP);

            _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_rd_extended_ecc(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
#if (defined(PHY_AND16_K1))
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 xStart;
    uint32 data, temp;
    uint32 rd_dqs_pos4, rd_en_pos4, rd_dqs_delta4;
    uint32 rd_dq_fail_count4;
    and16_shmoo_error_array_t sea;

    if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
    {
        yCapMin = 163;
        yCapMax = 196;
    }
    else
    {
        yCapMin = 112;
        yCapMax = 145;
    }
    xStart = 0;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = SHMOO_AND16_MAX_VREF_RANGE;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).yJump = 0;
    (*scPtr).shmooType = SHMOO_AND16_RD_EXTENDED_ECC;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
    rd_dqs_pos4 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, &data));
    rd_en_pos4 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    x = 0;
    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, x);

    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

    rd_dqs_delta4 = x - rd_dqs_pos4;

    temp = rd_en_pos4 + rd_dqs_delta4;
    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
    if(temp & 0x80000000)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
    }
    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
    }
    else
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
    }
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));

    rd_dq_fail_count4 = 0;

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQ0_CS0, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQ0_CS0, VDL_STEP, x);
        if(rd_dq_fail_count4 <= SHMOO_AND16_RD_DQ_FAIL_CAP)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DM_CS0r(unit, data));
        }

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
        }
        #endif

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));

        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

        if(sea[0])
        {
            rd_dq_fail_count4++;
        }

        if(rd_dq_fail_count4 > SHMOO_AND16_RD_DQ_FAIL_CAP)
        {
            break;
        }
    }

    for(y = yCapMin; y < yCapMax; y++)
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, y);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, y);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, x);

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

            rd_dqs_delta4 = x - rd_dqs_pos4;

            temp = rd_en_pos4 + rd_dqs_delta4;
            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
            if(temp & 0x80000000)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
            }
            else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
            }
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
            }
            #endif

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));

            sal_usleep(SHMOO_AND16_SHORT_SLEEP);

            _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

            (*scPtr).result2D[x + xStart] = sea[0];
        }

        xStart += (*scPtr).sizeX;
    }
#endif

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_addr_extended(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND16_ADDR_EXTENDED;

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, x);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
        }
        #endif

        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_ctrl_extended(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    and16_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_AND16_CTRL_EXTENDED;

    _shmoo_and16_initialize_bist(unit, phy_ndx, -1, scPtr);

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, DEFER, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, x);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUTO_OEB_ENr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKE1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
        }
        #endif

        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        _shmoo_and16_run_bist(unit, phy_ndx, scPtr, &sea);

        (*scPtr).result2D[x] = sea[0];
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_do(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
            return _shmoo_and16_rd_en(unit, phy_ndx, scPtr);
        case SHMOO_AND16_RD_EXTENDED:
            return _shmoo_and16_rd_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND16_WR_EXTENDED:
            return _shmoo_and16_wr_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND16_ADDR_EXTENDED:
            return _shmoo_and16_addr_extended(unit, phy_ndx, scPtr);
        case SHMOO_AND16_CTRL_EXTENDED:
            if(!SHMOO_AND16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _shmoo_and16_ctrl_extended(unit, phy_ndx, scPtr);
            }
            break;
        case SHMOO_AND16_RD_EN_ECC:
            return _shmoo_and16_rd_en_ecc(unit, phy_ndx, scPtr);
        case SHMOO_AND16_RD_EXTENDED_ECC:
            return _shmoo_and16_rd_extended_ecc(unit, phy_ndx, scPtr);
        case SHMOO_AND16_WR_EXTENDED_ECC:
            return _shmoo_and16_wr_extended_ecc(unit, phy_ndx, scPtr);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (unsigned long)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_and16_calib_2D(int unit, int phy_ndx, and16_shmoo_container_t *scPtr, uint32 calibMode, uint32 calibPos)
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
    uint32 maxPassLengthArray[SHMOO_AND16_WORD];

    xStart = 0;
    sizeX = (*scPtr).sizeX;
    calibStart = (*scPtr).calibStart;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;

    switch(calibMode)
    {
        case SHMOO_AND16_BIT:
            iter = shmoo_dram_info.interface_bitwidth;
            shiftAmount = 0;
            dataMask = 0x1;
            break;
        case SHMOO_AND16_BYTE:
            iter = shmoo_dram_info.interface_bitwidth >> 3;
            shiftAmount = 3;
            dataMask = 0xFF;
            break;
        case SHMOO_AND16_HALFWORD:
            iter = shmoo_dram_info.interface_bitwidth >> 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            break;
        case SHMOO_AND16_WORD:
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

            switch (calibPos) {
                case SHMOO_AND16_CALIB_FAIL_START:
                case SHMOO_AND16_CALIB_RISING_EDGE:
                    if(failStartSeen > 0)
                    {
                        maxMidPointX = failStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_AND16_CALIB_PASS_START:
                case SHMOO_AND16_CALIB_FALLING_EDGE:
                    if(passStartSeen > 0)
                    {
                        maxMidPointX = passStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_AND16_CALIB_CENTER_PASS:
                    if((maxPassLength > 0) && (maxPassLengthArray[i] < maxPassLength))
                    {
                        maxMidPointX = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                        maxPassLengthArray[i] = maxPassLength;
                    }
                    break;
                case SHMOO_AND16_CALIB_VDL_ZERO:
                    maxMidPointX = 0;
                    (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
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
_shmoo_and16_calib_2D(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_BYTE, SHMOO_AND16_CALIB_CENTER_PASS);
        case SHMOO_AND16_RD_EXTENDED:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_BYTE, SHMOO_AND16_CALIB_CENTER_PASS);
        case SHMOO_AND16_WR_EXTENDED:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_BYTE, SHMOO_AND16_CALIB_CENTER_PASS);
        case SHMOO_AND16_ADDR_EXTENDED:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_WORD, SHMOO_AND16_CALIB_CENTER_PASS);
        case SHMOO_AND16_CTRL_EXTENDED:
            if(!SHMOO_AND16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_WORD, SHMOO_AND16_CALIB_CENTER_PASS);
            }
            break;
        case SHMOO_AND16_RD_EN_ECC:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_WORD, SHMOO_AND16_CALIB_CENTER_PASS);
        case SHMOO_AND16_RD_EXTENDED_ECC:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_WORD, SHMOO_AND16_CALIB_CENTER_PASS);
        case SHMOO_AND16_WR_EXTENDED_ECC:
            return _and16_calib_2D(unit, phy_ndx, scPtr, SHMOO_AND16_WORD, SHMOO_AND16_CALIB_CENTER_PASS);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_set_new_step(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    uint32 calibMode;
/*  uint32 engageUIshift; */
    uint32 val, yVal;
    uint32 data, temp;
    uint32 rd_dqs_pos0, rd_dqs_pos1, rd_en_pos0, rd_en_pos1, rd_dqs_delta0, rd_dqs_delta1;
    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        uint32 rd_dqs_pos2 = 0, rd_dqs_pos3 = 0, rd_en_pos2 = 0, rd_en_pos3 = 0, rd_dqs_delta2, rd_dqs_delta3;
    #endif
    #if (defined(PHY_AND16_K1))
        uint32 rd_dqs_pos4, rd_en_pos4, rd_dqs_delta4;
    #endif

    calibMode = (*scPtr).calibMode;
/*  engageUIshift = (*scPtr).engageUIshift; */

    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_BYTE:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif
                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                case SHMOO_AND16_HALFWORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif
                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                case SHMOO_AND16_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif
                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_RD_EXTENDED:
            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
            rd_dqs_pos0 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
            rd_dqs_pos1 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
                rd_dqs_pos2 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);
                SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
                rd_dqs_pos3 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);
            }
            #endif

            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, &data));
            rd_en_pos0 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, &data));
            rd_en_pos1 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, &data));
                rd_en_pos2 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

                SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, &data));
                rd_en_pos3 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP);
            }
            #endif

            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_BYTE:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    val = (*scPtr).resultData[1] & 0xFFFF;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                    }
                    #endif

                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        val = (*scPtr).resultData[2] & 0xFFFF;
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));

                        val = (*scPtr).resultData[3] & 0xFFFF;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                        #if (defined(PHY_AND16_K1))
                        if(shmoo_dram_info.ecc_32p4)
                        {
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                        }
                        #endif

                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));

                        #if (defined(PHY_AND16_K1))
                        if(shmoo_dram_info.ecc_32p4)
                        {
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        }
                        #endif
                    }
                    #endif

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)
                                + (((*scPtr).resultData[2] >> 16) & 0xFFFF) + (((*scPtr).resultData[3] >> 16) & 0xFFFF)) >> 2;
                    }
                    else
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    }
                    #else
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    #endif
                    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                case SHMOO_AND16_HALFWORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                    }
                    #endif

                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        val = (*scPtr).resultData[1] & 0xFFFF;
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        rd_dqs_delta3 = val - rd_dqs_pos3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                        #if (defined(PHY_AND16_K1))
                        if(shmoo_dram_info.ecc_32p4)
                        {
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                        }
                        #endif

                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));

                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        #if (defined(PHY_AND16_K1))
                        if(shmoo_dram_info.ecc_32p4)
                        {
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        }
                        #endif
                    }
                    #endif

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        yVal = ((((*scPtr).resultData[0] >> 16) & 0xFFFF) + (((*scPtr).resultData[1] >> 16) & 0xFFFF)) >> 1;
                    }
                    else
                    {
                        yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    }
                    #else
                        yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    #endif
                    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                case SHMOO_AND16_WORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta0 = val - rd_dqs_pos0;
                    rd_dqs_delta1 = val - rd_dqs_pos1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                    }
                    #endif

                    temp = rd_en_pos0 + rd_dqs_delta0;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    temp = rd_en_pos1 + rd_dqs_delta1;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                    }
                    #endif

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        rd_dqs_delta2 = val - rd_dqs_pos2;
                        rd_dqs_delta3 = val - rd_dqs_pos3;

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                        #if (defined(PHY_AND16_K1))
                        if(shmoo_dram_info.ecc_32p4)
                        {
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                        }
                        #endif

                        temp = rd_en_pos2 + rd_dqs_delta2;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));

                        temp = rd_en_pos3 + rd_dqs_delta3;
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                        if(temp & 0x80000000)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                        }
                        else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                        }
                        else
                        {
                            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                        }
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));

                        #if (defined(PHY_AND16_K1))
                        if(shmoo_dram_info.ecc_32p4)
                        {
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
                        }
                        #endif
                    }
                    #endif

                    yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_WR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_BYTE:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, data));

                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_1, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, data));

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[2] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, data));
                        
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_3, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[3] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    }
                    #endif
                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    }
                    #endif

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                case SHMOO_AND16_HALFWORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, data));

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_2, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[1] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    }
                    #endif
                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    }
                    #endif

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                case SHMOO_AND16_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, data));

                    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
                    if(shmoo_dram_info.interface_bitwidth == 32)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    }
                    #endif
                    #if (defined(PHY_AND16_K1))
                    if(shmoo_dram_info.ecc_32p4)
                    {
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    }
                    #endif

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_ADDR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                case SHMOO_AND16_BYTE:
                case SHMOO_AND16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, data));
                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_CTRL_EXTENDED:
            if(SHMOO_AND16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, &data));
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, DEFER, 0);

                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUTO_OEB_ENr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKE1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODT1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, data));
                sal_usleep(SHMOO_AND16_SHORT_SLEEP);
            }
            else
            {
                switch(calibMode)
                {
                    case SHMOO_AND16_BIT:
                    case SHMOO_AND16_BYTE:
                    case SHMOO_AND16_HALFWORD:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                        return SOC_E_FAIL;
                    case SHMOO_AND16_WORD:
                        data = 0;
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, DEFER, 0);
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUTO_OEB_ENr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKE1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODT1r(unit, data));
                        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, data));
                        sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                        return SOC_E_FAIL;
                }
            }
            break;
        case SHMOO_AND16_RD_EN_ECC:
        #if (defined(PHY_AND16_K1))
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                case SHMOO_AND16_BYTE:
                case SHMOO_AND16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
        #endif
            break;
        case SHMOO_AND16_RD_EXTENDED_ECC:
        #if (defined(PHY_AND16_K1))
            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
            rd_dqs_pos4 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP);

            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, &data));
            rd_en_pos4 = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP);

            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                case SHMOO_AND16_BYTE:
                case SHMOO_AND16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_WORD:
                    val = (*scPtr).resultData[0] & 0xFFFF;
                    rd_dqs_delta4 = val - rd_dqs_pos4;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, val);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

                    temp = rd_en_pos4 + rd_dqs_delta4;
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
                    if(temp & 0x80000000)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, 0);
                    }
                    else if(temp >= SHMOO_AND16_MAX_VDL_LENGTH)
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
                    }
                    else
                    {
                        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_RD_EN_CS0, VDL_STEP, temp);
                    }
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));

                    yVal = ((*scPtr).resultData[0] >> 16) & 0xFFFF;
                    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, yVal);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, yVal);
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));
                    
                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
        #endif
            break;
        case SHMOO_AND16_WR_EXTENDED_ECC:
        #if (defined(PHY_AND16_K1))
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                case SHMOO_AND16_BYTE:
                case SHMOO_AND16_HALFWORD:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
                case SHMOO_AND16_WORD:
                    data = 0;
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
                    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_4, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, (*scPtr).resultData[0] & 0xFFFF);

                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, data));
                    
                    sal_usleep(SHMOO_AND16_SHORT_SLEEP);
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02u / %02u\n"),
                        (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
        #endif
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);

    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
    }
    #endif
    #if (defined(PHY_AND16_K1))
    if(shmoo_dram_info.ecc_32p4)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
    }
    #endif

    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    return SOC_E_NONE;
}

STATIC int
_and16_plot(int unit, int phy_ndx, and16_shmoo_container_t *scPtr, uint32 plotMode)
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
    char str0[SHMOO_AND16_STRING_LENGTH];
    char str1[SHMOO_AND16_STRING_LENGTH];
    char str2[SHMOO_AND16_STRING_LENGTH];
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
        case SHMOO_AND16_CALIB_RISING_EDGE:
        case SHMOO_AND16_CALIB_FALLING_EDGE:
            pass_low[0] = '_';
            pass_low[1] = 0;
            fail_high[0] = '|';
            fail_high[1] = 0;
            break;
        case SHMOO_AND16_CALIB_CENTER_PASS:
        case SHMOO_AND16_CALIB_PASS_START:
        case SHMOO_AND16_CALIB_FAIL_START:
        case SHMOO_AND16_CALIB_VDL_ZERO:
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
        case SHMOO_AND16_BIT:
            iter = shmoo_dram_info.interface_bitwidth;
            shiftAmount = 0;
            dataMask = 0x1;
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_BYTE:
                    calibShiftAmount = 3;
                    break;
                case SHMOO_AND16_HALFWORD:
                    calibShiftAmount = 4;
                    break;
                case SHMOO_AND16_WORD:
                    calibShiftAmount = 5;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_BYTE:
            iter = shmoo_dram_info.interface_bitwidth >> 3;
            shiftAmount = 3;
            dataMask = 0xFF;
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from byte mode to bit mode\n")));
                    iter = shmoo_dram_info.interface_bitwidth;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_BYTE:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_HALFWORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_AND16_WORD:
                    calibShiftAmount = 2;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_HALFWORD:
            iter = shmoo_dram_info.interface_bitwidth >> 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to byte mode\n")));
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_HALFWORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_WORD:
                    calibShiftAmount = 1;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_AND16_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to byte mode\n")));
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to halfword mode\n")));
                    iter = 2;
                    shiftAmount = 4;
                    dataMask = 0xFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_AND16_WORD:
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
            if((ui < SHMOO_AND16_MAX_VISIBLE_UI_COUNT) && (x > (*scPtr).endUI[ui]))
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
                case SHMOO_AND16_RD_EN:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EN\n")));
                    break;
                case SHMOO_AND16_RD_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EXTENDED\n")));
                    break;
                case SHMOO_AND16_WR_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_EXTENDED\n")));
                    break;
                case SHMOO_AND16_ADDR_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: ADDR_EXTENDED\n")));
                    break;
                case SHMOO_AND16_CTRL_EXTENDED:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: CTRL_EXTENDED\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Quick Shmoo.....: Off\n")));
                    break;
                case SHMOO_AND16_RD_EN_ECC:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EN_ECC\n")));
                    break;
                case SHMOO_AND16_RD_EXTENDED_ECC:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EXTENDED_ECC\n")));
                    break;
                case SHMOO_AND16_WR_EXTENDED_ECC:
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
                case SHMOO_AND16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Bit-wise\n")));
                    break;
                case SHMOO_AND16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Byte-wise\n")));
                    break;
                case SHMOO_AND16_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Halfword-wise\n")));
                    break;
                case SHMOO_AND16_WORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Word-wise\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                    return SOC_E_FAIL;
            }

            switch(plotMode)
            {
                case SHMOO_AND16_BIT:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Bit-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Bit.............: %03u\n"), i));
                    break;
                case SHMOO_AND16_BYTE:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Byte-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Byte............: %03u\n"), i));
                    break;
                case SHMOO_AND16_HALFWORD:
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Halfword-wise\n")));
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Halfword........: %03u\n"), i));
                    break;
                case SHMOO_AND16_WORD:
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
                            if((calibPos == SHMOO_AND16_CALIB_RISING_EDGE) || (calibPos == SHMOO_AND16_CALIB_FAIL_START))
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
                    case SHMOO_AND16_BIT:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_AND16_BYTE:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_AND16_HALFWORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_AND16_WORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02u\n"), calibMode));
                        return SOC_E_FAIL;
                }

                switch(plotMode)
                {
                    case SHMOO_AND16_BIT:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_AND16_BYTE:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_AND16_HALFWORD:
                        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_AND16_WORD:
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
                        if((calibPos == SHMOO_AND16_CALIB_RISING_EDGE) || (calibPos == SHMOO_AND16_CALIB_FAIL_START))
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
_shmoo_and16_plot(int unit, int phy_ndx, and16_shmoo_container_t *scPtr)
{
    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_BYTE);
        case SHMOO_AND16_RD_EXTENDED:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_BYTE);
        case SHMOO_AND16_WR_EXTENDED:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_BIT);
        case SHMOO_AND16_ADDR_EXTENDED:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_WORD);
        case SHMOO_AND16_CTRL_EXTENDED:
            if(!SHMOO_AND16_QUICK_SHMOO_CTRL_EXTENDED)
            {
                return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_WORD);
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
        case SHMOO_AND16_RD_EN_ECC:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_WORD);
        case SHMOO_AND16_RD_EXTENDED_ECC:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_WORD);
        case SHMOO_AND16_WR_EXTENDED_ECC:
            return _and16_plot(unit, phy_ndx, scPtr, SHMOO_AND16_WORD);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02u\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

int
_and16_calculate_step_size(int unit, int phy_ndx, and16_step_size_t *ssPtr)
{
    uint32 data;
    uint32 timeout;

    if(shmoo_dram_info.sim_system_mode)
    {
        (*ssPtr).step1000 = 8000;
        (*ssPtr).size1000UI = 78125;
        
        return SOC_E_NONE;
    }

    data = 0;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VDL_CALIBRATEr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VDL_CALIBRATE, CALIB_ONCE, 1);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VDL_CALIBRATE, CALIB_FAST, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VDL_CALIBRATEr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    timeout = 2000;
    data = 0;
    do
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VDL_CALIB_STATUS1r(unit, &data));

        if(DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VDL_CALIB_STATUS1, CALIB_IDLE))
        {
        /*    printf("     VDL calibration complete.\n"); */
            break;
        }

        if (timeout == 0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration failed!!! (Timeout)\n")));
            return SOC_E_TIMEOUT;
        }

        timeout--;
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);
    }
    while(TRUE);

    if(DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VDL_CALIB_STATUS1, CALIB_LOCK_4B) == 0)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration failed!!! (No lock)\n")));
        return SOC_E_FAIL;
    }

    (*ssPtr).size1000UI = DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VDL_CALIB_STATUS1, CALIB_TOTAL_STEPS) * 500;
    (*ssPtr).step1000 = ((1000000000 / shmoo_dram_info.data_rate_mbps) * 1000) / ((*ssPtr).size1000UI);

    data = 0;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VDL_CALIBRATEr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    return SOC_E_NONE;
}

int
_and16_zq_calibration(int unit, int phy_ndx)
{
    int i;
    uint32 data;
    uint32 vref_dac;
    uint32 p_drive, n_drive;
    uint32 p_term, n_term;
    uint32 p_idle, n_idle;
    uint32 aq_p_drive, aq_n_drive;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &vref_dac));
    if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
    {
        data = vref_dac;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, 128);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, 128);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);
    }

    if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
    {
        p_drive = 12;
        n_drive = 18;
        aq_p_drive = 18;
        aq_n_drive = 18;
    }
    else
    {
        p_drive = 18;
        n_drive = 18;
        aq_p_drive = 18;
        aq_n_drive = 18;
    }

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, &data));
    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_RXENB, 1);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    #endif
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_MODE_SSTL, 0);/* Done for VDDO/2 */
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_IDDQ, 0);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_P, 0);
    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_N, 31);
    #endif
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_RXENB, 0);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    #endif
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    for(i = 0; i < SHMOO_AND16_MAX_ZQ_CAL_RANGE; i++)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_P, i);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, &data));
        #if (defined(PHY_AND16_K1))
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     P Code - Iteration %2d / Status: %d\n"), 
                        i, DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_STATUS)));
            if(DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_STATUS))
        #else
            if(DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_PCOMP_STATUS))
        #endif
        {
            #if (defined(PHY_AND16_K1))
                if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
                {
                    p_drive = i << 1;
                    aq_p_drive = i * 3;
                }
                else
                {
                    p_drive = i * 3;
                    aq_p_drive = p_drive;
                }
            #else
                p_drive = i;
                aq_p_drive = i;
            #endif
            break;
        }
    }

    #if (defined(PHY_AND16_K1))
        if(((shmoo_dram_info.dram_type != SHMOO_AND16_DRAM_TYPE_DDR4) && (i >= (SHMOO_AND16_MAX_ZQ_CAL_RANGE / 3)))
            || (i >= (SHMOO_AND16_MAX_ZQ_CAL_RANGE >> 1))
            || (i == 0))
    #else
        if(i == SHMOO_AND16_MAX_ZQ_CAL_RANGE)
    #endif
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error (P) - Manual IO programming required for correct operation\n")));
        /* return SOC_E_FAIL; */
    }

    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_RXENB, 1);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 0);
    #endif
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_P, p_drive);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_P, 31);
    #endif
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_RXENB, 0);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 0);
    #endif
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    for(i = 0; i < SHMOO_AND16_MAX_ZQ_CAL_RANGE; i++)
    {
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_N, i);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, &data));
        #if (defined(PHY_AND16_K1))
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     N Code - Iteration %2d / Status: %d\n"), 
                        i, DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_STATUS)));
            if(!DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_STATUS))
        #else
            if(DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_NCOMP_STATUS))
        #endif
        {
            #if (defined(PHY_AND16_K1))
                n_drive = (i - 1) * 3;
                aq_n_drive = n_drive;
            #else
                n_drive = i;
                aq_n_drive = i;
            #endif
            break;
        }
    }

    #if (defined(PHY_AND16_K1))
        if(((shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4) && (i >= (SHMOO_AND16_MAX_ZQ_CAL_RANGE / 3)))
            || (i >= ((SHMOO_AND16_MAX_ZQ_CAL_RANGE << 1) / 3))
            || (i == 0))
    #else
        if(i == SHMOO_AND16_MAX_ZQ_CAL_RANGE)
    #endif
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error (N) - Manual IO programming required for correct operation\n")));
        /* return SOC_E_FAIL; */
    }

    #if (defined(PHY_AND16_K1))
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_RXENB, 1);
    #else
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_PCOMP_ENB, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_NCOMP_ENB, 1);
    #endif
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_IDDQ, 1);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_P, 0);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, ZQ_CAL, ZQ_DRIVE_N, 0);
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_ZQ_CALr(unit, data));
    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
    {
        p_term = p_drive;
        n_term = 0;

        p_idle = p_drive;
        n_idle = 0;
    }
    else
    {
        p_term = 6;
        n_term = 6;

        p_idle = 0;
        n_idle = 0;
    }

    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_IDLE_STRENGTH, p_idle);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_IDLE_STRENGTH, n_idle);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_TERM_STRENGTH, p_term);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_TERM_STRENGTH, n_term);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_PD_STRENGTH, aq_p_drive);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL, ADDR_CTL_ND_STRENGTH, aq_n_drive);
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DRIVE_PAD_CTLr(unit, data));
    #if (defined(PHY_AND16_K1))
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DRIVE_PAD_CTL_2Tr(unit, data));
    #endif

    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_IDLE_STRENGTH, p_idle);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_IDLE_STRENGTH, n_idle);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);

    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_DRIVE_PAD_CTLr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_DRIVE_PAD_CTLr(unit, data));

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_DRIVE_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_DRIVE_PAD_CTLr(unit, data));
    }
    #endif
    #if (defined(PHY_AND16_K1))
    if(shmoo_dram_info.ecc_32p4)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_DRIVE_PAD_CTLr(unit, data));
    }
    #endif

    #if (defined(PHY_AND16_K1))
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_IDLE_STRENGTH, p_idle);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_IDLE_STRENGTH, n_idle);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_TERM_STRENGTH, p_term);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_TERM_STRENGTH, n_term);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_PD_STRENGTH, p_drive);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, DQSP_DRIVE_PAD_CTL, BL_ND_STRENGTH, n_drive);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_CK_P_DRIVE_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_CK_N_DRIVE_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_DQSP_DRIVE_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_DQSN_DRIVE_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_DQSP_DRIVE_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_DQSN_DRIVE_PAD_CTLr(unit, data));

        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_DQSP_DRIVE_PAD_CTLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_DQSN_DRIVE_PAD_CTLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_DQSP_DRIVE_PAD_CTLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_DQSN_DRIVE_PAD_CTLr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_DQSP_DRIVE_PAD_CTLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_DQSN_DRIVE_PAD_CTLr(unit, data));
        }
        #endif

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL_ALERT, ADDR_CTL_PD_IDLE_STRENGTH, p_idle);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL_ALERT, ADDR_CTL_ND_IDLE_STRENGTH, n_idle);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL_ALERT, ADDR_CTL_PD_TERM_STRENGTH, p_term);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL_ALERT, ADDR_CTL_ND_TERM_STRENGTH, n_term);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL_ALERT, ADDR_CTL_PD_STRENGTH, p_drive);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DRIVE_PAD_CTL_ALERT, ADDR_CTL_ND_STRENGTH, n_drive);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DRIVE_PAD_CTL_ALERTr(unit, data));
    #endif

    if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, vref_dac));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);
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
_shmoo_and16_entry(int unit, int phy_ndx, and16_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential entry
     * Mode 1: Single entry
     */

    uint32 i;
    uint32 data, temp;
    and16_step_size_t ss;

    (*scPtr).calibStart = 0;

    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
    /*A04*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "R04. Configure reference voltage\n")));
    /*R04*/ SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
            if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, 179);
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, 179);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, 128);
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, 128);
            }
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));
            sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    /*A08*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "R08. ZQ calibration\n")));
    /*R08*/ if(shmoo_dram_info.sim_system_mode)
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Skipped for emulation\n")));
                goto SHMOO_AND16_RD_EN_ZQ_CALIBRATION_END;
            }

            _and16_zq_calibration(unit, phy_ndx);

            SHMOO_AND16_RD_EN_ZQ_CALIBRATION_END:

            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 4) / 1000;      /* 400% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            (*scPtr).yJump = 2;
            temp = temp >> (*scPtr).yJump;
            if(temp > SHMOO_AND16_MAX_VREF_RANGE)
            {
                (*scPtr).sizeY = SHMOO_AND16_MAX_VREF_RANGE;
            }
            else
            {
                (*scPtr).sizeY = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_CONTROL, RD_EN_TOGGLE_MODE, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_CONTROL, MODE, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_CONTROL, RD_EN_MODE, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_CONTROL, RD_DATA_DLY, SHMOO_AND16_RD_DATA_DLY_INIT);

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_CONTROLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_CONTROLr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_CONTROLr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_CONTROLr(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS0_CYCLES, SHMOO_AND16_RD_EN_CYC_INIT);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, RD_EN_DLY_CYC, CS1_CYCLES, SHMOO_AND16_RD_EN_CYC_INIT);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_RD_EN_DLY_CYCr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_RD_EN_DLY_CYCr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_RD_EN_DLY_CYCr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_RD_EN_DLY_CYCr(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS0, VDL_STEP, SHMOO_AND16_RD_EN_VDL_INIT);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS1, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_EN_CS1, VDL_STEP, SHMOO_AND16_RD_EN_VDL_INIT);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0_CS0, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQ0_CS0, VDL_STEP, SHMOO_AND16_RD_DQ_VDL_INIT);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DM_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DM_CS0r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DM_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DM_CS0r(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DM_CS0r(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, DEFER, 0);
            temp = (((*scPtr).size1000UI * 3) / 4000) + SHMOO_AND16_RD_DQS_VDL_OFFSET;       /* 75% + Offset */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_RD_DQSP_CS0, VDL_STEP, temp);
            }
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_CHAN_DQS_DLY_CYC, DEFER, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_CHAN_DQS_DLY_CYC, CS0_CYCLES, SHMOO_AND16_WR_CYC_INIT);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_CHAN_DQS_DLY_CYC, CS1_CYCLES, SHMOO_AND16_WR_CYC_INIT);
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_WR_CHAN_DQS_DLY_CYCr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_WR_CHAN_DQS_DLY_CYCr(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_WR_CHAN_DQS_DLY_CYCr(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DQS_DLY_CYCr(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_WR_CHAN_DQS_DLY_CYCr(unit, data));
            }
            #endif

            data = 0;
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, DEFER, 0);
            temp = (*scPtr).size1000UI / 2000;
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
            }
            else
            {
                DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, VDL_CONTROL_WR_DQ0_CS0, VDL_STEP, temp);
            }
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, data));

            #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
            if(shmoo_dram_info.interface_bitwidth == 32)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
            }
            #endif
            #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, data));
            }
            #endif

            sal_usleep(SHMOO_AND16_SHORT_SLEEP);
            break;
        case SHMOO_AND16_RD_EXTENDED:
            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND16_WR_EXTENDED:
            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND16_ADDR_EXTENDED:
            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND16_CTRL_EXTENDED:
            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 25) / 10000;      /* 250% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND16_RD_EN_ECC:
            iproc16_enable_ecc(unit);

            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 4) / 1000;      /* 400% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            (*scPtr).yJump = 2;
            temp = temp >> (*scPtr).yJump;
            if(temp > SHMOO_AND16_MAX_VREF_RANGE)
            {
                (*scPtr).sizeY = SHMOO_AND16_MAX_VREF_RANGE;
            }
            else
            {
                (*scPtr).sizeY = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND16_RD_EXTENDED_ECC:
            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_AND16_WR_EXTENDED_ECC:
            _and16_calculate_step_size(unit, phy_ndx, &ss);
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            temp = (ss.size1000UI * 125) / 100000;      /* 125% */
            if(temp > SHMOO_AND16_MAX_VDL_LENGTH)
            {
                (*scPtr).sizeX = SHMOO_AND16_MAX_VDL_LENGTH;
            }
            else
            {
                (*scPtr).sizeX = temp;
            }

            for(i = 0; i < SHMOO_AND16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (unsigned long)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_exit(int unit, int phy_ndx, and16_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential exit
     * Mode 1: Single exit
     */

    switch ((*scPtr).shmooType) {
        case SHMOO_AND16_RD_EN:
            break;
        case SHMOO_AND16_RD_EXTENDED:
            break;
        case SHMOO_AND16_WR_EXTENDED:
            break;
        case SHMOO_AND16_ADDR_EXTENDED:
            break;
        case SHMOO_AND16_CTRL_EXTENDED:
            break;
        case SHMOO_AND16_RD_EN_ECC:
            break;
        case SHMOO_AND16_RD_EXTENDED_ECC:
            break;
        case SHMOO_AND16_WR_EXTENDED_ECC:
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02lu\n"), (unsigned long)(*scPtr).shmooType));
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_save(int unit, int phy_ndx, and16_shmoo_config_param_t *config_param)
{
    uint32 data;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, &data));
    (*config_param).control_regs_ad[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, &data));
    (*config_param).control_regs_ad[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, &data));
    (*config_param).control_regs_ad[2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, &data));
    (*config_param).control_regs_ad[3] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, &data));
    (*config_param).control_regs_ad[4] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, &data));
    (*config_param).control_regs_ad[5] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, &data));
    (*config_param).control_regs_ad[6] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, &data));
    (*config_param).control_regs_ad[7] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, &data));
    (*config_param).control_regs_ad[8] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, &data));
    (*config_param).control_regs_ad[9] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, &data));
    (*config_param).control_regs_ad[10] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, &data));
    (*config_param).control_regs_ad[11] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, &data));
    (*config_param).control_regs_ad[12] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, &data));
    (*config_param).control_regs_ad[13] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, &data));
    (*config_param).control_regs_ad[14] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, &data));
    (*config_param).control_regs_ad[15] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, &data));
    (*config_param).control_regs_ba[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, &data));
    (*config_param).control_regs_ba[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, &data));
    (*config_param).control_regs_ba[2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, &data));
    (*config_param).control_regs_aux[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, &data));
    (*config_param).control_regs_aux[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, &data));
    (*config_param).control_regs_aux[2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, &data));
    (*config_param).control_regs_cs[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, &data));
    (*config_param).control_regs_cs[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, &data));
    (*config_param).control_regs_par = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, &data));
    (*config_param).control_regs_ras_n = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, &data));
    (*config_param).control_regs_cas_n = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, &data));
    (*config_param).control_regs_cke = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, &data));
    (*config_param).control_regs_rst_n = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, &data));
    (*config_param).control_regs_odt = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, &data));
    (*config_param).control_regs_we_n = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
    (*config_param).control_regs_vref_dac_control = data;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_CS0r(unit, &data));
    (*config_param).wr_vdl_dqsp[0] = (uint16) data;
    (*config_param).wr_vdl_dqsn[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][3] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][4] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][5] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][6] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[0][7] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, &data));
    (*config_param).wr_vdl_dm[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_WR_CHAN_DQS_DLY_CYCr(unit, &data));
    (*config_param).wr_chan_dly_cyc[0] = (uint8) data;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
    (*config_param).rd_vdl_dqsp[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, &data));
    (*config_param).rd_vdl_dqsn[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][3] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][4] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][5] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][6] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[0][7] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DM_CS0r(unit, &data));
    (*config_param).rd_vdl_dmp[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, &data));
    (*config_param).rd_en_vdl_cs[0][0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, &data));
    (*config_param).rd_en_vdl_cs[0][1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_RD_EN_DLY_CYCr(unit, &data));
    (*config_param).rd_en_dly_cyc[0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_CONTROLr(unit, &data));
    (*config_param).rd_control[0] = (uint8) data;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_CS0r(unit, &data));
    (*config_param).wr_vdl_dqsp[1] = (uint16) data;
    (*config_param).wr_vdl_dqsn[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][3] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][4] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][5] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][6] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, &data));
    (*config_param).wr_vdl_dq[1][7] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, &data));
    (*config_param).wr_vdl_dm[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_WR_CHAN_DQS_DLY_CYCr(unit, &data));
    (*config_param).wr_chan_dly_cyc[1] = (uint8) data;

    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
    (*config_param).rd_vdl_dqsp[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, &data));
    (*config_param).rd_vdl_dqsn[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][2] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][3] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][4] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][5] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][6] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7_CS0r(unit, &data));
    (*config_param).rd_vdl_dqp[1][7] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DM_CS0r(unit, &data));
    (*config_param).rd_vdl_dmp[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, &data));
    (*config_param).rd_en_vdl_cs[1][0] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, &data));
    (*config_param).rd_en_vdl_cs[1][1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_RD_EN_DLY_CYCr(unit, &data));
    (*config_param).rd_en_dly_cyc[1] = (uint16) data;
    SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_CONTROLr(unit, &data));
    (*config_param).rd_control[1] = (uint8) data;

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_CS0r(unit, &data));
        (*config_param).wr_vdl_dqsp[2] = (uint16) data;
        (*config_param).wr_vdl_dqsn[2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][5] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][6] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[2][7] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, &data));
        (*config_param).wr_vdl_dm[2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_WR_CHAN_DQS_DLY_CYCr(unit, &data));
        (*config_param).wr_chan_dly_cyc[2] = (uint8) data;

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
        (*config_param).rd_vdl_dqsp[2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, &data));
        (*config_param).rd_vdl_dqsn[2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][5] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][6] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[2][7] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DM_CS0r(unit, &data));
        (*config_param).rd_vdl_dmp[2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, &data));
        (*config_param).rd_en_vdl_cs[2][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, &data));
        (*config_param).rd_en_vdl_cs[2][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_RD_EN_DLY_CYCr(unit, &data));
        (*config_param).rd_en_dly_cyc[2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_CONTROLr(unit, &data));
        (*config_param).rd_control[2] = (uint8) data;

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_CS0r(unit, &data));
        (*config_param).wr_vdl_dqsp[3] = (uint16) data;
        (*config_param).wr_vdl_dqsn[3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][5] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][6] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[3][7] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, &data));
        (*config_param).wr_vdl_dm[3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DQS_DLY_CYCr(unit, &data));
        (*config_param).wr_chan_dly_cyc[3] = (uint8) data;

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
        (*config_param).rd_vdl_dqsp[3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, &data));
        (*config_param).rd_vdl_dqsn[3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][5] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][6] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[3][7] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DM_CS0r(unit, &data));
        (*config_param).rd_vdl_dmp[3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, &data));
        (*config_param).rd_en_vdl_cs[3][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, &data));
        (*config_param).rd_en_vdl_cs[3][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, &data));
        (*config_param).rd_en_dly_cyc[3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, &data));
        (*config_param).rd_control[3] = (uint8) data;
    }
    #endif
    #if (defined(PHY_AND16_K1))
    if(shmoo_dram_info.ecc_32p4)
    {
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQS_CS0r(unit, &data));
        (*config_param).wr_vdl_dqsp[4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ1_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ2_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ3_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ4_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ5_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][5] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ6_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][6] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ7_CS0r(unit, &data));
        (*config_param).wr_vdl_dq[4][7] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DM_CS0r(unit, &data));
        (*config_param).wr_vdl_dm[4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_WR_CHAN_DQS_DLY_CYCr(unit, &data));
        (*config_param).wr_chan_dly_cyc[4] = (uint8) data;

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSP_CS0r(unit, &data));
        (*config_param).rd_vdl_dqsp[4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQSN_CS0r(unit, &data));
        (*config_param).rd_vdl_dqsn[4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ0_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ1_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ2_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][2] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ3_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][3] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ4_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ5_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][5] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ6_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][6] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DQ7_CS0r(unit, &data));
        (*config_param).rd_vdl_dqp[4][7] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_DM_CS0r(unit, &data));
        (*config_param).rd_vdl_dmp[4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS0r(unit, &data));
        (*config_param).rd_en_vdl_cs[4][0] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_RD_EN_CS1r(unit, &data));
        (*config_param).rd_en_vdl_cs[4][1] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_RD_EN_DLY_CYCr(unit, &data));
        (*config_param).rd_en_dly_cyc[4] = (uint16) data;
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_CONTROLr(unit, &data));
        (*config_param).rd_control[4] = (uint8) data;
    }
    #endif

    return SOC_E_NONE;
}

STATIC int
_shmoo_and16_restore(int unit, int phy_ndx, and16_shmoo_config_param_t *config_param)
{
    uint32 data;
    data = (uint32) (*config_param).control_regs_ad[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[3];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[4];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[5];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[6];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[7];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[8];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[9];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[10];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[11];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[12];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[13];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[14];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, data));
    data = (uint32) (*config_param).control_regs_ad[15];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUTO_OEB_ENr(unit, data));
    data = (uint32) (*config_param).control_regs_ba[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, data));
    data = (uint32) (*config_param).control_regs_ba[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, data));
    data = (uint32) (*config_param).control_regs_ba[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, data));
    data = (uint32) (*config_param).control_regs_aux[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, data));
    data = (uint32) (*config_param).control_regs_aux[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, data));
    data = (uint32) (*config_param).control_regs_aux[2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, data));
    data = (uint32) (*config_param).control_regs_cs[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, data));
    data = (uint32) (*config_param).control_regs_cs[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, data));
    data = (uint32) (*config_param).control_regs_par;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, data));
    data = (uint32) (*config_param).control_regs_ras_n;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, data));
    data = (uint32) (*config_param).control_regs_cas_n;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, data));
    data = (uint32) (*config_param).control_regs_cke;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKE1r(unit, data));
    data = (uint32) (*config_param).control_regs_rst_n;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, data));
    data = (uint32) (*config_param).control_regs_odt;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODT1r(unit, data));
    data = (uint32) (*config_param).control_regs_we_n;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, data));
    data = (*config_param).control_regs_vref_dac_control;
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));

    data = (uint32) (*config_param).wr_vdl_dqsp[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[0][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dm[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DM_CS0r(unit, data));
    data = (uint32) (*config_param).wr_chan_dly_cyc[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_WR_CHAN_DQS_DLY_CYCr(unit, data));

    data = (uint32) (*config_param).rd_vdl_dqsp[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqsn[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[0][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dmp[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_DM_CS0r(unit, data));
    data = (uint32) (*config_param).rd_en_vdl_cs[0][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS0r(unit, data));
    data = (uint32) (*config_param).rd_en_vdl_cs[0][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_VDL_CONTROL_RD_EN_CS1r(unit, data));
    data = (uint32) (*config_param).rd_en_dly_cyc[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_RD_EN_DLY_CYCr(unit, data));
    data = (uint32) (*config_param).rd_control[0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_CONTROLr(unit, data));

    data = (uint32) (*config_param).wr_vdl_dqsp[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQS_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dq[1][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
    data = (uint32) (*config_param).wr_vdl_dm[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_WR_DM_CS0r(unit, data));
    data = (uint32) (*config_param).wr_chan_dly_cyc[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_WR_CHAN_DQS_DLY_CYCr(unit, data));

    data = (uint32) (*config_param).rd_vdl_dqsp[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqsn[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][2];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][3];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][4];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][5];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][6];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dqp[1][7];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
    data = (uint32) (*config_param).rd_vdl_dmp[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_DM_CS0r(unit, data));
    data = (uint32) (*config_param).rd_en_vdl_cs[1][0];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS0r(unit, data));
    data = (uint32) (*config_param).rd_en_vdl_cs[1][1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_VDL_CONTROL_RD_EN_CS1r(unit, data));
    data = (uint32) (*config_param).rd_en_dly_cyc[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_RD_EN_DLY_CYCr(unit, data));
    data = (uint32) (*config_param).rd_control[1];
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_CONTROLr(unit, data));

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        data = (uint32) (*config_param).wr_vdl_dqsp[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQS_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][5];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][6];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[2][7];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dm[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_WR_DM_CS0r(unit, data));
        data = (uint32) (*config_param).wr_chan_dly_cyc[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_WR_CHAN_DQS_DLY_CYCr(unit, data));

        data = (uint32) (*config_param).rd_vdl_dqsp[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqsn[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][5];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][6];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[2][7];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dmp[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_DM_CS0r(unit, data));
        data = (uint32) (*config_param).rd_en_vdl_cs[2][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS0r(unit, data));
        data = (uint32) (*config_param).rd_en_vdl_cs[2][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_VDL_CONTROL_RD_EN_CS1r(unit, data));
        data = (uint32) (*config_param).rd_en_dly_cyc[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_RD_EN_DLY_CYCr(unit, data));
        data = (uint32) (*config_param).rd_control[2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_CONTROLr(unit, data));

        data = (uint32) (*config_param).wr_vdl_dqsp[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQS_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][5];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][6];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[3][7];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dm[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
        data = (uint32) (*config_param).wr_chan_dly_cyc[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DQS_DLY_CYCr(unit, data));

        data = (uint32) (*config_param).rd_vdl_dqsp[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqsn[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][5];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][6];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[3][7];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dmp[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DM_CS0r(unit, data));
        data = (uint32) (*config_param).rd_en_vdl_cs[3][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
        data = (uint32) (*config_param).rd_en_vdl_cs[3][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
        data = (uint32) (*config_param).rd_en_dly_cyc[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, data));
        data = (uint32) (*config_param).rd_control[3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, data));
    }
    #endif
    #if (defined(PHY_AND16_K1))
    if(shmoo_dram_info.ecc_32p4)
    {
        data = (uint32) (*config_param).wr_vdl_dqsp[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQS_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_VDL_CONTROL_WR_DQ0_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ1_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ2_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ3_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ4_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][5];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ5_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][6];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ6_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dq[4][7];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DQ7_CS0r(unit, data));
        data = (uint32) (*config_param).wr_vdl_dm[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_WR_DM_CS0r(unit, data));
        data = (uint32) (*config_param).wr_chan_dly_cyc[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_WR_CHAN_DQS_DLY_CYCr(unit, data));

        data = (uint32) (*config_param).rd_vdl_dqsp[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSP_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqsn[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQSN_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ0_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ1_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][2];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ2_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][3];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ3_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ4_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][5];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ5_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][6];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ6_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dqp[4][7];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DQ7_CS0r(unit, data));
        data = (uint32) (*config_param).rd_vdl_dmp[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_DM_CS0r(unit, data));
        data = (uint32) (*config_param).rd_en_vdl_cs[4][0];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS0r(unit, data));
        data = (uint32) (*config_param).rd_en_vdl_cs[4][1];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_VDL_CONTROL_RD_EN_CS1r(unit, data));
        data = (uint32) (*config_param).rd_en_dly_cyc[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_RD_EN_DLY_CYCr(unit, data));
        data = (uint32) (*config_param).rd_control[4];
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_CONTROLr(unit, data));
    }
    #endif

    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    data = 0;
    DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, READ_FIFO_CLEAR, CLEAR, 1);

    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_READ_FIFO_CLEARr(unit, data));
    SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_READ_FIFO_CLEARr(unit, data));

    #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
    if(shmoo_dram_info.interface_bitwidth == 32)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_READ_FIFO_CLEARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_READ_FIFO_CLEARr(unit, data));
    }
    #endif
    #if (defined(PHY_AND16_K1))
    if(shmoo_dram_info.ecc_32p4)
    {
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_READ_FIFO_CLEARr(unit, data));
    }
    #endif

    sal_usleep(SHMOO_AND16_SHORT_SLEEP);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_and16_shmoo_ctl
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
soc_and16_shmoo_ctl(int unit, int phy_ndx, int shmoo_type, int stat, int plot, int action, and16_shmoo_config_param_t *config_param)
{
    and16_shmoo_container_t *scPtr = NULL;
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
        scPtr = sal_alloc(sizeof(and16_shmoo_container_t), "AND16 Shmoo Container");
        if(scPtr == NULL)
        {
            return SOC_E_MEMORY;
        }
        sal_memset(scPtr, 0, sizeof(and16_shmoo_container_t));

        if(phy_ndx != SHMOO_AND16_INTERFACE_RSVP)
        {
            ndx = phy_ndx;
            ndxEnd = phy_ndx + 1;
        }
        else
        {
            ndx = 0;
            ndxEnd = SHMOO_AND16_MAX_INTERFACES;
        }

        for(; ndx < ndxEnd; ndx++)
        {
            if(!_shmoo_and16_check_dram(ndx)) {
                continue;
            }

            if(action == SHMOO_AND16_ACTION_RESTORE)
            {
                switch(ctlType)
                {
                    case SHMOO_AND16_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_AND16_CTL_TYPE_1:
                        _shmoo_and16_restore(unit, phy_ndx, config_param);
                        break;
                    default:
                        if(scPtr != NULL)
                        {
                            sal_free(scPtr);
                        }

                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02lu\n"), (unsigned long)ctlType));
                        return SOC_E_FAIL;
                }
            }
            else if((action == SHMOO_AND16_ACTION_RUN) || (action == SHMOO_AND16_ACTION_RUN_AND_SAVE))
            {
                switch(ctlType)
                {
                    case SHMOO_AND16_CTL_TYPE_RSVP:
                        break;
                    case SHMOO_AND16_CTL_TYPE_1:
                        switch(dramType)
                        {
                            case SHMOO_AND16_DRAM_TYPE_DDR3:
                                seqPtr = &shmoo_order_and16_ddr3[0];
                                if(shmoo_dram_info.ecc_32p4)
                                {
                                    seqCount = SHMOO_AND16_DDR3_ECC_32P4_SEQUENCE_COUNT;
                                }
                                else
                                {
                                    seqCount = SHMOO_AND16_DDR3_SEQUENCE_COUNT;
                                }
                                break;
                            case SHMOO_AND16_DRAM_TYPE_DDR3L:
                                seqPtr = &shmoo_order_and16_ddr3l[0];
                                if(shmoo_dram_info.ecc_32p4)
                                {
                                    seqCount = SHMOO_AND16_DDR3L_ECC_32P4_SEQUENCE_COUNT;
                                }
                                else
                                {
                                    seqCount = SHMOO_AND16_DDR3L_SEQUENCE_COUNT;
                                }
                                break;
                            case SHMOO_AND16_DRAM_TYPE_DDR4:
                                seqPtr = &shmoo_order_and16_ddr4[0];
                                if(shmoo_dram_info.ecc_32p4)
                                {
                                    seqCount = SHMOO_AND16_DDR4_ECC_32P4_SEQUENCE_COUNT;
                                }
                                else
                                {
                                    seqCount = SHMOO_AND16_DDR4_SEQUENCE_COUNT;
                                }
                                break;
                            default:
                                if(scPtr != NULL)
                                {
                                    sal_free(scPtr);
                                }
                                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported dram type: %02lu\n"), (unsigned long)dramType));
                                return SOC_E_FAIL;
                        }

                        (*scPtr).dramType = dramType;
                        (*scPtr).ctlType = ctlType;

                        if(shmoo_type != SHMOO_AND16_SHMOO_RSVP)
                        {
                            (*scPtr).shmooType = shmoo_type;
                            _shmoo_and16_entry(unit, ndx, scPtr, SHMOO_AND16_SINGLE);
                            _shmoo_and16_do(unit, ndx, scPtr);
                            _shmoo_and16_calib_2D(unit, ndx, scPtr);
                            _shmoo_and16_set_new_step(unit, ndx, scPtr);
                            if(plot)
                            {
                                _shmoo_and16_plot(unit, ndx, scPtr);
                            }
                            _shmoo_and16_exit(unit, ndx, scPtr, SHMOO_AND16_SINGLE);
                        }
                        else
                        {
                            for(i = 0; i < seqCount; i++)
                            {
                                (*scPtr).shmooType = seqPtr[i];
                                _shmoo_and16_entry(unit, ndx, scPtr, SHMOO_AND16_SEQUENTIAL);
                                _shmoo_and16_do(unit, ndx, scPtr);
                                _shmoo_and16_calib_2D(unit, ndx, scPtr);
                                _shmoo_and16_set_new_step(unit, ndx, scPtr);
                                if(plot)
                                {
                                    _shmoo_and16_plot(unit, ndx, scPtr);
                                }
                                _shmoo_and16_exit(unit, ndx, scPtr, SHMOO_AND16_SEQUENTIAL);
                            }
                        }

                        break;
                    default:
                        if(scPtr != NULL)
                        {
                            sal_free(scPtr);
                        }

                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02lu\n"), (unsigned long)ctlType));
                        return SOC_E_FAIL;
                }
            }

            if((action == SHMOO_AND16_ACTION_RUN_AND_SAVE) || (action == SHMOO_AND16_ACTION_SAVE))
            {
                _shmoo_and16_save(unit, phy_ndx, config_param);
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
            case SHMOO_AND16_CTL_TYPE_RSVP:
                break;
            case SHMOO_AND16_CTL_TYPE_1:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported controller type: %02lu\n"), (unsigned long)ctlType));
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/* Set Dram Parameters/Info to Shmoo driver */
int
soc_and16_shmoo_dram_info_set(int unit, and16_shmoo_dram_info_t *sdi)
{
#if(!SHMOO_AND16_PHY_CONSTANT_CONFIG)
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
_soc_and16_shmoo_phy_cfg_pll(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data;
    uint32 timeout;
    uint32 pll_power;
    uint32 pll_ldo;
    uint32 pll_controls;
    uint32 pll_ndiv;

    if(shmoo_dram_info.ref_clk_mhz != 50)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Unsupported reference flock frequency: %4d MHz\n"), shmoo_dram_info.ref_clk_mhz));
        return SOC_E_FAIL;
    }

    switch(shmoo_dram_info.data_rate_mbps)
    {
        case 800:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x01000000;
            break;
        case 1066:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x01555555;
            break;
        case 1333:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x01AAAAAA;
            break;
        case 1600:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x02000000;
            break;
        case 1866:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x02555555;
            break;
        case 2133:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x02AAAAAA;
            break;
        case 2400:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x03000000;
            break;
        case 2666:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x03555555;
            break;
        case 2933:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x03AAAAAA;
            break;
        case 3200:
            pll_power = 0x00000003;
            pll_ldo = 0x0000020A;
            pll_controls = 0x00000001;
            pll_ndiv = 0x04000000;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Unsupported data rate: %4d Mbps\n"), shmoo_dram_info.data_rate_mbps));
            return SOC_E_FAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PHY PLL Configuration\n")));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Fref.............: %4d MHz\n"), shmoo_dram_info.ref_clk_mhz));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Data rate........: %4d Mbps\n"), shmoo_dram_info.data_rate_mbps));

    if(phy_ndx != SHMOO_AND16_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_AND16_MAX_INTERFACES;
    }

    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_and16_check_dram(ndx))
        {
            continue;
        }

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_RESET_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, PLL_RESET_CONTROL, RESETB, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, PLL_RESET_CONTROL, POST_RESETB, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_RESET_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_POWER_CONTROLr(unit, pll_power));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_LDO_CONTROLr(unit, pll_ldo));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_CONTROLSr(unit, pll_controls));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_NDIVr(unit, pll_ndiv));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_REFERENCEr(unit, shmoo_dram_info.ref_clk_mhz));

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_POST_CHANNELr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, PLL_POST_CHANNEL, MDIV, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_POST_CHANNELr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_RESET_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, PLL_RESET_CONTROL, RESETB, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_RESET_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        timeout = 2000;
        do
        {
            SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_STATUSr(unit, &data));
            
            if(DDR_PHY_GET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, PLL_STATUS, LOCK))
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PLL locked.\n")));
                break;
            }

            if (timeout == 0)
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     PLL not locked!!! (Timeout)\n")));
                return SOC_E_TIMEOUT;
            }

            timeout--;
            sal_usleep(SHMOO_AND16_SHORT_SLEEP);
        }
        while(TRUE);

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_RESET_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, PLL_RESET_CONTROL, POST_RESETB, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_PLL_RESET_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);
    }

    return SOC_E_NONE;
}

int
soc_and16_shmoo_phy_init(int unit, int phy_ndx)
{
    int ndx, ndxEnd;
    uint32 data;
    uint32 dfi_ctrl;
    uint32 dram_config;
    uint32 dram_timing2;
    uint32 dram_timing3;
    uint32 step1000, size1000UI, sizeUI;
    and16_step_size_t ss;

    if(phy_ndx != SHMOO_AND16_INTERFACE_RSVP)
    {
        ndx = phy_ndx;
        ndxEnd = phy_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_AND16_MAX_INTERFACES;
    }

    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_and16_check_dram(ndx))
        {
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization (PHY index: %02d)\n"), ndx));

/*A01*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A01. Take over DFI control\n")));
        dfi_ctrl = 0;
        DDR_PHY_SET_FIELD(dfi_ctrl, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DFI_CNTRL, ASSERT_REQ, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DFI_CNTRLr(unit, dfi_ctrl));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A02*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A02. Configure timing parameters\n")));
        switch(shmoo_dram_info.data_rate_mbps)
        {
            case 800:
                dram_timing2 = 0x060C0C0C;
                dram_timing3 = 0x00400000;
                break;
            case 1600:
                dram_timing2 = 0x060C0C0C;
                dram_timing3 = 0x00400000;
                break;
            case 2133:
                dram_timing2 = 0x08101010;
                dram_timing3 = 0x00400000;
                break;
            case 2400:
                dram_timing2 = 0x09121212;
                dram_timing3 = 0x00500000;
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR,
                           (BSL_META_U(unit, "Unsupported data rate: %4d Mbps\n"),
                           shmoo_dram_info.data_rate_mbps));
                return SOC_E_FAIL;
        }

        dram_config = 0x0;
        #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                dram_config |= 0x08000000;
            }
        #endif
        if(shmoo_dram_info.interface_bitwidth <= 16)
        {
            dram_config |= 0x02000000;
        }
        if(shmoo_dram_info.interface_bitwidth <= 8)
        {
            dram_config |= 0x01000000;
        }

        switch(shmoo_dram_info.dram_type)
        {
            case SHMOO_AND16_DRAM_TYPE_DDR3:    dram_config |= 0x00000000; break;
            case SHMOO_AND16_DRAM_TYPE_DDR3L:   dram_config |= 0x00000000; break;
            case SHMOO_AND16_DRAM_TYPE_DDR4:    dram_config |= 0x00000001; break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR,
                           (BSL_META_U(unit, "Unsupported dram type: %lu\n"),
                           (unsigned long)shmoo_dram_info.dram_type));
                return SOC_E_FAIL;
        }

        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DPFE_CONTROLLER_CONFIGr(unit, dram_config));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DRAM_TIMING2r(unit, dram_timing2));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DRAM_TIMING3r(unit, dram_timing3));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A03*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A03. Configure PHY PLL\n")));
        _soc_and16_shmoo_phy_cfg_pll(unit, ndx);
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A04*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A04. Configure reference voltage\n")));
/*R04*/ SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, &data));
        if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, 179);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, 179);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC0, 128);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, DAC1, 128);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, PUP0, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, PUP1, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, PUP2, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, VREF_DAC_CONTROL, PUP3, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_VREF_DAC_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A05*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A05. Compute VDL step size\n")));
        _and16_calculate_step_size(unit, ndx, &ss);

        step1000 = ss.step1000;
        size1000UI = ss.size1000UI;
        sizeUI = size1000UI / 1000;
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL calibration complete.\n")));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL step size....: %3lu.%03lu ps\n"), (unsigned long)(step1000 / 1000), (unsigned long)(step1000 % 1000)));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     UI size..........: %3lu.%03lu steps\n"), (unsigned long)sizeUI, (unsigned long)(size1000UI % 1000)));

/*A06*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A06. Configure ADDR/CTRL VDLs\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, DEFER, 0);
        if(sizeUI > SHMOO_AND16_MAX_VDL_LENGTH)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, SHMOO_AND16_MAX_VDL_LENGTH - 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VDL_CONTROL_AD00, VDL_STEP, sizeUI);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD00r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD01r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD02r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD03r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD04r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD05r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD06r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD07r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD08r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD09r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD10r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD11r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD12r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD13r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD14r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AD15r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUTO_OEB_ENr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_BA2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_AUX2r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS0r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CS1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_PARr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RAS_Nr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CAS_Nr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKEr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_CKE1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_RST_Nr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODTr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_ODT1r(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VDL_CONTROL_WE_Nr(unit, data));

/*A07*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A07. Disable Virtual VTT\n")));
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CTL_IDLE, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CS_IDLE, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CS1_IDLE, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CKE_IDLE, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, VIRTUAL_VTT_CONTROL, ENABLE_CKE1_IDLE, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_VIRTUAL_VTT_CONTROLr(unit, data));

/*A08*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A08. ZQ calibration\n")));
/*R08*/ if(shmoo_dram_info.sim_system_mode)
        {
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Skipped for emulation\n")));
            
            goto SHMOO_AND16_ZQ_CALIBRATION_END;
        }

        _and16_zq_calibration(unit, ndx);

        SHMOO_AND16_ZQ_CALIBRATION_END:

/*A09*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A09. Configure Static Pad Control\n")));
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_COMMON_REGS_STATIC_PAD_CTLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, STATIC_PAD_CTL, IDDQ_CLK, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, STATIC_PAD_CTL, IDDQ_CLK1, 0);
        if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, STATIC_PAD_CTL, MODE_SSTL, 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_COMMON_REGS, STATIC_PAD_CTL, MODE_SSTL, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_COMMON_REGS_STATIC_PAD_CTLr(unit, data));

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_STATIC_PAD_CTLr(unit, &data));
        if(shmoo_dram_info.dram_type == SHMOO_AND16_DRAM_TYPE_DDR4)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, AUTO_OEB, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, RXENB_ALERT_N, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_ALERT_N, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, MODE_SSTL, 1);
        }
        else
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, AUTO_OEB, 1);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, RXENB_ALERT_N, 1);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_ALERT_N, 0);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, MODE_SSTL, 0);
        }
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STATIC_PAD_CTL, IDDQ_CS1, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_STATIC_PAD_CTLr(unit, data));

  #ifdef SOC_DDR4_SUPPORT
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_STATIC_PAD_CTLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, STATIC_PAD_CTL, DM_MODE, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, STATIC_PAD_CTL, DM_INVERT, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_STATIC_PAD_CTLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_STATIC_PAD_CTLr(unit, data));
        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_STATIC_PAD_CTLr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_STATIC_PAD_CTLr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
            if(shmoo_dram_info.ecc_32p4)
            {
                SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_STATIC_PAD_CTLr(unit, data));
            }
        #endif
#endif
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A10*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A10. Configure ODT\n")));
        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT_FORCE, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, WRITE_ODT_CNTRL, ODT_FORCE_VALUE, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_WRITE_ODT_CNTRLr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, WRITE_ODT1_CNTRL, ODT_FORCE, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, WRITE_ODT1_CNTRL, ODT_FORCE_VALUE, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_WRITE_ODT1_CNTRLr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_ENABLE, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_DELAY, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_POST_LENGTH, 2);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, ODT_CONTROL, ODT_PRE_LENGTH, 4);

        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, DDR_PHY_DDR34_PHY_BYTE_LANE_0_ODT_CONTROL, data));
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, DDR_PHY_DDR34_PHY_BYTE_LANE_1_ODT_CONTROL, data));
        #if(SHMOO_AND28_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, DDR_PHY_DDR34_PHY_BYTE_LANE_2_ODT_CONTROL, data));
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, DDR_PHY_DDR34_PHY_BYTE_LANE_3_ODT_CONTROL, data));
        }
        #endif
        #if (defined(PHY_AND28_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, DDR_PHY_DDR34_PHY_BYTE_LANE_4_ODT_CONTROL, data));
        }
        #endif
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A11*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A11. Configure Write Pre-/Post-amble\n")));
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_WR_PREAMBLE_MODEr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQ_POSTAM_BITS, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQ_PREAM_BITS, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DM_POSTAM_BITS, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DM_PREAM_BITS, 1);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS, 0xE);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS_POSTAM_BITS, 0);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, WR_PREAMBLE_MODE, DQS_PREAM_BITS, 2);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_WR_PREAMBLE_MODEr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_WR_PREAMBLE_MODEr(unit, data));
        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth == 32)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_WR_PREAMBLE_MODEr(unit, data));
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_WR_PREAMBLE_MODEr(unit, data));
        }
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {

            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_WR_PREAMBLE_MODEr(unit, data));
        }
        #endif
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A12*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A12. Configure Auto Idle\n")));
        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_BYTE_LANE_0_IDLE_PAD_CONTROLr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, AUTO_DQ_RXENB_MODE, 3);
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, AUTO_DQ_IDDQ_MODE, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_0_IDLE_PAD_CONTROLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_1_IDLE_PAD_CONTROLr(unit, data));
        #if(SHMOO_AND16_PHY_BITWIDTH_IS_32)
        if(shmoo_dram_info.interface_bitwidth != 32)
        {
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, IDLE, 1);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, RXENB, 1);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, IDDQ, 1);
            DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_BYTE_LANE_0, IDLE_PAD_CONTROL, IO_IDLE_ENABLE, 0x7FF);
        }
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_2_IDLE_PAD_CONTROLr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_3_IDLE_PAD_CONTROLr(unit, data));
        #endif
        #if (defined(PHY_AND16_K1))
        if(shmoo_dram_info.ecc_32p4)
        {
            SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_BYTE_LANE_4_IDLE_PAD_CONTROLr(unit, data));
        }
        #endif

        SOC_IF_ERROR_RETURN(READ_DDR_PHY_DDR34_PHY_CONTROL_REGS_AC_DEFAULTr(unit, &data));
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, AC_DEFAULT, RST, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_AC_DEFAULTr(unit, data));

        data = 0;
        DDR_PHY_SET_FIELD(data, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, STANDBY_CONTROL, RST_N, 1);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_STANDBY_CONTROLr(unit, data));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

/*A13*/ LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A13. Release PHY control\n")));
        DDR_PHY_SET_FIELD(dfi_ctrl, AND16_DDR_PHY_DDR34_PHY_CONTROL_REGS, DFI_CNTRL, ASSERT_REQ, 0);
        SOC_IF_ERROR_RETURN(WRITE_DDR_PHY_DDR34_PHY_CONTROL_REGS_DFI_CNTRLr(unit, dfi_ctrl));
        sal_usleep(SHMOO_AND16_SHORT_SLEEP);

        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization complete (PHY index: %02d)\n"), ndx));
    }

    return SOC_E_NONE;
}
#endif
