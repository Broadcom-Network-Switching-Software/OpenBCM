/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      ser.c
 * Purpose:   SER enable/detect and test functionality.
 * Requires:  sal/soc/shared layer
 */


#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/iproc.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif

#ifdef BCM_HURRICANE4_SUPPORT

#include <soc/soc_ser_log.h>
#include <soc/hurricane4.h>
#if defined(INCLUDE_XFLOW_MACSEC)
#include <bcm/xflow_macsec.h>
#include <esw/xflow_macsec_esw_defs.h>
#endif

#define _SOC_MMU_BASE_INDEX_GLB 0
#define _SOC_MMU_BASE_INDEX_XPE 0
#define _SOC_MMU_BASE_INDEX_SC  0
#define _SOC_MMU_BASE_INDEX_SED 0

#define _SOC_HR4_IPIPE0_CEV_INTR  0x00000001
#define _SOC_HR4_IPIPE0_IDB_INTR  0x00000040
#define _SOC_HR4_IPIPE0_IP_INTR   0x00000080
#define _SOC_HR4_IPIPE0_ISW4_INTR 0x00000100

#if defined(BCM_56275_A0)
extern _soc_bus_ser_en_info_t _soc_bcm56275_a0_ip_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56275_a0_ip_buffer_ser_info[];
extern _soc_bus_ser_en_info_t _soc_bcm56275_a0_ep_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56275_a0_ep_buffer_ser_info[];
#endif

typedef struct _soc_hr4_ser_mmu_intr_info_s {
    soc_reg_t   int_statf;
    int         mmu_base_index;
    int         ser_info_index;
} _soc_hr4_ser_mmu_intr_info_t;

typedef _soc_td3_ser_info_type_t _soc_hr4_ser_info_type_t;

typedef struct _soc_hr4_ser_route_block_s {
    uint8               cmic_reg; /* 3: intr3, 4: intr4, 5: intr5 */
    uint32              cmic_bit;
    soc_block_t         blocktype;
    int                 pipe;
    soc_reg_t           enable_reg;
    soc_reg_t           status_reg;
    soc_field_t         enable_field;
    void                *info;
    uint8               id;
} _soc_hr4_ser_route_block_t;

typedef struct _soc_hr4_tcam_wrapper_info_s {
    soc_mem_t   mem;
    soc_reg_t   enable_reg;
    soc_field_t enable_field;
    soc_field_t parity_field; /* used to sw inject test */
} _soc_hr4_tcam_wrapper_info_t;

typedef _soc_td3_ser_reg_t _soc_hr4_ser_reg_t;

typedef _soc_td3_ser_info_t _soc_hr4_ser_info_t;

typedef struct _soc_hr4_tmp_ser_info_s {
    soc_reg_t           enable_reg;
    soc_field_t         enable_field;
    uint32              value;
} _soc_hr4_tmp_ser_info_t;

typedef struct _soc_hr4_hash_table_ser_info_s {
    soc_mem_t       mem;
    soc_field_t     enable_field;
} _soc_hr4_hash_table_ser_info_t;

static soc_ser_functions_t _hr4_ser_functions;

static soc_field_t _soc_hr4_mmu_xpe_enable_fields[] = {
    AGING_PARITY_ENf,
    THDM_PARITY_ENf,
    ENQX_PARITY_ENf,
    EPRG_PARITY_ENf,
    RQE_PARITY_ENf,
    THDI_PARITY_ENf,
    THDU_PARITY_ENf,
    PQE_PARITY_ENf,
    WRED_PARITY_ENf,
    DQX_PARITY_ENf,
    CCP_PARITY_ENf,
    CTR_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_hr4_mmu_sc_enable_fields[] = {
    TDM_PARITY_ENf,
    VBS_PARITY_ENf,
    TOQ_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_hr4_mmu_sed_enable_fields[] = {
    INTFI_PARITY_ENf,
    MB0_PARITY_ENf,
    /*MB1_PARITY_ENf,*/
    ENQS_PARITY_ENf,
    MTRO_PARITY_ENf,
    DQS_PARITY_ENf,
    CFAP_PARITY_ENf,
    INVALIDf
};

static _soc_hr4_ser_info_t _soc_hr4_mmu_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_MMU_GLB, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU GLB MEM PAR",
        MMU_GCFG_PARITY_ENr, INTFO_PARITY_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_GCFG_GLB_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_MMU_XPE, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU XPE MEM PAR",
        MMU_XCFG_PARITY_ENr, INVALIDf, _soc_hr4_mmu_xpe_enable_fields,
        MMU_XCFG_XPE_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_XCFG_XPE_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_XCFG_XPE_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_MMU_SC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SC MEM PAR",
        MMU_SCFG_PARITY_ENr, INVALIDf, _soc_hr4_mmu_sc_enable_fields,
        MMU_SCFG_SC_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_SCFG_SC_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_SCFG_SC_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_MMU_SED, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SED MEM PAR",
        MMU_SEDCFG_PARITY_ENr, INVALIDf, _soc_hr4_mmu_sed_enable_fields,
        MMU_SEDCFG_SED_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_SEDCFG_SED_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_SEDCFG_SED_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static soc_field_t _soc_hr4_mmu_top_int_enable_fields[] = {
    SED_CPU_INT_ENf,
    SC_CPU_INT_ENf,
    XPE_CPU_INT_ENf,
    GLB_CPU_INT_ENf,
    INVALIDf
};

static soc_field_t _soc_hr4_mmu_top_int_status_fields[] = {
    SED_CPU_INT_STATf,
    SC_CPU_INT_STATf,
    XPE_CPU_INT_STATf,
    GLB_CPU_INT_STATf,
    INVALIDf
};

static _soc_hr4_ser_info_t _soc_hr4_mmu_top_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_MMU_SER, _soc_hr4_mmu_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GCFG_ALL_CPU_INT_ENr, INVALIDf,
        _soc_hr4_mmu_top_int_enable_fields,
        MMU_GCFG_ALL_CPU_INT_STATr,
        NULL, INVALIDf, _soc_hr4_mmu_top_int_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_hr4_ser_reg_t _soc_hr4_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC" },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_hr4_ser_reg_t _soc_hr4_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC" },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_hr4_ser_reg_t _soc_hr4_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_hr4_ser_reg_t _soc_hr4_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_hr4_ser_info_t _soc_hr4_pm_clp_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC single bit",
        CLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC double bits",
        CLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC single bit",
        CLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC double bits",
        CLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_hr4_pm_clp_rx_status_reg, INVALIDf, NULL,
        CLPORT_INTR_STATUSr, MIB_RX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_hr4_pm_clp_tx_status_reg, INVALIDf, NULL,
        CLPORT_INTR_STATUSr, MIB_TX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_hr4_ser_info_t _soc_hr4_pm_xlp_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC TX CDC memory",
        XLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_hr4_pm_xlp_rx_status_reg, INVALIDf, NULL,
        XLPORT_INTR_STATUSr, MIB_RX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_hr4_pm_xlp_tx_status_reg, INVALIDf, NULL,
        XLPORT_INTR_STATUSr, MIB_TX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_hr4_ser_reg_t _soc_hr4_idb_status_reg[] = {
    { IDB_CA_PM0_Q_CA_ECC_STATUSr, "IDB PM0_Q CA packet buffer memory" },
    { IDB_CA_PM1_Q_CA_ECC_STATUSr, "IDB PM1_Q CA packet buffer memory" },
    { IDB_CA_PM2_Q_CA_ECC_STATUSr, "IDB PM2_Q CA packet buffer memory" },
    { IDB_CA_PM3_Q_CA_ECC_STATUSr, "IDB PM3_Q CA packet buffer memory" },
    { IDB_CA_PM4_Q_CA_ECC_STATUSr, "IDB PM4_Q CA packet buffer memory" },
    { IDB_CA_PM5_Q_CA_ECC_STATUSr, "IDB PM5_Q CA packet buffer memory" },
    { IDB_CA_PM6_Q_CA_ECC_STATUSr, "IDB PM6_Q CA packet buffer memory" },
    { IDB_CA_PM7_Q_CA_ECC_STATUSr, "IDB PM7_Q CA packet buffer memory" },
    { IDB_CA_PM0_48_CA_ECC_STATUSr, "IDB PM0 CA packet buffer memory" },
    { IDB_CA_PM1_48_CA_ECC_STATUSr, "IDB PM1 CA packet buffer memory" },
    { IDB_CA_PM2_48_CA_ECC_STATUSr, "IDB PM2 CA packet buffer memory" },
    { IDB_CA_CPU_ECC_STATUSr, "CA-CPU packet buffer memory" },
    { IDB_CA_LPBK_ECC_STATUSr, "CA-LPBK packet buffer memory" },
    { IDB_CA_BSK_ECC_STATUSr, "CA-BSK packet buffer memory" },
    { IDB_CA_MGMT_ECC_STATUSr, "CA-MGMT packet buffer memory" },
    { IDB_SOP_EVENT_ECC_STATUSr, "IDB SOP Event Fifo ECC Status" },
    { IDB_EOP_EVENT_ECC_STATUSr, "IDB EOP Event Fifo ECC Status" },
    { INVALIDr }
};

static _soc_hr4_ser_info_t _soc_hr4_idb_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        IS_TDM_CALENDAR0m, "IS_TDM CALENDAR0/1",
        IDB_SER_CONTROLr, IS_TDM_ECC_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_IS_TDM_CAL_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_hr4_idb_status_reg, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_hr4_ser_info_t _soc_hr4_isw4_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        SLOT_PIPELINE_ECC_ERRf, SLOT_PIPELINE_ECC_ERRf,
        INVALIDm, "Slot pipeline",
        SW4_SER_CONTROLr, SLOT_PIPELINE_ECC_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        SLOT_PIPELINE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        CELL_QUEUE_ECC_ERRf, CELL_QUEUE_ECC_ERRf,
        INVALIDm, "Cell queue",
        SW4_SER_CONTROLr, CELL_QUEUE_ECC_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        CELL_QUEUE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        EVENT_FIFO_ECC_ERRf, EVENT_FIFO_ECC_ERRf,
        INVALIDm, "Event FIFO",
        SW4_SER_CONTROLr, EVENT_FIFO_ECC_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        EVENT_FIFO_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_TD3_PARITY_TYPE_NONE} /* table terminator */
};

static _soc_hr4_ser_info_t _soc_hr4_cev_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        FIFO_PARITY_ERRORf, FIFO_PARITY_ERRORf,
        CENTRAL_CTR_EVICTION_FIFOm, "CENTRAL CTR_EVICTION FIFO",
        CENTRAL_CTR_EVICTION_FIFO_PARITY_CONTROLr, PARITY_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        CENTRAL_CTR_EVICTION_INTR_STATUSr, NULL, FIFO_PARITY_ERRORf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_TD3_PARITY_TYPE_NONE} /* table terminator */
};
#ifdef INCLUDE_XFLOW_MACSEC
STATIC _soc_hr4_ser_info_t _soc_hr4_macsec_ser_info[] = {

    { _SOC_TD3_PARITY_TYPE_MACSEC_SER, NULL, 0,
        ECCERR_INT_ENf, INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_TD3_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        EGRESS_SA_SOFT_EXPIRE_INT_ENf, INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_TD3_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        EGRESS_SA_EXPIRE_INT_ENf, INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_TD3_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        INGRESS_SA_SOFT_EXPIRE_INT_ENf, INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_TD3_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        INGRESS_SA_EXPIRE_INT_ENf, INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_TD3_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        ISEC_INT_ENf, INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};
/* MACSEC packet buffer SER control data */
_soc_buffer_ser_en_info_t _soc_hr4_macsec_pkt_buffer_ser_info[] = {
    { "ECC_CTLSTS_EPKTRAM", ECC_CTLSTS_EPKTRAMr, P_MODE_DISABLE_ECCf, INVALIDr, INVALIDf },
    { "", INVALIDr } /* end */
};
#endif
static const
_soc_hr4_ser_route_block_t  _soc_hr4_ser_route_blocks[] = {
    { 0, _SOC_HR4_IPIPE0_CEV_INTR, /* CEV_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 0, CENTRAL_CTR_EVICTION_INTR_ENABLEr,
      CENTRAL_CTR_EVICTION_INTR_STATUSr, INVALIDf,
      _soc_hr4_cev_ser_info, 0 },
    { 0, 0x00000002, /* L2_MGMT_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 0, L2_MGMT_INTR_MASKr, L2_MGMT_INTRr, SER_FIFO_NOT_EMPTYf,
      NULL, 0 },
    { 0, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTR */
      SOC_BLK_MMU, 0, MMU_GCFG_MISCCONFIGr, MMU_GCFG_MEM_SER_FIFO_STSr, PARITY_ENf,
      _soc_hr4_mmu_top_ser_info, 0 },
    { 0, 0x00000020, /* PIPE0_EP_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, EGR_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, _SOC_HR4_IPIPE0_IDB_INTR, /* PIPE0_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_hr4_idb_ser_info, 0 },
    { 0, _SOC_HR4_IPIPE0_IP_INTR, /* PIPE0_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 0, _SOC_HR4_IPIPE0_ISW4_INTR, /* PIPE0_ISW4_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, ISW4_INTR_ENABLEr, ISW4_INTR_STATUSr, INVALIDf,
      _soc_hr4_isw4_ser_info, 0 },
    { 1, 0x00000002, /* PM0_INTR */
      SOC_BLK_XLPORT, 0, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_hr4_pm_xlp_ser_info, 0 },
    { 1, 0x00000004, /* PM1_INTR */
      SOC_BLK_XLPORT, 0, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_hr4_pm_xlp_ser_info, 1 },
    { 1, 0x00000002, /* PM0_INTR */
      SOC_BLK_PMQPORT, 0, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 1, 0x00000004, /* PM1_INTR */
      SOC_BLK_PMQPORT, 0, INVALIDr, INVALIDr, INVALIDf, NULL, 1 },
    { 1, 0x00000008, /* PM2_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_hr4_pm_clp_ser_info, 0 },
    { 1, 0x00000010, /* PM3_INTR */
      SOC_BLK_XLPORT, 0, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_hr4_pm_xlp_ser_info, 2 },
#ifdef INCLUDE_XFLOW_MACSEC
    { 1, 0x00000040, /* MACSEC_INTR */
        SOC_BLK_MACSEC, 0, SP_GLB_INTR_ENr, SP_GLB_INTR_STATUSr, INVALIDf,
        _soc_hr4_macsec_ser_info, 0 },
#endif
#if 0
    { 1, 0x00000400, /* PM_IPROC_INTR */
      SOC_BLK_XLPORT, 0, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_hr4_pm_xlp_ser_info, 7 },
#endif
    { 0 } /* table terminator */
};

static _soc_generic_ser_info_t _soc_hr4_tcam_ser_info[] =
{
    { BSK_FTFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    /* Global view */
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_PKT_FLOW_SELECT_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_QOS_CTRL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ZONE_0_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ZONE_1_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ZONE_2_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ZONE_3_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { EGR_ZONE_4_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { FLEX_RTAG7_HASH_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { FORWARDING_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { FORWARDING_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { FORWARDING_3_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER0_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_3m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_4m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_MICE_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER1_MICE_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_HME_STAGE_TCAM_ONLY_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_HME_STAGE_TCAM_ONLY_4m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_MICE_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { IP_PARSER2_MICE_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_REMAP_READ |
      _SOC_SER_FLAG_SIZE_VERIFY | _SOC_SER_FLAG_XY_READ},

    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_REMAP_READ |
      _SOC_SER_FLAG_SIZE_VERIFY | _SOC_SER_FLAG_XY_READ},

    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { MY_STATION_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { PHB_SELECT_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { PKT_FLOW_SELECT_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { PKT_FLOW_SELECT_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { PKT_FLOW_SELECT_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    /* Global view */
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},

    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_XY_READ},
#ifdef INCLUDE_XFLOW_MACSEC
    { ISEC_SP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      -1, -1,
      { {0, 239}, {1, 239}, {240, 480}, {241, 480} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SW_COMPARE
      },
    { ISEC_SC_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      -1, -1,
      { {0, 75}, {2, 75}, {76,153 }, {78, 153} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SW_COMPARE
      },
#endif

    /* End of list */
    { INVALIDm },
};

STATIC soc_error_t
_soc_hr4_ser_tcam_wrapper_enable(int unit, int enable)
{
    return soc_td3_ser_tcam_wrapper_enable(unit, enable);
}

STATIC int
_soc_hr4_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                               soc_reg_t group_reg, uint64 *group_rval,
                               _soc_hr4_ser_info_t *info_list,
                               soc_mem_t mem, int enable)
{
    return soc_td3_ser_enable_info(unit, block_info_idx, inst, port,
                                   group_reg, group_rval,
                                   info_list, mem, enable);
}

STATIC int
_soc_hr4_ser_block_enable(int unit, int enable, int type,
                          soc_reg_t fifo_reset_reg, void *info)
{
    uint16                          pcount;
    soc_reg_t                       reg, ecc1b_reg;
    soc_field_t                     field, ecc1b_field;
    soc_mem_t                       en_mem;
    soc_mem_t                       ecc1b_en_mem;
    int                             field_position = -1;
    int                             ecc1b_field_position = -1;
    uint8                           flag = 0;
    uint8                           ecc1b_flag = 0;
    _soc_reg_ser_en_info_t          *reg_info;
    _soc_mem_ser_en_info_t          *mem_info;
    _soc_bus_ser_en_info_t          *bus_info;
    _soc_buffer_ser_en_info_t       *buf_info;
    char                            *str_type;
    char                            *str_name;
    static char *parity_module_str[4] = {"REG", "MEM", "BUS", "BUF"};
    int done = 0;

    reg_info = (_soc_reg_ser_en_info_t*)info;
    mem_info = (_soc_mem_ser_en_info_t*)info;
    bus_info = (_soc_bus_ser_en_info_t*)info;
    buf_info = (_soc_buffer_ser_en_info_t*)info;
    for (pcount = 0;;pcount++) { /* walk thro each entry of block_info */
        reg = INVALIDr;
        ecc1b_reg = INVALIDr;
        ecc1b_en_mem = INVALIDm;
        en_mem = INVALIDm;
        field = INVALIDf;
        ecc1b_field = INVALIDf;
        switch (type) {
        case _SOC_SER_TYPE_REG:
            if (reg_info[pcount].reg == INVALIDr) {
                done = 1;
            } else {
                reg = reg_info[pcount].en_ctrl.ctrl_type.en_reg;
                en_mem = reg_info[pcount].en_ctrl.ctrl_type.en_mem;
                flag = reg_info[pcount].en_ctrl.flag_reg_mem;
                field = reg_info[pcount].en_ctrl.en_fld;
                field_position = reg_info[pcount].en_ctrl.en_fld_position;
                ecc1b_reg = reg_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
                ecc1b_en_mem = reg_info[pcount].ecc1b_ctrl.ctrl_type.en_mem;
                ecc1b_flag = reg_info[pcount].ecc1b_ctrl.flag_reg_mem;
                ecc1b_field = reg_info[pcount].ecc1b_ctrl.en_fld;
                ecc1b_field_position = reg_info[pcount].ecc1b_ctrl.en_fld_position;
                str_name = SOC_REG_NAME(unit, reg_info[pcount].reg);
            }
            break;
        case _SOC_SER_TYPE_MEM:
            if (mem_info[pcount].mem == INVALIDm) {
                done = 1;
            } else if ((!SOC_MEM_IS_VALID(unit, mem_info[pcount].mem) ||
                       (SOC_MEM_INFO(unit, mem_info[pcount].mem).flags &
                        SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) ||
                       _soc_hr4_mem_skip(unit, mem_info[pcount].mem)) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "skip SER enable for mem %s\n"),
                    SOC_MEM_NAME(unit, mem_info[pcount].mem)));
                continue;
            } else {
                reg = mem_info[pcount].en_ctrl.ctrl_type.en_reg;
                en_mem = mem_info[pcount].en_ctrl.ctrl_type.en_mem;
                flag = mem_info[pcount].en_ctrl.flag_reg_mem;
                field = mem_info[pcount].en_ctrl.en_fld;
                field_position = mem_info[pcount].en_ctrl.en_fld_position;
                ecc1b_reg = mem_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
                ecc1b_en_mem = mem_info[pcount].ecc1b_ctrl.ctrl_type.en_mem;
                ecc1b_flag = mem_info[pcount].ecc1b_ctrl.flag_reg_mem;
                ecc1b_field = mem_info[pcount].ecc1b_ctrl.en_fld;
                ecc1b_field_position = mem_info[pcount].ecc1b_ctrl.en_fld_position;
                str_name = SOC_MEM_NAME(unit, mem_info[pcount].mem);
            }
            break;
        case _SOC_SER_TYPE_BUS:
            if (bus_info[pcount].en_reg == INVALIDr) {
                done = 1;
            } else {
                reg = bus_info[pcount].en_reg;
                field = bus_info[pcount].en_fld;
                ecc1b_reg = bus_info[pcount].ecc1b_en_reg;
                ecc1b_field = bus_info[pcount].ecc1b_en_fld;
                ecc1b_flag = 0;
                str_name = bus_info[pcount].bus_name;
                flag = 0;
            }
            break;
        case _SOC_SER_TYPE_BUF:
            if (buf_info[pcount].en_reg == INVALIDr) {
                done = 1;
            } else {
                reg = buf_info[pcount].en_reg;
                field = buf_info[pcount].en_fld;
                ecc1b_reg = buf_info[pcount].ecc1b_en_reg;
                ecc1b_field = buf_info[pcount].ecc1b_en_fld;
                ecc1b_flag = 0;
                str_name = buf_info[pcount].buffer_name;
                flag = 0;
                if (_soc_hr4_reg_skip(unit, reg)) {
                    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "skip SER enable for buf %s\n"),
                        str_name));
                    continue;
                }
            }
            break;
        default:
            LOG_CLI((BSL_META_U(unit,
                                "Unknown parity module "
                                "[pcount: %d].\n"), pcount));
            return SOC_E_INTERNAL;
        }
        if (done) {
            break;
        } else {
            str_type = parity_module_str[type];
        }

        en_mem = flag ? en_mem : INVALIDm;
        reg = flag ? INVALIDr : reg;
        ecc1b_en_mem = ecc1b_flag ? ecc1b_en_mem : INVALIDm;
        ecc1b_reg = ecc1b_flag ? INVALIDr : ecc1b_reg;

        SOC_IF_ERROR_RETURN(soc_td3_parity_bit_enable
            (unit, reg, en_mem, field, field_position, enable));

        if (ecc1b_reg == ISW3_SER_CONTROL_1r &&
            ecc1b_field == HG_COUNTERS_EN_COR_ERR_RPTf) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_td3_parity_bit_enable
            (unit, ecc1b_reg, ecc1b_en_mem, ecc1b_field, ecc1b_field_position, enable));

        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "SER enable for %s: %s\n"),
                     str_type, str_name));
    } /* walk through each entry of ser_info structures */

    /* reset (toggle) fifo if applicable */
    if (fifo_reset_reg != INVALIDr) {
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, fifo_reset_reg,
                                     REG_PORT_ANY, FIFO_RESETf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, fifo_reset_reg,
                                     REG_PORT_ANY, FIFO_RESETf, 0));
    }

    return SOC_E_NONE;
}

int
soc_hr4_ser_enable_all(int unit, int enable)
{
    uint8                           rbi;
    uint32                          cmic_bit;
    uint32                          cmic_rval0;
    uint32                          cmic_rval1;
    int                             port;
    int                             rv, block_info_idx;
    uint64                          rval64;
    const _soc_hr4_ser_route_block_t *rb;

    soc_iproc_getreg(unit,soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                     &cmic_rval0);
    soc_iproc_getreg(unit,soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r, REG_PORT_ANY, 0),
                     &cmic_rval1);

    soc_td3_ip_pipe_fifo_bmask[unit][0] = _SOC_HR4_IPIPE0_IP_INTR;

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_MEM, ING_SER_FIFO_CTRLr,
        (void*) SOC_IP_MEM_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_MEM, EGR_SER_FIFO_CTRLr,
        (void*) SOC_EP_MEM_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_MEM, INVALIDr,
        (void*) SOC_MMU_MEM_SER_INFO(unit)));

#if defined(INCLUDE_XFLOW_MACSEC)
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable ? 0 : 1,
                    _SOC_SER_TYPE_MEM, INVALIDr,
                    (void*) SOC_MACSEC_MEM_SER_INFO(unit)));
        SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable ? 0 : 1,
                    _SOC_SER_TYPE_BUF, INVALIDr,
                    (void*) &_soc_hr4_macsec_pkt_buffer_ser_info));
    }
#endif

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_REG, L2_MGMT_SER_FIFO_CTRLr,
        (void*) SOC_IP_REG_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_REG, INVALIDr,
        (void*) SOC_EP_REG_SER_INFO(unit)));

#if defined(BCM_56275_A0)
    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_BUS, INVALIDr,
        (void*) &_soc_bcm56275_a0_ip_bus_ser_info));

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_BUS, INVALIDr,
        (void*) &_soc_bcm56275_a0_ep_bus_ser_info));

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_BUF, INVALIDr,
        (void*) &_soc_bcm56275_a0_ip_buffer_ser_info));

    SOC_IF_ERROR_RETURN(_soc_hr4_ser_block_enable(unit, enable,
        _SOC_SER_TYPE_BUF, INVALIDr,
        (void*) &_soc_bcm56275_a0_ep_buffer_ser_info));
#endif

    /* Loop through each place-and-route block entry to enable legacy
       style SER stuff */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_hr4_ser_route_blocks[rbi];
        cmic_bit = rb->cmic_bit;
        port = REG_PORT_ANY;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        /* Ignore MACSEC block */
        if ((rb->blocktype == SOC_BLK_MACSEC) &&
            (!soc_feature(unit, soc_feature_xflow_macsec))) {
            continue;
        }

        switch (rb->cmic_reg) {
            case 0: cmic_rval0 &= ~cmic_bit; break;
            case 1: cmic_rval1 &= ~cmic_bit; break;
            default: break;
        }

        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }

        if (rb->enable_reg != INVALIDr) {
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                SOC_BLK_PORT)) {
                if (port == REG_PORT_ANY)  {
                    /* This port block is not configured */
                    continue;
                }
                if (IS_QSGMII_PORT(unit, port) &&
                    (rb->blocktype == SOC_BLK_XLPORT)) {
                   continue;
                }
            }
        }

        if (enable) {
            switch (rb->cmic_reg) {
                case 0: cmic_rval0 |= cmic_bit; break;
                case 1: cmic_rval1 |= cmic_bit; break;
                default: break;
            }
        }

        if (rb->enable_field != INVALIDf) {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify
                (unit, rb->enable_reg, port, rb->enable_field, enable));
        }

        if (rb->info != NULL) {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, rb->enable_reg,
                                            port, 0, &rval64));
            rv = _soc_hr4_ser_enable_info(unit, block_info_idx, rb->id, port,
                                          rb->enable_reg, &rval64,
                                          rb->info, INVALIDm, enable);
            if (rv == SOC_E_NOT_FOUND) {
                continue;
            } else if (SOC_FAILURE(rv)) {
                return rv;
            }
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, rb->enable_reg,
                                            port, 0, rval64));
        }
    }

    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                     cmic_rval0);
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r, REG_PORT_ANY, 0),
                     cmic_rval1);
    if (enable) {
        /* Write CMIC enable register */
        soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
        (void)soc_ser_tcam_scan_engine_enable(unit, FALSE);
        (void)_soc_hr4_ser_tcam_wrapper_enable(unit, TRUE);
    } else {
        /* Write CMIC disable register */
        soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
        (void)soc_ser_tcam_scan_engine_enable(unit, FALSE);
        (void)_soc_hr4_ser_tcam_wrapper_enable(unit, FALSE);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_hr4_ser_process_pmqport(int unit, int blocktype, int inst)
{
    uint32 rval;
    soc_stat_t *stat = SOC_STAT(unit);

    SOC_IF_ERROR_RETURN(READ_PMQ_ECCr(unit, inst, &rval));
    /* ECC_ERROR_EVENT is clear on read */
    if (soc_reg_field_get(unit, PMQ_ECCr, rval, ECC_ERROR_EVENTf)) {
        SOC_IF_ERROR_RETURN(READ_PMQ_GP_ECCERR_STSr(unit, inst, &rval));
        if (rval != 0) {
            LOG_ERROR(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "ECC error on GPORT memory. Instance %d \n"),
                                  inst));
            soc_ser_stat_update(unit, 0, blocktype, SOC_PARITY_TYPE_ECC,
                                0, SocSerCorrectTypeNoAction, stat);
        }
        SOC_IF_ERROR_RETURN(READ_PMQ_UMAC_ECCERR_STSr(unit, inst, &rval));
        if (rval != 0) {
            LOG_ERROR(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "ECC error on UNIMAC memory. Instance %d \n"),
                                  inst));
            soc_ser_stat_update(unit, 0, blocktype, SOC_PARITY_TYPE_ECC,
                                0, SocSerCorrectTypeNoAction, stat);
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Hardware inconsistency. Instance: %d\n"),
                     inst));
    }
    return SOC_E_NONE;
}
#ifdef INCLUDE_XFLOW_MACSEC
/*
 * Function:
 *      soc_hr4_process_macsec_intr
 * Description:
 *      Monterey specific DPC function to service MACSEC interrupts
 * Parameters:
 *      unit           - Device number
 *      status_reg     - Status register
 *      rb_rval64      - Value of status register
 */
    int
soc_hr4_process_macsec_intr( int unit, soc_reg_t status_reg,
        uint64 rb_rval64 )
{
    if (COMPILER_64_IS_ZERO(rb_rval64)) {
        return SOC_E_NONE;
    }

    /* Process ECC interrupts */
    if(soc_reg64_field32_get(unit, status_reg, rb_rval64,
                ECCERR_INTf)) {
        (void)xflow_macsec_process_ecc_intr(unit, NULL);
    }
    /* Process MACSEC functional interrupts */
    (void)xflow_macsec_process_functional_intr(unit, rb_rval64);

    return SOC_E_NONE;
}
#endif
STATIC int
_soc_hr4_ser_process(int unit, int block_info_idx, int inst, int pipe,
                     int port, soc_reg_t group_reg, uint64 group_rval,
                     const _soc_hr4_ser_info_t *info_list,
                     soc_block_t blocktype,
                     char *prefix_str)
{
#ifdef INCLUDE_XFLOW_MACSEC
    if ((soc_feature(unit, soc_feature_xflow_macsec)) &&
            (blocktype == SOC_BLK_MACSEC)) {
        return soc_hr4_process_macsec_intr(unit, group_reg, group_rval);
    } else
#endif
    if (blocktype == SOC_BLK_PMQPORT) {
        return _soc_hr4_ser_process_pmqport(unit, blocktype, inst);
    } else {
        return soc_td3_ser_process(unit, block_info_idx, inst, pipe,
                                   port, group_reg, group_rval,
                                   info_list, blocktype, prefix_str);
    }
}

STATIC int
soc_hr4_ser_process_fifo(int unit, soc_block_t blk, int pipe,
                         char *prefix_str, int l2_mgmt_ser_fifo)
{
    return soc_trident3_process_ser_fifo(unit, blk, pipe,
                                         prefix_str, l2_mgmt_ser_fifo);
}


STATIC int
_soc_hr4_ser_process_all(int unit, int reg_type, int bit)
{
    uint8      rbi;
    int        port;
    uint32     cmic_bit;
    uint64     rb_enable64, rb_rval64, tmp64;
    const      _soc_hr4_ser_route_block_t *rb;
    char       prefix_str[10];
    int        block_info_idx;
    soc_reg_t  tmp_reg;
    int        processed_an_intr = 0;
    soc_stat_t *stat = SOC_STAT(unit);
    COMPILER_64_ZERO(rb_rval64);

    sal_sprintf(prefix_str, "\nUnit: %d ", unit);

    LOG_VERBOSE(BSL_LS_SOC_SER,
             (BSL_META_U(unit,
                         "unit %d, _soc_hr4_ser_process_all called: reg_type %d, bit %d \n"),
              unit, reg_type, bit));

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
        port = REG_PORT_ANY;
        rb = &_soc_hr4_ser_route_blocks[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (!((rb->cmic_reg == reg_type) && (cmic_bit == 1 << bit))) {
            continue;
        }

        /* Ignore MACSEC block */
        if ((rb->blocktype == SOC_BLK_MACSEC) &&
            (!soc_feature(unit, soc_feature_xflow_macsec))) {
            continue;
        }

        if ((rb->blocktype == SOC_BLK_IPIPE || rb->blocktype == SOC_BLK_EPIPE)
             && (rb->cmic_bit != _SOC_HR4_IPIPE0_CEV_INTR)
             && (rb->cmic_bit != _SOC_HR4_IPIPE0_IDB_INTR)
             && (rb->cmic_bit != _SOC_HR4_IPIPE0_ISW4_INTR)) {
            /* New fifo style processing */
            (void)soc_hr4_ser_process_fifo(unit, rb->blocktype, rb->pipe,
                                           prefix_str, 0);
            stat->ser_err_fifo++;
            processed_an_intr = 1;
        } else {
            /* Legacy processing */
            SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
                if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                    port = SOC_BLOCK_PORT(unit, block_info_idx);
                    break;
                }
            }
            if (rb->enable_reg != INVALIDr) {
                if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                    SOC_BLK_PORT)) {
                    if (port == REG_PORT_ANY) {
                        /* This port block is not configured */
                        LOG_ERROR(BSL_LS_SOC_SER,
                                  (BSL_META_U(unit,
                                              "unit %d SER error on disabled "
                                              "port block %d !!\n"),
                                   unit, block_info_idx));
                        continue;
                    }
                    if (IS_QSGMII_PORT(unit, port) &&
                        (rb->blocktype == SOC_BLK_XLPORT)) {
                        continue;
                    }
                }
            }
            /* Read per route block parity status register */
            if ((rb->status_reg != INVALIDr) &&
                (rb->blocktype != SOC_BLK_MMU)){
                /* Top level MMU register used to enable parity is also used
                 * to control various other functions in MMU.
                 * So we don't read/modify enable_reg and status_reg for MMU
                 * It is handled in _soc_trident3_ser_process_mmu_err()
                 */
                if ((rb->blocktype == SOC_BLK_IPIPE ||
                     rb->blocktype == SOC_BLK_EPIPE) &&
                    (SOC_REG_UNIQUE_ACC(unit, rb->status_reg) != NULL)) {
                    tmp_reg = SOC_REG_UNIQUE_ACC(unit,
                                                 rb->status_reg)[rb->pipe];
                } else {
                    tmp_reg = rb->status_reg;
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, tmp_reg, port, 0, &rb_rval64));

                if (COMPILER_64_IS_ZERO(rb_rval64)) {
                    continue;
                }
            }
            processed_an_intr = 1;
            SOC_IF_ERROR_RETURN
                (_soc_hr4_ser_process(unit, block_info_idx, rb->id,
                                      rb->pipe, port, rb->status_reg,
                                      rb_rval64, rb->info, rb->blocktype,
                                      prefix_str));

            if ((rb->enable_reg != INVALIDr) &&
                (rb->blocktype == SOC_BLK_IPIPE ||
                 rb->blocktype == SOC_BLK_EPIPE)) {
                if ((SOC_REG_UNIQUE_ACC(unit, rb->enable_reg) != NULL)) {
                    tmp_reg = SOC_REG_UNIQUE_ACC(unit,
                                                 rb->enable_reg)[rb->pipe];
                } else {
                    tmp_reg = rb->enable_reg;
                }
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, tmp_reg, port,
                                                0, &rb_enable64));
                COMPILER_64_SET(tmp64, COMPILER_64_HI(rb_rval64),
                                COMPILER_64_LO(rb_rval64));
                COMPILER_64_NOT(tmp64);
                COMPILER_64_AND(rb_enable64, tmp64);
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, tmp_reg, port,
                                                0, rb_enable64));
                COMPILER_64_OR(rb_enable64, rb_rval64);
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, tmp_reg, port,
                                                0, rb_enable64));
            }
            stat->ser_err_int++;
        } /* legacy processing */
    }
    if (!processed_an_intr) {
        LOG_WARN(BSL_LS_SOC_SER,
                 (BSL_META_U(unit,
                             "unit %d, reg_type %d, bit %d "
                             "could not process interrupt !!\n"),
                  unit, reg_type, bit));
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

void
soc_hr4_ser_error(void *unit_vp, void *d1, void *d2, void *d3,
                       void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    int rv;

    rv = _soc_hr4_ser_process_all(unit, PTR_TO_INT(d3), PTR_TO_INT(d4));
    sal_usleep(SAL_BOOT_QUICKTURN ? 1000000 : 10000);

    if (SOC_SUCCESS(rv)) {
        /* Interrupts not processed properly should not be enabled */
        soc_ser_top_intr_reg_enable(unit, PTR_TO_INT(d3), PTR_TO_INT(d4), 1);
    }
    soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
}

int
soc_hr4_mem_ser_control(int unit, soc_mem_t mem, int copyno, int enable)
{
    if (enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                           REG_PORT_ANY, TOP_MMU_RST_Lf, 0));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                           REG_PORT_ANY, TOP_MMU_RST_Lf, 1));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(soc_trident3_clear_mmu_memory(unit));
    }

    if (enable) {
        if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
            SOC_IF_ERROR_RETURN(soc_hr4_ser_enable_all(unit, TRUE));
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_hr4_ser_enable_all(unit, FALSE));
    }

    if (enable) {
        uint32 rval;
        /* Initialize MMU memory */
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
        rval = 0;
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
    }
    return SOC_E_NONE;
}

void
soc_hr4_ser_register(int unit)
{
    soc_cmic_intr_handler_t handle;

#ifdef INCLUDE_MEM_SCAN
    if (!soc_feature(unit, soc_feature_tcam_scan_engine)) {
        /* To access Narrow view for HME stage TCAMs */
        WRITE_IP_PARSER1_HME_STAGE_CONFIG_2r(unit, 0);
        WRITE_IP_PARSER2_HME_STAGE_CONFIG_2r(unit, 0);
        WRITE_IP_PARSER2_HME_STAGE_CONFIG_3r(unit, 0);
        soc_mem_scan_ser_list_register(unit, TRUE, _soc_hr4_tcam_ser_info);
    }
#endif

    memset(&_hr4_ser_functions, 0, sizeof(soc_ser_functions_t));
    _hr4_ser_functions._soc_ser_parity_error_cmicx_intr_f = &soc_hr4_ser_error;
    soc_ser_function_register(unit, &_hr4_ser_functions);

    /* Handler need to be registered for CHIP_INTR_LOW_PRIORITY */
    handle.num = CHIP_INTR_LOW_PRIORITY;
    handle.intr_fn = soc_ser_cmicx_intr_handler;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);
}

int
soc_hr4_ser_lp_mem_info_get(int unit, soc_mem_t mem, int index,
                            soc_mem_t *hash_mem, int *hash_index)
{
    int bank_index, lp_index = 0, bank;
    int rv = SOC_E_NONE;

    switch(mem) {
        case L2_ENTRY_LPm:
            hash_mem[0] = L2Xm;
            if (index < SOC_HR4_NUM_ENTRIES_L2_LP_BANK * SOC_HR4_NUM_BANKS_L2) {
                /* dedicated L2 entries(1W)  */
                lp_index = index;
            } else {
                return SOC_E_INTERNAL;
            }
            hash_index[0] = lp_index * 4;
            break;
        case L3_ENTRY_LPm:
            if (index < SOC_HR4_NUM_ENTRIES_L3_LP_BANK * SOC_HR4_NUM_BANKS_L3) {
                /* dedicated L3 entries(1W, 2W, 4W)  */
                bank = index / SOC_HR4_NUM_ENTRIES_L3_LP_BANK;
                bank_index = index % SOC_HR4_NUM_ENTRIES_L3_LP_BANK;

                if (bank_index >= 0 && bank_index < 4096) {
                    /* 1W view - 4K entries per bank */
                    hash_mem[0] = L3_ENTRY_SINGLEm;
                    lp_index = bank_index + (4096 * bank);
                } else if (bank_index >= 4096 && bank_index < 6144) {
                    /* 2W view - 2K entries per bank */
                    hash_mem[0] = L3_ENTRY_DOUBLEm;
                    lp_index = (bank_index - 4096) + (2048 * bank);
                } else if (bank_index >= 6144 && bank_index < 7168) {
                    /* 4W view - 1K entries per bank */
                    hash_mem[0] = L3_ENTRY_QUADm;
                    lp_index = (bank_index - 6144) + (1024 * bank);
                }
            } else {
                return SOC_E_INTERNAL;
            }
            hash_index[0] = lp_index * 4;
            break;
        case MPLS_ENTRY_LPm:
        case VLAN_XLATE_1_LPm:
        case VLAN_XLATE_2_LPm:
        case EGR_VLAN_XLATE_1_LPm:
        case EGR_VLAN_XLATE_2_LPm:
            /* UAT mem (1W, 2W) */
            bank = index / SOC_HR4_NUM_ENTRIES_UAT_LP_BANK;
            bank_index = index % SOC_HR4_NUM_ENTRIES_UAT_LP_BANK;

            if (bank_index >= 0 && bank_index < 128) {
                /* 1W view - 128 entries per bank */
                switch (mem) {
                    case MPLS_ENTRY_LPm: hash_mem[0] = MPLS_ENTRY_SINGLEm; break;
                    case VLAN_XLATE_1_LPm: hash_mem[0] = VLAN_XLATE_1_SINGLEm; break;
                    case VLAN_XLATE_2_LPm: hash_mem[0] = VLAN_XLATE_2_SINGLEm; break;
                    case EGR_VLAN_XLATE_1_LPm: hash_mem[0] = EGR_VLAN_XLATE_1_SINGLEm; break;
                    case EGR_VLAN_XLATE_2_LPm: hash_mem[0] = EGR_VLAN_XLATE_2_SINGLEm; break;
                }
                lp_index = bank_index + (128 * bank);
            } else if (bank_index >= 128 && bank_index < 192) {
                /* 2W view - 64 entries per bank */
                switch (mem) {
                    case MPLS_ENTRY_LPm: hash_mem[0] = MPLS_ENTRYm; break;
                    case VLAN_XLATE_1_LPm: hash_mem[0] = VLAN_XLATE_1_DOUBLEm; break;
                    case VLAN_XLATE_2_LPm: hash_mem[0] = VLAN_XLATE_2_DOUBLEm; break;
                    case EGR_VLAN_XLATE_1_LPm: hash_mem[0] = EGR_VLAN_XLATE_1_DOUBLEm; break;
                    case EGR_VLAN_XLATE_2_LPm: hash_mem[0] = EGR_VLAN_XLATE_2_DOUBLEm; break;
                }
                lp_index = (bank_index - 128) + (64 * bank);
            }
            hash_index[0] = lp_index * 4;
            break;
        default:
            rv = SOC_E_PARAM;
    }

    return rv;
}

int
soc_hurricane4_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                             _soc_ser_sram_info_t *sram_info)
{
    int i, base;

    switch (mem) {
    case L2_ENTRY_ECCm:
        sram_info->disable_mem = L2_ENTRY_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        if (index < SOC_HR4_NUM_ENTRIES_L2) {
            /* dedicated L2 entries - XOR not used */
            sram_info->ram_count = 1;
            base = index;
        } else {
            /* NO UFT */
            return SOC_E_INTERNAL;
        }
        break;
    case L3_ENTRY_ECCm:
        sram_info->disable_mem = L3_ENTRY_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        if (index < SOC_HR4_NUM_ENTRIES_L3) {
            /* dedicated L3 entries - XOR not used */
            sram_info->ram_count = 1;
            base = index;
        } else {
            /* NO UFT */
            return SOC_E_INTERNAL;
        }
        break;
    case VLAN_XLATE_1_ECCm:
        sram_info->disable_mem = VLAN_XLATE_1_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        /* XOR not used */
        sram_info->ram_count = 1;
        base = index;
        break;
    case VLAN_XLATE_2_ECCm:
        sram_info->disable_mem = VLAN_XLATE_2_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        /* XOR not used */
        sram_info->ram_count = 1;
        base = index;
        break;
    case EGR_VLAN_XLATE_1_ECCm:
        sram_info->disable_mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        /* XOR not used */
        sram_info->ram_count = 1;
        base = index;
        break;
    case EGR_VLAN_XLATE_2_ECCm:
        sram_info->disable_mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        /* XOR not used */
        sram_info->ram_count = 1;
        base = index;
        break;
    case MPLS_ENTRY_ECCm:
        sram_info->disable_mem = MPLS_ENTRY_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        /* XOR not used */
        sram_info->ram_count = 1;
        base = index;
        break;
    case ING_DNAT_ADDRESS_TYPE_ECCm:
    case ING_VP_VLAN_MEMBERSHIP_ECCm:
    case EGR_VP_VLAN_MEMBERSHIP_ECCm:
    case SUBPORT_ID_TO_SGPP_MAP_ECCm:
        sram_info->view[0] = mem;
        sram_info->index_count[0] = 1;
        sram_info->ram_count = 1;
        base = index;
        break;
    default:
        return SOC_E_UNAVAIL;
    }
    sram_info->mem_indexes[0][0] = base;
    for (i = 1; i < sram_info->ram_count; i++) {
        sram_info->mem_indexes[i][0] = sram_info->mem_indexes[i-1][0] + 1;
    }
    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_hr4_ser_parity_control_reg_set(int unit, ser_test_data_t *test_data, int enable)
{
    return soc_td3_ser_parity_control_reg_set(unit, test_data, enable);
}

STATIC soc_error_t
_soc_hr4_ser_error_injection_support(int unit, soc_mem_t mem, int pipe_target)
{
   return soc_td3_ser_error_injection_support(unit, mem, pipe_target);
}

STATIC soc_error_t
_ser_hr4_ser_correction_info_get(int unit, ser_correction_info_t *corr_info)
{
    return soc_td3_ser_correction_info_get(unit, corr_info);
}

/*
 * Function:
 *      soc_hr4_ser_inject_error
 * Purpose:
 *      Injects an error into a single td3 memory
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 */
STATIC soc_error_t
soc_hr4_ser_inject_error(int unit, uint32 flags, soc_mem_t mem, int pipe_target,
                        int block, int index)
{
    return soc_td3_ser_inject_or_test_mem(unit, mem, pipe_target, block, index,
                                          SER_SINGLE_INDEX, TRUE, FALSE, flags);
}

/*
 * Function:
 *      soc_hr4_ser_test_mem
 * Purpose:
 *      Perform SER test on a single memory, or generate a test the user can
 *      enter by the command line.
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      test_type   - (IN) How many indices to test in the passes memory
 *      cmd         - (IN) TRUE if a command-line test is desired.
 * Returns:
 *      SOC_E_NONE if test passes, an error otherwise (multiple types of errors
 *      are possible.)
 */
STATIC soc_error_t
soc_hr4_ser_test_mem(int unit, soc_mem_t mem, _soc_ser_test_t test_type,
                    int cmd)
{
    return soc_td3_ser_inject_or_test_mem(unit, mem, -1, MEM_BLOCK_ANY, 0,
                                          test_type, FALSE, cmd, FALSE);
}

STATIC void
soc_hr4_ser_parity_control_check_all(int unit)
{
#if defined(BCM_56275_A0)
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                            (void*) _soc_bcm56275_a0_ip_bus_ser_info);
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                            (void*) _soc_bcm56275_a0_ep_bus_ser_info);
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                            (void*) _soc_bcm56275_a0_ip_buffer_ser_info);
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                            (void*) _soc_bcm56275_a0_ep_buffer_ser_info);
#endif
}

/*
 * Function:
 *      _soc_hr4_dual_parser_parity_bit_enable
 * Purpose:
 *      DUAL parser memories need parity bit
 *      to be disable on both the PARSERs (1 & 2). When working on any of the
 *      PARSER
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) memory
 *      enable      - (IN) PARITY enable/disable
 * Returns:
 *  SOC_E_XXXX
 */
int
_soc_hr4_dual_parser_parity_bit_enable(int unit, soc_mem_t mem, int enable)
{
    int i;
    soc_reg_t en_reg = INVALIDr;
    soc_mem_t en_mem = INVALIDm;
    soc_mem_t new_mem = INVALIDm;
    _soc_mem_ser_en_info_t *ser_info_list;

    ser_info_list = SOC_IP_MEM_SER_INFO(unit);

    switch (mem) {
        /* PARSER1 */
        case IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_0m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_0m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_1m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_1m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_2m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_2m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_3m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_3m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_4m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_4m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_0m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_0m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_1m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_1m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_3m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_4m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_4m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_ONLY_0m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_ONLY_0m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_ONLY_1m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_ONLY_1m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_ONLY_2m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_ONLY_2m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_ONLY_3m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_ONLY_3m;
            break;
        case IP_PARSER1_HME_STAGE_TCAM_ONLY_4m:
            new_mem = IP_PARSER2_HME_STAGE_TCAM_ONLY_4m;
            break;
        case IP_PARSER1_MICE_TCAM_0m:
            new_mem = IP_PARSER2_MICE_TCAM_0m;
            break;
        case IP_PARSER1_MICE_TCAM_1m:
            new_mem = IP_PARSER2_MICE_TCAM_1m;
            break;
        case IP_PARSER1_MICE_TCAM_DATA_ONLY_0m:
            new_mem = IP_PARSER2_MICE_TCAM_DATA_ONLY_0m;
            break;
        case IP_PARSER1_MICE_TCAM_DATA_ONLY_1m:
            new_mem = IP_PARSER2_MICE_TCAM_DATA_ONLY_1m;
            break;
        case IP_PARSER1_MICE_TCAM_ONLY_0m:
            new_mem = IP_PARSER2_MICE_TCAM_ONLY_0m;
            break;
        case IP_PARSER1_MICE_TCAM_ONLY_1m:
            new_mem = IP_PARSER2_MICE_TCAM_ONLY_1m;
            break;

        /* PARSER2 */
        case IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_0m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_0m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_1m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_1m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_2m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_2m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_3m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_3m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_4m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_4m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_0m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_0m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_1m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_1m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_3m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_4m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_4m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_ONLY_0m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_ONLY_0m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_ONLY_1m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_ONLY_1m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_ONLY_2m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_ONLY_2m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_ONLY_3m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_ONLY_3m;
            break;
        case IP_PARSER2_HME_STAGE_TCAM_ONLY_4m:
            new_mem = IP_PARSER1_HME_STAGE_TCAM_ONLY_4m;
            break;
        case IP_PARSER2_MICE_TCAM_0m:
            new_mem = IP_PARSER1_MICE_TCAM_0m;
            break;
        case IP_PARSER2_MICE_TCAM_1m:
            new_mem = IP_PARSER1_MICE_TCAM_1m;
            break;
        case IP_PARSER2_MICE_TCAM_DATA_ONLY_0m:
            new_mem = IP_PARSER1_MICE_TCAM_DATA_ONLY_0m;
            break;
        case IP_PARSER2_MICE_TCAM_DATA_ONLY_1m:
            new_mem = IP_PARSER1_MICE_TCAM_DATA_ONLY_1m;
            break;
        case IP_PARSER2_MICE_TCAM_ONLY_0m:
            new_mem = IP_PARSER1_MICE_TCAM_ONLY_0m;
            break;
        case IP_PARSER2_MICE_TCAM_ONLY_1m:
            new_mem = IP_PARSER1_MICE_TCAM_ONLY_1m;
            break;

        default:
            return SOC_E_NONE;
    }
    LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "Main mem %s Map mem %s Enable %d !!\n"),
                 SOC_MEM_NAME(unit, mem), SOC_MEM_NAME(unit, new_mem), enable));
    for (i = 0; ser_info_list[i].mem != INVALIDm; i++) {
        if (ser_info_list[i].mem == new_mem) {
            en_reg = ser_info_list[i].en_ctrl.flag_reg_mem ? INVALIDr :
                ser_info_list[i].en_ctrl.ctrl_type.en_reg;
            en_mem = ser_info_list[i].en_ctrl.flag_reg_mem ?
                ser_info_list[i].en_ctrl.ctrl_type.en_mem : INVALIDm;
            return soc_td3_parity_bit_enable(unit, en_reg, en_mem,
                                             ser_info_list[i].en_ctrl.en_fld,
                                             ser_info_list[i].en_ctrl.en_fld_position,
                                             enable);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hr4_ser_test
 * Purpose:
 *      Tests that SER is working for all supported memories.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  SOC_E_NONE if test passes, an error otherwise (multiple types of errors are
 *      possible.)
 */
STATIC soc_error_t
soc_hr4_ser_test(int unit, _soc_ser_test_t test_type)
{
    soc_hr4_ser_parity_control_check_all(unit);
    return soc_td3_ser_test(unit, test_type);
}

static soc_ser_test_functions_t _soc_hr4_ser_test_fun;

void
soc_hr4_ser_test_register(int unit)
{
    /*Initialize chip-specific functions for SER testing*/
    memset(&_soc_hr4_ser_test_fun, 0, sizeof(soc_ser_test_functions_t));
    _soc_hr4_ser_test_fun.inject_error_f = &soc_hr4_ser_inject_error;
    _soc_hr4_ser_test_fun.test_mem = &soc_hr4_ser_test_mem;
    _soc_hr4_ser_test_fun.test = &soc_hr4_ser_test;
    _soc_hr4_ser_test_fun.parity_control = &_soc_hr4_ser_parity_control_reg_set;
    _soc_hr4_ser_test_fun.injection_support = &_soc_hr4_ser_error_injection_support;
    _soc_hr4_ser_test_fun.correction_info_get = &_ser_hr4_ser_correction_info_get;
    soc_ser_test_functions_register(unit, &_soc_hr4_ser_test_fun);
}

#endif /* BCM_HURRICANE4_SUPPORT */
