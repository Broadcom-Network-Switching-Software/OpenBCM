/*! \file bcm56780_a0_ptm_ser.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmbd/bcmbd_port_intr.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmbd/chip/bcm56780_a0_ipep_intr.h>
#include <bcmbd/chip/bcm56780_a0_port_intr.h>
#include <bcmbd/chip/bcm56780_a0_lp_intr.h>
#include <bcmbd/chip/bcm56780_a0_mmu_intr.h>

#include "bcm56780_a0_ptm_ser.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* ACC_TYPE     => $ACC_TYPE{SINGLE} */
#define ACC_TYPE_SINGLE           20

/* ACC_TYPE     => $ACC_TYPE{UNIQUE} */
#define ACC_TYPE_UNIQUE           31

/* ACC_TYPE     => $ACC_TYPE{DUPLICATE} */
#define ACC_TYPE_DUPLICATE        9

/* ACC_TYPE     => $ACC_TYPE{DATA_SPLIT} */
#define ACC_TYPE_DATA_SPLIT       14

/* ACC_TYPE     => $ACC_TYPE{ADDR_SPLIT} */
#define ACC_TYPE_ADDR_SPLIT       10

/* ACC_TYPE     => $ACC_TYPE{ADDR_SPLIT_SPLIT} */
#define ACC_TYPE_ADDR_SPLIT_SPLIT 12

/* ACC_TYPE     => $ACC_TYPE{DUPLICATE_2} */
#define ACC_TYPE_DUPLICATE_2      15

/* ACC_TYPE     => $ACC_TYPE{DUPLICATE_4} */
#define ACC_TYPE_DUPLICATE_4      16

/* ACC_TYPE is duplicate */
#define SER_ACC_TYPE_IS_DUPLICATE(acc_type) \
    (ACC_TYPE_DUPLICATE == acc_type || \
     ACC_TYPE_DUPLICATE_2 == acc_type || \
     ACC_TYPE_DUPLICATE_4 == acc_type || \
     ACC_TYPE_DATA_SPLIT == acc_type)

/* Refer to bcmbd_cmicx_schan.h */
#define READ_MEMORY_CMD_MSG           0x07
#define READ_REGISTER_CMD_MSG         0x0b
#define WRITE_MEMORY_CMD_MSG          0x09
#define WRITE_REGISTER_CMD_MSG        0x0d

#define BCMPTM_SER_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define BCMPTM_SER_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define BCMPTM_SER_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define BCMPTM_SER_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define BCMPTM_SER_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define BCMPTM_SER_OPCODE_SET(d,v)    F32_SET(d,26,6,v)

/* Each PM has 4 physical ports */
#define PORT_TO_PM_INDEX(_port)          \
    (((_port) - 1) / 8)

/*!
 * \brief SER logical block types (per chip).
 * Just can be used by SER_LOG and SER_STATS.
 * Consistent with block type definition in LT SER_LOG.map.ltl.
 */
typedef enum {
    /*! All block types. */
    BCMPTM_SER_BLK_ALL = 0,
    /*! MMU */
    BCMPTM_SER_BLK_MMU = 1,
    /*! IPIPE */
    BCMPTM_SER_BLK_IPIPE = 2,
    /*! EPIPE */
    BCMPTM_SER_BLK_EPIPE = 3,
    /*! Packet gateway. */
    BCMPTM_SER_BLK_PGW = 4,
    /*! Ports. */
    BCMPTM_SER_BLK_PORT = 5,
    /*! Macsec. */
    BCMPTM_SER_BLK_SEC = 6,
} bcmptm_ser_blk_type_t;

/*!
 * \brief Some instances do not exist, map them to valid instance.
 */
typedef struct bcmptm_fld_inst_map_s {
    /*!< Invalid instance  */
    int  invalid_inst;
    /*!< Valid instance  */
    int  valid_inst;
} bcmptm_fld_inst_map_t;

/* List of PTs which are needed to be cleared before enabling SER parity */
static bcmptm_ser_mem_clr_list_t bcm56780_a0_mem_clr_lst[] = {
    { EGR_TIMESTAMP_EGR_1588_LINK_DELAY_64m, -1 },
    { EPOST_TRACE_DROP_EVENT_EGR_HIGHEST_DROP_COUNTERm, -1 }
};

/* In order to clear ING/EDB/EGR SER FIFO, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t ing_egr_edb_ser_fifo_ctrl_fld_lst[] = {
    {FIFO_RESETf, {0}, 1, 0},
    {FIFO_RESETf, {0}, 0, 0} /* reset */
};

static bcmptm_ser_fld_info_t mmu_mtro_config_fld_lst[] = {
    { REFRESH_DISABLEf, {0}, 1, 0},
    { REFRESH_DISABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t mmu_ebpts_cbmg_value_fld_lst[] = {
    {MMU_NULL_SLOT_COUNTER_VALUEf, {0}, 0, 0},
    {TERMINAL_COUNTER_VALUEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t mmu_qsch_voq_fairness_fld_lst[] = {
    {DYNAMIC_WEIGHT_ENABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t mmu_thdo_engine_enables_cfg_fld_lst[] = {
    {RESET_WRED_ENGINEf, {0}, 1, 0},
    {SRC_PORT_DROP_COUNT_ENABLEf, {0}, 0, 0},
    {DLB_INTERFACE_ENABLEf, {0}, 0, 0}
};
static bcmptm_ser_fld_info_t sec_macsec_mib_mode_fld_lst[] = {
    {MIB_CLEAR_ON_READf, {1}, 0, 0}
};
static bcmptm_ser_fld_info_t mmu_wred_refresh_control_fld_lst[] = {
    { REFRESH_DISABLEf, {0}, 1, 0},
    { REFRESH_DISABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t dlb_refresh_disable_fld_lst[] = {
    { DLB_REFRESH_DISABLEf, {0}, 1, 0},
    { DLB_REFRESH_DISABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t dii_aux_arb_control_fld_lst[] = {
    { FP_REFRESH_ENABLEf, {0}, 0, 0},
    { DLB_HGT_256NS_REFRESH_ENABLEf , {0}, 0, 0},
    { FP_REFRESH_ENABLEf, {0}, 1, 0},
    { DLB_HGT_256NS_REFRESH_ENABLEf , {0}, 1, 0}
};

/*
 * List of control registers which are needed to be configured, in order to clear memory tables
 * Those memory tables should be cleared before enabling SER
 */
static bcmptm_ser_ctrl_reg_info_t bcm56780_a0_clear_mem_before_ser_enable_info[] = {
    /* Clear EGR SER FIFO */
    { EPOST_EGR_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    { EPOST_EGR_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    { EDB_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    { EDB_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    { EGR_METADATA_EXTRACTION_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    { EGR_METADATA_EXTRACTION_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    /* Clear ING SER FIFO */
    { IPOST_SER_STATUS_BLK_ING_SER_FIFO_CTRLr, 0,
      &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    { IPOST_SER_STATUS_BLK_ING_SER_FIFO_CTRLr, 0,
      &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    { DLB_ECMP_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    { DLB_ECMP_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    /* Disable memory refresh */
    { EGR_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[0], 2, 0},
    { ING_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[0], 2, 0},
    /* {MMU_MTRO_CONFIGr, 0, &mmu_mtro_config_fld_lst[0], 1, 0}, */
    /* {MMU_WRED_REFRESH_CONTROLr, 0, &mmu_wred_refresh_control_fld_lst[0], 1, 0}, */
    { DLB_LAG_REFRESH_DISABLEr, 0, &dlb_refresh_disable_fld_lst[0], 1, 0},
    { DLB_ECMP_REFRESH_DISABLEr, 0, &dlb_refresh_disable_fld_lst[0], 1, 0}
    /* Add new items here */
};

/*
 * List of control registers which are needed to be configured, in order to clear memory tables
 * The memory tables should be cleared after enabling SER
 */
static bcmptm_ser_ctrl_reg_info_t bcm56780_a0_clear_mem_after_ser_enable_info[] = {
    /* Enable memory refresh */
    { ING_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[2], 2, 0},
    { EGR_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[2], 2, 0},
    /* { MMU_MTRO_CONFIGr, 0, &mmu_mtro_config_fld_lst[1], 1, 0}, */
    /* { MMU_WRED_REFRESH_CONTROLr, 0, &mmu_wred_refresh_control_fld_lst[1], 1, 0}, */
    { DLB_LAG_REFRESH_DISABLEr, 0, &dlb_refresh_disable_fld_lst[1], 1, 0},
    { DLB_ECMP_REFRESH_DISABLEr, 0, &dlb_refresh_disable_fld_lst[1], 1, 0}
    /* add new items here */
};

/* Configure ING_DII_SER_SCAN_CONFIGr to start H/W TCAM SER scan engine (non-cmic). */
static bcmptm_ser_fld_info_t ing_dii_ser_scan_config_fld_lst[] = {
    { START_IDXf, {0}, 0, 0},
    { MAX_IDXf, {0}, 1023, 0},
    { OP_BLOCKf, {0}, 0, 0},
    { INTERVALf, {0}, 64, 0},
    /* {START_BLOCK_OPf, ASSIGN_ARRAY_VAL(1), 0},
    {END_BLOCK_OPf, ASSIGN_ARRAY_VAL(1), 0}, */
    { ENABLEf, {0}, 1, 0},
    { ENABLEf, {0}, 0, 0}
};

/* Configure EGR_DII_SER_SCAN_CONFIGr to start H/W TCAM SER scan engine (non-cmic). */
static bcmptm_ser_fld_info_t egr_dii_ser_scan_config_fld_lst[] = {
    { START_IDXf, {0}, 0, 0},
    { MAX_IDXf, {0}, 1023, 0},
    { OP_BLOCKf, {0}, 0, 0},
    { INTERVALf, {0}, 640, 0},
    { ENABLEf, {0}, 1, 0},
    { ENABLEf, {0}, 0, 0},
};

/* Configure ISEC_SER_SCAN_CONFIGr to start H/W TCAM SER scan engine (non-cmic). */
static bcmptm_ser_fld_info_t isec_ser_scan_config_fld_lst[] = {
    { ENABLEf, {0}, 1, 1},
    { ENABLEf, {0}, 0, 0},
};

/*
 * List of control registers which are needed to be configured, in order to
 * start H/W TCAM ser scan engine.
 */
static bcmptm_ser_ctrl_reg_info_t bcm56780_a0_tcam_scan_ctrl_reg_lst_en[] = {
    /* { ING_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[2], 1, 0}, */
    { ING_DII_SER_SCAN_CONFIGr, 0, &ing_dii_ser_scan_config_fld_lst[0], 4, 0},
    { ING_DII_SER_SCAN_CONFIGr, 0, &ing_dii_ser_scan_config_fld_lst[4], 1, 0},
    { EGR_DII_SER_SCAN_CONFIGr, 0, &egr_dii_ser_scan_config_fld_lst[0], 4, 0},
    { EGR_DII_SER_SCAN_CONFIGr, 0, &egr_dii_ser_scan_config_fld_lst[4], 1, 0},
    { ISEC_SER_SCAN_CONFIGr, 0, &isec_ser_scan_config_fld_lst[0], 1, 0}
};

/*
 * List of control registers which are needed to be configured, in order to
 * stop H/W TCAM ser scan engine.
 */
static bcmptm_ser_ctrl_reg_info_t bcm56780_a0_tcam_scan_ctrl_reg_lst_dis[] = {
    /* { ING_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[0], 1, 0}, */
    { ING_DII_SER_SCAN_CONFIGr, 0, &ing_dii_ser_scan_config_fld_lst[0], 4, 0},
    { ING_DII_SER_SCAN_CONFIGr, 0, &ing_dii_ser_scan_config_fld_lst[5], 1, 0},
    { EGR_DII_SER_SCAN_CONFIGr, 0, &egr_dii_ser_scan_config_fld_lst[0], 4, 0},
    { EGR_DII_SER_SCAN_CONFIGr, 0, &egr_dii_ser_scan_config_fld_lst[5], 1, 0}
};

/*! Can't enable parity checking for some memory tables. */
static bcmdrd_sid_t  bcm56780_a0_mem_ser_mute[] = {
    /* Must */
    INVALIDr
};

/*!
 * Do not need to enable DROP_PKT for some PTs.
 * Take EGR_METADATA_BITP_PROFILEm for example,
 * its control register is EGR_METADATA_BITP_PROFILE_SER_CONTROLr, we don't need to
 * enable field SER_PKT_DROP_EN of this control register for EGR_METADATA_BITP_PROFILEm.
 */
static bcmdrd_sid_t  bcm56780_a0_ctrl_reg_drop_ptk_mute[] = {
    EGR_METADATA_BITP_PROFILE_SER_CONTROLr,
    EPOST_EGR_DOP_STORAGE_MEM_SER_CONTROLr,
    EPOST_EGR_PERQ_XMT_COUNTERS_0_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_0_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_1_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_2_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_3_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_4_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_5_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_6_SER_CONTROLr,
    FLEX_CTR_EGR_COUNTER_TABLE_7_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_0_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_1_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_2_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_3_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_4_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_5_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_6_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_7_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_8_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_9_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_10_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_11_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_12_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_13_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_14_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_15_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_16_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_17_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_18_SER_CONTROLr,
    FLEX_CTR_ING_COUNTER_TABLE_19_SER_CONTROLr,
    IFP_METER_STORM_CONTROL_METER_TABLE_SER_CONTROLr,
    IPOST_SER_STATUS_BLK_ING_DOP_STORAGE_MEM_SER_CONTROLr,
    IPOST_TRACE_DROP_EVENT_HIGHEST_DROP_COUNTER_SER_CONTROLr,
    EPOST_TRACE_DROP_EVENT_EGR_HIGHEST_DROP_COUNTER_SER_CONTROLr,
    /* Last one */
    INVALIDr
};

/* Field list of status registers for  BCMPTM_SER_INFO_TYPE_ECC, except ECC_ERR. */
static bcmdrd_fid_t bcm56780_a0_ecc_status_fields[] = {
    MULTIPLE_ERRf,
    ENTRY_IDXf,
    DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

/* Field list of registers which are used to enable SER function. */
static bcmdrd_fid_t bcm56780_a0_edb_ctrl_buffer_ecc_en_fields[] = {
    PM3_ECC_ENf,
    PM2_ECC_ENf,
    PM1_ECC_ENf,
    PM0_ECC_ENf,
    CM_ECC_ENf,
    LPBK_ECC_ENf,
    XMIT_START_COUNT_ECC_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_edb_data_buffer_en_fields[] = {
    PM3_ECC_ENf,
    PM2_ECC_ENf,
    PM1_ECC_ENf,
    PM0_ECC_ENf,
    RESIDUAL_ECC_ENf,
    CM_ECC_ENf,
    LPBK_ECC_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_edb_ctrl_buffer_1b_en_fields[] = {
    PM3_ENf,
    PM2_ENf,
    PM1_ENf,
    PM0_ENf,
    CM_ENf,
    LPBK_ENf,
    XMIT_START_COUNT_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_edb_data_buffer_1b_en_fields[] = {
    PM3_ENf,
    PM2_ENf,
    PM1_ENf,
    PM0_ENf,
    RESIDUAL_ENf,
    CM_ENf,
    LPBK_ENf,
    INVALIDf /* must */
};

/*
 * In bcm56780_a0_ser_hwmem_base_info, we need to use 'membase' to find 'bd_name'.
 * And then use 'bd_name' to find SER controlling register
 * to disable SER checking if there is hard fault in internal buffers or buses.
 */
static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_obm0_status_reg[] = {
    { IDB_OBM0_DATA_ECC_STATUSr, "IDB_OBM0_DATA_ECC_STATUS", 1, 0 },
    { IDB_OBM0_CTRL_ECC_STATUSr, "IDB_OBM0_CTRL_ECC_STATUS", 0, 0 },
    { IDB_OBM0_OVERSUB_MON_ECC_STATUSr, "IDB_OBM0_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_obm1_status_reg[] = {
    { IDB_OBM1_DATA_ECC_STATUSr, "IDB_OBM1_DATA_ECC_STATUS", 1, 1 },
    { IDB_OBM1_CTRL_ECC_STATUSr, "IDB_OBM1_CTRL_ECC_STATUS", 0, 1 },
    { IDB_OBM1_OVERSUB_MON_ECC_STATUSr, "IDB_OBM1_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_obm2_status_reg[] = {
    { IDB_OBM2_DATA_ECC_STATUSr, "IDB_OBM2_DATA_ECC_STATUS", 1, 2 },
    { IDB_OBM2_CTRL_ECC_STATUSr, "IDB_OBM2_CTRL_ECC_STATUS", 0, 2 },
    { IDB_OBM2_OVERSUB_MON_ECC_STATUSr, "IDB_OBM2_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_obm3_status_reg[] = {
    { IDB_OBM3_DATA_ECC_STATUSr, "IDB_OBM3_DATA_ECC_STATUS", 1, 3 },
    { IDB_OBM3_CTRL_ECC_STATUSr, "IDB_OBM3_CTRL_ECC_STATUS", 0, 3 },
    { IDB_OBM3_OVERSUB_MON_ECC_STATUSr, "IDB_OBM3_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};
static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_obm4_status_reg[] = {
    { IDB_OBM4_DATA_ECC_STATUSr, "IDB_OBM4_DATA_ECC_STATUS", 1, 3 },
    { IDB_OBM4_CTRL_ECC_STATUSr, "IDB_OBM4_CTRL_ECC_STATUS", 0, 3 },
    { IDB_OBM4_OVERSUB_MON_ECC_STATUSr, "IDB_OBM4_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca0_status_reg[] = {
    { IDB_CA_ECC_STATUSr, "IDB_CA_ECC_STATUS", 2, 0 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca1_status_reg[] = {
    { IDB_CA_ECC_STATUSr, "IDB_CA_ECC_STATUS", 2, 1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca2_status_reg[] = {
    { IDB_CA_ECC_STATUSr, "IDB_CA_ECC_STATUS", 2, 2 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca3_status_reg[] = {
    { IDB_CA_ECC_STATUSr, "IDB_CA_ECC_STATUS", 2, 3 },
    { INVALIDr, "INVALID" }
};
static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca4_status_reg[] = {
    { IDB_CA_ECC_STATUSr, "IDB_CA_ECC_STATUS", 2, 3 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca_cpu_status_reg[] = {
    { IDB_CA_CPU_ECC_STATUSr, "IDB_CA_CPU_ECC_STATUS", 3, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_idb_ca_lpbk_status_reg[] = {
    { IDB_CA_LPBK_ECC_STATUSr, "IDB_CA_LPBK_ECC_STATUS", 4, -1 },
    { INVALIDr, "INVALID" }
};

static bcmdrd_fid_t bcm56780_a0_idb_ca_lpbk_en_fields[] = {
    CA_LPBK_FIFO_ECC_ENABLEf,
    CA_LPBK_FIFO_EN_COR_ERR_RPTf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_idb_ca_cpu_en_fields[] = {
    CA_CPU_FIFO_ECC_ENABLEf,
    CA_CPU_FIFO_EN_COR_ERR_RPTf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_idb_ca_fifo_en_fields[] = {
    CA_FIFO_EN_COR_ERR_RPTf,
    CA_FIFO_ECC_ENABLEf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_idb_obm_en_fields[] = {
    MON_EN_COR_ERR_RPTf,
    MON_ECC_ENABLEf,
    CTRL_EN_COR_ERR_RPTf,
    CTRL_ECC_ENABLEf,
    DATA_EN_COR_ERR_RPTf,
    DATA_ECC_ENABLEf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_ser_sec_en_fields[] = {
    ESEC_SC_TABLE_SER_EVENTf,
    ESEC_SA_TABLE_SER_EVENTf,
    ISEC_SC_MAP_SER_EVENTf,
    ISEC_SP_MAP_SER_EVENTf,
    ISEC_SA_TABLE_SER_EVENTf,
    ISEC_SC_TABLE_SER_EVENTf,
    ISEC_SP_POLICY_SER_EVENTf,
    ESEC_HASH_TABLE_SER_EVENTf,
    ISEC_HASH_TABLE_SER_EVENTf,
    MIB_SER_EVENTf,
    ISEC_SC_TCAM_SER_EVENTf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_ecc_sec_en_fields[] = {
    EG_SC_TBL_ECC_ENf,
    EG_SA_TBL_ECC_ENf,
    IG_SC_MAP_TBL_ECC_ENf,
    IG_SP_MAP_TBL_ECC_ENf,
    IG_SA_TBL_ECC_ENf,
    IG_SC_TBL_ECC_ENf,
    IG_SP_POL_TBL_ECC_ENf,
    EG_SA_TBL_ECC_ENf,
    IG_SA_TBL_ECC_ENf,
    MIB_ECC_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_tcam_sec_en_fields[] = {
    SC_TCAM_SER_ENf,
    SP_TCAM_SER_ENf,
    INVALIDf /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_cdmac_ecc_tx_1bit_flds[] = {
    { TX_CDC_SINGLE_BIT_ERRf, INVALIDf, 1, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_cdmac_ecc_tx_2bit_flds[] = {
    { TX_CDC_DOUBLE_BIT_ERRf, INVALIDf, 0, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_cdmac_ecc_mib_1bit_flds[] = {
    { MIB_COUNTER_SINGLE_BIT_ERRf, INVALIDf, 1, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_cdmac_ecc_mib_2bit_flds[] = {
    { MIB_COUNTER_DOUBLE_BIT_ERRf, INVALIDf, 0, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_cdmac_ecc_mib_multi_flds[] = {
    { MIB_COUNTER_MULTIPLE_ERRf, INVALIDf, 2, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmdrd_fid_t bcm56780_a0_cdmac_ecc_ctrl_flds[] = {
    TX_CDC_ECC_CTRL_ENf,
    MIB_COUNTER_ECC_CTRL_ENf,
    INVALIDf /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC", -1, -1 },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56780_a0_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC", -1, -1 },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_xlmac_rx_cdc_1bit_err[] = {
    { RX_CDC_SINGLE_BIT_ERRf, CLEAR_RX_CDC_SINGLE_BIT_ERRf, 1, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_xlmac_rx_cdc_2bit_err[] = {
    { RX_CDC_DOUBLE_BIT_ERRf, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, 0, BCMPTM_SER_CLR_INTR_RAISE_EDGE},
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_xlmac_tx_cdc_1bit_err[] = {
    { TX_CDC_SINGLE_BIT_ERRf, CLEAR_TX_CDC_SINGLE_BIT_ERRf, 1, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56780_a0_xlmac_tx_cdc_2bit_err[] = {
    { TX_CDC_DOUBLE_BIT_ERRf, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, 0, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmdrd_fid_t bcm56780_a0_xlmac_ecc_ctrl_flds[] = {
    TX_CDC_ECC_CTRL_ENf,
    RX_CDC_ECC_CTRL_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_xlport_ecc_ctrl_flds[] = {
    MIB_TSC_MEM_ENf,
    MIB_RSC_MEM_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56780_a0_idb_intr_en_flds[] = {
    LPBK_FIFO_ECC_ERRf,
    CPU_FIFO_ECC_ERRf,
    CA3_FIFO_ECC_ERRf,
    OBM3_ECC_ERRf,
    CA2_FIFO_ECC_ERRf,
    OBM2_ECC_ERRf,
    CA1_FIFO_ECC_ERRf,
    OBM1_ECC_ERRf,
    CA0_FIFO_ECC_ERRf,
    OBM0_ECC_ERRf,
    INVALIDf /* must */
};

static bcmptm_ser_ecc_status_fld_t bcm56780_a0_ecc_status_flds = {
    ECC_ERRf, bcm56780_a0_ecc_status_fields
};

static bcmptm_ser_ctrl_en_t bcm56780_a0_ser_blocks_ctrl_regs[] = {

    { EPOST_EGR_INTR_ENABLEr, ENABLEf, NULL },

    { IPOST_SER_STATUS_BLK_ING_INTR_ENABLEr, ENABLEf, NULL },

    { CDMAC_ECC_CTRLr, INVALIDf, bcm56780_a0_cdmac_ecc_ctrl_flds },

    { XLMAC_ECC_CTRLr, INVALIDf, bcm56780_a0_xlmac_ecc_ctrl_flds },

    { XLPORT_ECC_CONTROLr, INVALIDf, bcm56780_a0_xlport_ecc_ctrl_flds },

    { EDB_INTR_ENABLEr, SER_FIFO_NON_EMPTYf, NULL },

    { EDB_CONTROL_BUFFER_ECC_ENr, INVALIDf, bcm56780_a0_edb_ctrl_buffer_ecc_en_fields },

    { EDB_DATA_BUFFER_ECC_ENr, INVALIDf, bcm56780_a0_edb_data_buffer_en_fields },

    { EDB_CONTROL_BUFFER_EN_COR_ERR_RPTr, INVALIDf, bcm56780_a0_edb_ctrl_buffer_1b_en_fields },

    { EDB_DATA_BUFFER_EN_COR_ERR_RPTr, INVALIDf, bcm56780_a0_edb_data_buffer_1b_en_fields },

    { IDB_INTR_ENABLEr, INVALIDf, bcm56780_a0_idb_intr_en_flds },

    { IDB_CA_LPBK_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_ca_lpbk_en_fields },

    { IDB_CA_CPU_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_ca_cpu_en_fields },

    { IDB_CA3_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_ca_fifo_en_fields },

    { IDB_OBM3_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_obm_en_fields },

    { IDB_CA2_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_ca_fifo_en_fields },

    { IDB_OBM2_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_obm_en_fields },

    { IDB_CA1_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_ca_fifo_en_fields },

    { IDB_OBM1_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_obm_en_fields },

    { IDB_CA0_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_ca_fifo_en_fields },

    { IDB_OBM0_SER_CONTROLr, INVALIDf, bcm56780_a0_idb_obm_en_fields },

    { MACSEC_INTR_ENABLEr, INVALIDf, bcm56780_a0_ser_sec_en_fields},

    { MACSEC_ECC_CTRLr, INVALIDf, bcm56780_a0_ecc_sec_en_fields},

    { ISEC_SER_CONTROLr, INVALIDf, bcm56780_a0_tcam_sec_en_fields} ,
};

/*
 * field list of memory ING_DOI_SER_FIFOm/IPOST_SER_STATUS_BLK_ING_SER_FIFOm
 * DLB_ECMP_SER_FIFOm/EPOST_EGR_SER_FIFOm/EGR_DOI_SER_FIFOm.
 */
bcmdrd_fid_t  bcm56780_a0_fs_ser_status_bus[] = {
    VALIDf,
    ECC_PARITYf,
    MEM_TYPEf,
    ADDRESSf,
    PIPE_STAGEf,
    INSTRUCTION_TYPEf,
    DROPf,
    MULTIPLEf,
    NON_SBUSf,
    XFS_MEMBASEf,
    XFS_MEMINDEXf,
    XFS_REGBASEf,
    XFS_REGINDEXf,
    INVALIDf
};

/*
 * field list of memory EDB_SER_FIFOm.
 */
bcmdrd_fid_t  bcm56780_a0_ser_status_bus[] = {
    VALIDf,
    ECC_PARITYf,
    MEM_TYPEf,
    ADDRESSf,
    PIPE_STAGEf,
    INSTRUCTION_TYPEf,
    DROPf,
    MULTIPLEf,
    NON_SBUSf,
    MEMBASEf,
    MEMINDEXf,
    REGBASEf,
    REGINDEXf,
    INVALIDf
};

/*
 * fields of MMU_SCFG_MEM_FAIL_ADDR_64m/ MMU_XCFG_MEM_FAIL_ADDR_64m/
 * MMU_GCFG_MEM_FAIL_ADDR_64m.
 * Sequence of fields cannot be changed.
 */
static bcmdrd_fid_t bcm56780_a0_mem_fail_addr_flds[] = {
    ERR_MULTf,
    ERR_TYPEf,
    EADDRf
};

static bcmptm_serc_non_fifo_t bcm56780_a0_serc_idb_info[] = {
    {
        IDB_INTR_STATUSr, LPBK_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca_lpbk_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA-LPBK packet buffer memory"
    },
    {
        IDB_INTR_STATUSr, CPU_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca_cpu_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA-CPU packet buffer memory"
    },
    {
        IDB_INTR_STATUSr, CA4_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca4_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA4 FIFO"
    },
    {
        IDB_INTR_STATUSr, OBM3_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_obm4_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM4 packet data memory"
    },
    {
        IDB_INTR_STATUSr, CA3_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca3_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA3 FIFO"
    },
    {
        IDB_INTR_STATUSr, OBM3_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_obm3_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM3 packet data memory"
    },
    {
        IDB_INTR_STATUSr, CA2_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca2_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA2 FIFO"
    },
    {
        IDB_INTR_STATUSr, OBM2_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_obm2_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM2 packet data memory"
    },
    {
        IDB_INTR_STATUSr, CA1_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca1_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA1 FIFO"
    },
    {
        IDB_INTR_STATUSr, OBM1_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_obm1_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM1 packet data memory"
    },
    {
        IDB_INTR_STATUSr, CA0_FIFO_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_ca0_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA0 FIFO"
    },
    {
        IDB_INTR_STATUSr, OBM0_ECC_ERRf,
        1, INVALIDr, bcm56780_a0_idb_obm0_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM0 packet data memory"
    },
};

static bcmptm_serc_fifo_t bcm56780_a0_serc_edb_info[] = {
    {
        EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        EDB_SER_FIFOm, bcm56780_a0_ser_status_bus,
        INVALIDr, BLKTYPE_PORT_IF, "PORT_IF_EDB"
    }
};

static bcmptm_serc_fifo_t bcm56780_a0_serc_mmu_info[] = {
    {
        MMU_GLBCFG_MEM_SER_FIFO_STSr, EMPTYf,
        MMU_GLBCFG_MEM_FAIL_ADDR_64m, bcm56780_a0_mem_fail_addr_flds,
        MMU_GLBCFG_MEM_FAIL_INT_CTRr, BLKTYPE_MMU_GLB, "MMU_GLB"
    },
    {
        MMU_ITMCFG_MEM_SER_FIFO_STSr, EMPTYf,
        MMU_ITMCFG_MEM_FAIL_ADDR_64m, bcm56780_a0_mem_fail_addr_flds,
        MMU_ITMCFG_MEM_FAIL_INT_CTRr, BLKTYPE_MMU_ITM, "MMU_ITM"
    },
    {
        MMU_EBCFG_MEM_SER_FIFO_STSr, EMPTYf,
        MMU_EBCFG_MEM_FAIL_ADDR_64m, bcm56780_a0_mem_fail_addr_flds,
        MMU_EBCFG_MEM_FAIL_INT_CTRr, BLKTYPE_MMU_EB, "MMU_EB"
    }
};

static bcmptm_serc_fifo_t bcm56780_a0_serc_ep_info[] = {
    {
        INVALIDr, INVALIDf,
        EGR_DOI_SER_FIFOm, bcm56780_a0_fs_ser_status_bus,
        INVALIDr, BLKTYPE_EPIPE, "EPIPE"
    },
    {
        EPOST_EGR_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        EPOST_EGR_SER_FIFOm, bcm56780_a0_fs_ser_status_bus,
        INVALIDr, BLKTYPE_EPIPE, "EPIPE_EPOST"
    }
};

static bcmptm_serc_fifo_t bcm56780_a0_serc_ip_info[] = {
    {
        IPOST_SER_STATUS_BLK_ING_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        IPOST_SER_STATUS_BLK_ING_SER_FIFOm, bcm56780_a0_fs_ser_status_bus,
        INVALIDr, BLKTYPE_IPIPE, "IPIPE_IPOST"
    },
    {
        INVALIDr, INVALIDf,
        ING_DOI_SER_FIFOm, bcm56780_a0_fs_ser_status_bus,
        INVALIDr, BLKTYPE_IPIPE, "IPIPE"
    }
};

static bcmptm_serc_fifo_t bcm56780_a0_serc_ip_dlb_info[] = {
    {
        DLB_ECMP_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        DLB_ECMP_SER_FIFOm, bcm56780_a0_fs_ser_status_bus,
        INVALIDr, BLKTYPE_IPIPE, "IPIPE_DLB"
    }
};

/* CDMAC_ECC_STATUSr is cleared-on-read */
static bcmptm_serc_non_fifo_t bcm56780_a0_serc_cdp_info[] = {
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56780_a0_cdmac_ecc_tx_1bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56780_a0_cdmac_ecc_tx_2bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56780_a0_cdmac_ecc_mib_1bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56780_a0_cdmac_ecc_mib_2bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56780_a0_cdmac_ecc_mib_multi_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    }
};

static bcmptm_serc_non_fifo_t bcm56780_a0_serc_xlp_info[] = {
    {
        INVALIDr, INVALIDf,
        0, XLMAC_RX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56780_a0_xlmac_rx_cdc_1bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC RX 1 bit error"
    },
    {
        INVALIDr, INVALIDf,
        0, XLMAC_RX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56780_a0_xlmac_rx_cdc_2bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC RX 2 bit error"
    },
    {
        INVALIDr, INVALIDf,
        0, XLMAC_TX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56780_a0_xlmac_tx_cdc_1bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC TX 1 bit error"
    },
    {
        INVALIDr, INVALIDf,
        0, XLMAC_TX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56780_a0_xlmac_tx_cdc_2bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC TX 2 bit error"
    },
    /* W1TC */
    {
        XLPORT_INTR_STATUSr, MIB_TX_MEM_ERRf,
        1, INVALIDr, bcm56780_a0_pm_xlp_tx_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_XLPORT, "XLP MIB TX Statistic Counter memory"
    },
    /* W1TC */
    {
        XLPORT_INTR_STATUSr, MIB_RX_MEM_ERRf,
        1, INVALIDr, bcm56780_a0_pm_xlp_rx_status_reg, INVALIDr,
        &bcm56780_a0_ecc_status_flds, NULL, NULL,
        BLKTYPE_XLPORT, "XLP MIB RX Statistic Counter memory"
    }
};

static bcmdrd_fid_t bcm56780_a0_sec_tcam_status_fields[] = {
    SER_BUSf,
    INVALIDf /* must */
};

static bcmptm_ser_ecc_status_fld_t bcm56780_a0_isec_tcam_status_flds = {
    INVALIDf, bcm56780_a0_sec_tcam_status_fields
};

/* Field list of status registers for  BCMPTM_SER_INFO_TYPE_ECC, except ECC_ERR. */
static bcmdrd_fid_t bcm56780_a0_sec_status_fields[] = {
    SINGLE_BIT_ERRf,
    DOUBLE_BIT_ERRf,
    MULTIPLE_ERRf,
    ECC_ERR_ADDRESSf,
    INVALIDf /* must */
};

static bcmptm_ser_ecc_status_fld_t bcm56780_a0_sec_ecc_status_flds = {
    INVALIDf, bcm56780_a0_sec_status_fields
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_misc_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_MISC_SINGLE_BIT_ERRORf, 0 , 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_misc_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_MISC_DOUBLE_BIT_ERRORf, 0, 0 },
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sc_unctrl_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SC_UNCTRL_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sc_unctrl_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SC_UNCTRL_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sc_ctrl_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SC_CTRL_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sc_ctrl_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SC_CTRL_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sc_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SC_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sc_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SC_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sa_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SA_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_esec_mib_sa_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ESEC_MIB_SA_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sptcam_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_SPTCAM_HIT_COUNT_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sptcam_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_SPTCAM_HIT_COUNT_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sctcam_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_SCTCAM_HIT_COUNT_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sctcam_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_SCTCAM_HIT_COUNT_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_port_counters_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_PORT_COUNTERS_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_port_counters_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_PORT_COUNTERS_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_unctrl_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SP_UNCTRL_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_unctrl_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SP_UNCTRL_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_ctrl1_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SP_CTRL_1_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_ctrl1_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SP_CTRL_1_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_ctrl2_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SP_CTRL_2_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_ctrl2_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SP_CTRL_2_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sc_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SC_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sc_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SC_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sa_1_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SA_SINGLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_mib_sa_2_status_flds[] = {
    { MACSEC_MIB_INTR_STATUSr, ISEC_MIB_SA_DOUBLE_BIT_ERRORf, 0, 0},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sc_tcam_corrupt[] = {
    { ISEC_SER_CONTROLr, SC_TCAM_ONE_BIT_ERR_INJECTf, 0, 1},
    { INVALIDf, INVALIDf },
};

static bcmptm_ser_sec_status_fld_t bcm56780_a0_isec_sp_tcam_corrupt[] = {
    { ISEC_SER_CONTROLr, SP_TCAM_ONE_BIT_ERR_INJECTf, 0, 1},
    { INVALIDf, INVALIDf },
};

static bcmptm_serc_non_fifo_t bcm56780_a0_serc_sec_info[] = {
    {
        MACSEC_INTR_STATUSr, ESEC_SC_TABLE_SER_EVENTf,
        2, ESEC_SC_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ESEC SC table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ESEC_SA_TABLE_SER_EVENTf,
        2, ESEC_SA_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ESEC SA table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SC_MAP_SER_EVENTf,
        2, ISEC_SC_MAP_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC SC MAP ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SP_MAP_SER_EVENTf,
        2, ISEC_SP_MAP_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC SP MAP ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SA_TABLE_SER_EVENTf,
        2, ISEC_SA_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC SA Table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SC_TABLE_SER_EVENTf,
        2, ISEC_SC_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC SC Table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SP_POLICY_SER_EVENTf,
        2, ISEC_SP_POLICY_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC SP POLICY Table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ESEC_HASH_TABLE_SER_EVENTf,
        2, ESEC_HASH_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ESEC HASH table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_HASH_TABLE_SER_EVENTf,
        2, ISEC_HASH_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC HASH table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_HASH_TABLE_SER_EVENTf,
        2, ISEC_HASH_TABLE_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL, NULL,
        BLKTYPE_MACSEC, "ISEC HASH table ser event"
    },
    /* Entries with same ECC_STATUS type must be consecutive. */
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_MISC_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_misc_1_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB MISC Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_MISC_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_misc_2_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB MISC Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SC_UNCTRL_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sc_unctrl_1_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SC UNCTRL Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SC_UNCTRL_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sc_unctrl_2_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SC UNCTRL Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SC_CTRL_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sc_ctrl_1_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SC CTRL Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SC_CTRL_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sc_ctrl_2_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SC CTRL Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SC_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sc_1_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SC Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SC_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sc_2_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SC Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sa_1_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SA Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ESEC_MIB_SA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_esec_mib_sa_2_status_flds,
        BLKTYPE_MACSEC, "ESEC MIB SA Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_SPTCAM_HIT_COUNT_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sptcam_1_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SPTCAM Hit Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_SPTCAM_HIT_COUNT_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sptcam_2_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SPTCAM Hit Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_SCTCAM_HIT_COUNT_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sctcam_1_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SCTCAM Hit Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_SCTCAM_HIT_COUNT_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sctcam_2_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SCTCAM Hit Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_PORT_COUNTERS_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_port_counters_1_status_flds,
        BLKTYPE_MACSEC, "ISEC PORT COUNTERS Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_PORT_COUNTERS_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_port_counters_2_status_flds,
        BLKTYPE_MACSEC, "ISEC PORT COUNTERS Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SP_UNCTRL_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_sc_unctrl_1_status_flds,
        BLKTYPE_MACSEC, "ISEC SP UNCTRL Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SP_UNCTRL_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_sc_unctrl_2_status_flds,
        BLKTYPE_MACSEC, "ISEC SP UNCTRL  Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SP_CTRL_1_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_sc_ctrl1_1_status_flds,
        BLKTYPE_MACSEC, "ISEC SP CTRL 1 Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SP_CTRL_1_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_sc_ctrl1_2_status_flds,
        BLKTYPE_MACSEC, "ISEC SP CTRL 1 Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SP_CTRL_2_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_sc_ctrl2_1_status_flds,
        BLKTYPE_MACSEC, "ISEC SP CTRL 2 Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SP_CTRL_2_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_sc_ctrl2_2_status_flds,
        BLKTYPE_MACSEC, "ISEC SP CTRL 2 Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SC_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sc_1_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SC Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SC_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sc_2_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SC Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sa_1_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SA Single table ser event"
    },
    {
        MACSEC_INTR_STATUSr, MIB_SER_EVENTf,
        2, ISEC_MIB_SA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56780_a0_sec_ecc_status_flds, NULL,
        bcm56780_a0_isec_mib_sa_2_status_flds,
        BLKTYPE_MACSEC, "ISEC MIB SA Double table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SC_TCAM_SER_EVENTf,
        2, ISEC_SCTCAM_SER_STATUSr, NULL, ISEC_SER_CONTROLr,
        &bcm56780_a0_isec_tcam_status_flds, NULL,
        bcm56780_a0_isec_sc_tcam_corrupt,
        BLKTYPE_MACSEC, "ISEC SC TCAM table ser event"
    },
    {
        MACSEC_INTR_STATUSr, ISEC_SP_TCAM_SER_EVENTf,
        2, ISEC_SPTCAM_SER_STATUSr, NULL, ISEC_SER_CONTROLr,
        &bcm56780_a0_isec_tcam_status_flds, NULL,
        bcm56780_a0_isec_sp_tcam_corrupt,
        BLKTYPE_MACSEC, "ISEC SP TCAM table ser event"
    },
};

static bcmptm_ser_intr_route_info_t bcm56780_a0_intr_map[] = {
    {
        BCMPTM_SER_MMU_INTR_FLG,
        { MMU_INTR_GLB_MEM_PAR_ERR, MMU_INTR_ITM_MEM_PAR_ERR, MMU_INTR_EB_MEM_PAR_ERR },
        MMU_TO_CMIC_INTR, MMU_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_MMU,
        (void *)bcm56780_a0_serc_mmu_info, COUNTOF(bcm56780_a0_serc_mmu_info), 1
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        { EGR_DOI_SER_FIFO_NON_EMPTY, EPOST_EGR_SER_FIFO_NON_EMPTY },
         PIPE0_EP_TO_CMIC_INTR,  PIPE1_EP_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_EPIPE,
        (void *)bcm56780_a0_serc_ep_info, COUNTOF(bcm56780_a0_serc_ep_info), 1
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        { IPOST_SER_STATUS_BLK_ING_SER_FIFO_NON_EMPTY, ING_DOI_SER_FIFO_NON_EMPTY },
         PIPE0_EP_TO_CMIC_INTR,  PIPE1_EP_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_IPIPE,
        (void *)bcm56780_a0_serc_ip_info, COUNTOF(bcm56780_a0_serc_ip_info), 1
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        { DLB_ECMP_INTR },
        DLB_CREDIT_TO_CMIC_INTR, DLB_CREDIT_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_IPIPE_DLB_ECMP,
        (void *)bcm56780_a0_serc_ip_dlb_info, COUNTOF(bcm56780_a0_serc_ip_dlb_info), 1
    },
    {
        BCMPTM_SER_NON_SUB_LVL_INT,
        { BCMPTM_SER_NON_SUB_LVL_INT },
        PIPE0_EDB_TO_CMIC_PERR_INTR, PIPE3_EDB_TO_CMIC_PERR_INTR, 0,
        BCMPTM_SER_INFO_TYPE_PORT_IF_EDB,
        (void *)bcm56780_a0_serc_edb_info, COUNTOF(bcm56780_a0_serc_edb_info), 1
    },
    {
        BCMPTM_SER_NON_SUB_LVL_INT,
        { BCMPTM_SER_NON_SUB_LVL_INT },
        PIPE0_IDB_TO_CMIC_PERR_INTR, PIPE3_IDB_TO_CMIC_PERR_INTR, 0,
        BCMPTM_SER_INFO_TYPE_PORT_IF_IDB,
        (void *)bcm56780_a0_serc_idb_info, COUNTOF(bcm56780_a0_serc_idb_info), 0
    },
    /* Use to clear SER error */
    {
        BCMPTM_SER_PORT_INTR_FLG,
        {
            CDMAC_INTR_TX_CDC_SINGLE_BIT_ERR, CDMAC_INTR_TX_CDC_DOUBLE_BIT_ERR,
            CDMAC_INTR_MIB_COUNTER_SINGLE_BIT_ERR, CDMAC_INTR_MIB_COUNTER_DOUBLE_BIT_ERR,
            CDMAC_INTR_MIB_COUNTER_MULTIPLE_ERR
        },
        PM0_INTR, PM19_INTR, 0,
        BCMPTM_SER_INFO_TYPE_CDPORT,
        (void *)bcm56780_a0_serc_cdp_info, COUNTOF(bcm56780_a0_serc_cdp_info), 0
    },
    {
        BCMPTM_SER_PORT_INTR_FLG,
        {
            XLMAC_INTR_SUM_RX_CDC_SINGLE_BIT_ERR, XLMAC_INTR_SUM_RX_CDC_DOUBLE_BIT_ERR,
            XLMAC_INTR_SUM_TX_CDC_SINGLE_BIT_ERR, XLMAC_INTR_SUM_TX_CDC_DOUBLE_BIT_ERR,
            XLPORT_INTR_MIB_TX_MEM_ERR, XLPORT_INTR_MIB_RX_MEM_ERR
        },
        PM_MGMT_INTR, PM_MGMT_INTR, 0,
        BCMPTM_SER_INFO_TYPE_XLPORT,
        (void *)bcm56780_a0_serc_xlp_info, COUNTOF(bcm56780_a0_serc_xlp_info), 0
    },
    {
        BCMPTM_SER_NON_SUB_LVL_INT,
        { BCMPTM_SER_NON_SUB_LVL_INT },
        MACSEC0_TO_CMIC_PERR_INTR, MACSEC5_TO_CMIC_PERR_INTR, 0,
        BCMPTM_SER_INFO_TYPE_SEC,
        (void *)bcm56780_a0_serc_sec_info, COUNTOF(bcm56780_a0_serc_sec_info), 0
    },
};

/*
 * If 'stage id' is 0xFF, that means 'membase' can identify buffer or bus.
 * The 'stage id' reported by H/W is ignored.
 * If 'stage id' is less than 0xFF, that means two or more buffers or buses share the
 * same 'membase', so both 'membase' and 'stage id' should be used to
 * identify buffer or bus.
 */
static bcmptm_ser_buffer_bus_info_t bcm56780_a0_ser_hwmem_base_info[] = {
    /* XFS */
    { "ING_DII_EVENT_FIFO_0", "ING DII 0", 186, 0xff, 1},
    { "ING_DII_EVENT_FIFO_1", "ING DII 1", 187, 0xff, 1},
    { "ING_DII_EVENT_FIFO_2", "ING DII 1", 188, 0xff, 1},
    { "ING_DII_EVENT_FIFO_3", "ING DII 1", 189, 0xff, 1},
    { "ING_DOI_OUTPUT_FIFO_0", "Ingress DOI output event fifo 0", 190, 0xff, 1},
    { "ING_DOI_OUTPUT_FIFO_1", "Ingress DOI output event fifo 1", 191, 0xff, 1},
    { "ING_DOI_OUTPUT_FIFO_2", "Ingress DOI output event fifo 2", 192, 0xff, 1},
    { "ING_DOI_OUTPUT_FIFO_3", "Ingress DOI output event fifo 3", 193, 0xff, 1},
    { "ING_DOI_SLOT_PIPELINE_0", "Ingress DOI slot pipeline 0", 194, 0xff, 0},
    { "ING_DOI_SLOT_PIPELINE_1", "Ingress DOI slot pipeline 1", 195, 0xff, 0},
    { "ING_DOI_SLOT_PIPELINE_2", "Ingress DOI slot pipeline 1", 196, 0xff, 0},
    { "ING_DOI_SLOT_PIPELINE_3", "Ingress DOI slot pipeline 1", 197, 0xff, 0},
    { "ING_DOI_PACKET_BUFFER_0", "Ingress DOI packet buffer 0", 198, 0xff, 1},
    { "ING_DOI_PACKET_BUFFER_1", "Ingress DOI packet buffer 1", 199, 0xff, 1},
    { "ING_DOI_PACKET_BUFFER_2", "Ingress DOI packet buffer 2", 200, 0xff, 1},
    { "ING_DOI_PACKET_BUFFER_3", "Ingress DOI packet buffer 3", 201, 0xff, 1},
    { "ING_DOI_CELL_QUEUE_0", "Ingress DOI cell queues 0", 202, 0xff, 1},
    { "ING_DOI_CELL_QUEUE_1", "Ingress DOI cell queues 1", 203, 0xff, 1},
    { "ING_DOI_CELL_QUEUE_2", "Ingress DOI cell queues 1", 204, 0xff, 1},
    { "ING_DOI_CELL_QUEUE_3", "Ingress DOI cell queues 1", 205, 0xff, 1},
    { "FIFO_PTHRU_EP0_0", "FIFO_PTHRU_EP0_0", 206, 0xff, 1},
    { "FIFO_PTHRU_EP0_1", "FIFO_PTHRU_EP0_1", 207, 0xff, 1},
    { "FIFO_PTHRU_EP1_1_0", "FIFO_PTHRU_EP1_1_0", 207, 0xff, 1},
    { "FIFO_PTHRU_IP0_0", "FIFO_PTHRU_IP0_0", 208, 0xff, 1},
    { "FIFO_PTHRU_IP1_1", "FIFO_PTHRU_IP1_0", 209, 0xff, 1},
    { "FIFO_PTHRU_IP1_2", "FIFO_PTHRU_IP1_0", 210, 0xff, 1},
    { "FIFO_PTHRU_IP1_3", "FIFO_PTHRU_IP1_0", 211, 0xff, 1},
    { "FIFO_PTHRU_IP3_0_0", "FIFO_PTHRU_IP3_0_0", 212, 0xff, 1},
    { "FIFO_PTHRU_IP5_0", "FIFO_PTHRU_IP5_0", 213, 0xff, 1},
    { "FIFO_PTHRU_IP5_1", "FIFO_PTHRU_IP5_1", 214, 0xff, 1},
    { "FIFO_PTHRU_IP5_2", "FIFO_PTHRU_IP5_2", 215, 0xff, 1},
    { "FIFO_PTHRU_IP8_1_0", "FIFO_PTHRU_IP8_1_0", 216, 0xff, 1},
    { "FIFO_PTHRU_IP8_1_1", "FIFO_PTHRU_IP8_1_1", 217, 0xff, 1},
    { "FIFO_PTHRU_IP9_2_0", "FIFO_PTHRU_IP9_2_0", 218, 0xff, 1},
    { "FIFO_PTHRU_IP9_2_1", "FIFO_PTHRU_IP9_2_1", 219, 0xff, 1},
    { "FT_LEARN_FIFO", "FT_LEARN_FIFO", 220, 0xff, 1},
    { "FT_PT_BUF", "FT_PT_BUF", 221, 0xff, 1},
    { "FT_NOTIFY_FIFO", "FT_NOTIFY_FIFO", 222, 0xff, 1},
    { "M_FT_LEARN_FIFO", "M_FT_LEARN_FIFO", 223, 0xff, 1},
    { "M_FT_PT_BUF", "M_FT_PT_BUF", 224, 0xff, 1},
    { "M_FT_NOTIFY_FIFO", "M_FT_NOTIFY_FIFO", 225, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_0", "Egress DOI output event fifo 0", 80, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_1", "Egress DOI output event fifo 1", 81, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_2", "Egress DOI output event fifo 2 ", 82, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_3", "Egress DOI output event fifo 3", 83, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_0", "Egress DOI slot pipeline 0", 84, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_1", "Egress DOI slot pipeline 1", 85, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_2", "Egress DOI slot pipeline 2", 86, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_3", "Egress DOI slot pipeline 3", 87, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_0", "Egress DOI packet buffer 0", 88, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_1", "Egress DOI packet buffer 1", 89, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_2", "Egress DOI packet buffer 2", 90, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_3", "Egress DOI packet buffer 3", 91, 0xff, 1},
    { "EP_PACKET_DATA_0", "Egress DOI", 92, 0xff, 1},
    { "EP_PACKET_DATA_1", "Egress DOI", 93, 0xff, 1},
    { "EP_PACKET_DATA_2", "Egress DOI", 94, 0xff, 1},
    { "EP_PACKET_DATA_3", "Egress DOI", 95, 0xff, 1},
    { "EP_PACKET_DATA_3", "Egress DOI", 95, 0xff, 1},
    { "EP_MPB_BUS_0", "eGRess DOI 0", 96, 0xff, 1},
    { "EP_MPB_BUS_1", "eGRess DOI 0", 97, 0xff, 1},
    { "EP_MPB_BUS_2", "eGRess DOI 0", 98, 0xff, 1},
    { "EP_MPB_BUS_3", "eGRess DOI 0", 99, 0xff, 1},
    { "EGR_DII_EVENT_FIFO_0", "EGR DII", 100, 0xff, 1},
    { "EGR_DII_EVENT_FIFO_1", "EGR DII", 101, 0xff, 1},
    { "EGR_DII_EVENT_FIFO_2", "EGR DII", 102, 0xff, 1},
    { "EGR_DII_EVENT_FIFO_3", "EGR DII", 103, 0xff, 1},
    { "EP_INITBUF", "Einitbuf", 104, 0xff, 1},
    { "EGR_COMPOSITES", "Egr composites buffer", 105, 0xff, 1},
    { "CENTRAL_EVICTION_FIFO", "In CEV, out of IPEP spaces", 106, 0xff, 1},

    /* IDB */
    { "OBM0_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 0, 1},
    { "OBM1_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 1, 1},
    { "OBM2_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 2, 1},
    { "OBM3_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 3, 1},
    { "OBM4_QUEUE_FIFO", "OBM_QUEUE_FIFO - In Idb", 0x0, 4, 1},
    { "OBM0_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 0, 1},
    { "OBM1_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 1, 1},
    { "OBM2_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 2, 1},
    { "OBM3_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 3, 1},
    { "OBM4_DATA_FIFO", "OBM_DATA_FIFO - In Idb", 0x1, 4, 1},
    { "CELL_ASSEM_BUFFER0", "CELL_ASSEM_BUFFER - In Idb", 0x2, 0, 1},
    { "CELL_ASSEM_BUFFER1", "CELL_ASSEM_BUFFER - In Idb", 0x2, 1, 1},
    { "CELL_ASSEM_BUFFER2", "CELL_ASSEM_BUFFER - In Idb", 0x2, 2, 1},
    { "CELL_ASSEM_BUFFER3", "CELL_ASSEM_BUFFER - In Idb", 0x2, 3, 1},
    { "CELL_ASSEM_BUFFER4", "CELL_ASSEM_BUFFER - In Idb", 0x2, 4, 1},
    { "CPU_CELL_ASSEM_BUFFER", "CPU_CELL_ASSEM_BUFFER - In Idb", 0x3, 0xff, 1},
    { "LPBK_CELL_ASSEM_BUFFER", "LPBK_CELL_ASSEM_BUFFER - In Idb", 0x4, 0xff, 1},
    /* EDB */
    { "EDB_RESIDUAL_DATA_BUF", "EDB residual data buffer", 0x67, 0xff, 1},
    { "EDB_XMIT_START_COUNT", "EDB xmit start count", 0x68, 0xff, 1},
    { "EDB_PM0_EOP_STATUS", "EDB PM0 DATA FIFO", 0x69, 0xff, 1},
    { "EDB_PM0_CTRL_FIFO", "EDB PM0 CTRL FIFO", 0x6a, 0xff, 1},
    { "EDB_PM0_DATA_FIFO", "EDB PM0 DATA FIFO", 0x6b, 0xff, 1},
    { "EDB_PM1_EOP_STATUS", "EDB PM0 DATA FIFO", 0x6c, 0xff, 1},
    { "EDB_PM1_CTRL_FIFO", "EDB PM1 CTRL FIFO", 0x6d, 0xff, 1},
    { "EDB_PM1_DATA_FIFO", "EDB PM1 DATA FIFO", 0x6e, 0xff, 1},
    { "EDB_PM2_EOP_STATUS", "EDB PM0 DATA FIFO", 0x6f, 0xff, 1},
    { "EDB_PM2_CTRL_FIFO", "EDB PM2 CTRL FIFO", 0x70, 0xff, 1},
    { "EDB_PM2_DATA_FIFO", "EDB PM2 DATA FIFO", 0x71, 0xff, 1},
    { "EDB_PM3_EOP_STATUS", "EDB PM0 DATA FIFO", 0x72, 0xff, 1},
    { "EDB_PM3_CTRL_FIFO", "EDB PM3 CTRL FIFO", 0x73, 0xff, 1},
    { "EDB_PM3_DATA_FIFO", "EDB PM3 DATA FIFO", 0x74, 0xff, 1},
    { "EDB_CM_EOP_STATUS", "EDB PM0 DATA FIFO", 0x75, 0xff, 1},
    { "EDB_CM_CTRL_FIFO", "EDB CM CTRL FIFO", 0x76, 0xff, 1},
    { "EDB_CM_DATA_FIFO", "EDB CM DATA FIFO", 0x77, 0xff, 1},
    { "EDB_LPBK_EOP_STATUS", "EDB PM0 DATA FIFO", 0x78, 0xff, 1},
    { "EDB_LPBK_CTRL_FIFO", "EDB LPBK CTRL FIFO", 0x79, 0xff, 1},
    { "EDB_LPBK_DATA_FIFO", "EDB LPBK DATA FIFO", 0x7a, 0xff, 1}
};

/* During SER error injection, Memory refresh should be disable */
static bcmptm_ser_ctrl_reg_info_t bcm56780_a0_clear_refresh_registers[] = {
    { ING_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[0], 2, 0},
    { EGR_DII_AUX_ARB_CONTROLr, 0, &dii_aux_arb_control_fld_lst[0], 2, 0},
    { DLB_ECMP_REFRESH_DISABLEr, 0, &dlb_refresh_disable_fld_lst[0], 1, 0},
    { DLB_LAG_REFRESH_DISABLEr, 0, &dlb_refresh_disable_fld_lst[0], 1, 0},
    { MMU_WRED_REFRESH_CONTROLr, 0, &mmu_wred_refresh_control_fld_lst[0], 1, 0},
    { MMU_MTRO_CONFIGr, 0, &mmu_mtro_config_fld_lst[0], 1, 0},
    { MMU_EBPTS_CBMG_VALUEr, 0, &mmu_ebpts_cbmg_value_fld_lst[0], 2, 0 },
    { MMU_QSCH_VOQ_FAIRNESS_CONFIGr, 0, &mmu_qsch_voq_fairness_fld_lst[0], 1, 0 },
    { MMU_THDO_ENGINE_ENABLES_CFGr, 0, &mmu_thdo_engine_enables_cfg_fld_lst[0], 2, 0 },
    { MACSEC_MIB_MODEr, 0, &sec_macsec_mib_mode_fld_lst[0], 1, 0 }
};

static bcmptm_ser_acctype_info_t bcm56780_a0_acctype_info[] = {
    { ACC_TYPE_SINGLE , ACC_TYPE_SINGLE_STR },
    { ACC_TYPE_UNIQUE, ACC_TYPE_UNIQUE_STR },
    { ACC_TYPE_DUPLICATE, ACC_TYPE_DUPLICATE_STR},
    { ACC_TYPE_DATA_SPLIT, ACC_TYPE_DATA_SPLIT_STR},
    { ACC_TYPE_ADDR_SPLIT, ACC_TYPE_ADDR_SPLIT_STR},
    { ACC_TYPE_ADDR_SPLIT_SPLIT, ACC_TYPE_ADDR_SPLIT_SPLIT_STR},
    { ACC_TYPE_DUPLICATE_2, ACC_TYPE_DUPLICATE_2_STR},
    { ACC_TYPE_DUPLICATE_4, ACC_TYPE_DUPLICATE_4_STR},
    { 0, "unknown" }
};

static bcmptm_ser_blk_map_t bcm56780_a0_blktype_info[] = {
    { BLKTYPE_EPIPE, BCMPTM_SER_BLK_EPIPE, { -1, -1 } },
    { BLKTYPE_IPIPE, BCMPTM_SER_BLK_IPIPE, { -1, -1 } },
    { BLKTYPE_PORT_IF, BCMPTM_SER_BLK_MMU, { -1, -1 } },
    { BLKTYPE_MMU_GLB, BCMPTM_SER_BLK_MMU, { BLKTYPE_MMU_EB, BLKTYPE_MMU_ITM } },
    { BLKTYPE_MMU_EB, BCMPTM_SER_BLK_MMU,  { -1, -1 } },
    { BLKTYPE_MMU_ITM, BCMPTM_SER_BLK_MMU, { -1, -1 } },
    { BLKTYPE_CDPORT, BCMPTM_SER_BLK_PORT, { -1, -1 } },
    { BLKTYPE_XLPORT, BCMPTM_SER_BLK_PORT, { -1, -1 } },
    { BLKTYPE_MGMT_OBM, BCMPTM_SER_BLK_PORT, { -1, -1 } },
    { BLKTYPE_MACSEC, BCMPTM_SER_BLK_SEC, { -1, -1 } }
};

static const bcmdrd_sid_t bcm56780_a0_idb_obm_mem_list[] = {
    IDB_OBM0_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM1_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM2_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM3_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM4_IOM_STATS_WINDOW_RESULTSm,
    ESEC_SC_TABLEm,
    ESEC_SA_TABLEm,
    SC_MAP_TABLEm,
    SUB_PORT_MAP_TABLEm,
    ISEC_SA_TABLEm,
    ISEC_SC_TABLEm,
    SUB_PORT_POLICY_TABLEm,
    ESEC_SA_TABLEm,
    ISEC_SA_TABLEm,
    ESEC_MIB_MISCm,
    ESEC_MIB_SC_UNCTRLm,
    ESEC_MIB_SC_CTRLm,
    ESEC_MIB_SCm,
    ESEC_MIB_SAm,
    ISEC_SPTCAM_HIT_COUNTm,
    ISEC_SCTCAM_HIT_COUNTm,
    ISEC_PORT_COUNTERSm,
    ISEC_MIB_SP_UNCTRLm,
    ISEC_MIB_SP_CTRL_1m,
    ISEC_MIB_SP_CTRL_2m,
    ISEC_MIB_SCm,
    ISEC_MIB_SAm,
    ISEC_SC_TCAMm,
    ISEC_SP_TCAMm,
    MGMT_OBM_IOM_STATS_WINDOW_RESULTSm,
};

/******************************************************************************
 * Private Functions
 */
static int
bcm56780_a0_ser_mem_clr_list_get(int unit,
                                 bcmptm_ser_mem_clr_list_t **mem_list,
                                 int *entry_num)
{
    *mem_list = bcm56780_a0_mem_clr_lst;
    *entry_num = COUNTOF(bcm56780_a0_mem_clr_lst);

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_clear_mem_before_ser_enable(int unit,
                                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                            int *reg_num)
{
    *reg_ctrl_lst = bcm56780_a0_clear_mem_before_ser_enable_info;
    *reg_num = COUNTOF(bcm56780_a0_clear_mem_before_ser_enable_info);
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_clear_mem_after_ser_enable(int unit,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num)
{
    *reg_ctrl_lst = bcm56780_a0_clear_mem_after_ser_enable_info;
    *reg_num = COUNTOF(bcm56780_a0_clear_mem_after_ser_enable_info);

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_tcam_hw_scan_ctrl_info_get(int unit, int enable,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num)
{
    if (enable) {
        *reg_ctrl_lst = bcm56780_a0_tcam_scan_ctrl_reg_lst_en;
        *reg_num = COUNTOF(bcm56780_a0_tcam_scan_ctrl_reg_lst_en);
    } else {
        *reg_ctrl_lst = bcm56780_a0_tcam_scan_ctrl_reg_lst_dis;
        *reg_num = COUNTOF(bcm56780_a0_tcam_scan_ctrl_reg_lst_dis);
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_acc_type_map_lst_get(int unit,
                                     bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                     int *reg_num)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_tcam_ser_info_get(int unit,
                                  bcmptm_ser_tcam_generic_info_t **tcam_ser_info,
                                  int *tcam_info_num)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_mem_mute_list_get(int unit,
                                  bcmdrd_sid_t **mem_ser_mute_list,
                                  int *mem_ser_mute_num)
{
    *mem_ser_mute_list = bcm56780_a0_mem_ser_mute;
    *mem_ser_mute_num = COUNTOF(bcm56780_a0_mem_ser_mute);

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_drop_ptk_ctrl_reg_mute_list_get(int unit,
                                  bcmdrd_sid_t **ctrl_reg_mute_list,
                                  int *ctrl_reg_mute_num)
{
    *ctrl_reg_mute_list = bcm56780_a0_ctrl_reg_drop_ptk_mute;
    *ctrl_reg_mute_num = COUNTOF(bcm56780_a0_ctrl_reg_drop_ptk_mute);

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_blk_ctrl_get(int unit,
                             bcmptm_ser_ctrl_en_t **ser_blks_ctrl_regs,
                             int *ctrl_item_num)
{
    *ser_blks_ctrl_regs = bcm56780_a0_ser_blocks_ctrl_regs;
    *ctrl_item_num = COUNTOF(bcm56780_a0_ser_blocks_ctrl_regs);

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_intr_map_get(int unit,
                             int *cdmac_inst,
                             bcmptm_ser_intr_route_info_t **intr_map,
                             int *intr_map_num)
{

    *intr_map = bcm56780_a0_intr_map;
    *intr_map_num = COUNTOF(bcm56780_a0_intr_map);

    if (cdmac_inst != NULL) {
        *cdmac_inst = 8;
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_blk_type_map(int unit,
                             int blk_type, int *blk_type_overlay,
                             int *ser_blk_type)
{
    int count = COUNTOF(bcm56780_a0_blktype_info);
    int i = 0, index = -1;

    for (i = 0; i < count; i++) {
        if (blk_type == bcm56780_a0_blktype_info[i].blk_type) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        if (ser_blk_type != NULL) {
            *ser_blk_type = bcm56780_a0_blktype_info[index].ser_blk_type;
        }
        if (blk_type_overlay != NULL) {
            for (i = 0; i < BCMPTM_SER_OVERLAY_BLK_TYPE_NUM; i++) {
                blk_type_overlay[i] =
                    bcm56780_a0_blktype_info[index].blk_type_overlay[i];
            }
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

static void
bcm56780_a0_ser_lp_intr_enable(int unit, int sync, int cmicx_intr_num, int enable)
{
    if (enable) {
        bcmbd_cmicx_lp_intr_enable(unit, cmicx_intr_num);
    } else {
        if (sync == 0) {
            bcmbd_cmicx_lp_intr_disable_nosync(unit, cmicx_intr_num);
        } else {
            bcmbd_cmicx_lp_intr_disable(unit, cmicx_intr_num);
        }

    }
}

static int
bcm56780_a0_ser_lp_intr_func_set(int unit,
                                 unsigned int cmic_intr_num,
                                 bcmbd_ser_intr_f intr_func,
                                 uint32_t intr_param)
{
    return bcmbd_cmicx_lp_intr_func_set(unit, cmic_intr_num,
                                        (bcmbd_intr_f)intr_func,
                                        intr_param);
}

static int
bcm56780_a0_ser_cmic_ser_engine_regs_get(int unit,
                                         bcmdrd_sid_t *range_cfg_list,
                                         bcmdrd_sid_t *range_start_list,
                                         bcmdrd_sid_t *range_end_list,
                                         bcmdrd_sid_t *range_result_list,
                                         bcmdrd_sid_t **prot_list,
                                         bcmdrd_sid_t *range_addr_bits_list,
                                         bcmdrd_sid_t *range_disable_list,
                                         int *group_num,
                                         bcmdrd_fid_t  **range_cfg_fid_list,
                                         bcmdrd_fid_t  **prot_fid_list,
                                         bcmdrd_sid_t  *ser_memory_sid)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_cmic_ser_engine_enable(int unit, uint32_t ser_range_enable,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num, int *intr_num)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_cmic_ser_result_get(int unit,
                                    bcmptm_ser_ctrl_reg_info_t **ser_error_lst,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_0,
                                    bcmptm_ser_ctrl_reg_info_t **ser_result_1,
                                    int *err_num)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_cmicx_adext_get(int unit, bcmptm_cmic_adext_info_t adext_info,
                                uint32_t *adext)
{
    BCMBD_CMICX_ADEXT_ACCTYPE_SET(*adext, adext_info.acc_type);
    BCMBD_CMICX_ADEXT_BLOCK_SET(*adext, adext_info.blk_num);
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_acc_type_get(int unit, bcmdrd_sid_t sid,
                             uint32_t *acc_type, const char **acctype_str)
{
    int rv = SHR_E_NONE;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t blkacc = 0, acctype_tmp = 0;
    int i = 0, acctype_num = COUNTOF(bcm56780_a0_acctype_info);
    bcmptm_ser_acctype_info_t *acctype_info = bcm56780_a0_acctype_info;

    rv = bcmdrd_pt_info_get(unit, sid, sinfo);
    if (SHR_FAILURE(rv)) {
        return SHR_E_PARAM;
    }

    blkacc = sinfo->blktypes;
    acctype_tmp = BCMBD_CMICX_BLKACC2ACCTYPE(blkacc);

    *acc_type = acctype_tmp;
    for (i = 0; i < acctype_num; i++) {
        if (acctype_info[i].acctype == acctype_tmp) {
            *acctype_str = acctype_info[i].acctype_name;
            break;
        }
    }
    /* last one is "unknown" */
    if (i == acctype_num) {
        *acctype_str = acctype_info[i - 1].acctype_name;
    }
    return SHR_E_NONE;
}

static bool
bcm56780_a0_ser_pt_inst_remap(int unit, bcmdrd_sid_t sid,
                              int *tbl_inst, int *tbl_copy)
{
    int blk_type, inst_num, pipe_num;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t inst_v_bmp, ipep_valid_pipes = 0;
    int num_item = 0, base_type, i;
    int inst = *tbl_inst, copy = *tbl_copy, inst_mapped;
    bcmptm_fld_inst_map_t pipe_map_dup2[] = {
        {0, 0}, {1, 2}, {2, 2}, {3, 0}
    };
    bcmptm_fld_inst_map_t *map_ptr = NULL;
    uint32_t blkacc = 0, acctype = 0;

    pipe_num = bcmdrd_pt_num_pipe_inst(unit, sid);
    inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    /* pipe_num is not zero only for PT belonged to IP/EP/MMU/PORT_IF */
    if (pipe_num == 0) {
        if (inst_num > inst) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
    if (copy >= 0) {
        inst_num = pipe_num;
        inst = copy;
    } else if (inst >= 0) {
        if (bcmdrd_pt_is_port_reg(unit, sid) ||
            bcmdrd_pt_is_soft_port_reg(unit, sid)) {
            return (inst_num > inst) ? TRUE : FALSE;
        }
        if (pipe_num > inst_num) {
            inst_num = pipe_num;
        }
    } else {
        /* -1 is ok */
        return TRUE;
    }
    inst_mapped = inst;
    inst_num = (inst_num < 0) ? 0 : inst_num;
    inst_v_bmp = (1 << inst_num) - 1;

    (void)bcmdrd_pt_info_get(unit, sid, sinfo);
    blkacc = sinfo->blktypes;
    acctype = BCMBD_CMICX_BLKACC2ACCTYPE(blkacc);

    if (acctype == ACC_TYPE_DUPLICATE_2) {
        inst_v_bmp = 0x5;
        num_item = COUNTOF(pipe_map_dup2);
        map_ptr = pipe_map_dup2;
        for (i = 0; i < num_item; i++) {
            if (map_ptr[i].invalid_inst == inst) {
                inst_mapped = map_ptr[i].valid_inst;
                if (copy >= 0) {
                    *tbl_copy = inst_mapped;
                } else {
                    *tbl_inst = inst_mapped;
                }
                break;
            }
        }
    }

    ipep_valid_pipes = 0;
    (void)bcmdrd_dev_valid_blk_pipes_get
        (unit, 0, BLKTYPE_IPIPE, &ipep_valid_pipes);
    /* half chip */
    if (ipep_valid_pipes < 0x3) {
        num_item = 0;
        blk_type = bcmdrd_pt_blktype_get(unit, sid, 0);
        if (blk_type == BLKTYPE_EPIPE || blk_type == BLKTYPE_IPIPE) {
            num_item = 4;
            inst_v_bmp = ipep_valid_pipes;
        } else if (blk_type == BLKTYPE_MMU_ITM) {
            num_item = 1;
            inst_v_bmp =  0x1;
            base_type = ((sinfo->offset) >> 23) & 0x7;
            /* Base type is IP EP */
            if (base_type == 2 || base_type == 3) {
                num_item = 8;
                inst_v_bmp = 0xf;
            }
        } else if (blk_type == BLKTYPE_MMU_EB ||
                   blk_type == BLKTYPE_PORT_IF) {
            num_item = 4;
            inst_v_bmp =  0xf;
        }
        if (inst_v_bmp & (1 << inst)) {
            inst_mapped = inst;
        } else {
            if (num_item == 4) {
                if (inst < 2) {
                    inst_mapped = (inst == 1) ? 0 : 1;
                } else {
                    inst_mapped = (inst == 3) ? 2 : 3;
                }
            } else if (num_item == 8) {
                if (inst < 4) {
                    /* inst: 0, 1, 2, 3 */
                    inst_mapped = (inst >= 2) ? (inst - 2) : (inst + 2);
                } else {
                    /* inst: 4, 5, 6, 7 */
                    inst_mapped = ((inst - 4) >= 2) ? (inst - 2) : (inst + 2);
                }
            }
            if (copy >= 0) {
                *tbl_copy = inst_mapped;
            } else {
                *tbl_inst = inst_mapped;
            }
        }
    }
    return (inst_v_bmp & (1 << inst_mapped)) ? TRUE : FALSE;
}

static int
bcm56780_a0_ser_cmicx_schan_opcode_get(int unit, int cmd, int blk_num,
                                       int blk_types, int tbl_inst, int tbl_copy,
                                       int data_len, int dma, uint32_t ignore_mask,
                                       uint32_t *opcode)
{
    struct {
        int    cmd;
        int    opcode;
        int    use_tbl_copy;
    }  cmd_map[] = {
        { BCMPTM_SER_READ_MEMORY_CMD_MSG, READ_MEMORY_CMD_MSG, 1 },
        { BCMPTM_SER_READ_REGISTER_CMD_MSG, READ_REGISTER_CMD_MSG, 1 },
        { BCMPTM_SER_WRITE_MEMORY_CMD_MSG, WRITE_MEMORY_CMD_MSG, 0 },
        { BCMPTM_SER_WRITE_REGISTER_CMD_MSG, WRITE_REGISTER_CMD_MSG, 0 }
    };
    int i = 0, map_num = COUNTOF(cmd_map), acc_type;
    uint32_t oc = 0;

    if (tbl_inst > 7) {
        return SHR_E_PARAM;
    }
    for (i = 0; i < map_num; i++) {
        if (cmd_map[i].cmd == cmd) {
            break;
        }
    }
    if (i == map_num) {
        return SHR_E_PARAM;
    }
    acc_type = BCMBD_CMICX_BLKACC2ACCTYPE(blk_types);

    if(acc_type == ACC_TYPE_UNIQUE) {
        if (tbl_inst >= 0) {
            acc_type = tbl_inst;
        } else {
            return SHR_E_PARAM;
        }
    }
    /*
     * For writing operation, use duplicate access type to write all instances.
     * For reading opreation, use unique access type to read appointed instance.
     * If use duplicate access type to read, only can read data from default instance(0).
     */
    if (SER_ACC_TYPE_IS_DUPLICATE(acc_type) && cmd_map[i].use_tbl_copy == 1) {
        if (tbl_copy >= 0) {
            acc_type = tbl_copy;
        } else if (tbl_inst > 0) {
            acc_type = tbl_inst;
        }
    }
    BCMPTM_SER_OPCODE_SET(oc, cmd_map[i].opcode);
    BCMPTM_SER_DSTBLK_SET(oc, blk_num);
    BCMPTM_SER_ACCTYPE_SET(oc, acc_type);
    BCMPTM_SER_DATALEN_SET(oc, data_len);
    BCMPTM_SER_DMA_SET(oc, dma);
    BCMPTM_SER_BANK_SET(oc, ignore_mask);
    *opcode = oc;

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_hwmem_base_info_get(int unit, int membase, int stage_id,
                                    char **bd_bb_name, char **bb_name, bool *mem)
{
    int base_num = COUNTOF(bcm56780_a0_ser_hwmem_base_info), i = 0;
    bcmptm_ser_buffer_bus_info_t *hwmem_info = bcm56780_a0_ser_hwmem_base_info;

    for (i = 0; i < base_num; i++) {
        if (hwmem_info[i].membase == membase) {
            if (hwmem_info[i].stage_id == 0xff ||
                hwmem_info[i].stage_id == stage_id) {
                *bd_bb_name = hwmem_info[i].bd_name;
                *bb_name = hwmem_info[i].membase_info;
                *mem = hwmem_info[i].mem;
                return SHR_E_NONE;
            }
        }
    }
    return SHR_E_NOT_FOUND;
}

static int
bcm56780_a0_ser_hw_info_remap(int unit, uint32_t address, int blk_type,
                              uint32_t membase, uint32_t instruction,
                              uint32_t *address_mapped, int *non_sbus)
{
 uint32_t address_map[][4] = {
        { 0x30000000, 0x30280001, 0x3FFF0000, BLKTYPE_PORT_IF }, /* EDB_XMIT_START_COUNT */
        };
    int i, map_depth = COUNTOF(address_map);

    *address_mapped = address;

    for (i = 0; i < map_depth; i++) {
        if (blk_type != (int)address_map[i][3]) {

            continue;
        }
        if ((address & address_map[i][2]) == address_map[i][0]) {
            *address_mapped = address & (~address_map[i][2]);
            *address_mapped |= (address_map[i][1] & address_map[i][2]);
            break;
        }
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_mmu_phy2log_mem_remap(int unit, bcmptm_ser_correct_info_t *spci)
{
    bcmdrd_sid_t phy_to_log_mapping[][2] = {
        /* WRED */
        {MMU_MTRO_CPU_L1_Bm, MMU_MTRO_CPU_L1_Am},
        {MMU_MTRO_CPU_L1_Cm, MMU_MTRO_CPU_L1_Am},
        {MMU_MTRO_L0_Bm, MMU_MTRO_L0_Am},
        {MMU_MTRO_L0_Cm, MMU_MTRO_L0_Am},
    };

    int num_mapping = COUNTOF(phy_to_log_mapping), i = 0;

    if (spci->sid[0] == INVALIDm) {
        return SHR_E_FAIL;
    }
    for (i = 0; i < num_mapping; i++) {
        if (spci->sid[0] == phy_to_log_mapping[i][0]) {
            spci->reported_sid = spci->sid[0];
            spci->sid[0] = phy_to_log_mapping[i][1];
            spci->flags |= BCMPTM_SER_FLAG_ERR_IN_SHADOW;
            return SHR_E_NONE;
        }
    }
    spci->reported_sid = INVALIDm;
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_mmu_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                              uint32_t adext, int blk_inst)
{
    return bcm56780_a0_ser_mmu_phy2log_mem_remap(unit, spci);
}


static int
bcm56780_a0_ser_ip_ep_sram_remap(int unit, bcmdrd_sid_t sid, bcmdrd_sid_t *remap_sid)
{
    bcmdrd_sid_t mapping[][2] = {
        {INVALIDm, INVALIDm}
    };
    int i = 0, num_mapping = COUNTOF(mapping);

    *remap_sid = sid;
    for (i = 0; i < num_mapping; i++) {
        if (sid == mapping[i][0]) {
            *remap_sid = mapping[i][1];
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_data_split_mem_test_field_get(int unit, bcmdrd_sid_t sid,
                                              int acc_type, bcmdrd_fid_t *test_fid)
{
    /* There is no PT which access type is DATA_SPLIT */
    return SHR_E_NOT_FOUND;
}

static int
bcm56780_a0_ser_tcam_remap(int unit, bcmdrd_sid_t tcam_sid,
                           bcmdrd_sid_t *hw_protected_sid,
                           int *inject_err_to_key_fld)
{
    uint32_t mem_map_info[][2] = {
        { INVALIDm, INVALIDm }
    };
    int num = COUNTOF(mem_map_info), i = 0;

    *inject_err_to_key_fld = 0;
    *hw_protected_sid = tcam_sid;
    for (i = 0; i < num; i++) {
        if (mem_map_info[i][0] == tcam_sid) {
            *hw_protected_sid = mem_map_info[i][1];
            *inject_err_to_key_fld = 1;
            break;
        }
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_tcam_sticky_bit_clr_get(int unit, bcmdrd_sid_t tcam_sid,
                                        bcmdrd_sid_t *ctrl_sid,
                                        bcmdrd_fid_t *ctrl_fid)
{
    static uint32_t mem_map_info[][2] = {
        { FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm, FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLY_SER_CONTROLr },
        { FLEX_DIGEST_LKUP_NET_LAYER_TCAM_1_ONLYm, FLEX_DIGEST_LKUP_NET_LAYER_TCAM_1_ONLY_SER_CONTROLr },
        { FLEX_DIGEST_LKUP_NET_LAYER_TCAM_2_ONLYm, FLEX_DIGEST_LKUP_NET_LAYER_TCAM_2_ONLY_SER_CONTROLr },
        { IPOST_CPU_COS_CPU_COS_MAP_TCAM_ONLYm,
          IPOST_CPU_COS_CPU_COS_MAP_TCAM_ONLY_SER_CONTROLr },
        { IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_ONLYm,
          IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_ONLY_SER_CONTROLr },
        { IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_ONLYm,
          IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM0_0_ONLYm, MEMDB_IFTA100_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM0_1_ONLYm, MEMDB_IFTA100_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM0_2_ONLYm, MEMDB_IFTA100_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM0_3_ONLYm, MEMDB_IFTA100_MEM0_0_ONLY_SER_CONTROLr },

        { MEMDB_IFTA100_MEM1_0_ONLYm, MEMDB_IFTA100_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM1_1_ONLYm, MEMDB_IFTA100_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM1_2_ONLYm, MEMDB_IFTA100_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM1_3_ONLYm, MEMDB_IFTA100_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM2_0_ONLYm, MEMDB_IFTA100_MEM2_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM2_1_ONLYm, MEMDB_IFTA100_MEM2_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM2_2_ONLYm, MEMDB_IFTA100_MEM2_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM2_3_ONLYm, MEMDB_IFTA100_MEM2_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM3_0_ONLYm, MEMDB_IFTA100_MEM3_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM3_1_ONLYm, MEMDB_IFTA100_MEM3_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM3_2_ONLYm, MEMDB_IFTA100_MEM3_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA100_MEM3_3_ONLYm, MEMDB_IFTA100_MEM3_0_ONLY_SER_CONTROLr },

        { MEMDB_IFTA150_MEM0_0_ONLYm, MEMDB_IFTA150_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA150_MEM0_1_ONLYm, MEMDB_IFTA150_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA150_MEM0_2_ONLYm, MEMDB_IFTA150_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_IFTA150_MEM0_3_ONLYm, MEMDB_IFTA150_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_EFTA30_MEM0_0_ONLYm, MEMDB_TCAM_EFTA30_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_EFTA30_MEM0_1_ONLYm, MEMDB_TCAM_EFTA30_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_EFTA30_MEM0_2_ONLYm, MEMDB_TCAM_EFTA30_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_EFTA30_MEM0_3_ONLYm, MEMDB_TCAM_EFTA30_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM0_0_ONLYm, MEMDB_TCAM_IFTA40_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM0_1_ONLYm, MEMDB_TCAM_IFTA40_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM0_2_ONLYm, MEMDB_TCAM_IFTA40_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM0_3_ONLYm, MEMDB_TCAM_IFTA40_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM1_0_ONLYm, MEMDB_TCAM_IFTA40_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM1_1_ONLYm, MEMDB_TCAM_IFTA40_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM1_2_ONLYm, MEMDB_TCAM_IFTA40_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA40_MEM1_3_ONLYm, MEMDB_TCAM_IFTA40_MEM1_0_ONLY_SER_CONTROLr },

        { MEMDB_TCAM_IFTA80_MEM0_0_ONLYm, MEMDB_TCAM_IFTA80_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM0_1_ONLYm, MEMDB_TCAM_IFTA80_MEM0_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM1_0_ONLYm, MEMDB_TCAM_IFTA80_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM1_1_ONLYm, MEMDB_TCAM_IFTA80_MEM1_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM2_0_ONLYm, MEMDB_TCAM_IFTA80_MEM2_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM2_1_ONLYm, MEMDB_TCAM_IFTA80_MEM2_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM3_0_ONLYm, MEMDB_TCAM_IFTA80_MEM3_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM3_1_ONLYm, MEMDB_TCAM_IFTA80_MEM3_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM4_0_ONLYm, MEMDB_TCAM_IFTA80_MEM4_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM4_1_ONLYm, MEMDB_TCAM_IFTA80_MEM4_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM5_0_ONLYm, MEMDB_TCAM_IFTA80_MEM5_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM5_1_ONLYm, MEMDB_TCAM_IFTA80_MEM5_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM6_0_ONLYm, MEMDB_TCAM_IFTA80_MEM6_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM6_1_ONLYm, MEMDB_TCAM_IFTA80_MEM6_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM7_0_ONLYm, MEMDB_TCAM_IFTA80_MEM7_0_ONLY_SER_CONTROLr },
        { MEMDB_TCAM_IFTA80_MEM7_1_ONLYm, MEMDB_TCAM_IFTA80_MEM7_0_ONLY_SER_CONTROLr }
    };
    int num = COUNTOF(mem_map_info), i = 0;

    *ctrl_fid = SER_STICKY_BIT_CLEARf;
    *ctrl_sid = INVALIDr;
    for (i = 0; i < num; i++) {
        if (mem_map_info[i][0] == tcam_sid) {
            *ctrl_sid = mem_map_info[i][1];
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_mem_ecc_info_get(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                                 bcmdrd_sid_t *sid, int *index, int *ecc_checking)
{
    uint32_t mem_map_info[][3] = {
        { IFTA40_E2T_00_B0_SINGLEm,  IFTA40_E2T_00_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA40_E2T_00_B0_DOUBLEm,  IFTA40_E2T_00_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA40_E2T_00_B0_QUADm,    IFTA40_E2T_00_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA40_E2T_00_B1_SINGLEm,  IFTA40_E2T_00_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA40_E2T_00_B1_DOUBLEm,  IFTA40_E2T_00_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA40_E2T_00_B1_QUADm,    IFTA40_E2T_00_B1_ECCm, 4 }, /* 1 : 4 */

        { IFTA40_E2T_01_B0_SINGLEm,  IFTA40_E2T_01_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA40_E2T_01_B0_DOUBLEm,  IFTA40_E2T_01_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA40_E2T_01_B0_QUADm,    IFTA40_E2T_01_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA40_E2T_01_B1_SINGLEm,  IFTA40_E2T_01_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA40_E2T_01_B1_DOUBLEm,  IFTA40_E2T_01_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA40_E2T_01_B1_QUADm,    IFTA40_E2T_01_B1_ECCm, 4 }, /* 1 : 4 */

        { IFTA30_E2T_00_B0_SINGLEm,  IFTA30_E2T_00_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA30_E2T_00_B0_DOUBLEm,  IFTA30_E2T_00_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA30_E2T_00_B0_QUADm,    IFTA30_E2T_00_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA30_E2T_00_B1_SINGLEm,  IFTA30_E2T_00_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA30_E2T_00_B1_DOUBLEm,  IFTA30_E2T_00_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA30_E2T_00_B1_QUADm,    IFTA30_E2T_00_B1_ECCm, 4 }, /* 1 : 4 */

        { IFTA90_E2T_00_B0_SINGLEm,  IFTA90_E2T_00_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_00_B0_DOUBLEm,  IFTA90_E2T_00_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_00_B0_QUADm,    IFTA90_E2T_00_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA90_E2T_00_B1_SINGLEm,  IFTA90_E2T_00_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_00_B1_DOUBLEm,  IFTA90_E2T_00_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_00_B1_QUADm,    IFTA90_E2T_00_B1_ECCm, 4 }, /* 1 : 4 */
        { IFTA90_E2T_00_B2_SINGLEm,  IFTA90_E2T_00_B2_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_00_B2_DOUBLEm,  IFTA90_E2T_00_B2_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_00_B2_QUADm,    IFTA90_E2T_00_B2_ECCm, 4 }, /* 1 : 4 */
        { IFTA90_E2T_00_B3_SINGLEm,  IFTA90_E2T_00_B3_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_00_B3_DOUBLEm,  IFTA90_E2T_00_B3_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_00_B3_QUADm,    IFTA90_E2T_00_B3_ECCm, 4 }, /* 1 : 4 */

        { IFTA90_E2T_01_B0_SINGLEm,  IFTA90_E2T_01_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_01_B0_DOUBLEm,  IFTA90_E2T_01_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_01_B0_QUADm,    IFTA90_E2T_01_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA90_E2T_01_B1_SINGLEm,  IFTA90_E2T_01_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_01_B1_DOUBLEm,  IFTA90_E2T_01_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_01_B1_QUADm,    IFTA90_E2T_01_B1_ECCm, 4 }, /* 1 : 4 */
        { IFTA90_E2T_01_B2_SINGLEm,  IFTA90_E2T_01_B2_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_01_B2_DOUBLEm,  IFTA90_E2T_01_B2_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_01_B2_QUADm,    IFTA90_E2T_01_B2_ECCm, 4 }, /* 1 : 4 */
        { IFTA90_E2T_01_B3_SINGLEm,  IFTA90_E2T_01_B3_ECCm, 1 }, /* 1 : 1 */
        { IFTA90_E2T_01_B3_DOUBLEm,  IFTA90_E2T_01_B3_ECCm, 2 }, /* 1 : 2 */
        { IFTA90_E2T_01_B3_QUADm,    IFTA90_E2T_01_B3_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B0_SINGLEm ,  M_E2T_B0_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B0_SINGLE_2m ,M_E2T_B0_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B0_DOUBLEm,   M_E2T_B0_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B0_QUADm,     M_E2T_B0_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B1_SINGLEm,   M_E2T_B1_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B1_SINGLE_2m, M_E2T_B1_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B1_DOUBLEm,   M_E2T_B1_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B1_QUADm,     M_E2T_B1_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B2_SINGLEm,   M_E2T_B2_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B2_SINGLE_2m, M_E2T_B2_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B2_DOUBLEm,   M_E2T_B2_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B2_QUADm,     M_E2T_B2_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B3_SINGLEm,   M_E2T_B3_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B3_SINGLE_2m, M_E2T_B3_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B3_DOUBLEm,   M_E2T_B3_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B3_QUADm,     M_E2T_B3_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B4_SINGLEm,   M_E2T_B4_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B4_SINGLE_2m,  M_E2T_B4_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B4_DOUBLEm,   M_E2T_B4_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B4_QUADm,     M_E2T_B4_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B5_SINGLEm,   M_E2T_B5_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B5_SINGLE_2m, M_E2T_B5_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B5_DOUBLEm,   M_E2T_B5_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B5_QUADm,     M_E2T_B5_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B6_SINGLEm,   M_E2T_B6_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B6_SINGLE_2m, M_E2T_B6_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B6_DOUBLEm,   M_E2T_B6_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B6_QUADm,     M_E2T_B6_ECCm, 4 }, /* 1 : 4 */
        { M_E2T_B7_SINGLEm,   M_E2T_B7_ECCm, 1 }, /* 1 : 1 */
        { M_E2T_B7_SINGLE_2m, M_E2T_B7_ECC_2m, 1 }, /* 1 : 1 */
        { M_E2T_B7_DOUBLEm,   M_E2T_B7_ECCm, 2 }, /* 1 : 2 */
        { M_E2T_B7_QUADm,     M_E2T_B7_ECCm, 4 }, /* 1 : 4 */


        { IFTA80_E2T_00_B0_SINGLEm,  IFTA80_E2T_00_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_00_B0_DOUBLEm,  IFTA80_E2T_00_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_00_B0_QUADm,    IFTA80_E2T_00_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA80_E2T_00_B1_SINGLEm,  IFTA80_E2T_00_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_00_B1_DOUBLEm,  IFTA80_E2T_00_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_00_B1_QUADm,    IFTA80_E2T_00_B1_ECCm, 4 }, /* 1 : 4 */

        { IFTA80_E2T_01_B0_SINGLEm,  IFTA80_E2T_01_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_01_B0_DOUBLEm,  IFTA80_E2T_01_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_01_B0_QUADm,    IFTA80_E2T_01_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA80_E2T_01_B1_SINGLEm,  IFTA80_E2T_01_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_01_B1_DOUBLEm,  IFTA80_E2T_01_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_01_B1_QUADm,    IFTA80_E2T_01_B1_ECCm, 4 }, /* 1 : 4 */

        { IFTA80_E2T_02_B0_SINGLEm,  IFTA80_E2T_02_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_02_B0_DOUBLEm,  IFTA80_E2T_02_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_02_B0_QUADm,    IFTA80_E2T_02_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA80_E2T_02_B1_SINGLEm,  IFTA80_E2T_02_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_02_B1_DOUBLEm,  IFTA80_E2T_02_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_02_B1_QUADm,    IFTA80_E2T_02_B1_ECCm, 4 }, /* 1 : 4 */

        { IFTA80_E2T_03_B0_SINGLEm,  IFTA80_E2T_03_B0_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_03_B0_DOUBLEm,  IFTA80_E2T_03_B0_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_03_B0_QUADm,    IFTA80_E2T_03_B0_ECCm, 4 }, /* 1 : 4 */
        { IFTA80_E2T_03_B1_SINGLEm,  IFTA80_E2T_03_B1_ECCm, 1 }, /* 1 : 1 */
        { IFTA80_E2T_03_B1_DOUBLEm,  IFTA80_E2T_03_B1_ECCm, 2 }, /* 1 : 2 */
        { IFTA80_E2T_03_B1_QUADm,    IFTA80_E2T_03_B1_ECCm, 4 }, /* 1 : 4 */

        { EFTA30_E2T_00_B0_SINGLEm,  EFTA30_E2T_00_B0_ECCm, 1 }, /* 1 : 1 */
        { EFTA30_E2T_00_B0_DOUBLEm,  EFTA30_E2T_00_B0_ECCm, 2 }, /* 1 : 2 */
        { EFTA30_E2T_00_B0_QUADm,    EFTA30_E2T_00_B0_ECCm, 4 }, /* 1 : 4 */
        { EFTA30_E2T_00_B1_SINGLEm,  EFTA30_E2T_00_B1_ECCm, 1 }, /* 1 : 1 */
        { EFTA30_E2T_00_B1_DOUBLEm,  EFTA30_E2T_00_B1_ECCm, 2 }, /* 1 : 2 */
        { EFTA30_E2T_00_B1_QUADm,    EFTA30_E2T_00_B1_ECCm, 4 }, /* 1 : 4 */

        { L3_DEFIP_ALPM_LEVEL3_B0_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B0_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B0m,  L3_DEFIP_ALPM_LEVEL3_B0_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B1_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B1_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B1m,  L3_DEFIP_ALPM_LEVEL3_B1_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B2_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B2_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B2m,  L3_DEFIP_ALPM_LEVEL3_B2_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B3_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B3_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B3m,  L3_DEFIP_ALPM_LEVEL3_B3_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B4_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B4_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B4m,  L3_DEFIP_ALPM_LEVEL3_B4_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B5_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B5_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B5m,  L3_DEFIP_ALPM_LEVEL3_B5_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B6_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B6_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B6m,  L3_DEFIP_ALPM_LEVEL3_B6_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_B7_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_B7_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3_B7m,  L3_DEFIP_ALPM_LEVEL3_B7_ECCm, 4 }, /* 1 : 4 */

        { M_DEFIP_ALPM_LEVEL3_B0_SINGLEm, M_DEFIP_ALPM_LEVEL3_B0_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B0_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B0_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B0m,  M_DEFIP_ALPM_LEVEL3_B0_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B1_SINGLEm, M_DEFIP_ALPM_LEVEL3_B1_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B1_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B1_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B1m,  M_DEFIP_ALPM_LEVEL3_B1_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B2_SINGLEm,  M_DEFIP_ALPM_LEVEL3_B2_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B2_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B2_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B2m,  M_DEFIP_ALPM_LEVEL3_B2_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B3_SINGLEm,  M_DEFIP_ALPM_LEVEL3_B3_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B3_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B3_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B3m,  M_DEFIP_ALPM_LEVEL3_B3_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B4_SINGLEm,  M_DEFIP_ALPM_LEVEL3_B4_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B4_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B4_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B4m,  M_DEFIP_ALPM_LEVEL3_B4_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B5_SINGLEm,  M_DEFIP_ALPM_LEVEL3_B5_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B5_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B5_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B5m,  M_DEFIP_ALPM_LEVEL3_B5_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B6_SINGLEm,  M_DEFIP_ALPM_LEVEL3_B6_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B6_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B6_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B6m,  M_DEFIP_ALPM_LEVEL3_B6_ECCm, 2 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL3_B7_SINGLEm,  M_DEFIP_ALPM_LEVEL3_B7_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B7_SINGLE_2m,M_DEFIP_ALPM_LEVEL3_B7_ECC_2m, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL3_B7m,  M_DEFIP_ALPM_LEVEL3_B7_ECCm, 2 }, /* 1 : 4 */


        { L3_DEFIP_ALPM_LEVEL2_B0_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_B0_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2_B0m,  L3_DEFIP_ALPM_LEVEL2_B0_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL2_B1_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_B1_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2_B1m,  L3_DEFIP_ALPM_LEVEL2_B1_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL2_B2_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_B2_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2_B2m,  L3_DEFIP_ALPM_LEVEL2_B2_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL2_B3_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_B3_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2_B3m,  L3_DEFIP_ALPM_LEVEL2_B3_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL2_B4_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_B4_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2_B4m,  L3_DEFIP_ALPM_LEVEL2_B4_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL2_B5_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_B5_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2_B5m,  L3_DEFIP_ALPM_LEVEL2_B5_ECCm, 4 },  /* 1 : 4 */
        { ESEC_SC_TABLEm,  ESEC_SC_TABLEm, 0 },  /* 1 : 1 */
        { ESEC_SA_TABLEm,  ESEC_SA_TABLEm, 0 },  /* 1 : 1 */
        { SC_MAP_TABLEm,  SC_MAP_TABLEm, 0 },  /* 1 : 1 */
        { SUB_PORT_MAP_TABLEm,  SUB_PORT_MAP_TABLEm, 0 },  /* 1 : 1 */
        { ISEC_SA_TABLEm,  ISEC_SA_TABLEm, 0 },  /* 1 : 1 */
        { ISEC_SC_TABLEm,  ISEC_SC_TABLEm, 0 },  /* 1 : 1 */
        { SUB_PORT_POLICY_TABLEm,  SUB_PORT_POLICY_TABLEm, 0 },  /* 1 : 1 */
        { ESEC_SA_HASH_TABLEm, ESEC_SA_TABLEm, 0}, /* 1 : 1 */
        { ISEC_SA_HASH_TABLEm, ISEC_SA_TABLEm, 0}, /* 1 : 1 */
        { ESEC_MIB_MISCm,  ESEC_MIB_MISCm, 0 },  /* 1 : 1 */
        { ESEC_MIB_SC_UNCTRLm,  ESEC_MIB_SC_UNCTRLm, 0 },  /* 1 : 1 */
        { ESEC_MIB_SC_CTRLm,  ESEC_MIB_SC_CTRLm, 0 },  /* 1 : 1 */
        { ESEC_MIB_SCm,  ESEC_MIB_SCm, 0 },  /* 1 : 1 */
        { ESEC_MIB_SAm,  ESEC_MIB_SAm, 0 },  /* 1 : 1 */
        { ISEC_SPTCAM_HIT_COUNTm,  ISEC_SPTCAM_HIT_COUNTm, 0 },  /* 1 : 1 */
        { ISEC_SCTCAM_HIT_COUNTm,  ISEC_SCTCAM_HIT_COUNTm, 0 },  /* 1 : 1 */
        { ISEC_PORT_COUNTERSm,  ISEC_PORT_COUNTERSm, 0 },  /* 1 : 1 */
        { ISEC_MIB_SP_UNCTRLm,  ISEC_MIB_SP_UNCTRLm, 0 },  /* 1 : 1 */
        { ISEC_MIB_SP_CTRL_1m,  ISEC_MIB_SP_CTRL_1m, 0 },  /* 1 : 1 */
        { ISEC_MIB_SP_CTRL_2m,  ISEC_MIB_SP_CTRL_2m, 0 },  /* 1 : 1 */
        { ISEC_MIB_SCm,  ISEC_MIB_SCm, 0 },  /* 1 : 1 */
        { ISEC_MIB_SAm,  ISEC_MIB_SAm, 0 },  /* 1 : 1 */

        { M_DEFIP_ALPM_LEVEL2_B0m,  M_DEFIP_ALPM_LEVEL2_B0_ECCm, 4 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL2_B1_SINGLEm,  M_DEFIP_ALPM_LEVEL2_B1_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL2_B1m,  M_DEFIP_ALPM_LEVEL2_B1_ECCm, 4 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL2_B2_SINGLEm,  M_DEFIP_ALPM_LEVEL2_B2_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL2_B2m,  M_DEFIP_ALPM_LEVEL2_B2_ECCm, 4 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL2_B3_SINGLEm,  M_DEFIP_ALPM_LEVEL2_B3_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL2_B3m,  M_DEFIP_ALPM_LEVEL2_B3_ECCm, 4 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL2_B4_SINGLEm,  M_DEFIP_ALPM_LEVEL2_B4_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL2_B4m,  M_DEFIP_ALPM_LEVEL2_B4_ECCm, 4 }, /* 1 : 4 */
        { M_DEFIP_ALPM_LEVEL2_B5_SINGLEm,  M_DEFIP_ALPM_LEVEL2_B5_ECCm, 1 }, /* 1 : 1 */
        { M_DEFIP_ALPM_LEVEL2_B5m,  M_DEFIP_ALPM_LEVEL2_B5_ECCm, 4 }  /* 1 : 4 */

   };
    uint32_t ecc_remap[][2] = {
        {  M_E2T_B0_ECCm,   M_E2T_B0_ECC_2m },
        {  M_E2T_B1_ECCm,   M_E2T_B1_ECC_2m },
        {  M_E2T_B2_ECCm,   M_E2T_B2_ECC_2m },
        {  M_E2T_B3_ECCm,   M_E2T_B3_ECC_2m },
        {  M_E2T_B4_ECCm,   M_E2T_B4_ECC_2m },
        {  M_E2T_B5_ECCm,   M_E2T_B5_ECC_2m },
        {  M_E2T_B6_ECCm,   M_E2T_B6_ECC_2m },
        {  M_E2T_B7_ECCm,   M_E2T_B7_ECC_2m },
     };
    int j = 0 ;
    int num = COUNTOF(mem_map_info), i = 0;
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    int ecc_protection = 0, inject_err_to_key_fld = 0;
    bcmdrd_sid_t sid_tcam_only = sid_fv;

    if (sid != NULL && index != NULL) {
        *sid = sid_fv;
        *index = index_fv;
    }

    for (i = 0; i < num; i++) {
        if (mem_map_info[i][0] == sid_fv) {
            ecc_protection = 1;
            break;
        }
    }
    /* found */
    if (ecc_protection == 1 && sid != NULL && index != NULL) {
        *sid = mem_map_info[i][1];
        *index = index_fv * mem_map_info[i][2];
       if ( *index > 65535 ) {
          for (j = 0; j < 8 ;j++) {
               if (*sid == ecc_remap[j][0]) {
                   *sid = ecc_remap[j][1];
                   *index = (*index - 65536);
               }
           }
       }
    }
    if (ecc_checking == NULL) {
        return SHR_E_NONE;
    }

    (void)bcm56780_a0_ser_tcam_remap(unit, sid_fv, &sid_tcam_only,
                                     &inject_err_to_key_fld);
    if (ecc_protection == 0) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_ECC1B, sid_fv, &ctrl_info);
        if (SHR_FAILURE(rv)) {
            ecc_protection = 0;
        } else {
            ecc_protection = 1;
        }
    }
    *ecc_checking = ecc_protection;
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_refresh_regs_info_get(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                      uint32_t *info_num)
{
    *mem_refresh_info = bcm56780_a0_clear_refresh_registers;
    *info_num = COUNTOF(bcm56780_a0_clear_refresh_registers);

    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_ecc_status_translate(int unit,
                                     bcmptm_ser_correct_info_t *spci,
                                     const bcmdrd_sid_t **corrected_sids,
                                     uint32_t *index,
                                     uint32_t *sid_num)
{
    bcmdrd_sid_t idb_oam_status_regs[] = {
        IDB_OBM0_OVERSUB_MON_ECC_STATUSr, IDB_OBM1_OVERSUB_MON_ECC_STATUSr,
        IDB_OBM2_OVERSUB_MON_ECC_STATUSr, IDB_OBM3_OVERSUB_MON_ECC_STATUSr,
        IDB_OBM4_OVERSUB_MON_ECC_STATUSr, ESEC_SC_TABLE_ECC_STATUSr,
        ESEC_SA_TABLE_ECC_STATUSr, ISEC_SC_MAP_ECC_STATUSr,
        ISEC_SP_MAP_ECC_STATUSr, ISEC_SA_TABLE_ECC_STATUSr,
        ISEC_SC_TABLE_ECC_STATUSr, ISEC_SP_POLICY_ECC_STATUSr,
        ESEC_HASH_TABLE_ECC_STATUSr, ISEC_HASH_TABLE_ECC_STATUSr,
        ESEC_MIB_MISC_ECC_STATUSr, ESEC_MIB_SC_UNCTRL_ECC_STATUSr,
        ESEC_MIB_SC_CTRL_ECC_STATUSr, ESEC_MIB_SC_ECC_STATUSr,
        ESEC_MIB_SA_ECC_STATUSr, ISEC_SPTCAM_HIT_COUNT_ECC_STATUSr,
        ISEC_SCTCAM_HIT_COUNT_ECC_STATUSr, ISEC_PORT_COUNTERS_ECC_STATUSr,
        ISEC_MIB_SP_UNCTRL_ECC_STATUSr, ISEC_MIB_SP_CTRL_1_ECC_STATUSr,
        ISEC_MIB_SP_CTRL_2_ECC_STATUSr, ISEC_MIB_SC_ECC_STATUSr,
        ISEC_MIB_SA_ECC_STATUSr, ISEC_SCTCAM_SER_STATUSr,
        ISEC_SPTCAM_SER_STATUSr,
    };
    int idx = 0, num = COUNTOF(idb_oam_status_regs);
    int found = 0;

    *sid_num = 0;
    for (idx = 0; idx < num; idx++) {
        if (spci->status_reg_sid == idb_oam_status_regs[idx]) {
            found = 1;
            break;
        }
    }
    if (found) {
        *corrected_sids = &bcm56780_a0_idb_obm_mem_list[idx];
        *index = spci->index[0];
        *sid_num = 1;
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_mem_xor_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                 bcmptm_ser_sram_info_t *sram_info)
{
    /* sid ecc view,  entries_per_sram,  ram_count */
    uint32_t xor_sram_info[][3] = {
        { IFTA40_E2T_00_B0_ECCm, 8192, 2 },
        { IFTA40_E2T_00_B1_ECCm, 8192, 2 },
        { IFTA40_E2T_01_B0_ECCm, 8192, 2 },
        { IFTA40_E2T_01_B1_ECCm, 8192, 2 },
        { IFTA30_E2T_00_B0_ECCm, 8192, 2 },
        { IFTA30_E2T_00_B1_ECCm, 8192, 2 },
        { IFTA80_E2T_00_B0_ECCm, 8192, 2 },
        { IFTA80_E2T_00_B1_ECCm, 8192, 2 },
        { IFTA80_E2T_01_B0_ECCm, 8192, 2 },
        { IFTA80_E2T_01_B1_ECCm, 8192, 2 },
        { IFTA80_E2T_02_B0_ECCm, 8192, 2 },
        { IFTA80_E2T_02_B1_ECCm, 8192, 2 },
        { IFTA80_E2T_03_B0_ECCm, 8192, 2 },
        { IFTA80_E2T_03_B1_ECCm, 8192, 2 },
        { IFTA90_E2T_00_B0_ECCm, 8192, 8 },
        { IFTA90_E2T_00_B1_ECCm, 8192, 8 },
        { IFTA90_E2T_00_B2_ECCm, 8192, 8 },
        { IFTA90_E2T_00_B3_ECCm, 8192, 8 },
        { IFTA90_E2T_01_B0_ECCm, 8192, 8 },
        { IFTA90_E2T_01_B1_ECCm, 8192, 8 },
        { IFTA90_E2T_01_B2_ECCm, 8192, 8 },
        { IFTA90_E2T_01_B3_ECCm, 8192, 8 },

        { M_E2T_B0_ECCm, 8192, 8 },
        { M_E2T_B1_ECCm, 8192, 8 },
        { M_E2T_B2_ECCm, 8192, 8 },
        { M_E2T_B3_ECCm, 8192, 8 },
        { M_E2T_B4_ECCm, 8192, 8 },
        { M_E2T_B5_ECCm, 8192, 8 },
        { M_E2T_B6_ECCm, 8192, 8 },
        { M_E2T_B7_ECCm, 8192, 8 },

        { EFTA30_E2T_00_B0_ECCm, 8192, 2 },
        { EFTA30_E2T_00_B1_ECCm, 8192, 2 },
        /* Same to IFTA90_E2T_0*_B*_ECCm */
        { L3_DEFIP_ALPM_LEVEL3_B0_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B1_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B2_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B3_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B4_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B5_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B6_ECCm, 8192, 8 },
        { L3_DEFIP_ALPM_LEVEL3_B7_ECCm, 8192, 8 },

        { M_DEFIP_ALPM_LEVEL3_B0_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B1_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B2_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B3_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B4_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B5_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B6_ECCm, 8192, 8 },
        { M_DEFIP_ALPM_LEVEL3_B7_ECCm, 8192, 8 }
    };
    uint32_t entry_num_per_ram = 0, ram_count = 0;
    uint32_t index_num[2];
    uint32_t i = 0, num = 0;
    uint32_t base = 0;

    num = COUNTOF(xor_sram_info);
    for (i = 0; i < num; i++) {
        if (sid == xor_sram_info[i][0]) {
            entry_num_per_ram = xor_sram_info[i][1];
            ram_count = xor_sram_info[i][2];
            break;
        }
    }
    if (i < num) {
        index_num[0] = ram_count * entry_num_per_ram;
        index_num[1] = bcmptm_pt_index_count(unit, sid);

        if (index_num[0] != index_num[1]) {
            /* UFT */
            if (ram_count == 8) {
                index_num[0] = 4 * entry_num_per_ram;
                if (index_num[0] == index_num[1]) {
                    ram_count = 4;
                } else {
                    return SHR_E_INTERNAL;
                }
            } else {
                return SHR_E_INTERNAL;
            }
        }
        base = index % entry_num_per_ram;
        sram_info->hw_recovery = FALSE;
        sram_info->ram_count = ram_count;
        sram_info->indexes[0][0] = base;
        sram_info->index_count[0] = 1;
        sram_info->pt_view[0] = sid;
        /* Start from 1 */
        for (i = 1; i < ram_count; i++) {
            sram_info->index_count[i] = 1;
            sram_info->pt_view[i] = sid;
            sram_info->indexes[i][0] = sram_info->indexes[i - 1][0] + entry_num_per_ram;
        }
        return SHR_E_NONE;
    }
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_sram_ctrl_register_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                       bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info)
{
    /* Disables XOR bank write. Allows SW to inject parity error on Table Banks or XOR Bank. */
    uint32_t xor_write_reg_map[][3] = {
        { IFTA40_E2T_00_B0_ECCm, IFTA40_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA40_E2T_00_B1_ECCm, IFTA40_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA40_E2T_01_B0_ECCm, IFTA40_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA40_E2T_01_B1_ECCm, IFTA40_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA30_E2T_00_B0_ECCm, IFTA30_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA30_E2T_00_B1_ECCm, IFTA30_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_00_B0_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_00_B1_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_00_B2_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_00_B3_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_01_B0_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_01_B1_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_01_B2_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA90_E2T_01_B3_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },

        { M_E2T_B0_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B1_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B2_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B3_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B4_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B5_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B6_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { M_E2T_B7_ECCm, M_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_00_B0_ECCm, IFTA80_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_00_B1_ECCm, IFTA80_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_01_B0_ECCm, IFTA80_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_01_B1_ECCm, IFTA80_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_02_B0_ECCm, IFTA80_E2T_02_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_02_B1_ECCm, IFTA80_E2T_02_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_03_B0_ECCm, IFTA80_E2T_03_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { IFTA80_E2T_03_B1_ECCm, IFTA80_E2T_03_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { EFTA30_E2T_00_B0_ECCm, EFTA30_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { EFTA30_E2T_00_B1_ECCm, EFTA30_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B0_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B1_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B2_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B3_ECCm, IFTA90_E2T_00_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B4_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B5_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B6_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
        { L3_DEFIP_ALPM_LEVEL3_B7_ECCm, IFTA90_E2T_01_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf },
    };
    uint32_t i = 0, map_num, j;
    int index_ecc = 0;
    bcmdrd_sid_t sid_ecc = INVALIDm;

    /* initialize */
    sram_ctrl_reg_info->force_field = INVALIDf;
    sram_ctrl_reg_info->force_reg = INVALIDr;
    sram_ctrl_reg_info->wr_check_reg = INVALIDr;
    sram_ctrl_reg_info->wr_check_field = INVALIDf;
    sram_ctrl_reg_info->xor_wr_reg = INVALIDr;
    sram_ctrl_reg_info->xor_wr_field = INVALIDf;

    (void)bcm56780_a0_ser_mem_ecc_info_get(unit, sid, index, &sid_ecc, &index_ecc, NULL);

    for (j = 0; j < BCMPTM_SER_EXTRA_REG_NUM; j++) {
        sram_ctrl_reg_info->extra_reg[j] = INVALIDr;
        sram_ctrl_reg_info->extra_field[j] = INVALIDf;
    }
    map_num = COUNTOF(xor_write_reg_map);
    for (i = 0; i < map_num; i++) {
        if (sid == xor_write_reg_map[i][0] || sid_ecc == xor_write_reg_map[i][0]) {
            sram_ctrl_reg_info->xor_wr_reg = xor_write_reg_map[i][1];
            sram_ctrl_reg_info->xor_wr_field = xor_write_reg_map[i][2];
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_sram_scan_mem_skip(int unit, bcmdrd_sid_t sid, bool *skip)
{
    bcmdrd_sid_t sid_skip_list[] = {
        INVALIDm
    };
    int i = 0, sid_num = COUNTOF(sid_skip_list);
    int index_ecc = 0;
    bcmdrd_sid_t sid_ecc = sid;

    (void)bcm56780_a0_ser_mem_ecc_info_get(unit, sid, 0, &sid_ecc,
                                           &index_ecc, NULL);
    /* scan ECC view */
    if (sid != sid_ecc) {
        *skip = TRUE;
    } else {
        *skip = FALSE;
        for (i = 0; i < sid_num; i++) {
            if (sid_skip_list[i] == sid) {
                *skip = TRUE;
                break;
            }
        }
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_copy_to_cpu_regs_info_get(int unit,
                                         bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                         int *reg_num)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56780_a0_ser_expected_value_get(int unit, bcmdrd_sid_t sid,
                                   bcmdrd_fid_t fid, uint32_t *value)
{
    int min_bit, max_bit;

    min_bit = bcmdrd_pt_field_minbit(unit, sid, fid);
    max_bit = bcmdrd_pt_field_maxbit(unit, sid, fid);
    *value = (1 << (max_bit - min_bit + 1)) - 1;

    return SHR_E_NONE;

}

static int
bcm56780_a0_ser_force_error_value_get(int unit, bcmdrd_sid_t sid,
                                      bcmdrd_fid_t fid, uint32_t *value)
{
    bcmdrd_sym_field_info_t finfo;
    int rv = SHR_E_NONE;
    uint16_t len = 0;

    sal_memset(&finfo, 0, sizeof(finfo));
    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_SUCCESS(rv)) {
        len = finfo.maxbit - finfo.minbit + 1;
    } else {
        len = 1;
    }
    *value = (1 << len) - 1;

    return rv;
}

/*
 * Certain memories are bit interleaved across different banks.
 * In such cases, a 2 bit error can only be triggered by writing
 * to the same bank.
 */
static int
bcm56780_a0_num_bit_interleave_get(int unit, bcmdrd_sid_t sid,
                               bcmdrd_fid_t fid, uint32_t *num_interleave)
{
    bcmdrd_sid_t select_sid[] = {
        ESEC_SA_TABLEm,
        ISEC_SA_TABLEm,
        ESEC_MIB_SCm,
        ESEC_MIB_SC_UNCTRLm,
        ISEC_MIB_SP_CTRL_2m,
        ISEC_MIB_SP_UNCTRLm,
    };
    bcmdrd_fid_t num_ileave[] = {
        2, /* ESEC_SA_TABLEm, */
        2, /* ISEC_SA_TABLEm, */
        1, /* ESEC_MIB_SCm, */
        1, /* ESEC_MIB_SC_UNCTRLm, */
        1, /* ISEC_MIB_SP_CTRL_2m, */
        1, /* ISEC_MIB_SP_UNCTRLm, */
    };
    int i;
    int num_sid = COUNTOF(select_sid);
    *num_interleave = 0;
    for (i = 0; i < num_sid; i++) {
        if (sid != select_sid[i]) {
            continue;
        }
        *num_interleave = num_ileave[i];
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_ser_status_field_parse(int unit, bcmdrd_sid_t sid,
                                   int fid_count, int *fval)
{
    if ((sid != ISEC_SCTCAM_SER_STATUSr) &&
        (sid != ISEC_SPTCAM_SER_STATUSr)) {
        return SHR_E_NONE;
    }
    if (fval[0] & 0x1) {
        /* Extract entry index as per SER_STATUS_BUS. */
        fval[3] = (fval[0] >> 5) & 0x3ffff;
        fval[0] = 0;
        fval[1] = 0;
        return SHR_E_NONE;
    }
    return SHR_E_PARAM;
}

static const bcmptm_ser_data_driver_t bcm56780_a0_data_driver = {

    .mem_clr_list_get = bcm56780_a0_ser_mem_clr_list_get,

    .clear_mem_after_ser_enable = bcm56780_a0_ser_clear_mem_after_ser_enable,

    .clear_mem_before_ser_enable = bcm56780_a0_ser_clear_mem_before_ser_enable,

    .tcam_ser_hw_scan_reg_info_get = bcm56780_a0_ser_tcam_hw_scan_ctrl_info_get,

    .acc_type_map_info_get = bcm56780_a0_ser_acc_type_map_lst_get,

    .tcam_cmic_ser_info_get = bcm56780_a0_ser_tcam_ser_info_get,

    .mute_mem_list_get = bcm56780_a0_ser_mem_mute_list_get,

    .mute_ctrl_reg_list_get = bcm56780_a0_ser_drop_ptk_ctrl_reg_mute_list_get,

    .blk_ctrl_regs_get = bcm56780_a0_ser_blk_ctrl_get,

    .intr_map_get = bcm56780_a0_ser_intr_map_get,

    .blk_type_map = bcm56780_a0_ser_blk_type_map,

    .ser_intr_enable = bcm56780_a0_ser_lp_intr_enable,

    .intr_cb_func_set = bcm56780_a0_ser_lp_intr_func_set,

    .cmic_ser_engine_regs_get = bcm56780_a0_ser_cmic_ser_engine_regs_get,

    .cmic_ser_engine_enable = bcm56780_a0_ser_cmic_ser_engine_enable,

    .cmic_ser_result_get = bcm56780_a0_ser_cmic_ser_result_get,

    .cmic_adext_get = bcm56780_a0_ser_cmicx_adext_get,

    .pt_acctype_get = bcm56780_a0_ser_acc_type_get,

    .pt_inst_remap = bcm56780_a0_ser_pt_inst_remap,

    .cmic_schan_opcode_get = bcm56780_a0_ser_cmicx_schan_opcode_get,

    .hwmem_base_info_get = bcm56780_a0_ser_hwmem_base_info_get,

    .ip_ep_hw_info_remap = bcm56780_a0_ser_hw_info_remap,

    .mmu_mem_remap = bcm56780_a0_ser_mmu_mem_remap,

    .ip_ep_mem_remap = bcm56780_a0_ser_ip_ep_sram_remap,

    .data_split_mem_test_fid_get = bcm56780_a0_ser_data_split_mem_test_field_get,

    .mem_ecc_info_get = bcm56780_a0_ser_mem_ecc_info_get,

    .tcam_remap = bcm56780_a0_ser_tcam_remap,

    .tcam_sticky_bit_clr_get = bcm56780_a0_ser_tcam_sticky_bit_clr_get,

    .refresh_regs_info_get = bcm56780_a0_ser_refresh_regs_info_get,

    .ser_status_reg_translate = bcm56780_a0_ser_ecc_status_translate,

    .sram_ctrl_reg_get = bcm56780_a0_ser_sram_ctrl_register_get,

    .xor_lp_mem_info_get = bcm56780_a0_ser_mem_xor_info_get,

    .sram_scan_mem_skip = bcm56780_a0_ser_sram_scan_mem_skip,

    .copy_to_cpu_regs_info_get = bcm56780_a0_ser_copy_to_cpu_regs_info_get,

    .num_bit_interleave_get = bcm56780_a0_num_bit_interleave_get,

    .ser_status_field_parse = bcm56780_a0_ser_status_field_parse,

    .expected_value_get = bcm56780_a0_ser_expected_value_get,

    .force_error_value_get = bcm56780_a0_ser_force_error_value_get,

    .tcam_ctrl_reg_list_get = NULL
};

/******************************************************************************
 * Public Functions
 */
const bcmptm_ser_data_driver_t *
bcm56780_a0_ptm_ser_data_driver_register(int unit)
{
    return &bcm56780_a0_data_driver;
}

