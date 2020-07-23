/*! \file bcm56990_b0_ptm_ser.c
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
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmbd/bcmbd_db_intr.h>
#include <bcmbd/bcmbd_port_intr.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>

#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmbd/chip/bcm56990_b0_mmu_intr.h>
#include <bcmbd/chip/bcm56990_b0_lp_intr.h>
#include <bcmbd/chip/bcm56990_b0_ipep_intr.h>
#include <bcmbd/chip/bcm56990_b0_db_intr.h>
#include <bcmbd/chip/bcm56990_b0_port_intr.h>

#include "bcm56990_b0_ptm_ser.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* ACC_TYPE     => $ACC_TYPE{SINGLE} */
#define ACC_TYPE_SINGLE           28

/* ACC_TYPE     => $ACC_TYPE{UNIQUE} */
#define ACC_TYPE_UNIQUE           31

/* ACC_TYPE     => $ACC_TYPE{DUPLICATE} */
#define ACC_TYPE_DUPLICATE        17

/* ACC_TYPE     => $ACC_TYPE{DATA_SPLIT} */
#define ACC_TYPE_DATA_SPLIT       22

/* ACC_TYPE     => $ACC_TYPE{ADDR_SPLIT} */
#define ACC_TYPE_ADDR_SPLIT       18

/* ACC_TYPE     => $ACC_TYPE{ADDR_SPLIT_SPLIT} */
#define ACC_TYPE_ADDR_SPLIT_SPLIT 20

/* ACC_TYPE     => $ACC_TYPE{DUPLICATE_2} */
#define ACC_TYPE_DUPLICATE_2      23

/* ACC_TYPE is duplicate */
#define SER_ACC_TYPE_IS_DUPLICATE(acc_type) \
    (ACC_TYPE_DUPLICATE == acc_type || \
     ACC_TYPE_DUPLICATE_2 == acc_type || \
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
    (((_port) - 1) / 4)
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
    BCMPTM_SER_BLK_PORT = 5
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
static bcmptm_ser_mem_clr_list_t bcm56990_b0_mem_clr_lst[] = {
    { MMU_THDO_QUEUE_DROP_COUNTm, -1 },
    { MMU_THDO_PORT_DROP_COUNT_MCm, -1 },
    { MMU_THDO_PORT_DROP_COUNT_UCm, -1 },
    { MMU_THDO_COUNTER_QUEUEm, -1 },
    { MMU_THDI_PORT_PG_MIN_COUNTERm, -1 },
    { MMU_THDI_PORT_PG_SHARED_COUNTERm, -1 },
    { MMU_THDI_PORT_PG_HDRM_COUNTERm, -1 },
    { MMU_THDI_PORTSP_COUNTERm, -1 },
    { MMU_THDO_SRC_PORT_DROP_COUNTm, -1 },
    { EGR_1588_LINK_DELAY_64r, -1 },
};

/* In order to clear ING/EDB/EGR SER FIFO, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t ing_egr_edb_ser_fifo_ctrl_fld_lst[] = {
    {FIFO_RESETf, {0}, 1, 0},
    {FIFO_RESETf, {0}, 0, 0} /* reset */
};

/* In order to init mmu(gcfg) memory tables, the following field should be set as 'val'  */
static bcmptm_ser_fld_info_t mmu_gcfg_miscconfig_fld_lst[] = {
    {ECCP_ENf, {0}, 1, 0},
    {INIT_MEMf, {0}, 1, 0},
    {INIT_MEMf, {0}, 0, 0},
    {ECCP_ENf, {0}, 0, 0} /* reset */
};

/* 1/ Disable memory refresh. 2/ Enable SER checking. 3/ Enable memory refresh.  */
static bcmptm_ser_fld_info_t ifp_refresh_config_fld_lst[] = {
    {IFP_REFRESH_DISABLEf, {0}, 1, 0},
    {IFP_REFRESH_DISABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t mmu_mtro_config_fld_lst[] = {
    {REFRESH_DISABLEf, {0}, 1, 0},
    {REFRESH_DISABLEf, {0}, 0, 0}
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
    {SRC_PORT_DROP_COUNT_ENABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t mmu_wred_refresh_control_fld_lst[] = {
    {REFRESH_DISABLEf, {0}, 1, 0},
    {REFRESH_DISABLEf, {0}, 0, 0}
};

static bcmptm_ser_fld_info_t dlb_ecmp_refresh_disable_fld_lst[] = {
    {DLB_REFRESH_DISABLEf, {0}, 1, 0},
    {DLB_REFRESH_DISABLEf, {0}, 0, 0}
};

/*
 * List of control registers which are needed to be configured, in order to clear memory tables
 * Those memory tables should be cleared before enabling SER
 */
static bcmptm_ser_ctrl_reg_info_t bcm56990_b0_clear_mem_before_ser_enable_info[] = {
    /* Clear ING SER FIFO */
    {ING_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {ING_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    {ING_DOI_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {ING_DOI_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    {FLEX_CTR_ING_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {FLEX_CTR_ING_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    /* Clear EGR SER FIFO */
    {EGR_DOI_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {EGR_DOI_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    {EGR_SER_FIFO_CTRL_2r, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {EGR_SER_FIFO_CTRL_2r, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    {FLEX_CTR_EGR_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {FLEX_CTR_EGR_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    /* Clear EDB SER FIFO */
    {EDB_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[0], 1, 0},
    {EDB_SER_FIFO_CTRLr, 0, &ing_egr_edb_ser_fifo_ctrl_fld_lst[1], 1, 0},
    /* Disable memory refresh */
    {IFP_REFRESH_CONFIGr, 0, &ifp_refresh_config_fld_lst[0], 1, 0},
    {MMU_MTRO_CONFIGr, 0, &mmu_mtro_config_fld_lst[0], 1, 0},
    {MMU_WRED_REFRESH_CONTROLr, 0, &mmu_wred_refresh_control_fld_lst[0], 1, 0},
    {DLB_ECMP_REFRESH_DISABLEr, 0, &dlb_ecmp_refresh_disable_fld_lst[0], 1, 0},
    /* used to clear memory tables belonged to MMU GCFG */
    {MMU_GLBCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[0], 1, 30},
    {MMU_GLBCFG_MISCCONFIGr, 0, &mmu_gcfg_miscconfig_fld_lst[1], 2, 20},
    /* Add new items here */
};

/*
 * List of control registers which are needed to be configured, in order to clear memory tables
 * The memory tables should be cleared after enabling SER
 */
static bcmptm_ser_ctrl_reg_info_t bcm56990_b0_clear_mem_after_ser_enable_info[] = {
    /* Enable memory refresh */
    {IFP_REFRESH_CONFIGr, 0, &ifp_refresh_config_fld_lst[1], 1, 0},
    {MMU_MTRO_CONFIGr, 0, &mmu_mtro_config_fld_lst[1], 1, 0},
    {MMU_WRED_REFRESH_CONTROLr, 0, &mmu_wred_refresh_control_fld_lst[1], 1, 0},
    {DLB_ECMP_REFRESH_DISABLEr, 0, &dlb_ecmp_refresh_disable_fld_lst[1], 1, 0}
     /* add new items here */
};

static bcmptm_ser_fld_info_t aux_arb_control_fld_lst[] = {
    {FP_REFRESH_ENABLEf, {0}, 0, 0},
    {DLB_HGT_256NS_REFRESH_ENABLEf , {0}, 0, 0},
    {FP_REFRESH_ENABLEf, {0}, 1, 0},
};

/* Configure IARB_SER_SCAN_CONFIGr to start H/W TCAM SER scan engine (non-cmic). */
static bcmptm_ser_fld_info_t iarb_ser_scan_config_fld_lst[] = {
    {START_IDXf, {0}, 0, 0},
    {MAX_IDXf, {0}, 1023, 0},
    {OP_BLOCKf, {0}, 0, 0},
    {INTERVALf, {0}, 64, 0},
    {ENABLEf, {0}, 1, 0},
    {ENABLEf, {0}, 0, 0},
};

/* Configure EGR_ARB_SER_SCAN_CONFIGr to start H/W TCAM SER scan engine (non-cmic). */
static bcmptm_ser_fld_info_t egr_arb_ser_scan_config_fld_lst[] = {
    {START_IDXf, {0}, 0, 0},
    {MAX_IDXf, {0}, 1023, 0},
    {OP_BLOCKf, {0}, 0, 0},
    {INTERVALf, {0}, 1000000, 0},
    {ENABLEf, {0}, 1, 0},
    {ENABLEf, {0}, 0, 0},
};

/*
 * List of control registers which are needed to be configured, in order to
 * start H/W TCAM ser scan engine.
 */
static bcmptm_ser_ctrl_reg_info_t bcm56990_b0_tcam_scan_ctrl_reg_lst_en[] = {
    /* {AUX_ARB_CONTROLr, 0, &aux_arb_control_fld_lst[2], 1, 0}, */
    {ING_DII_SER_SCAN_CONFIGr, 0, &iarb_ser_scan_config_fld_lst[0], 4, 0},
    {ING_DII_SER_SCAN_CONFIGr, 0, &iarb_ser_scan_config_fld_lst[4], 1, 0},
    {EGR_DII_SER_SCAN_CONFIGr, 0, &egr_arb_ser_scan_config_fld_lst[0], 4, 0},
    {EGR_DII_SER_SCAN_CONFIGr, 0, &egr_arb_ser_scan_config_fld_lst[4], 1, 0}
};

/*
 * List of control registers which are needed to be configured, in order to
 * stop H/W TCAM ser scan engine.
 */
static bcmptm_ser_ctrl_reg_info_t bcm56990_b0_tcam_scan_ctrl_reg_lst_dis[] = {
    /* {AUX_ARB_CONTROLr, 0, &aux_arb_control_fld_lst[0], 1, 0}, */
    {ING_DII_SER_SCAN_CONFIGr, 0, &iarb_ser_scan_config_fld_lst[0], 4, 0},
    {ING_DII_SER_SCAN_CONFIGr, 0, &iarb_ser_scan_config_fld_lst[5], 1, 0},
    {EGR_DII_SER_SCAN_CONFIGr, 0, &egr_arb_ser_scan_config_fld_lst[0], 4, 0},
    {EGR_DII_SER_SCAN_CONFIGr, 0, &egr_arb_ser_scan_config_fld_lst[5], 1, 0}
};

/* Fields to enable parity in SER_CONTROL. */
static bcmptm_ser_fld_info_t bcm56990_b0_ing_ser_ctrl_fld_lst[] = {
    {PARITY_ERR_TOCPUf, {0}, 0, 0},
};

/*
 * Control registers for configuring controls in SER_CONTROL.
 */
static bcmptm_ser_ctrl_reg_info_t bcm56990_b0_ser_ctrl_info[] = {
    {CPU_CONTROL_0r, 0, &bcm56990_b0_ing_ser_ctrl_fld_lst[0], 1, 0},
};

/*!
 * Can't enable parity checking for some memory tables.
 */
static bcmdrd_sid_t  bcm56990_b0_mem_ser_mute[] = {
    INVALIDm
};

/* Field list of status registers for  BCMPTM_SER_INFO_TYPE_ECC, except ECC_ERR. */
static bcmdrd_fid_t bcm56990_b0_ecc_status_fields[] = {
    MULTIPLE_ERRf,
    ENTRY_IDXf,
    DOUBLE_BIT_ERRf,
    INVALIDf /* must */
};

static bcmptm_ser_ecc_status_fld_t bcm56990_b0_ecc_status_flds = {
    ECC_ERRf, bcm56990_b0_ecc_status_fields
};

/* Field list of registers which are used to enable SER function. */
static bcmdrd_fid_t bcm56990_b0_edb_ctrl_buffer_ecc_en_fields[] = {
    PM3_ECC_ENf,
    PM2_ECC_ENf,
    PM1_ECC_ENf,
    PM0_ECC_ENf,
    XMIT_START_COUNT_ECC_ENf,
    AUX_ECC_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_edb_data_buffer_en_fields[] = {
    PM3_ECC_ENf,
    PM2_ECC_ENf,
    PM1_ECC_ENf,
    PM0_ECC_ENf,
    RESIDUAL_ECC_ENf,
    AUX_ECC_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_edb_ctrl_data_buffer_1b_en_fields[] = {
    PM3_ENf,
    PM2_ENf,
    PM1_ENf,
    PM0_ENf,
    XMIT_START_COUNT_ENf,
    AUX_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_edb_data_buffer_1b_en_fields[] = {
    PM3_ENf,
    PM2_ENf,
    PM1_ENf,
    PM0_ENf,
    RESIDUAL_ENf,
    AUX_ENf,
    INVALIDf /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56990_b0_idb_obm0_status_reg[] = {
    { IDB_OBM0_DATA_ECC_STATUSr, "IDB_OBM0_DATA_ECC_STATUS", -1, -1 },
    { IDB_OBM0_CTRL_ECC_STATUSr, "IDB_OBM0_CTRL_ECC_STATUS", -1, -1 },
    { IDB_OBM0_OVERSUB_MON_ECC_STATUSr, "IDB_OBM0_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56990_b0_idb_obm1_status_reg[] = {
    { IDB_OBM1_DATA_ECC_STATUSr, "IDB_OBM1_DATA_ECC_STATUS", -1, -1 },
    { IDB_OBM1_CTRL_ECC_STATUSr, "IDB_OBM1_CTRL_ECC_STATUS", -1, -1 },
    { IDB_OBM1_OVERSUB_MON_ECC_STATUSr, "IDB_OBM1_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56990_b0_idb_obm2_status_reg[] = {
    { IDB_OBM2_DATA_ECC_STATUSr, "IDB_OBM2_DATA_ECC_STATUS", -1, -1 },
    { IDB_OBM2_CTRL_ECC_STATUSr, "IDB_OBM2_CTRL_ECC_STATUS", -1, -1 },
    { IDB_OBM2_OVERSUB_MON_ECC_STATUSr, "IDB_OBM2_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56990_b0_idb_obm3_status_reg[] = {
    { IDB_OBM3_DATA_ECC_STATUSr, "IDB_OBM3_DATA_ECC_STATUS", -1, -1 },
    { IDB_OBM3_CTRL_ECC_STATUSr, "IDB_OBM3_CTRL_ECC_STATUS", -1, -1 },
    { IDB_OBM3_OVERSUB_MON_ECC_STATUSr, "IDB_OBM3_OVERSUB_MON_ECC_STATUS", -1, -1 },
    { INVALIDr, "INVALID" }
};

static bcmdrd_fid_t bcm56990_b0_idb_pfc_mon_en_fields[] = {
    PFC_MON_EN_COR_ERR_RPTf,
    PFC_MON_ECC_ENABLEf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_idb_ca_aux_en_fields[] = {
    CA_AUX_FIFO_EN_COR_ERR_RPTf,
    CA_AUX_FIFO_ECC_ENABLEf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_idb_ca_fifo_en_fields[] = {
    CA_FIFO_EN_COR_ERR_RPTf,
    CA_FIFO_ECC_ENABLEf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_idb_obm_en_fields[] = {
    MON_EN_COR_ERR_RPTf,
    MON_ECC_ENABLEf,
    CTRL_EN_COR_ERR_RPTf,
    CTRL_ECC_ENABLEf,
    DATA_EN_COR_ERR_RPTf,
    DATA_ECC_ENABLEf,
    INVALIDf /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_cdmac_ecc_tx_1bit_flds[] = {
    { TX_CDC_SINGLE_BIT_ERRf, INVALIDf, 1, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_cdmac_ecc_tx_2bit_flds[] = {
    { TX_CDC_DOUBLE_BIT_ERRf, INVALIDf, 0, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_cdmac_ecc_mib_1bit_flds[] = {
    { MIB_COUNTER_SINGLE_BIT_ERRf, INVALIDf, 1, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_cdmac_ecc_mib_2bit_flds[] = {
    { MIB_COUNTER_DOUBLE_BIT_ERRf, INVALIDf, 0, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_cdmac_ecc_mib_multi_flds[] = {
    { MIB_COUNTER_MULTIPLE_ERRf, INVALIDf, 2, BCMPTM_SER_CLR_INTR_READ },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmdrd_fid_t bcm56990_b0_cdmac_ecc_ctrl_flds[] = {
    TX_CDC_ECC_CTRL_ENf,
    MIB_COUNTER_ECC_CTRL_ENf,
    INVALIDf /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56990_b0_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC", -1, -1 },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_buf_bus_ctrl_reg_t bcm56990_b0_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC", -1, -1 },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC", -1, -1 },
    { INVALIDr } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_xlmac_rx_cdc_1bit_err[] = {
    { RX_CDC_SINGLE_BIT_ERRf, CLEAR_RX_CDC_SINGLE_BIT_ERRf, 1, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_xlmac_rx_cdc_2bit_err[] = {
    { RX_CDC_DOUBLE_BIT_ERRf, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, 0, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_xlmac_tx_cdc_1bit_err[] = {
    { TX_CDC_SINGLE_BIT_ERRf, CLEAR_TX_CDC_SINGLE_BIT_ERRf, 1, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_xlmac_tx_cdc_2bit_err[] = {
    { TX_CDC_DOUBLE_BIT_ERRf, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, 0, BCMPTM_SER_CLR_INTR_RAISE_EDGE },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmdrd_fid_t bcm56990_b0_xlmac_ecc_ctrl_flds[] = {
    TX_CDC_ECC_CTRL_ENf,
    RX_CDC_ECC_CTRL_ENf,
    INVALIDf /* must */
};

static bcmdrd_fid_t bcm56990_b0_xlport_ecc_ctrl_flds[] = {
    MIB_TSC_MEM_ENf,
    MIB_RSC_MEM_ENf,
    INVALIDf /* must */
};

static bcmptm_ser_ctrl_en_t bcm56990_b0_ser_blocks_ctrl_regs[] = {
    { CDMAC_ECC_CTRLr, INVALIDf, bcm56990_b0_cdmac_ecc_ctrl_flds },

    { XLMAC_ECC_CTRLr, INVALIDf, bcm56990_b0_xlmac_ecc_ctrl_flds },

    { XLPORT_ECC_CONTROLr, INVALIDf, bcm56990_b0_xlport_ecc_ctrl_flds },

    { EDB_CONTROL_BUFFER_ECC_ENr, INVALIDf, bcm56990_b0_edb_ctrl_buffer_ecc_en_fields },

    { EDB_DATA_BUFFER_ECC_ENr, INVALIDf, bcm56990_b0_edb_data_buffer_en_fields },

    { EDB_CONTROL_BUFFER_EN_COR_ERR_RPTr, INVALIDf, bcm56990_b0_edb_ctrl_data_buffer_1b_en_fields },

    { EDB_DATA_BUFFER_EN_COR_ERR_RPTr, INVALIDf, bcm56990_b0_edb_data_buffer_1b_en_fields },

    { IDB_PFC_MON_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_pfc_mon_en_fields },

    { IDB_CA_AUX_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_ca_aux_en_fields },

    { IDB_CA3_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_ca_fifo_en_fields },

    { IDB_OBM3_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_obm_en_fields },

    { IDB_CA2_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_ca_fifo_en_fields },

    { IDB_OBM2_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_obm_en_fields },

    { IDB_CA1_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_ca_fifo_en_fields },

    { IDB_OBM1_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_obm_en_fields },

    { IDB_CA0_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_ca_fifo_en_fields },

    { IDB_OBM0_SER_CONTROLr, INVALIDf, bcm56990_b0_idb_obm_en_fields }
};

/*
 * field list of memory ING_SER_FIFOm etc.
 */
bcmdrd_fid_t  bcm56990_b0_ser_ing_egr_fifo_fld[] = {
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
    HW_CORRECTIONf
};

/*
 * fields of MMU_SCFG_MEM_FAIL_ADDR_64m/ MMU_XCFG_MEM_FAIL_ADDR_64m/
 * MMU_GCFG_MEM_FAIL_ADDR_64m.
 * Sequence of fields cannot be changed.
 */
static bcmdrd_fid_t bcm56990_b0_mem_fail_addr_flds[] = {
    ERR_MULTf,
    ERR_TYPEf,
    EADDRf
};

static bcmptm_serc_fifo_t bcm56990_b0_serc_mmu_info[] = {
    {
        MMU_GLBCFG_MEM_SER_FIFO_STSr, EMPTYf,
        MMU_GLBCFG_MEM_FAIL_ADDR_64m, bcm56990_b0_mem_fail_addr_flds,
        MMU_GLBCFG_MEM_FAIL_INT_CTRr, BLKTYPE_MMU_GLB, "MMU_GLB"
    },
    {
        MMU_ITMCFG_MEM_SER_FIFO_STSr, EMPTYf,
        MMU_ITMCFG_MEM_FAIL_ADDR_64m, bcm56990_b0_mem_fail_addr_flds,
        MMU_ITMCFG_MEM_FAIL_INT_CTRr, BLKTYPE_MMU_ITM, "MMU_ITM"
    },
    {
        MMU_EBCFG_MEM_SER_FIFO_STSr, EMPTYf,
        MMU_EBCFG_MEM_FAIL_ADDR_64m, bcm56990_b0_mem_fail_addr_flds,
        MMU_EBCFG_MEM_FAIL_INT_CTRr, BLKTYPE_MMU_EB, "MMU_EB"
    }
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_ing_dii_intr_stat_flds[] = {
    { EVENT_FIFO_SER_ERRf, EVENT_FIFO_SER_ERRf, 0, BCMPTM_SER_CLR_INTR_WRITE_ZERO },
    { INVALIDf, INVALIDf, -1 } /* must */
};

static bcmptm_ser_mac_status_fld_t bcm56990_b0_egr_dii_intr_stat_flds[] = {
    { EVENT_FIFO_SER_ERRf, EVENT_FIFO_SER_ERRf, 0, BCMPTM_SER_CLR_INTR_WRITE_ZERO },
    { INVALIDf, INVALIDf, -1 } /* must */
};

/* Write EGR_DII_INTR_STATUSr to 0 to clear interrupt */
static bcmptm_serc_non_fifo_t bcm56990_b0_serc_egr_dii_info[] = {
    {
        INVALIDr, INVALIDf,
        0, EGR_DII_INTR_STATUSr, NULL, EGR_DII_INTR_STATUSr,
        NULL, bcm56990_b0_egr_dii_intr_stat_flds, NULL,
        BLKTYPE_EPIPE, "EPIPE DII memory"
    },
    {
        INVALIDr, INVALIDf,
        0, EGR_DII_INTR_STATUSr, NULL, EGR_DII_INTR_STATUSr,
        NULL, bcm56990_b0_egr_dii_intr_stat_flds, NULL,
        BLKTYPE_EPIPE, "EPIPE DII memory"
    },
    {
        INVALIDr, INVALIDf,
        0, EGR_DII_INTR_STATUSr, NULL, EGR_DII_INTR_STATUSr,
        NULL, bcm56990_b0_egr_dii_intr_stat_flds, NULL,
        BLKTYPE_EPIPE, "EPIPE DII memory"
    },
    {
        INVALIDr, INVALIDf,
        0, EGR_DII_INTR_STATUSr, NULL, EGR_DII_INTR_STATUSr,
        NULL, bcm56990_b0_egr_dii_intr_stat_flds, NULL,
        BLKTYPE_EPIPE, "EPIPE DII memory"
    }
};

/* Write ING_DII_INTR_STATUSr to 0 to clear interrupt */
static bcmptm_serc_non_fifo_t bcm56990_b0_serc_ing_dii_info[] = {
    {
        INVALIDr, INVALIDf,
        0, ING_DII_INTR_STATUSr, NULL, ING_DII_INTR_STATUSr,
        NULL, bcm56990_b0_ing_dii_intr_stat_flds, NULL,
        BLKTYPE_IPIPE, "IPIPE DII memory"
    },
    {
        INVALIDr, INVALIDf,
        0, ING_DII_INTR_STATUSr, NULL, ING_DII_INTR_STATUSr,
        NULL, bcm56990_b0_ing_dii_intr_stat_flds, NULL,
        BLKTYPE_IPIPE, "IPIPE DII memory"
    },
    {
        INVALIDr, INVALIDf,
        0, ING_DII_INTR_STATUSr, NULL, ING_DII_INTR_STATUSr,
        NULL, bcm56990_b0_ing_dii_intr_stat_flds, NULL,
        BLKTYPE_IPIPE, "IPIPE DII memory"
    },
    {
        INVALIDr, INVALIDf,
        0, ING_DII_INTR_STATUSr, NULL, ING_DII_INTR_STATUSr,
        NULL, bcm56990_b0_ing_dii_intr_stat_flds, NULL,
        BLKTYPE_IPIPE, "IPIPE DII memory"
    }
};

/* CDMAC_ECC_STATUSr is cleared-on-read */
static bcmptm_serc_non_fifo_t bcm56990_b0_serc_cdp_info[] = {
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56990_b0_cdmac_ecc_tx_1bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56990_b0_cdmac_ecc_tx_2bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56990_b0_cdmac_ecc_mib_1bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56990_b0_cdmac_ecc_mib_2bit_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    },
    {
        INVALIDr, INVALIDf,
        0, CDMAC_ECC_STATUSr, NULL, INVALIDr,
        NULL, bcm56990_b0_cdmac_ecc_mib_multi_flds, NULL,
        BLKTYPE_CDPORT, "CDMAC SER error"
    }
};

static bcmptm_serc_non_fifo_t bcm56990_b0_serc_xlp_info[] = {
    {
        INVALIDr, INVALIDf,
        0, XLMAC_RX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56990_b0_xlmac_rx_cdc_1bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC RX 1 bit error"
    },
    {
        INVALIDr, INVALIDf,
        0, XLMAC_RX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56990_b0_xlmac_rx_cdc_2bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC RX 2 bit error"
    },
    {
        INVALIDr, INVALIDf,
        0, XLMAC_TX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56990_b0_xlmac_tx_cdc_1bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC TX 1 bit error"
    },
    {
        INVALIDr, INVALIDf,
        0, XLMAC_TX_CDC_ECC_STATUSr, NULL, XLMAC_CLEAR_ECC_STATUSr,
        NULL, bcm56990_b0_xlmac_tx_cdc_2bit_err, NULL,
        BLKTYPE_XLPORT, "XLP MAC TX 2 bit error"
    },
    /* W1TC */
    {
        XLPORT_INTR_STATUSr, MIB_TX_MEM_ERRf,
        1, INVALIDr, bcm56990_b0_pm_xlp_tx_status_reg, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_XLPORT, "XLP MIB TX Statistic Counter memory"
    },
    /* W1TC */
    {
        XLPORT_INTR_STATUSr, MIB_RX_MEM_ERRf,
        1, INVALIDr, bcm56990_b0_pm_xlp_rx_status_reg, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_XLPORT, "XLP MIB RX Statistic Counter memory"
    }
};

static bcmptm_serc_fifo_t bcm56990_b0_serc_dlb_info[] = {
    {
        DLB_ECMP_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        DLB_ECMP_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_IPIPE, "IPIPE"
    }
};

static bcmptm_serc_fifo_t bcm56990_b0_serc_ip_info[] = {
    {
        INVALIDr, INVALIDf,
        ING_DOI_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_IPIPE, "IPIPE"
    },
    {
        FLEX_CTR_ING_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        FLEX_CTR_ING_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_FLEX_CTR, "IPIPE"
    },
    {
        IP4_INTR_STATUSr, ISWf,
        ING_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_IPIPE, "IPIPE"
    }
};

static bcmptm_serc_fifo_t bcm56990_b0_serc_ep_info[] = {
    {
        INVALIDr, INVALIDf,
        EGR_SER_FIFO_2m, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_EPIPE, "EPIPE"
    },
    {
        INVALIDr, INVALIDf,
        EGR_DOI_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_EPIPE, "EPIPE"
    },
    {
        FLEX_CTR_EGR_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        FLEX_CTR_EGR_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_FLEX_CTR, "EPIPE"
    }
};

static bcmptm_serc_non_fifo_t bcm56990_b0_serc_idb_info[] = {
    {
        INVALIDr, INVALIDf,
        1, IDB_PFC_MON_ECC_STATUSr, NULL, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA-LPBK packet buffer memory"
    },
    {
        INVALIDr, INVALIDf,
        1, IDB_CA_AUX_ECC_STATUSr, NULL, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "AUX CA ECC error"
    },
    {
        INVALIDr, INVALIDf,
        1, IDB_CA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA3 FIFO"
    },
    {
        INVALIDr, INVALIDf,
        1, INVALIDr, bcm56990_b0_idb_obm3_status_reg, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM3 packet data memory"
    },
    {
        INVALIDr, INVALIDf,
        1, IDB_CA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA2 FIFO"
    },
    {
        INVALIDr, INVALIDf,
        1, INVALIDr, bcm56990_b0_idb_obm2_status_reg, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM2 packet data memory"
    },
    {
        INVALIDr, INVALIDf,
        1, IDB_CA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA1 FIFO"
    },
    {
        INVALIDr, INVALIDf,
        1, INVALIDr, bcm56990_b0_idb_obm1_status_reg, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM1 packet data memory"
    },
    {
        INVALIDr, INVALIDf,
        1, IDB_CA_ECC_STATUSr, NULL, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "CA0 FIFO"
    },
    {
        INVALIDr, INVALIDf,
        1, INVALIDr, bcm56990_b0_idb_obm0_status_reg, INVALIDr,
        &bcm56990_b0_ecc_status_flds, NULL, NULL,
        BLKTYPE_PORT_IF, "IDB OBM0 packet data memory"
    }
};

static bcmptm_serc_fifo_t bcm56990_b0_serc_edb_info[] = {
    {
        EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
        EDB_SER_FIFOm, bcm56990_b0_ser_ing_egr_fifo_fld,
        INVALIDr, BLKTYPE_PORT_IF, "EPIPE EDB"
    }
};

static bcmptm_ser_intr_route_info_t bcm56990_b0_intr_map[] = {
    {
        BCMPTM_SER_MMU_INTR_FLG,
        { MMU_INTR_GLB_MEM_PAR_ERR, MMU_INTR_ITM_MEM_PAR_ERR, MMU_INTR_EB_MEM_PAR_ERR },
        MMU_TO_CMIC_INTR, MMU_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_MMU,
        (void *)bcm56990_b0_serc_mmu_info, COUNTOF(bcm56990_b0_serc_mmu_info), 1
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        {
            ING_DII_EVENT_FIFO0_SER_ERR, ING_DII_EVENT_FIFO1_SER_ERR,
            ING_DII_EVENT_FIFO2_SER_ERR, ING_DII_EVENT_FIFO3_SER_ERR
        },
        QUAD0_IP_TO_CMIC_PIPE_INTR, QUAD3_IP_TO_CMIC_PIPE_INTR, 1,
        BCMPTM_SER_INFO_TYPE_IPIPE_DII,
        (void *)bcm56990_b0_serc_ing_dii_info, COUNTOF(bcm56990_b0_serc_ing_dii_info), 0
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        {
            EGR_DII_EVENT_FIFO0_SER_ERR, EGR_DII_EVENT_FIFO1_SER_ERR,
            EGR_DII_EVENT_FIFO2_SER_ERR, EGR_DII_EVENT_FIFO3_SER_ERR
        },
        QUAD0_EP_TO_CMIC_INTR, QUAD3_EP_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_EPIPE_DII,
        (void *)bcm56990_b0_serc_egr_dii_info, COUNTOF(bcm56990_b0_serc_egr_dii_info), 0
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        { DLB_ECMP_SER_FIFO_NON_EMPTY },
        DLB_CREDIT_TO_CMIC_INTR, DLB_CREDIT_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_IPIPE_DLB_ECMP,
        (void *)bcm56990_b0_serc_dlb_info, COUNTOF(bcm56990_b0_serc_dlb_info), 1
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        { ING_DOI_SER_FIFO_NON_EMPTY, FLEX_CTR_ING_SER_FIFO_NON_EMPTY, IP4_ISW },
        QUAD0_IP_TO_CMIC_PIPE_INTR, QUAD3_IP_TO_CMIC_PIPE_INTR, 1,
        BCMPTM_SER_INFO_TYPE_IPIPE,
        (void *)bcm56990_b0_serc_ip_info, COUNTOF(bcm56990_b0_serc_ip_info), 1
    },
    {
        BCMPTM_SER_IPEP_INTR_FLG,
        { EP2C_EFPMOD, EGR_DOI_SER_FIFO_NON_EMPTY, FLEX_CTR_EGR_SER_FIFO_NON_EMPTY },
        QUAD0_EP_TO_CMIC_INTR, QUAD3_EP_TO_CMIC_INTR, 0,
        BCMPTM_SER_INFO_TYPE_EPIPE,
        (void *)bcm56990_b0_serc_ep_info, COUNTOF(bcm56990_b0_serc_ep_info), 1
    },
    {
        BCMPTM_SER_EDB_IDB_INTR_FLG,
        {
            IDB_PFC_MON_ECC_ERR, IDB_AUX_FIFO_ECC_ERR, IDB_CA3_FIFO_ECC_ERR,
            IDB_OBM3_ECC_ERR, IDB_CA2_FIFO_ECC_ERR, IDB_OBM2_ECC_ERR,
            IDB_CA1_FIFO_ECC_ERR, IDB_OBM1_ECC_ERR, IDB_CA0_FIFO_ECC_ERR,
            IDB_OBM0_ECC_ERR
        },
        PIPE0_IDB_EDB_TO_CMIC_PERR_INTR, PIPE15_IDB_EDB_TO_CMIC_PERR_INTR, 0,
        BCMPTM_SER_INFO_TYPE_IPIPE_IDB,
        (void *)bcm56990_b0_serc_idb_info, COUNTOF(bcm56990_b0_serc_idb_info), 0
    },
    {
        BCMPTM_SER_EDB_IDB_INTR_FLG,
        { EDB_SER_FIFO_NON_EMPTY },
        PIPE0_IDB_EDB_TO_CMIC_PERR_INTR, PIPE15_IDB_EDB_TO_CMIC_PERR_INTR, 0,
        BCMPTM_SER_INFO_TYPE_EPIPE_EDB,
        (void *)bcm56990_b0_serc_edb_info, COUNTOF(bcm56990_b0_serc_edb_info), 1
    },
    /* Use to clear SER error */
    {
        BCMPTM_SER_PORT_INTR_FLG,
        {
            CDMAC_INTR_TX_CDC_SINGLE_BIT_ERR, CDMAC_INTR_TX_CDC_DOUBLE_BIT_ERR,
            CDMAC_INTR_MIB_COUNTER_SINGLE_BIT_ERR, CDMAC_INTR_MIB_COUNTER_DOUBLE_BIT_ERR,
            CDMAC_INTR_MIB_COUNTER_MULTIPLE_ERR
        },
        PM0_INTR, PM63_INTR, 0,
        BCMPTM_SER_INFO_TYPE_CDPORT,
        (void *)bcm56990_b0_serc_cdp_info, COUNTOF(bcm56990_b0_serc_cdp_info), 0
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
        (void *)bcm56990_b0_serc_xlp_info, COUNTOF(bcm56990_b0_serc_xlp_info), 0
    }
};

/*
 * If 'stage id' is 0xFF, that means 'membase' can identify buffer or bus.
 * The 'stage id' reported by H/W is ignored.
 * If 'stage id' is less than 0xFF, that means two or more buffers or buses share the
 * same 'membase', so both 'membase' and 'stage id' should be used to
 * identify buffer or bus.
 */
static bcmptm_ser_buffer_bus_info_t bcm56990_b0_ser_hwmem_base_info[] = {
    { "IDII_EVENT_FIFO_0", "Ingress DII event fifo 0", 0x0, 0xff, 1},
    { "IDII_EVENT_FIFO_1", "Ingress DII event fifo 2", 0x1, 0xff, 1},
    { "IDII_EVENT_FIFO_2", "Ingress DII event fifo 3", 0x2, 0xff, 0},
    { "IDII_EVENT_FIFO_3", "Ingress DII event fifo 4", 0x3, 0xff, 0},
    { "INGRESS_PACKET_BUFFER", "In Ipars", 0x7, 0xff, 0},
    { "ISW_EOP_BUFFER_B", "In Isw3", 0x9, 0xff, 0},
    { "IADAPT_PASSTHRU", "Iadapt passthru bus", 0xb, 0xff, 0},
    { "T0_SOP_IPARS_TO_ISW_PT_HWY_FIFO", "In Isw, Passthru Highway bus fifo", 0x10, 0xff, 0},
    { "T1_SOP_IPARS_TO_ISW_PT_HWY_FIFO", "In Isw, Passthru Highway bus fifo", 0x11, 0xff, 1},
    { "T0_SOP_IFWD1_TO_ISW_PT_HWY_FIFO", "In Isw, Passthru Highway bus fifo", 0x12, 0xff, 1},
    { "T1_SOP_IFWD1_TO_ISW_PT_HWY_FIFO", "In Isw, Passthru Highway bus fifo", 0x13, 0xff, 1},
    { "T0_SOP_IFP_TO_ISW_PT_HWY_FIFO", "In Isw, Passthru Highway bus fifo", 0x14, 0xff, 0},
    { "T1_SOP_IFP_TO_ISW_PT_HWY_FIFO", "In Isw, Passthru Highway bus fifo", 0x15, 0xff, 0},
    { "IFP_BUS", "PassThrought bus parity check, MEMINDEX is 0", 0x17, 0xff, 0},
    { "ING_DOI_OUTPUT_FIFO_0", "Ingress DOI output event fifo 0", 0x20, 0xff, 0},
    { "ING_DOI_OUTPUT_FIFO_1", "Ingress DOI output event fifo 1", 0x21, 0xff, 1},
    { "ING_DOI_OUTPUT_FIFO_2", "Ingress DOI output event fifo 2", 0x22, 0xff, 1},
    { "ING_DOI_OUTPUT_FIFO_3", "Ingress DOI output event fifo 3", 0x23, 0xff, 1},
    { "ING_DOI_SLOT_PIPELINE_0", "Ingress DOI slot pipeline 0", 0x24, 0xff, 1},
    { "ING_DOI_SLOT_PIPELINE_1", "Ingress DOI slot pipeline 1", 0x25, 0xff, 1},
    { "ING_DOI_SLOT_PIPELINE_2", "Ingress DOI slot pipeline 2", 0x26, 0xff, 1},
    { "ING_DOI_SLOT_PIPELINE_3", "Ingress DOI slot pipeline 3", 0x27, 0xff, 1},
    { "ING_DOI_PKT_BUFFER_0", "Ingress DOI packet buffer 0", 0x28, 0xff, 1},
    { "ING_DOI_PKT_BUFFER_1", "Ingress DOI packet buffer 1", 0x29, 0xff, 1},
    { "ING_DOI_PKT_BUFFER_2", "Ingress DOI packet buffer 2", 0x2a, 0xff, 1},
    { "ING_DOI_PKT_BUFFER_3", "Ingress DOI packet buffer 3", 0x2b, 0xff, 1},
    { "ING_DOI_CELL_QUEUES_0", "Ingress DOI cell queue 0", 0x2c, 0xff, 1},
    { "ING_DOI_CELL_QUEUES_1", "Ingress DOI cell queue 1", 0x2d, 0xff, 1},
    { "ING_DOI_CELL_QUEUES_2", "Ingress DOI cell queue 2", 0x2e, 0xff, 1},
    { "ING_DOI_CELL_QUEUES_3", "Ingress DOI cell queue 3", 0x2f, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_0", "Egress DOI output event control fifo 0", 0x30, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_1", "Egress DOI output event control fifo 0", 0x31, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_2", "Egress DOI output event control fifo 0", 0x32, 0xff, 1},
    { "EGR_DOI_OUTPUT_FIFO_3", "Egress DOI output event control fifo 0", 0x33, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_0", "Egress DOI slot pipeline 0", 0x34, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_1", "Egress DOI slot pipeline 0", 0x35, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_2", "Egress DOI slot pipeline 0", 0x36, 0xff, 1},
    { "EGR_DOI_SLOT_PIPELINE_3", "Egress DOI slot pipeline 0", 0x37, 0xff, 1},
    { "CENTRAL_EVICTION_FIFO", "In CEV", 0x38, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_0", "Egress DOI packet buffer 0", 0x38, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_1", "Egress DOI packet buffer 1", 0x39, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_2", "Egress DOI packet buffer 2", 0x3a, 0xff, 1},
    { "EGR_DOI_PKT_BUFFER_3", "Egress DOI packet buffer 3", 0x3b, 0xff, 1},
    { "EP_PACKET_DATA_0", "Egress DOI 0", 0x3c, 0xff, 1},
    { "EP_PACKET_DATA_1", "Egress DOI 1", 0x3d, 0xff, 1},
    { "EP_PACKET_DATA_2", "Egress DOI 2", 0x3e, 0xff, 1},
    { "EP_PACKET_DATA_3", "Egress DOI 3", 0x3f, 0xff, 1},
    { "EDII_EVENT_FIFO_0", "Egress DII event fifo 0", 0x44, 0xff, 1},
    { "EDII_EVENT_FIFO_1", "Egress DII event fifo 1", 0x45, 0xff, 1},
    { "EDII_EVENT_FIFO_2", "Egress DII event fifo 2", 0x46, 0xff, 1},
    { "EDII_EVENT_FIFO_3", "Egress DII event fifo 3", 0x47, 0xff, 1},
    { "EP_MPB_BUS_0", "Egress DOI 0", 0x48, 0xff, 1},
    { "EP_MPB_BUS_1", "Egress DOI 1", 0x49, 0xff, 1},
    { "EP_MPB_BUS_2", "Egress DOI 1", 0x4a, 0xff, 1},
    { "EP_MPB_BUS_3", "Egress DOI 1", 0x4b, 0xff, 1},
    { "EGR_DOI_OUTPUT_DATA_FIFO_0", "Egress DOI output event data fifo 0", 0x4c, 0xff, 1},
    { "EGR_DOI_OUTPUT_DATA_FIFO_1", "Egress DOI output event data fifo 0", 0x4d, 0xff, 1},
    { "EGR_DOI_OUTPUT_DATA_FIFO_2", "Egress DOI output event data fifo 0", 0x4e, 0xff, 1},
    { "EGR_DOI_OUTPUT_DATA_FIFO_3", "Egress DOI output event data fifo 0", 0x4f, 0xff, 1},
    { "EGR_PMOD_BUS", "PassThrought bus parity check, MEMINDEX is 0", 0x5c, 0xff, 1},
    { "EP_EFPBUF", "In Efpmod", 0x60, 0xff, 1},
    { "EDB_RESIDUAL_DATA_BUF", "EDB residual data buffer", 0x67, 0xff, 1},
    { "EDB_AUX_CPU_CTRL_FIFO", "EDB AUX CPU CTRL FIFO", 0x7c, 0xff, 1},
    { "EDB_AUX_EOP_CTRL_FIFO", "EDB AUX EOP CTRL FIFO", 0x7d, 0xff, 1},
    { "EDB_AUX_DATA_FIFO", "EDB AUX DATA FIFO", 0x7e, 0xff, 1},
    { "IFWD1_PASSTHRU", "Ifwd1 passthru bus", 0x80, 0xff, 1},
    { "IFWD1_PASSTHRU_1", "Ifwd1 passthru bus 1", 0x81, 0xff, 1},
    { "IFWD1_PASSTHRU_2A", "Ifwd1 passthru bus 2A", 0x82, 0xff, 1},
    { "IFWD1_PASSTHRU_2B", "Ifwd1 passthru bus 2B", 0x83, 0xff, 1},
    { "T0_EPARS_TO_EFPMOD_PT_HWY_FIFO", "In Efpmod, T0 EPARS_TO_EFPMOD_PT_HWY_FIFO", 0x84, 0xff, 1},
    { "T1_EPARS_TO_EFPMOD_PT_HWY_FIFO", "In Efpmod, T1 EPARS_TO_EFPMOD_PT_HWY_FIFO", 0x85, 0xff, 1}
};

/* During SER error injection, Memory refresh should be disable */
static bcmptm_ser_ctrl_reg_info_t bcm56990_b0_clear_refresh_registers[] = {
    { ING_DII_AUX_ARB_CONTROLr, 0, &aux_arb_control_fld_lst[1], 1, 0 },
    { IFP_REFRESH_CONFIGr, 0, &ifp_refresh_config_fld_lst[0], 1, 0 },
    { DLB_ECMP_REFRESH_DISABLEr, 0, &dlb_ecmp_refresh_disable_fld_lst[0], 1, 0 },
    { MMU_WRED_REFRESH_CONTROLr, 0, &mmu_wred_refresh_control_fld_lst[0], 1, 0 },
    { MMU_MTRO_CONFIGr, 0, &mmu_mtro_config_fld_lst[0], 1, 0 },
    { MMU_EBPTS_CBMG_VALUEr, 0, &mmu_ebpts_cbmg_value_fld_lst[0], 2, 0 },
    { MMU_QSCH_VOQ_FAIRNESS_CONFIGr, 0, &mmu_qsch_voq_fairness_fld_lst[0], 1, 0 },
    { MMU_THDO_ENGINE_ENABLES_CFGr, 0, &mmu_thdo_engine_enables_cfg_fld_lst[0], 2, 0 }
};

static bcmptm_ser_acctype_info_t bcm56990_b0_acctype_info[] = {
    { ACC_TYPE_SINGLE , ACC_TYPE_SINGLE_STR },
    { ACC_TYPE_UNIQUE, ACC_TYPE_UNIQUE_STR },
    { ACC_TYPE_DUPLICATE, ACC_TYPE_DUPLICATE_STR},
    { ACC_TYPE_DATA_SPLIT, ACC_TYPE_DATA_SPLIT_STR},
    { ACC_TYPE_ADDR_SPLIT, ACC_TYPE_ADDR_SPLIT_STR},
    { ACC_TYPE_ADDR_SPLIT_SPLIT, ACC_TYPE_ADDR_SPLIT_SPLIT_STR},
    { ACC_TYPE_DUPLICATE_2, ACC_TYPE_DUPLICATE_2_STR},
    /*
     * There is no access type for some PTs,
     * Such as: DLB_ECMP_PORT_AVG_QUALITY_MEASUREm.
     */
    { 0, "unknown" }
};

static bcmptm_ser_blk_map_t bcm56990_b0_blktype_info[] = {
    { BLKTYPE_EPIPE, BCMPTM_SER_BLK_EPIPE, { -1, -1 } },
    { BLKTYPE_IPIPE, BCMPTM_SER_BLK_IPIPE, { -1, -1 } },
    { BLKTYPE_PORT_IF, BCMPTM_SER_BLK_MMU, { -1, -1 } },
    { BLKTYPE_MMU_GLB, BCMPTM_SER_BLK_MMU, { BLKTYPE_MMU_EB, BLKTYPE_MMU_ITM } },
    { BLKTYPE_MMU_EB, BCMPTM_SER_BLK_MMU,  { -1, -1 } },
    { BLKTYPE_MMU_ITM, BCMPTM_SER_BLK_MMU, { -1, -1 } },
    { BLKTYPE_CDPORT, BCMPTM_SER_BLK_PORT, { -1, -1 } },
    { BLKTYPE_XLPORT, BCMPTM_SER_BLK_PORT, { -1, -1 } },
    { BLKTYPE_FLEX_CTR, BCMPTM_SER_BLK_IPIPE, { -1, -1 } }
};

static const bcmdrd_sid_t bcm56990_b0_idb_obm_mem_list[] = {
    IDB_OBM0_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM1_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM2_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM3_IOM_STATS_WINDOW_RESULTSm
};

/******************************************************************************
 * Private Functions
 */
static int
bcm56990_b0_ser_mem_clr_list_get(int unit,
                                 bcmptm_ser_mem_clr_list_t **mem_list,
                                 int *entry_num)
{
    *mem_list = bcm56990_b0_mem_clr_lst;
    *entry_num = COUNTOF(bcm56990_b0_mem_clr_lst);

    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_clear_mem_before_ser_enable(int unit,
                                            bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                            int *reg_num)
{
    *reg_ctrl_lst = bcm56990_b0_clear_mem_before_ser_enable_info;
    *reg_num = COUNTOF(bcm56990_b0_clear_mem_before_ser_enable_info);
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_clear_mem_after_ser_enable(int unit,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num)
{
    *reg_ctrl_lst = bcm56990_b0_clear_mem_after_ser_enable_info;
    *reg_num = COUNTOF(bcm56990_b0_clear_mem_after_ser_enable_info);

    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_tcam_hw_scan_ctrl_info_get(int unit, int enable,
                                           bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                           int *reg_num)
{
    if (enable) {
        *reg_ctrl_lst = bcm56990_b0_tcam_scan_ctrl_reg_lst_en;
        *reg_num = COUNTOF(bcm56990_b0_tcam_scan_ctrl_reg_lst_en);
    } else {
        *reg_ctrl_lst = bcm56990_b0_tcam_scan_ctrl_reg_lst_dis;
        *reg_num = COUNTOF(bcm56990_b0_tcam_scan_ctrl_reg_lst_dis);
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_mem_mute_list_get(int unit,
                                  bcmdrd_sid_t **mem_ser_mute_list,
                                  int *mem_ser_mute_num)
{
    *mem_ser_mute_list = bcm56990_b0_mem_ser_mute;
    *mem_ser_mute_num = COUNTOF(bcm56990_b0_mem_ser_mute);

    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_blk_ctrl_get(int unit,
                             bcmptm_ser_ctrl_en_t **ser_blks_ctrl_regs,
                             int *ctrl_item_num)
{
    *ser_blks_ctrl_regs = bcm56990_b0_ser_blocks_ctrl_regs;
    *ctrl_item_num = COUNTOF(bcm56990_b0_ser_blocks_ctrl_regs);

    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_intr_map_get(int unit,
                             int *cdmac_inst,
                             bcmptm_ser_intr_route_info_t **intr_map,
                             int *intr_map_num)
{
    *intr_map = bcm56990_b0_intr_map;
    *intr_map_num = COUNTOF(bcm56990_b0_intr_map);

    if (cdmac_inst != NULL) {
        *cdmac_inst = 4;
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_blk_type_map(int unit,
                             int blk_type, int *blk_type_overlay,
                             int *ser_blk_type)
{
    int count = COUNTOF(bcm56990_b0_blktype_info);
    int i = 0, index = -1;

    for (i = 0; i < count; i++) {
        if (blk_type == bcm56990_b0_blktype_info[i].blk_type) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        if (ser_blk_type != NULL) {
            *ser_blk_type = bcm56990_b0_blktype_info[index].ser_blk_type;
        }
        if (blk_type_overlay != NULL) {
            for (i = 0; i < BCMPTM_SER_OVERLAY_BLK_TYPE_NUM; i++) {
                blk_type_overlay[i] =
                    bcm56990_b0_blktype_info[index].blk_type_overlay[i];
            }
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

static void
bcm56990_b0_ser_lp_intr_enable(int unit, int sync, int cmicx_intr_num, int enable)
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
bcm56990_b0_ser_lp_intr_func_set(int unit,
                                 unsigned int cmic_intr_num,
                                 bcmbd_ser_intr_f intr_func,
                                 uint32_t intr_param)
{
    return bcmbd_cmicx_lp_intr_func_set(unit, cmic_intr_num,
                                        (bcmbd_intr_f)intr_func,
                                        intr_param);
}

static int
bcm56990_b0_ser_cmicx_adext_get(int unit, bcmptm_cmic_adext_info_t adext_info,
                                uint32_t *adext)
{
    BCMBD_CMICX_ADEXT_ACCTYPE_SET(*adext, adext_info.acc_type);
    BCMBD_CMICX_ADEXT_BLOCK_SET(*adext, adext_info.blk_num);
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_acc_type_get(int unit, bcmdrd_sid_t sid,
                             uint32_t *acc_type, const char **acctype_str)
{
    int rv = SHR_E_NONE;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t blkacc = 0, acctype_tmp = 0;
    int i = 0, acctype_num = COUNTOF(bcm56990_b0_acctype_info);
    bcmptm_ser_acctype_info_t *acctype_info = bcm56990_b0_acctype_info;

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
bcm56990_b0_ser_pt_inst_remap(int unit, bcmdrd_sid_t sid,
                              int *tbl_inst, int *tbl_copy)
{
    int inst_num, pipe_num;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    uint32_t blkacc = 0, acctype = 0, inst_v_bmp;
    bcmptm_fld_inst_map_t pipe_map_dup2[] = {
        {0, 0}, {1, 0}, {2, 3}, {3, 3}, {4, 4}, {5, 4}, {6, 7}, {7, 7}
    };
    bcmptm_fld_inst_map_t *map_ptr = NULL;
    int num_item = 0, i = 0, inst_mapped;
    int inst = *tbl_inst, copy = *tbl_copy;

    pipe_num = bcmdrd_pt_num_pipe_inst(unit, sid);
    inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    /* pipe_num is not zero only for PT belonged to IP/EP/MMU */
    if (pipe_num == 0) {
        if (inst_num > inst) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else if (pipe_num == -1){
        return FALSE;
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
    inst_v_bmp = (1 << inst_num) - 1;

    (void)bcmdrd_pt_info_get(unit, sid, sinfo);
    blkacc = sinfo->blktypes;
    acctype = BCMBD_CMICX_BLKACC2ACCTYPE(blkacc);

    if (acctype == ACC_TYPE_DUPLICATE_2) {
        inst_v_bmp = 0x99;
        num_item = COUNTOF(pipe_map_dup2);
        map_ptr = pipe_map_dup2;
    }
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
    return (inst_v_bmp & (1 << inst_mapped)) ? TRUE : FALSE;
}

static int
bcm56990_b0_ser_cmicx_schan_opcode_get(int unit, int cmd, int blk_num,
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
    uint32_t oc = 0;
    int i = 0, map_num = COUNTOF(cmd_map), acc_type;

    if (tbl_inst > 15) {
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

    if (acc_type == ACC_TYPE_UNIQUE) {
        if (tbl_inst >= 0) {
            acc_type = tbl_inst;
        } else {
            return SHR_E_PARAM;
        }
    }
    /*
     * For writing operation, use duplicate access type to write all instances.
     * For reading opreation, use unique access type to read appointed instance.
     * If use duplicate access type to read, only can read data from default instance.
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
bcm56990_b0_ser_hwmem_base_info_get(int unit, int membase, int stage_id,
                                    char **bd_bb_name, char **bb_name, bool *mem)
{
    int base_num = COUNTOF(bcm56990_b0_ser_hwmem_base_info), i = 0;
    bcmptm_ser_buffer_bus_info_t *hwmem_info = bcm56990_b0_ser_hwmem_base_info;

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
bcm56990_b0_ser_hw_info_remap(int unit, uint32_t address, int blk_type,
                              uint32_t membase, uint32_t instruction,
                              uint32_t *address_mapped, int *non_sbus)
{
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_mmu_phy2log_mem_remap(int unit, bcmptm_ser_correct_info_t *spci)
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
bcm56990_b0_ser_mmu_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                              uint32_t adext, int blk_inst)
{
    return bcm56990_b0_ser_mmu_phy2log_mem_remap(unit, spci);
}


static int
bcm56990_b0_ser_ip_ep_sram_remap(int unit, bcmdrd_sid_t sid, bcmdrd_sid_t *remap_sid)
{
    bcmdrd_sid_t mapping[][2] = {
        {CPU_COS_MAP_ONLYm, CPU_COS_MAPm},
        {EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, EXACT_MATCH_LOGICAL_TABLE_SELECTm},
        {IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, IFP_LOGICAL_TABLE_SELECTm},
        {L2_USER_ENTRY_ONLYm, L2_USER_ENTRYm},
        {L2_USER_ENTRY_DATA_ONLYm, L2_USER_ENTRYm},
        {MY_STATION_TCAM_ENTRY_ONLYm, MY_STATION_TCAMm},
        {MY_STATION_TCAM_DATA_ONLYm, MY_STATION_TCAMm}
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
bcm56990_b0_ser_data_split_mem_test_field_get(int unit, bcmdrd_sid_t sid,
                                              int acc_type, bcmdrd_fid_t *test_fid)
{
    uint32_t mem_test_fields[][3] = {
        /* mem with acc_type=data_split, has separate parity field for each pipe */
        { STG_TAB_Am, 0, SPLIT_DATA_P0f },
        { STG_TAB_Am, 0, SPLIT_DATA_P1f },
        { STG_TAB_Am, 1, SPLIT_DATA_P2f },
        { STG_TAB_Am, 1, SPLIT_DATA_P3f },
        { STG_TAB_Bm, 2, SPLIT_DATA_P4f },
        { STG_TAB_Bm, 2, SPLIT_DATA_P5f },
        { STG_TAB_Bm, 3, SPLIT_DATA_P6f },
        { STG_TAB_Bm, 3, SPLIT_DATA_P7f },
        { EGR_VFI_MEMBERSHIPm, 0, PORT_BITMAP_P0f },
        { EGR_VFI_MEMBERSHIPm, 0, PORT_BITMAP_P1f },
        { EGR_VFI_MEMBERSHIPm, 1, PORT_BITMAP_P2f },
        { EGR_VFI_MEMBERSHIPm, 1, PORT_BITMAP_P3f },
        { EGR_VFI_MEMBERSHIPm, 2, PORT_BITMAP_P4f },
        { EGR_VFI_MEMBERSHIPm, 2, PORT_BITMAP_P5f },
        { EGR_VFI_MEMBERSHIPm, 3, PORT_BITMAP_P6f },
        { EGR_VFI_MEMBERSHIPm, 3, PORT_BITMAP_P7f },
        { EGR_VFI_MEMBERSHIP_2m, 0, UT_PORT_BITMAP_P0f },
        { EGR_VFI_MEMBERSHIP_2m, 0, UT_PORT_BITMAP_P1f },
        { EGR_VFI_MEMBERSHIP_2m, 1, UT_PORT_BITMAP_P2f },
        { EGR_VFI_MEMBERSHIP_2m, 1, UT_PORT_BITMAP_P3f },
        { EGR_VFI_MEMBERSHIP_2m, 2, UT_PORT_BITMAP_P4f },
        { EGR_VFI_MEMBERSHIP_2m, 2, UT_PORT_BITMAP_P5f },
        { EGR_VFI_MEMBERSHIP_2m, 3, UT_PORT_BITMAP_P6f },
        { EGR_VFI_MEMBERSHIP_2m, 3, UT_PORT_BITMAP_P7f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 0, ING_PORT_BITMAP_P0f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 0, ING_PORT_BITMAP_P1f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 1, ING_PORT_BITMAP_P2f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 1, ING_PORT_BITMAP_P3f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 2, ING_PORT_BITMAP_P4f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 2, ING_PORT_BITMAP_P5f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 3, ING_PORT_BITMAP_P6f },
        { VFI_MEMBERSHIP_PROFILE_TABm, 3, ING_PORT_BITMAP_P7f },
    };
    int num = COUNTOF(mem_test_fields), i = 0;
    uint32_t acc_type_u = (acc_type < 0) ? 0 : acc_type;
    int sid_hit = 0;

    for (i = 0; i < num; i++) {
        if (mem_test_fields[i][0] == sid) {
            sid_hit = 1;
        }
        if (mem_test_fields[i][0] == sid &&
            mem_test_fields[i][1] == acc_type_u) {
            *test_fid = mem_test_fields[i][2];
            return SHR_E_NONE;
        }
    }
    if (sid_hit) {
        return SHR_E_PARAM;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

static int
bcm56990_b0_ser_alpm_bank_num_get(int unit)
{
    int index_count = 0;

    index_count = bcmptm_scor_pt_index_count(unit, L3_DEFIP_ALPM_LEVEL3m);

    if (index_count == 32768) {
        return 4;
    } else if (index_count == 65536) {
        return 8;
    } else {
        return 0;
    }
}

static int
bcm56990_b0_ser_em_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                            bcmptm_ser_sram_info_t *sram_info)
{
    uint32_t entries_num_per_bank = 0, stride = 0, ram_num = 0, j = 0;
    uint32_t base_index = 0, base = 0;
    bcmdrd_sid_t sid_corrected = INVALIDm;

    ram_num = 4;
    sid_corrected = EXACT_MATCH_ECCm;
    stride = 8192;
    /* stride is equal to entries_per_ram */
    entries_num_per_bank = stride * ram_num;
    base_index = index;
    base = (base_index / entries_num_per_bank) * entries_num_per_bank;
    base = base + base_index % stride;

    sram_info->ram_count = ram_num;
    sram_info->indexes[0][0] = base;
    sram_info->pt_view[0] = sid_corrected;
    sram_info->index_count[0] = 1;
    for (j = 1; j < sram_info->ram_count; j++) {
        sram_info->pt_view[j] = sid_corrected;
        sram_info->index_count[j] = 1;
        /* stride can be 1, also can be entries_per_ram */
        sram_info->indexes[j][0] = sram_info->indexes[j - 1][0] + stride;
    }
    sram_info->hw_recovery = FALSE;
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_alpm_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                              bcmptm_ser_sram_info_t *sram_info)
{
    int bank_num = 0;
    uint32_t bucket_mask, bit_shift;
    uint32_t base_bucket, bank, bank_mask, offset_shift, bkt_offset;
    uint32_t i = 0, ram_count = 4, bucket_num_per_ram = 2048;
    uint32_t offset_in_bucket = 0, offset_mask;

    bank_num = bcm56990_b0_ser_alpm_bank_num_get(unit);
    if (bank_num == 0) {
        return SHR_E_INTERNAL;
    }
    /* For ECC view, index format: bank[17:15] | bucket[14:2] | entry_in_bucket[1:0] */
    bit_shift = 15;
    offset_shift = 0x2;
    offset_mask = 0x3;
    bank_mask = (bank_num == 4) ? 0x3 : 0x7;
    bank_mask = bank_mask << bit_shift;

    bucket_mask = (1 << bit_shift) - 1;
    base_bucket = (index & bucket_mask) >> offset_shift;
    base_bucket = base_bucket % bucket_num_per_ram;
    offset_in_bucket = index & offset_mask;
    bank = index & bank_mask;
    sram_info->ram_count = ram_count;
    for (i = 0; i < ram_count; i++) {
        sram_info->pt_view[i] = sid;
        sram_info->index_count[i] = 1;
        bkt_offset = base_bucket + bucket_num_per_ram * i;
        sram_info->indexes[i][0] = bank | (bkt_offset << offset_shift) |
                                   offset_in_bucket;
    }
    sram_info->hw_recovery = FALSE;
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_tcam_remap(int unit, bcmdrd_sid_t tcam_sid,
                           bcmdrd_sid_t *tcam_only_sid, int *inject_err_to_key_fld)
{
    uint32_t mem_map_info[][2] = {
        /* DATA view has no SER protection */
        { CPU_COS_MAPm, CPU_COS_MAP_ONLYm },
        { CPU_COS_MAP_ONLYm, CPU_COS_MAP_ONLYm },
        { EXACT_MATCH_LOGICAL_TABLE_SELECTm, EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm },
        { EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm },
        { IFP_LOGICAL_TABLE_SELECTm, IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm },
        { IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm },
        /* DATA view has SER protection */
        { L2_USER_ENTRY_ONLYm, L2_USER_ENTRY_ONLYm },
        { L2_USER_ENTRYm, L2_USER_ENTRY_ONLYm },
        { MY_STATION_TCAM_ENTRY_ONLYm, MY_STATION_TCAM_ENTRY_ONLYm },
        { MY_STATION_TCAMm, MY_STATION_TCAM_ENTRY_ONLYm },
        { MY_STATION_2_TCAMm, MY_STATION_2_TCAM_ENTRY_ONLYm },
        { MY_STATION_2_TCAM_ENTRY_ONLYm, MY_STATION_2_TCAM_ENTRY_ONLYm },
        { L3_DEFIP_TCAM_LEVEL1m, L3_DEFIP_TCAM_LEVEL1m },
        { L3_DEFIP_TCAM_LEVEL1_WIDEm, L3_DEFIP_TCAM_LEVEL1_WIDEm },
        { L3_DEFIP_PAIR_LEVEL1m, L3_DEFIP_TCAM_LEVEL1m },
        { L3_DEFIP_PAIR_LEVEL1_WIDEm, L3_DEFIP_PAIR_LEVEL1_WIDEm },
        { L3_DEFIP_LEVEL1m, L3_DEFIP_TCAM_LEVEL1m },
        /* Only TCAM view */
        { L3_TUNNEL_PAIR_TCAMm, L3_TUNNEL_PAIR_TCAMm },
        { FP_UDF_TCAMm, FP_UDF_TCAMm},
        { VFP_TCAMm, VFP_TCAMm },
        { EFP_TCAMm, EFP_TCAMm },
        { IFP_TCAMm, IFP_TCAMm }
    };
    int num = COUNTOF(mem_map_info), i = 0;

    *tcam_only_sid = tcam_sid;
    *inject_err_to_key_fld = 0;
    for (i = 0; i < num; i++) {
        if (mem_map_info[i][0] == tcam_sid) {
            *tcam_only_sid = mem_map_info[i][1];
            if (tcam_sid == mem_map_info[i][1]) {
                *inject_err_to_key_fld = 1;
            } else {
                *inject_err_to_key_fld = 0;
            }
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_mem_ecc_info_get(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                                 bcmdrd_sid_t *sid, int *index, int *ecc_checking)
{
    uint32_t mem_map_info[][3] = {
        { L3_DEFIP_ALPM_LEVEL2_SINGLEm,  L3_DEFIP_ALPM_LEVEL2_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL2m,  L3_DEFIP_ALPM_LEVEL2_ECCm, 4 }, /* 1 : 4 */
        { L3_DEFIP_ALPM_LEVEL3_SINGLEm,  L3_DEFIP_ALPM_LEVEL3_ECCm, 1 }, /* 1 : 1 */
        { L3_DEFIP_ALPM_LEVEL3m,  L3_DEFIP_ALPM_LEVEL3_ECCm, 4 }, /* 1 : 4 */
        { EGR_ADAPT_SINGLEm, EGR_ADAPT_ECCm }, /* 1 : 1 */
        { EXACT_MATCH_2m, EXACT_MATCH_ECCm, 2 }, /* 1 : 2 */
        { EXACT_MATCH_4m, EXACT_MATCH_ECCm, 4 }, /* 1 : 4 */
        { L2_ENTRY_SINGLEm, L2_ENTRY_ECCm, 1 }, /* 1 : 1 */
        { L3_TUNNEL_SINGLEm, L3_TUNNEL_ECCm, 1 }, /* 1 : 1 */
        { L3_TUNNEL_DOUBLEm, L3_TUNNEL_ECCm, 2 }, /* 1 : 2 */
        { L3_TUNNEL_QUADm, L3_TUNNEL_ECCm, 4 }, /* 1 : 4 */
        { MPLS_ENTRY_SINGLEm, MPLS_ENTRY_ECCm, 1 }, /* 1 : 1 */
        { MPLS_ENTRY_DOUBLEm, MPLS_ENTRY_ECCm, 2 }, /* 1 : 2 */
        { L3_DEFIP_LEVEL1m, L3_DEFIP_LEVEL1m, 1 },
        { L3_DEFIP_LEVEL1_WIDEm, L3_DEFIP_LEVEL1_WIDEm, 1 },
        { L3_DEFIP_PAIR_LEVEL1m, L3_DEFIP_PAIR_LEVEL1m, 1 },
        { L3_DEFIP_PAIR_LEVEL1_WIDEm, L3_DEFIP_PAIR_LEVEL1_WIDEm, 1 },
        { L3_DEFIP_TCAM_LEVEL1m, L3_DEFIP_TCAM_LEVEL1m, 1 },
        { L3_DEFIP_TCAM_LEVEL1_WIDEm, L3_DEFIP_TCAM_LEVEL1_WIDEm, 1 },
        { L3_TUNNEL_PAIR_TCAMm, L3_TUNNEL_PAIR_TCAMm, 1 },
        { L3_TUNNEL_TCAMm, L3_TUNNEL_TCAMm, 1 },
        { L3_TUNNEL_TCAM_ONLYm, L3_TUNNEL_TCAM_ONLYm, 1 },
        { MY_STATION_TCAMm, MY_STATION_TCAMm, 1 },
        { MY_STATION_TCAM_ENTRY_ONLYm, MY_STATION_TCAM_ENTRY_ONLYm, 1 },
        { MY_STATION_2_TCAMm, MY_STATION_2_TCAMm, 1 },
        { MY_STATION_2_TCAM_ENTRY_ONLYm, MY_STATION_2_TCAM_ENTRY_ONLYm, 1 },
        { CPU_COS_MAPm, CPU_COS_MAPm, 1 },
        { CPU_COS_MAP_ONLYm, CPU_COS_MAP_ONLYm, 1 },
        { EFP_TCAMm, EFP_TCAMm, 1 },
        { EXACT_MATCH_LOGICAL_TABLE_SELECTm, EXACT_MATCH_LOGICAL_TABLE_SELECTm, 1 },
        { EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm, 1 },
        { FP_UDF_TCAMm, FP_UDF_TCAMm, 1 },
        { IFP_LOGICAL_TABLE_SELECTm, IFP_LOGICAL_TABLE_SELECTm, 1 },
        { IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm, 1 },
        { IFP_TCAMm, IFP_TCAMm, 1 },
        { L2_USER_ENTRYm, L2_USER_ENTRYm, 1 },
        { L2_USER_ENTRY_ONLYm, L2_USER_ENTRY_ONLYm, 1 },
        { VFP_TCAMm, VFP_TCAMm, 1 },
    };
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

    if (ecc_protection == 1 && sid != NULL && index != NULL) {
        *sid = mem_map_info[i][1];
        *index = index_fv * mem_map_info[i][2];
    }

    if (ecc_checking == NULL) {
        return SHR_E_NONE;
    }
    /* AGGR view uses SER information of TCAM_ONLY */
    (void)bcm56990_b0_ser_tcam_remap(unit, sid_fv, &sid_tcam_only,
                                     &inject_err_to_key_fld);
    if (ecc_protection == 0) {
        rv = bcmdrd_pt_ser_en_ctrl_get(unit, BCMDRD_SER_EN_TYPE_ECC1B,
                                       sid_tcam_only, &ctrl_info);
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
bcm56990_b0_ser_refresh_regs_info_get(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                      uint32_t *info_num)
{
    *mem_refresh_info = bcm56990_b0_clear_refresh_registers;
    *info_num = COUNTOF(bcm56990_b0_clear_refresh_registers);

    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_ecc_status_translate(int unit,
                                     bcmptm_ser_correct_info_t *spci,
                                     const bcmdrd_sid_t **corrected_sids,
                                     uint32_t *index,
                                     uint32_t *sid_num)
{
    bcmdrd_sid_t idb_oam_status_regs[] = {
        IDB_OBM0_OVERSUB_MON_ECC_STATUSr, IDB_OBM1_OVERSUB_MON_ECC_STATUSr,
        IDB_OBM2_OVERSUB_MON_ECC_STATUSr, IDB_OBM3_OVERSUB_MON_ECC_STATUSr
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
        *corrected_sids = &bcm56990_b0_idb_obm_mem_list[idx];
        *index = spci->index[0];
        *sid_num = 1;
    }
    return SHR_E_NONE;
}


static int
bcm56990_b0_ser_mem_xor_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                 bcmptm_ser_sram_info_t *sram_info)
{
    int rv = SHR_E_UNAVAIL;

    if (EXACT_MATCH_ECCm == sid) {
        rv = bcm56990_b0_ser_em_info_get(unit, sid, index, sram_info);
    } else if (L3_DEFIP_ALPM_LEVEL3_ECCm == sid) {
        rv = bcm56990_b0_ser_alpm_info_get(unit, sid, index, sram_info);
    } else {
        rv = SHR_E_UNAVAIL;
    }
    return rv;
}

static int
bcm56990_b0_ser_sram_ctrl_register_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                                       bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info)
{
   /*
     * If there is SER error in XOR bank, and force register is enabled,
     * when we read corresponding table bank, the XOR SER error can be triggered.
     */
    uint32_t force_reg_map[][3] = {
        { EXACT_MATCH_ECCm, UFT_SHARED_BANKS_CONTROLm, FORCE_XOR_GENf},
        { L3_DEFIP_ALPM_LEVEL3_ECCm, UFT_SHARED_BANKS_CONTROLm, FORCE_XOR_GENf}
    };
    /* Disables XOR bank write. Allows SW to inject parity error on Table Banks or XOR Bank. */
    uint32_t xor_write_reg_map[][3] = {
        { EXACT_MATCH_ECCm, UFT_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf},
        { L3_DEFIP_ALPM_LEVEL3_ECCm, UFT_SHARED_BANKS_CONTROLm, DISABLE_XOR_WRITEf}
    };
    uint32_t i = 0, map_num, j;
    bcmdrd_sid_t sid_ecc = INVALIDm;
    int index_ecc = index;

    /* initialize */
    sram_ctrl_reg_info->force_field = INVALIDf;
    sram_ctrl_reg_info->force_reg = INVALIDr;
    sram_ctrl_reg_info->wr_check_reg = INVALIDr;
    sram_ctrl_reg_info->wr_check_field = INVALIDf;
    sram_ctrl_reg_info->xor_wr_reg = INVALIDr;
    sram_ctrl_reg_info->xor_wr_field = INVALIDf;

    for (j = 0; j < BCMPTM_SER_EXTRA_REG_NUM; j++) {
        sram_ctrl_reg_info->extra_reg[j] = INVALIDr;
        sram_ctrl_reg_info->extra_field[j] = INVALIDf;
    }
    (void)bcm56990_b0_ser_mem_ecc_info_get(unit, sid, index, &sid_ecc, &index_ecc, NULL);
    map_num = COUNTOF(force_reg_map);
    for (i = 0; i < map_num; i++) {
        if (sid_ecc == force_reg_map[i][0]) {
            sram_ctrl_reg_info->force_reg = force_reg_map[i][1];
            sram_ctrl_reg_info->force_field = force_reg_map[i][2];
            break;
        }
    }
    map_num = COUNTOF(xor_write_reg_map);
    for (i = 0; i < map_num; i++) {
        if (sid_ecc == xor_write_reg_map[i][0]) {
            sram_ctrl_reg_info->xor_wr_reg = xor_write_reg_map[i][1];
            sram_ctrl_reg_info->xor_wr_field = xor_write_reg_map[i][2];
            break;
        }
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_sram_scan_mem_skip(int unit, bcmdrd_sid_t sid, bool *skip)
{
    bcmdrd_sid_t sid_skip_list[] = {
        INVALIDm
        /*
         * If there is SER protection for DATA_ONLY view, the DATA_ONLY view will be scanned.
         * If there is no SER protection for DATA_ONLY, BCMPTM_SER_PT_CONTROL_SCAN_MODE_OPER
         * returns 0, so the DATA_ONLY view will not be scanned.
         * All TCAM_ONLY view and AGGR view is not scanned by SRAM memory scan thread.
         * Because bcmdrd_pt_attr_is_cam(unit, sid) return TRUE for TCAM_ONLY view
         * and AGGR view.
        */
    };
    int i = 0, sid_num = COUNTOF(sid_skip_list);
    int index_ecc = 0;
    bcmdrd_sid_t sid_ecc = sid;

    (void)bcm56990_b0_ser_mem_ecc_info_get(unit, sid, 0, &sid_ecc,
                                           &index_ecc, NULL);
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
bcm56990_b0_ser_copy_to_cpu_regs_info_get(int unit,
                                         bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                         int *reg_num)
{
    *reg_ctrl_lst = bcm56990_b0_ser_ctrl_info;
    *reg_num = COUNTOF(bcm56990_b0_ser_ctrl_info);
    return SHR_E_NONE;
}

static int
bcm56990_b0_ser_expected_value_get(int unit, bcmdrd_sid_t sid,
                                   bcmdrd_fid_t fid, uint32_t *value)
{
    uint32_t special_pt[][2] = {
        { CDPORT_INTR_MASKr, CDMAC_INTRf}
    };
    int num = COUNTOF(special_pt), i = 0;
    int min_bit, max_bit;

    for (i = 0; i < num; i++) {
        if (sid == special_pt[i][0] && fid == special_pt[i][1]) {
            *value = 0;
            break;
        }
    }

    if (i == num) {
        min_bit = bcmdrd_pt_field_minbit(unit, sid, fid);
        max_bit = bcmdrd_pt_field_maxbit(unit, sid, fid);
        *value = (1 << (max_bit - min_bit + 1)) - 1;
    }

    return SHR_E_NONE;

}

static int
bcm56990_b0_num_bit_interleave_get(int unit, bcmdrd_sid_t sid,
                               bcmdrd_fid_t fid, uint32_t *num_interleave)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56990_b0_ser_status_field_parse(int unit, bcmdrd_sid_t sid,
                                   int fid_count, int *fval)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56990_b0_ser_force_error_value_get(int unit, bcmdrd_sid_t sid,
                                      bcmdrd_fid_t fid, uint32_t *value)
{
    *value = 1;

    return SHR_E_NONE;
}

bcmdrd_sid_t bcm56990_b0_tcam_ctrl_lst[] = {
    CPU_COS_MAP_CAM_CONTROLr,
    L2_USER_ENTRY_CAM_DBGCTRLr,
    L3_TUNNEL_CAM_CONTROLr,
    MY_STATION_2_CAM_CONTROLr,
    MY_STATION_CAM_CONTROLr,
    UDF_CAM_CONTROLr,
    VFP_CAM_CONTROLr,
    EFP_CAM_CONTROLr,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr,
    IFP_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr,
    IFP_CAM_DBGCTRLr,
    L3_DEFIP_CAM_DBGCTRLr
};

static int
bcm56990_b0_ser_tcam_ctrl_reg_list_get(int unit, bcmdrd_sid_t **mem_list,
                                       int *entry_num, bcmdrd_fid_t *fid)
{
    *mem_list = bcm56990_b0_tcam_ctrl_lst;
    *entry_num = COUNTOF(bcm56990_b0_tcam_ctrl_lst);
    *fid = AUTO_CORRECT_1BIT_ECC_ENf;

    return SHR_E_NONE;
}

static const bcmptm_ser_data_driver_t bcm56990_b0_data_driver = {

    .mem_clr_list_get = bcm56990_b0_ser_mem_clr_list_get,

    .clear_mem_after_ser_enable = bcm56990_b0_ser_clear_mem_after_ser_enable,

    .clear_mem_before_ser_enable = bcm56990_b0_ser_clear_mem_before_ser_enable,

    .tcam_ser_hw_scan_reg_info_get = bcm56990_b0_ser_tcam_hw_scan_ctrl_info_get,

    .acc_type_map_info_get = NULL,

    .tcam_cmic_ser_info_get = NULL,

    .mute_mem_list_get = bcm56990_b0_ser_mem_mute_list_get,

    .blk_ctrl_regs_get = bcm56990_b0_ser_blk_ctrl_get,

    .intr_map_get = bcm56990_b0_ser_intr_map_get,

    .blk_type_map = bcm56990_b0_ser_blk_type_map,

    .ser_intr_enable = bcm56990_b0_ser_lp_intr_enable,

    .intr_cb_func_set = bcm56990_b0_ser_lp_intr_func_set,

    .cmic_ser_engine_regs_get = NULL,

    .cmic_ser_engine_enable = NULL,

    .cmic_ser_result_get = NULL,

    .cmic_adext_get = bcm56990_b0_ser_cmicx_adext_get,

    .pt_acctype_get = bcm56990_b0_ser_acc_type_get,

    .pt_inst_remap = bcm56990_b0_ser_pt_inst_remap,

    .cmic_schan_opcode_get = bcm56990_b0_ser_cmicx_schan_opcode_get,

    .hwmem_base_info_get = bcm56990_b0_ser_hwmem_base_info_get,

    .ip_ep_hw_info_remap = bcm56990_b0_ser_hw_info_remap,

    .mmu_mem_remap = bcm56990_b0_ser_mmu_mem_remap,

    .ip_ep_mem_remap = bcm56990_b0_ser_ip_ep_sram_remap,

    .data_split_mem_test_fid_get = bcm56990_b0_ser_data_split_mem_test_field_get,

    .mem_ecc_info_get = bcm56990_b0_ser_mem_ecc_info_get,

    .tcam_remap = bcm56990_b0_ser_tcam_remap,

    .refresh_regs_info_get = bcm56990_b0_ser_refresh_regs_info_get,

    .ser_status_reg_translate = bcm56990_b0_ser_ecc_status_translate,

    .sram_ctrl_reg_get = bcm56990_b0_ser_sram_ctrl_register_get,

    .xor_lp_mem_info_get = bcm56990_b0_ser_mem_xor_info_get,

    .sram_scan_mem_skip = bcm56990_b0_ser_sram_scan_mem_skip,

    .copy_to_cpu_regs_info_get = bcm56990_b0_ser_copy_to_cpu_regs_info_get,

    .expected_value_get = bcm56990_b0_ser_expected_value_get,

    .num_bit_interleave_get = bcm56990_b0_num_bit_interleave_get,

    .ser_status_field_parse = bcm56990_b0_ser_status_field_parse,

    .force_error_value_get = bcm56990_b0_ser_force_error_value_get,

    .tcam_ctrl_reg_list_get = bcm56990_b0_ser_tcam_ctrl_reg_list_get
};

/******************************************************************************
 * Public Functions
 */
const bcmptm_ser_data_driver_t *
bcm56990_b0_ptm_ser_data_driver_register(int unit)
{
    return &bcm56990_b0_data_driver;
}

