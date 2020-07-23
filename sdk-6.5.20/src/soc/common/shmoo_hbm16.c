/** \file src/soc/common/shmoo_hbm16.c
 *  
 * HBM shmoo (HBM PHY tuning) implementation.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <soc/memtune.h>
#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/hbm_phy_regs.h>
#include <soc/hbmc_shmoo.h>
#include <soc/shmoo_hbm16.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
/*
 * }
 */

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_DDR

soc_hbm16_phy_reg_read_t soc_hbm16_phy_reg_read = NULL;
soc_hbm16_phy_reg_write_t soc_hbm16_phy_reg_write = NULL;
soc_hbm16_phy_reg_modify_t soc_hbm16_phy_reg_modify = NULL;
static shmoo_hbm16_hbmc_bist_conf_set_t _shmoo_hbm16_hbmc_bist_conf_set = NULL;
static shmoo_hbm16_hbmc_bist_err_cnt_t _shmoo_hbm16_hbmc_bist_err_cnt = NULL;
static shmoo_hbm16_hbmc_custom_bist_run_t _shmoo_hbm16_hbmc_custom_bist_run = NULL;
static shmoo_hbm16_hbmc_dram_init_t _shmoo_hbm16_hbmc_dram_init = NULL;
static shmoo_hbm16_hbmc_pll_set_t _shmoo_hbm16_hbmc_pll_set = NULL;
static shmoo_hbm16_hbmc_modify_mrs_t _shmoo_hbm16_hbmc_modify_mrs = NULL;
static shmoo_hbm16_hbmc_enable_wr_parity_t _shmoo_hbm16_hbmc_enable_wr_parity = NULL;
static shmoo_hbm16_hbmc_enable_rd_parity_t _shmoo_hbm16_hbmc_enable_rd_parity = NULL;
static shmoo_hbm16_hbmc_enable_addr_parity_t _shmoo_hbm16_hbmc_enable_addr_parity = NULL;
static shmoo_hbm16_hbmc_enable_wr_dbi_t _shmoo_hbm16_hbmc_enable_wr_dbi = NULL;
static shmoo_hbm16_hbmc_enable_rd_dbi_t _shmoo_hbm16_hbmc_enable_rd_dbi = NULL;
static shmoo_hbm16_hbmc_enable_refresh_t _shmoo_hbm16_hbmc_enable_refresh = NULL;
static shmoo_hbm16_hbmc_soft_reset_controller_without_dram_t _shmoo_hbm16_hbmc_soft_reset_controller_without_dram = NULL;
static shmoo_hbm16_hbmc_dram_info_access_t _shmoo_hbm16_hbmc_dram_info_access = NULL;
static shmoo_hbm16_hbmc_bist_status_get_t _shmoo_hbm16_hbmc_bist_status_get = NULL;
static shmoo_hbm16_hbmc_model_part_get_t _shmoo_hbm16_hbmc_shmoo_model_part_get = NULL;
static shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check_t _shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check = NULL;

const uint32 shmoo_order_hbm16_gen2[SHMOO_HBM16_GEN2_SEQUENCE_COUNT] =
{
    SHMOO_HBM16_RD_EXTENDED,
    SHMOO_HBM16_WR_EXTENDED,
    SHMOO_HBM16_ADDR_CTRL_EXTENDED
};

const uint32 shmoo_hbm16_gen2_wr_vref[SHMOO_HBM16_MAX_VREF_RANGE] =
{
    3,  /* 38% */
    2,  /* 42% */
    1,  /* 46% */
    0,  /* 50% */
    4,  /* 54% */
    5,  /* 58% */
    6,  /* 62% */
    7   /* 66% */
};

/* const array which hold the shmoo tune metadata per channel */
const hbmc_shmoo_channel_config_param_t channel_config_param_metadata[HBMC_SHMOO_CHANNEL_METADATA_SIZE] = 
{   
    /* DWORD0 */
    {HBM16_DWORD0_WRITE_MAX_VDL_DATAr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD0_WRITE_MAX_VDL_DATA__FIELDS_ALL"},
    {HBM16_DWORD0_WRITE_MAX_VDL_DATAr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_WRITE_MAX_VDL_DATAr   , HBM16_UI_SHIFTf                       , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_WRITE_MAX_VDL_DQSr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_WRITE_MAX_VDL_DQS__FIELDS_ALL"},
    {HBM16_DWORD0_READ_MIN_VDL_DATAr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_READ_MIN_VDL_DATA__FIELDS_ALL"},    
    {HBM16_DWORD0_READ_MAX_VDL_DQS_Pr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD0_READ_MAX_VDL_DQS_P__FIELDS_ALL"},   
    {HBM16_DWORD0_READ_MAX_VDL_DQS_Pr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_READ_MAX_VDL_DQS_Nr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD0_READ_MAX_VDL_DQS_N__FIELDS_ALL"},
    {HBM16_DWORD0_READ_MAX_VDL_DQS_Nr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_2                      ,   NULL},
    {HBM16_DWORD0_READ_MAX_VDL_FSMr     , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_READ_MAX_VDL_FSM__FIELDS_ALL"}, 
    {HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr , HBM16_AUTO_UPDATEf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_RXEN_RXBV_GEN_CONFIG__AUTO_UPDATE"},    
    {HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr , HBM16_RD_DATA_DELAYf                  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_RXEN_RXBV_GEN_CONFIG__RD_DATA_DELAY"},  
    {HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr , HBM16_PAR_LATENCYf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_RXEN_RXBV_GEN_CONFIG__PAR_LATENCY"},    
    {HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_PRE_CYCLESf                , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_RXEN_RXBV_GEN_CONFIG__RXEN_PRE_CYCLES"},        
    {HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_POST_CYCLESf               , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_RXEN_RXBV_GEN_CONFIG__RXEN_POST_CYCLES"},   
    {HBM16_DWORD0_READ_MACHINE_CONFIGr  , HBM16_PHASE_FIFO_MIN_DELAY_2f         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_READ_MACHINE_CONFIG__PHASE_FIFO_MIN_DELAY_2"},
    {HBM16_DWORD0_READ_MACHINE_CONFIGr  , HBM16_RDATA_FIFO_WR2RD_DELAYf         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_READ_MACHINE_CONFIG__RDATA_FIFO_WR2RD_DELAY"},  
    {HBM16_DWORD0_IO_CONFIGr            , HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf   , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_IO_CONFIG__DIFF_PORT_DQS_DRIVE_STRENGTH"},      
    {HBM16_DWORD0_IO_CONFIGr            , HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_IO_CONFIG__SINGLE_PORT_DQ_DRIVE_STRENGTH"},     
    {HBM16_DWORD0_LANE_REMAPr           , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD0_LANE_REMAP__FIELDS_ALL"},   
    {HBM16_DWORD0_LANE_REMAPr           , HBM16_SEL0f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_LANE_REMAPr           , HBM16_SEL1f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_LANE_REMAPr           , HBM16_SEL2f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_LANE_REMAPr           , HBM16_SEL3f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD0_LATENCY_SHIFTERr      , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD0_LATENCY_SHIFTER__FIELDS_ALL"},  
    {HBM16_DWORD0_DAC_CONFIGr           , HBM16_DAC_DATAf                       , HBMC_SHMOO_LEVEL_0 | HBMC_SHMOO_OTP_VER_1_2 ,   "DWORD0_DAC_CONFIG__DAC_DATA"},
    {HBM16_DWORD0_REG_RESERVEDr         , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        ,   "DWORD0_REG_RESERVED__FIELDS_ALL"},      
    /* DWORD1 */
    {HBM16_DWORD1_WRITE_MAX_VDL_DATAr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD1_WRITE_MAX_VDL_DATA__FIELDS_ALL"},
    {HBM16_DWORD1_WRITE_MAX_VDL_DATAr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_WRITE_MAX_VDL_DATAr   , HBM16_UI_SHIFTf                       , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_WRITE_MAX_VDL_DQSr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_WRITE_MAX_VDL_DQS__FIELDS_ALL"},
    {HBM16_DWORD1_READ_MIN_VDL_DATAr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_READ_MIN_VDL_DATA__FIELDS_ALL"},    
    {HBM16_DWORD1_READ_MAX_VDL_DQS_Pr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD1_READ_MAX_VDL_DQS_P__FIELDS_ALL"},   
    {HBM16_DWORD1_READ_MAX_VDL_DQS_Pr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_READ_MAX_VDL_DQS_Nr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD1_READ_MAX_VDL_DQS_N__FIELDS_ALL"},
    {HBM16_DWORD1_READ_MAX_VDL_DQS_Nr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_2                      ,   NULL},
    {HBM16_DWORD1_READ_MAX_VDL_FSMr     , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_READ_MAX_VDL_FSM__FIELDS_ALL"}, 
    {HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr , HBM16_AUTO_UPDATEf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_RXEN_RXBV_GEN_CONFIG__AUTO_UPDATE"},    
    {HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr , HBM16_RD_DATA_DELAYf                  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_RXEN_RXBV_GEN_CONFIG__RD_DATA_DELAY"},  
    {HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr , HBM16_PAR_LATENCYf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_RXEN_RXBV_GEN_CONFIG__PAR_LATENCY"},    
    {HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_PRE_CYCLESf                , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_RXEN_RXBV_GEN_CONFIG__RXEN_PRE_CYCLES"},        
    {HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_POST_CYCLESf               , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_RXEN_RXBV_GEN_CONFIG__RXEN_POST_CYCLES"},   
    {HBM16_DWORD1_READ_MACHINE_CONFIGr  , HBM16_PHASE_FIFO_MIN_DELAY_2f         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_READ_MACHINE_CONFIG__PHASE_FIFO_MIN_DELAY_2"},
    {HBM16_DWORD1_READ_MACHINE_CONFIGr  , HBM16_RDATA_FIFO_WR2RD_DELAYf         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_READ_MACHINE_CONFIG__RDATA_FIFO_WR2RD_DELAY"},  
    {HBM16_DWORD1_IO_CONFIGr            , HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf   , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_IO_CONFIG__DIFF_PORT_DQS_DRIVE_STRENGTH"},      
    {HBM16_DWORD1_IO_CONFIGr            , HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_IO_CONFIG__SINGLE_PORT_DQ_DRIVE_STRENGTH"},     
    {HBM16_DWORD1_LANE_REMAPr           , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD1_LANE_REMAP__FIELDS_ALL"},   
    {HBM16_DWORD1_LANE_REMAPr           , HBM16_SEL0f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_LANE_REMAPr           , HBM16_SEL1f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_LANE_REMAPr           , HBM16_SEL2f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_LANE_REMAPr           , HBM16_SEL3f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD1_LATENCY_SHIFTERr      , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD1_LATENCY_SHIFTER__FIELDS_ALL"},  
    {HBM16_DWORD1_DAC_CONFIGr           , HBM16_DAC_DATAf                       , HBMC_SHMOO_LEVEL_0 | HBMC_SHMOO_OTP_VER_1_2 ,   "DWORD1_DAC_CONFIG__DAC_DATA"},
    {HBM16_DWORD1_REG_RESERVEDr         , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        ,   "DWORD1_REG_RESERVED__FIELDS_ALL"}, 
    /* DWORD2 */
    {HBM16_DWORD2_WRITE_MAX_VDL_DATAr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD2_WRITE_MAX_VDL_DATA__FIELDS_ALL"},
    {HBM16_DWORD2_WRITE_MAX_VDL_DATAr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_WRITE_MAX_VDL_DATAr   , HBM16_UI_SHIFTf                       , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_WRITE_MAX_VDL_DQSr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_WRITE_MAX_VDL_DQS__FIELDS_ALL"},
    {HBM16_DWORD2_READ_MIN_VDL_DATAr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_READ_MIN_VDL_DATA__FIELDS_ALL"},    
    {HBM16_DWORD2_READ_MAX_VDL_DQS_Pr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD2_READ_MAX_VDL_DQS_P__FIELDS_ALL"},   
    {HBM16_DWORD2_READ_MAX_VDL_DQS_Pr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_READ_MAX_VDL_DQS_Nr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD2_READ_MAX_VDL_DQS_N__FIELDS_ALL"},
    {HBM16_DWORD2_READ_MAX_VDL_DQS_Nr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_2                      ,   NULL},
    {HBM16_DWORD2_READ_MAX_VDL_FSMr     , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_READ_MAX_VDL_FSM__FIELDS_ALL"}, 
    {HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr , HBM16_AUTO_UPDATEf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_RXEN_RXBV_GEN_CONFIG__AUTO_UPDATE"},    
    {HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr , HBM16_RD_DATA_DELAYf                  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_RXEN_RXBV_GEN_CONFIG__RD_DATA_DELAY"},  
    {HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr , HBM16_PAR_LATENCYf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_RXEN_RXBV_GEN_CONFIG__PAR_LATENCY"},    
    {HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_PRE_CYCLESf                , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_RXEN_RXBV_GEN_CONFIG__RXEN_PRE_CYCLES"},        
    {HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_POST_CYCLESf               , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_RXEN_RXBV_GEN_CONFIG__RXEN_POST_CYCLES"},   
    {HBM16_DWORD2_READ_MACHINE_CONFIGr  , HBM16_PHASE_FIFO_MIN_DELAY_2f         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_READ_MACHINE_CONFIG__PHASE_FIFO_MIN_DELAY_2"},
    {HBM16_DWORD2_READ_MACHINE_CONFIGr  , HBM16_RDATA_FIFO_WR2RD_DELAYf         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_READ_MACHINE_CONFIG__RDATA_FIFO_WR2RD_DELAY"},  
    {HBM16_DWORD2_IO_CONFIGr            , HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf   , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_IO_CONFIG__DIFF_PORT_DQS_DRIVE_STRENGTH"},      
    {HBM16_DWORD2_IO_CONFIGr            , HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_IO_CONFIG__SINGLE_PORT_DQ_DRIVE_STRENGTH"},     
    {HBM16_DWORD2_LANE_REMAPr           , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD2_LANE_REMAP__FIELDS_ALL"},   
    {HBM16_DWORD2_LANE_REMAPr           , HBM16_SEL0f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_LANE_REMAPr           , HBM16_SEL1f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_LANE_REMAPr           , HBM16_SEL2f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_LANE_REMAPr           , HBM16_SEL3f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD2_LATENCY_SHIFTERr      , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD2_LATENCY_SHIFTER__FIELDS_ALL"},  
    {HBM16_DWORD2_DAC_CONFIGr           , HBM16_DAC_DATAf                       , HBMC_SHMOO_LEVEL_0 | HBMC_SHMOO_OTP_VER_1_2 ,   "DWORD2_DAC_CONFIG__DAC_DATA"},
    {HBM16_DWORD2_REG_RESERVEDr         , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        ,   "DWORD2_REG_RESERVED__FIELDS_ALL"}, 
    /* DWORD3 */
    {HBM16_DWORD3_WRITE_MAX_VDL_DATAr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD3_WRITE_MAX_VDL_DATA__FIELDS_ALL"},
    {HBM16_DWORD3_WRITE_MAX_VDL_DATAr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_WRITE_MAX_VDL_DATAr   , HBM16_UI_SHIFTf                       , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_WRITE_MAX_VDL_DQSr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_WRITE_MAX_VDL_DQS__FIELDS_ALL"},
    {HBM16_DWORD3_READ_MIN_VDL_DATAr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_READ_MIN_VDL_DATA__FIELDS_ALL"},    
    {HBM16_DWORD3_READ_MAX_VDL_DQS_Pr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD3_READ_MAX_VDL_DQS_P__FIELDS_ALL"},   
    {HBM16_DWORD3_READ_MAX_VDL_DQS_Pr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_READ_MAX_VDL_DQS_Nr   , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD3_READ_MAX_VDL_DQS_N__FIELDS_ALL"},
    {HBM16_DWORD3_READ_MAX_VDL_DQS_Nr   , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_2                      ,   NULL},
    {HBM16_DWORD3_READ_MAX_VDL_FSMr     , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_READ_MAX_VDL_FSM__FIELDS_ALL"}, 
    {HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr , HBM16_AUTO_UPDATEf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_RXEN_RXBV_GEN_CONFIG__AUTO_UPDATE"},    
    {HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr , HBM16_RD_DATA_DELAYf                  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_RXEN_RXBV_GEN_CONFIG__RD_DATA_DELAY"},  
    {HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr , HBM16_PAR_LATENCYf                    , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_RXEN_RXBV_GEN_CONFIG__PAR_LATENCY"},    
    {HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_PRE_CYCLESf                , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_RXEN_RXBV_GEN_CONFIG__RXEN_PRE_CYCLES"},        
    {HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr , HBM16_RXEN_POST_CYCLESf               , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_RXEN_RXBV_GEN_CONFIG__RXEN_POST_CYCLES"},   
    {HBM16_DWORD3_READ_MACHINE_CONFIGr  , HBM16_PHASE_FIFO_MIN_DELAY_2f         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_READ_MACHINE_CONFIG__PHASE_FIFO_MIN_DELAY_2"},
    {HBM16_DWORD3_READ_MACHINE_CONFIGr  , HBM16_RDATA_FIFO_WR2RD_DELAYf         , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_READ_MACHINE_CONFIG__RDATA_FIFO_WR2RD_DELAY"},  
    {HBM16_DWORD3_IO_CONFIGr            , HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf   , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_IO_CONFIG__DIFF_PORT_DQS_DRIVE_STRENGTH"},      
    {HBM16_DWORD3_IO_CONFIGr            , HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf  , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_IO_CONFIG__SINGLE_PORT_DQ_DRIVE_STRENGTH"},     
    {HBM16_DWORD3_LANE_REMAPr           , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "DWORD3_LANE_REMAP__FIELDS_ALL"},   
    {HBM16_DWORD3_LANE_REMAPr           , HBM16_SEL0f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_LANE_REMAPr           , HBM16_SEL1f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_LANE_REMAPr           , HBM16_SEL2f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_LANE_REMAPr           , HBM16_SEL3f                           , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_DWORD3_LATENCY_SHIFTERr      , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "DWORD3_LATENCY_SHIFTER__FIELDS_ALL"},  
    {HBM16_DWORD3_DAC_CONFIGr           , HBM16_DAC_DATAf                       , HBMC_SHMOO_LEVEL_0 | HBMC_SHMOO_OTP_VER_1_2 ,   "DWORD3_DAC_CONFIG__DAC_DATA"},
    {HBM16_DWORD3_REG_RESERVEDr         , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        ,   "DWORD3_REG_RESERVED__FIELDS_ALL"}, 
    /* AWORD */
    {HBM16_AWORD_WRITE_MAX_VDL_CKr      , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "AWORD_WRITE_MAX_VDL_CK__FIELDS_ALL"},  
    {HBM16_AWORD_WRITE_MAX_VDL_ADDRr    , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "AWORD_WRITE_MAX_VDL_ADDR__FIELDS_ALL"},    
    {HBM16_AWORD_WRITE_MAX_VDL_ADDRr    , HBM16_MAX_VDL_STEPf                   , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_AWORD_WRITE_MAX_VDL_ADDRr    , HBM16_UI_SHIFTf                       , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_AWORD_IO_CONFIGr             , HBM16_DIFF_PORT_CK_DRIVE_STRENGTHf    , HBMC_SHMOO_LEVEL_2                        ,   "AWORD_IO_CONFIG__DIFF_PORT_CK_DRIVE_STRENGTH"},    
    {HBM16_AWORD_IO_CONFIGr             , HBM16_SINGLE_PORT_AQ_DRIVE_STRENGTHf  , HBMC_SHMOO_LEVEL_2                        ,   "AWORD_IO_CONFIG__SINGLE_PORT_AQ_DRIVE_STRENGTH"},  
    {HBM16_AWORD_LANE_REMAPr            , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_0                        ,   "AWORD_LANE_REMAP__FIELDS_ALL"},
    {HBM16_AWORD_LANE_REMAPr            , HBM16_SEL_CAf                         , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_AWORD_LANE_REMAPr            , HBM16_SEL_RAf                         , HBMC_SHMOO_OTP_VER_1_2                    ,   NULL},
    {HBM16_AWORD_DAC_CONFIGr            , HBM16_DAC_DATAf                       , HBMC_SHMOO_LEVEL_0 | HBMC_SHMOO_OTP_VER_1_2 ,   "AWORD_DAC_CONFIG__DAC_DATA"},
    {HBM16_AWORD_LATENCY_SHIFTERr       , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_2                        ,   "AWORD_LATENCY_SHIFTER__FIELDS_ALL"},
    {HBM16_AWORD_REG_RESERVEDr          , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        ,   "AWORD_REG_RESERVED__FIELDS_ALL"},
    {HBM16_CONTROL_REG_RESERVEDr        , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        ,   "CONTROL_REG_RESERVED__FIELDS_ALL"}
};

/* const array which hold the shmoo tune metadata per midstack */
const hbmc_shmoo_midstack_config_param_t midstack_config_param_metadata [HBMC_SHMOO_MIDSTACK_METADATA_SIZE] = 
{   
    { HBM16_MIDSTACK_DAC_CONFIGr        , HBM16_DAC_DATAf                       , HBMC_SHMOO_LEVEL_0 | HBMC_SHMOO_OTP_VER_1_2 , "MIDSTACK_DAC_CONFIG__DAC_DATA"},
    { HBM16_MIDSTACK_IO_CONFIGr         , HBM16_RESET_DRIVE_STRENGTHf           , HBMC_SHMOO_LEVEL_2                        , "MIDSTACK_IO_CONFIG__RESET_DRIVE_STRENGTH"},
    { HBM16_MIDSTACK_REG_RESERVEDr      , HBMC_SHMOO_FIELDS_ALL                 , HBMC_SHMOO_LEVEL_1                        , "MIDSTACK_REG_RESERVED__FIELDS_ALL"}
};


/* BEGIN: HELPER FUNCTIONS */
STATIC uint32
_hbm16_get_random(void)
{
    static uint32 m_w = 6483;       /* must not be zero */
    static uint32 m_z = 31245;      /* must not be zero */
    
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;       /* 32-bit result */
}
/* END: HELPER FUNCTIONS */

uint32
_shmoo_hbm16_check_dram(int unit, uint32 hbm_ndx)
{
    hbmc_shmoo_dram_info_t shmoo_dram_info;
    SOC_IF_ERROR_RETURN( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));
    return ((shmoo_dram_info.dram_bitmap >> hbm_ndx) & 0x1);
}


uint32
_shmoo_hbm16_validate_config(int unit)
{
    uint32 ndx;
    hbmc_shmoo_dram_info_t shmoo_dram_info;
    SOC_IF_ERROR_RETURN( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));

    for(ndx = 0; ndx < SHMOO_HBM16_MAX_INTERFACES; ndx++)
    {
        if(!_shmoo_hbm16_check_dram(unit, ndx))
        {
            continue;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "shmo_dram_info: ctl_type=%d, dram_type=%d, num_columns=%d, num_rows=%d, num_banks=%d, data_rate_mbps=%d, ref_clk_mhz=%d, dram_bitmap=0x%08X\n"),
                                            shmoo_dram_info.ctl_type, shmoo_dram_info.dram_type, shmoo_dram_info.num_columns, shmoo_dram_info.num_rows,
                                            shmoo_dram_info.num_banks, shmoo_dram_info.data_rate_mbps, shmoo_dram_info.ref_clk_mhz, shmoo_dram_info.dram_bitmap));
    
    return  (
                (shmoo_dram_info.dram_type != 0)
                && (shmoo_dram_info.num_columns != -1)
                && (shmoo_dram_info.num_rows != -1)
                && (shmoo_dram_info.num_banks != -1)
                && (shmoo_dram_info.data_rate_mbps != -1)
                && (shmoo_dram_info.ref_clk_mhz != -1)
            /*  && (shmoo_dram_info.ctl_type >= 0) */
            /*  && (shmoo_dram_info.refi != 0) */
            /*  && (shmoo_dram_info.command_parity_latency != 0) */
            /*  && (shmoo_dram_info.sim_system_mode != 0) */
            );
}

uint32
_shmoo_hbm16_validate_cbi(void)
{
    return  (
                (soc_hbm16_phy_reg_read != NULL)
                && (soc_hbm16_phy_reg_write != NULL)
                && (soc_hbm16_phy_reg_modify != NULL)
                && (_shmoo_hbm16_hbmc_bist_conf_set != NULL)
                && (_shmoo_hbm16_hbmc_bist_err_cnt != NULL)
                && (_shmoo_hbm16_hbmc_dram_init != NULL)
                && (_shmoo_hbm16_hbmc_pll_set != NULL)
                && (_shmoo_hbm16_hbmc_modify_mrs != NULL)
                && (_shmoo_hbm16_hbmc_enable_wr_parity != NULL)
                && (_shmoo_hbm16_hbmc_enable_rd_parity != NULL)
                && (_shmoo_hbm16_hbmc_enable_addr_parity != NULL)
                && (_shmoo_hbm16_hbmc_enable_wr_dbi != NULL)
                && (_shmoo_hbm16_hbmc_enable_rd_dbi != NULL)
                && (_shmoo_hbm16_hbmc_enable_refresh != NULL)
                && (_shmoo_hbm16_hbmc_soft_reset_controller_without_dram != NULL)
                && (_shmoo_hbm16_hbmc_dram_info_access != NULL)
                && (_shmoo_hbm16_hbmc_bist_status_get != NULL)
                && (_shmoo_hbm16_hbmc_shmoo_model_part_get != NULL)
                && (_shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check != NULL)
            );
}

STATIC int
_hbm16_initialize_bist(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr, hbmc_shmoo_bist_info_t *biPtr)
{
    uint32 i;
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
        case SHMOO_HBM16_RD_EXTENDED:
        case SHMOO_HBM16_WR_EXTENDED:
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            (*biPtr).write_weight = 512;
            (*biPtr).read_weight = 512;
            (*biPtr).bist_timer_us = 0;
            (*biPtr).bist_num_actions[0] = 1024*128;
            (*biPtr).bist_num_actions[1] = 0;
            (*biPtr).row_start_index = 0x00000000;
            (*biPtr).column_start_index = 0x00000000;
            (*biPtr).bank_start_index = 0x00000000;
            (*biPtr).row_end_index = 0x00003FFF;
            (*biPtr).column_end_index = 0x0000001F;
            (*biPtr).bank_end_index = 0x0000001F;
            (*biPtr).bist_refresh_enable = 0;
            (*biPtr).bist_ignore_address = 0;
            (*biPtr).same_row_commands = 8;
            (*biPtr).data_mode = 0;
            for(i = 0; i < HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS; i++)
            {
                (*biPtr).prbs_seeds[i] = _hbm16_get_random() & 0xFFFFF;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_hbm16_run_bist(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr, hbmc_shmoo_bist_info_t *biPtr, hbmc_shmoo_error_array_t *seaPtr)
{
    uint32 data[1];
    uint32 status_rdata_fifo_ptr[4] = {0};
    uint32 p0, p1, p2, p3;
    hbmc_shmoo_bist_err_cnt_t be;
    uint32 bist_cb_flags = 0;
    uint32 write_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
    uint32 read_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
    uint32 read_data_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
    

    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
        case SHMOO_HBM16_RD_EXTENDED:
        case SHMOO_HBM16_WR_EXTENDED:
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            if (scPtr->customBistMode)
            {
                /** run custom BIST defined by a user CB instead of conventional DRAN Bist data errors, can be used for example to scan with traffic instead of DRAM Bist*/
                SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_custom_bist_run(unit, hbm_ndx, channel, biPtr, seaPtr));
            }
            else
            {
                (*biPtr).bank_start_index = 0x00000000;
                (*biPtr).bank_end_index = 0x0000001F;

                /** set bist CB flags for actions like loopback and such */
                if(scPtr->read_lfsr_loopback)
                {
                    bist_cb_flags |= HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_READ_LFSR;
                }
                else if (scPtr->write_lfsr_loopback)
                {
                    bist_cb_flags |= HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_WRITE_LFSR;
                }

                SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_bist_conf_set(unit, bist_cb_flags, hbm_ndx, channel, biPtr));
                SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_bist_err_cnt(unit, bist_cb_flags, hbm_ndx, channel, &be));
                SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_bist_status_get(unit, hbm_ndx, channel, read_cmd_counter, write_cmd_counter, read_data_counter));

                SHR_BITCOPY_RANGE(data, 0, be.bist_data_err_bits, 0, 32);
                (*seaPtr)[0] = data[0];
                p0 = data[0];
                SHR_BITCOPY_RANGE(data, 0, be.bist_data_err_bits, 32, 32);
                (*seaPtr)[1] = data[0];
                p1 = data[0];
                SHR_BITCOPY_RANGE(data, 0, be.bist_data_err_bits, 64, 32);
                (*seaPtr)[2] = data[0];
                p2 = data[0];
                SHR_BITCOPY_RANGE(data, 0, be.bist_data_err_bits, 96, 32);
                (*seaPtr)[3] = data[0];
                p3 = data[0];
                sal_usleep(SHMOO_HBM16_SHORT_SLEEP);

                SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_STATUS_RDATA_FIFO_PTRr(unit, hbm_ndx, channel, &status_rdata_fifo_ptr[0]));
                SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_STATUS_RDATA_FIFO_PTRr(unit, hbm_ndx, channel, &status_rdata_fifo_ptr[1]));
                SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_STATUS_RDATA_FIFO_PTRr(unit, hbm_ndx, channel, &status_rdata_fifo_ptr[2]));
                SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_STATUS_RDATA_FIFO_PTRr(unit, hbm_ndx, channel, &status_rdata_fifo_ptr[3]));

                LOG_VERBOSE(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,"      Raw 0: 0x %08X %08X %08X %08X | STATUS_RDATA_FIFO_PTR: 0x %08X %08X %08X %08X | READ_DATA_CNT: 0x%08X%08X\n"),
                                    p3, p2, p1, p0, status_rdata_fifo_ptr[3], status_rdata_fifo_ptr[2], status_rdata_fifo_ptr[1], status_rdata_fifo_ptr[0], read_data_counter[1], read_data_counter[0]));
            }
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_hbm16_calculate_ui_position(uint32 linearPos, hbm16_shmoo_container_t *scPtr, hbm16_ui_position_t *upPtr)
{
    uint32 ui;
    uint32 position;
    
    position = linearPos;
    
    for(ui = 0; ui < SHMOO_HBM16_MAX_EFFECTIVE_UI_COUNT; ui++)
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
    
    if(ui == SHMOO_HBM16_MAX_EFFECTIVE_UI_COUNT)
    {
        ui--;
        position = linearPos - ((*scPtr).endUI[ui] + 1);
    }
    
    (*upPtr).ui = ui;
    (*upPtr).position = position;
    
    return SOC_E_NONE;
}

/** try to align dword by trying to fix rcmd latency
 * change the field value of the input_dword to its value -1, set the value of the field in the other dwords to the input dword value
 */
static int
hbm16_phy_channel_dword_align(int unit, int hbm_ndx, int channel, int input_dword)
{
    uint32 reg32_val;
    int curr_dword;
    uint32 read_command_additonal_latency;
    soc_reg_t DWORD_LATENCY_SHIFTERl[SHMOO_HBM16_WORDS_PER_CHANNEL] = { HBM16_DWORD0_LATENCY_SHIFTERr, HBM16_DWORD1_LATENCY_SHIFTERr,
                                                                        HBM16_DWORD2_LATENCY_SHIFTERr, HBM16_DWORD3_LATENCY_SHIFTERr};
    if (input_dword >= SHMOO_HBM16_WORDS_PER_CHANNEL)
    {
        return SOC_E_FAIL;
    }

    /** read rcmd_latency of input dword */
    SOC_IF_ERROR_RETURN(soc_hbm16_channel_read(unit, hbm_ndx, channel, DWORD_LATENCY_SHIFTERl[input_dword], REG_PORT_ANY, &reg32_val));
    /** get value of RCMD_ADDITIONAL_LATENCYf */
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_get(unit, DWORD_LATENCY_SHIFTERl[input_dword], reg32_val, HBM16_RCMD_ADDITIONAL_LATENCYf, &read_command_additonal_latency));

    /** write read field value to all dwords */
    for(curr_dword = 0; curr_dword < SHMOO_HBM16_WORDS_PER_CHANNEL; ++curr_dword)
    {
        SOC_IF_ERROR_RETURN(soc_hbm16_channel_read(unit, hbm_ndx, channel, DWORD_LATENCY_SHIFTERl[curr_dword], REG_PORT_ANY, &reg32_val));
        SOC_IF_ERROR_RETURN(soc_hbm16_channel_field_set(unit, DWORD_LATENCY_SHIFTERl[curr_dword], &reg32_val, HBM16_RCMD_ADDITIONAL_LATENCYf, read_command_additonal_latency));
        SOC_IF_ERROR_RETURN(soc_hbm16_channel_write(unit, hbm_ndx, channel, DWORD_LATENCY_SHIFTERl[curr_dword], REG_PORT_ANY, reg32_val));
    }

    /** write read value - 1 to input dword */
    SOC_IF_ERROR_RETURN(soc_hbm16_channel_read(unit, hbm_ndx, channel, DWORD_LATENCY_SHIFTERl[input_dword], REG_PORT_ANY, &reg32_val));
    SOC_IF_ERROR_RETURN(soc_hbm16_channel_field_set(unit, DWORD_LATENCY_SHIFTERl[input_dword], &reg32_val, HBM16_RCMD_ADDITIONAL_LATENCYf, read_command_additonal_latency - 1));
    SOC_IF_ERROR_RETURN(soc_hbm16_channel_write(unit, hbm_ndx, channel, DWORD_LATENCY_SHIFTERl[input_dword], REG_PORT_ANY, reg32_val));
    LOG_VERBOSE(BSL_LS_SOC_DDR, (BSL_META_U(unit, "DWORD%d - RCMD_ADDITIONAL_LATENCY = %d\n"), input_dword, read_command_additonal_latency - 1));

    return SOC_E_NONE;
}

int
_hbm16_phy_channel_reset(int unit, int flags, int hbm_ndx, int channel)
{
    uint32 data;

    SOC_IF_ERROR_RETURN(READ_HBM16_CONTROL_RESETSr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_CONTROL_RESETSr, &data, HBM16_PHY_RSTB_1Gf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_CONTROL_RESETSr(unit, hbm_ndx, channel, data));

    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));

    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN(READ_HBM16_CONTROL_RESETSr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_CONTROL_RESETSr, &data, HBM16_PHY_RSTB_1Gf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_CONTROL_RESETSr(unit, hbm_ndx, channel, data));
    
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, hbm_ndx, channel, data));
    
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_BV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_BV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
    
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);

    /** check if need to fix sync problem */
    if ((flags & SHMOO_HBM16_PHY_RESET_FLAG_DWORD_ALIGNMENT_NOT_NEEDED) == 0)
    {
        int is_aligned = 0;
        SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check(unit, hbm_ndx, channel, &is_aligned));
        if (!is_aligned)
        {
            int dword_index;
            for(dword_index = 0; dword_index < SHMOO_HBM16_WORDS_PER_CHANNEL; ++dword_index)
            {
                /** try to fix rcmd latency */
                SOC_IF_ERROR_RETURN(hbm16_phy_channel_dword_align(unit, hbm_ndx, channel, dword_index));
                /** reset hbc */
                SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_soft_reset_controller_without_dram(unit, hbm_ndx, channel));
                /** re-check alignment and stop if aligned */
                SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check(unit, hbm_ndx, channel, &is_aligned));
                if (is_aligned)
                {
                    break;
                }
            }

            if(dword_index == SHMOO_HBM16_WORDS_PER_CHANNEL)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "A Sync problem was detected and couldn't be fixed\n")));
                return SOC_E_FAIL;
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_rd_en_fish(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    uint32 d;
    uint32 fish0, fish1, fish2, fish3;
    uint32 data;
    hbmc_shmoo_bist_info_t bi;
    hbmc_shmoo_error_array_t sea;
    
    fish0 = 1;
    fish1 = 1;
    fish2 = 1;
    fish3 = 1;
    
    (*scPtr).engageUIshift = 0;
    (*scPtr).shmooType = SHMOO_HBM16_RD_EN_FISH;
    
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "\n\n")));
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "***** Interface.......: %3d\n"),
                 hbm_ndx));
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "***** Channel.........: %3d\n"),
                 channel));
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            " **** Shmoo type......: RD_EN_FISH\n")));
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "  *** Searching.......: RD_DATA_DELAY\n")));
    
    _hbm16_initialize_bist(unit, hbm_ndx, channel, scPtr, &bi);
    
    for(d = 0; d < 16; d++)
    {        
        if(fish0)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RD_DATA_DELAYf, d));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
        }
        if(fish1)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RD_DATA_DELAYf, d));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
        }
        if(fish2)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RD_DATA_DELAYf, d));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
        }
        if(fish3)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RD_DATA_DELAYf, d));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
        }
            
        _hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel);
        
        _hbm16_run_bist(unit, hbm_ndx, channel, scPtr, &bi, &sea);
        if(!sea[0])
        {
            fish0 = 0;
        }
        if(!sea[1])
        {
            fish1 = 0;
        }
        if(!sea[2])
        {
            fish2 = 0;
        }
        if(!sea[3])
        {
            fish3 = 0;
        }
        
        LOG_INFO(BSL_LS_SOC_DDR,
                    (BSL_META_U(unit,
                                "   ** Delay %02d........: %01d %01d %01d %01d\n"),
                     d, fish3, fish2, fish1, fish0));
        
        if(!(fish0 || fish1 || fish2 || fish3))
        {
            break;
        }
    }
    
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "\n")));
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_wr_extended(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
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
    hbmc_shmoo_bist_info_t bi;
    hbmc_shmoo_error_array_t sea;
    
    xStart = 0;
    
    if((*scPtr).debugMode & SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
    {
        yCapMin = 0;
        yCapMax = 8;
        sizeY = 8;
    }
    else
    {
        yCapMin = 3;
        yCapMax = 4;
        sizeY = 1;
    }

    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[0] + 1;
    (*scPtr).sizeY = sizeY;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_HBM16_WR_EXTENDED;
    
    _hbm16_initialize_bist(unit, hbm_ndx, channel, scPtr, &bi);
    
    for(y = yCapMin; y < yCapMax; y++)
    {
        SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_modify_mrs(unit, hbm_ndx, channel, 15, shmoo_hbm16_gen2_wr_vref[y], 0x7));
        
        sal_usleep(SHMOO_HBM16_SHORT_SLEEP);

        position = 0;
        ui = 0;

        for(x = 0; x < (*scPtr).sizeX; x++)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, &data));
            if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_HBM16_LAST_EFFECTIVE_UI))
            {
                ui++;
                position = 0;
            }
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, ui));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, position));
            position++;
            
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
            
            _hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel);

            _hbm16_run_bist(unit, hbm_ndx, channel, scPtr, &bi, &sea);
            
            (*scPtr).result2D[0][x + xStart] = sea[0];
            (*scPtr).result2D[1][x + xStart] = sea[1];
            (*scPtr).result2D[2][x + xStart] = sea[2];
            (*scPtr).result2D[3][x + xStart] = sea[3];
        }
        
        xStart += (*scPtr).sizeX;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_rd_extended(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 y;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 sizeY;
    uint32 xStart;
    uint32 data;
    hbmc_shmoo_bist_info_t bi;
    hbmc_shmoo_error_array_t sea;
    
    xStart = 0;
    
    if((*scPtr).debugMode & SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
    {
        yCapMin = 0;
        yCapMax = 8;
        sizeY = 8;
    }
    else
    {
        hbmc_shmoo_hbm_model_part_t model_part;
        SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_shmoo_model_part_get(unit, hbm_ndx, &model_part));
        if(model_part == HBM_TYPE_SAMSUNG_DIE_B)
        {
            yCapMin = 2;
            yCapMax = 3;
        }
        else if(model_part == HBM_TYPE_SAMSUNG_DIE_X)
        {
            yCapMin = 4;
            yCapMax = 5;
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Invalid HBM model part detected\n")));
            return SOC_E_FAIL;
        }
        sizeY = 1;
    }

    (*scPtr).engageUIshift = 0;
    (*scPtr).sizeX = (*scPtr).endUI[0] + 1;
    (*scPtr).sizeY = sizeY;
    (*scPtr).yCapMin = yCapMin;
    (*scPtr).yCapMax = yCapMax;
    (*scPtr).shmooType = SHMOO_HBM16_RD_EXTENDED;
    
    _hbm16_initialize_bist(unit, hbm_ndx, channel, scPtr, &bi);

    for(y = yCapMin; y < yCapMax; y++)
    {
        SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, &data));
        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, y << 1));
        
        /** configure DWORD according to bitmap that indicates which needed to be changed */
        if(scPtr->dwordsBitmap & 0x1)
        {
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, data));
        }
        if(scPtr->dwordsBitmap & 0x2)
        {
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, hbm_ndx, channel, data));
        }
        if(scPtr->dwordsBitmap & 0x4)
        {
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, hbm_ndx, channel, data));
        }
        if(scPtr->dwordsBitmap & 0x8)
        {
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, hbm_ndx, channel, data));
        }
        
        sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
        
        for(x = (*scPtr).calibStart; x < (*scPtr).sizeX; x++)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, x));
            
            /** configure DWORD according to bitmap that indicates which needed to be changed */
            if(scPtr->dwordsBitmap & 0x1)
            {
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
            }
            if(scPtr->dwordsBitmap & 0x2)
            {
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
            }
            if(scPtr->dwordsBitmap & 0x4)
            {
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
            }
            if(scPtr->dwordsBitmap & 0x8)
            {
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
            }
            
            _hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel);
            
            _hbm16_run_bist(unit, hbm_ndx, channel, scPtr, &bi, &sea);

            /** check bist errors according to dwords bitmap, if dword is not in bitmap mark no errors */
            if(scPtr->dwordsBitmap & 0x1)
            {
                (*scPtr).result2D[0][x + xStart] = sea[0];
            }
            else
            {
                (*scPtr).result2D[0][x + xStart] = 0;
            }
            if(scPtr->dwordsBitmap & 0x2)
            {
                (*scPtr).result2D[1][x + xStart] = sea[1];
            }
            else
            {
                (*scPtr).result2D[1][x + xStart] = 0;
            }
            if(scPtr->dwordsBitmap & 0x4)
            {
                (*scPtr).result2D[2][x + xStart] = sea[2];
            }
            else
            {
                (*scPtr).result2D[2][x + xStart] = 0;
            }
            if(scPtr->dwordsBitmap & 0x8)
            {
                (*scPtr).result2D[3][x + xStart] = sea[3];
            }
            else
            {
                (*scPtr).result2D[3][x + xStart] = 0;
            }
        }
        
        xStart += (*scPtr).sizeX;
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_addr_ctrl_extended(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    uint32 x;
    uint32 ui;
    uint32 position;
    uint32 data;
    hbmc_shmoo_bist_info_t bi;
    hbmc_shmoo_error_array_t sea;

    (*scPtr).engageUIshift = 1;
    (*scPtr).sizeX = (*scPtr).endUI[0] + 1;
    (*scPtr).sizeY = 1;
    (*scPtr).yCapMin = 0;
    (*scPtr).yCapMax = 1;
    (*scPtr).shmooType = SHMOO_HBM16_ADDR_CTRL_EXTENDED;
    
    _hbm16_initialize_bist(unit, hbm_ndx, channel, scPtr, &bi);
    
    position = 0;
    ui = 0;

    for(x = 0; x < (*scPtr).sizeX; x++)
    {
        SOC_IF_ERROR_RETURN(READ_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, hbm_ndx, channel, &data));
        if((x > (*scPtr).endUI[ui]) && (ui < SHMOO_HBM16_LAST_EFFECTIVE_UI))
        {
            ui++;
            position = 0;
        }
        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_UI_SHIFTf, ui));
        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_MAX_VDL_STEPf, position));
        position++;
        
        SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, hbm_ndx, channel, data));
        
        sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
        
        _hbm16_run_bist(unit, hbm_ndx, channel, scPtr, &bi, &sea);
        
        (*scPtr).result2D[0][x] = sea[0];
        (*scPtr).result2D[1][x] = sea[1];
        (*scPtr).result2D[2][x] = sea[2];
        (*scPtr).result2D[3][x] = sea[3];
        
        if((*scPtr).result2D[0][x] || (*scPtr).result2D[1][x] || (*scPtr).result2D[2][x] || (*scPtr).result2D[3][x])
        {
            SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_dram_init(unit, hbm_ndx, 0));

            SOC_IF_ERROR_RETURN(_hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel));
            
            sal_usleep(SHMOO_HBM16_DEEP_SLEEP);
        }
    }
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_do(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
            return _shmoo_hbm16_rd_en_fish(unit, hbm_ndx, channel, scPtr);
        case SHMOO_HBM16_RD_EXTENDED:
            return _shmoo_hbm16_rd_extended(unit, hbm_ndx, channel, scPtr);
        case SHMOO_HBM16_WR_EXTENDED:
            return _shmoo_hbm16_wr_extended(unit, hbm_ndx, channel, scPtr);
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            return _shmoo_hbm16_addr_ctrl_extended(unit, hbm_ndx, channel, scPtr);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported shmoo type: %02d\n"),
                       (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_hbm16_calib_2D(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr, uint32 calibMode, uint32 calibPos, uint32 decisionMode)
{
    uint32 x;
    uint32 y;
    uint32 xStart;
    uint32 sizeX;
    uint32 calibStart[SHMOO_HBM16_WORDS_PER_CHANNEL];
    uint32 calibEnd[SHMOO_HBM16_WORDS_PER_CHANNEL];
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 i, j;
    uint32 shiftAmount;
    int32 passStart;
    int32 failStart;
    int32 passStartSeen;
    int32 failStartSeen;
    int32 passLength;
    int32 maxPassStart;
    int32 maxPassLength;
    int32 maxMidPointX;
    uint32 maxPassLengthArray[SHMOO_HBM16_WORDS_PER_CHANNEL];
    uint32 result;
    uint32 iter = 0;
    uint32 dataMask = 0xFFFFFFFF;

    xStart = 0;
    sizeX = (*scPtr).sizeX;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;
    
    for(i = 0; i < SHMOO_HBM16_WORDS_PER_CHANNEL; i++)
    {
        calibStart[i] = (*scPtr).calibStart;
        calibEnd[i] = sizeX;
    }

    switch(calibMode)
    {
        case SHMOO_HBM16_WORD:
            iter = 4;
            shiftAmount = 0;
            break;
        case SHMOO_HBM16_DOUBLEWORD:
            iter = 2;
            shiftAmount = 1;
            break;
        case SHMOO_HBM16_QUADWORD:
            iter = 1;
            shiftAmount = 2;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported 2D calibration mode: %02d\n"),
                       calibMode));
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
            for(x = calibStart[i]; x < calibEnd[i]; x++)
            {                
                switch(decisionMode)
                {
                    case SHMOO_HBM16_CALIB_PASS_ALL:
                        result = (*scPtr).result2D[i << shiftAmount][xStart + x];
                        for(j = 1; j < shiftAmount; j++)
                        {
                            result |= (*scPtr).result2D[(i << shiftAmount) + j][xStart + x];
                        }
                        break;
                    case SHMOO_HBM16_CALIB_PASS_ANY:
                        result = ((*scPtr).result2D[i << shiftAmount][xStart + x] == dataMask);
                        for(j = 1; j < shiftAmount; j++)
                        {
                            result &= ((*scPtr).result2D[i << shiftAmount][xStart + x] == dataMask);
                        }
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR,
                                  (BSL_META_U(unit,
                                              "Unsupported calibration decision mode: %02d\n"),
                                   decisionMode));
                        return SOC_E_FAIL;
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
                case SHMOO_HBM16_CALIB_FAIL_START:
                case SHMOO_HBM16_CALIB_RISING_EDGE:
                    if(failStartSeen > 0)
                    {
                        maxMidPointX = failStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_HBM16_CALIB_PASS_START:
                case SHMOO_HBM16_CALIB_FALLING_EDGE:
                    if(passStartSeen > 0)
                    {
                        maxMidPointX = passStartSeen;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                    }
                    break;
                case SHMOO_HBM16_CALIB_CENTER_PASS:
                    if((maxPassLength > 0) && (maxPassLengthArray[i] < maxPassLength))
                    {
                        maxMidPointX = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        (*scPtr).resultData[i] = (y << 16) | (maxMidPointX & 0xFFFF);
                        maxPassLengthArray[i] = maxPassLength;
                    }
                    break;
                case SHMOO_HBM16_CALIB_ANY_EDGE:
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration position: %02d\n"),
                               calibPos));
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
_shmoo_hbm16_calib_2D(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
            break;
        case SHMOO_HBM16_RD_EXTENDED:
            return _hbm16_calib_2D(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_WORD, SHMOO_HBM16_CALIB_CENTER_PASS, SHMOO_HBM16_CALIB_PASS_ALL);
        case SHMOO_HBM16_WR_EXTENDED:
            return _hbm16_calib_2D(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_WORD, SHMOO_HBM16_CALIB_CENTER_PASS, SHMOO_HBM16_CALIB_PASS_ALL);
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            return _hbm16_calib_2D(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_QUADWORD, SHMOO_HBM16_CALIB_CENTER_PASS, SHMOO_HBM16_CALIB_PASS_ALL);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported shmoo type: %02d\n"),
                       (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_set_new_step(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    uint32 calibMode;
    /* uint32 dramType; */
    uint32 engageUIshift;
    int MRfield0, MRfield1, MRfield2, MRfield3;
    uint32 data;
    hbm16_ui_position_t up;
    hbmc_shmoo_dram_info_t shmoo_dram_info;

    SOC_IF_ERROR_RETURN( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));    
        
    calibMode = (*scPtr).calibMode;
    /* dramType = (*scPtr).dramType; */
    engageUIshift = (*scPtr).engageUIshift;
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
            break;
        case SHMOO_HBM16_RD_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, &data));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[0] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[1] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[2] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[3] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, &data));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[1] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[2] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[3] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, &data));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[0] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[1] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, &data));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[1] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    break;
                case SHMOO_HBM16_QUADWORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, &data));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[0] & 0xFFFF));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, hbm_ndx, channel, data));

                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, &data));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, (((*scPtr).resultData[0] >> 16) & 0xFFFF) << 1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, hbm_ndx, channel, data));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"),
                               (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            
            _hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel);
            break;
        case SHMOO_HBM16_WR_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, &data));
                    if(engageUIshift)
                    {
                        _hbm16_calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        _hbm16_calculate_ui_position((*scPtr).resultData[1] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        _hbm16_calculate_ui_position((*scPtr).resultData[2] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        _hbm16_calculate_ui_position((*scPtr).resultData[3] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                    }
                    else
                    {
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[0] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[1] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[2] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[3] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                    }
                    
                    MRfield0 = ((int) (((*scPtr).resultData[0] >> 16) & 0xFFFF));
                    MRfield1 = ((int) (((*scPtr).resultData[1] >> 16) & 0xFFFF));
                    MRfield2 = ((int) (((*scPtr).resultData[2] >> 16) & 0xFFFF));
                    MRfield3 = ((int) (((*scPtr).resultData[3] >> 16) & 0xFFFF));
                    
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield1));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield2));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield3));
                    
                    MRfield0 = (MRfield0 + MRfield1 + MRfield2 + MRfield3) >> 2;
                    
                    SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_modify_mrs(unit, hbm_ndx, channel, 15, shmoo_hbm16_gen2_wr_vref[MRfield0], 0x7));
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, &data));
                    if(engageUIshift)
                    {
                        _hbm16_calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        _hbm16_calculate_ui_position((*scPtr).resultData[1] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                    }
                    else
                    {
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[0] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[1] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                    }
                    
                    MRfield0 = ((int) (((*scPtr).resultData[0] >> 16) & 0xFFFF));
                    MRfield2 = ((int) (((*scPtr).resultData[1] >> 16) & 0xFFFF));
                    
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield2));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield2));
                    
                    MRfield0 = (MRfield0 + MRfield2) >> 1;
                    
                    SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_modify_mrs(unit, hbm_ndx, channel, 15, shmoo_hbm16_gen2_wr_vref[MRfield0], 0x7));
                    break;
                case SHMOO_HBM16_QUADWORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, &data));
                    if(engageUIshift)
                    {
                        _hbm16_calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, up.position));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                    }
                    else
                    {
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, (*scPtr).resultData[0] & 0xFFFF));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                        SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbm_ndx, channel, data));
                    }
                    
                    MRfield0 = ((int) (((*scPtr).resultData[0] >> 16) & 0xFFFF));
                    
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_REG_RESERVEDr(unit, hbm_ndx, channel, MRfield0));
                    
                    SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_modify_mrs(unit, hbm_ndx, channel, 15, shmoo_hbm16_gen2_wr_vref[MRfield0], 0x7));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"),
                               (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            break;
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            switch(calibMode)
            {
                case SHMOO_HBM16_QUADWORD:
                    SOC_IF_ERROR_RETURN(READ_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, hbm_ndx, channel, &data));
                    if(engageUIshift)
                    {
                        _hbm16_calculate_ui_position((*scPtr).resultData[0] & 0xFFFF, scPtr, &up);
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_UI_SHIFTf, up.ui));
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_MAX_VDL_STEPf, up.position));
                    }
                    else
                    {
                        SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_UI_SHIFTf, (*scPtr).resultData[0] & 0xFFFF));
                    }
                    SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, hbm_ndx, channel, data));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported shmoo type and calibration mode combination during set new step: %02d / %02d\n"),
                               (*scPtr).shmooType, calibMode));
                    return SOC_E_FAIL;
            }
            
            SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_dram_init(unit, hbm_ndx, 0));

            SOC_IF_ERROR_RETURN(_hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel));
            
            sal_usleep(SHMOO_HBM16_DEEP_SLEEP);
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported shmoo type: %02d\n"),
                       (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_hbm16_plot(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr, uint32 plotMode)
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
    uint32 wordIter;
    uint32 shiftAmount;
    uint32 dataMask;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    /* uint32 wckInv; */
    uint32 engageUIshift;
    uint32 step1000;
    uint32 size1000UI;
    uint32 calibShiftAmount;
    uint32 maxMidPointX;
    uint32 maxMidPointY;
    uint32 result;
    char *str0 = NULL;
    char *str1 = NULL;
    char *str2 = NULL;
    int rv = SOC_E_NONE;
    char pass_low[2];
    char fail_high[2];
    char outOfSearch[2];
    
    outOfSearch[0] = ' ';
    outOfSearch[1] = 0;

    sizeX = (*scPtr).sizeX;
    sizeY = (*scPtr).sizeY;
    yCapMin = (*scPtr).yCapMin;
    yCapMax = (*scPtr).yCapMax;
    calibMode = (*scPtr).calibMode;
    calibPos = (*scPtr).calibPos;
    calibStart = (*scPtr).calibStart;
    /* wckInv = (*scPtr).wckInv; */
    engageUIshift = (*scPtr).engageUIshift;
    step1000 = (*scPtr).step1000;
    size1000UI = (*scPtr).size1000UI;

    switch(calibPos)
    {
        case SHMOO_HBM16_CALIB_ANY_EDGE:
        case SHMOO_HBM16_CALIB_RISING_EDGE:
        case SHMOO_HBM16_CALIB_FALLING_EDGE:
            pass_low[0] = '_';
            pass_low[1] = 0;
            fail_high[0] = '|';
            fail_high[1] = 0;
            break;
        case SHMOO_HBM16_CALIB_CENTER_PASS:
        case SHMOO_HBM16_CALIB_PASS_START:
        case SHMOO_HBM16_CALIB_FAIL_START:
            pass_low[0] = '+';
            pass_low[1] = 0;
            fail_high[0] = '-';
            fail_high[1] = 0;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported calibration position: %02d\n"),
                       calibPos));
            return SOC_E_FAIL;
    }

    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "\n\n")));

    switch(plotMode)
    {
        case SHMOO_HBM16_BIT:
            iter = 32;
            wordIter = 4;
            shiftAmount = 0;
            dataMask = 0x1;
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    calibShiftAmount = 5;
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    calibShiftAmount = 6;
                    break;
                case SHMOO_HBM16_QUADWORD:
                    calibShiftAmount = 7;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_HBM16_BYTE:
            iter = 4;
            wordIter = 4;
            shiftAmount = 3;
            dataMask = 0xFF;
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    calibShiftAmount = 2;
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    calibShiftAmount = 3;
                    break;
                case SHMOO_HBM16_QUADWORD:
                    calibShiftAmount = 4;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_HBM16_HALFWORD:
            iter = 2;
            wordIter = 4;
            shiftAmount = 4;
            dataMask = 0xFFFF;
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    calibShiftAmount = 2;
                    break;
                case SHMOO_HBM16_QUADWORD:
                    calibShiftAmount = 3;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_HBM16_WORD:
            iter = 1;
            wordIter = 4;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    calibShiftAmount = 1;
                    break;
                case SHMOO_HBM16_QUADWORD:
                    calibShiftAmount = 2;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_HBM16_DOUBLEWORD:
            iter = 1;
            wordIter = 2;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "WARNING: Plot mode coerced from doubleword mode to word mode\n")));
                    iter = 1;
                    wordIter = 4;
                    shiftAmount = 5;
                    dataMask = 0xFFFFFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    calibShiftAmount = 0;
                    break;
                case SHMOO_HBM16_QUADWORD:
                    calibShiftAmount = 1;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    return SOC_E_FAIL;
            }
            break;
        case SHMOO_HBM16_QUADWORD:
            iter = 1;
            wordIter = 1;
            shiftAmount = 5;
            dataMask = 0xFFFFFFFF;
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "WARNING: Plot mode coerced from quadword mode to word mode\n")));
                    iter = 1;
                    wordIter = 4;
                    shiftAmount = 5;
                    dataMask = 0xFFFFFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "WARNING: Plot mode coerced from quadword mode to doubleword mode\n")));
                    iter = 1;
                    wordIter = 2;
                    shiftAmount = 5;
                    dataMask = 0xFFFFFFFF;
                    calibShiftAmount = 0;
                    break;
                case SHMOO_HBM16_QUADWORD:
                    calibShiftAmount = 0;
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    return SOC_E_FAIL;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported plot mode: %02d\n"),
                       plotMode));
            return SOC_E_FAIL;
    }
/*    
    if(engageUIshift)
    { */
    str0 = sal_alloc(sizeof(char) * SHMOO_HBM16_STRING_LENGTH, "shmoo hbm16 str0");
    if (NULL == str0) {
        return SOC_E_MEMORY;
    }
    str1 = sal_alloc(sizeof(char) * SHMOO_HBM16_STRING_LENGTH, "shmoo hbm16 str1");
    if (NULL == str1) {
        sal_free(str0);
        return SOC_E_MEMORY;
    }
    str2 = sal_alloc(sizeof(char) * SHMOO_HBM16_STRING_LENGTH, "shmoo hbm16 str2");
    if (NULL == str2) {
        sal_free(str0);
        sal_free(str1);
        return SOC_E_MEMORY;
    }

    sal_memset(str0, 0, sizeof(char) * SHMOO_HBM16_STRING_LENGTH);
    sal_memset(str1, 0, sizeof(char) * SHMOO_HBM16_STRING_LENGTH);
    sal_memset(str2, 0, sizeof(char) * SHMOO_HBM16_STRING_LENGTH);

    {
        ui = 0;
        
        for(x = 0; x < sizeX; x++)
        {
            if((ui < SHMOO_HBM16_MAX_VISIBLE_UI_COUNT) && (x > (*scPtr).endUI[ui]))
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
    
    str0[x] = 0;
    str1[x] = 0;
    str2[x] = 0;

    for(i = 0; i < (iter * wordIter); i++)
    {
        xStart = 0;
        maxMidPointX = (*scPtr).resultData[i >> calibShiftAmount] & 0xFFFF;
        maxMidPointY = ((*scPtr).resultData[i >> calibShiftAmount] >> 16) & 0xFFFF;
        
        if((sizeY > 1) || (i == 0))
        {
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "***** Interface.......: %3d\n"),
                         hbm_ndx));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "***** Channel.........: %3d\n"),
                         channel));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    " **** VDL step size...: %3d.%03d ps\n"),
                         (step1000 / 1000), (step1000 % 1000)));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    " **** UI size.........: %3d.%03d steps\n"),
                         (size1000UI / 1000), (size1000UI % 1000)));
            
            switch((*scPtr).shmooType)
            {
                case SHMOO_HBM16_RD_EXTENDED:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            " **** Shmoo type......: RD_EXTENDED\n")));
                    break;
                case SHMOO_HBM16_WR_EXTENDED:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            " **** Shmoo type......: WR_EXTENDED\n")));
                    break;
                case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            " **** Shmoo type......: ADDR_CTRL_EXTENDED\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported shmoo type: %02d\n"),
                               (*scPtr).shmooType));
                    rv = SOC_E_FAIL;
                    goto cleanup_hbm16;
            }
            
            if(engageUIshift)
            {
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "  *** UI shift........: On\n")));
            }
            else
            {
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "  *** UI shift........: Off or N/A\n")));
            }
        }
        
        if(sizeY > 1)
        {
            switch(calibMode)
            {
                case SHMOO_HBM16_WORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Calib mode......: 2D Word-wise\n")));
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Calib mode......: 2D Doubleword-wise\n")));
                    break;
                case SHMOO_HBM16_QUADWORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Calib mode......: 2D Quadword-wise\n")));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported calibration mode during plot: %02d\n"),
                               calibMode));
                    rv = SOC_E_FAIL;
                    goto cleanup_hbm16;
            }
            
            switch(plotMode)
            {
                case SHMOO_HBM16_BIT:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Plot mode.......: 2D Bit-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "   ** Bit.............: %03d\n"),
                                 i));
                    break;
                case SHMOO_HBM16_BYTE:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Plot mode.......: 2D Byte-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "   ** Byte............: %03d\n"),
                                 i));
                    break;
                case SHMOO_HBM16_HALFWORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Plot mode.......: 2D Halfword-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "   ** Halfword........: %03d\n"),
                                 i));
                    break;
                case SHMOO_HBM16_WORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Plot mode.......: 2D Word-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "   ** Word............: %03d\n"),
                                 i));
                    break;
                case SHMOO_HBM16_DOUBLEWORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Plot mode.......: 2D Doubleword-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "   ** Halfword........: %03d\n"),
                                 i));
                    break;
                case SHMOO_HBM16_QUADWORD:
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  *** Plot mode.......: 2D Quadword-wise\n")));
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "   ** Word............: %03d\n"),
                                 i));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_DDR,
                              (BSL_META_U(unit,
                                          "Unsupported plot mode: %02d\n"),
                               plotMode));
                    rv = SOC_E_FAIL;
                    goto cleanup_hbm16;
            }

            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "    * Center X........: %03d\n"),
                         maxMidPointX));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "    * Center Y........: %03d\n"),
                         maxMidPointY));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "      %s\n"),
                         str0));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "      %s\n"),
                         str1));
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "      %s\n"),
                         str2));

            for(y = yCapMin; y < yCapMax; y++)
            {
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "  %03d "),
                             y));
                
                for(x = 0; x < calibStart; x++)
                {
                    LOG_INFO(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "%s"),
                                 outOfSearch));
                }
                
                for(x = calibStart; x < sizeX; x++)
                {
                    switch(wordIter)
                    {
                        case 4:
                            result = ((*scPtr).result2D[i / iter][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            break;
                        case 2:
                            result = ((*scPtr).result2D[i / iter][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            result &= ((*scPtr).result2D[(i / iter) + 1][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            break;
                        case 1:
                            result = ((*scPtr).result2D[i / iter][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            result &= ((*scPtr).result2D[(i / iter) + 1][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            result &= ((*scPtr).result2D[(i / iter) + 2][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            result &= ((*scPtr).result2D[(i / iter) + 3][xStart + x] >> ((i % iter) << shiftAmount)) & dataMask;
                            break;
                        default:
                            LOG_ERROR(BSL_LS_SOC_DDR,
                                      (BSL_META_U(unit,
                                                  "Unsupported plot word iteration: %02d\n"),
                                       wordIter));
                            return SOC_E_FAIL;
                    }
                    
                    if(result)
                    {   /* FAIL - RISING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR FAIL */
                            LOG_INFO(BSL_LS_SOC_DDR,
                                        (BSL_META_U(unit,
                                                    "%s"),
                                         fail_high));
                        }
                        else
                        {   /* FAIL - RISING EDGE */
                            if((calibPos == SHMOO_HBM16_CALIB_ANY_EDGE) || (calibPos == SHMOO_HBM16_CALIB_RISING_EDGE) || (calibPos == SHMOO_HBM16_CALIB_FAIL_START))
                            {   /* RISING EDGE */
                                LOG_INFO(BSL_LS_SOC_DDR,
                                            (BSL_META_U(unit,
                                                        "X")));
                            }
                            else
                            {   /* FAIL */
                                LOG_INFO(BSL_LS_SOC_DDR,
                                            (BSL_META_U(unit,
                                                        "%s"),
                                                        fail_high));
                            }
                        }
                    }
                    else
                    {   /* PASS - MIDPOINT - FALLING EDGE */
                        if(x != maxMidPointX)
                        {   /* REGULAR PASS */
                            LOG_INFO(BSL_LS_SOC_DDR,
                                        (BSL_META_U(unit,
                                                    "%s"),
                                         pass_low));
                        }
                        else
                        {   /* POTENTIAL MIDPOINT - FALLING EDGE */
                            if(y == maxMidPointY)
                            {   /* MID POINT - FALLING EDGE */
                                LOG_INFO(BSL_LS_SOC_DDR,
                                            (BSL_META_U(unit,
                                                        "X")));
                            }
                            else
                            {   /* PASS */
                                LOG_INFO(BSL_LS_SOC_DDR,
                                            (BSL_META_U(unit,
                                                        "%s"),
                                             pass_low));
                            }
                        }
                    }
                }
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "\n")));
                xStart += sizeX;
            }
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "\n")));
        }
        else
        {
            if(i == 0)
            {
                switch(calibMode)
                {
                    case SHMOO_HBM16_WORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Calib mode......: 1D Word-wise\n")));
                        break;
                    case SHMOO_HBM16_DOUBLEWORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Calib mode......: 1D Doubleword-wise\n")));
                        break;
                    case SHMOO_HBM16_QUADWORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Calib mode......: 1D Quadword-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR,
                                  (BSL_META_U(unit,
                                              "Unsupported calibration mode during plot: %02d\n"),
                                   calibMode));
                        rv = SOC_E_FAIL;
                        goto cleanup_hbm16;
                }
                
                switch(plotMode)
                {
                    case SHMOO_HBM16_BIT:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Plot mode.......: 1D Bit-wise\n")));
                        break;
                    case SHMOO_HBM16_BYTE:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Plot mode.......: 1D Byte-wise\n")));
                        break;
                    case SHMOO_HBM16_HALFWORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Plot mode.......: 1D Halfword-wise\n")));
                        break;
                    case SHMOO_HBM16_WORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Plot mode.......: 1D Word-wise\n")));
                        break;
                    case SHMOO_HBM16_DOUBLEWORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Plot mode.......: 1D Doubleword-wise\n")));
                        break;
                    case SHMOO_HBM16_QUADWORD:
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "  *** Plot mode.......: 1D Quadword-wise\n")));
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR,
                                  (BSL_META_U(unit,
                                              "Unsupported plot mode: %02d\n"),
                                   plotMode));
                        rv = SOC_E_FAIL;
                        goto cleanup_hbm16;
                }
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "      %s\n"),
                             str0));
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "      %s\n"),
                             str1));
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "      %s\n"),
                             str2));
            }
            
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "  %03d "),
                         i));
            
            for(x = 0; x < calibStart; x++)
            {
                LOG_INFO(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "%s"),
                             outOfSearch));
            }
            
            for(x = calibStart; x < sizeX; x++)
            {
                switch(wordIter)
                {
                    case 4:
                        result = ((*scPtr).result2D[i / iter][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        break;
                    case 2:
                        result = ((*scPtr).result2D[i / iter][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        result &= ((*scPtr).result2D[(i / iter) + 1][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        break;
                    case 1:
                        result = ((*scPtr).result2D[i / iter][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        result &= ((*scPtr).result2D[(i / iter) + 1][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        result &= ((*scPtr).result2D[(i / iter) + 2][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        result &= ((*scPtr).result2D[(i / iter) + 3][x] >> ((i % iter) << shiftAmount)) & dataMask;
                        break;
                    default:
                        LOG_ERROR(BSL_LS_SOC_DDR,
                                  (BSL_META_U(unit,
                                              "Unsupported plot word iteration: %02d\n"),
                                   wordIter));
                        return SOC_E_FAIL;
                }
                
                if(result)
                {   /* FAIL - RISING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR FAIL */
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "%s"),
                                                fail_high));
                    }
                    else
                    {   /* FAIL - RISING EDGE */
                        if((calibPos == SHMOO_HBM16_CALIB_ANY_EDGE) || (calibPos == SHMOO_HBM16_CALIB_RISING_EDGE) || (calibPos == SHMOO_HBM16_CALIB_FAIL_START))
                        {   /* RISING EDGE */
                            LOG_INFO(BSL_LS_SOC_DDR,
                                        (BSL_META_U(unit,
                                                    "X")));
                        }
                        else
                        {   /* FAIL */
                            LOG_INFO(BSL_LS_SOC_DDR,
                                        (BSL_META_U(unit,
                                                    "%s"),
                                         fail_high));
                        }
                    }
                }
                else
                {   /* PASS - MIDPOINT - FALLING EDGE */
                    if(x != maxMidPointX)
                    {   /* REGULAR PASS */
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "%s"),
                                     pass_low));
                    }
                    else
                    {   /* MID POINT - FALLING EDGE */
                        LOG_INFO(BSL_LS_SOC_DDR,
                                    (BSL_META_U(unit,
                                                "X")));
                    }
                }
            }
            LOG_INFO(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "\n")));
        }
    }
    LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "\n")));

cleanup_hbm16:
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
_shmoo_hbm16_plot(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr)
{
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
            break;
        case SHMOO_HBM16_RD_EXTENDED:
            if((*scPtr).debugMode & SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
            {
                return _hbm16_plot(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_WORD);
            }
            else
            {
                return _hbm16_plot(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_BIT);
            }
        case SHMOO_HBM16_WR_EXTENDED:
            if((*scPtr).debugMode & SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT)
            {
                return _hbm16_plot(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_WORD);
            }
            else
            {
                return _hbm16_plot(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_BIT);
            }
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            return _hbm16_plot(unit, hbm_ndx, channel, scPtr, SHMOO_HBM16_QUADWORD);
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported shmoo type: %02d\n"), (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

int
soc_hbm16_calculate_step_size(int unit, int hbm_ndx, hbm16_step_size_t *ssPtr)
{
    uint32 data;
    uint32 obs_interval, ro_vdl_step1, ro_vdl_step2, ucount1, ucount2;
    uint32 reset_ctrl_data;
    uint32 fld_dcount;
    uint32 fld_ro_overflow;
    soc_timeout_t to;
    sal_usecs_t to_val;
    hbmc_shmoo_dram_info_t shmoo_dram_info;

    SOC_IF_ERROR_RETURN( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));
    
    if(shmoo_dram_info.sim_system_mode)
    {
        (*ssPtr).step1000 = 1953;
        (*ssPtr).size1000UI = 128000;
        
        return SOC_E_NONE;
    }
    
    data = 0;
    obs_interval = 2000;
    ro_vdl_step1 = 128;
    ro_vdl_step2 = 256;
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_OBS_INTERVALf, obs_interval));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_SEL_FC_REFCLKf, 0)); 
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP); 
    
    SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbm_ndx, &reset_ctrl_data));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_FC_RESET_Nf, 0));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_RO_RESET_Nf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbm_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP); 
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_START_OBSf, 0)); 
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_EN_NLDL_CLKOUT_BARf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_RO_VDL_STEPf, ro_vdl_step1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP); 
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_EN_NLDL_CLKOUT_BARf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_FC_RESET_Nf, 1));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_RO_RESET_Nf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbm_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_START_OBSf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
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
        SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr(unit, hbm_ndx, &data));
        SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_get(unit, HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr, data, HBM16_DCOUNTf, &fld_dcount));
        if(!fld_dcount)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr(unit, hbm_ndx, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_get(unit, HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr, data, HBM16_RO_OVERFLOWf, &fld_ro_overflow));
            if( fld_ro_overflow & 0x4)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL step size computation rollover during first pass\n")));
                return SOC_E_FAIL;
            }
            SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_get(unit, HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr, data, HBM16_RO_UCOUNTf, &ucount1));
            break;
        }
        if (soc_timeout_check(&to))
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Timed out waiting for first pass of VDL step size computation\n")));
            return SOC_E_TIMEOUT;
        }
    }
    while(TRUE);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_FC_RESET_Nf, 0));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_RO_RESET_Nf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbm_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP); 
    
    SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_START_OBSf, 0)); 
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_EN_NLDL_CLKOUT_BARf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_RO_VDL_STEPf, ro_vdl_step2));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP); 
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_EN_NLDL_CLKOUT_BARf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_FC_RESET_Nf, 1));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_RO_RESET_Nf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbm_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_START_OBSf, 1));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
        
    soc_timeout_init(&to, to_val, 0);
    do
    {
        SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr(unit, hbm_ndx, &data));
        SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_get(unit, HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr, data, HBM16_DCOUNTf, &fld_dcount));
        if(!fld_dcount)
        {
            SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr(unit, hbm_ndx, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_get(unit, HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr, data, HBM16_RO_OVERFLOWf, &fld_ro_overflow));
            if( fld_ro_overflow & 0x4)
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "VDL step size computation rollover during first pass\n")));
                return SOC_E_FAIL;
            }
            SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_get(unit, HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr, data, HBM16_RO_UCOUNTf, &ucount2));
            break;
        }
        if (soc_timeout_check(&to))
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Timed out waiting for second pass of VDL step size computation\n")));
            return SOC_E_TIMEOUT;
        }
    }
    while(TRUE);
    
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_FC_RESET_Nf, 0));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, &reset_ctrl_data, HBM16_FREQ_CNTR_RO_RESET_Nf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbm_ndx, reset_ctrl_data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP); 
    
    SOC_IF_ERROR_RETURN(READ_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, &data));
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_START_OBSf, 0)); 
    SOC_IF_ERROR_RETURN( soc_hbm16_midstack_field_set(unit, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, &data, HBM16_EN_NLDL_CLKOUT_BARf, 0));
    SOC_IF_ERROR_RETURN(WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbm_ndx, data));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    
    /** some values for ucount1 and ucount2 to prevent division by 0 in simulated systems.
     * this is used in simulated systems to debug a "full tuning" and as a sanity test, however -
     * this option is blocked by default by the same macro SAL_BOOT_PLISIM */
    /** { */
    if(SAL_BOOT_PLISIM)
    {
		ucount1 = 4;
		ucount2 = 2;
    }
    /** } */

    (*ssPtr).step1000 = (((((((1000000000 / (ro_vdl_step2 - ro_vdl_step1)) << 3) / shmoo_dram_info.data_rate_mbps) * obs_interval) / ucount2) * (ucount1 - ucount2)) / ucount1) >> 1;
    (*ssPtr).size1000UI = ((1000000000 / shmoo_dram_info.data_rate_mbps) * 1000) / ((*ssPtr).step1000);
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_entry(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential entry
     * Mode 1: Single entry
     */
    
    uint32 i;
    uint32 data;
    hbm16_step_size_t ss;
    
    (*scPtr).calibStart = 0;
    
    /* this enable_refresh is commented out, it is the controller's responsibility to disable refresh to the channel before tuning it
    SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_enable_refresh(unit, hbm_ndx, channel, 0));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    */
    
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
            SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_modify_mrs(unit, hbm_ndx, channel, 15, shmoo_hbm16_gen2_wr_vref[3], 0x7));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_FORCE_RXENf, 0));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_FORCE_RXENf, 0));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_FORCE_RXENf, 0));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_FORCE_RXENf, 0));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            
            SOC_IF_ERROR_RETURN(soc_hbm16_calculate_step_size(unit, hbm_ndx, &ss));
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            
            for(i = 0; i < SHMOO_HBM16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_HBM16_RD_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_hbm16_calculate_step_size(unit, hbm_ndx, &ss));
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            
            for(i = 0; i < SHMOO_HBM16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_HBM16_WR_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_hbm16_calculate_step_size(unit, hbm_ndx, &ss));
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            
            for(i = 0; i < SHMOO_HBM16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            SOC_IF_ERROR_RETURN(soc_hbm16_calculate_step_size(unit, hbm_ndx, &ss));
            (*scPtr).step1000 = ss.step1000;
            (*scPtr).size1000UI = ss.size1000UI;
            
            for(i = 0; i < SHMOO_HBM16_MAX_VISIBLE_UI_COUNT; i++)
            {
                (*scPtr).endUI[i] = ((i + 1) * (ss.size1000UI)) / 1000;
            }
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported shmoo type: %02d\n"),
                       (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_exit(int unit, int hbm_ndx, int channel, hbm16_shmoo_container_t *scPtr, uint32 mode)
{
    /* Mode 0: Sequential exit
     * Mode 1: Single exit
     */
    
    uint32 data;
        
    switch((*scPtr).shmooType)
    {
        case SHMOO_HBM16_RD_EN_FISH:
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 2));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 2));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 2));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 2));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 2));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 2));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 2));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 2));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbm_ndx, channel, data));
            break;
        case SHMOO_HBM16_RD_EXTENDED:
            break;
        case SHMOO_HBM16_WR_EXTENDED:
            break;
        case SHMOO_HBM16_ADDR_CTRL_EXTENDED:
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "Unsupported shmoo type: %02d\n"),
                       (*scPtr).shmooType));
            return SOC_E_FAIL;
    }
    
    /* this enable_refresh is commented out, it is the controller's responsibility to enable refresh to the channel after tuning it
    SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_enable_refresh(unit, hbm_ndx, channel, 1));
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
    */
    
    return SOC_E_NONE;
}

STATIC int
_shmoo_hbm16_save(int unit, int hbm_ndx, int channel, hbmc_shmoo_config_param_t *config_param)
{
    uint32 regval;
    int metadata_ndx;

    /** save tune data for one channel */
    for (metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_ndx++)
    {
        /* read specific field */
        if(HBMC_SHMOO_FIELDS_ALL != channel_config_param_metadata[metadata_ndx].field)
        {
            SOC_IF_ERROR_RETURN(soc_hbm16_channel_read(unit, hbm_ndx, channel, 
                channel_config_param_metadata[metadata_ndx].reg, 0, &regval));
            SOC_IF_ERROR_RETURN(soc_hbm16_channel_field_get(unit, channel_config_param_metadata[metadata_ndx].reg, regval, 
                channel_config_param_metadata[metadata_ndx].field, &config_param->channel_data[channel][metadata_ndx].value));
        }           
        /* read all register */
        else
        {
            SOC_IF_ERROR_RETURN(soc_hbm16_channel_read(unit, hbm_ndx, channel, 
                channel_config_param_metadata[metadata_ndx].reg, 0, &config_param->channel_data[channel][metadata_ndx].value));
        }
        /* set valid indication */
        config_param->channel_data[channel][metadata_ndx].valid = TRUE;                            
    }

    /** save tune data for midstack */
    for (metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_ndx++)
    {  
        /* read specific field */
        if(HBMC_SHMOO_FIELDS_ALL != midstack_config_param_metadata[metadata_ndx].field)
        {
            SOC_IF_ERROR_RETURN(soc_hbm16_midstack_read(unit, hbm_ndx, midstack_config_param_metadata[metadata_ndx].reg, 0, &regval));
            SOC_IF_ERROR_RETURN(soc_hbm16_midstack_field_get(unit, midstack_config_param_metadata[metadata_ndx].reg, regval, 
                midstack_config_param_metadata[metadata_ndx].field, &config_param->midstack_data[metadata_ndx].value));
        }           
        /* read all register */
        else
        {
            SOC_IF_ERROR_RETURN(soc_hbm16_midstack_read(unit, hbm_ndx, midstack_config_param_metadata[metadata_ndx].reg, 0, 
                &config_param->midstack_data[metadata_ndx].value));
        }
        /* set valid indication */
        config_param->midstack_data[metadata_ndx].valid = TRUE;                                    
    }
    
    sal_usleep(SHMOO_HBM16_SHORT_SLEEP);    
    return SOC_E_NONE;    
}

STATIC int
_shmoo_hbm16_restore(int unit, int hbm_ndx, int channel, hbmc_shmoo_config_param_t *config_param)
{
    uint32 regval;
    int metadata_ndx;
    
    /** restore tune data for one channel */
    for (metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_ndx++)
    {
        if(config_param->channel_data[channel][metadata_ndx].valid)
        {               
            /* write specific field */
            if(HBMC_SHMOO_FIELDS_ALL != channel_config_param_metadata[metadata_ndx].field)
            {
                SOC_IF_ERROR_RETURN(soc_hbm16_channel_read(unit, hbm_ndx, channel, 
                    channel_config_param_metadata[metadata_ndx].reg, 0, &regval));
                SOC_IF_ERROR_RETURN(soc_hbm16_channel_field_set(unit, channel_config_param_metadata[metadata_ndx].reg, &regval, 
                    channel_config_param_metadata[metadata_ndx].field, config_param->channel_data[channel][metadata_ndx].value));
                SOC_IF_ERROR_RETURN(soc_hbm16_channel_write(unit, hbm_ndx, channel, 
                    channel_config_param_metadata[metadata_ndx].reg, 0, regval));
            }           
            /* write all register */
            else
            {
                SOC_IF_ERROR_RETURN(soc_hbm16_channel_write(unit, hbm_ndx, channel, 
                    channel_config_param_metadata[metadata_ndx].reg, 0, config_param->channel_data[channel][metadata_ndx].value));
            }
        }
    }

    /** restore tune data for midstack */
    for (metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_ndx++)
    {
        if(config_param->midstack_data[metadata_ndx].valid)
        {               
            /* write specific field */
            if(HBMC_SHMOO_FIELDS_ALL != midstack_config_param_metadata[metadata_ndx].field)
            {
                SOC_IF_ERROR_RETURN(soc_hbm16_midstack_read(unit, hbm_ndx, midstack_config_param_metadata[metadata_ndx].reg, 0, &regval));
                SOC_IF_ERROR_RETURN(soc_hbm16_midstack_field_set(unit, midstack_config_param_metadata[metadata_ndx].reg, &regval, 
                    midstack_config_param_metadata[metadata_ndx].field, config_param->midstack_data[metadata_ndx].value));
                SOC_IF_ERROR_RETURN(soc_hbm16_midstack_write(unit, hbm_ndx, midstack_config_param_metadata[metadata_ndx].reg, 0, regval));
            }           
            /* write all register */
            else
            {
                SOC_IF_ERROR_RETURN(soc_hbm16_midstack_write(unit, hbm_ndx, midstack_config_param_metadata[metadata_ndx].reg, 0, 
                    config_param->midstack_data[metadata_ndx].value));
            }
        }
    }
    
    _hbm16_phy_channel_reset(unit, 0, hbm_ndx, channel);

    return SOC_E_NONE;    
}

/*
 * Function:
 *      soc_hbm16_shmoo_ctl
 * Purpose:
 *      Perform shmoo (PHY calibration) on specific DRC index.
 * Parameters:
 *      unit                - unit number
 *      hbm_ndx             - HBM index to perform shmoo on.
 *      channel             - Channel number to perform shmoo on.
 *      shmoo_type          - Selects shmoo sub-section to be performs (-1 for full shmoo)
 *      flags               -   SHMOO_HBM16_CTL_FLAGS_STAT_BIT            
 *                                  0: Runs normal shmoo functions
 *                                  1: Doesn't run shmoo. Only prints statistics. (Nothing is printed at the moment)
 *                              SHMOO_HBM16_CTL_FLAGS_PLOT_BIT            
 *                                  0: No shmoo plots are printed during shmoo
 *                                  1: Shmoo plots are printed during shmoo
 *                              SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT
 *                                  0: Shmoo runs with a regular range for Vref sweep in *EXTENDED shmoos
 *                                  1: Shmoo runs with an extended range for Vref sweep in *EXTENDED shmoos
 *                              SHMOO_HBM16_CTL_FLAGS_CUSTOM_BIST_BIT
 *                                  0: Shmoo runs with a regular DRAM Bist
 *                                  1: Shmoo runs with traffic instead of DRAM Bist, error indications are ECC and parity and resolution is per pseudo channel or per dword
 *      action              - Save/restore functionality
 *      *config_param       - PHY configuration saved/restored
 * Returns:
 *      SOC_E_XXX
 *      This routine may be called after a device is attached
 *      or whenever a chip reset is required.
 */

int
soc_hbm16_shmoo_ctl(int unit, int hbm_ndx, int channel, int shmoo_type, uint32 flags, int action, hbmc_shmoo_config_param_t *config_param)
{
    hbm16_shmoo_container_t *scPtr = NULL;
    uint32 dramType;
    uint32 ctlType;
    uint32 i;
    int ndx, ndxEnd;
    int chn, chnEnd;
    uint32 stat, plot;
    const uint32 *seqPtr;
    uint32 seqCount;
    hbmc_shmoo_dram_info_t shmoo_dram_info;
        
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));

    if(!_shmoo_hbm16_validate_config(unit))
    {
        LOG_ERROR(BSL_LS_SOC_DDR,
                  (BSL_META_U(unit,
                              "Invalid PHY PLL or DRAM configuration identified during Shmoo\n")));
        return SOC_E_FAIL;
    }
    
    dramType = shmoo_dram_info.dram_type;
    ctlType = shmoo_dram_info.ctl_type;

    stat = flags & SHMOO_HBM16_CTL_FLAGS_STAT_BIT;
    plot = flags & SHMOO_HBM16_CTL_FLAGS_PLOT_BIT;
    
    if(!stat)
    {
        scPtr = sal_alloc(sizeof(hbm16_shmoo_container_t), "HBM16 Shmoo Container");
        if(scPtr == NULL)
        {
            return SOC_E_MEMORY;
        }
        sal_memset(scPtr, 0, sizeof(hbm16_shmoo_container_t));
        
        (*scPtr).debugMode = flags & SHMOO_HBM16_DEBUG_MODE_FLAG_BITS;
        (*scPtr).customBistMode = !!(flags & SHMOO_HBM16_CTL_FLAGS_CUSTOM_BIST_BIT);
        
        /** set which dwords the tuning should effect (relevant for RD_EXTENDED) */
        scPtr->dwordsBitmap |= (flags & SHMOO_HBM16_CTL_FLAGS_DWORD0) / SHMOO_HBM16_CTL_FLAGS_DWORD0;
        scPtr->dwordsBitmap |= (flags & SHMOO_HBM16_CTL_FLAGS_DWORD1) / SHMOO_HBM16_CTL_FLAGS_DWORD0;
        scPtr->dwordsBitmap |= (flags & SHMOO_HBM16_CTL_FLAGS_DWORD2) / SHMOO_HBM16_CTL_FLAGS_DWORD0;
        scPtr->dwordsBitmap |= (flags & SHMOO_HBM16_CTL_FLAGS_DWORD3) / SHMOO_HBM16_CTL_FLAGS_DWORD0;

        /** set read/write lfsr loopback flags */
        scPtr->read_lfsr_loopback = flags & SHMOO_HBM16_LOOPBACK_MODE_READ_LFSR;
        scPtr->write_lfsr_loopback = flags & SHMOO_HBM16_LOOPBACK_MODE_WRITE_LFSR;

        if(hbm_ndx != SHMOO_HBM16_INTERFACE_RSVP)
        {
            ndx = hbm_ndx;
            ndxEnd = hbm_ndx + 1;
        }
        else
        {
            ndx = 0;
            ndxEnd = SHMOO_HBM16_MAX_INTERFACES;
        }
        
        if(channel != SHMOO_HBM16_CHANNEL_RSVP)
        {
            chn = channel;
            chnEnd = channel + 1;
        }
        else
        {
            chn = 0;
            chnEnd = SHMOO_HBM16_CHANNELS_PER_INTERFACE;
        }
        
        for(; ndx < ndxEnd; ndx++)
        {
            if(!_shmoo_hbm16_check_dram(unit, ndx)) {
                continue;
            }
            
            for(; chn < chnEnd; chn++)
            {
                if(action == SHMOO_HBM16_ACTION_RESTORE)
                {
                    (*scPtr).restore = 1;
                    switch(ctlType)
                    {
                        case SHMOO_HBM16_CTL_TYPE_RSVP:
                            break;
                        case SHMOO_HBM16_CTL_TYPE_1:
                            _SOC_IF_ERR_EXIT(_shmoo_hbm16_restore(unit, hbm_ndx, chn, config_param));
                            break;
                        default:
                            LOG_ERROR(BSL_LS_SOC_DDR,
                                      (BSL_META_U(unit,
                                                  "Unsupported controller type: %02d\n"),
                                       ctlType));
                            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META("%s\n"), soc_errmsg(SOC_E_FAIL)));
                    }
                }
                else
                {
                    (*scPtr).restore = 0;
                }
                
                if((action == SHMOO_HBM16_ACTION_RUN) || (action == SHMOO_HBM16_ACTION_RUN_AND_SAVE) || (action == SHMOO_HBM16_ACTION_RESTORE))
                {
                    switch(ctlType)
                    {
                        case SHMOO_HBM16_CTL_TYPE_RSVP:
                            break;
                        case SHMOO_HBM16_CTL_TYPE_1:
                            if((*scPtr).restore)
                            {
                                switch(dramType)
                                {
                                    case SHMOO_HBM16_DRAM_TYPE_GEN2:
                                        seqPtr = NULL;
                                        seqCount = SHMOO_HBM16_GEN2_RESTORE_SEQUENCE_COUNT;
                                        break;
                                    default:
                                        LOG_ERROR(BSL_LS_SOC_DDR,
                                                  (BSL_META_U(unit,
                                                              "Unsupported dram type: %02d\n"),
                                                   dramType));
                                        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META("%s\n"), soc_errmsg(SOC_E_FAIL)));
                                }
                            }
                            else
                            {
                                switch(dramType)
                                {
                                    case SHMOO_HBM16_DRAM_TYPE_GEN2:
                                        seqPtr = &shmoo_order_hbm16_gen2[0];
                                        seqCount = SHMOO_HBM16_GEN2_SEQUENCE_COUNT;
                                        break;
                                    default:
                                        LOG_ERROR(BSL_LS_SOC_DDR,
                                                  (BSL_META_U(unit,
                                                              "Unsupported dram type: %02d\n"),
                                                   dramType));
                                        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META("%s\n"), soc_errmsg(SOC_E_FAIL)));
                                }
                            }
                            
                            (*scPtr).dramType = dramType;
                            (*scPtr).ctlType = ctlType;
                            
                            if(shmoo_type != SHMOO_HBM16_SHMOO_RSVP)
                            {
                                (*scPtr).shmooType = shmoo_type;
                                _SOC_IF_ERR_EXIT(_shmoo_hbm16_entry(unit, ndx, chn, scPtr, SHMOO_HBM16_SINGLE));
                                _SOC_IF_ERR_EXIT(_shmoo_hbm16_do(unit, ndx, chn, scPtr));
                                _SOC_IF_ERR_EXIT(_shmoo_hbm16_calib_2D(unit, ndx, chn, scPtr));
                                _SOC_IF_ERR_EXIT(_shmoo_hbm16_set_new_step(unit, ndx, chn, scPtr));
                                if(plot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO))
                                {
                                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_plot(unit, ndx, chn, scPtr));
                                }
                                _SOC_IF_ERR_EXIT(_shmoo_hbm16_exit(unit, ndx, chn, scPtr, SHMOO_HBM16_SINGLE));
                            }
                            else
                            {
                                for(i = 0; i < seqCount; i++)
                                {
                                    (*scPtr).shmooType = seqPtr[i];
                                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_entry(unit, ndx, chn, scPtr, SHMOO_HBM16_SEQUENTIAL));
                                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_do(unit, ndx, chn, scPtr));
                                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_calib_2D(unit, ndx, chn, scPtr));
                                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_set_new_step(unit, ndx, chn, scPtr));
                                    if(plot || LOG_CHECK(BSL_LS_SOC_DDR | BSL_INFO))
                                    {
                                        _SOC_IF_ERR_EXIT(_shmoo_hbm16_plot(unit, ndx, chn, scPtr));
                                    }
                                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_exit(unit, ndx, chn, scPtr, SHMOO_HBM16_SEQUENTIAL));
                                }
                            }
                            
                            break;
                        default:
                            LOG_ERROR(BSL_LS_SOC_DDR,
                                      (BSL_META_U(unit,
                                                  "Unsupported controller type: %02d\n"),
                                       ctlType));
                            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META("%s\n"), soc_errmsg(SOC_E_FAIL)));
                    }
                }
                
                if((action == SHMOO_HBM16_ACTION_RUN_AND_SAVE) || (action == SHMOO_HBM16_ACTION_SAVE))
                {
                    _SOC_IF_ERR_EXIT(_shmoo_hbm16_save(unit, hbm_ndx, chn, config_param));
                }
            }
        }
        
        LOG_INFO(BSL_LS_SOC_DDR,
                    (BSL_META_U(unit,
                                "DDR Tuning Complete\n")));
    }
    else
    {
        /* Report only */
        switch(ctlType)
        {
            case SHMOO_HBM16_CTL_TYPE_RSVP:
                break;
            case SHMOO_HBM16_CTL_TYPE_1:
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_DDR,
                          (BSL_META_U(unit,
                                      "Unsupported controller type: %02d\n"),
                           ctlType));
                _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (BSL_META("%s\n"), soc_errmsg(SOC_E_FAIL)));
        }
    }

exit:
    SOC_FREE(scPtr);
    SOC_FUNC_RETURN;
}

/* Configure PHY PLL and wait for lock */
int
soc_hbm16_shmoo_phy_cfg_pll(int unit, int hbm_ndx)
{
    int ndx, ndxEnd;
    int fref, fref_post, fref_eff_int, fref_eff_frac, freq_vco, data_rate;
    int pdiv10, ndiv_int10;
    hbmc_shmoo_pll_t pll_info;
    hbmc_shmoo_dram_info_t shmoo_dram_info;

    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "get dram_info_access\n")));
    SOC_IF_ERROR_RETURN( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));
    
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "validate CBs are initialized\n")));
    if(!_shmoo_hbm16_validate_cbi())
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Null callback function detected during PLL initialization\n")));
        return SOC_E_FAIL;
    }
    
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "validate dram configuration\n")));
    if(!_shmoo_hbm16_validate_config(unit))
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Invalid PHY PLL or DRAM configuration identified during PLL initialization\n")));
        return SOC_E_FAIL;
    }
    
    data_rate = shmoo_dram_info.data_rate_mbps;
    fref = shmoo_dram_info.ref_clk_mhz;
    
    pll_info.ndiv_p = 0;
    pll_info.ndiv_q = 0;
    pll_info.ndiv_frac_mode_sel = 0;
    pll_info.ssc_limit = 0;
    pll_info.ssc_mode = 0;
    pll_info.ssc_step = 0;
    
    fref_eff_int = 50;
    fref_eff_frac = 000;
    freq_vco = data_rate << 1;
    
    fref_post = fref;
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

    pll_info.mdiv_aux_post = 1;
    pll_info.mdiv_ch[0] = 1;
    pll_info.mdiv_ch[1] = 1;
    pll_info.mdiv_ch[2] = 1;
    pll_info.mdiv_ch[3] = 1;
    pll_info.mdiv_ch[4] = 1;
    pll_info.mdiv_ch[5] = 1;
    pll_info.vco_fb_div2 = 0;
    pll_info.fref_eff_info = fref_eff_int;
    
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "PHY PLL Configuration\n")));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Fref...........: %4d\n"), fref));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "PDIV...........: %4d\n"), pll_info.pdiv));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "NDIV...........: %4d\n"), pll_info.ndiv_int));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Fvco...........: %4d\n"), freq_vco));
    LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Data rate......: %4d\n"), data_rate));

    pll_info.enableb_aux_post = 0;
    pll_info.enableb_ch_bitmap = 0x3F;

    if(hbm_ndx != SHMOO_HBM16_INTERFACE_RSVP)
    {
        ndx = hbm_ndx;
        ndxEnd = hbm_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_HBM16_MAX_INTERFACES;
    }
    
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_hbm16_check_dram(unit, ndx))
        {
            continue;
        }
        
        LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "setting pll for channel %d\n"), ndx));
        if(_shmoo_hbm16_hbmc_pll_set(unit, ndx, &pll_info) != SOC_E_NONE)
        {
            return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

int
soc_hbm16_shmoo_phy_init(int unit, int hbm_ndx, int channel)
{
    int ndx, ndxEnd;
    int chn, chnEnd;
    uint32 data, reset_ctrl_data;
    uint32 dramType;
    hbm16_step_size_t ss;
    uint32 step1000;
    uint32 size1000UI;
    uint32 setting;
    uint32 ck_ui=0, addr_ctrl_ui=0 , dqs_ui=0 , data_ui=0;
    uint32 single_ended_drive;
    uint32 differential_drive;
    uint32 dac_data_val;
    hbmc_shmoo_hbm_model_part_t model_part;
    hbmc_shmoo_dram_info_t shmoo_dram_info;

    SOC_IF_ERROR_RETURN( _shmoo_hbm16_hbmc_dram_info_access(unit, &shmoo_dram_info));

    if(!_shmoo_hbm16_validate_cbi())
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Null callback function detected during PHY initialization\n")));
        return SOC_E_FAIL;
    }
    
    if(!_shmoo_hbm16_validate_config(unit))
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Invalid PHY PLL or DRAM configuration identified during PHY initialization\n")));
        return SOC_E_FAIL;
    }

    dramType = shmoo_dram_info.dram_type;
    switch(dramType)
    {
        case SHMOO_HBM16_DRAM_TYPE_GEN2:
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Unsupported dram type: %02d\n"), dramType));
            return SOC_E_FAIL;
    }

    if(hbm_ndx != SHMOO_HBM16_INTERFACE_RSVP)
    {
        ndx = hbm_ndx;
        ndxEnd = hbm_ndx + 1;
    }
    else
    {
        ndx = 0;
        ndxEnd = SHMOO_HBM16_MAX_INTERFACES;
    }
    
    if(channel != SHMOO_HBM16_CHANNEL_RSVP)
    {
        chn = channel;
        chnEnd = channel + 1;
    }
    else
    {
        chn = 0;
        chnEnd = SHMOO_HBM16_CHANNELS_PER_INTERFACE;
    }
    
    for(; ndx < ndxEnd; ndx++)
    {
        if(!_shmoo_hbm16_check_dram(unit, ndx))
        {
            continue;
        }
        
        for(; chn < chnEnd; chn++)
        {
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C Series - PHY Initialization (HBM index: %d / Channel index: %d)\n"), ndx, chn));
            
    /*C01*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C01. Reset all PHY logic\n")));
            reset_ctrl_data = 0;
            SOC_IF_ERROR_RETURN(WRITE_HBM16_CONTROL_RESETSr(unit, ndx, chn, reset_ctrl_data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C02*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C02. Configure input shift control\n")));
            data = 0;
            switch(dramType)
            {
                case SHMOO_HBM16_DRAM_TYPE_GEN2:
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_LATENCY_SHIFTERr, &data, HBM16_DATA_PATH_SHIFT_ENABLEf, 0));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_LATENCY_SHIFTERr, &data, HBM16_DATA_PATH_ADDITIONAL_LATENCYf, 0));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_LATENCY_SHIFTERr, &data, HBM16_RCMD_SHIFT_ENABLEf, 1));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_LATENCY_SHIFTERr, &data, HBM16_RCMD_ADDITIONAL_LATENCYf, 12));
                    break;
            }
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_LATENCY_SHIFTERr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_LATENCY_SHIFTERr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_LATENCY_SHIFTERr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_LATENCY_SHIFTERr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            data = 0;
            switch(dramType)
            {
                case SHMOO_HBM16_DRAM_TYPE_GEN2:
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_LATENCY_SHIFTERr, &data, HBM16_DATA_PATH_SHIFT_ENABLEf, 0));
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_LATENCY_SHIFTERr, &data, HBM16_DATA_PATH_ADDITIONAL_LATENCYf, 0));
                    break;
            }
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_LATENCY_SHIFTERr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C03*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C03. Configure reference voltage\n")));
            data = 0;
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_PDf, 1));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            SOC_IF_ERROR_RETURN(_shmoo_hbm16_hbmc_shmoo_model_part_get(unit, ndx, &model_part));
            if(model_part == HBM_TYPE_SAMSUNG_DIE_B)
            {
                dac_data_val = 0x4;
            }
            else if(model_part == HBM_TYPE_SAMSUNG_DIE_X)
            {
                dac_data_val = 0x8;
            }
            else
            {
                LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Invalid HBM model part detected\n")));
                return SOC_E_FAIL;
            }
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_DATAf, dac_data_val));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_DAC_CONFIGr, &data, HBM16_DAC_PDf, 0));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C04*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C04. Compute VDL step size\n")));
            SOC_IF_ERROR_RETURN(soc_hbm16_calculate_step_size(unit, ndx, &ss));
            
            step1000 = ss.step1000;
            size1000UI = ss.size1000UI;
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     VDL step size........: %3d.%03d ps\n"), (step1000 / 1000), (step1000 % 1000)));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "     UI size..............: %3d.%03d steps\n"), (size1000UI / 1000), (size1000UI % 1000)));
            
    /*C05*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C05. Configure read clock\n")));
            data = 0;
            switch(dramType)
            {
                case SHMOO_HBM16_DRAM_TYPE_GEN2:
                    SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MACHINE_CONFIGr, &data, HBM16_CONTINUOUS_MODEf, 0));
                    break;
            }
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            
            if(dramType == SHMOO_HBM16_DRAM_TYPE_GEN2)
            {
                data = 0;
                setting = size1000UI / 1000;                                                        /* 50% placement on FSM clock = 1 UI */
                SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MAX_VDL_FSMr, &data, HBM16_MAX_VDL_STEPf, setting));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_FSMr(unit, ndx, chn, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_FSMr(unit, ndx, chn, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_FSMr(unit, ndx, chn, data));
                SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_FSMr(unit, ndx, chn, data));
            }
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C06*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C06. Configure UI shifts\n")));
            switch(dramType)
            {
                case SHMOO_HBM16_DRAM_TYPE_GEN2:
                    ck_ui = SHMOO_HBM16_GEN2_INIT_CK_UI_SHIFT;
                    addr_ctrl_ui = SHMOO_HBM16_GEN2_INIT_ADDR_CTRL_UI_SHIFT;
                    dqs_ui = SHMOO_HBM16_GEN2_INIT_DQS_UI_SHIFT;
                    data_ui = SHMOO_HBM16_GEN2_INIT_DATA_UI_SHIFT;
                    break;
            }
            
            data = 0;
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_CKr, &data, HBM16_UI_SHIFTf, ck_ui));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_WRITE_MAX_VDL_CKr(unit, ndx, chn, data));
            
            data = 0;
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_UI_SHIFTf, addr_ctrl_ui));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, ndx, chn, data));
            
            data = 0;
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DQSr, &data, HBM16_UI_SHIFTf, dqs_ui));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            data = 0;
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_UI_SHIFTf, data_ui));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            
    /*C07*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C07. Configure VDLs\n")));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_WRITE_MAX_VDL_DQSr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DQSr, &data, HBM16_MAX_VDL_STEPf, 0));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DQSr(unit, ndx, chn, data));
            
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, ndx, chn, &data));  /* Known working RDQS position */
            setting = (size1000UI * 435) / 1000000;                                             /* 43.5% adjustment */
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, &data, HBM16_MAX_VDL_STEPf, setting));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, ndx, chn, data));
            
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, ndx, chn, &data));  /* Known working WDATA position */
            setting = (size1000UI * 462) / 1000000;                                             /* 46.2% adjustment */
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, &data, HBM16_MAX_VDL_STEPf, setting));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, ndx, chn, data));

            data = 0;
            switch(dramType)
            {
                case SHMOO_HBM16_DRAM_TYPE_GEN2:
                    data = SHMOO_HBM16_GEN2_INIT_READ_MIN_VDL_POS;
                    break;
            }

            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MIN_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MIN_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MIN_VDL_DATAr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MIN_VDL_DATAr(unit, ndx, chn, data));
            
            SOC_IF_ERROR_RETURN(READ_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, ndx, chn, &data));   /* ADDR/CTRL should be in a working position */
            setting = (size1000UI * 462) / 1000000;                                             /* 46.2% adjustment */
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, &data, HBM16_MAX_VDL_STEPf, setting));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C08*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C08. Configure Lane Repair\n")));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_LANE_REMAPr(unit, ndx, chn, 0x000FFFFF));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_LANE_REMAPr(unit, ndx, chn, 0x000FFFFF));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_LANE_REMAPr(unit, ndx, chn, 0x000FFFFF));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_LANE_REMAPr(unit, ndx, chn, 0x000FFFFF));
            
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_LANE_REMAPr(unit, ndx, chn, 0x000003FF));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C09*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C09. Configure IOs\n")));
            single_ended_drive = 4;
            differential_drive = 4;
            
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              Single-ended Drive...............: 0x%02X\n"), single_ended_drive));
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "              Differential Drive...............: 0x%02X\n"), differential_drive));
            
            SOC_IF_ERROR_RETURN(READ_HBM16_AWORD_IO_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_IO_CONFIGr, &data, HBM16_RXENB_AERRf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_IO_CONFIGr, &data, HBM16_RXENBf, 1));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_IO_CONFIGr, &data, HBM16_SINGLE_PORT_AQ_DRIVE_STRENGTHf, single_ended_drive));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_AWORD_IO_CONFIGr, &data, HBM16_DIFF_PORT_CK_DRIVE_STRENGTHf, differential_drive));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_AWORD_IO_CONFIGr(unit, ndx, chn, data));

            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_IO_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_IO_CONFIGr, &data, HBM16_RXENB_DERRf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_IO_CONFIGr, &data, HBM16_RXENB_RDQSf, 0));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_IO_CONFIGr, &data, HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf, single_ended_drive));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_IO_CONFIGr, &data, HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf, differential_drive));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_IO_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_IO_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_IO_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_IO_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_LONG_SLEEP);
            
    /*C10*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C10. Configure FSMs\n")));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MACHINE_CONFIGr, &data, HBM16_RDATA_FIFO_WR2RD_DELAYf, 4));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_POST_CYCLESf, 2));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RXEN_PRE_CYCLESf, 2));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_PAR_LATENCYf, shmoo_dram_info.parity_latency));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_RD_DATA_DELAYf, 8));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_FORCE_RXENf, 1));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_AUTO_UPDATEf, 1));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C11*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C11. Release PHY resets\n")));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_CONTROL_RESETSr, &reset_ctrl_data, HBM16_PHY_RSTB_1Gf, 1));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_CONTROL_RESETSr(unit, ndx, chn, reset_ctrl_data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
    /*C12*/ LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C12. Enable FSMs\n")));
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_READ_MACHINE_CONFIGr, &data, HBM16_ENABLEf, 1));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            SOC_IF_ERROR_RETURN(READ_HBM16_DWORD0_BV_GEN_CONFIGr(unit, ndx, chn, &data));
            SOC_IF_ERROR_RETURN( soc_hbm16_channel_field_set(unit, HBM16_DWORD0_BV_GEN_CONFIGr, &data, HBM16_ENABLEf, 1));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD0_BV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD1_BV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD2_BV_GEN_CONFIGr(unit, ndx, chn, data));
            SOC_IF_ERROR_RETURN(WRITE_HBM16_DWORD3_BV_GEN_CONFIGr(unit, ndx, chn, data));
            sal_usleep(SHMOO_HBM16_SHORT_SLEEP);
            
            LOG_INFO(BSL_LS_SOC_DDR, (BSL_META_U(unit, "C Series - PHY Initialization complete (HBM index: %d / Channel index: %d)\n"), ndx, chn));
        }
    }
    
    return SOC_E_NONE;
}

/* Register Device interface CB to shmoo */
int
soc_hbm16_shmoo_interface_cb_register(int unit, hbmc_shmoo_cbi_t shmoo_cbi)
{
    soc_hbm16_phy_reg_read = shmoo_cbi.hbmc_phy_reg_read;
    soc_hbm16_phy_reg_write = shmoo_cbi.hbmc_phy_reg_write;
    soc_hbm16_phy_reg_modify = shmoo_cbi.hbmc_phy_reg_modify;
    _shmoo_hbm16_hbmc_bist_conf_set = shmoo_cbi.hbmc_shmoo_bist_conf_set;
    _shmoo_hbm16_hbmc_bist_err_cnt = shmoo_cbi.hbmc_shmoo_bist_err_cnt;
    _shmoo_hbm16_hbmc_dram_init = shmoo_cbi.hbmc_shmoo_dram_init;
    _shmoo_hbm16_hbmc_custom_bist_run = shmoo_cbi.hbmc_shmoo_custom_bist_run;
    _shmoo_hbm16_hbmc_pll_set = shmoo_cbi.hbmc_shmoo_pll_set;
    _shmoo_hbm16_hbmc_modify_mrs = shmoo_cbi.hbmc_shmoo_modify_mrs;
    _shmoo_hbm16_hbmc_enable_wr_parity = shmoo_cbi.hbmc_shmoo_enable_wr_parity;
    _shmoo_hbm16_hbmc_enable_rd_parity = shmoo_cbi.hbmc_shmoo_enable_rd_parity;
    _shmoo_hbm16_hbmc_enable_addr_parity = shmoo_cbi.hbmc_shmoo_enable_addr_parity;
    _shmoo_hbm16_hbmc_enable_wr_dbi = shmoo_cbi.hbmc_shmoo_enable_wr_dbi;
    _shmoo_hbm16_hbmc_enable_rd_dbi = shmoo_cbi.hbmc_shmoo_enable_rd_dbi;
    _shmoo_hbm16_hbmc_enable_refresh = shmoo_cbi.hbmc_shmoo_enable_refresh;
    _shmoo_hbm16_hbmc_soft_reset_controller_without_dram = shmoo_cbi.hbmc_shmoo_soft_reset_controller_without_dram;
    _shmoo_hbm16_hbmc_dram_info_access = shmoo_cbi.hbmc_shmoo_dram_info_access;
    _shmoo_hbm16_hbmc_bist_status_get = shmoo_cbi.hbmc_shmoo_bist_status_get;
    _shmoo_hbm16_hbmc_shmoo_model_part_get = shmoo_cbi.hbmc_shmoo_model_part_get;
    _shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check = shmoo_cbi.hbmc_shmoo_phy_channel_dwords_alignment_check;

    if(!_shmoo_hbm16_validate_cbi())
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Null callback function detected during CBI registration\n")));
        return SOC_E_FAIL;
    }
    
    return SOC_E_NONE;
}

const hbmc_shmoo_channel_config_param_t *
soc_hbm16_shmoo_channel_config_param_metadata_get(void)
{
    return channel_config_param_metadata;
}
const hbmc_shmoo_midstack_config_param_t *
soc_hbm16_shmoo_midstack_config_param_metadata_get(void)
{
    return midstack_config_param_metadata;
}

#undef _ERR_MSG_MODULE_NAME

