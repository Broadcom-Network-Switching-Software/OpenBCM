/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * GDDR6 Memory Support
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <soc/memtune.h>

#include <soc/drv.h>
#include <soc/cm.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

/* BEGIN: TEMPORARY */
#ifndef BCM_DDRC16_SUPPORT
#define BCM_DDRC16_SUPPORT
#endif
/* END: TEMPORARY */

#ifdef BCM_DDRC16_SUPPORT
#include <soc/g6ddrc16.h>
#include <soc/shmoo_g6phy16.h>

#include <shared/utilex/utilex_integer_arithmetic.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_SHMOO

soc_g6phy16_phy_reg_read_t soc_g6phy16_phy_reg_read = NULL;
soc_g6phy16_phy_reg_write_t soc_g6phy16_phy_reg_write = NULL;
soc_g6phy16_phy_reg_modify_t soc_g6phy16_phy_reg_modify = NULL;
static _shmoo_g6phy16_drc_bist_conf_set_t _shmoo_g6phy16_drc_bist_conf_set = NULL;
static _shmoo_g6phy16_drc_bist_run_t _shmoo_g6phy16_drc_bist_run = NULL;
static _shmoo_g6phy16_drc_bist_err_cnt_t _shmoo_g6phy16_drc_bist_err_cnt = NULL;
static _shmoo_g6phy16_drc_dram_init_t _shmoo_g6phy16_drc_dram_init = NULL;
static _shmoo_g6phy16_drc_pll_set_t _shmoo_g6phy16_drc_pll_set = NULL;
static _shmoo_g6phy16_drc_modify_mrs_t _shmoo_g6phy16_drc_modify_mrs = NULL;
static _shmoo_g6phy16_drc_enable_wck2ck_training_t _shmoo_g6phy16_drc_enable_wck2ck_training = NULL;
static _shmoo_g6phy16_drc_enable_write_leveling_t _shmoo_g6phy16_drc_enable_write_leveling = NULL;
static _shmoo_g6phy16_drc_mpr_en_t _shmoo_g6phy16_drc_mpr_en = NULL;
static _shmoo_g6phy16_drc_mpr_load_t _shmoo_g6phy16_drc_mpr_load = NULL;
static _shmoo_g6phy16_drc_vendor_info_get_t _shmoo_g6phy16_drc_vendor_info_get = NULL;
static _shmoo_g6phy16_drc_dqs_pulse_gen_t _shmoo_g6phy16_drc_dqs_pulse_gen = NULL;
static _shmoo_g6phy16_training_bist_conf_set_t _shmoo_g6phy16_training_bist_conf_set = NULL;
static _shmoo_g6phy16_drc_gddr6_bist_err_cnt_t _shmoo_g6phy16_training_bist_err_cnt = NULL;
static _shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get_t _shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get = NULL;
static _shmoo_g6phy16_drc_enable_wr_crc_t _shmoo_g6phy16_drc_enable_wr_crc = NULL;
static _shmoo_g6phy16_drc_enable_rd_crc_t _shmoo_g6phy16_drc_enable_rd_crc = NULL;
static _shmoo_g6phy16_drc_enable_wr_dbi_t _shmoo_g6phy16_drc_enable_wr_dbi = NULL;
static _shmoo_g6phy16_drc_enable_rd_dbi_t _shmoo_g6phy16_drc_enable_rd_dbi = NULL;
static _shmoo_g6phy16_drc_force_dqs_t _shmoo_g6phy16_drc_force_dqs = NULL;
static _shmoo_g6phy16_drc_soft_reset_drc_without_dram_t _shmoo_g6phy16_drc_soft_reset_drc_without_dram = NULL;
static _shmoo_g6phy16_drc_dram_info_access_t _shmoo_g6phy16_drc_dram_info_access = NULL;

const shmoo_g6phy16_shmoo_type_t shmoo_order_g6phy16_gddr6[] =
{
    SHMOO_G6PHY16_ADDR_CTRL_SHORT,
    SHMOO_G6PHY16_WCK2CK,
    SHMOO_G6PHY16_RD_START_FISH,
    SHMOO_G6PHY16_EDC_START_FISH,
    SHMOO_G6PHY16_RD_DESKEW,
    SHMOO_G6PHY16_DBI_EDC_RD_DESKEW,
    SHMOO_G6PHY16_RD_SHORT,
    SHMOO_G6PHY16_WR_DESKEW,
    SHMOO_G6PHY16_DBI_WR_DESKEW,
    SHMOO_G6PHY16_WR_SHORT,
    SHMOO_G6PHY16_RD_EXTENDED,
    SHMOO_G6PHY16_WR_EXTENDED,
    SHMOO_G6PHY16_CDR,
};

const shmoo_g6phy16_shmoo_type_t shmoo_order_g6phy16_gddr6_restore[] =
{
    SHMOO_G6PHY16_WCK2CK,
    SHMOO_G6PHY16_RD_START_FISH,
    SHMOO_G6PHY16_EDC_START_FISH,
    SHMOO_G6PHY16_RD_SHORT,
    SHMOO_G6PHY16_WR_SHORT,
    SHMOO_G6PHY16_RD_EXTENDED,
    SHMOO_G6PHY16_WR_EXTENDED,
    SHMOO_G6PHY16_CDR,
};

/* BEGIN: HELPER FUNCTIONS */
STATIC uint32
_g6phy16_get_random(void)
{
    static uint32 m_w = 6483;       /* must not be zero */
    static uint32 m_z = 31245;      /* must not be zero */
    
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;       /* 32-bit result */
}
/* END: HELPER FUNCTIONS */

uint32
_shmoo_g6phy16_check_dram(int unit, uint32 drc_ndx)
{
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_IF_ERROR_RETURN( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));
    return ((shmoo_dram_info.dram_bitmap >> drc_ndx) & 0x1);
}


/**
 * \brief - validate dram info configuration, also validates that dram type is GDDR6
 *
 * \param [in] unit - unit number
 *
 * \return
 *   SOC_E_NONE if ok.
 *   SOC_E_FAIL if validation failed.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32
_shmoo_g6phy16_validate_config(
    int unit)
{
    uint32 dram_index;
    uint32 dram_bitmap[1];
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    /** get dram info */
    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    /** iterate on dram bitmap */
    dram_bitmap[0] = shmoo_dram_info.dram_bitmap;
    SHR_BIT_ITER(dram_bitmap, SHMOO_G6PHY16_MAX_INTERFACES, dram_index)
    {
        if(shmoo_dram_info.zq_cal_array[dram_index] == 0xff)
        {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "bad zq_cal value\n")));
        }
    }

    if(
        (shmoo_dram_info.ctl_type == 0)
        || (shmoo_dram_info.dram_type != SHMOO_G6PHY16_DRAM_TYPE_GDDR6)
        || (shmoo_dram_info.num_columns == -1)
        || (shmoo_dram_info.num_rows == -1)
        || (shmoo_dram_info.num_banks == -1)
        || (shmoo_dram_info.data_rate_mbps == -1)
        || (shmoo_dram_info.ref_clk_mhz == -1)
        || (shmoo_dram_info.ref_clk_bitmap == 0))
    {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "configuration validation failed\n")));
    }

exit:
    SOC_FUNC_RETURN;
}

uint32
_shmoo_g6phy16_validate_cbi(int unit)
{
    SOC_INIT_FUNC_DEFS;
    if(
        (soc_g6phy16_phy_reg_read == NULL)
        || (soc_g6phy16_phy_reg_write == NULL)
        || (soc_g6phy16_phy_reg_modify == NULL)
        || (_shmoo_g6phy16_drc_bist_conf_set == NULL)
        || (_shmoo_g6phy16_drc_bist_run == NULL)
        || (_shmoo_g6phy16_drc_bist_err_cnt == NULL)
        || (_shmoo_g6phy16_drc_dram_init == NULL)
        || (_shmoo_g6phy16_drc_pll_set == NULL)
        || (_shmoo_g6phy16_drc_modify_mrs == NULL)
        || (_shmoo_g6phy16_drc_enable_wck2ck_training == NULL)
        || (_shmoo_g6phy16_drc_enable_write_leveling == NULL)
        || (_shmoo_g6phy16_drc_mpr_en == NULL)
        || (_shmoo_g6phy16_drc_mpr_load == NULL)
        || (_shmoo_g6phy16_drc_vendor_info_get == NULL)
        || (_shmoo_g6phy16_drc_dqs_pulse_gen == NULL)
        || (_shmoo_g6phy16_training_bist_conf_set == NULL)
        || (_shmoo_g6phy16_training_bist_err_cnt == NULL)
        || (_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get == NULL)
        || (_shmoo_g6phy16_drc_enable_wr_crc == NULL)
        || (_shmoo_g6phy16_drc_enable_rd_crc == NULL)
        || (_shmoo_g6phy16_drc_enable_wr_dbi == NULL)
        || (_shmoo_g6phy16_drc_enable_rd_dbi == NULL)
        || (_shmoo_g6phy16_drc_force_dqs == NULL)
        || (_shmoo_g6phy16_drc_soft_reset_drc_without_dram == NULL)
        || (_shmoo_g6phy16_drc_dram_info_access == NULL))
    {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "a SHMOO CB is missing\n")));
    }
exit:
    SOC_FUNC_RETURN;
}

static void g6phy16_training_bist_info_t_init(g6phy16_training_bist_info_t *gbiPtr)
{
    sal_memset(gbiPtr, 0, sizeof(*gbiPtr));
}

static void g6phy16_bist_info_t_init(g6phy16_bist_info_t *biPtr)
{
    sal_memset(biPtr, 0, sizeof(*biPtr));
}

/**
 * \brief - initialize BIST structures according to step.
 * each SHMOO step is using either a training BIST or a regular BIST procedure.
 * this function initialize the structures with the relevant parameters for each step.
 *
 * \param [in] unit - unit number.
 * \param [in] drc_ndx - DRAM index.
 * \param [in] bit - bit to be tested, used to set the pattern to test a specific bit in DESKEW steps.
 * \param [in] scPtr - SHMOO container, has all the relevant info on the SHMOO that is undergoing now. used to determine the current step.
 * \param [out] biPtr - BIST info structure. loaded with the relevant parameters according to step. used in XXX_EXTENDED steps only.
 * \param [out] gbiPtr - training BIST info structure. loaded with the relevant parameters according to steps.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
STATIC int
_g6phy16_initialize_bist(
    int unit,
    int drc_ndx,
    int bit,
    g6phy16_shmoo_container_t *scPtr,
    g6phy16_bist_info_t *biPtr,
    g6phy16_training_bist_info_t *gbiPtr)
{
    uint32 i;
    uint8 dbi_pattern;
    uint8 edc_pattern;
    int pattern_iter;
    int arr_iter;
    g6phy16_vendor_info_t shmoo_vendor_info;
    SOC_INIT_FUNC_DEFS;

    if(scPtr->shmooType != SHMOO_G6PHY16_ADDR_CTRL_SHORT)
    {
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_vendor_info_get(unit, drc_ndx, &shmoo_vendor_info));
    }

    /** init training BIST struct */
    g6phy16_training_bist_info_t_init(gbiPtr);

    /** init BIST struct */
    g6phy16_bist_info_t_init(biPtr);

    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
        {
            gbiPtr->nof_commands = 255;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_ADDR_TRAINING;
            gbiPtr->cadt_prbs_enabled = 1;
            gbiPtr->cadt_seed = 0x123;
            gbiPtr->cadt_invert_caui2 = 1;
            gbiPtr->cadt_to_cadt_prd = 10;
            gbiPtr->cadt_to_rden_prd = 30;
            gbiPtr->cadt_mode = RISING_EDGE;

            /** not used in PRBS mode */
            for(i = 0; i < SHMOO_G6PHY16_BIST_NOF_PATTERNS; i++)
            {
                gbiPtr->cadt_pattern[i] = 0x0;
            }
            break;
        }
        case SHMOO_G6PHY16_WCK2CK:
        {
            break;
        }
        case SHMOO_G6PHY16_RD_START_FISH:
        case SHMOO_G6PHY16_EDC_START_FISH:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_LOAD_READ_FIFO;

            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                for(arr_iter = 0; arr_iter < 4; ++arr_iter)
                {
                    gbiPtr->ldff_data_pattern[pattern_iter][arr_iter] = _g6phy16_get_random();
                }
                gbiPtr->ldff_dbi_pattern[pattern_iter]  = _g6phy16_get_random() & 0xFF;
                gbiPtr->ldff_edc_pattern[pattern_iter]  = _g6phy16_get_random() & 0xFF;
            }
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));

            gbiPtr->nof_commands = (shmoo_vendor_info.fifo_depth) << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_READ_FROM_LDFF;
            break;
        }
        case SHMOO_G6PHY16_RD_DESKEW:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_LOAD_READ_FIFO;

            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                gbiPtr->ldff_data_pattern[pattern_iter][0] = ~(0x00000101 << bit);
                gbiPtr->ldff_data_pattern[pattern_iter][1] = ~(0x00000101 << bit);
                gbiPtr->ldff_data_pattern[pattern_iter][2] = ~(0x00000101 << bit);
                gbiPtr->ldff_data_pattern[pattern_iter][3] = ~(0x00000101 << bit);

                gbiPtr->ldff_dbi_pattern[pattern_iter]  = 0xFF;
                gbiPtr->ldff_edc_pattern[pattern_iter]  = 0xFF;
            }

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));

            gbiPtr->nof_commands = (shmoo_vendor_info.fifo_depth) << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_READ_FROM_LDFF;
            break;
        }
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_LOAD_READ_FIFO;

            if(bit)
            {
                dbi_pattern = 0xFF;
                edc_pattern = 0xF0;
            }
            else
            {
                dbi_pattern = 0xF0;
                edc_pattern = 0xFF;
            }

            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                for(arr_iter = 0; arr_iter < 4; ++arr_iter)
                {
                    gbiPtr->ldff_data_pattern[pattern_iter][arr_iter] = 0xFFFFFFFF;
                }
                gbiPtr->ldff_dbi_pattern[pattern_iter]  = dbi_pattern;
                gbiPtr->ldff_edc_pattern[pattern_iter]  = edc_pattern;
            }

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));

            gbiPtr->nof_commands = (shmoo_vendor_info.fifo_depth) << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_READ_FROM_LDFF;
            break;
        }
        case SHMOO_G6PHY16_RD_SHORT:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                gbiPtr->ldff_dbi_pattern[pattern_iter] = 0xFF;
                gbiPtr->ldff_edc_pattern[pattern_iter] = 0xFF;
            }
            break;
        }
        case SHMOO_G6PHY16_WR_DESKEW:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_WRITE_THEN_READ_FROM_LDFF;
            gbiPtr->data_mode = 0;
            gbiPtr->dbi_mode = 0;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;

            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                gbiPtr->bist_data_pattern[pattern_iter][0] = ~(0x01010101 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][1] = ~(0x01010101 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][2] = ~(0x01010101 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][3] = ~(0x01010101 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][4] = ~(0x00000000 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][5] = ~(0x00000000 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][6] = ~(0x00000000 << bit);
                gbiPtr->bist_data_pattern[pattern_iter][7] = ~(0x00000000 << bit);
            }
            break;
        }
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_WRITE_THEN_READ_FROM_LDFF;
            gbiPtr->data_mode = 0;
            gbiPtr->dbi_mode = 0;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;

            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                /** set data to all ones */
                sal_memset(gbiPtr->bist_data_pattern[pattern_iter], 0xFF, sizeof(gbiPtr->bist_data_pattern[pattern_iter]));
                /** check dbi bits */
                gbiPtr->bist_dbi_pattern[pattern_iter]  = 0xFFF0;
            }
            break;
        }
        case SHMOO_G6PHY16_WR_SHORT:
        {
            gbiPtr->fifo_depth = shmoo_vendor_info.fifo_depth;
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_WRITE_THEN_READ_FROM_LDFF;
            gbiPtr->dbi_mode = 0;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;

            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                gbiPtr->bist_dbi_pattern[pattern_iter]  = 0xFFFF;
            }
            break;
        }
        case SHMOO_G6PHY16_RD_EXTENDED:
        case SHMOO_G6PHY16_WR_EXTENDED:
        {
            biPtr->write_weight = 256;
            biPtr->read_weight = 256;
            biPtr->bist_timer_us = 0;
            biPtr->bist_num_actions = 256 * 200;
            biPtr->mpr_mode = 0;
            biPtr->prbs_mode = 1;
            break;
        }
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }
exit:
    SOC_FUNC_RETURN;
}

STATIC int
_g6phy16_run_bist(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr, g6phy16_bist_info_t *biPtr, g6phy16_training_bist_info_t *gbiPtr, g6phy16_shmoo_error_array_t *seaPtr)
{
    g6phy16_bist_err_cnt_t be;
    g6phy16_training_bist_err_cnt_t gbe;
    int pattern_iter;
    int arr_iter;
    int seed_iter;
    SOC_INIT_FUNC_DEFS;

    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            /** In GDDR6 do not look at bits 9 and 19 (the last bit in every channel)*/
            /* (*seaPtr)[0] = gbe.cadt_err_bitmap[0] & 0x7FDFF; */
            (*seaPtr)[0] = (((gbe.cadt_err_bitmap[0] >> 10) & 0x1FF) << 16) | (gbe.cadt_err_bitmap[0] & 0x1FF);
            break;
        }
        case SHMOO_G6PHY16_WCK2CK:
        {
            break;
        }
        case SHMOO_G6PHY16_RD_START_FISH:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_data_err_bitmap[0];
            break;
        }
        case SHMOO_G6PHY16_EDC_START_FISH:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));

            if(gbe.bist_sanity_err_bitmap[0])
            {
                LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "sanity error in this spot, error in bytes 0x%x\n"), gbe.bist_sanity_err_bitmap[0]));
            }

            /** combine the results from the array with the sanity errors results */
            (*seaPtr)[0] = gbe.bist_edc_err_bitmap[0] | gbe.bist_sanity_err_bitmap[0];

            break;
        }
        case SHMOO_G6PHY16_RD_DESKEW:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_data_err_bitmap[0];
            break;
        }
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_dbi_err_bitmap[0];
            (*seaPtr)[1] = gbe.bist_edc_err_bitmap[0];
            break;
        }
        case SHMOO_G6PHY16_RD_SHORT:
        {
            /** load read fifo with random pattern */
            /** the short SHMOO steps are used to get us to the correct area but not the specific point */
            g6phy16_vendor_info_t shmoo_vendor_info;
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_vendor_info_get(unit, drc_ndx, &shmoo_vendor_info));
            gbiPtr->nof_commands = shmoo_vendor_info.fifo_depth;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_LOAD_READ_FIFO;
            gbiPtr->read_weight = shmoo_vendor_info.fifo_depth;
            gbiPtr->write_weight = shmoo_vendor_info.fifo_depth;
            for(pattern_iter = 0; pattern_iter < SHMOO_G6PHY16_BIST_NOF_PATTERNS; pattern_iter++)
            {
                for(arr_iter = 0; arr_iter < 4; ++arr_iter)
                {
                    gbiPtr->ldff_data_pattern[pattern_iter][arr_iter] = _g6phy16_get_random();
                }
            }
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));

            /** run training BIST with LDFF(load FIFO) and get relevant results to SHMOO error array */
            gbiPtr->nof_commands = (shmoo_vendor_info.fifo_depth) << 5;
            gbiPtr->training_bist_mode = SHMOO_G6PHY16_TRAINING_BIST_READ_FROM_LDFF;
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_data_err_bitmap[0];
            break;
        }
        case SHMOO_G6PHY16_WR_DESKEW:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_data_err_bitmap[0];
            (*seaPtr)[1] = gbe.bist_data_err_cnt;
            break;
        }
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_dbi_err_bitmap[0];
            break;
        }
        case SHMOO_G6PHY16_WR_SHORT:
        {
            /** run training BIST and get relevant results to SHMOO error array */
            /** the short SHMOO steps are used to get us to the correct area but not the specific point */
            gbiPtr->data_mode = 1;
            for(seed_iter = 0; seed_iter < 8; ++seed_iter)
            {
                gbiPtr->prbs_data_seed[seed_iter] = _g6phy16_get_random();
            }

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_conf_set(unit, drc_ndx, gbiPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_training_bist_err_cnt(unit, drc_ndx, &gbe));
            (*seaPtr)[0] = gbe.bist_data_err_bitmap[0];
            (*seaPtr)[1] = gbe.bist_data_err_cnt;
            break;
        }
        case SHMOO_G6PHY16_RD_EXTENDED:
        {
            /** we use here regular bist to stress the IOs, training bist is used in other READ SHMOO steps */
            /** the extended SHMOO steps are used to get us to the exact right point */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_bist_conf_set(unit, drc_ndx, biPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_bist_run(unit, drc_ndx, biPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_bist_err_cnt(unit, drc_ndx, &be));
            (*seaPtr)[0] = be.bist_err_bmp;
            break;
        }
        case SHMOO_G6PHY16_WR_EXTENDED:
        {
            /** we use here regular bist to stress the IOs, training bist is used in other WRITE SHMO steps */
            /** the extended SHMOO steps are used to get us to the exact right point */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_bist_conf_set(unit, drc_ndx, biPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_bist_run(unit, drc_ndx, biPtr));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_bist_err_cnt(unit, drc_ndx, &be));
            (*seaPtr)[0] = be.bist_err_bmp;
            break;
        }
        default:
        {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_g6phy16_calculate_ui_position(uint32 linearPos, g6phy16_shmoo_container_t *scPtr, g6phy16_ui_position_t *upPtr)
{
    uint32 ui;
    uint32 position;
    
    position = linearPos;
    
    for(ui = 0; ui < SHMOO_G6PHY16_MAX_EFFECTIVE_UI_COUNT; ui++)
    {
        if(linearPos <= scPtr->endUI[ui])
        {
            if(ui != 0)
            {
                position = linearPos - (scPtr->endUI[ui - 1] + 1);
            }
            break;
        }
    }
    
    if(ui == SHMOO_G6PHY16_MAX_EFFECTIVE_UI_COUNT)
    {
        ui--;
        position = linearPos - (scPtr->endUI[ui] + 1);
    }
    
    upPtr->ui = ui;
    upPtr->position = position;
    
    return SOC_E_NONE;
}

STATIC int
_g6phy16_calculate_addr_ui_position(uint32 linearPos, g6phy16_shmoo_container_t *scPtr, g6phy16_ui_position_t *upPtr)
{
    uint32 ui;
    uint32 position;
    
    position = linearPos;
    
    for(ui = 1; ui < SHMOO_G6PHY16_MAX_EFFECTIVE_UI_COUNT; ui += 2)
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
    
    if(ui >= SHMOO_G6PHY16_MAX_EFFECTIVE_UI_COUNT)
    {
        ui -= 2;
        position = linearPos - ((*scPtr).endUI[ui] + 1);
    }
    
    (*upPtr).ui = ui >> 1;
    (*upPtr).position = position;
    
    return SOC_E_NONE;
}


/** This function is used to modify VDLs in a glitch free manner - meaning - moves the VDL in 90 degrees increments. this is done not to violates fifos */
STATIC int
_g6phy16_move_read_max_vdl_glitch_free(int unit, int dram_index, uint32 byte_map, uint32 position, g6phy16_shmoo_container_t *scPtr)
{
    uint32 data;
    uint32 curPosition;
    uint32 halfUI;
    int byte_iter;
    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    uint32 byte_map_arr[1];
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[] = { SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr};

    SOC_INIT_FUNC_DEFS;
    byte_map_arr[0] = byte_map;
    halfUI = scPtr->endUI[0] >> 1;
    SHR_BIT_ITER(byte_map_arr, SHMOO_G6PHY16_BYTES_PER_INTERFACE, byte_iter)
    {
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], dram_index, &data));
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &curPosition));

        /** this makes sure that in each time we're moving only up to a half UI */
        while(curPosition != position)
        {
            if(curPosition > position)
            {
                if(curPosition - position > halfUI)
                {
                    curPosition -= halfUI;
                }
                else
                {
                    curPosition = position;
                }
            }
            else
            {
                if(position - curPosition > halfUI)
                {
                    curPosition += halfUI;
                }
                else
                {
                    curPosition = position;
                }
            }
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, curPosition));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], dram_index, data));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_wck2ck(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    int32 x;
    uint32 i;
    uint32 data0, data1, data2, data3;
    uint32 result0, result1, result2, result3;
    uint32 stop0, stop1, stop2, stop3;
    uint32 stopable0, stopable1, stopable2, stopable3;
    uint32 Hcount0, Hcount1, Hcount2, Hcount3;
    uint32 Lcount0, Lcount1, Lcount2, Lcount3;
    uint32 result;
    uint32 wckInv01, wckInv23;
    uint32 wckInvMR01, wckInvMR23;
    uint32 numWck;
    g6phy16_vendor_info_t shmoo_vendor_info;
/*    int swap; */
    SOC_INIT_FUNC_DEFS;

    scPtr->engageUIshift = 0;
    scPtr->sizeX = scPtr->endUI[4] + 20;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_WCK2CK;
    scPtr->calibMode = SHMOO_G6PHY16_BYTE;
    scPtr->calibPos = SHMOO_G6PHY16_CALIB_ANY_EDGE;

    result = 0x00000000;
    wckInv01 = 0x00000000;
    wckInv23 = 0x00000000;
    wckInvMR01 = 0x00000000;
    wckInvMR23 = 0x00000000;

    stop0 = 0;
    stop1 = 0;
    stop2 = 0;
    stop3 = 0;
    stopable0 = 0;
    stopable1 = 0;
    stopable2 = 0;
    stopable3 = 0;

    Hcount0 = 0;
    Hcount1 = 0;
    Hcount2 = 0;
    Hcount3 = 0;
    Lcount0 = 0;
    Lcount1 = 0;
    Lcount2 = 0;
    Lcount3 = 0;

    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_vendor_info_get(unit, drc_ndx, &shmoo_vendor_info));
    if(shmoo_vendor_info.manufacture_id == SHMOO_G6PHY16_VENDOR_MICRON)
    {
        numWck = 2;
    }
    else
    {
        numWck = 4;
    }

    /*_SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 10, 0x050, 0x0F0));*/

    for(x = 0; x < scPtr->sizeX; x++)
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, drc_ndx, &data0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr, &data0, SOC_PHY_GDDR6_MAX_VDL_STEPf, x));

        if(!stop0)
        {
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data0));
        }
        if(!stop1)
        {
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data0));
        }
        if(!stop2)
        {
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data0));
        }
        if(!stop3)
        {
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data0));
        }

        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

        result0 = 0;
        result1 = 0;
        result2 = 0;
        result3 = 0;

        for(i = 0; i < SHMOO_G6PHY16_REPEAT; i++)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_WRITE_LEVELINGr(unit, drc_ndx, &data0));
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_WRITE_LEVELINGr(unit, drc_ndx, &data1));
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_WRITE_LEVELINGr(unit, drc_ndx, &data2));
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_WRITE_LEVELINGr(unit, drc_ndx, &data3));

            result0 += ((data0 >> 9) & 0x1);
            result1 += ((data1 >> 9) & 0x1);
            result2 += ((data2 >> 9) & 0x1);
            result3 += ((data3 >> 9) & 0x1);

            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        }
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Results: %d %d %d %d\n"), result3, result2, result1, result0));

        if(numWck == 2) {
            result0 = (result0 + result1) >> 1;
            result1 = result0;
            result2 = (result2 + result3) >> 1;
            result3 = result2;
        }

        if(!stop0)
        {
            if(result0 > SHMOO_G6PHY16_REPEAT_HALF)
            {
                result = 0x000000FF;
                Hcount0++;
                Lcount0 = 0;
            }
            else
            {
                result = 0x00000000;
                Lcount0++;
                Hcount0 = 0;
            }

            if(stopable0)
            {
                if(result ^ ((*scPtr).result2D[x - 1] & 0x000000FF))
                {
                    stop0 = 1;
                    (*scPtr).resultData[0] = x;
                }
            }
        }
        else
        {
            result = (*scPtr).result2D[x - 1] & 0x000000FF;
        }

        if(!stop1)
        {
            if(result1 > SHMOO_G6PHY16_REPEAT_HALF)
            {
                result |= 0x0000FF00;
                Hcount1++;
                Lcount1 = 0;
            }
            else
            {
                Lcount1++;
                Hcount1 = 0;
            }

            if(stopable1)
            {
                if((result & 0x0000FF00) ^ ((*scPtr).result2D[x - 1] & 0x0000FF00))
                {
                    stop1 = 1;
                    (*scPtr).resultData[1] = x;
                }
            }
        }
        else
        {
            result |= (*scPtr).result2D[x - 1] & 0x0000FF00;
        }

        if(!stop2)
        {
            if(result2 > SHMOO_G6PHY16_REPEAT_HALF)
            {
                result |= 0x00FF0000;
                Hcount2++;
                Lcount2 = 0;
            }
            else
            {
                Lcount2++;
                Hcount2 = 0;
            }

            if(stopable2)
            {
                if((result & 0x00FF0000) ^ ((*scPtr).result2D[x - 1] & 0x00FF0000))
                {
                    stop2 = 1;
                    (*scPtr).resultData[2] = x;
                }
            }
        }
        else
        {
            result |= (*scPtr).result2D[x - 1] & 0x00FF0000;
        }

        if(!stop3)
        {
            if(result3 > SHMOO_G6PHY16_REPEAT_HALF)
            {
                result |= 0xFF000000;
                Hcount3++;
                Lcount3 = 0;
            }
            else
            {
                Lcount3++;
                Hcount3 = 0;
            }

            if(stopable3)
            {
                if((result & 0xFF000000) ^ ((*scPtr).result2D[x - 1] & 0xFF000000))
                {
                    stop3 = 1;
                    (*scPtr).resultData[3] = x;
                }
            }
        }
        else
        {
            result |= (*scPtr).result2D[x - 1] & 0xFF000000;
        }

        (*scPtr).result2D[x] = result;

        if(!stopable0)
        {
            if((Hcount0 == 12) || (Lcount0 == 12))
            {
                stopable0 = 1;

                if(Lcount0)
                {
                    wckInv01 |= 0x00000020;
                }
            }
        }

        if(!stopable1)
        {
            if((Hcount1 == 12) || (Lcount1 == 12))
            {
                stopable1 = 1;

                if(Lcount1)
                {
                    wckInv01 |= 0x00000080;
                }
            }
        }

        if(!stopable2)
        {
            if((Hcount2 == 12) || (Lcount2 == 12))
            {
                stopable2 = 1;

                if(Lcount2)
                {
                    wckInv23 |= 0x00000020;
                }
            }
        }

        if(!stopable3)
        {
            if((Hcount3 == 12) || (Lcount3 == 12))
            {
                stopable3 = 1;

                if(Lcount3)
                {
                    wckInv23 |= 0x00000080;
                }
            }
        }

        if(stop0 && stop1 && stop2 && stop3)
        {
            /* (*scPtr).calibStart = x; */
            scPtr->sizeX = x + 1;
            break;
        }
    }

    if(stop0 && stop1 && stop2 && stop3)
    {
        /** swaps between channel, byte 0,1 and bytes 2,3. */
/*        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get(unit, drc_ndx, &swap));

        if(swap)
        {
            wckInvMR23 = wckInv01;
            wckInvMR01 = wckInv23;
        }
        else
        {
            wckInvMR01 = wckInv01;
            wckInvMR23 = wckInv23;
        } */
        wckInvMR01 = wckInv01;
        wckInvMR23 = wckInv23;

        if(wckInvMR01)
        {
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 10, wckInvMR01, 0x0F0));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        }

        if(wckInvMR23)
        {
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 10, wckInvMR23, 0x0F0));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        }
        scPtr->wckInv = (wckInv23 << 16) | wckInv01;
    }
    else
    {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "WARNING: WCK2CK alignment failed.\n")));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_rd_start_fish(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 delay;
    uint32 ui;
    uint32 jump;
    uint32 fish0, fish1, fish2, fish3;
    uint32 data, result;
    uint32 inc;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;

    SOC_INIT_FUNC_DEFS;

    jump = 2;

    fish0 = 2;
    fish1 = 2;
    fish2 = 2;
    fish3 = 2;

    scPtr->engageUIshift = 1;
    scPtr->shmooType = SHMOO_G6PHY16_RD_START_FISH;

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), drc_ndx));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_START_FISH\n")));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Searching.......: DATA_VALID_WR2RD_DELAY\n")));

    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));

    for(delay = 0; delay < 16; delay++)
    {
        /** after 8 UIs we start to get the same results as again, so need to check only 8 */
        for(ui = 3; ui < 11; ui++)
        {
            _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, 0, scPtr));

            if(fish0)
            {
                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui));
                _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

                fish0 = 2;
            }
            if(fish1)
            {
                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui));
                _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

                fish1 = 2;
            }
            if(fish2)
            {
                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui));
                _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

                fish2 = 2;
            }
            if(fish3)
            {
                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui));
                _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

                fish3 = 2;
            }

            for(x = 0; x < ((scPtr->endUI[0] + 1) >> jump); x++)
            {
                _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, x << jump, scPtr));

                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, NULL, &gbi, &sea));

                result = sea[0];

                if(result != 0xFFFFFFFF)
                {
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "d: %02d    UI: %d    Pos: %03d    Result: 0x%08X\n"), delay, ui, x << jump, result));
                }

                if(fish0)
                {
                    if((result & 0x000000FF) != 0x000000FF)
                    {
                        fish0--;

                        if(!fish0)
                        {
                            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui + 2));
                            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
                        }
                    }
                    else
                    {
                        fish0 = 2;
                    }
                }
                if(fish1)
                {
                    if((result & 0x0000FF00) != 0x0000FF00)
                    {
                        fish1--;

                        if(!fish1)
                        {
                            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui + 2));
                            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
                        }
                    }
                    else
                    {
                        fish1 = 2;
                    }
                }
                if(fish2)
                {
                    if((result & 0x00FF0000) != 0x00FF0000)
                    {
                        fish2--;

                        if(!fish2)
                        {
                            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui + 2));
                            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
                        }
                    }
                    else
                    {
                        fish2 = 2;
                    }
                }
                if(fish3)
                {
                    if((result & 0xFF000000) != 0xFF000000)
                    {
                        fish3--;

                        if(!fish3)
                        {
                            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
                            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, ui + 2));
                            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
                        }
                    }
                    else
                    {
                        fish3 = 2;
                    }
                }
            }
        }

        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Delay %02d........: %01d %01d %01d %01d\n"), delay, fish3, fish2, fish1, fish0));

        if(!(fish0 || fish1 || fish2 || fish3))
        {
            break;
        }

        if(delay == 15)
        {
            break;
        }

        if(fish0)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }
        if(fish1)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }
        if(fish2)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }
        if(fish3)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }

    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_edc_start_fish(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 delay;
    uint32 ui;
    uint32 jump;
    uint32 fish0, fish1, fish2, fish3;
    uint32 data, result;
    uint32 inc;
    uint32 valid_wr2rd_delay[4] = {0};
    uint32 min_valid_wr2rd_delay = 16;
    int byte_index;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    jump = 2;

    fish0 = 1;
    fish1 = 1;
    fish2 = 1;
    fish3 = 1;

    scPtr->engageUIshift = 1;
    scPtr->shmooType = SHMOO_G6PHY16_EDC_START_FISH;

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), drc_ndx));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: EDC_START_FISH\n")));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Searching.......: EDC_VALID_WR2RD_DELAY\n")));

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, &ui));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, &valid_wr2rd_delay[0]));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_UI_SHIFTf, ui));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, &ui));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, &valid_wr2rd_delay[1]));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_UI_SHIFTf, ui));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, &ui));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, &valid_wr2rd_delay[2]));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_UI_SHIFTf, ui));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, &ui));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, &valid_wr2rd_delay[3]));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_UI_SHIFTf, ui));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

    /** get minimal valid read to write delay found and use it (value - 1) to start edc scan */
    for(byte_index = 0; byte_index < 4; ++byte_index)
    {
        min_valid_wr2rd_delay = UTILEX_MIN(min_valid_wr2rd_delay, valid_wr2rd_delay[byte_index]);
    }
    min_valid_wr2rd_delay = (min_valid_wr2rd_delay > 0) ? min_valid_wr2rd_delay - 1 : min_valid_wr2rd_delay;

    for(delay = 0; delay < 16; delay++)
    {
        /** start scanning only for relevant delays to save time */
        if(delay >= min_valid_wr2rd_delay)
        {
            for(x = 0; x < ((scPtr->endUI[3] + 1) >> jump); x++)
            {
                _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, x << jump, scPtr));

                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                /** DRC init */
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_soft_reset_drc_without_dram(unit, drc_ndx));
                /** init and run BIST */
                _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));
                _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, NULL, &gbi, &sea));

                result = sea[0];

                if(result != 0x0000000F)
                {
                    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "delay: %02d    UI: %delay    Pos: %03d    Result: 0x%08X\n"), delay, ui, x << jump, result));
                }

                if(fish0)
                {
                    if(!(result & 0x00000001))
                    {
                        fish0--;
                    }
                }
                if(fish1)
                {
                    if(!(result & 0x00000002))
                    {
                        fish1--;
                    }
                }
                if(fish2)
                {
                    if(!(result & 0x00000004))
                    {
                        fish2--;
                    }
                }
                if(fish3)
                {
                    if(!(result & 0x00000008))
                    {
                        fish3--;
                    }
                }
            }
        }

        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Delay %02d........: %01d %01d %01d %01d\n"), delay, fish3, fish2, fish1, fish0));

        if(!(fish0 || fish1 || fish2 || fish3))
        {
            break;
        }

        if(delay == 15)
        {
            break;
        }

        if(fish0)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }
        if(fish1)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }
        if(fish2)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }
        if(fish3)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, &inc));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_INCf, inc ^ 0x1));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, delay + 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
        }

    }

    /** DRC init */
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_soft_reset_drc_without_dram(unit, drc_ndx));

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_wr_deskew(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui;
    uint32 position;
    uint32 b;
    uint32 data;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    scPtr->engageUIshift = 1;
    scPtr->sizeX =  scPtr->endUI[SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT - 1] - 19;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_WR_DESKEW;

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    for(b = 0; b < SHMOO_G6PHY16_BYTE; b++)
    {
        _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, b, scPtr, &bi, &gbi));

        position = 0;
        ui = 0;

        for(x = 0; x < scPtr->sizeX; x++)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
            if((x > scPtr->endUI[ui]) && (ui < SHMOO_G6PHY16_LAST_EFFECTIVE_UI))
            {
                ui++;
                position = 0;
            }

            /** both the max and the min VDL controls the VDL,
             * the only difference is that max VDL moves the whole byte where as min moves only the bit. */
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, ui));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, position));
            position++;

            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));

            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "bit in byte: 0x%01x; VDL position:%04d; UI position:%02d; SHMOO error array:0x%08x; SHMOO error counter=%05d \n"), b, position, ui, sea[0], sea[1]));
            scPtr->result2D[x] |= (sea[0] & (0x01010101 << b));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_dbi_wr_deskew(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui;
    uint32 position;
    uint32 data;
    uint32 pos0, pos1, pos2, pos3;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    scPtr->engageUIshift = 1;
    scPtr->sizeX =  scPtr->endUI[SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT - 1] - 19;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_DBI_WR_DESKEW;

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &pos0));
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &pos1));
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &pos2));
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &pos3));

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));

    position = 0;
    ui = 0;

    for(x = 0; x < scPtr->sizeX; x++)
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
        if((x > scPtr->endUI[ui]) && (ui < SHMOO_G6PHY16_LAST_EFFECTIVE_UI))
        {
            ui++;
            position = 0;
        }
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, ui));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, position));
        position++;

        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));

        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

        _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));

        scPtr->result2D[x] |= (sea[0] & 0x0000000F);
    }

    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, pos0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, pos1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, pos2));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, pos3));

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_wr_short(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui;
    uint32 position;
    uint32 data;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    scPtr->engageUIshift = 1;
    scPtr->sizeX = scPtr->endUI[SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT - 1] - 19;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_WR_SHORT;

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));

    position = 0;
    ui = 0;

    for(x = 0; x < scPtr->sizeX; x++)
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
        if((x > scPtr->endUI[ui]) && (ui < SHMOO_G6PHY16_LAST_EFFECTIVE_UI))
        {
            ui++;
            position = 0;
        }
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, ui));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, position));
        position++;

        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));

        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

        _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL position:%04d; UI position:%02d; SHMOO error array:0x%08x; SHMOO error counter=%05d \n"), position, ui, sea[0], sea[1]));

        scPtr->result2D[x] = sea[0];
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_wr_extended(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
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
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    uint8 optimized_run = TRUE;
    SHR_BIT_DCL_CLR_NAME(positions_to_scan, 1000);
    int scanning_range = scPtr->endUI[1];
    SOC_INIT_FUNC_DEFS;

    /** clear result2D array (set all positions to errors */
    sal_memset(scPtr->result2D, 0xFF, sizeof(scPtr->result2D));

    sizeY = SHMOO_G6PHY16_MAX_VREF_RANGE;
    xStart = 0;

    yCapMin = 2;
    yCapMax = 83;

    if(scPtr->restore)
    {
        yCapMin = 0;
        yCapMax = 1;
        sizeY = 1;
    }

    scPtr->engageUIshift = 1;
    scPtr->sizeX = scPtr->endUI[SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT - 1] - 19; /* -20 + 1; */
    scPtr->sizeY = sizeY;
    scPtr->yCapMin = yCapMin;
    scPtr->yCapMax = yCapMax;
    scPtr->shmooType = SHMOO_G6PHY16_WR_EXTENDED;

    if(optimized_run)
    {
        soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_WRITE_MAX_VDL_DATAl[] = {   SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr,
                                                                        SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr,
                                                                        SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr,
                                                                        SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr};
        int byte_index;
        uint32 data;
        /** read results from write short, define range around found centers and check only that range, assume all other points have failed. */
        for(byte_index = 0; byte_index < 4; ++byte_index)
        {
            int center_position;
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_WRITE_MAX_VDL_DATAl[byte_index], drc_ndx, &data));
            /** translate ui and position to flat position */
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_WRITE_MAX_VDL_DATAl[byte_index], data, SOC_PHY_GDDR6_UI_SHIFTf, &ui));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_WRITE_MAX_VDL_DATAl[byte_index], data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &position));
            center_position = (scPtr->size1000UI * ui) / 1000 + position;
            /** mark position in bitmap (mark x positions around center) */
            SHR_BITSET_RANGE(positions_to_scan, center_position - scanning_range / 2, scanning_range);
        }
    }

    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));

    for(y = yCapMin; y < yCapMax; y++)
    {
        if(!scPtr->restore)
        {
            MRfield = y | 0x780;
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, MRfield, 0xFFF));
            MRfield = y | 0xF80;
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, MRfield, 0xFFF));
        }

        position = 0;
        ui = 0;

        for(x = 0; x < scPtr->sizeX; x++)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
            if((x > scPtr->endUI[ui]) && (ui < SHMOO_G6PHY16_LAST_EFFECTIVE_UI))
            {
                ui++;
                position = 0;
            }

            if(optimized_run && (!SHR_IS_BITSET(positions_to_scan, x)))
            {
                position++;
                continue;
            }

            /** the bits are already de-skewed, so need to touch only the max (byte granularity) VDL. */
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, ui));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, position));
            position++;

            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));

            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));

            scPtr->result2D[x + xStart] = sea[0];
        }

        xStart += scPtr->sizeX;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_rd_deskew(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 b;
    uint32 data;
    uint32 result0; /*, resultHalfUI; */
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    scPtr->engageUIshift = 0;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_RD_DESKEW;

    result0 = 0;

    /** Preliminary check to see if we're in the SHMOO eye or not. */
    for(b = 0; b < SHMOO_G6PHY16_BYTE; b++)
    {
        _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, b, scPtr, &bi, &gbi));
        _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, 0, scPtr));
        _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));

        /** check results relative only to the bit that was checked in this iteration */
        result0 |= (sea[0] & (0x01010101 << b));
    }

    if(result0 == 0xFFFFFFFF)
    {
        /** if none of the bits was correct, it means that we're not in the eye */
        scPtr->sizeX = scPtr->endUI[4] + 1;
        scPtr->calibStart = 0;

        data = 1;
    }
    else
    {
        scPtr->sizeX = scPtr->endUI[4] + 1;
        scPtr->calibStart = 0;

        data = 2;
    }
    /** this data will be used in a later stage to indicate in which area the eye was found or not found,
     * and as indication that this stage (rd_deskew) was already done.*/
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, data));

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    for(b = 0; b < SHMOO_G6PHY16_BYTE; b++)
    {
        /** begin actual RD_DESKEW */
        _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, b, scPtr, &bi, &gbi));
        _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, scPtr->calibStart, scPtr));

        for(x = scPtr->calibStart; x <scPtr->sizeX; x++)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, x));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));

            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));

            scPtr->result2D[x] |= (sea[0] & (0x01010101 << b));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_dbi_edc_rd_deskew(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 b;
    uint32 data;
    uint32 pos0, pos1, pos2, pos3;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;

    SOC_INIT_FUNC_DEFS;

    scPtr->engageUIshift = 0;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_DBI_EDC_RD_DESKEW;

    /** Check that RD_DESKEW was already done. */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, &data));
    switch(data & 0x3)
    {
        case 1:
        case 2:
            scPtr->sizeX = scPtr->endUI[4] + 1;
            scPtr->calibStart = 0;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "WARNING: RD_DESKEW SHMOO has not been run to its end, or Control Reserved Register has been tempered with.\n")));
    }

    /** save found VDL position for READ_MAX_VDLs */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &pos0));
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &pos1));
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &pos2));
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &pos3));
    scPtr->resultData[0] = pos0;
    scPtr->resultData[1] = pos1;
    scPtr->resultData[2] = pos2;
    scPtr->resultData[3] = pos3;
      
    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    for(b = 0; b < 2; b++)
    {
        _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, b, scPtr, &bi, &gbi));
        _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, scPtr->calibStart, scPtr));
        
        for(x = scPtr->calibStart; x <scPtr->sizeX; x++)
        {
            /** change read VDL to get results for DBI and EDC */
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, x));
            
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            
            _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));
            
            scPtr->result2D[x] |= ((sea[b] & 0x0000000F) << (b << 2));
            LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "bit %d; DBI SHMOO error array: 0x%08x; EDC SHMOO error array: 0x%08x; position:%d\n"), b, sea[0], sea[1], x));
        }
    }
    
    /** restore found VDL position of READ_MAX_VDLs */
    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x1, pos0, scPtr));
    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x2, pos1, scPtr));
    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x4, pos2, scPtr));
    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x8, pos3, scPtr));

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_rd_short(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 data;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    scPtr->sizeX = scPtr->endUI[4] + 1;
    scPtr->engageUIshift = 0;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_RD_SHORT;

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));
    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, scPtr->calibStart, scPtr));

    for(x = 0; x < scPtr->sizeX; x++)
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, x));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));

        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

        _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));

        scPtr->result2D[x] = sea[0];
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_rd_extended(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 sizeY;
    uint32 xStart;
    uint32 data;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    sizeY = SHMOO_G6PHY16_MAX_VREF_RANGE;
    xStart = 0;

    /** those VREF values represents 0.7 vddo, they are taken from the PHY spec */
    yCapMin = 33;
    yCapMax = 114;

    /** verify that read short was run before running read extended. */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, &data));
    switch((data & 0xC) >> 2)
    {
        case 1:
            scPtr->sizeX = scPtr->endUI[4] + 1; /* + ((data >> 4) & 0x1FF) + 20; */
            scPtr->calibStart = 0;
            break;
        case 2:
            scPtr->sizeX = scPtr->endUI[4] + 1; /* + ((data >> 4) & 0x1FF) + 20; */
            scPtr->calibStart = 0;
            break;
        default:
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: RD_SHORT shmoo has not been run to its end, or Control Reserved Register has been tempered with.\n")));
            scPtr->sizeX = scPtr->endUI[4] + 1;
            scPtr->calibStart = 0;
            break;
    }
    
    /** for extended VREF scan a larger range of VERFs */
    if(scPtr->debugMode & SHMOO_G6PHY16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
    {
        yCapMin = 11;
        yCapMax = 126;
    }
    
    /** when restoring tune parameters, there's no need to find the correct VREF again, it is restored.
     * so need only a single iteration on the vref. regrading the VDL, still need to find it exact position
     * even though 'restoring' the tuning info */
    if(scPtr->restore)
    {
        yCapMin = 0;
        yCapMax = 1;
        sizeY = 1;
    }

    scPtr->engageUIshift = 0;
    scPtr->sizeY = sizeY;
    scPtr->yCapMin = yCapMin;
    scPtr->yCapMax = yCapMax;
    scPtr->shmooType = SHMOO_G6PHY16_RD_EXTENDED;
    
    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));

    for(y = yCapMin; y < yCapMax; y++)
    {
        if(!scPtr->restore)
        {
            /** the LSB of the DAC is not used, so need to shift left by 1 */
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, y << 1));

            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, data));
        }

        _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, scPtr->calibStart, scPtr));

        for(x = scPtr->calibStart; x < scPtr->sizeX; x++)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, x));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, drc_ndx, data));

            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, &bi, &gbi, &sea));

            scPtr->result2D[x + xStart] = sea[0];
        }

        xStart += scPtr->sizeX;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_addr_ctrl_short(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui, addr_ui;
    uint32 position;
    uint32 data;
    uint32 prevResult;
    g6phy16_bist_info_t bi;
    g6phy16_training_bist_info_t gbi;
    g6phy16_shmoo_error_array_t sea;
    SOC_INIT_FUNC_DEFS;

    /** clear result2D array */
    sal_memset(scPtr->result2D, 0, sizeof(scPtr->result2D));

    scPtr->engageUIshift = 1;
    scPtr->sizeX = scPtr->endUI[SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT >> 1] + 1;
    scPtr->sizeY = 1;
    scPtr->yCapMin = 0;
    scPtr->yCapMax = 1;
    scPtr->shmooType = SHMOO_G6PHY16_ADDR_CTRL_SHORT;

    _SOC_IF_ERR_EXIT(_g6phy16_initialize_bist(unit, drc_ndx, -1, scPtr, &bi, &gbi));

    position = 0;
    ui = 0;
    addr_ui = 0;
    prevResult = 1;

    for(x = 0; x < scPtr->sizeX; x++)
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, &data));
        if((x > scPtr->endUI[ui]) && (ui < SHMOO_G6PHY16_LAST_EFFECTIVE_UI))
        {
            ui++;
            addr_ui = ui >> 1;
            if((addr_ui << 1) == ui)
            {
                position = 0;
            }
        }
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_UI_SHIFTf, addr_ui));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, position));
        position++;

        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, drc_ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, drc_ndx, data));

        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

        if(prevResult)
        {
            /** if previous result has an error - a violation has occurred and it is needed to re-init again to the appropriate place (before CADT) */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_init(unit, drc_ndx, SHMOO_GDDR6_DRAM_INIT_UNTIL_CADT));
        }

        _SOC_IF_ERR_EXIT(_g6phy16_run_bist(unit, drc_ndx, scPtr, NULL, &gbi, &sea));

        scPtr->result2D[x] = sea[0];
        LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Shmoo error array: 0x%x, position:%d\n"), sea[0], x));
        prevResult = sea[0];
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_do(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    SOC_INIT_FUNC_DEFS;
    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_addr_ctrl_short(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_WCK2CK:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_wck2ck(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_RD_START_FISH:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_rd_start_fish(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_EDC_START_FISH:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_edc_start_fish(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_rd_deskew(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_dbi_edc_rd_deskew(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_RD_SHORT:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_rd_short(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_wr_deskew(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_dbi_wr_deskew(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_WR_SHORT:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_wr_short(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_RD_EXTENDED:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_rd_extended(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_WR_EXTENDED:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_wr_extended(unit, drc_ndx, scPtr));
            break;
        case SHMOO_G6PHY16_CDR:
            _SOC_IF_ERR_EXIT(soc_g6phy16_cdr_enable(unit, drc_ndx));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_g6phy16_calib_2D(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr, uint32 calibMode, uint32 calibPos, uint32 calibSweep, uint32 decisionMode, uint32 groupOrder, uint32 groupNumBits)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 calibStart[SHMOO_G6PHY16_WORD];
    uint32 calibEnd[SHMOO_G6PHY16_WORD];
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 i, j;
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
    uint32 maxPassLengthArray[SHMOO_G6PHY16_WORD];
    uint32 xSum[SHMOO_G6PHY16_WORD], ySum[SHMOO_G6PHY16_WORD], passCount[SHMOO_G6PHY16_WORD];
    uint32 result;
    uint32 iter = 0;
    SOC_INIT_FUNC_DEFS;

    xStart = 0;
    sizeX = scPtr->sizeX;
    yCapMin = scPtr->yCapMin;
    yCapMax = scPtr->yCapMax;
    
    switch(calibSweep)
    {
        case SHMOO_G6PHY16_CALIB_SWEEP_1:
            for(i = 0; i < SHMOO_G6PHY16_WORD; i++)
            {
                calibStart[i] = scPtr->calibStart;
                calibEnd[i] = sizeX;
            }
            break;
        case SHMOO_G6PHY16_CALIB_SWEEP_BYTE_TO_BIT:
            iter = 4;
            break;
        case SHMOO_G6PHY16_CALIB_SWEEP_HALFWORD_TO_BIT:
            iter = 2;
            break;
        case SHMOO_G6PHY16_CALIB_SWEEP_WORD_TO_BIT:
            iter = 1;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported 2D calibration sweep: %02d\n"), calibSweep));
    }
    
    if(calibSweep != SHMOO_G6PHY16_CALIB_SWEEP_1)
    {
        switch(groupOrder)
        {
            case SHMOO_G6PHY16_CALIB_GROUP_ORDER_00112233:
                for(i = 0; i < iter; i++)
                {
                    maxMidPointX = scPtr->resultData[i] & 0xFFFF;
                    
                    if(maxMidPointX < scPtr->endUI[0])
                    {
                        calibStart[i * groupNumBits] = 0;
                    }
                    else
                    {
                        calibStart[i * groupNumBits] = maxMidPointX - scPtr->endUI[0];
                    }
                    
                    if(maxMidPointX + scPtr->endUI[0] < sizeX)
                    {
                        calibEnd[i * groupNumBits] = maxMidPointX + scPtr->endUI[0];
                    }
                    else
                    {
                        calibEnd[i * groupNumBits] = sizeX;
                    }

                    for(j = 1; j < groupNumBits; j++)
                    {
                        calibStart[(i * groupNumBits) + j] = calibStart[i * groupNumBits];
                        calibEnd[(i * groupNumBits) + j] = calibEnd[i * groupNumBits];
                    }
                }
                break;
            case SHMOO_G6PHY16_CALIB_GROUP_ORDER_01230123:
                for(i = 0; i < iter; i++)
                {
                    /* maxMidPointX = scPtr->resultData[i] & 0xFFFF; */
                    calibStart[i] = 0;
                    calibEnd[i] = sizeX;

                    /* if(maxMidPointX < scPtr->endUI[0])
                    {
                        calibStart[i] = 0;
                    }
                    else
                    {
                        calibStart[i] = maxMidPointX - scPtr->endUI[0];
                    }
                    
                    if(maxMidPointX + scPtr->endUI[0] < sizeX)
                    {
                        calibEnd[i] = maxMidPointX + scPtr->endUI[0];
                    }
                    else
                    {
                        calibEnd[i] = sizeX;
                    } */
                }

                for(j = 1; j < groupNumBits; j++)
                {
                    for(i = 0; i < iter; i++)
                    {
                        calibStart[(j * iter) + i] = calibStart[i];
                        calibEnd[(j * iter) + i] = calibEnd[i];
                    }
                }
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported 2D calibration group order: %02d\n"), groupOrder));
        }

        for(i = iter * groupNumBits; i < SHMOO_G6PHY16_WORD; i++)
        {
            calibStart[i] = 0;
            calibEnd[i] = 0;
        }
    }

    switch(calibMode)
    {
        case SHMOO_G6PHY16_BIT:
            iter = 32;
            shiftAmount = 0;
            dataMask = 0x1;
            break;
        case SHMOO_G6PHY16_BYTE:
            iter = 4;
            shiftAmount = 3;
            dataMask = 0xFF;
            break;
        case SHMOO_G6PHY16_HALFWORD:
            iter = 2;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            break;
        case SHMOO_G6PHY16_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported 2D calibration mode: %02d\n"), calibMode));
    }

    for(i = 0; i < iter; i++)
    {
        scPtr->resultData[i] = 0;
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
            for(x = calibStart[i]; x < calibEnd[i]; x++)
            {
                switch(decisionMode)
                {
                    case SHMOO_G6PHY16_CALIB_PASS_ALL:
                        result = (scPtr->result2D[xStart + x] >> (i << shiftAmount)) & dataMask;
                        break;
                    case SHMOO_G6PHY16_CALIB_PASS_ANY:
                        result = (((scPtr->result2D[xStart + x] >> (i << shiftAmount)) & dataMask) == dataMask);
                        break;
                    default:
                        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported calibration decision mode: %02d\n"), decisionMode));
                }

                if(result)
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

                    if(x == calibEnd[i] - 1)
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
                case SHMOO_G6PHY16_CALIB_FAIL_START:
                case SHMOO_G6PHY16_CALIB_RISING_EDGE:
                    if(failStartSeen > 0)
                    {
                        maxMidPointX = failStartSeen;
                        scPtr->resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_G6PHY16_CALIB_PASS_START:
                case SHMOO_G6PHY16_CALIB_FALLING_EDGE:
                    if(passStartSeen > 0)
                    {
                        maxMidPointX = passStartSeen;
                        scPtr->resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_G6PHY16_CALIB_CENTER_PASS:
                    if((maxPassLength > 0) && (maxPassLengthArray[i] < maxPassLength))
                    {
                        maxMidPointX = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        scPtr->resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                        maxPassLengthArray[i] = maxPassLength;
                    }
                    break;
                case SHMOO_G6PHY16_CALIB_XY_AVERAGE:
                    if(passCount[i])
                    {
                        scPtr->resultData[i] = ((ySum[i] / passCount[i]) << 16) | ((xSum[i] / passCount[i]) & 0xFFFF);
                    }
                    else
                    {
                        scPtr->resultData[i] = 0;
                    }
                    break;
                case SHMOO_G6PHY16_CALIB_ANY_EDGE:
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported calibration position: %02d\n"), calibPos));
            }
        }
        xStart += sizeX;
    }

    scPtr->calibMode = calibMode;
    scPtr->calibPos = calibPos;

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_calib_2D(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    SOC_INIT_FUNC_DEFS;
    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_HALFWORD, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_WCK2CK:
            break;
        case SHMOO_G6PHY16_RD_START_FISH:
            break;
        case SHMOO_G6PHY16_EDC_START_FISH:
            break;
        case SHMOO_G6PHY16_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ANY, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_BYTE_TO_BIT, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_00112233, 8));
            break;
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_BYTE_TO_BIT, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_01230123, 2));
            break;
        case SHMOO_G6PHY16_RD_SHORT:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_WR_SHORT:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE, SHMOO_G6PHY16_CALIB_CENTER_PASS, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_RD_EXTENDED:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE, SHMOO_G6PHY16_CALIB_XY_AVERAGE, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_WR_EXTENDED:
            _SOC_IF_ERR_EXIT(_g6phy16_calib_2D(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE, SHMOO_G6PHY16_CALIB_XY_AVERAGE, SHMOO_G6PHY16_CALIB_SWEEP_1, SHMOO_G6PHY16_CALIB_PASS_ALL, SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE, 0));
            break;
        case SHMOO_G6PHY16_CDR:
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }

exit:
    SOC_FUNC_RETURN;
}

static int shmoo_g6phy16_min_vdls_adujst(
    int unit,
    int drc_index,
    int byte_index,
    uint32 add_value)
{
    int bit_iter;
    uint32 reg_val;
    uint32 min_vdl_val;
    uint32 new_min_vdl_val;
    soc_phy_gddr6_reg_t DQ_MIN_VDL_REGS[4][8]={     {   SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r},
                                                    {   SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r},
                                                    {   SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r},
                                                    {   SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r,
                                                        SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r}
                                             };
    SOC_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "changing min VDL values for interface%d; byte %d; by value of %d .\n"), drc_index, byte_index, add_value));
    /** iterate over bits in relevant byte */
    for(bit_iter = 0; bit_iter < 8; ++bit_iter)
    {
        /** read current min VDL value */
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, DQ_MIN_VDL_REGS[byte_index][bit_iter], drc_index, &reg_val));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, DQ_MIN_VDL_REGS[byte_index][bit_iter], reg_val, SOC_PHY_GDDR6_MIN_VDL_STEPf, &min_vdl_val));

        /** set to new Value */
        new_min_vdl_val = min_vdl_val + add_value;
        if(new_min_vdl_val >= SHMOO_G6PHY16_MIN_VDL_LENGTH)
        {
            /** if new value is too big, set to biggest possible value and print a warning */
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Bit skew adjustment is beyond available MIN VDL range. Results may not be optimal.\n")));
            new_min_vdl_val = SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
        }

        /** write new VDL value */
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, DQ_MIN_VDL_REGS[byte_index][bit_iter], &reg_val, SOC_PHY_GDDR6_MIN_VDL_STEPf, new_min_vdl_val));
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, DQ_MIN_VDL_REGS[byte_index][bit_iter], drc_index, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}
STATIC int
_shmoo_g6phy16_set_new_step(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 calibMode;
    uint32 engageUIshift;
    uint32 min0, min1, min2, min3;
    uint32 max0, max1, max2, max3;
    uint32 val0, val1, val2, val3;
    uint32 byte0, byte1, byte2, byte3;
    uint32 x, xMin, xMax;
    uint32 b;
    uint32 MRfield0, MRfield1, MRfield2, MRfield3;
    uint32 data;
    uint32 initAdjustment;
    g6phy16_ui_position_t up;
    int swap;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    calibMode = scPtr->calibMode;
    engageUIshift = scPtr->engageUIshift;

    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_HALFWORD:
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _g6phy16_calculate_addr_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up);
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, data));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, drc_ndx, data));

                    if(engageUIshift)
                    {
                        _g6phy16_calculate_addr_ui_position(scPtr->resultData[1] & 0xFFFF, scPtr, &up);
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[1] & 0xFFFF));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, drc_ndx, data));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, drc_ndx, data));

                    /** clean-up in case of violations, do dram re-init until the relevant point */
                    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_init(unit, drc_ndx, SHMOO_GDDR6_DRAM_INIT_UNTIL_CADT));
                    break;
                case SHMOO_G6PHY16_WORD:
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _g6phy16_calculate_addr_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up);
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, data));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, drc_ndx, data));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, drc_ndx, data));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, drc_ndx, data));

                    /** clean-up in case of violations, do dram re-init until the relevant point */
                    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_init(unit, drc_ndx, SHMOO_GDDR6_DRAM_INIT_UNTIL_CADT));
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_WCK2CK:
            break;
        case SHMOO_G6PHY16_RD_START_FISH:
            break;
        case SHMOO_G6PHY16_EDC_START_FISH:
            break;
        case SHMOO_G6PHY16_RD_DESKEW:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    min0 = scPtr->sizeX;
                    min1 = min0;
                    min2 = min0;
                    min3 = min0;
                    max0 = 0;
                    max1 = 0;
                    max2 = 0;
                    max3 = 0;
                    
                    for(b = 0; b < SHMOO_G6PHY16_BYTE; b++)
                    {
                        /** take from resultData only the info that is relevant for VDLs (16 LSBs) */
                        /** check the position of bit X in every byte to determine what is the max and min position of that byte */
                        val0 = scPtr->resultData[b] & 0xFFFF;
                        val1 = scPtr->resultData[b + 8] & 0xFFFF;
                        val2 = scPtr->resultData[b + 16] & 0xFFFF;
                        val3 = scPtr->resultData[b + 24] & 0xFFFF;
                        
                        if(min0 > val0)
                        {
                            min0 = val0;
                        }
                        if(min1 > val1)
                        {
                            min1 = val1;
                        }
                        if(min2 > val2)
                        {
                            min2 = val2;
                        }
                        if(min3 > val3)
                        {
                            min3 = val3;
                        }
                        
                        if(max0 < val0)
                        {
                            max0 = val0;
                        }
                        if(max1 < val1)
                        {
                            max1 = val1;
                        }
                        if(max2 < val2)
                        {
                            max2 = val2;
                        }
                        if(max3 < val3)
                        {
                            max3 = val3;
                        }
                    }

                    /** Check for each byte that de-skewing is possible.
                     * Do it by checking that the distance between the max bit and the min bit is not bigger than the length of the per bit VDL.
                     * If such a case was found - print a warning and change max position to a place that will allow to de-skew to the best of our ability*/
                    if((max0 - min0) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val0 = 1;
                        max0 = min0 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val0 = 0;
                    }
                    if((max1 - min1) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val1 = 1;
                        max1 = min1 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val1 = 0;
                    }
                    if((max2 - min2) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val2 = 1;
                        max2 = min2 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val2 = 0;
                    }
                    if((max3 - min3) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val3 = 1;
                        max3 = min3 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val3 = 0;
                    }

                    if(val0 || val1 || val2 || val3)
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Bit skew beyond available MIN VDL range. Results may not be optimal.\n")));
                    }

                    /** re-align swaps */
                    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get(unit, drc_ndx, &swap));
                    if(swap)
                    {
                        byte0 = 2;
                        byte1 = 3;
                        byte2 = 0;
                        byte3 = 1;
                    }
                    else
                    {
                        byte0 = 0;
                        byte1 = 1;
                        byte2 = 2;
                        byte3 = 3;
                    }

                    initAdjustment = 0;
                    data = 0;
                    /** de-skew each bit in each byte by setting the delta between the relevant bit to the max bit in that byte,
                     * make sure to take into acount DQ swaps so the correct physical bits are de-skewed according to the results gathered. */
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][0]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][1]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][2]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][3]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][4]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][5]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][6]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max0 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][7]] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][0] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][1] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][2] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][3] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][4] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][5] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][6] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max1 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][7] + 8] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][0] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][1] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][2] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][3] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][4] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][5] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][6] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max2 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][7] + 16] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][0] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][1] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][2] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][3] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][4] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][5] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][6] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, max3 - (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][7] + 24] & 0xFFFF) + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x1, max0 - initAdjustment, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x2, max1 - initAdjustment, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x4, max2 - initAdjustment, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x8, max3 - initAdjustment, scPtr));

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                {
                    soc_phy_gddr6_reg_t DQ_MAX_VDL_REGS[] = {   SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr};
                    soc_phy_gddr6_reg_t DBI_MIN_VDL_REGS[] = {  SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_DBI_Nr, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_DBI_Nr,
                                                                SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_DBI_Nr, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_DBI_Nr};
                    soc_phy_gddr6_reg_t EDC_MIN_VDL_REGS[] = {  SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_EDCr, SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_EDCr,
                                                                SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_EDCr, SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_EDCr};
                    int bit_iter;

                    /** same as with the DQ bits, we need to calculate the individual de-skew of those 2 bits (DBI and EDC) */
                    for(bit_iter = 0; bit_iter < 4; ++bit_iter)
                    {
                        int dbi_val = scPtr->resultData[bit_iter] & 0xFFFF;
                        int edc_val = scPtr->resultData[bit_iter + 4] & 0xFFFF;
                        int new_max;
                        uint32 old_max;

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, DQ_MAX_VDL_REGS[bit_iter], drc_ndx, &data));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, DQ_MAX_VDL_REGS[bit_iter], data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &old_max));

                        /** get new max value for the min VDL */
                        new_max = UTILEX_MAX3(old_max, dbi_val, edc_val);

                        /** fix DQ min vdls according to new max */
                        if(old_max < new_max)
                        {
                            _SOC_IF_ERR_EXIT(shmoo_g6phy16_min_vdls_adujst(unit, drc_ndx, bit_iter, new_max - old_max));
                        }

                        /** set DBI min VDL */
                        data = 0;
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, DBI_MIN_VDL_REGS[bit_iter], &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, new_max - dbi_val));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, DBI_MIN_VDL_REGS[bit_iter], drc_ndx, data));

                        /** set EDCI min VDL */
                        data = 0;
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, EDC_MIN_VDL_REGS[bit_iter], &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, new_max - edc_val));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, EDC_MIN_VDL_REGS[bit_iter], drc_ndx, data));
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                }
                default:
                {
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
                    break;
                }
            }
            break;
        case SHMOO_G6PHY16_RD_SHORT:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BYTE:
                    /** take from resultData only the info that is related to VDLs (16 LSBs, the 16 MSBs contain vref info) */
                    _g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x1, scPtr->resultData[0] & 0xFFFF, scPtr);
                    _g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x2, scPtr->resultData[1] & 0xFFFF, scPtr);
                    _g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x4, scPtr->resultData[2] & 0xFFFF, scPtr);
                    _g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x8, scPtr->resultData[3] & 0xFFFF, scPtr);

                    xMin = scPtr->resultData[0] & 0xFFFF;
                    xMax = xMin;

                    x = scPtr->resultData[1] & 0xFFFF;
                    if(x < xMin)
                    {
                        xMin = x;
                    }
                    if(x > xMax)
                    {
                        xMax = x;
                    }

                    x = scPtr->resultData[2] & 0xFFFF;
                    if(x < xMin)
                    {
                        xMin = x;
                    }
                    if(x > xMax)
                    {
                        xMax = x;
                    }

                    x = scPtr->resultData[3] & 0xFFFF;
                    if(x < xMin)
                    {
                        xMin = x;
                    }
                    if(x > xMax)
                    {
                        xMax = x;
                    }

                    xMin = xMin % scPtr->endUI[1];
                    xMax = xMax % scPtr->endUI[1];

                    if(xMax < xMin)
                    {
                        x = xMin;
                        xMin = xMax;
                        xMax = x;
                    }

                    /** Check the data that was saved in this reserved reg to determine in which UI the eye is found */
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, &data));
                    if(xMin < (scPtr->endUI[2] >> 2))
                    {
                        if((xMax - xMin) < scPtr->endUI[0])
                        {
                            data = (((((xMax - xMin) + (scPtr->endUI[0] >> 2)) << 4) | 0x4) & 0x1FFC) | (data & 0xFFFFE003);
                        }
                        else
                        {
                            data = (((((xMin + scPtr->endUI[1]) - xMax) << 4) | 0x4) & 0x1FFC) | (data & 0xFFFFE003);
                        }
                    }
                    else
                    {
                        data = ((((scPtr->endUI[0] >> 2) << 4) | 0x8) & 0x1FFC) | (data & 0xFFFFE003);
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, data));

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    /** use this calib mode only for debug */
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x3, scPtr->resultData[0] & 0xFFFF, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xC, scPtr->resultData[1] & 0xFFFF, scPtr));

                    xMin = scPtr->resultData[0] & 0xFFFF;
                    xMax = xMin;

                    x = scPtr->resultData[1] & 0xFFFF;
                    if(x < xMin)
                    {
                        xMin = x;
                    }
                    if(x > xMax)
                    {
                        xMax = x;
                    }

                    xMin = xMin % scPtr->endUI[1];
                    xMax = xMax % scPtr->endUI[1];

                    if(xMax < xMin)
                    {
                        x = xMin;
                        xMin = xMax;
                        xMax = x;
                    }

                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, &data));
                    if(xMin < (scPtr->endUI[2] >> 2))
                    {
                        if((xMax - xMin) < scPtr->endUI[0])
                        {
                            data = (((((xMax - xMin) + (scPtr->endUI[0] >> 2)) << 4) | 0x4) & 0x1FFC) | (data & 0xFFFFE003);
                        }
                        else
                        {
                            data = (((((xMin + scPtr->endUI[1]) - xMax) << 4) | 0x4) & 0x1FFC) | (data & 0xFFFFE003);
                        }
                    }
                    else
                    {
                        data = ((((scPtr->endUI[0] >> 2) << 4) | 0x8) & 0x1FFC) | (data & 0xFFFFE003);
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, data));

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_WORD:
                    /** use this calib mode only for debug */
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, scPtr->resultData[0] & 0xFFFF, scPtr));

                    xMin = scPtr->resultData[0] & 0xFFFF;

                    xMin = xMin % scPtr->endUI[1];

                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, &data));
                    if(xMin < (scPtr->endUI[2] >> 2))
                    {
                        data = ((((0x000) << 4) | 0x4) & 0x1FFC) | (data & 0xFFFFE003);
                    }
                    else
                    {
                        data = ((((0x000) << 4) | 0x8) & 0x1FFC) | (data & 0xFFFFE003);
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, data));

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_WR_DESKEW:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    min0 = scPtr->sizeX;
                    min1 = min0;
                    min2 = min0;
                    min3 = min0;
                    max0 = 0;
                    max1 = 0;
                    max2 = 0;
                    max3 = 0;

                    for(b = 0; b < SHMOO_G6PHY16_BYTE; b++)
                    {
                        val0 = scPtr->resultData[b] & 0xFFFF;
                        val1 = scPtr->resultData[b + 8] & 0xFFFF;
                        val2 = scPtr->resultData[b + 16] & 0xFFFF;
                        val3 = scPtr->resultData[b + 24] & 0xFFFF;

                        if(min0 > val0)
                        {
                            min0 = val0;
                        }
                        if(min1 > val1)
                        {
                            min1 = val1;
                        }
                        if(min2 > val2)
                        {
                            min2 = val2;
                        }
                        if(min3 > val3)
                        {
                            min3 = val3;
                        }

                        if(max0 < val0)
                        {
                            max0 = val0;
                        }
                        if(max1 < val1)
                        {
                            max1 = val1;
                        }
                        if(max2 < val2)
                        {
                            max2 = val2;
                        }
                        if(max3 < val3)
                        {
                            max3 = val3;
                        }
                    }

                    if((max0 - min0) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val0 = 1;
                        max0 = min0 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val0 = 0;
                    }
                    if((max1 - min1) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val1 = 1;
                        max1 = min1 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val1 = 0;
                    }
                    if((max2 - min2) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val2 = 1;
                        max2 = min2 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val2 = 0;
                    }
                    if((max3 - min3) > SHMOO_G6PHY16_MIN_VDL_LENGTH)
                    {
                        val3 = 1;
                        max3 = min3 + SHMOO_G6PHY16_MIN_VDL_LENGTH - 1;
                    }
                    else
                    {
                        val3 = 0;
                    }

                    if(val0 || val1 || val2 || val3)
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Bit skew beyond available MIN VDL range. Results may not be optimal.\n")));
                    }

                    /** re-align swaps */
                    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get(unit, drc_ndx, &swap));
                    if(swap)
                    {
                        byte0 = 2;
                        byte1 = 3;
                        byte2 = 0;
                        byte3 = 1;
                    }
                    else
                    {
                        byte0 = 0;
                        byte1 = 1;
                        byte2 = 2;
                        byte3 = 3;
                    }

                    initAdjustment = 0;
                    data = 0;
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][0]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][1]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][2]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][3]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][4]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][5]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][6]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));

                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte0][7]] & 0xFFFF) - min0 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][0] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][1] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][2] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][3] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][4] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][5] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][6] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte1][7] + 8] & 0xFFFF) - min1 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][0] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][1] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][2] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][3] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][4] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][5] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][6] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte2][7] + 16] & 0xFFFF) - min2 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT0r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][0] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT1r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][1] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT2r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][2] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT3r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][3] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT4r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][4] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT5r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][5] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT6r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][6] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT7r, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[shmoo_dram_info.dq_swap[drc_ndx][byte3][7] + 24] & 0xFFFF) - min3 + initAdjustment));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
                    
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(min0 - initAdjustment, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(min1 - initAdjustment, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(min2 - initAdjustment, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(min3 - initAdjustment, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, min0 - initAdjustment));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, min1 - initAdjustment));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, min2 - initAdjustment));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, min3 - initAdjustment));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    /** we already have the info from the WR_DESKEW, it is used to calculate the deskew of the DBI bit.
                     * we linearize the VDL position before calculating the VDL shift required to deskew the DBI bit in every byte. */
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &min0));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_UI_SHIFTf, &val0));
                    if(val0)
                    {
                        min0 += (scPtr->endUI[val0 - 1] + 1);
                    }
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &min1));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_UI_SHIFTf, &val1));
                    if(val1)
                    {
                        min1 += (scPtr->endUI[val1 - 1] + 1);
                    }
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &min2));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_UI_SHIFTf, &val2));
                    if(val2)
                    {
                        min2 += (scPtr->endUI[val2 - 1] + 1);
                    }
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &min3));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, data, SOC_PHY_GDDR6_UI_SHIFTf, &val3));
                    if(val3)
                    {
                        min3 += (scPtr->endUI[val3 - 1] + 1);
                    }
                    
                    data = 0;
                    if(min0 > (scPtr->resultData[0] & 0xFFFF))
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, 0));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[0] & 0xFFFF) - min0));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

                    data = 0;
                    if(min1 > (scPtr->resultData[1] & 0xFFFF))
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, 0));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[1] & 0xFFFF) - min1));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

                    data = 0;
                    if(min2 > (scPtr->resultData[2] & 0xFFFF))
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, 0));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[2] & 0xFFFF) - min2));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

                    data = 0;
                    if(min3 > (scPtr->resultData[3] & 0xFFFF))
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, 0));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr, &data, SOC_PHY_GDDR6_MIN_VDL_STEPf, (scPtr->resultData[3] & 0xFFFF) - min3));
                    }
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_WR_SHORT:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BYTE:
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[1] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[2] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[3] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[1] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[2] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[3] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    /** use this calib mode only for debug */
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[1] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[1] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_WORD:
                    /** use this calib mode only for debug */
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_RD_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BYTE:
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x1, scPtr->resultData[0] & 0xFFFF, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x2, scPtr->resultData[1] & 0xFFFF, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x4, scPtr->resultData[2] & 0xFFFF, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x8, scPtr->resultData[3] & 0xFFFF, scPtr));

                    if(!scPtr->restore)
                    {
                        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[0] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, data));

                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[1] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, data));

                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[2] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, data));

                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[3] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    /** use this calib mode only for debug */
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0x3, scPtr->resultData[0] & 0xFFFF, scPtr));
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xC, scPtr->resultData[1] & 0xFFFF, scPtr));
                    if(!scPtr->restore)
                    {
                        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[0] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, data));

                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[1] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_WORD:
                    /** use this calib mode only for debug */
                    _SOC_IF_ERR_EXIT(_g6phy16_move_read_max_vdl_glitch_free(unit, drc_ndx, 0xF, scPtr->resultData[0] & 0xFFFF, scPtr));

                    if(!scPtr->restore)
                    {
                        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, ((scPtr->resultData[0] >> 16) & 0xFFFF) << 1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, data));
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_WR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BYTE:
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));

                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[1] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[2] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[3] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[1] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[2] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[3] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }

                    if(!scPtr->restore)
                    {
                        /** take the info from resultData that is related to the VREF and encode it to MR format */
                        MRfield0 = (scPtr->resultData[0] >> 16) & 0xFFFF;
                        MRfield1 = (scPtr->resultData[1] >> 16) & 0xFFFF;
                        MRfield2 = (scPtr->resultData[2] >> 16) & 0xFFFF;
                        MRfield3 = (scPtr->resultData[3] >> 16) & 0xFFFF;

                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield1));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield2));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield3));

                        /** get the swap info to set the correct physical MR for each channel */
                        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get(unit, drc_ndx, &swap));

                        if(swap)
                        {
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield2 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield3 | 0xF80, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield0 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield1 | 0xF80, 0xFFF));
                        }
                        else
                        {
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield0 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield1 | 0xF80, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield2 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield3 | 0xF80, 0xFFF));
                        }
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[1] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[1] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }

                    if(!scPtr->restore)
                    {
                        MRfield0 = (scPtr->resultData[0] >> 16) & 0xFFFF;
                        MRfield2 = (scPtr->resultData[1] >> 16) & 0xFFFF;
                        /* coverity[copy_paste_error:FALSE] */
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield2));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield2));
                        /* coverity[copy_paste_error:FALSE] */

                        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get(unit, drc_ndx, &swap));

                        if(swap)
                        {
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield2 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield2 | 0xF80, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield0 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield0 | 0xF80, 0xFFF));
                        }
                        else
                        {
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield0 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield0 | 0xF80, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield2 | 0x780, 0xFFF));
                            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield2 | 0xF80, 0xFFF));
                        }
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                case SHMOO_G6PHY16_WORD:
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, &data));
                    if(engageUIshift)
                    {
                        _SOC_IF_ERR_EXIT(_g6phy16_calculate_ui_position(scPtr->resultData[0] & 0xFFFF, scPtr, &up));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, up.ui));
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, up.position));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->resultData[0] & 0xFFFF));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, drc_ndx, data));
                    }

                    if(!scPtr->restore)
                    {
                        MRfield0 = (scPtr->resultData[0] >> 16) & 0xFFFF;

                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));
                        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_MACRO_RESERVED_REGr(unit, drc_ndx, MRfield0));

                        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, MRfield0 | 0x780, 0xFFF));
                        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, MRfield0 | 0xF80, 0xFFF));
                    }

                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                    break;
                default:
                    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"), scPtr->shmooType, calibMode));
            }
            break;
        case SHMOO_G6PHY16_CDR:
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_g6phy16_plot(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr, uint32 plotMode)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 i;
    uint32 ui;
    uint32 iter;
    uint32 shiftAmount;
    uint32 dataMask;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 wckInv;
    uint32 engageUIshift;
    uint32 step1000;
    uint32 size1000UI;
    uint32 calibShiftAmount;
    uint32 maxMidPointX;
    uint32 maxMidPointY;
    char *str0 = NULL;
    char *str1 = NULL;
    char *str2 = NULL;
    int rv = SOC_E_NONE;
    char pass_low[2];
    char fail_high[2];
    char outOfSearch[2];
    
    outOfSearch[0] = ' ';
    outOfSearch[1] = 0;

    sizeX = scPtr->sizeX;
    sizeY = scPtr->sizeY;
    yCapMin = scPtr->yCapMin;
    yCapMax = scPtr->yCapMax;
    calibMode = scPtr->calibMode;
    calibPos = scPtr->calibPos;
    calibStart = scPtr->calibStart;
    wckInv = scPtr->wckInv;
    engageUIshift = scPtr->engageUIshift;
    step1000 = scPtr->step1000;
    size1000UI = scPtr->size1000UI;

    switch(calibPos)
    {
        case SHMOO_G6PHY16_CALIB_ANY_EDGE:
        case SHMOO_G6PHY16_CALIB_RISING_EDGE:
        case SHMOO_G6PHY16_CALIB_FALLING_EDGE:
            pass_low[0] = '_';
            pass_low[1] = 0;
            fail_high[0] = '|';
            fail_high[1] = 0;
            break;
        case SHMOO_G6PHY16_CALIB_CENTER_PASS:
        case SHMOO_G6PHY16_CALIB_PASS_START:
        case SHMOO_G6PHY16_CALIB_FAIL_START:
        case SHMOO_G6PHY16_CALIB_XY_AVERAGE:
            pass_low[0] = '+';
            pass_low[1] = 0;
            fail_high[0] = '-';
            fail_high[1] = 0;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration position: %02d\n"), calibPos));
            return SOC_E_FAIL;
    }

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n\n")));

    switch(plotMode)
    {
        case SHMOO_G6PHY16_BIT:
            if(scPtr->shmooType == SHMOO_G6PHY16_DBI_WR_DESKEW)
            {
                iter = 4;
            }
            else if(scPtr->shmooType == SHMOO_G6PHY16_DBI_EDC_RD_DESKEW)
            {
                iter = 8;
            }
            else
            {
                iter = 32;
            }
            shiftAmount = 0;
            dataMask = 0x1;
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_BYTE:
                    calibShiftAmount = 3;
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    calibShiftAmount = 4;
                    break;
                case SHMOO_G6PHY16_WORD:
                    calibShiftAmount = 5;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02d\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_G6PHY16_BYTE:
            iter = 4;
            shiftAmount = 3;
            dataMask = 0xFF;
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from byte mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_BYTE:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_G6PHY16_WORD:
                    calibShiftAmount = 2;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02d\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_G6PHY16_HALFWORD:
            iter = 2;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_BYTE:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from halfword mode to byte mode\n")));
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_WORD:
                    calibShiftAmount = 1;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02d\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_G6PHY16_WORD:
            iter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to bit mode\n")));
                    iter = 32;
                    shiftAmount = 0;
                    dataMask = 0x1;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_BYTE:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to byte mode\n")));
                    iter = 4;
                    shiftAmount = 3;
                    dataMask = 0xFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Plot mode coerced from word mode to halfword mode\n")));
                    iter = 2;
                    shiftAmount = 4;
                    dataMask = 0xFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_G6PHY16_WORD:
                    calibShiftAmount = 0;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02d\n"), calibMode));
                    return SOC_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02d\n"), plotMode));
            return SOC_E_FAIL;
    }
/*    
    if(engageUIshift)
    { */
    str0 = sal_alloc(sizeof(char) * SHMOO_G6PHY16_STRING_LENGTH, "shmoo g6phy16 str0");
    if (NULL == str0) {
        return SOC_E_MEMORY;
    }
    str1 = sal_alloc(sizeof(char) * SHMOO_G6PHY16_STRING_LENGTH, "shmoo g6phy16 str1");
    if (NULL == str1) {
        sal_free(str0);
        return SOC_E_MEMORY;
    }
    str2 = sal_alloc(sizeof(char) * SHMOO_G6PHY16_STRING_LENGTH, "shmoo g6phy16 str2");
    if (NULL == str2) {
        sal_free(str0);
        sal_free(str1);
        return SOC_E_MEMORY;
    }

    sal_memset(str0, 0, sizeof(char) * SHMOO_G6PHY16_STRING_LENGTH);
    sal_memset(str1, 0, sizeof(char) * SHMOO_G6PHY16_STRING_LENGTH);
    sal_memset(str2, 0, sizeof(char) * SHMOO_G6PHY16_STRING_LENGTH);

    {
        ui = 0;
        
        for(x = 0; x < sizeX; x++)
        {
            if((ui < SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT) && (x > scPtr->endUI[ui]))
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
        maxMidPointX = scPtr->resultData[i >> calibShiftAmount] & 0xFFFF;
        maxMidPointY = (scPtr->resultData[i >> calibShiftAmount] >> 16) & 0xFFFF;
        
        if((sizeY > 1) || (i == 0))
        {
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "***** Interface.......: %3d\n"), drc_ndx));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** VDL step size...: %3d.%03d ps\n"), (step1000 / 1000), (step1000 % 1000)));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** UI size.........: %3d.%03d steps\n"), (size1000UI / 1000), (size1000UI % 1000)));
            
            switch(scPtr->shmooType)
            {
                case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: ADDR_CTRL_SHORT\n")));
                    break;
                case SHMOO_G6PHY16_WCK2CK:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WCK2CK\n")));
                    if((wckInv >> 5) & 0x1)
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK0 invert.....: On (Post-inversion)\n")));
                    }
                    else
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK0 invert.....: Off\n")));
                    }
                    if((wckInv >> 7) & 0x1)
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK1 invert.....: On (Post-inversion)\n")));
                    }
                    else
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK1 invert.....: Off\n")));
                    }
                    if((wckInv >> 21) & 0x1)
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK2 invert.....: On (Post-inversion)\n")));
                    }
                    else
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK2 invert.....: Off\n")));
                    }
                    if((wckInv >> 23) & 0x1)
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK3 invert.....: On (Post-inversion)\n")));
                    }
                    else
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** WCK3 invert.....: Off\n")));
                    }
                    break;
                case SHMOO_G6PHY16_RD_DESKEW:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_DESKEW\n")));
                    break;
                case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: DBI_EDC_RD_DESKEW\n")));
                    break;
                case SHMOO_G6PHY16_RD_SHORT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_SHORT\n")));
                    break;
                case SHMOO_G6PHY16_WR_DESKEW:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_DESKEW\n")));
                    break;
                case SHMOO_G6PHY16_DBI_WR_DESKEW:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: DBI_WR_DESKEW\n")));
                    break;
                case SHMOO_G6PHY16_WR_SHORT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_SHORT\n")));
                    break;
                case SHMOO_G6PHY16_RD_EXTENDED:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: RD_EXTENDED\n")));
                    break;
                case SHMOO_G6PHY16_WR_EXTENDED:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, " **** Shmoo type......: WR_EXTENDED\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
                    rv = SOC_E_FAIL;
                    goto cleanup_g6phy16;
            }
            
            if(engageUIshift)
            {
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** UI shift........: On\n")));
            }
            else
            {
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** UI shift........: Off or N/A\n")));
            }
        }
        
        if(sizeY > 1)
        {
            switch(calibMode)
            {
                case SHMOO_G6PHY16_BIT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Bit-wise\n")));
                    break;
                case SHMOO_G6PHY16_BYTE:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Byte-wise\n")));
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Halfword-wise\n")));
                    break;
                case SHMOO_G6PHY16_WORD:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 2D Word-wise\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02d\n"), calibMode));
                    rv = SOC_E_FAIL;
                    goto cleanup_g6phy16;
            }
            
            switch(plotMode)
            {
                case SHMOO_G6PHY16_BIT:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Bit-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Bit.............: %03d\n"), i));
                    break;
                case SHMOO_G6PHY16_BYTE:
                    LOG_INFO(BSL_LS_SOC_DDR,(BSL_META_U(unit, "  *** Plot mode.......: 2D Byte-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,(BSL_META_U(unit, "   ** Byte............: %03d\n"), i));
                    break;
                case SHMOO_G6PHY16_HALFWORD:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Halfword-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Halfword........: %03d\n"), i));
                    break;
                case SHMOO_G6PHY16_WORD:
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 2D Word-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "   ** Word............: %03d\n"), i));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02d\n"), plotMode));
                    rv = SOC_E_FAIL;
                    goto cleanup_g6phy16;
            }

            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "    * Center X........: %03d\n"), maxMidPointX));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "    * Center Y........: %03d\n"), maxMidPointY));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str0));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str1));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str2));

            for(y = yCapMin; y < yCapMax; y++)
            {
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  %03d "), y));
                
                for(x = 0; x < calibStart; x++)
                {
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), outOfSearch));
                }
                
                for(x = calibStart; x < sizeX; x++)
                {
                    if((scPtr->result2D[xStart + x] >> (i << shiftAmount)) & dataMask)
                    {   /* FAIL - RISING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR FAIL */
                            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                        }
                        else
                        {   /* FAIL - RISING EDGE */
                            if((calibPos == SHMOO_G6PHY16_CALIB_ANY_EDGE) || (calibPos == SHMOO_G6PHY16_CALIB_RISING_EDGE) || (calibPos == SHMOO_G6PHY16_CALIB_FAIL_START))
                            {   /* RISING EDGE */
                                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                            }
                            else
                            {   /* FAIL */
                                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                            }
                        }
                    }
                    else
                    {   /* PASS - MIDPOINT - FALLING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR PASS */
                            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), pass_low));
                        }
                        else
                        {   /* POTENTIAL MIDPOINT - FALLING EDGE */
                            if(y == maxMidPointY)
                            {   /* MID POINT - FALLING EDGE */
                                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                            }
                            else
                            {   /* PASS */
                                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), pass_low));
                            }
                        }
                    }
                }
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
                xStart += sizeX;
            }
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        }
        else
        {
            if(i == 0)
            {
                switch(calibMode)
                {
                    case SHMOO_G6PHY16_BIT:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_G6PHY16_BYTE:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_G6PHY16_HALFWORD:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_G6PHY16_WORD:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Calib mode......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported calibration mode during plot: %02d\n"),
                                   calibMode));
                        rv = SOC_E_FAIL;
                        goto cleanup_g6phy16;
                }
                
                switch(plotMode)
                {
                    case SHMOO_G6PHY16_BIT:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_G6PHY16_BYTE:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_G6PHY16_HALFWORD:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_G6PHY16_WORD:
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  *** Plot mode.......: 1D Word-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported plot mode: %02d\n"), plotMode));
                        rv = SOC_E_FAIL;
                        goto cleanup_g6phy16;
                }
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str0));
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str1));
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "      %s\n"), str2));
            }
            
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "  %03d "), i));
            
            for(x = 0; x < calibStart; x++)
            {
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), outOfSearch));
            }
            
            for(x = calibStart; x < sizeX; x++)
            {
                if((scPtr->result2D[x] >> (i << shiftAmount)) & dataMask)
                {   /* FAIL - RISING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR FAIL */
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                    }
                    else
                    {   /* FAIL - RISING EDGE */
                        if((calibPos == SHMOO_G6PHY16_CALIB_ANY_EDGE) || (calibPos == SHMOO_G6PHY16_CALIB_RISING_EDGE) || (calibPos == SHMOO_G6PHY16_CALIB_FAIL_START))
                        {   /* RISING EDGE */
                            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                        }
                        else
                        {   /* FAIL */
                            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), fail_high));
                        }
                    }
                }
                else
                {   /* PASS - MIDPOINT - FALLING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR PASS */
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "%s"), pass_low));
                    }
                    else
                    {   /* MID POINT - FALLING EDGE */
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "X")));
                    }
                }
            }
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        }
    }
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));

cleanup_g6phy16:
    if (str0 != NULL) {
        sal_free(str0);
    }
    if (str1 != NULL) {
        sal_free(str1);
    }
    if (str2 != NULL) {
        sal_free(str2);
    }
    return rv;
}

STATIC int
_shmoo_g6phy16_plot(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    SOC_INIT_FUNC_DEFS;
    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_HALFWORD));
            break;
        case SHMOO_G6PHY16_WCK2CK:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE));
            break;
        case SHMOO_G6PHY16_RD_START_FISH:
            break;
        case SHMOO_G6PHY16_EDC_START_FISH:
            break;
        case SHMOO_G6PHY16_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            break;
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            break;
        case SHMOO_G6PHY16_RD_SHORT:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            break;
        case SHMOO_G6PHY16_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            break;
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            break;
        case SHMOO_G6PHY16_WR_SHORT:
            _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            break;
        case SHMOO_G6PHY16_RD_EXTENDED:
            if(scPtr->restore)
            {
                _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            }
            else
            {
                _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE));
            }
            break;
        case SHMOO_G6PHY16_WR_EXTENDED:
            if(scPtr->restore)
            {
                _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BIT));
            }
            else
            {
                _SOC_IF_ERR_EXIT(_g6phy16_plot(unit, drc_ndx, scPtr, SHMOO_G6PHY16_BYTE));
            }
            break;
        case SHMOO_G6PHY16_CDR:
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }

exit:
    SOC_FUNC_RETURN;
}

int
soc_g6phy16_calculate_step_size(int unit, int drc_ndx, g6phy16_step_size_t *ssPtr)
{
    uint32 data;
    uint32 obs_interval, ro_vdl_step1, ro_vdl_step2, ucount1, ucount2;
    uint32 reset_ctrl_data;
    uint32 fld_dcount;
    uint32 fld_ro_overflow;
    soc_timeout_t to;
    sal_usecs_t to_val;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    if(shmoo_dram_info.sim_system_mode)
    {
        ssPtr->step1000 = 1953;
        ssPtr->size1000UI = 128000;
        SOC_EXIT;
    }

    data = 0;
    obs_interval = 2000;
    ro_vdl_step1 = 64;
    ro_vdl_step2 = 128;
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_OBS_INTERVALf, obs_interval));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_SEL_FC_REFCLKf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP); 
    
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, drc_ndx, &reset_ctrl_data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_FC_RESET_Nf, 0));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_RO_RESET_Nf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, drc_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP); 
    
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_START_OBSf, 0));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_EN_NLDL_CLKOUT_BARf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
    
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_RO_VDL_STEPf, ro_vdl_step1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP); 
    
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_EN_NLDL_CLKOUT_BARf, 1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
    
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_FC_RESET_Nf, 1));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_RO_RESET_Nf, 1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, drc_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
    
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_START_OBSf, 1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
    
    if (SAL_BOOT_QUICKTURN)
    {
        to_val = 10000000;  /* 10 Sec */
    }
    else
    {
        to_val = 50000;     /* 50 mS */
    }
    
    soc_timeout_init(&to, to_val, 0);
    do
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_DCOUNTr(unit, drc_ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_DCOUNTr, data, SOC_PHY_GDDR6_DCOUNTf, &fld_dcount));
        if(!fld_dcount)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr, data, SOC_PHY_GDDR6_RO_OVERFLOWf, &fld_ro_overflow));
            if( fld_ro_overflow & 0x4)
            {
                _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "VDL step size computation rollover during first pass\n")));
            }
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr, data, SOC_PHY_GDDR6_RO_UCOUNTf, &ucount1));
            break;
        }
        if (soc_timeout_check(&to))
        {
            _SOC_EXIT_WITH_ERR(SOC_E_TIMEOUT, (BSL_META_U(unit, "Timed out waiting for first pass of VDL step size computation\n")));
        }
    }
    while(TRUE);

    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_FC_RESET_Nf, 0));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_RO_RESET_Nf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, drc_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_START_OBSf, 0));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_EN_NLDL_CLKOUT_BARf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_RO_VDL_STEPf, ro_vdl_step2));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP); 

    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_EN_NLDL_CLKOUT_BARf, 1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_FC_RESET_Nf, 1));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_RO_RESET_Nf, 1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, drc_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_START_OBSf, 1));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    soc_timeout_init(&to, to_val, 0);
    do
    {
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_DCOUNTr(unit, drc_ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_DCOUNTr, data, SOC_PHY_GDDR6_DCOUNTf, &fld_dcount));
        if(!fld_dcount)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr, data, SOC_PHY_GDDR6_RO_OVERFLOWf, &fld_ro_overflow));
            if( fld_ro_overflow & 0x4)
            {
                _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "VDL step size computation rollover during second pass\n")));
            }
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_STATUS_FREQ_CNTR_UCOUNTr, data, SOC_PHY_GDDR6_RO_UCOUNTf, &ucount2));
            break;
        }
        if (soc_timeout_check(&to))
        {
            _SOC_EXIT_WITH_ERR(SOC_E_TIMEOUT, (BSL_META_U(unit, "Timed out waiting for second pass of VDL step size computation\n")));
        }
    }
    while(TRUE);

    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_FC_RESET_Nf, 0));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_FREQ_CNTR_RO_RESET_Nf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, drc_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, &data));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_START_OBSf, 0));
    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr, &data, SOC_PHY_GDDR6_EN_NLDL_CLKOUT_BARf, 0));
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_FREQ_CNTR_CONFIGr(unit, drc_ndx, data));
    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

    ssPtr->step1000 = ((((((1000000000 / (ro_vdl_step2 - ro_vdl_step1)) << 4) / shmoo_dram_info.data_rate_mbps) * obs_interval) / ucount2) * (ucount1 - ucount2)) / ucount1;
    ssPtr->size1000UI = ((1000000000 / shmoo_dram_info.data_rate_mbps) * 1000) / (ssPtr->step1000);

exit:
    SOC_FUNC_RETURN;
}

static int
_shmoo_g6phy16_entry(
    int unit,
    int drc_ndx,
    g6phy16_shmoo_container_t *scPtr)
{
    uint32 data;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    scPtr->calibStart = 0;

    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
        {
            data = 0;                   /* ADDR/CTRL should be in a working position */                         /* No adjustment */
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_UI_SHIFTf, SHMOO_G6PHY16_GDDR6_INIT_ADDR_CTRL_UI_SHIFT));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, 0x92));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, data));

            /** writing 0.7VDDO per byte */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, 0x2A | 0x780, 0xFFF));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, 0x2A | 0xF80, 0xFFF));

            /** override CA termination value */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 8, 0x10, 0x010));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            /** re-init to the relevant place after changing MR_6 */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_init(unit, drc_ndx, SHMOO_GDDR6_DRAM_INIT_UNTIL_CADT));
            break;
        }
        case SHMOO_G6PHY16_WCK2CK:
        {
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 10, 0x000, 0x0F0));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 1, 0x005, 0x00F));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 9, 0x5 | 0x780, 0xFFF));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 9, 0x5 | 0xF80, 0xFFF));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_force_dqs(unit, drc_ndx , 1));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            /* scPtr->sizeX = scPtr->endUI[4] + 20; */
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, drc_ndx, &data));
            /* _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, scPtr->sizeX - 1)); */
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, 0));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_WCKr(unit, drc_ndx, data));

            /** SDK179348 - get rid of this CB, it is not needed, it is done directly using the MR change CB, just add comment about the MR written */
            /* _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wck2ck_training(unit, drc_ndx, 1)); */
            /* sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP); */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 10, 0x100, 0x100));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_force_dqs(unit, drc_ndx , 0));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            data = 0;
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_LEVELING_CONFIGr, &data, SOC_PHY_GDDR6_WRITE_LEVELING_MODEf, 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        }
        case SHMOO_G6PHY16_RD_START_FISH:
        {
            data = 0;
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_RDATA_WR2RD_DELAYf, 4));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, drc_ndx, data));

            data = 0;
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_SYSTEM_ENABLEf, 0));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_GEN_ENABLEf, 0));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_FSM_CTRLr(unit, drc_ndx, data));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_GEN_ENABLEf, 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_FSM_CTRLr(unit, drc_ndx, data));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_SYSTEM_ENABLEf, 1));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_FSM_CTRLr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_FSM_CTRLr(unit, drc_ndx, data));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        }
        case SHMOO_G6PHY16_EDC_START_FISH:
        {
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_crc(unit, drc_ndx, 1));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_crc(unit, drc_ndx, 1));
            break;
        }
        case SHMOO_G6PHY16_RD_DESKEW:
        {
            data = 0;
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, &data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESERVED_REGr(unit, drc_ndx, data & 0xFFFFFFFC));
            break;
        }
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
        {
            data = 0;
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_crc(unit, drc_ndx, 1));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_crc(unit, drc_ndx, 1));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_dbi(unit, drc_ndx, 1));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_dbi(unit, drc_ndx, 1));
            break;
        }
        case SHMOO_G6PHY16_WR_DESKEW:
        {
            data = 0;
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            break;
        }
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
        {
            data = 0;
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_dbi(unit, drc_ndx, 1));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_dbi(unit, drc_ndx, 1));
            break;
        }
        case SHMOO_G6PHY16_RD_SHORT:
        case SHMOO_G6PHY16_WR_SHORT:
        case SHMOO_G6PHY16_RD_EXTENDED:
        case SHMOO_G6PHY16_WR_EXTENDED:
        case SHMOO_G6PHY16_CDR:
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_exit(int unit, int drc_ndx, g6phy16_shmoo_container_t *scPtr)
{
    uint32 data;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    switch(scPtr->shmooType)
    {
        case SHMOO_G6PHY16_ADDR_CTRL_SHORT:
        {
            /** complete dram init after finishing the step */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_init(unit, drc_ndx, SHMOO_GDDR6_DRAM_INIT_AFTER_CADT));

            /** writing 0.7VDDO per byte */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, 0x2A | 0x780, 0xFFF));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 6, 0x2A | 0xF80, 0xFFF));

            /** override CA termination value */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 8, 0x10, 0x010));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        }
        case SHMOO_G6PHY16_WCK2CK:
        {
            g6phy16_vendor_info_t shmoo_vendor_info;
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_vendor_info_get(unit, drc_ndx, &shmoo_vendor_info));
            if ((shmoo_vendor_info.manufacture_id == SHMOO_G6PHY16_VENDOR_MICRON) && (shmoo_dram_info.data_rate_mbps > 7000))
            {
                /** this should happen only to devices where the PLL is on (basically Micron with data frequency above 7G) */
                sal_usleep(SHMOO_G6PHY16_DEEP_SLEEP);
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 1, 0x80, 0x80));
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 1, 0x00, 0x80));
            }

            sal_usleep(SHMOO_G6PHY16_DEEP_SLEEP);
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 10, 0x000, 0x100));

            data = 0;
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_LEVELING_CONFIGr, &data, SOC_PHY_GDDR6_WRITE_LEVELING_MODEf, 0));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_LEVELING_CONFIGr(unit, drc_ndx, data));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        }
        case SHMOO_G6PHY16_RD_START_FISH:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_soft_reset_drc_without_dram(unit, drc_ndx));
            break;
        case SHMOO_G6PHY16_EDC_START_FISH:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_crc(unit, drc_ndx, 0));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_crc(unit, drc_ndx, 0));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        case SHMOO_G6PHY16_RD_DESKEW:
            break;
        case SHMOO_G6PHY16_DBI_EDC_RD_DESKEW:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_crc(unit, drc_ndx, 0));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_crc(unit, drc_ndx, 0));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_dbi(unit, drc_ndx, 0));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_dbi(unit, drc_ndx, 0));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        case SHMOO_G6PHY16_RD_SHORT:
            break;
        case SHMOO_G6PHY16_WR_DESKEW:
            break;
        case SHMOO_G6PHY16_DBI_WR_DESKEW:
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_dbi(unit, drc_ndx, 0));
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_dbi(unit, drc_ndx, 0));
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            break;
        case SHMOO_G6PHY16_WR_SHORT:
            break;
        case SHMOO_G6PHY16_RD_EXTENDED:
            break;
        case SHMOO_G6PHY16_WR_EXTENDED:
            break;
        case SHMOO_G6PHY16_CDR:
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), scPtr->shmooType));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_shmoo_g6phy16_save(int unit, int drc_ndx, g6phy16_shmoo_config_param_t *config_param)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    /** SHMOO_G6PHY16_ADDR_CTRL_SHORT */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, &data));
    config_param->aq_l_max_vdl_addr = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, drc_ndx, &data));
    config_param->aq_l_max_vdl_ctrl = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, drc_ndx, &data));
    config_param->aq_u_max_vdl_addr = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, drc_ndx, &data));
    config_param->aq_u_max_vdl_ctrl = data;

    /** SHMOO_G6PHY16_RD_DESKEW */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[0][7] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[1][7] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[2][7] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_bit[3][7] = data;

    /** SHMOO_G6PHY16_DBI_EDC_RD_DESKEW */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_dbi[0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_edc[0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_dbi[1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_edc[1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_dbi[2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_edc[2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_dbi[3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, drc_ndx, &data));
    config_param->dq_byte_rd_min_vdl_edc[3] = data;

    /** SHMOO_G6PHY16_WR_DESKEW */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[0][7] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[1][7] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[2][7] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][3] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][4] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][5] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][6] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_bit[3][7] = data;

    /** SHMOO_G6PHY16_DBI_WR_DESKEW */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_dbi[0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_dbi[1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_dbi[2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, &data));
    config_param->dq_byte_wr_min_vdl_dbi[3] = data;

    /** SHMOO_G6PHY16_RD_EXTENDED */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
    config_param->dq_byte_vref_dac_config[0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
    config_param->dq_byte_vref_dac_config[1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
    config_param->dq_byte_vref_dac_config[2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, &data));
    config_param->dq_byte_vref_dac_config[3] = data;

    /** SHMOO_G6PHY16_WR_EXTENDED */
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_MACRO_RESERVED_REGr(unit, drc_ndx, &data));
    config_param->dq_byte_macro_reserved_reg[0] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_MACRO_RESERVED_REGr(unit, drc_ndx, &data));
    config_param->dq_byte_macro_reserved_reg[1] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_MACRO_RESERVED_REGr(unit, drc_ndx, &data));
    config_param->dq_byte_macro_reserved_reg[2] = data;
    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_MACRO_RESERVED_REGr(unit, drc_ndx, &data));
    config_param->dq_byte_macro_reserved_reg[3] = data;

exit:
    SOC_FUNC_RETURN;
}

int
shmoo_g6phy16_restore(int unit, int drc_ndx, g6phy16_shmoo_config_param_t *config_param)
{
    uint32 data;
    uint32 MRfield0, MRfield1, MRfield2, MRfield3;
    int swap;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    /** SHMOO_G6PHY16_ADDR_CTRL_SHORT */
    data = config_param->aq_l_max_vdl_addr;
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, drc_ndx, data));
    data = config_param->aq_l_max_vdl_ctrl;
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, drc_ndx, data));
    data = config_param->aq_u_max_vdl_addr;
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, drc_ndx, data));
    data = config_param->aq_u_max_vdl_ctrl;
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, drc_ndx, data));

    /** SHMOO_G6PHY16_RD_DESKEW */
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[0][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[1][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[2][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_bit[3][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, drc_ndx, data));

    /** SHMOO_G6PHY16_DBI_EDC_RD_DESKEW */
    data = (uint32) config_param->dq_byte_rd_min_vdl_dbi[0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_edc[0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_dbi[1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_edc[1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_dbi[2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_edc[2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_dbi[3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_rd_min_vdl_edc[3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, drc_ndx, data));

    /** SHMOO_G6PHY16_WR_DESKEW */
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[0][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[1][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[2][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][4];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][5];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][6];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_bit[3][7];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, drc_ndx, data));

    /** SHMOO_G6PHY16_DBI_WR_DESKEW */
    data = (uint32) config_param->dq_byte_wr_min_vdl_dbi[0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_dbi[1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_dbi[2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));
    data = (uint32) config_param->dq_byte_wr_min_vdl_dbi[3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr(unit, drc_ndx, data));

    /** SHMOO_G6PHY16_RD_EXTENDED */
    data = config_param->dq_byte_vref_dac_config[0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, drc_ndx, data));
    data = config_param->dq_byte_vref_dac_config[1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, drc_ndx, data));
    data = config_param->dq_byte_vref_dac_config[2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, drc_ndx, data));
    data = config_param->dq_byte_vref_dac_config[3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, drc_ndx, data));


    /** SHMOO_G6PHY16_WR_EXTENDED */
    data = config_param->dq_byte_macro_reserved_reg[0];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_MACRO_RESERVED_REGr(unit, drc_ndx, data));
    data = config_param->dq_byte_macro_reserved_reg[1];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_MACRO_RESERVED_REGr(unit, drc_ndx, data));
    data = config_param->dq_byte_macro_reserved_reg[2];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_MACRO_RESERVED_REGr(unit, drc_ndx, data));
    data = config_param->dq_byte_macro_reserved_reg[3];
    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_MACRO_RESERVED_REGr(unit, drc_ndx, data));

    /** this also changes the wr/rd crc/dbi in the MR back to thier defaults, so need to disable them again afterwards */
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_init(unit, drc_ndx, SHMOO_GDDR6_DRAM_INIT_ALL));

    /** disable wr/rd crc/dbi to align back to controler's expectations */
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_crc(unit, drc_ndx, 0));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_crc(unit, drc_ndx, 0));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_wr_dbi(unit, drc_ndx, 0));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_enable_rd_dbi(unit, drc_ndx, 0));

    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 8, 0x10, 0x010));

    MRfield0 = config_param->dq_byte_macro_reserved_reg[0];
    MRfield1 = config_param->dq_byte_macro_reserved_reg[1];
    MRfield2 = config_param->dq_byte_macro_reserved_reg[2];
    MRfield3 = config_param->dq_byte_macro_reserved_reg[3];

    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get(unit, drc_ndx, &swap));
    if(swap)
    {
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield2 | 0x780, 0xFFF));
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield3 | 0xF80, 0xFFF));
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield0 | 0x780, 0xFFF));
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield1 | 0xF80, 0xFFF));
    }
    else
    {
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield0 | 0x780, 0xFFF));
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 0, 6, MRfield1 | 0xF80, 0xFFF));
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield2 | 0x780, 0xFFF));
        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, drc_ndx, 1, 6, MRfield3 | 0xF80, 0xFFF));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      soc_g6phy16_shmoo_ctl
 * Purpose:
 *      Perform shmoo (PHY calibration) on specific DRC index.
 * Parameters:
 *      unit                - unit number
 *      dram_index          - dram index to perform shmoo on.
 *      shmoo_type          - Selects shmoo sub-section to be performs (SHMOO_G6PHY16_SHMOO_ALL for full shmoo)
 *      flags               -   SHMOO_G6PHY16_CTL_FLAGS_STAT_BIT            
 *                                  0: Runs normal shmoo functions
 *                                  1: Doesn't run shmoo. Only prints statistics. (Nothing is printed at the moment)
 *                              SHMOO_G6PHY16_CTL_FLAGS_PLOT_BIT            
 *                                  0: No shmoo plots are printed during shmoo
 *                                  1: Shmoo plots are printed during shmoo
 *                              SHMOO_G6PHY16_CTL_FLAGS_EXT_VREF_RANGE_BIT
 *                                  0: Shmoo runs with a regular range for Vref sweep in *EXTENDED shmoos
 *                                  1: Shmoo runs with an extended range for Vref sweep in *EXTENDED shmoos
 *      action              - Save/restore functionality
 *      *config_param       - PHY configuration saved/restored
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 */

int
soc_g6phy16_shmoo_ctl(int unit, int dram_index, shmoo_g6phy16_shmoo_type_t shmoo_type, uint32 flags, int action, g6phy16_shmoo_config_param_t *config_param)
{
    g6phy16_shmoo_container_t *scPtr = NULL;
    uint32 ctlType;
    uint32 seq_step_iter;
    int dram_iter;
    uint32 plot;
    uint32 dram_bitmap[1] = {0};
    const shmoo_g6phy16_shmoo_type_t *seqPtr;
    uint32 seqCount;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    /** Validate dram info configuration */
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_validate_config(unit));

    ctlType = shmoo_dram_info.ctl_type;
    plot = flags & SHMOO_G6PHY16_CTL_FLAGS_PLOT_BIT;

    /** Allocate SHMOO container and init - has to be as allocation, structure is very big */
    scPtr = sal_alloc(sizeof(g6phy16_shmoo_container_t), "Combo16 SHMOO Container");
    if(scPtr == NULL)
    {
        return SOC_E_MEMORY;
    }
    sal_memset(scPtr, 0, sizeof(g6phy16_shmoo_container_t));
    scPtr->debugMode = flags & SHMOO_G6PHY16_DEBUG_MODE_FLAG_BITS;
    scPtr->dramType = shmoo_dram_info.dram_type;
    scPtr->ctlType = ctlType;

    /** determine which DRAMs to operate on, either single or all */
    if (dram_index == SHMOO_G6PHY16_INTERFACE_ALL)
    {
        SHR_BITCOPY_RANGE(dram_bitmap, 0, &shmoo_dram_info.dram_bitmap, 0, SHMOO_G6PHY16_MAX_INTERFACES);
    }
    else
    {
        SHR_BITSET(dram_bitmap, dram_index);
    }

    /** iterate over DRAMs and run tune */
    SHR_BIT_ITER(dram_bitmap, SHMOO_G6PHY16_MAX_INTERFACES, dram_iter)
    {
        if(action == SHMOO_G6PHY16_ACTION_RESTORE)
        {
            /** restore tune parameters and choose restore sequence */
            scPtr->restore = 1;
            _SOC_IF_ERR_EXIT(shmoo_g6phy16_restore(unit, dram_iter, config_param));
            seqPtr = shmoo_order_g6phy16_gddr6_restore;
            seqCount = sizeof(shmoo_order_g6phy16_gddr6_restore) / sizeof(shmoo_order_g6phy16_gddr6_restore[0]);
        }
        else
        {
            /** choose regular tune sequence */
            seqPtr = shmoo_order_g6phy16_gddr6;
            seqCount = sizeof(shmoo_order_g6phy16_gddr6) / sizeof(shmoo_order_g6phy16_gddr6[0]);
        }
        
        if((action == SHMOO_G6PHY16_ACTION_RUN) || (action == SHMOO_G6PHY16_ACTION_RUN_AND_SAVE) || (action == SHMOO_G6PHY16_ACTION_RESTORE))
        {
            g6phy16_step_size_t ss;
            int end_ui_index;
            int cdr_is_enabled = 0;

            /** if a specific SHMOO type was chosen, adjust the sequence array to have only that entry */
            if(shmoo_type != SHMOO_G6PHY16_SHMOO_ALL)
            {
                seqPtr = &shmoo_type;
                seqCount = 1;
            }

            /** calculate step size and UI size, init endUI array */
            _SOC_IF_ERR_EXIT(soc_g6phy16_calculate_step_size(unit, dram_iter, &ss));
            scPtr->step1000 = ss.step1000;
            scPtr->size1000UI = ss.size1000UI;
            for(end_ui_index = 0; end_ui_index < SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT; end_ui_index++)
            {
                scPtr->endUI[end_ui_index] = ((end_ui_index + 1) * (ss.size1000UI)) / 1000;
            }

            /** check if CdR is enabled, disable it in that case */
            _SOC_IF_ERR_EXIT(soc_g6phy16_cdr_is_enabled(unit, dram_iter, &cdr_is_enabled));
            if(cdr_is_enabled)
            {
                /** disable CDR */
                _SOC_IF_ERR_EXIT(soc_g6phy16_cdr_disable(unit, dram_iter));
            }

            /** iterate over sequence array and perform tuning - each step in sequence array has its corresponding action in the functions below */
            for(seq_step_iter = 0; seq_step_iter < seqCount; seq_step_iter++)
            {
                scPtr->shmooType = seqPtr[seq_step_iter];
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_entry(unit, dram_iter, scPtr));
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_do(unit, dram_iter, scPtr));
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_calib_2D(unit, dram_iter, scPtr));
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_set_new_step(unit, dram_iter, scPtr));
                if(plot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO))
                {
                    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_plot(unit, dram_iter, scPtr));
                }
                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_exit(unit, dram_iter, scPtr));
            }
        }

        if((action == SHMOO_G6PHY16_ACTION_RUN_AND_SAVE) || (action == SHMOO_G6PHY16_ACTION_SAVE))
        {
            /** save tune parameters */
            _SOC_IF_ERR_EXIT(_shmoo_g6phy16_save(unit, dram_iter, config_param));
        }
    }

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DDR Tuning Complete\n")));

exit:
    SOC_FREE(scPtr);
    SOC_FUNC_RETURN;
}

/* Configure PHY PLL and wait for lock */
int
soc_g6phy16_shmoo_phy_cfg_pll(int unit, int drc_ndx)
{
    int ndx, ndxEnd;
    int fref, fref_post, fref_eff_int, fref_eff_frac, freq_vco, data_rate;
    int pdiv10, ndiv_int10;
    uint32 frequency_doubler_mode;
    uint32 ref_clk_bitmap;
    g6phy16_drc_pll_t pll_info;
    int orig_ndx = -1;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_validate_cbi(unit));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_validate_config(unit));

    data_rate = shmoo_dram_info.data_rate_mbps;
    fref = shmoo_dram_info.ref_clk_mhz;
    ref_clk_bitmap = shmoo_dram_info.ref_clk_bitmap;
    
    pll_info.iso_in = 0;
    pll_info.ldo_ctrl = 0x0000;
    pll_info.ssc_limit = 0x000000;
    pll_info.ssc_mode = 0;
    pll_info.ssc_step = 0x0000;
    pll_info.pll_ctrl = 0x00000000;
    
    fref_eff_int = 50;
    fref_eff_frac = 000;
    freq_vco = data_rate;
    
    frequency_doubler_mode = 0;                                                     /* Frequency doubler OFF */
    pll_info.en_ctrl = frequency_doubler_mode;
    
    fref_post = fref << frequency_doubler_mode;
    pdiv10 = (10000 * fref_post) / ((1000 * fref_eff_int) + fref_eff_frac);
    
    if((pdiv10 % 10) >= 5)
    {
        pll_info.pdiv = (pdiv10 / 10) + 1;
    }
    else
    {
        pll_info.pdiv = (pdiv10 / 10);
    }
    
    ndiv_int10 = (10000 * freq_vco) / ((1000 * fref_eff_int) + fref_eff_frac);
    
    if((ndiv_int10 % 10) >= 5)
    {
        pll_info.ndiv_int = (ndiv_int10 / 10) + 1;
    }
    else
    {
        pll_info.ndiv_int = (ndiv_int10 / 10);
    }
    
    pll_info.ndiv_frac = 0;                                                         /* ndiv fraction OFF */
    pll_info.mdiv = 1;
    pll_info.fref_eff_info = fref_eff_int;
    
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "PHY PLL Configuration\n")));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Fref...........: %4d\n"), fref));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "PDIV...........: %4d\n"), pll_info.pdiv));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "NDIV...........: %4d\n"), pll_info.ndiv_int));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Fvco...........: %4d\n"), freq_vco));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Data rate......: %4d\n"), data_rate));
    
    if(freq_vco > 5500)
    {
        pll_info.vco_sel = 1;
    }
    else
    {
        pll_info.vco_sel = 0;
    }

    if(drc_ndx != SHMOO_G6PHY16_INTERFACE_ALL)
    {
        ndx = drc_ndx;
        ndxEnd = drc_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_G6PHY16_MAX_INTERFACES;
    }

    /* MASTER PHYS -- Reference clock: Differential */
    orig_ndx = ndx;
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_g6phy16_check_dram(unit, ndx))
        {
            continue;
        }

        if(!(ref_clk_bitmap & (0x1 << ndx)))
        {
            continue;
        }

        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_pll_set(unit, ndx, &pll_info));
    }

    /* SLAVE PHYS -- Reference clock: Single-ended */
    ndx = orig_ndx;
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_g6phy16_check_dram(unit, ndx))
        {
            continue;
        }

        if(ref_clk_bitmap & (0x1 << ndx))
        {
            continue;
        }

        _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_pll_set(unit, ndx, &pll_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int
soc_g6phy16_shmoo_phy_init(int unit, int drc_ndx)
{
    int ndx, ndxEnd;
    uint32 data, reset_ctrl_data, read_fifo_ctrl_data;
    uint32 temp1 = 0, temp2 = 0;
    g6phy16_step_size_t ss;
    uint32 step1000;
    uint32 size1000UI;
    uint32 setting;
    uint32 ck_ui, addr_ctrl_ui, dqs_ui, data_ui;
    uint32 error, status;
    uint32 dq_pd, dq_nd, dq_pterm, dq_nterm;
    uint32 aq_pd, aq_nd, aq_pterm, aq_nterm;
    /* uint32 fld_comp_done;
    soc_timeout_t to;
    sal_usecs_t to_val; */
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    uint32 i;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_validate_cbi(unit));
    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_validate_config(unit));

    /* if (SAL_BOOT_QUICKTURN)
    {
        to_val = 10000000;
    }
    else
    {
        to_val = 50000;
    } */
    
    if(drc_ndx != SHMOO_G6PHY16_INTERFACE_ALL)
    {
        ndx = drc_ndx;
        ndxEnd = drc_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_G6PHY16_MAX_INTERFACES;
    }
    
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_g6phy16_check_dram(unit, ndx))
        {
            continue;
        }
        
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization (DRC index: %02d)\n"), ndx));
        
/*A01*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A01. Reset all PHY logic\n")));
        reset_ctrl_data = 0;
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, ndx, reset_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A02*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A02. Enable Read FIFO\n")));
        read_fifo_ctrl_data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_READ_FIFO_CTRLr, &read_fifo_ctrl_data, SOC_PHY_GDDR6_W2R_MIN_DELAY_2f, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_READ_FIFO_CTRLr(unit, ndx, read_fifo_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_READ_FIFO_RESET_Nf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, ndx, reset_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_READ_FIFO_CTRLr, &read_fifo_ctrl_data, SOC_PHY_GDDR6_ENABLEf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_READ_FIFO_CTRLr(unit, ndx, read_fifo_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A03*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A03. Configure input shift control\n")));
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr, &data, SOC_PHY_GDDR6_DATAPATH_SHIFT_ENABLEf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr, &data, SOC_PHY_GDDR6_DATAPATH_ADDITIONAL_LATENCYf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr, &data, SOC_PHY_GDDR6_ADDRPATH_SHIFT_ENABLEf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr, &data, SOC_PHY_GDDR6_ADDRPATH_ADDITIONAL_LATENCYf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr, &data, SOC_PHY_GDDR6_RCMD_SHIFT_ENABLEf, 1));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr, &data, SOC_PHY_GDDR6_RCMD_ADDITIONAL_LATENCYf, 10));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_INPUT_SHIFT_CTRLr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A04*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A04. Configure power regulation\n")));
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_PWRDNf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_LDO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_LDO_W_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_BIAS_CTRLf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_BYPASSf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_CTRLf, 0x00));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_REF_EXTf, 1));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_REF_SEL_EXTf, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_LDO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_LDO_W_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr, &data, SOC_PHY_GDDR6_PWRDNf, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_LDO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_LDO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_LDO_W_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_LDO_R_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_LDO_W_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A05*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A05. Configure reference voltage\n")));
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_CTRLf, 0x0001));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, 0x92));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_CTRLf, 0x0000));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_VREF_DAC_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_VREF_DAC_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A06*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A06. Release VDL resets\n")));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_VDL_1G_RESET_Nf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, ndx, reset_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A07*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A07. Compute VDL step size\n")));
        _SOC_IF_ERR_EXIT(soc_g6phy16_calculate_step_size(unit, ndx, &ss));
        
        step1000 = ss.step1000;
        size1000UI = ss.size1000UI;
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL step size........: %3d.%03d ps\n"), (step1000 / 1000), (step1000 % 1000)));
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     UI size..............: %3d.%03d steps\n"), (size1000UI / 1000), (size1000UI % 1000)));
        
/*A08*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A08. Configure UI shifts\n")));
        ck_ui = SHMOO_G6PHY16_GDDR6_INIT_CK_UI_SHIFT;
        addr_ctrl_ui = SHMOO_G6PHY16_GDDR6_INIT_ADDR_CTRL_UI_SHIFT;
        dqs_ui = SHMOO_G6PHY16_GDDR6_INIT_DQS_UI_SHIFT;
        data_ui = SHMOO_G6PHY16_GDDR6_INIT_DATA_UI_SHIFT;
        
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_MAX_VDL_CKr, &data, SOC_PHY_GDDR6_UI_SHIFTf, ck_ui));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_MAX_VDL_CKr(unit, ndx, data));
        
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr, &data, SOC_PHY_GDDR6_UI_SHIFTf, addr_ctrl_ui));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_ADDRr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_ADDRr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_L_MAX_VDL_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_U_MAX_VDL_CTRLr(unit, ndx, data));
        
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr, &data, SOC_PHY_GDDR6_UI_SHIFTf, dqs_ui));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr, &data, SOC_PHY_GDDR6_UI_SHIFTf, data_ui));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_DATAr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_DATAr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_DATAr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_DATAr(unit, ndx, data));
        
/*A09*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A09. Configure VDLs\n")));
        data = 0;
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_DBI_Nr(unit, ndx, data));
    /*    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MIN_VDL_EDCr(unit, ndx, data)); */
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_DBI_Nr(unit, ndx, data));
    /*    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MIN_VDL_EDCr(unit, ndx, data)); */
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_DBI_Nr(unit, ndx, data));
    /*    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MIN_VDL_EDCr(unit, ndx, data)); */
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_DBI_Nr(unit, ndx, data));
    /*    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MIN_VDL_EDCr(unit, ndx, data)); */

        data = 0;
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_DBI_Nr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MIN_VDL_EDCr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_DBI_Nr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MIN_VDL_EDCr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_DBI_Nr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MIN_VDL_EDCr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT0r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT1r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT2r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT3r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT4r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT5r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT6r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_BIT7r(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_DBI_Nr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MIN_VDL_EDCr(unit, ndx, data));
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, SHMOO_G6PHY16_MAX_VDL_LENGTH - 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WRITE_MAX_VDL_WCKr(unit, ndx, data));
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr, &data, SOC_PHY_GDDR6_MASTER_MODEf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A10*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A10. ZQ calibration\n")));
        if(shmoo_dram_info.sim_system_mode)
        {
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Skipped for emulation\n")));
            goto SHMOO_G6PHY16_ZQ_CALIBRATION_END;
        }

        if(shmoo_dram_info.zq_cal_array[ndx] != ndx)
        {
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     Copying ZQ calibration results from PHY %02d\n"), shmoo_dram_info.zq_cal_array[ndx]));
            error = 0;
            status = 0;
            goto SHMOO_G6PHY16_ZQ_CALIBRATION_SLAVE_READ;
        }

        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, data, SOC_PHY_GDDR6_DATAf, &setting));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, 0x42));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_LONG_SLEEP);
        
        /* _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_INIT_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_INIT_CONFIGr, &data, SOC_PHY_GDDR6_COMP_INIT_FDEPTHf, 0xB));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_INIT_CONFIGr, &data, SOC_PHY_GDDR6_PNCOMP_INIT_DIFFf, 0x1F));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_INIT_CONFIGr, &data, SOC_PHY_GDDR6_CLK_DIV_RATIOf, 0x7));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_INIT_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_RESCAL_RESET_Nf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, ndx, reset_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_COMP_ENf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        error = 0;
        status = 0;
        soc_timeout_init(&to, to_val, 0);
        do
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr(unit, ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr, data, SOC_PHY_GDDR6_COMP_DONEf, &fld_comp_done));
            if(fld_comp_done)
            {
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr, data, SOC_PHY_GDDR6_COMP_ERRORf, &status));
                error = status & 0x1C;
                if(error)
                {
                    LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error - Manual IO programming may be required for correct operation\n")));
                    error |= 0x10020;
                }
                break;
            }
            if (soc_timeout_check(&to))
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: Timed out during ZQ calibration\n")));
                error |= 0x20000;
                break;
            }
        }
        while(TRUE);
        
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr, data, SOC_PHY_GDDR6_PCOMP_CODE_2COREf, &temp1));
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr, data, SOC_PHY_GDDR6_NCOMP_CODE_2COREf, &temp2));
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_COMP_ENf, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, data));
        
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_RESCAL_RESET_Nf, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, ndx, reset_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP); */
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_MANUAL_OVERRIDE_ENf, 1));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_COMP_ENf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, data));

        temp1 = 32;
        temp2 = 32;

        for(i = 0; i < 32; i++)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_OVERRIDE_VALUEf, i));
            _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, data));
            sal_usleep(SHMOO_G6PHY16_DEEP_SLEEP);

            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr(unit, ndx, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr, data, SOC_PHY_GDDR6_PCOMP_DINf, &status));
            if((temp1 == 32) && status)
            {
                temp1 = i;
            }
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_STATUS_RESCALr, data, SOC_PHY_GDDR6_NCOMP_DINf, &status));
            if((temp2 == 32) && !status)
            {
                temp2 = i;
            }
        }

        if((temp1 == 32) || (temp2 == 32))
        {
            error = 1;
            status = 1;
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration error - Default settings are used\n")));

            temp1=19;
            temp2=22;
        }
        else
        {
            error = 0;
            status = 0;
        }
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_MANUAL_OVERRIDE_ENf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr, &data, SOC_PHY_GDDR6_COMP_ENf, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_RESCAL_OPERATION_CONFIGr(unit, ndx, data));

        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr, &data, SOC_PHY_GDDR6_DATAf, setting));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_SHARED_VREF_DAC_CONFIGr(unit, ndx, data));
        
        SHMOO_G6PHY16_ZQ_CALIBRATION_SLAVE_READ:

        if(shmoo_dram_info.zq_cal_array[ndx] != ndx)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr(unit, (uint32) shmoo_dram_info.zq_cal_array[ndx], &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, data, SOC_PHY_GDDR6_PDf, &temp1));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, data, SOC_PHY_GDDR6_NDf, &temp2));

            /* Remove manually added offsets below to PD and ND. They will be added back below as needed. */
            temp1 -= 6;
            /* temp2; */
        }

        dq_pd = temp1 + 6;
        dq_nd = temp2;
        dq_pterm = temp1;
        dq_nterm = temp2;
        aq_pd = temp1 - 5;
        aq_nd = temp2;
        aq_pterm = temp1 - 5;
        aq_nterm = temp2;
        
        if((dq_pd | dq_nd | dq_pterm | dq_nterm | aq_pd | aq_pterm | aq_nterm) & 0xFFFFFFE0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     WARNING: ZQ calibration out of bounds - Manual IO programming may be required for correct operation\n")));
            error |= 0x40000;
        }
        
        if(error)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              Status Code......................: 0x%02X\n"), status));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Pdrive.................: 0x%02X\n"), 0xFF & dq_pd));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Ndrive.................: 0x%02X\n"), 0xFF & dq_nd));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Ptermination...........: 0x%02X\n"), 0xFF & dq_pterm));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Ntermination (RFU).....: 0x%02X\n"), 0xFF & dq_nterm));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Pdrive........................: 0x%02X\n"), 0xFF & aq_pd));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Ndrive........................: 0x%02X\n"), 0xFF & aq_nd));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Ptermination..................: 0x%02X\n"), 0xFF & aq_pterm));
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Ntermination (RFU)............: 0x%02X\n"), 0xFF & aq_nterm));
            
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
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              Status Code......................: 0x%02X\n"), status));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Pdrive.................: 0x%02X\n"), 0xFF & dq_pd));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Ndrive.................: 0x%02X\n"), 0xFF & dq_nd));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Ptermination...........: 0x%02X\n"), 0xFF & dq_pterm));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              DQ/DQS/CK Ntermination (RFU).....: 0x%02X\n"), 0xFF & dq_nterm));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Pdrive........................: 0x%02X\n"), 0xFF & aq_pd));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Ndrive........................: 0x%02X\n"), 0xFF & aq_nd));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Ptermination..................: 0x%02X\n"), 0xFF & aq_pterm));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              AQ Ntermination (RFU)............: 0x%02X\n"), 0xFF & aq_nterm));
        }
        
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_PDf, dq_pd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_PTERMf, dq_pterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_NDf, dq_nd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_NTERMf, dq_nterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_PULL_UP_OFF_Bf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_RX_BIASf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_PEAKf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_DEMf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr, &data, SOC_PHY_GDDR6_RXENBf, 0x1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_IO_CONFIGr(unit, ndx, data));

        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_PDf, aq_pd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_PTERMf, aq_pterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_NDf, aq_nd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_NTERMf, aq_nterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_PULL_UP_OFF_Bf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_RX_BIASf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_PEAKf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_DEMf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr, &data, SOC_PHY_GDDR6_RXENBf, 0x1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_AQ_IO_CONFIGr(unit, ndx, data));

        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_PDf, dq_pd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_PTERMf, dq_pterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_NDf, dq_nd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_NTERMf, dq_nterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_PULL_UP_OFF_Bf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_RX_BIASf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_RXENBf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_PEAKf, 0x2));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr, &data, SOC_PHY_GDDR6_DEMf, 0x0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_DATA_IO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_DATA_IO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_DATA_IO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_DATA_IO_CONFIGr(unit, ndx, data));

        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_PDf, dq_pd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_PTERMf, dq_pterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_NDf, dq_nd));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_NTERMf, dq_nterm));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_PULL_UP_OFF_Bf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_RX_BIASf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_PEAKf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_DEMf, 0x0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr, &data, SOC_PHY_GDDR6_RXENBf, 0x1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_WCK_IO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_WCK_IO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_WCK_IO_CONFIGr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_WCK_IO_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_LONG_SLEEP);
        
        SHMOO_G6PHY16_ZQ_CALIBRATION_END:
        
/*A11*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A11. Enable CK\n")));
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_CK_CONFIGr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_COMMON_CK_CONFIGr, &data, SOC_PHY_GDDR6_CK_ENABLEf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_COMMON_CK_CONFIGr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A12*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A12. Configure FIFOs\n")));
        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr, &data, SOC_PHY_GDDR6_RDATA_WR2RD_DELAYf, 4));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr(unit, ndx, data));

        data = 0;
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_SYSTEM_ENABLEf, 0));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_GEN_ENABLEf, 0));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_FSM_CTRLr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A13*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A13. Release PHY resets\n")));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_DRC_1G_RESET_Nf, 1));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_PHY_1G_RESET_Nf, 1));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &reset_ctrl_data, SOC_PHY_GDDR6_CLOCK_DIV_RESET_Nf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, ndx, reset_ctrl_data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
/*A14*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A14. Enable FIFOs\n")));
        _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, ndx, &data));
        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_DATA_VALID_GEN_ENABLEf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_FSM_CTRLr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

        _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr, &data, SOC_PHY_GDDR6_SYSTEM_ENABLEf, 1));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_FSM_CTRLr(unit, ndx, data));
        _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_FSM_CTRLr(unit, ndx, data));
        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
        
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Series - PHY Initialization complete (DRC index: %02d)\n"), ndx));
    }

exit:
    SOC_FUNC_RETURN;
}

/* Register Device interface CB to shmoo */
int
soc_g6phy16_shmoo_interface_cb_register(int unit, g6phy16_shmoo_cbi_t shmoo_cbi)
{
    SOC_INIT_FUNC_DEFS;

    soc_g6phy16_phy_reg_read = shmoo_cbi.g6phy16_phy_reg_read;
    soc_g6phy16_phy_reg_write = shmoo_cbi.g6phy16_phy_reg_write;
    soc_g6phy16_phy_reg_modify = shmoo_cbi.g6phy16_phy_reg_modify;
    _shmoo_g6phy16_drc_bist_conf_set = shmoo_cbi.g6phy16_drc_bist_conf_set;
    _shmoo_g6phy16_drc_bist_run = shmoo_cbi.g6phy16_drc_bist_run;
    _shmoo_g6phy16_drc_bist_err_cnt = shmoo_cbi.g6phy16_drc_bist_err_cnt;
    _shmoo_g6phy16_drc_dram_init = shmoo_cbi.g6phy16_drc_dram_init;
    _shmoo_g6phy16_drc_pll_set = shmoo_cbi.g6phy16_drc_pll_set;
    _shmoo_g6phy16_drc_modify_mrs = shmoo_cbi.g6phy16_drc_modify_mrs;
    _shmoo_g6phy16_drc_enable_wck2ck_training = shmoo_cbi.g6phy16_drc_enable_wck2ck_training;
    _shmoo_g6phy16_drc_enable_write_leveling = shmoo_cbi.g6phy16_drc_enable_write_leveling;
    _shmoo_g6phy16_drc_mpr_en = shmoo_cbi.g6phy16_drc_mpr_en;
    _shmoo_g6phy16_drc_mpr_load = shmoo_cbi.g6phy16_drc_mpr_load;
    _shmoo_g6phy16_drc_vendor_info_get = shmoo_cbi.g6phy16_drc_vendor_info_get;
    _shmoo_g6phy16_drc_dqs_pulse_gen = shmoo_cbi.g6phy16_drc_dqs_pulse_gen;
    _shmoo_g6phy16_training_bist_conf_set = shmoo_cbi.g6phy16_training_bist_conf_set;
    _shmoo_g6phy16_training_bist_err_cnt = shmoo_cbi.g6phy16_training_bist_err_cnt;
    _shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get = shmoo_cbi.g6phy16_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get;
    _shmoo_g6phy16_drc_enable_wr_crc = shmoo_cbi.g6phy16_drc_enable_wr_crc;
    _shmoo_g6phy16_drc_enable_rd_crc = shmoo_cbi.g6phy16_drc_enable_rd_crc;
    _shmoo_g6phy16_drc_enable_wr_dbi = shmoo_cbi.g6phy16_drc_enable_wr_dbi;
    _shmoo_g6phy16_drc_enable_rd_dbi = shmoo_cbi.g6phy16_drc_enable_rd_dbi;
    _shmoo_g6phy16_drc_force_dqs = shmoo_cbi.g6phy16_drc_force_dqs;
    _shmoo_g6phy16_drc_soft_reset_drc_without_dram = shmoo_cbi.g6phy16_drc_soft_reset_drc_without_dram;
    _shmoo_g6phy16_drc_dram_info_access = shmoo_cbi.g6phy16_shmoo_dram_info_access;

    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_validate_cbi(unit));

exit:
    SOC_FUNC_RETURN;
}

/* BEGIN: CDR FUNCTIONS */
int
soc_g6phy16_cdr_ctl(int unit, int drc_ndx, int stat, g6phy16_cdr_config_param_t *config_param)
{
    int dram_index = drc_ndx;
    uint32 data, field;
    uint32 ep, dp, en, dn;
    uint32 data_valid_wr2rd, edc_valid_wr2rd, data_read_ui_shift, edc_read_ui_shift;
    uint32 timeout;
    uint32 init_operation_status, init_vdl_status;
    uint32 current_operation_status, current_vdl_status;
    uint32 check_lock;
    uint32 vdl_value_max_cap, vdl_value_min_cap, vdl_init_pos;
    uint32 vdl_init_ep[SHMOO_G6PHY16_BYTES_PER_INTERFACE];
    uint32 vdl_init_dp[SHMOO_G6PHY16_BYTES_PER_INTERFACE];
    uint32 vdl_init_en[SHMOO_G6PHY16_BYTES_PER_INTERFACE];
    uint32 vdl_init_dn[SHMOO_G6PHY16_BYTES_PER_INTERFACE];
    uint32 fld_lock_status;
    uint32 vmin, vmax;
    uint32 fld_init_track_status;
    g6phy16_step_size_t ss;
    soc_timeout_t to;
    sal_usecs_t to_val;
    g6phy16_shmoo_dram_info_t shmoo_dram_info;
    int byte_iter;

    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[] = { SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCK_MASTER_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCK_MASTER_CTRLr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKEPl[] = {           SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCKEPr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCKEPr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCKEPr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCKEPr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKDPl[] = {           SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCKDPr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCKDPr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCKDPr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCKDPr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKENl[] = {           SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCKENr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCKENr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCKENr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCKENr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKDNl[] = {           SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_MAX_VDL_RCKDNr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_MAX_VDL_RCKDNr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_MAX_VDL_RCKDNr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_MAX_VDL_RCKDNr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[] = {          SOC_PHY_GDDR6_REG_DQ_BYTE0_READ_ALIGNMENT_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_READ_ALIGNMENT_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_READ_ALIGNMENT_CTRLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_READ_ALIGNMENT_CTRLr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[] = {         SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_OPERATION_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_OPERATION_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_OPERATION_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_OPERATION_CONFIGr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[] = {            SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_UPDATE_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_UPDATE_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_UPDATE_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_UPDATE_CONFIGr};
    /* soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TRACK_CONFIGl[] = {             SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_TRACK_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_TRACK_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_TRACK_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_TRACK_CONFIGr}; */
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[] = {            SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_TIMING_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_TIMING_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_TIMING_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_TIMING_CONFIGr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[] = {           SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_VDL_CAP_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_VDL_CAP_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_VDL_CAP_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_VDL_CAP_CONFIGr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[] = {             SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_P_VDL_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_P_VDL_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_P_VDL_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_P_VDL_CONFIGr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[] = {             SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_N_VDL_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_N_VDL_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_N_VDL_CONFIGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_N_VDL_CONFIGr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[] = {             SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_P_VDLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_P_VDLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_P_VDLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_P_VDLr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[] = {             SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_N_VDLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_N_VDLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_N_VDLr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_N_VDLr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EP_VDL_STATl[] = {       SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_EP_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_EP_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_EP_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_EP_VDL_STATr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DP_VDL_STATl[] = {       SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_DP_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_DP_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_DP_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_DP_VDL_STATr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EN_VDL_STATl[] = {       SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_EN_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_EN_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_EN_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_EN_VDL_STATr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DN_VDL_STATl[] = {       SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_DN_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_DN_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_DN_VDL_STATr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_DN_VDL_STATr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[] = {         SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_OPERATIONr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_OPERATIONr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_OPERATIONr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_OPERATIONr};
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_TIMINGl[] = {            SOC_PHY_GDDR6_REG_DQ_BYTE0_STATUS_CDR_TIMINGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE1_STATUS_CDR_TIMINGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE2_STATUS_CDR_TIMINGr,
                                                                            SOC_PHY_GDDR6_REG_DQ_BYTE3_STATUS_CDR_TIMINGr};
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_g6phy16_drc_dram_info_access(unit, &shmoo_dram_info));

    if (SAL_BOOT_QUICKTURN)
    {
        to_val = 10000000;  /* 10 Sec */
    }
    else
    {
        to_val = 5000000;   /* 5 Sec */
    }

    if(!_shmoo_g6phy16_check_dram(unit, dram_index))
    {
        SOC_EXIT;
    }

    if((!stat) || ((*config_param).enable != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE))
    {
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "CDR ACTIONS (DRC index: %3d)\n"), dram_index));
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "--------------------------------------------------------------------------------------------------------------\n")));

        if((*config_param).reset_n == 0)
        {
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, data, SOC_PHY_GDDR6_CDR_RESET_Nf, &field));

            if(field)
            {
                if((*config_param).enable == 1)
                {
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Enable request is ignored. CDR is going into reset.\n")));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "All bytes are disabled by force.\n")));
                }
                
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "All programming requests are ignored. CDR is going into reset.\n")));

                for(byte_iter = 0; byte_iter < SHMOO_G6PHY16_BYTES_PER_INTERFACE; byte_iter++)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ENABLEf, &field));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_ENABLEf, 0));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, data));

                    if(field && ((*config_param).auto_copy == 1))
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], data, SOC_PHY_GDDR6_EP_VDL_VALUEf, &ep));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], data, SOC_PHY_GDDR6_DP_VDL_VALUEf, &dp));

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[byte_iter], data, SOC_PHY_GDDR6_EN_VDL_VALUEf, &en));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[byte_iter], data, SOC_PHY_GDDR6_DN_VDL_VALUEf, &dn));

                        data = 0;
                        if((ep == dp) && (dp == en) && (en == dn))
                        {
                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_MASTER_MODEf, 1));
                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_MAX_VDL_STEPf, ep));
                        }
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], dram_index, data));

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKEPl[byte_iter], dram_index, ep));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKDPl[byte_iter], dram_index, dp));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKENl[byte_iter], dram_index, en));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKDNl[byte_iter], dram_index, dn));

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_TIMINGl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_TIMINGl[byte_iter], data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, &data_valid_wr2rd));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_TIMINGl[byte_iter], data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, &edc_valid_wr2rd));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_TIMINGl[byte_iter], data, SOC_PHY_GDDR6_DATA_READ_UI_SHIFTf, &data_read_ui_shift));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_TIMINGl[byte_iter], data, SOC_PHY_GDDR6_EDC_READ_UI_SHIFTf, &edc_read_ui_shift));

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, data_valid_wr2rd));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, edc_valid_wr2rd));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, data_read_ui_shift));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], &data, SOC_PHY_GDDR6_EDC_UI_SHIFTf, edc_read_ui_shift));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], dram_index, data));
                    }
                }
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL settings, UI shifts, and FIFO distances are auto-copied back into the register file for all bytes. (Rollovers are TBI.)\n")));
                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                for(byte_iter = 0; byte_iter < SHMOO_G6PHY16_BYTES_PER_INTERFACE; byte_iter++)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_SWITCHf, 0));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                }
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL controls are coerced to the register file for all bytes.\n")));
                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &data, SOC_PHY_GDDR6_CDR_RESET_Nf, 0));
                _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, data));
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "All bytes are put into reset.\n")));
                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 4, 0x00F, 0x00F));
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "EDC hold pattern is set to 1111.\n")));
                sal_usleep(SHMOO_G6PHY16_LONG_SLEEP);
            }
            else
            {
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "No reset-related actions are taken.\n")));
            }
        }
        else
        {
            _SOC_IF_ERR_EXIT(soc_g6phy16_calculate_step_size(unit, dram_index, &ss));
            vdl_value_max_cap = SHMOO_G6PHY16_MAX_VDL_LENGTH - 1;   /* MAX VDL CAP */
            vdl_value_min_cap = 0;                                  /* MIN VDL CAP */

            for(byte_iter = 0; byte_iter < SHMOO_G6PHY16_BYTES_PER_INTERFACE; byte_iter++)
            {
                if((*config_param).auto_copy == 1)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], data, SOC_PHY_GDDR6_MASTER_MODEf, &field));
                    if(field)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCK_MASTER_CTRLl[byte_iter], data, SOC_PHY_GDDR6_MAX_VDL_STEPf, &vdl_init_pos));
                        vdl_init_ep[byte_iter] = vdl_init_pos;
                        vdl_init_dp[byte_iter] = vdl_init_pos;
                        vdl_init_en[byte_iter] = vdl_init_pos;
                        vdl_init_dn[byte_iter] = vdl_init_pos;
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKEPl[byte_iter], dram_index, &vdl_init_ep[byte_iter]));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKDPl[byte_iter], dram_index, &vdl_init_dp[byte_iter]));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKENl[byte_iter], dram_index, &vdl_init_en[byte_iter]));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_MAX_VDL_RCKDNl[byte_iter], dram_index, &vdl_init_dn[byte_iter]));
                    }
                }
                else
                {
                    vdl_init_pos = ((ss.size1000UI) << 1) / 1000;       /* 2 UIs */
                    vdl_init_ep[byte_iter] = vdl_init_pos;
                    vdl_init_dp[byte_iter] = vdl_init_pos;
                    vdl_init_en[byte_iter] = vdl_init_pos;
                    vdl_init_dn[byte_iter] = vdl_init_pos;
                }
            }

            if((*config_param).reset_n == 1)
            {
                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, data, SOC_PHY_GDDR6_CDR_RESET_Nf, &field));

                if(!field)
                {
                    _SOC_IF_ERR_EXIT(_shmoo_g6phy16_drc_modify_mrs(unit, dram_index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS, 4, 0x005, 0x00F));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "EDC hold pattern is set to 1010.\n")));
                    sal_usleep(SHMOO_G6PHY16_LONG_SLEEP);

                    for(byte_iter = 0; byte_iter < SHMOO_G6PHY16_BYTES_PER_INTERFACE; byte_iter++)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_ENABLEf, 0));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, data));
                    }
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "All bytes are disabled by force.\n")));
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                    for(byte_iter = 0; byte_iter < SHMOO_G6PHY16_BYTES_PER_INTERFACE; byte_iter++)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_FLEXIBLE_VDL_CAPf, 1));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_MAX_VALUE_CAPf, vdl_value_max_cap));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_MIN_VALUE_CAPf, vdl_value_min_cap));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], dram_index, data));
                    }
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL caps are updated for all bytes.\n")));
                    
                    _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, &data));
                    _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, &data, SOC_PHY_GDDR6_CDR_RESET_Nf, 1));
                    _SOC_IF_ERR_EXIT(WRITE_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, data));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "All bytes are released from reset.\n")));
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                }
                else
                {
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "No reset-related actions are taken.\n")));
                }
            }

            SHR_BIT_ITER(&((*config_param).byte_map), SHMOO_G6PHY16_BYTES_PER_INTERFACE, byte_iter)
            {
                if((*config_param).auto_copy == 1)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], dram_index, &data));
                    if((*config_param).ep != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EP_VDL_LOAD_VALUEf, (*config_param).ep + vdl_init_ep[byte_iter]));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EP_VDL_LOAD_VALUEf, vdl_init_ep[byte_iter]));
                    }
                    if((*config_param).dp != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DP_VDL_LOAD_VALUEf, (*config_param).dp + vdl_init_dp[byte_iter]));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DP_VDL_LOAD_VALUEf, vdl_init_dp[byte_iter]));
                    }
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], dram_index, data));

                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], dram_index, &data));
                    if((*config_param).en != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EN_VDL_LOAD_VALUEf, (*config_param).en + vdl_init_en[byte_iter]));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EN_VDL_LOAD_VALUEf, vdl_init_en[byte_iter]));
                    }
                    if((*config_param).dn != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DN_VDL_LOAD_VALUEf, (*config_param).dn + vdl_init_dn[byte_iter]));
                    }
                    else
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DN_VDL_LOAD_VALUEf, vdl_init_dn[byte_iter]));
                    }
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], dram_index, data));

                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAYf, &data_valid_wr2rd));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAYf, &edc_valid_wr2rd));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], data, SOC_PHY_GDDR6_DATA_UI_SHIFTf, &data_read_ui_shift));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_READ_ALIGNMENT_CTRLl[byte_iter], data, SOC_PHY_GDDR6_EDC_UI_SHIFTf, &edc_read_ui_shift));

                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DATA_VALID_WR2RD_DELAY_LOAD_VALUEf, data_valid_wr2rd));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EDC_VALID_WR2RD_DELAY_LOAD_VALUEf, edc_valid_wr2rd));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DATA_READ_UI_SHIFT_LOAD_VALUEf, data_read_ui_shift));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EDC_READ_UI_SHIFT_LOAD_VALUEf, edc_read_ui_shift));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));

                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_LOADf, 1));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                    sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_LOADf, 0));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                    
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL settings, UI shifts, and FIFO distances are auto-copied back into the CDR for Byte %d.\n"), byte_iter));
                }
                else
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], dram_index, &data));
                    if((*config_param).ep != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EP_VDL_LOAD_VALUEf, (*config_param).ep + vdl_init_ep[byte_iter]));
                    }
                    if((*config_param).dp != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DP_VDL_LOAD_VALUEf, (*config_param).dp + vdl_init_dp[byte_iter]));
                    }
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], dram_index, data));

                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], dram_index, &data));
                    if((*config_param).en != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_EN_VDL_LOAD_VALUEf, (*config_param).en + vdl_init_en[byte_iter]));
                    }
                    if((*config_param).dn != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_DN_VDL_LOAD_VALUEf, (*config_param).dn + vdl_init_dn[byte_iter]));
                    }
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], dram_index, data));

                    if(((*config_param).ep != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                       || ((*config_param).dp != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                       || ((*config_param).en != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                       || ((*config_param).dn != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE))
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_LOADf, 1));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_LOADf, 0));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));

                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Only manually-entered VDL settings are written into the CDR for Byte %d.\n"), byte_iter));
                    }
                }
                
                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[byte_iter], dram_index, &data));
                if((*config_param).update_gap != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_UPDATE_GAPf, (*config_param).update_gap));
                }
                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[byte_iter], dram_index, data));

                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
                if((*config_param).init_track_transition != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_INIT_TRACK_TRANSITIONf, (*config_param).init_track_transition));
                }
                if((*config_param).accu_pos_threshold != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_ACCU_POS_THRESHOLDf, (*config_param).accu_pos_threshold));
                }
                if((*config_param).update_mode != SHMOO_G6PHY16_CDR_UNDEFINED_VALUE)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_UPDATE_MODEf, (*config_param).update_mode));
                }
                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, data));

                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ENABLEf, &field));
                if((field) && ((*config_param).enable != 0))
                {
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: CDR configuration is modified while CDR is enabled for Byte %d.\n"), byte_iter));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "         CDR operation may have been disrupted, or some settings may have been ignored.\n")));
                }
                else
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_INIT_TRACK_OPTIONSf, 0));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, data));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "CDR configuration is complete for Byte %d.\n"), byte_iter));
                }
            }
            sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            
            if((*config_param).enable == 1)
            {
                _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, &data));
                _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, data, SOC_PHY_GDDR6_CDR_RESET_Nf, &field));
                
                if(!field)
                {
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Enable request is ignored. CDR is in reset.\n")));
                }
                else
                {
                    SHR_BIT_ITER(&((*config_param).byte_map), SHMOO_G6PHY16_BYTES_PER_INTERFACE, byte_iter)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_VDL_SWITCHf, 1));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL controls are transferred to the CDR for Byte %d.\n"), byte_iter));
                        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_CLEAR_VDL_STATSf, 1));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_CLEAR_VDL_STATSf, 0));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, data));
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL statistics are cleared for Byte %d.\n"), byte_iter));
                        sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);

                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ENABLEf, &field));
                        if(!field)
                        {
                            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Enabling Byte %d...\n"), byte_iter));
                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_ENABLEf, 1));
                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, data));
                            sal_usleep(SHMOO_G6PHY16_HIBERNATION);

                            soc_timeout_init(&to, to_val, 0);
                            do
                            {
                                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], dram_index, &data));
                                _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], data, SOC_PHY_GDDR6_INIT_SEARCH_COMPLETEf, &field));
                                if(field)
                                {
                                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Byte %d is locked and ready.\n"), byte_iter));
                                    break;
                                }

                                if(soc_timeout_check(&to))
                                {
                                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "WARNING: Byte %d could not lock.\n"), byte_iter));
                                    break;
                                }
                                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
                            }
                            while(TRUE);
                        }
                        else
                        {
                            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Enable request is ignored. Byte %d is already enabled.\n"), byte_iter));
                        }
                    }
                }
            }
            else if((*config_param).enable == 0)
            {
                SHR_BIT_ITER(&((*config_param).byte_map), SHMOO_G6PHY16_BYTES_PER_INTERFACE, byte_iter)
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_set(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], &data, SOC_PHY_GDDR6_ENABLEf, 0));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_write(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, data));
                    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Byte %d is disabled.\n"), byte_iter));
                }
                sal_usleep(SHMOO_G6PHY16_SHORT_SLEEP);
            }
            else
            {
                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "No enable-related actions are taken.\n")));
            }
        }
    }
    else
    {
        /* Report only */
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "CDR REPORT (DRC index: %3d)\n"), dram_index));
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "--------------------------------------------------------------------------------------------------------------\n")));


        SHR_BIT_ITER(&((*config_param).byte_map), SHMOO_G6PHY16_BYTES_PER_INTERFACE, byte_iter)
        {
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "BYTE %d\n"), byte_iter));
            _SOC_IF_ERR_EXIT(READ_G6DDRC16_SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr(unit, dram_index, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_CONTROL_REGS_RESET_CTRLr, data, SOC_PHY_GDDR6_CDR_RESET_Nf, &field));
            check_lock = field;
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Out of reset (Reset_N).............: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ENABLEf, &field));
            check_lock &= field;
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Enable.............................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ACCU_POS_THRESHOLDf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Accumulator positive threshold.....: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_INIT_TRACK_TRANSITIONf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Initial lock transition............: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ROLLOVER_MODEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Rollover mode......................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_UPDATE_MODEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Update mode........................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_VDL_UPDATE_GAPf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Update gap.........................: %3d\n"), field));
            _SOC_IF_ERR_EXIT( soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_UPDATE_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ROLLOVER_AMOUNTf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Rollover amount....................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_EP_VDL_LOAD_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Initial EP value...................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_P_VDL_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_DP_VDL_LOAD_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Initial DP value...................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_EN_VDL_LOAD_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Initial EN value...................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_N_VDL_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_DN_VDL_LOAD_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Initial DN value...................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], data, SOC_PHY_GDDR6_EP_VDL_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current EP value...................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], data, SOC_PHY_GDDR6_DP_VDL_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current DP value...................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[byte_iter], data, SOC_PHY_GDDR6_EN_VDL_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current EN value...................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_N_VDLl[byte_iter], data, SOC_PHY_GDDR6_DN_VDL_VALUEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current DN value...................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EP_VDL_STATl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EP_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MAX_VDL_VALUEf, &vmax));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EP_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MIN_VDL_VALUEf, &vmin));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Historical EP range (Min/Max)......: %3d / %3d\n"), vmin, vmax));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DP_VDL_STATl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DP_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MAX_VDL_VALUEf, &vmax));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DP_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MIN_VDL_VALUEf, &vmin));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Historical DP range (Min/Max)......: %3d / %3d\n"), vmin, vmax));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EN_VDL_STATl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EN_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MAX_VDL_VALUEf, &vmax));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_EN_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MIN_VDL_VALUEf, &vmin));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Historical EN range (Min/Max)......: %3d / %3d\n"), vmin, vmax));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DN_VDL_STATl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DN_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MAX_VDL_VALUEf, &vmax));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_DN_VDL_STATl[byte_iter], data, SOC_PHY_GDDR6_MIN_VDL_VALUEf, &vmin));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Historical DN range (Min/Max)......: %3d / %3d\n"), vmin, vmax));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_FLEXIBLE_VDL_CAPf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Flexible VDL cap...................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_VDL_MAX_VALUE_CAPf, &vmax));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_VDL_CAP_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_VDL_MIN_VALUE_CAPf, &vmin));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Rollover points (Min/Max)..........: %3d / %3d\n"), vmin, vmax));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_TIMING_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_VDL_SWITCHf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL switch.........................: %3d\n"), field));

            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], dram_index, &data));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], data, SOC_PHY_GDDR6_ACCU_VALUEf, &field));
            if(field & 0x40)
            {
                field |= 0xFFFFFF80;
            }
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current accumulator value..........: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], data, SOC_PHY_GDDR6_ROLLOVER_COUNTf, &field));
            if(field & 0x40)
            {
                field |= 0xFFFFFF80;
            }
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current rollover count.............: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], data, SOC_PHY_GDDR6_ROLLOVER_OVERFLOWf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Rollover overflow..................: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], data, SOC_PHY_GDDR6_INIT_SEARCH_COMPLETEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Initial search complete reading....: %3d\n"), field));
            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], data, SOC_PHY_GDDR6_TRACK_ACTIVEf, &field));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Current track reading..............: %3d\n"), field));

            if(check_lock)
            {
                timeout = 0;
                soc_timeout_init(&to, to_val, 0);
                do
                {
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], dram_index, &init_operation_status));
                    _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], init_operation_status, SOC_PHY_GDDR6_INIT_SEARCH_COMPLETEf, &fld_init_track_status));
                    if(fld_init_track_status)
                    {
                        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], dram_index, &init_vdl_status));
                        do
                        {
                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], dram_index, &current_operation_status));
                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_P_VDLl[byte_iter], dram_index, &current_vdl_status));
                            if(soc_timeout_check(&to))
                            {
                                timeout = 1;
                                LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Lock status........................: NOT LOCKED (Lock indication error)\n")));
                                break;
                            }

                            _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_STATUS_CDR_OPERATIONl[byte_iter], current_operation_status, SOC_PHY_GDDR6_TRACK_ACTIVEf, &fld_lock_status));
                        }
                        while((current_vdl_status == init_vdl_status) && (current_operation_status == init_operation_status) && !fld_lock_status);

                        if(timeout)
                        {
                            break;
                        }
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Lock status........................: LOCKED\n")));
                        break;
                    }
                    if (soc_timeout_check(&to))
                    {
                        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Lock status........................: NOT LOCKED (Lock indication error)\n")));
                        break;
                    }
                }
                while(TRUE);
            }
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "\n")));
        }
    }

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "--------------------------------------------------------------------------------------------------------------\n\n\n")));

exit:
    SOC_FUNC_RETURN;
}

int
soc_g6phy16_cdr_enable(
    int unit,
    int dram_index)
{

    g6phy16_cdr_config_param_t cdr_param;

    SOC_INIT_FUNC_DEFS;

    cdr_param.byte_map = 0xF;
    cdr_param.reset_n = 1;
    cdr_param.enable = 1;
    /** offset cdr(ep,dp,en,dn) from actual vdl value to promise locking of the cdr */
    cdr_param.ep = 3;
    cdr_param.dp = 3;
    cdr_param.en = 3;
    cdr_param.dn = 3;
    cdr_param.init_track_transition = 1;
    cdr_param.accu_pos_threshold = 64;
    cdr_param.update_gap = 8;
    cdr_param.update_mode = 1;
    cdr_param.auto_copy = 1;

    _SOC_IF_ERR_EXIT(soc_g6phy16_cdr_ctl(unit, dram_index, 0, &cdr_param));

exit:
    SOC_FUNC_RETURN;
}

int
soc_g6phy16_cdr_disable(
    int unit,
    int dram_index)
{

    g6phy16_cdr_config_param_t cdr_param = {0};

    SOC_INIT_FUNC_DEFS;

    cdr_param.byte_map = 0xF;
    cdr_param.reset_n = 0;
    cdr_param.enable = 0;
    /** offset cdr(ep,dp,en,dn) from actual vdl value to promise locking of the cdr */
    cdr_param.ep = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.dp = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.en = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.dn = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.init_track_transition = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.accu_pos_threshold = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.update_gap = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.update_mode = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.auto_copy = 0;

    _SOC_IF_ERR_EXIT(soc_g6phy16_cdr_ctl(unit, dram_index, 0, &cdr_param));

exit:
    SOC_FUNC_RETURN;
}

/** check if CDR is enabled for this dram index, indication that it is enabled will be given only if all bytes in the dram has CDR enabled */
int
soc_g6phy16_cdr_is_enabled(
    int unit,
    int dram_index,
    int * cdr_is_enabled)
{
    int byte_iter;
    uint32 data;
    uint32 field;
    soc_reg_t SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[] = { SOC_PHY_GDDR6_REG_DQ_BYTE0_CDR_OPERATION_CONFIGr, SOC_PHY_GDDR6_REG_DQ_BYTE1_CDR_OPERATION_CONFIGr, SOC_PHY_GDDR6_REG_DQ_BYTE2_CDR_OPERATION_CONFIGr, SOC_PHY_GDDR6_REG_DQ_BYTE3_CDR_OPERATION_CONFIGr};

    SOC_INIT_FUNC_DEFS;

    /** Assume CDR is enabled */
    *cdr_is_enabled = 1;

    /** Check all bytes to see if cdr is actually enabled, only if CDR is enabled in all byte the final indication will be that the CDR is enabled */
    for(byte_iter = 0; byte_iter < SHMOO_G6PHY16_BYTES_PER_INTERFACE; byte_iter++)
    {
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg32_read(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], dram_index, &data));
        _SOC_IF_ERR_EXIT(soc_phy_g6ddrc16_reg_field_get(unit, SOC_PHY_GDDR6_REG_DQ_BYTE_CDR_OPERATION_CONFIGl[byte_iter], data, SOC_PHY_GDDR6_ENABLEf, &field));
        *cdr_is_enabled &= field;
    }

exit:
    SOC_FUNC_RETURN;
}

/** print CDR status */
int
soc_g6phy16_cdr_status(
    int unit,
    int dram_index)
{

    g6phy16_cdr_config_param_t cdr_param = {0};
    SOC_INIT_FUNC_DEFS;

    cdr_param.enable = SHMOO_G6PHY16_CDR_UNDEFINED_VALUE;
    cdr_param.byte_map = 0xF;
    _SOC_IF_ERR_EXIT(soc_g6phy16_cdr_ctl(unit, dram_index, 1, &cdr_param));

exit:
    SOC_FUNC_RETURN;
}

/* END: CDR FUNCTIONS */

#undef _ERR_MSG_MODULE_NAME

#endif /* BCM_DDRC16_SUPPORT */

