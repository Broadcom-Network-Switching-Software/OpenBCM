/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:      ser.c
 * Purpose:   SER enable/detect and test functionality.
 * Requires:  sal/soc/shared layer
 */


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/intr.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif
#ifdef ALPM_ENABLE
#include <soc/esw/alpm_int.h>
#endif

#ifdef BCM_TRIDENT3_SUPPORT

#include <soc/soc_ser_log.h>
#include <soc/trident3.h>
#include <bcm_int/common/field.h>

#define _SOC_TD3_IFP_SLICE_MODE_NARROW 0
#define _SOC_TD3_IFP_SLICE_MODE_WIDE 1

#define _SOC_MMU_BASE_INDEX_GLB 0
#define _SOC_MMU_BASE_INDEX_XPE 0
#define _SOC_MMU_BASE_INDEX_SC 0
#define _SOC_MMU_BASE_INDEX_SED 0

#define _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD 0xfffc7fff /* [17:15] = 3'b000 */

#define _SOC_TD3_ACC_TYPE_UNIQUE_PIPE0   0
#define _SOC_TD3_ACC_TYPE_UNIQUE_PIPE1   1
#define _SOC_TD3_ACC_TYPE_SINGLE   20
#define _SOC_TD3_ACC_TYPE_DUPLICATE   9
#define _SOC_TD3_ACC_TYPE_ADDR_SPLIT_DIST   10
#define _SOC_TD3_ACC_TYPE_ADDR_SPLIT_SPLIT  12
#define _SOC_TD3_ACC_TYPE_DATA_SPLIT   14
#define _SOC_TD3_ACC_TYPE_NAME(at) \
            (at == _SOC_TD3_ACC_TYPE_UNIQUE_PIPE0) ? "AT_UNIQUE_PIPE0" : \
            (at == _SOC_TD3_ACC_TYPE_UNIQUE_PIPE1) ? "AT_UNIQUE_PIPE1" : \
            (at == _SOC_TD3_ACC_TYPE_SINGLE) ? "AT_SINGLE" : \
            (at == _SOC_TD3_ACC_TYPE_DUPLICATE) ? "AT_DUPLICATE" : \
            (at == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_DIST) ? "AT_ADDR_SPLIT_DIST" : \
            (at == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_SPLIT) ? "AT_ADDR_SPLIT_SPLIT" : \
            (at == _SOC_TD3_ACC_TYPE_DATA_SPLIT) ? "AT_DATA_SPLIT" : \
            "AT_UNKNOWN"

#define _SOC_TD3_CEV_INTR 0x00000001
#define _SOC_TD3_IPIPE0_IDB_INTR 0x00000080
#define _SOC_TD3_IPIPE1_IDB_INTR 0x00000100
#define _SOC_TD3_IPIPE0_ISW4_INTR 0x00000800
#define _SOC_TD3_IPIPE1_ISW4_INTR 0x00001000

typedef struct _soc_td3_ser_mmu_intr_info_s {
    soc_reg_t   int_statf;
    int         mmu_base_index;
    int         ser_info_index;
} _soc_td3_ser_mmu_intr_info_t;


typedef struct _soc_td3_ser_route_block_s {
    uint8               cmic_reg; /* 3: intr3, 4: intr4, 5: intr5 */
    uint32              cmic_bit;
    soc_block_t         blocktype;
    int                 pipe;
    soc_reg_t           enable_reg;
    soc_reg_t           status_reg;
    soc_field_t         enable_field;
    void                *info;
    uint8               id;
} _soc_td3_ser_route_block_t;

typedef struct _soc_td3_tcam_wrapper_info_s {
    soc_mem_t   mem;
    soc_reg_t   enable_reg;
    soc_field_t enable_field;
    soc_field_t parity_field; /* used to sw inject test */
} _soc_td3_tcam_wrapper_info_t;

#if defined(BCM_56870_A0)
extern _soc_bus_ser_en_info_t _soc_bcm56870_a0_ip_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56870_a0_ip_buffer_ser_info[];
extern _soc_bus_ser_en_info_t _soc_bcm56870_a0_ep_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56870_a0_ep_buffer_ser_info[];
#endif

typedef struct _soc_td3_tmp_ser_info_s {
    soc_reg_t           enable_reg;
    soc_field_t         enable_field;
    uint32              value;
} _soc_td3_tmp_ser_info_t;

typedef struct _soc_td3_hash_table_ser_info_s {
    soc_mem_t       mem;
    soc_field_t     enable_field;
} _soc_td3_hash_table_ser_info_t;

static soc_ser_functions_t _td3_ser_functions;

_soc_bus_buffer_ser_force_info_t ser_td3_bus_buffer_skip_test[] = {
    { "DPP_OUTPUT_EVENT_FIFO", SW4_SER_CONTROLr, EVENT_FIFO_ECC_FORCE_ERRf },
    { "SLOT_PIPELINE", SW4_SER_CONTROLr, SLOT_PIPELINE_ECC_FORCE_ERRf },
    { "SOP_EVENT_BUFFER", IDB_SER_CONTROLr, SOP_EVENT_ECC_CORRUPTf },
    { "EOP_EVENT_BUFFER", IDB_SER_CONTROLr, EOP_EVENT_ECC_CORRUPTf },
    { "MGMT_CELL_ASSEM_BUFFER", IDB_SER_CONTROLr, CA_MGMT_ECC_CORRUPTf },
    { "", INVALIDr } /* end */
};

uint32 ser_td3_bus_buffer_hwbase[] = {
    0x1b, /* IRSEL2_BUS */
    0x14, /* IPARS2_BUS */
    0x2b, /* ICFG_ISW3_PT_BUS */
    0x5e, /* EVLAN_BUS */
    0,    /* DPP_OUTPUT_EVENT_FIFO */
    0x2c, /* IRSEL1_FPEM_DELAY_BUFFER */
    0x75, /* EFP_DATA_PT_BUF */
    0x9,  /* ISW3_EOP_BUFFER_B */
    0,    /* MGMT_CELL_ASSEM_BUFFER */
    0x26, /* IFWD2_PT_BUF */
    0,    /* SLOT_PIPELINE */
    0x77, /* EGR_VLAN_HWY2 */
    0x41, /* EP_INITBUF_RAM_WRAPPER */
    0x11, /* IPARS1_BUS */
    0x40, /* INGRESS_PACKET_BUFFER */
    0x5a, /* EVLAN_BUS */
    0x23, /* IPARS2_PT_BUF */
    0x19, /* ISW1_BUS */
    0x21, /* IVXLT1_PT_BUF */
    0x18, /* IRSEL1_PT_BUS */
    0x1a, /* IFP_BUS */
    0x79, /* EGR_EL3_HWY2 */
    0x15, /* IVXLT2_BUS */
    0,    /* EOP_EVENT_BUFFER */
    0x78, /* EGR_EL3_HWY1 */
    0x12, /* IVXLT1_BUS */
    0x29, /* IRSEL2_PT_BUS */
    0x5c, /* EFP_BUS */
    0x3a, /* RSEL1_FLEX_CTR_BUS_FIFO */
    0x18, /* IRSEL1_BUS */
    0x1c, /* ISW2_BUS */
    0x13, /* IVP_BUS */
    0x22, /* IVP_PT_BUF */
    0x39, /* IPARS2_PT_BUF1 */
    0x5b, /* EPMOD_BUS */
    0x28, /* IFP_PT_BUF */
    0,    /* SOP_EVENT_BUFFER */
    0x40, /* IP_PACKET_BUFFER */
    0x17, /* IFWD2_BUS */
    0x25, /* IFWD1_PT_BUS */
    0x24, /* IVXLT2_PT_BUF */
    0x76, /* EGR_VLAN_HWY1 */
    0,    /* CELL_QUEUE */
    0x16, /* IFWD1_BUS */
    0x2a  /* ISW2_PT_BUS */
};

static soc_field_t _soc_td3_mmu_xpe_enable_fields[] = {
    AGING_PARITY_ENf,
    TCB_PARITY_ENf,
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

static soc_field_t _soc_td3_mmu_sc_enable_fields[] = {
    TDM_PARITY_ENf,
    VBS_PARITY_ENf,
    TOQ_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_td3_mmu_sed_enable_fields[] = {
    MB0_PARITY_ENf,
    MB1_PARITY_ENf,
    ENQS_PARITY_ENf,
    MTRO_PARITY_ENf,
    DQS_PARITY_ENf,
    CFAP_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_td3_mmu_sed_int_enable_fields[] = {
    CFAP_MEM_FAIL_ENf,
    MEM_PAR_ERR_ENf,
    INVALIDf
};
static soc_field_t _soc_td3_mmu_sed_int_status_fields[] = {
    CFAP_MEM_FAIL_STATf,
    MEM_PAR_ERR_STATf,
    INVALIDf
};
static soc_field_t _soc_td3_mmu_sed_int_clr_fields[] = {
    CFAP_MEM_FAIL_CLRf,
    MEM_PAR_ERR_CLRf,
    INVALIDf
};

static _soc_td3_ser_info_t _soc_td3_mmu_ser_info[] = {
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
        MMU_XCFG_PARITY_ENr, INVALIDf, _soc_td3_mmu_xpe_enable_fields,
        MMU_XCFG_XPE_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_XCFG_XPE_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_XCFG_XPE_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_MMU_SC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SC MEM PAR",
        MMU_SCFG_PARITY_ENr, INVALIDf, _soc_td3_mmu_sc_enable_fields,
        MMU_SCFG_SC_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_SCFG_SC_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf/*INVALIDf*/,
        NULL/*_soc_td3_mmu_sc_status_fields*/,
        MMU_SCFG_SC_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf/*INVALIDf*/,
        NULL/*_soc_td3_mmu_sc_clr_fields*/
    },
    { _SOC_TD3_PARITY_TYPE_MMU_SED, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SED MEM PAR",
        MMU_SEDCFG_PARITY_ENr, INVALIDf, _soc_td3_mmu_sed_enable_fields,
        MMU_SEDCFG_SED_CPU_INT_ENr, INVALIDf,
        _soc_td3_mmu_sed_int_enable_fields,
        MMU_SEDCFG_SED_CPU_INT_STATr, NULL, INVALIDf,
        _soc_td3_mmu_sed_int_status_fields,
        MMU_SEDCFG_SED_CPU_INT_CLEARr, INVALIDf,
        _soc_td3_mmu_sed_int_clr_fields
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static soc_field_t _soc_td3_mmu_top_int_enable_fields[] = {
    SED_CPU_INT_ENf,
    SC_CPU_INT_ENf,
    XPE_CPU_INT_ENf,
    GLB_CPU_INT_ENf,
    INVALIDf
};

static soc_field_t _soc_td3_mmu_top_int_status_fields[] = {
    SED_CPU_INT_STATf,
    SC_CPU_INT_STATf,
    XPE_CPU_INT_STATf,
    GLB_CPU_INT_STATf,
    INVALIDf
};

static _soc_td3_ser_info_t _soc_td3_mmu_top_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_MMU_SER, _soc_td3_mmu_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GCFG_ALL_CPU_INT_ENr, INVALIDf,
        _soc_td3_mmu_top_int_enable_fields,
        MMU_GCFG_ALL_CPU_INT_STATr,
        NULL, INVALIDf, _soc_td3_mmu_top_int_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static const
_soc_td3_ser_mmu_intr_info_t mmu_intr_info[] = {
    { GLB_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_GLB, 0 },
    { XPE_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE, 1 },
    { SC_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC, 2 },
    { SED_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SED, 3},
    { INVALIDf, -1, -1 }
};

static _soc_td3_ser_reg_t _soc_td3_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC" },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_td3_ser_reg_t _soc_td3_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC" },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_td3_ser_reg_t _soc_td3_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_td3_ser_reg_t _soc_td3_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" },
    { INVALIDr, "INVALIDr"}
};

static _soc_td3_ser_info_t _soc_td3_pm_clp_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        CLMAC_ECC_CTRLr, RX_TS_MEM_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        CLMAC_ECC_CTRLr, RX_TS_MEM_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC single bit",
        CLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC double bits",
        CLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC single bit",
        CLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC double bits",
        CLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_td3_pm_clp_rx_status_reg, INVALIDf, NULL,
        CLPORT_INTR_STATUSr, MIB_RX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_td3_pm_clp_tx_status_reg, INVALIDf, NULL,
        CLPORT_INTR_STATUSr, MIB_TX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_td3_ser_info_t _soc_td3_pm_xlp_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_XLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "MAC TX CDC memory",
        XLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_td3_pm_xlp_rx_status_reg, INVALIDf, NULL,
        XLPORT_INTR_STATUSr, MIB_RX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_td3_pm_xlp_tx_status_reg, INVALIDf, NULL,
        XLPORT_INTR_STATUSr, MIB_TX_MEM_ERRf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_td3_ser_reg_t _soc_td3_idb_status_reg[] = {
    { IDB_OBM0_OVERSUB_MON_ECC_STATUSr, "IDB OBM0 oversub Monitoring memory" },
    { IDB_OBM1_OVERSUB_MON_ECC_STATUSr, "IDB OBM1 oversub Monitoring memory" },
    { IDB_OBM2_OVERSUB_MON_ECC_STATUSr, "IDB OBM2 oversub Monitoring memory" },
    { IDB_OBM3_OVERSUB_MON_ECC_STATUSr, "IDB OBM3 oversub Monitoring memory" },
    { IDB_OBM4_OVERSUB_MON_ECC_STATUSr, "IDB OBM4 oversub Monitoring memory" },
    { IDB_OBM5_OVERSUB_MON_ECC_STATUSr, "IDB OBM5 oversub Monitoring memory" },
    { IDB_OBM6_OVERSUB_MON_ECC_STATUSr, "IDB OBM6 oversub Monitoring memory" },
    { IDB_OBM7_OVERSUB_MON_ECC_STATUSr, "IDB OBM7 oversub Monitoring memory" },
    { IDB_OBM8_OVERSUB_MON_ECC_STATUSr, "IDB OBM8 oversub Monitoring memory" },
    { IDB_OBM9_OVERSUB_MON_ECC_STATUSr, "IDB OBM9 oversub Monitoring memory" },
    { IDB_OBM10_OVERSUB_MON_ECC_STATUSr, "IDB OBM10 oversub Monitoring memory" },
    { IDB_OBM11_OVERSUB_MON_ECC_STATUSr, "IDB OBM11 oversub Monitoring memory" },
    { IDB_OBM12_OVERSUB_MON_ECC_STATUSr, "IDB OBM12 oversub Monitoring memory" },
    { IDB_OBM13_OVERSUB_MON_ECC_STATUSr, "IDB OBM13 oversub Monitoring memory" },
    { IDB_OBM14_OVERSUB_MON_ECC_STATUSr, "IDB OBM14 oversub Monitoring memory" },
    { IDB_OBM15_OVERSUB_MON_ECC_STATUSr, "IDB OBM15 oversub Monitoring memory" },
    { IDB_CA_CPU_ECC_STATUSr, "CA-CPU packet buffer memory" },
    { IDB_CA_LPBK_ECC_STATUSr, "CA-LPBK packet buffer memory" },
    { IDB_IS_TDM_CAL_ECC_STATUSr, "IDB IS_TDM0,1 calendar" },
    { IDB_SOP_EVENT_ECC_STATUSr, "IDB SOP Event Fifo ECC Status" },
    { IDB_EOP_EVENT_ECC_STATUSr, "IDB EOP Event Fifo ECC Status" },
    { INVALIDr }
};

static _soc_td3_ser_info_t _soc_td3_idb_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM0 DATA FIFO",
        IDB_OBM0_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM0_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM1 DATA FIFO",
        IDB_OBM1_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM1_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM2 DATA FIFO",
        IDB_OBM2_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM2_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM3 DATA FIFO",
        IDB_OBM3_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM3_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM4 DATA FIFO",
        IDB_OBM4_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM4_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM5 DATA FIFO",
        IDB_OBM5_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM5_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM6 DATA FIFO",
        IDB_OBM6_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM6_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM7 DATA FIFO",
        IDB_OBM7_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM7_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM8 DATA FIFO",
        IDB_OBM8_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM8_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM9 DATA FIFO",
        IDB_OBM9_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM9_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM10 DATA FIFO",
        IDB_OBM10_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM10_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM11 DATA FIFO",
        IDB_OBM11_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM11_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM12 DATA FIFO",
        IDB_OBM12_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM12_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM13 DATA FIFO",
        IDB_OBM13_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM13_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM14 DATA FIFO",
        IDB_OBM14_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM14_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM15 DATA FIFO",
        IDB_OBM15_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM15_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM0 CELL ASSEM BUFFER",
        IDB_OBM0_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM0_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM1 CELL ASSEM BUFFER",
        IDB_OBM1_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM1_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM2 CELL ASSEM BUFFER",
        IDB_OBM2_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM2_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM3 CELL ASSEM BUFFER",
        IDB_OBM3_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM3_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM4 CELL ASSEM BUFFER",
        IDB_OBM4_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM4_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM5 CELL ASSEM BUFFER",
        IDB_OBM5_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM5_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM6 CELL ASSEM BUFFER",
        IDB_OBM6_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM6_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM7 CELL ASSEM BUFFER",
        IDB_OBM7_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM7_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM8 CELL ASSEM BUFFER",
        IDB_OBM8_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM8_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM9 CELL ASSEM BUFFER",
        IDB_OBM9_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM9_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM10 CELL ASSEM BUFFER",
        IDB_OBM10_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM10_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM11 CELL ASSEM BUFFER",
        IDB_OBM11_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM11_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM12 CELL ASSEM BUFFER",
        IDB_OBM12_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM12_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM13 CELL ASSEM BUFFER",
        IDB_OBM13_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM13_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM14 CELL ASSEM BUFFER",
        IDB_OBM14_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM14_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM15 CELL ASSEM BUFFER",
        IDB_OBM15_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM15_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_td3_idb_status_reg, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_TD3_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_td3_ser_info_t _soc_td3_isw4_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        SLOT_PIPELINE_ECC_ERRf, SLOT_PIPELINE_ECC_ERRf,
        INVALIDm, "Slot pipeline",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        SLOT_PIPELINE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        CELL_QUEUE_ECC_ERRf, CELL_QUEUE_ECC_ERRf,
        INVALIDm, "Cell queue",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CELL_QUEUE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0,
        EVENT_FIFO_ECC_ERRf, EVENT_FIFO_ECC_ERRf,
        INVALIDm, "Event FIFO",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        EVENT_FIFO_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_TD3_PARITY_TYPE_NONE} /* table terminator */
};

static _soc_td3_ser_info_t _soc_td3_cev_ser_info[] = {
    { _SOC_TD3_PARITY_TYPE_ECC, NULL, 0, 
        FIFO_PARITY_ERRORf, FIFO_PARITY_ERRORf,
        INVALIDm, "Central fifo",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CENTRAL_CTR_EVICTION_INTR_STATUSr, NULL, FIFO_PARITY_ERRORf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_TD3_PARITY_TYPE_NONE} /* table terminator */
};

/* Setup dynamically in enable ser */
uint32 soc_td3_ip_pipe_fifo_bmask[SOC_MAX_NUM_DEVICES][2]={{0}};

static const
_soc_td3_ser_route_block_t  _soc_td3_ser_route_blocks[] = {
    { 0, 0x00000001, /* CEV_TO_CMIC_INTR */
      SOC_BLK_IPIPE, -1, CENTRAL_CTR_EVICTION_INTR_ENABLEr, CENTRAL_CTR_EVICTION_INTR_STATUSr, INVALIDf,
      _soc_td3_cev_ser_info, 0 },
    { 0, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTR */
      SOC_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      _soc_td3_mmu_top_ser_info, 0 },
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    { 0, 0x00000020, /* PIPE0_EP_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000040, /* PIPE1_EP_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000080, /* PIPE0_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_td3_idb_ser_info, 0 },
    { 0, 0x00000100, /* PIPE1_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_td3_idb_ser_info, 0 },
    { 0, 0x00000200, /* PIPE0_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 0, 0x00000400, /* PIPE1_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 0, 0x00000800, /* PIPE0_ISW4_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, ISW4_INTR_ENABLEr, ISW4_INTR_STATUSr, INVALIDf,
      _soc_td3_isw4_ser_info, 0 },
    { 0, 0x00001000, /* PIPE1_ISW4_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, ISW4_INTR_ENABLEr, ISW4_INTR_STATUSr, INVALIDf,
      _soc_td3_isw4_ser_info, 0 },
    { 1, 0x00000001, /* PM0_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 0 },
    { 1, 0x00000002, /* PM1_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 1 },
    { 1, 0x00000004, /* PM2_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 2 },
    { 1, 0x00000008, /* PM3_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 3 },
    { 1, 0x00000010, /* PM4_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 4 },
    { 1, 0x00000020, /* PM5_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 5 },
    { 1, 0x00000040, /* PM6_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 6 },
    { 1, 0x00000080, /* PM7_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 7 },
    { 1, 0x00000100, /* PM8_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 8 },
    { 1, 0x00000200, /* PM9_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 9 },
    { 1, 0x00000400, /* PM10_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 10 },
    { 1, 0x00000800, /* PM11_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 11 },
    { 1, 0x00001000, /* PM12_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 12 },
    { 1, 0x00002000, /* PM13_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 13 },
    { 1, 0x00004000, /* PM14_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 14 },
    { 1, 0x00008000, /* PM15_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 15 },
    { 1, 0x00010000, /* PM16_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 16 },
    { 1, 0x00020000, /* PM17_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 17 },
    { 1, 0x00040000, /* PM18_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 18 },
    { 1, 0x00080000, /* PM19_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 19 },
    { 1, 0x00100000, /* PM20_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 20 },
    { 1, 0x00200000, /* PM21_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 21 },
    { 1, 0x00400000, /* PM22_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 22 },
    { 1, 0x00800000, /* PM23_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 23 },
    { 1, 0x01000000, /* PM24_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 24 },
    { 1, 0x02000000, /* PM25_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 25 },
    { 1, 0x04000000, /* PM26_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 26 },
    { 1, 0x08000000, /* PM27_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 27 },
    { 1, 0x10000000, /* PM28_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 28 },
    { 1, 0x20000000, /* PM29_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 29 },
    { 1, 0x40000000, /* PM30_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 30 },
    { 1, 0x80000000, /* PM31_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_clp_ser_info, 31 },
    { 2, 0x00000001, /* PM32_INTR */
      SOC_BLK_XLPORT, 1, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_td3_pm_xlp_ser_info, 0 },
    { 0 } /* table terminator */
};

static _soc_generic_ser_info_t _soc_td3_tcam_ser_info_template[] = {
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { MY_STATION_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { MY_STATION_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* Global view */
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    /* Global view */
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { PHB_SELECT_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { PHB_SELECT_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { FLEX_RTAG7_HASH_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { FLEX_RTAG7_HASH_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { PKT_FLOW_SELECT_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { PKT_FLOW_SELECT_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { PKT_FLOW_SELECT_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { PKT_FLOW_SELECT_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { PKT_FLOW_SELECT_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { PKT_FLOW_SELECT_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { FORWARDING_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { FORWARDING_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { FORWARDING_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { FORWARDING_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { FORWARDING_3_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { FORWARDING_3_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ZONE_0_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ZONE_0_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ZONE_1_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ZONE_1_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ZONE_2_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ZONE_2_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ZONE_3_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ZONE_3_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ZONE_4_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ZONE_4_EDITOR_CONTROL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_PKT_FLOW_SELECT_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_PKT_FLOW_SELECT_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_QOS_CTRL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_QOS_CTRL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER0_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER0_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_3m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_3m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_HME_STAGE_TCAM_ONLY_4m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_4m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_MICE_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_MICE_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER1_MICE_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER1_MICE_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_HME_STAGE_TCAM_ONLY_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_HME_STAGE_TCAM_ONLY_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_HME_STAGE_TCAM_ONLY_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_HME_STAGE_TCAM_ONLY_4m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_HME_STAGE_TCAM_ONLY_4m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_MICE_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_MICE_TCAM_0m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

    { IP_PARSER2_MICE_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IP_PARSER2_MICE_TCAM_1m, INVALIDm, _SOC_SER_TYPE_PARITY, 0, 0,
      { {0, 0}, {0, 0}, {0, 0}, {0, 0} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },

      /* End of list */
    { INVALIDm },
};

static soc_mem_t _soc_td3_skip_populating[] = {
    /* Shared MMU_WRED_ENABLE_ECCP_MEM.PROFILE_ECCP_EN */
    MMU_WRED_DROP_CURVE_PROFILE_0m,
    MMU_WRED_DROP_CURVE_PROFILE_0_Am,
    MMU_WRED_DROP_CURVE_PROFILE_0_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_1m,
    MMU_WRED_DROP_CURVE_PROFILE_1_Am,
    MMU_WRED_DROP_CURVE_PROFILE_1_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_2m,
    MMU_WRED_DROP_CURVE_PROFILE_2_Am,
    MMU_WRED_DROP_CURVE_PROFILE_2_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_3m,
    MMU_WRED_DROP_CURVE_PROFILE_3_Am,
    MMU_WRED_DROP_CURVE_PROFILE_3_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_4m,
    MMU_WRED_DROP_CURVE_PROFILE_4_Am,
    MMU_WRED_DROP_CURVE_PROFILE_4_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_5m,
    MMU_WRED_DROP_CURVE_PROFILE_5_Am,
    MMU_WRED_DROP_CURVE_PROFILE_5_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_6m,
    MMU_WRED_DROP_CURVE_PROFILE_6_Am,
    MMU_WRED_DROP_CURVE_PROFILE_6_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_7m,
    MMU_WRED_DROP_CURVE_PROFILE_7_Am,
    MMU_WRED_DROP_CURVE_PROFILE_7_Bm,
    MMU_WRED_DROP_CURVE_PROFILE_8m,
    MMU_WRED_DROP_CURVE_PROFILE_8_Am,
    MMU_WRED_DROP_CURVE_PROFILE_8_Bm,

    /* Shared MMU_SCFG_PARITY_EN.TDM_PARITY_EN*/
    TDM_CALENDAR0m,
    TDM_CALENDAR1m,
    INVALIDm
};

static int _soc_td3_tcam_mode_info[SOC_MAX_NUM_DEVICES][7];
static _soc_generic_ser_info_t *_soc_td3_tcam_ser_info[SOC_MAX_NUM_DEVICES];

STATIC soc_error_t _soc_td3_ser_tcam_control_reg_get(int unit, soc_mem_t mem,
                  soc_reg_t   *ser_control_reg, soc_field_t *ser_enable_field);

int
soc_trident3_tcam_mode_set(int unit, int stage, int mode)
{
    if (stage < 0 || stage >=7) {
        if (stage == -1) {
            return SOC_E_NONE;
        }
        return SOC_E_PARAM;
    }

    _soc_td3_tcam_mode_info[unit][stage] = mode;

    return SOC_E_NONE;
}

int
soc_trident3_tcam_mode_get(int unit, soc_mem_t mem, int *mode)
{
	int stage = 0;

    switch(mem) {
        /* Field lookup TCAMs and SRAMs */
        case VFP_TCAMm:
        case VFP_POLICY_TABLEm:
            stage = _BCM_FIELD_STAGE_LOOKUP;
            break;

        /* Field Egress TCAMs and SRAMs */
        case EFP_TCAMm:
        case EFP_POLICY_TABLEm:
        case EFP_COUNTER_TABLEm:
        case EFP_METER_TABLEm:
            stage = _BCM_FIELD_STAGE_EGRESS;
            break;

        /* Field Ingress TCAMs */
        case IFP_LOGICAL_TABLE_SELECTm:
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
        case IFP_TCAMm:
        /* Field Ingress SRAMs */
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case IFP_LOGICAL_TABLE_ACTION_PRIORITYm:
        case IFP_KEY_GEN_PROGRAM_PROFILEm:
        case IFP_KEY_GEN_PROGRAM_PROFILE2m:
        case IFP_POLICY_TABLEm:
        case IFP_METER_TABLEm:
            stage = _BCM_FIELD_STAGE_INGRESS;
            break;

        /* Exact Match TCAMs */
        case EXACT_MATCH_LOGICAL_TABLE_SELECTm:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
        /* Exact Match SRAMs */
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm:
        case EXACT_MATCH_ACTION_PROFILEm:
        case EXACT_MATCH_QOS_ACTIONS_PROFILEm:
        case EXACT_MATCH_DEFAULT_POLICYm:
        case EXACT_MATCH_KEY_GEN_MASKm:
        case EXACT_MATCH_2m:
        case EXACT_MATCH_4m:
        case EXACT_MATCH_2_ENTRY_ONLYm:
        case EXACT_MATCH_4_ENTRY_ONLYm:
            stage = _BCM_FIELD_STAGE_EXACTMATCH;
            break;

        /* Field Ingress Compression TCAMs and SRAMs */
        case SRC_COMPRESSIONm:
        case SRC_COMPRESSION_TCAM_ONLYm:
        case SRC_COMPRESSION_DATA_ONLYm:
        case DST_COMPRESSIONm:
        case DST_COMPRESSION_TCAM_ONLYm:
        case DST_COMPRESSION_DATA_ONLYm:
            stage = _BCM_FIELD_STAGE_CLASS;
            break;
        case TTL_FNm:
        case TOS_FNm:
        case TCP_FNm:
        case IP_PROTO_MAPm:
            stage = _BCM_FIELD_STAGE_CLASS;
            break;
        case IFP_TCAM_WIDEm:
            if (soc_property_get(unit, spn_FIELD_ATOMIC_UPDATE, FALSE) == TRUE) {
                stage = _BCM_FIELD_STAGE_INGRESS;
            } else {
            /*
            *  Inorder to support bcmFieldQualifyInPorts in Global mode,
            *  IP_TCAM_WIDE should always be in pipe local mode.
            */
                *mode = _SOC_SER_MEM_MODE_PIPE_UNIQUE;
                return SOC_E_NONE;
            }
            break;

        default:
            return SOC_E_NOT_FOUND;
    }

    *mode = _soc_td3_tcam_mode_info[unit][stage];
    return SOC_E_NONE;
}

int
soc_trident3_tcam_ser_init(int unit)
{
    int alloc_size = 0;

    /* First, make per-unit copy of the master TCAM list */
    if (!soc_feature(unit, soc_feature_tcam_scan_engine)) {
        alloc_size = sizeof(_soc_td3_tcam_ser_info_template);

        if (NULL == _soc_td3_tcam_ser_info[unit]) {
            if ((_soc_td3_tcam_ser_info[unit] =
                 sal_alloc(alloc_size, "th tcam list")) == NULL) {
                return SOC_E_MEMORY;
            }
        }

        /* Make a fresh copy of the TCAM template info */
        sal_memcpy(_soc_td3_tcam_ser_info[unit],
                   &(_soc_td3_tcam_ser_info_template),
                   alloc_size);
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_mmu_ser_config(int unit, _soc_td3_ser_info_t *info_list,
                             int enable)
{
    int info_index, f, rv;
    soc_field_t *fields;
    uint32 rval;
    _soc_td3_ser_info_t *info;

    switch(info_list->type) {
    case _SOC_TD3_PARITY_TYPE_MMU_SER:
        /* Interrupt enable */
        fields = info_list->intr_enable_field_list;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                          &rval));
        for (f=0 ;; f++) {
            if (fields[f] == INVALIDf) {
                break;
            }
            soc_reg_field_set(unit, info_list->intr_enable_reg, &rval,
                              fields[f], enable ? 1 : 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                           rval));
        /* Loop through each info entry in the list */
        for (info_index = 0; ; info_index++) {
            info = &(info_list->info[info_index]);
            if (info->type == _SOC_TD3_PARITY_TYPE_NONE) {
                /* End of table */
                break;
            }
            rv = _soc_trident3_mmu_ser_config(unit, info, enable);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Error configuring %s !!\n"),
                         info->mem_str));
                return rv;
            }
        }
        break;
    case _SOC_TD3_PARITY_TYPE_MMU_GLB:
    case _SOC_TD3_PARITY_TYPE_MMU_XPE:
    case _SOC_TD3_PARITY_TYPE_MMU_SC:
    case _SOC_TD3_PARITY_TYPE_MMU_SED:
        /* Parity enable */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->enable_reg, REG_PORT_ANY, 0,
             &rval));
        if (info_list->enable_field_list) {
            fields = info_list->enable_field_list;
            for (f=0 ;; f++) {
                if (fields[f] == INVALIDf) {
                    break;
                }
                soc_reg_field_set(unit, info_list->enable_reg, &rval, fields[f],
                                  enable ? 1 : 0);
            }
        } else if (info_list->enable_reg != INVALIDr) {
            soc_reg_field_set(unit, info_list->enable_reg, &rval,
                              info_list->enable_field, enable ? 1 : 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, info_list->enable_reg, REG_PORT_ANY, 0, rval));
        /* Interrupt enable */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                           &rval));
        if (info_list->intr_enable_field_list) {
            fields = info_list->intr_enable_field_list;
            for (f=0 ;; f++) {
                if (fields[f] == INVALIDf) {
                    break;
                }
                soc_reg_field_set(unit, info_list->intr_enable_reg, &rval,
                                  fields[f], enable ? 1 : 0);
            }
        } else if (info_list->intr_enable_reg != INVALIDr) {
            soc_reg_field_set(unit, info_list->intr_enable_reg, &rval,
                              info_list->intr_enable_field, enable ? 1 : 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                           rval));
        break;
    default: break;
    }
    return SOC_E_NONE;
}

int
soc_td3_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                        soc_reg_t group_reg, uint64 *group_rval,
                        _soc_td3_ser_info_t *info_list,
                        soc_mem_t mem, int enable)
{
    _soc_td3_ser_info_t *info;
    int info_index, rv, rv1;
    uint32 rval;
    soc_reg_t reg;
    uint64 rval64;
    int log_port;
    rv = SOC_E_NOT_FOUND;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_TD3_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        rv = SOC_E_NONE;

        if (group_reg != INVALIDr && info->group_reg_enable_field != INVALIDf) {
            /* Enable the info entry in the group register */
            soc_reg64_field32_set(unit, group_reg, group_rval,
                                  info->group_reg_enable_field, enable ? 1 : 0);
        }
        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_TD3_PARITY_TYPE_MMU_SER:
            rv1 = _soc_trident3_mmu_ser_config(unit, info, enable);
            if (SOC_FAILURE(rv1)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Error configuring MMU SER !!\n")));
                return rv1;
            }
            break;
        case _SOC_TD3_PARITY_TYPE_ECC:
        case _SOC_TD3_PARITY_TYPE_PARITY:
            reg = info->enable_reg;
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, info->enable_field,
                                      enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                  enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "SER enable for: %s\n"),
                         (info->mem == INVALIDm) ? info->mem_str :
                         SOC_MEM_NAME(unit, info->mem)));
            break;
        case _SOC_TD3_PARITY_TYPE_CLMAC:
        case _SOC_TD3_PARITY_TYPE_XLMAC:
            reg = info->enable_reg;
            /* At this moment, port mapping info is not ready on PCID server */
            if (SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM) {
                break;
            }
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
                if (SOC_REG_IS_64(unit, reg)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, log_port, 0, &rval64));
                    soc_reg64_field32_set(unit, reg, &rval64, info->enable_field,
                                          enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, log_port, 0, rval64));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, log_port, 0, &rval));
                    soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                      enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, log_port, 0, rval));
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "SER enable for: %s\n"),
                         (info->mem == INVALIDm) ? info->mem_str :
                         SOC_MEM_NAME(unit, info->mem)));
            break;
        default: break;
        }
    }
    return rv;
}

STATIC int
_soc_trident3_ser_block_enable(int unit, int enable, uint32 *p_cmic_bit,
                              int type, soc_block_t blocktype,
                              soc_reg_t fifo_reset_reg, void *info)
{
    uint8                           rbi;
    uint16                          pcount;
    uint32                          cmic_bit, rval;
    int                             port = REG_PORT_ANY;
    uint64                          rval64;
    soc_reg_t                       reg, ecc1b_reg;
    soc_field_t                     field, ecc1b_field;
    int                             field_position = -1;
    int                             ecc1b_field_position = -1;
    uint8                           flag = 0;
    uint8                           ecc1b_flag = 0;
    uint8                           skip = 0;
    uint32                          value;
    _soc_reg_ser_en_info_t          *reg_info;
    _soc_mem_ser_en_info_t          *mem_info;
    _soc_bus_ser_en_info_t          *bus_info;
    _soc_buffer_ser_en_info_t       *buf_info;
    char                            *str_type;
    char                            *str_name;
    static char *mmu_base_type[8] = {
        "IPORT", "EPORT", "IPIPE", "EPIPE", "CHIP", "XPE", "SLICE", "LAYER"};
    static char *parity_module_str[4] = {"REG", "MEM", "BUS", "BUF"};
    const _soc_td3_ser_route_block_t *rb;

    int done = 0;

    reg_info = (_soc_reg_ser_en_info_t*)info;
    mem_info = (_soc_mem_ser_en_info_t*)info;
    bus_info = (_soc_bus_ser_en_info_t*)info;
    buf_info = (_soc_buffer_ser_en_info_t*)info;
    for (pcount = 0;;pcount++) { /* walk thro each entry of block_info */
        ecc1b_reg = INVALIDr;
        ecc1b_field = INVALIDf;
        switch (type) {
        case _SOC_SER_TYPE_REG:
            if (reg_info[pcount].reg == INVALIDr) {
                done = 1;
            } else {
                reg = reg_info[pcount].en_ctrl.ctrl_type.en_reg;
                flag = reg_info[pcount].en_ctrl.flag_reg_mem;
                field = reg_info[pcount].en_ctrl.en_fld;
                field_position = reg_info[pcount].en_ctrl.en_fld_position;
                ecc1b_reg = reg_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
                ecc1b_flag = reg_info[pcount].ecc1b_ctrl.flag_reg_mem;
                ecc1b_field = reg_info[pcount].ecc1b_ctrl.en_fld;
                ecc1b_field_position = reg_info[pcount].ecc1b_ctrl.en_fld_position;
                str_name = SOC_REG_NAME(unit, reg_info[pcount].reg);
            }
            break;
        case _SOC_SER_TYPE_MEM:
            if (mem_info[pcount].mem == INVALIDm) {
                done = 1;
            } else if (!SOC_MEM_IS_VALID(unit, mem_info[pcount].mem) ||
                       (SOC_MEM_INFO(unit, mem_info[pcount].mem).flags &
                        SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "skip SER enable for mem %s\n"),
                    SOC_MEM_NAME(unit, mem_info[pcount].mem)));
                continue;
            } else {
                reg = mem_info[pcount].en_ctrl.ctrl_type.en_reg;
                flag = mem_info[pcount].en_ctrl.flag_reg_mem;
                field = mem_info[pcount].en_ctrl.en_fld;
                field_position = mem_info[pcount].en_ctrl.en_fld_position;
                ecc1b_reg = mem_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
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
                str_name = bus_info[pcount].bus_name;
            }
            break;
        case _SOC_SER_TYPE_BUF:
            if (buf_info[pcount].en_reg == INVALIDr) {
                done = 1;
            } else {
                reg = buf_info[pcount].en_reg;
                field = buf_info[pcount].en_fld;
                str_name = buf_info[pcount].buffer_name;

                if (reg == IDB_SER_CONTROLr && field == IS_TDM_ECC_ENf) {
                    skip = 1;
                } else {
                    skip = 0;
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

        if (SOC_REG_IS_VALID(unit, reg) && (flag == 0) && !skip) {
            /* NOTE: Do not use the field modify routine in the following as
                     some regs do not return the correct value due to which the
                     modify routine skips the write */
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                if (field_position == -1) {
                    soc_reg64_field32_set(unit, reg, &rval64, field,
                                          enable ? 1 : 0);
                } else {
                    value = soc_reg64_field32_get(unit, reg, rval64, field);
                    if (enable) {
                        value |= (1 << field_position);
                    } else {
                        value &= ~(1 << field_position);
                    }
                    soc_reg64_field32_set(unit, reg, &rval64, field, value);
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                /* int at, bt; */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                if (field_position == -1) {
                    soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
                } else {
                    value = soc_reg_field_get(unit, reg, rval, field);
                    if (enable) {
                        value |= (1 << field_position);
                    } else {
                        value &= ~(1 << field_position);
                    }
                    soc_reg_field_set(unit, reg, &rval, field, value);
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
        }
        if (ecc1b_reg != INVALIDr && ecc1b_field != INVALIDf &&
            SOC_REG_IS_VALID(unit, ecc1b_reg) && (ecc1b_flag == 0)) {
            if (ecc1b_reg == ISW3_SER_CONTROL_1r &&
                ecc1b_field == HG_COUNTERS_EN_COR_ERR_RPTf) {
                continue;
            }
            if (SOC_REG_IS_64(unit, ecc1b_reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, ecc1b_reg, port, 0, &rval64));
                soc_reg64_field32_set(unit, ecc1b_reg, &rval64, ecc1b_field,
                                      enable ? 1 : 0);
                if (ecc1b_field_position == -1) {
                    soc_reg64_field32_set(unit, ecc1b_reg, &rval64, ecc1b_field,
                                          enable ? 1 : 0);
                } else {
                    value = soc_reg64_field32_get(unit, ecc1b_reg, rval64,
                                                  ecc1b_field);
                    if (enable) {
                        value |= (1 << ecc1b_field_position);
                    } else {
                        value &= ~(1 << ecc1b_field_position);
                    }
                    soc_reg64_field32_set(unit, ecc1b_reg, &rval64, ecc1b_field,
                                          value);
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, ecc1b_reg, port, 0, rval64));
            } else {
                int at, bt;
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, ecc1b_reg, port, 0, &rval));
                if (ecc1b_field_position == -1) {
                    soc_reg_field_set(unit, ecc1b_reg, &rval, ecc1b_field,
                                      enable ? 1 : 0);
                } else {
                    value = soc_reg_field_get(unit, ecc1b_reg, rval, ecc1b_field);
                    if (enable) {
                        value |= (1 << ecc1b_field_position);
                    } else {
                        value &= ~(1 << ecc1b_field_position);
                    }
                    soc_reg_field_set(unit, ecc1b_reg, &rval, ecc1b_field, value);
                }                at = SOC_REG_ACC_TYPE(unit, ecc1b_reg);
                bt = SOC_REG_BASE_TYPE(unit, ecc1b_reg);
                if (20 == at) { /* ACC_TYPE_SINGLE */
                    int inst, pipe, max_pipe;
                    switch (bt) {
                    case 2: /* IPIPE */
                    case 3: /* EPIPE */
                        max_pipe = NUM_PIPE(unit); break;
                    case 4: /* CHIP */
                        max_pipe = 1; break;
                    case 5: /* XPE */
                        max_pipe = NUM_XPE(unit); break;
                    case 6: /* SLICE */
                        max_pipe = NUM_SLICE(unit); break;
                    case 7: /* LAYER */
                        max_pipe = NUM_LAYER(unit); break;
                    default:
                        /* 0: IPORT */
                        /* 1: EPORT */
                        max_pipe = -1;
                        LOG_ERROR(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "unit %d, reg %s has unexpected "
                                        "base_type %s, will skip enable of "
                                        "1b err reporting\n"),
                             unit, SOC_REG_NAME(unit, ecc1b_reg),
                             mmu_base_type[bt]));
                        break;
                    }
                    for (pipe = 0; pipe < max_pipe; pipe++) {
                        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, ecc1b_reg, inst, 0, rval));
                    }
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, ecc1b_reg, port, 0, rval));
                }
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "SER enable for %s: %s\n"),
                     str_type, str_name));
    } /* walk thro each entry of block_info */

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_td3_ser_route_blocks[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (rb->blocktype == blocktype) {
            /* LHS has only SOC_BLK_MMU and * RHS has only
             * SOC_BLK_MMU_GLB. Thus for MMU, following will not be
             * executed.
             */
            /* New SER mechanism (except IP.IDB) */
            *p_cmic_bit |= cmic_bit;
            /* block_info has entries only for BLK_IPIPE, BLK_EPIPE,
             * BLK_MMU_GLB.
             * route_block_info on other hand has BLK_MMU, BLK_IPIPE,
             * BLK_EPIPE, BLK_CLPORT.
             * So, we can he here only for BLK_IPIPE, BLK_EPIPE and for
             * these cmic reg is always cmic_rval - see block_info[] array
             */
            if (rb->enable_reg != INVALIDr &&
                rb->enable_field != INVALIDf) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, rb->enable_reg,
                        REG_PORT_ANY, rb->enable_field, enable ? 1 : 0));
            }
            if ((rb->blocktype == SOC_BLK_IPIPE)
                && (rb->cmic_bit != _SOC_TD3_IPIPE0_IDB_INTR)
                && (rb->cmic_bit != _SOC_TD3_IPIPE1_IDB_INTR)
                && (rb->cmic_bit != _SOC_TD3_CEV_INTR)) {
                soc_td3_ip_pipe_fifo_bmask[unit][rb->pipe] |= cmic_bit;
            }
        }
    }

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
soc_trident3_ser_enable_parity_table(int unit, int type, void *info, int enable)
{
    uint32       i = 0;
    uint32      entry[SOC_MAX_MEM_WORDS];
    uint32      copyno;
    uint32      value = 0;
    uint32      done = 0;
    uint8       flag = 0;
    soc_mem_t   en_mem = INVALIDm;
    soc_mem_t   ecc1b_en_mem = INVALIDm;
    soc_field_t en_fld = INVALIDf;
    soc_field_t ecc1b_en_fld = INVALIDf;
    int         en_fld_position = -1;
    int         ecc1b_en_fld_position = -1;
    const _soc_mem_ser_en_info_t *mem_info;
    const _soc_reg_ser_en_info_t *reg_info;

    mem_info = (_soc_mem_ser_en_info_t*)info;
    reg_info = (_soc_reg_ser_en_info_t*)info;

    for (i = 0; ; i++) {
        switch(type) {
            case _SOC_SER_TYPE_REG:
                if (reg_info[i].reg == INVALIDr) {
                    done = 1;
                }
                en_mem = reg_info[i].en_ctrl.ctrl_type.en_mem;
                flag = reg_info[i].en_ctrl.flag_reg_mem;
                en_fld = reg_info[i].en_ctrl.en_fld;
                en_fld_position = reg_info[i].en_ctrl.en_fld_position;
                ecc1b_en_mem = reg_info[i].ecc1b_ctrl.ctrl_type.en_mem;
                ecc1b_en_fld = reg_info[i].ecc1b_ctrl.en_fld;
                ecc1b_en_fld_position = reg_info[i].ecc1b_ctrl.en_fld_position;
                break;
            case _SOC_SER_TYPE_MEM:
                if (mem_info[i].mem == INVALIDm) {
                    done = 1;
                }
                en_mem = mem_info[i].en_ctrl.ctrl_type.en_mem;
                flag = mem_info[i].en_ctrl.flag_reg_mem;
                en_fld = mem_info[i].en_ctrl.en_fld;
                en_fld_position = mem_info[i].en_ctrl.en_fld_position;
                ecc1b_en_mem = mem_info[i].ecc1b_ctrl.ctrl_type.en_mem;
                ecc1b_en_fld = mem_info[i].ecc1b_ctrl.en_fld;
                ecc1b_en_fld_position = mem_info[i].ecc1b_ctrl.en_fld_position;
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Unknown parity module "
                                    "[type: %d].\n"), type));
                break;
        }

        if (done == 1) {
            break;
        }

        if (flag == 0) {
            continue;
        }

        SOC_MEM_BLOCK_ITER(unit, en_mem, copyno) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, en_mem, copyno, 0, entry));
            if (en_fld_position == -1) {
                soc_mem_field32_set(unit, en_mem, entry, en_fld, enable ? 1 : 0);
            } else {
                value = soc_mem_field32_get(unit, en_mem, entry, en_fld);
                if (enable) {
                    value |= (1 << en_fld_position);
                } else {
                    value &= ~(1 << en_fld_position);
                }
                soc_mem_field32_set(unit, en_mem, entry, en_fld, value);
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, en_mem, copyno, 0, entry));
        }

        if (ecc1b_en_mem != INVALIDm) {
            SOC_MEM_BLOCK_ITER(unit, ecc1b_en_mem, copyno) {
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, ecc1b_en_mem, copyno,
                                                 0, entry));
                if (ecc1b_en_fld_position == -1) {
                    soc_mem_field32_set(unit, ecc1b_en_mem, entry,
                                        ecc1b_en_fld, enable ? 1 : 0);
                } else {
                    value = soc_mem_field32_get(unit, ecc1b_en_mem,
                                                entry, ecc1b_en_fld);
                    if (enable) {
                        value |= (1 << ecc1b_en_fld_position);
                    } else {
                        value &= ~(1 << ecc1b_en_fld_position);
                    }
                    soc_mem_field32_set(unit, ecc1b_en_mem, entry,
                                        ecc1b_en_fld, value);
                }
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, ecc1b_en_mem, copyno,
                                                  0, entry));
            }
        }
    }

    return SOC_E_NONE;
}

int
soc_trident3_ser_enable_parity_table_all(int unit, int enable)
{
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_MEM, (void*) SOC_IP_MEM_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_REG, (void*) SOC_IP_REG_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_MEM, (void*) SOC_EP_MEM_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_REG, (void*) SOC_EP_REG_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_MEM, (void*) SOC_MMU_MEM_SER_INFO(unit), enable));

    return SOC_E_NONE;
}


int
soc_trident3_ser_enable_all(int unit, int enable)
{
    uint8                           rbi;
    uint32                          cmic_bit;
    uint32                          cmic_rval0;
    uint32                          cmic_rval1;
    uint32                          cmic_rval2;
    int                             port = REG_PORT_ANY;
    int                             rv, block_info_idx;
    uint64                          rval64;
    const _soc_td3_ser_route_block_t *rb;

    soc_iproc_getreg(unit,soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                     &cmic_rval0);
    soc_iproc_getreg(unit,soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r, REG_PORT_ANY, 0),
                     &cmic_rval1);
    soc_iproc_getreg(unit,soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG2r, REG_PORT_ANY, 0),
                     &cmic_rval2);

    soc_td3_ip_pipe_fifo_bmask[unit][0] = 0;
    soc_td3_ip_pipe_fifo_bmask[unit][1] = 0;

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_REG, SOC_BLK_IPIPE, INVALIDr,
        (void*) SOC_IP_REG_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_MEM, SOC_BLK_IPIPE, ING_SER_FIFO_CTRLr,
        (void*) SOC_IP_MEM_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_REG, SOC_BLK_EPIPE, INVALIDr,
        (void*) SOC_EP_REG_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_MEM, SOC_BLK_EPIPE, EGR_SER_FIFO_CTRLr,
        (void*) SOC_EP_MEM_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_MEM, SOC_BLK_MMU_GLB, INVALIDr,
        (void*) SOC_MMU_MEM_SER_INFO(unit)));

#if defined(BCM_56870_A0)
    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_BUS, SOC_BLK_IPIPE, INVALIDr,
        (void*) &_soc_bcm56870_a0_ip_bus_ser_info));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_BUF, SOC_BLK_IPIPE, INVALIDr,
        (void*) &_soc_bcm56870_a0_ip_buffer_ser_info));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_BUS, SOC_BLK_EPIPE, INVALIDr,
        (void*) &_soc_bcm56870_a0_ep_bus_ser_info));

    SOC_IF_ERROR_RETURN(_soc_trident3_ser_block_enable(unit, enable, &cmic_rval0,
        _SOC_SER_TYPE_BUF, SOC_BLK_EPIPE, INVALIDr,
        (void*) &_soc_bcm56870_a0_ep_buffer_ser_info));
#endif

    /* Enable 1B error reporting for some special items */

    /* Loop through each place-and-route block entry to enable legacy
       style SER stuff */
    /* BLK_MMU, BLK_CLPORT will be handled here */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_td3_ser_route_blocks[rbi];

        port = REG_PORT_ANY;
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        if (rb->info == NULL) {
            continue;
        }

        if (enable) {
            switch (rb->cmic_reg) {
                case 0: cmic_rval0 |= cmic_bit; break;
                case 1: cmic_rval1 |= cmic_bit; break;
                case 2: cmic_rval2 |= cmic_bit; break;
                default: break;
            }
        } else {
            switch (rb->cmic_reg) {
                case 0: cmic_rval0 &= ~cmic_bit; break;
                case 1: cmic_rval1 &= ~cmic_bit; break;
                case 2: cmic_rval2 &= ~cmic_bit; break;
                default: break;
            }
        }
        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }
        if (rb->enable_reg != INVALIDr) {
            if (SOC_BLOCK_IN_LIST(SOC_REG_INFO(unit, rb->enable_reg).block,
                SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                    /* This port block is not configured */
                    continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, port, 0, &rval64));
        }

        rv = soc_td3_ser_enable_info(unit, block_info_idx, rb->id, port,
                                     rb->enable_reg, &rval64,
                                     rb->info, INVALIDm, enable);
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }

        if (rb->enable_reg != INVALIDr) {
            /* Write per route block parity enable register */
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rval64));
        }
    }

    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                     cmic_rval0);
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r, REG_PORT_ANY, 0),
                     cmic_rval1);
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG2r, REG_PORT_ANY, 0),
                     cmic_rval2);
    if (enable) {
        /* MMU enables */
        /* check trident2.c - may have to add some similar code here */

        /* Write CMIC enable register */
        soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
        /* Waiting for HW feedback, so disable tcam scan engine first */
        (void)soc_ser_tcam_scan_engine_enable(unit, FALSE);
        (void)soc_td3_ser_tcam_wrapper_enable(unit, TRUE);
    } else {
        /* MMU disables */
        /* check trident2.c - may have to add some similar code here */

        /* Write CMIC disable register */
        soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
        (void)soc_ser_tcam_scan_engine_enable(unit, FALSE);
        (void)soc_td3_ser_tcam_wrapper_enable(unit, FALSE);
    }
    return SOC_E_NONE;
}

int soc_trident3_pipe_select(int unit, int egress, int pipe);

STATIC void
_soc_td3_ser_control_reg_get(int unit,
                            const _soc_mem_ser_en_info_t *ser_en_info_list,
                            soc_mem_t   mem,
                            soc_reg_t   *ser_control_reg,
                            soc_mem_t   *ser_control_mem,
                            soc_field_t *ser_enable_field,
                            int *ser_enable_field_position)
{
    int i;
    if ((ser_en_info_list == NULL) || (ser_control_reg == NULL) ||
        (ser_enable_field == NULL)) {
        return;
    }
    for (i = 0; ser_en_info_list[i].mem != INVALIDm; i++) {
        if (ser_en_info_list[i].mem == mem) {
            *ser_control_reg = ser_en_info_list[i].en_ctrl.flag_reg_mem ? INVALIDr :
                               ser_en_info_list[i].en_ctrl.ctrl_type.en_reg;
            *ser_control_mem = ser_en_info_list[i].en_ctrl.flag_reg_mem ?
                               ser_en_info_list[i].en_ctrl.ctrl_type.en_mem :
                               INVALIDm;
            *ser_enable_field = ser_en_info_list[i].en_ctrl.en_fld;
            *ser_enable_field_position = ser_en_info_list[i].en_ctrl.en_fld_position;
            return;
        }
    }
    LOG_ERROR(BSL_LS_SOC_SER,
              (BSL_META_U(unit,
                          "unit %d, mem %s reported in ser_fifo but not "
                          "listed in ip, ep_mem_ser_info list !!\n"),
               unit, SOC_MEM_NAME(unit,mem) ));
    return;
}

STATIC void
_soc_td3_mem_parity_info(int unit, int block_info_idx, int pipe,
                        soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | ((pipe & 0xff) << SOC_ERROR_PIPE_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}

STATIC int
_soc_td3_mem_has_ecc(int unit, soc_mem_t mem)
{
    if (SOC_MEM_FIELD_VALID(unit, mem, ECCf) ||
        SOC_MEM_FIELD_VALID(unit, mem, ECC0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, ECC_0f) ||
        (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ECC_CORRECTABLE)) {
        return 1;
    }
        return 0;
}

STATIC int
_soc_td3_mem_is_dyn(int unit, soc_mem_t mem)
{
    int rv = 0; /* mem is not dynamic */
/*
 *
    if (SOC_MEM_FIELD_VALID(unit, mem, HITf) ||
        SOC_MEM_FIELD_VALID(unit, mem, HIT0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HIT_0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITDA_0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITSA_0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITSAf) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITDAf) ||
        SOC_MEM_FIELD_VALID(unit, mem, MY_STATION_HITf)) {
        return 1;
    }
 */
    switch (mem) {
    case IFP_METER_TABLEm:
    case EFP_METER_TABLEm:
    case FP_STORM_CONTROL_METERSm:
        rv = 1; break;
    default:
        /*
         * Cannot use following, because FLAG_DYNAMIC will be set by esw/drv.c
         * for any mem which contains PARITY, ECC, HIT bits
         * if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_DYNAMIC) {
         *     rv = 1; break;
         * }
         */
        if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ENTRY_CLEAR) ||
            !(SOC_MEM_SER_CORRECTION_TYPE(unit, mem))) {
            /* 1st term means mem is dynamic - eg: counter */
            /* 2nd term means sw cannot restore this - so mem must be dyn */
            rv = 1;
        } else {
            rv = 0;
        }
        break;
    }
    if (rv) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, mem %s is dynamic - so read of corrupted"
                        " entry will be skipped !!\n"),
             unit, SOC_MEM_NAME(unit, mem)));
    }
    return rv;
}

int
soc_td3_parity_bit_enable(int unit, soc_reg_t enreg, soc_mem_t enmem,
                          soc_field_t enfld, int enfld_position, int enable)
{
    uint32 rval = 0;
    uint32 value = 0;
    uint32      entry[SOC_MAX_MEM_WORDS];
    uint32      copyno;
    uint64 rval64;

    if (SOC_REG_IS_VALID(unit, enreg)) {
        if (SOC_REG_IS_64(unit, enreg)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, enreg, REG_PORT_ANY, 0, &rval64));
            if (enfld_position == -1) {
                soc_reg64_field32_set(unit, enreg, &rval64, enfld, enable ? 1 : 0);
            } else {
                value = soc_reg64_field32_get(unit, enreg, rval64, enfld);
                if (enable) {
                    value |= (1 << enfld_position);
                } else {
                    value &= ~(1 << enfld_position);
                }
                soc_reg64_field32_set(unit, enreg, &rval64, enfld, value);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, enreg, REG_PORT_ANY, 0, rval64));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, enreg, REG_PORT_ANY, 0, &rval));
            if (enfld_position == -1) {
                soc_reg_field_set(unit, enreg, &rval, enfld, enable ? 1 : 0);
            } else {
                value = soc_reg_field_get(unit, enreg, rval, enfld);
                if (enable) {
                    value |= (1 << enfld_position);
                } else {
                    value &= ~(1 << enfld_position);
                }
                soc_reg_field_set(unit, enreg, &rval, enfld, value);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, enreg, REG_PORT_ANY, 0, rval));
        }
    } else if (SOC_MEM_IS_VALID(unit, enmem)){
        SOC_MEM_BLOCK_ITER(unit, enmem, copyno) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, enmem, copyno, 0, entry));
            if (enfld_position == -1) {
                soc_mem_field32_set(unit, enmem, entry, enfld, enable ? 1 : 0);
            } else {
                value = soc_mem_field32_get(unit, enmem, entry, enfld);
                if (enable) {
                    value |= (1 << enfld_position);
                } else {
                    value &= ~(1 << enfld_position);
                }
                soc_mem_field32_set(unit, enmem, entry, enfld, value);
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, enmem, copyno, 0, entry));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_td3_mem_log_content_skip(int unit, soc_mem_t mem){
    soc_mem_t *mem_list = NULL;
    int i = 0;

    mem_list = _soc_td3_skip_populating;

    i = 0;
    while(mem_list[i] != INVALIDm) {
        if (mem == mem_list[i]) {
            return TRUE;
        }
        i++;
    }

    return FALSE;
}

int
_soc_td3_populate_ser_log(int unit, soc_reg_t parity_enable_reg,
                         soc_mem_t parity_enable_mem,
                         soc_field_t parity_enable_field,
                         int parity_enable_field_position,
                         soc_mem_t mem,
                         int mem_block,
                         int pipe_num,
                         int index,
                         sal_usecs_t detect_time,
                         uint32 sblk,
                         uint32 address,
                         int disable_parity,
                         int disable_mem_read,
                         int disable_fill_cache_log,
                         int force_cache_log,
                         int is_physical_index,
                         uint32 hwbase)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS];
    int log_entry_size, id, entry_dw;
    uint32 *cache;
    uint32 flags = SOC_MEM_NO_FLAGS;
    uint8 *vmap;
    soc_ser_log_tlv_memory_t log_mem;
    soc_ser_log_tlv_generic_t log_generic;
    int final_disable_parity = 0;
    int final_disable_mem_read = 0;
    int rv;

    sal_memset(&log_generic, 0, sizeof(log_generic));
    sal_memset(&log_mem, 0, sizeof(log_mem));
    sal_memset(tmp_entry, 0, sizeof(tmp_entry));

    /*
    must be large enough for at least generic and terminator, as well as the
    memory type since we might decode it in soc_ser_correction.
    */
    log_entry_size = sizeof(soc_ser_log_tlv_hdr_t)*3 +
                     sizeof(soc_ser_log_tlv_generic_t) +
                     sizeof(soc_ser_log_tlv_memory_t);

    if (mem != INVALIDm) {
        final_disable_mem_read = disable_mem_read ||
                                 _soc_td3_mem_is_dyn(unit, mem) ||
                                 _soc_td3_mem_log_content_skip(unit, mem) ||
                                 ((parity_enable_reg == INVALIDr) &&
                                 (parity_enable_mem == INVALIDm)) ||
                                 (parity_enable_field == INVALIDf);

        final_disable_parity = (!final_disable_mem_read) &&
                               disable_parity; /* over-ride if necessary */

        /* Check to make sure this isn't a duplicate */
        /* Search for a log entry with the same mem, and index with within
         * the last 5 seconds */
        rv = soc_ser_log_find_recent(unit, mem, index, sal_time_usecs());
        if (rv > 0) {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "unit %d, ser_logging was skipped for mem %s "
                            "- too close to similar event !!\n"),
                 unit, SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        entry_dw = soc_mem_entry_words(unit, mem);
        log_entry_size += sizeof(soc_ser_log_tlv_hdr_t) + entry_dw*4;

        if (sblk) {
            SOC_MEM_BLOCK_ITER(unit, mem, mem_block) {
                if (SOC_BLOCK2OFFSET(unit, mem_block) == sblk) {
                    break;
                }
            }
        } else {
            mem_block = SOC_MEM_BLOCK_ANY(unit, mem);
        }

        cache = SOC_MEM_STATE(unit, mem).cache[mem_block];
        vmap = SOC_MEM_STATE(unit, mem).vmap[mem_block];
        if ((cache != NULL && CACHE_VMAP_TST(vmap, index)) ||
            force_cache_log) {
            log_entry_size += sizeof(soc_ser_log_tlv_hdr_t) + entry_dw*4;
        }

        /* create the entry based on determined size, save id */
        id = soc_ser_log_create_entry(unit, log_entry_size);
        if (id == 0) {
            return id;
        }

        /* Add the memory information to the log now so we can detect duplicate errors */
        log_generic.time = detect_time;
        log_mem.memory = mem;
        log_mem.index = index;
        soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_MEMORY,
                            sizeof(soc_ser_log_tlv_memory_t), &log_mem);
        soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_GENERIC,
                            sizeof(soc_ser_log_tlv_generic_t), &log_generic);

        /* If we have decoded the memory we can record its contents
         * (which can help with analysis with nature of the error
         */
        if (final_disable_parity) {
            SOC_IF_ERROR_RETURN(soc_td3_parity_bit_enable(unit, parity_enable_reg,
                                        parity_enable_mem,
                                        parity_enable_field,
                                        parity_enable_field_position, 0));
        }

        /* read the memory into a buffer */
        if (!final_disable_mem_read) {
            if (is_physical_index) {
                flags = SOC_MEM_DONT_MAP_INDEX;
            }
            if ((pipe_num >= 0) && (pipe_num < NUM_PIPE(unit))) {
                soc_mem_pipe_select_read(unit, flags, mem, mem_block,
                                         pipe_num, index, tmp_entry);
            } else {
                flags |= SOC_MEM_DONT_USE_CACHE | SOC_MEM_SCHAN_ERR_RETURN;
                /*Enable NACK on read */
                soc_mem_read_extended(unit,
                                      flags,
                                      mem, 0, mem_block,
                                      index, tmp_entry);
            }
        }

        /* Re_enable Parity */
        if (final_disable_parity) {
            SOC_IF_ERROR_RETURN(soc_td3_parity_bit_enable(unit, parity_enable_reg,
                                        parity_enable_mem,
                                        parity_enable_field,
                                        parity_enable_field_position, 1));

        }

        /* fill in the memory contents tlv */
        if (!final_disable_mem_read) {
            if (soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_CONTENTS,
                entry_dw*4, tmp_entry) < 0) {
                return 0;
            }
        }

        if (cache != NULL && CACHE_VMAP_TST(vmap, index) &&
            !disable_fill_cache_log) {
            /* fill in the memory cache tlv */
            if (soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_CACHE,
                entry_dw*4, (cache + index*entry_dw)) < 0) {
                return 0;
            }
        }
    } else {
        /* create the entry based on determined size, save id */
        id = soc_ser_log_create_entry(unit, log_entry_size);
        if (id == 0) {
            return id;
        }
        log_mem.memory = mem;
        log_mem.index = index;
        log_mem.hwbase = hwbase;
        soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_MEMORY,
                            sizeof(soc_ser_log_tlv_memory_t), &log_mem);
    }
    return id;
}

STATIC int
_soc_td3_add_log_generic(int unit, _soc_ser_correct_info_t *spci)
{

    if (spci->log_id != 0) {
        soc_ser_log_tlv_generic_t log_generic;

        /* Add the memory information to the log now so we can detect duplicate errors */
        log_generic.flags = 0;
        if (spci->flags & SOC_SER_ERR_CPU) {
            log_generic.flags |= SOC_SER_LOG_FLAG_ERR_SRC;
        }
        if (spci->flags & SOC_SER_ERR_MULTI) {
            log_generic.flags |= SOC_SER_LOG_FLAG_MULTIBIT;
        }
        if (spci->double_bit != 0) {
            log_generic.flags |= SOC_SER_LOG_FLAG_DOUBLEBIT;
        }
        log_generic.boot_count = soc_ser_log_get_boot_count(unit);
        log_generic.address = spci->addr;
        log_generic.acc_type = spci->acc_type;
        log_generic.block_type = spci->blk_type;
        log_generic.parity_type = spci->parity_type;
        log_generic.ser_response_flag = 0;
        log_generic.corrected = SOC_SER_UNCORRECTED;
        log_generic.pipe_num = spci->pipe_num;
        log_generic.time = spci->detect_time;

        soc_ser_log_add_tlv(unit, spci->log_id, SOC_SER_LOG_TLV_GENERIC,
            sizeof(soc_ser_log_tlv_generic_t), &log_generic);

        return SOC_E_NONE;
    }
    return SOC_E_FAIL;
}

STATIC int
_soc_td3_ser_reg32_get(int unit, soc_reg_t reg, int port, int index,
                      uint32 *data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
       return soc_trident3_xpe_reg32_get(unit, reg, block_num,
                                         base_type_index, index, data);
    case SOC_BLK_MMU_SC:
       return soc_trident3_sc_reg32_get(unit, reg, block_num,
                                        base_type_index, index, data);
   case SOC_BLK_MMU_SED:
      return soc_trident3_sed_reg32_get(unit, reg, block_num,
                                       base_type_index, index, data);
    default:
       return soc_reg32_get(unit, reg, port, index, data);
    }
}

STATIC int
_soc_td3_ser_reg32_set(int unit, soc_reg_t reg, int port, int index,
                      uint32 data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
       return soc_trident3_xpe_reg32_set(unit, reg, block_num,
                                         base_type_index, index, data);
    case SOC_BLK_MMU_SC:
       return soc_trident3_sc_reg32_set(unit, reg, block_num,
                                        base_type_index, index, data);
   case SOC_BLK_MMU_SED:
      return soc_trident3_sed_reg32_set(unit, reg, block_num,
                                       base_type_index, index, data);
    default:
       return soc_reg32_set(unit, reg, port, index, data);
    }
}

int
soc_td3_ser_reg_field32_modify(int unit, soc_reg_t reg, soc_port_t port,
                               soc_field_t field, uint32 value,
                               int index, int mmu_base_index)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN
        (_soc_td3_ser_reg32_get(unit, reg, port, index, &rval, mmu_base_index));
    soc_reg_field_set(unit, reg, &rval, field, value);
    SOC_IF_ERROR_RETURN
        (_soc_td3_ser_reg32_set(unit, reg, port, index, rval, mmu_base_index));
    return SOC_E_NONE;
}

STATIC int
_soc_trident3_ser_mmu_mem_remap(_soc_ser_correct_info_t *spci)
{
    if (spci == NULL) {
        return SOC_E_PARAM;
    }

    /* LHS views are physical views of mem instances which encounter error and
     * are thus reported by HW.
     *
     * RHS views are logical views for which SW maintains cache and does
     * ser_correction writes.
     *
     * remap function thus maps physical mem views to logical mem views.
     */

    switch (spci->mem) {
    /* WRED */
    case MMU_WRED_DROP_CURVE_PROFILE_0_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_0_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_1_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_1_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_1m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_2_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_2_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_2m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_3_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_3_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_3m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_4_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_4_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_4m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_5_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_5_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_5m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_6_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_6_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_6m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_7_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_7_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_7m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_8_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_8_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_8m; break;


    /* THDU */
    case MMU_THDU_RESUME_PORT0_PIPE0m:
    case MMU_THDU_RESUME_PORT1_PIPE0m:
    case MMU_THDU_RESUME_PORT2_PIPE0m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE0m; break;
    case MMU_THDU_RESUME_PORT0_PIPE1m:
    case MMU_THDU_RESUME_PORT1_PIPE1m:
    case MMU_THDU_RESUME_PORT2_PIPE1m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE1m; break;

    case MMU_THDU_CONFIG_PORT0_PIPE0m:
    case MMU_THDU_CONFIG_PORT1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE0m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE1m:
    case MMU_THDU_CONFIG_PORT1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE1m; break;

    case MMU_THDU_CONFIG_QGROUP0_PIPE0m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE0m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE1m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE1m; break;

    case MMU_THDU_OFFSET_QGROUP0_PIPE0m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE0m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE0m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE1m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE1m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE1m; break;

    case MMU_THDU_CONFIG_QUEUE0_PIPE0m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE0m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE1m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE1m; break;

    case MMU_THDU_OFFSET_QUEUE0_PIPE0m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE0m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE0m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE1m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE1m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE1m; break;

    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE0m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE0m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE0m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE0m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE1m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE1m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE1m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE1m; break;


    /* THDI */
    case THDI_PORT_SP_CONFIG0_PIPE0m:
    case THDI_PORT_SP_CONFIG1_PIPE0m:
    case THDI_PORT_SP_CONFIG2_PIPE0m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE0m; break;
    case THDI_PORT_SP_CONFIG0_PIPE1m:
    case THDI_PORT_SP_CONFIG1_PIPE1m:
    case THDI_PORT_SP_CONFIG2_PIPE1m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE1m; break;


    /* MTRO */
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_Am:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_Bm:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE0m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE0m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE1m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE1m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m; break;
    case MMU_MTRO_L0_MEM_Am:
    case MMU_MTRO_L0_MEM_Bm:
        spci->mem = MMU_MTRO_L0_MEMm; break;
    case MMU_MTRO_L1_MEM_Am:
    case MMU_MTRO_L1_MEM_Bm:
        spci->mem = MMU_MTRO_L1_MEMm; break;

    /* THDM */
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE1m; break;

    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE0m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE0m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE0m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE1m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE1m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE1m; break;

    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE1m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE1m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE1m; break;

    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE0m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE0m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE1m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE1m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE1m; break;

    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE0m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE1m; break;

    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE0m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE0m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE0m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE0m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE1m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE1m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE1m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE1m; break;

    case TCB_THRESHOLD_PROFILE_MAP_A_XPE0m:
    case TCB_THRESHOLD_PROFILE_MAP_B_XPE0m:
        spci->mem = TCB_THRESHOLD_PROFILE_MAP_XPE0m; break;

    /* RQE */
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m:
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m:
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m; break;

    default:
        break;
    }
    return SOC_E_NONE;
}

/* correction routine for MMU_WRED_CONFIG mem */
STATIC int
_soc_td3_mmu_wred_correction(int unit, _soc_ser_correct_info_t *spci,
                         uint32 *addr, int *second_pass)
{
#if 0
    if ((spci == NULL) || (addr == NULL) || (second_pass == NULL)) {
        return SOC_E_PARAM;
    }
    switch (spci->mem) {
    case MMU_WRED_CONFIG_PIPE0m:
        spci->mem = MMU_WRED_CONFIG_PIPE1m;
        spci->mem_reported = MMU_WRED_CONFIG_PIPE1m;
        *addr &= _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_MMU_ADDR_SEG1;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    default:
        *second_pass = 0;
        break;
    }
#endif
    return SOC_E_NONE;
}

STATIC int
_soc_trident3_ser_process_mmu_err(int unit, int block_info_idx,
                                  const _soc_td3_ser_info_t *info_list,
                                  char *prefix_str, int mmu_base_index)
{
    int rv, type = 0, multi = 0;
    uint32 addr = 0, rval, rval_fifo_status, rval_intr_status_reg;
    uint32 non_ser_intr_status_mask;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_reg_t fifo_status_reg, mem_fail_ctr_reg;
    soc_reg_t parity_enable_reg = INVALIDr;
    soc_mem_t parity_enable_mem = INVALIDm;
    soc_field_t parity_enable_field = INVALIDf;
    soc_field_t *fields_stat = NULL;
    int parity_enable_field_position = -1;
    soc_mem_t ser_fifo_mem;
    soc_block_t blocktype = SOC_BLK_NONE;
    uint8 blk_idx;
    uint32 sblk = 0;
    _soc_ser_correct_info_t spci;
    int second_pass = 0;
    int i;
    int f, parity_error = 0;
    static int acc_type[] = {_SOC_ACC_TYPE_PIPE_ANY, _SOC_ACC_TYPE_PIPE_ANY};
    soc_stat_t *stat = SOC_STAT(unit);

    switch(info_list->type) {
    case _SOC_TD3_PARITY_TYPE_MMU_SER: {
        /* When we are here, we have been called by soc_td3_ser_process.
         * info_list points to 1st entry in _soc_td3_mmu_top_ser_info[].
         * Goal is to find src_sub_block in mmu: MMU_GLB, XPE0,1,2,3, SC0,1 */
        _soc_td3_ser_info_t *info = NULL;

        /* find src of interrupt in mmu */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_status_reg, REG_PORT_ANY, 0,
                           &rval)); /* status_reg has BT,AT = CHIP,SINGLE */
        for (i = 0; mmu_intr_info[i].int_statf != INVALIDf; i++) {
            if (soc_reg_field_get(unit, info_list->intr_status_reg, rval,
                                  mmu_intr_info[i].int_statf)) {
#define MMU_INTR_SRC(i)\
                ((i) == 0? "MMU_GLB" : \
                 (i) == 1? "MMU_XPE" : \
                 (i) == 2? "MMU_SC" : \
                 (i) == 3? "MMU_SED" : \
                 "MMU_UNNOWN")
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "intr_status_reg = 0x%08x, "
                                "int_statf = %d is set, mmu_intr_info_idx "
                                "= %d (%s) !!\n"),
                     rval, mmu_intr_info[i].int_statf, i, MMU_INTR_SRC(i)));
                info = &(info_list->info[mmu_intr_info[i].ser_info_index]);
                    /* 'info' now points to entry info_index[i]
                     * in _soc_td3_mmu_ser_info[] */
                rv = _soc_trident3_ser_process_mmu_err(unit, block_info_idx,
                                                       info, prefix_str,
                                                       mmu_intr_info[i].mmu_base_index);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "process_mmu_err: Error processing %s !!\n"),
                             info->mem_str));
                    return rv;
                }
            }
        }
        if (info == NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "In MMU SER processing with no error bit set "
                                "0x%08x !!\n"), rval));
            return SOC_E_NONE;
        }
    }   break;
    case _SOC_TD3_PARITY_TYPE_MMU_GLB:
    case _SOC_TD3_PARITY_TYPE_MMU_XPE:
    case _SOC_TD3_PARITY_TYPE_MMU_SC: 
    case _SOC_TD3_PARITY_TYPE_MMU_SED: {
        /* we are here when we were recursively called by process_mmu_ser itself
         * and info_list is pointing to one of the entries in
         * _soc_td3_mmu_ser_info[]
         */
        switch(info_list->type) {
        case _SOC_TD3_PARITY_TYPE_MMU_GLB: /* all of these regs,mem have BT,AT = CHIP,SINGLE */
            fifo_status_reg = MMU_GCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = MMU_GCFG_MEM_FAIL_ADDR_64m;
            mem_fail_ctr_reg = MMU_GCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_GLB;
            acc_type[0] = _SOC_ACC_TYPE_PIPE_ANY;
            non_ser_intr_status_mask = ~0x1;
                /* MMU_GCFG_GLB_CPU_INT_STAT has only MEM_PAR_ERR_STATf */
            break;
        case _SOC_TD3_PARITY_TYPE_MMU_XPE: /* all of these regs,mem have BT,AT = XPE,SINGLE */
            fifo_status_reg = MMU_XCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = MMU_XCFG_MEM_FAIL_ADDR_64_XPE0m;
            mem_fail_ctr_reg = MMU_XCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_XPE;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x1;
                /* bit 0 in MMU_XCFG_XPE_CPU_INT_STAT is MEM_PAR_ERR_STATf */
            break;
        case _SOC_TD3_PARITY_TYPE_MMU_SC: /* all of these regs,mem have BT,AT = SLICE,SINGLE */
            fifo_status_reg = MMU_SCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = MMU_SCFG_MEM_FAIL_ADDR_64_SC0m;
            mem_fail_ctr_reg = MMU_SCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_SC;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x1;
                /* bit 0 in MMU_SCFG_SC_CPU_INT_STAT is MEM_PAR_ERR_STATf */
            break;
        case _SOC_TD3_PARITY_TYPE_MMU_SED:
            fifo_status_reg = MMU_SEDCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = MMU_SEDCFG_MEM_FAIL_ADDR_64m;
            mem_fail_ctr_reg = MMU_SEDCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_SED;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x2;
            /* bit 1 in MMU_SEDCFG_SED_CPU_INT_STAT is MEM_PAR_ERR_STATf */
            break;
        default:
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN
            (_soc_td3_ser_reg32_get(unit, info_list->intr_status_reg,
                                   REG_PORT_ANY, 0, &rval_intr_status_reg, mmu_base_index));
        if (info_list->intr_status_field_list) {
            fields_stat = info_list->intr_status_field_list;
            for (f=0 ;; f++) {
                if (fields_stat[f] == INVALIDf) {
                    break;
                }
                if ((fields_stat[f] == MEM_PAR_ERR_STATf) &&
                       soc_reg_field_get(unit, info_list->intr_status_reg,
                                         rval_intr_status_reg, fields_stat[f])) {
                    parity_error = 1;
                }
            }
        } else if (info_list->intr_status_field == MEM_PAR_ERR_STATf) {
            if (soc_reg_field_get(unit, info_list->intr_status_reg,
                                   rval_intr_status_reg, info_list->intr_status_field)) {
                parity_error = 1;
            }
        }

        if (parity_error == 1) {
            /* Now we know that one of the reasons for interrupt from MMU is
             * parity error for sure, so mmu_ser_fifo has to be non-empty */
            SOC_IF_ERROR_RETURN
                (_soc_td3_ser_reg32_get(unit, fifo_status_reg, REG_PORT_ANY, 0,
                                       &rval_fifo_status, mmu_base_index));
            if (soc_reg_field_get(unit, fifo_status_reg, rval_fifo_status, EMPTYf)) {
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "unit %d %s SER interrupt with empty fifo !!\n"),
                           unit, info_list->mem_str));
                SOC_IF_ERROR_RETURN
                    (soc_td3_ser_reg_field32_modify(unit,
                                                    info_list->intr_clr_reg,
                                                    REG_PORT_ANY,
                                                    MEM_PAR_ERR_CLRf,
                                                    1, 0, mmu_base_index));
                return SOC_E_NONE;
            }
            SOC_IF_ERROR_RETURN
                (_soc_td3_ser_reg32_get(unit, mem_fail_ctr_reg, REG_PORT_ANY, 0,
                                       &rval, mmu_base_index));
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "unit %d %s mem error interrupt count: %d\n"),
                       unit, info_list->mem_str, rval));
            SOC_BLOCK_ITER(unit, blk_idx, blocktype) {
                sblk = SOC_BLOCK2SCH(unit, blk_idx);
                break;
            }
            do {
                if (!second_pass) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_pop(unit, ser_fifo_mem, MEM_BLOCK_ANY, entry));
                    /* process entry */
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "%s\n"), info_list->mem_str));
                    multi = soc_mem_field32_get(unit, ser_fifo_mem, entry,
                                                ERR_MULTf);
                    type = soc_mem_field32_get(unit, ser_fifo_mem, entry,
                                               ERR_TYPEf);
                    addr = soc_mem_field32_get(unit, ser_fifo_mem, entry, EADDRf);
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "unit %d %s %s %s error at address 0x%08x\n"),
                               unit, info_list->mem_str, multi ? "multiple" : "",
                               type ? ((type == 1) ? "1bit": "parity/2bit") : "", addr));

                    sal_memset(&spci, 0, sizeof(spci));
                    spci.flags |= SOC_SER_SRC_MEM;
                    if (multi) {
                        spci.flags |= SOC_SER_ERR_MULTI;
                    }
                    if (type != 1) {
                        spci.double_bit = 1;
                    }
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blocktype;
                    spci.pipe_num = -1;
                    spci.sblk = sblk;
                    spci.acc_type = -1;
                    spci.detect_time = sal_time_usecs();
                    spci.addr = addr;
                    parity_enable_reg = INVALIDr;
                    parity_enable_field = INVALIDf;

                    /* Try to decode memory */
                    for (i = 0; ; i++) {
                        spci.mem = soc_addr_to_mem_extended(unit, sblk,
                                                            acc_type[i],
                                                            addr);
                        if (spci.mem != INVALIDm) {
                            LOG_VERBOSE(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "unit %d acc_type = %d "
                                            "got us mem %s \n"),
                                 unit, acc_type[i],
                                 SOC_MEM_NAME(unit,spci.mem)));
                            break;
                        }
                        if (_SOC_ACC_TYPE_PIPE_ANY == acc_type[i]) {
                            break;
                        }
                    }
                    if (spci.mem != INVALIDm) {
                        spci.index = addr - soc_mem_base(unit, spci.mem);
                        spci.flags |= SOC_SER_REG_MEM_KNOWN;
                        spci.flags |= SOC_SER_LOG_WRITE_CACHE;
                            /* ser_correction will fill cache part in ser_log */
                        spci.mem_reported = spci.mem; /* before remap */
                        spci.flags |= SOC_SER_LOG_MEM_REPORTED;
                        SOC_IF_ERROR_RETURN
                            (_soc_trident3_ser_mmu_mem_remap(&spci));
                        spci.parity_type = _soc_td3_mem_has_ecc(unit, spci.mem)?
                                           SOC_PARITY_TYPE_ECC :
                                           SOC_PARITY_TYPE_PARITY;
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "unit %d mem for ser_correction ="
                                        " %s \n"),
                             unit, SOC_MEM_NAME(unit,spci.mem)));
                        _soc_td3_ser_control_reg_get(unit,
                                                    (const _soc_mem_ser_en_info_t *)SOC_MMU_MEM_SER_INFO(unit),
                                                    spci.mem, /* after remap */
                                                    &parity_enable_reg,
                                                    &parity_enable_mem,
                                                    &parity_enable_field,
                                                    &parity_enable_field_position);

                    } else {
                        spci.flags |= SOC_SER_REG_MEM_UNKNOWN;
                        spci.mem_reported = INVALIDm;
                        parity_enable_reg = INVALIDr;
                        parity_enable_field = INVALIDf;
                    }
                } /* !second_pass */
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                   sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                   addr);

                spci.log_id = _soc_td3_populate_ser_log(unit,
                                                       parity_enable_reg,
                                                       parity_enable_mem,
                                                       parity_enable_field,
                                                       parity_enable_field_position,
                                                       spci.mem_reported,
                                                       blk_idx,
                                                       spci.pipe_num,
                                                       spci.index,
                                                       spci.detect_time,
                                                       spci.sblk,
                                                       spci.addr,
                                                       1, /* disable_parity */
                                                       0, /* disable_mem_read */
                                                       0, /* disable_fill_cache_log */
                                                       1, /* force_cache_log */
                                                       FALSE, 0);
                if ((spci.mem != INVALIDm) &&
                    (SOC_MEM_SER_CORRECTION_TYPE(unit, spci.mem) != 0)) {
                    spci.inst = mmu_base_index;

                    if ((SOC_MEM_SER_CORRECTION_TYPE(unit, spci.mem) ==
                         SOC_MEM_FLAG_SER_ENTRY_CLEAR) &&
                        (info_list->type == _SOC_TD3_PARITY_TYPE_MMU_XPE)) {

                        spci.flags |= SOC_SER_ALSO_UPDATE_SW_COUNTER;
                                            /* xpe0, 1, 2, 3 */
                        spci.pipe_num = (spci.addr >> 15) & 0x3;
                                            /* bits 16:15 */
                        switch (spci.mem) {
#if 0 
                        case MMU_CTR_COLOR_DROP_MEM_XPE0_PIPE0m:
                            spci.counter_base_mem = MMU_CTR_COLOR_DROP_MEMm;
                            spci.pipe_num = 0; /* Note */
                            break;
                        case MMU_CTR_COLOR_DROP_MEM_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_CTR_COLOR_DROP_MEMm;
                            spci.pipe_num = 1; /* Note */
                            break;
#endif 
                        case MMU_CTR_ING_DROP_MEM_XPE0_PIPE0m:
                            spci.counter_base_mem = MMU_CTR_ING_DROP_MEMm;
                            spci.pipe_num = 0;
                            break;
                        case MMU_CTR_MC_DROP_MEM_XPE0_PIPE0m:
                        case MMU_CTR_MC_DROP_MEM_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_CTR_MC_DROP_MEMm;
                            break;

                        case MMU_CTR_UC_DROP_MEM_XPE0_PIPE0m:
                        case MMU_CTR_UC_DROP_MEM_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_CTR_UC_DROP_MEMm;
                            break;

                        case MMU_CTR_WRED_DROP_MEM_XPE0_PIPE0m:
                        case MMU_CTR_WRED_DROP_MEM_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_CTR_WRED_DROP_MEMm;
                            break;

                        /* Instantaneous counts */
                        case MMU_THDM_DB_PORTSP_BST_XPE0_PIPE0m:
                        case MMU_THDM_DB_PORTSP_BST_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_THDM_DB_PORTSP_BSTm;
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            break;

                        /* Instantaneous counts */
                        case MMU_THDM_DB_QUEUE_BST_XPE0_PIPE0m:
                        case MMU_THDM_DB_QUEUE_BST_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_THDM_DB_QUEUE_BSTm;
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            break;

                        /* Instantaneous counts */
                        case MMU_THDM_MCQE_PORTSP_BST_XPE0_PIPE0m:
                        case MMU_THDM_MCQE_PORTSP_BST_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_THDM_MCQE_PORTSP_BSTm;
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            break;

                        /* Instantaneous counts */
                        case MMU_THDM_MCQE_QUEUE_BST_XPE0_PIPE0m:
                        case MMU_THDM_MCQE_QUEUE_BST_XPE0_PIPE1m:
                            spci.counter_base_mem = MMU_THDM_MCQE_QUEUE_BSTm;
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            break;

                        /* Not collected by SW */
                        case THDI_PORT_PG_BST_XPE0_PIPE0m:
                            spci.counter_base_mem = THDI_PORT_PG_BSTm;
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            break;

                        /* Not collected by SW */
                        case THDI_PORT_SP_BST_XPE0_PIPE0m:
                            spci.counter_base_mem = THDI_PORT_SP_BSTm;
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            break;

                        /* Unexpected mems */
                        default:
                            spci.flags &= ~SOC_SER_ALSO_UPDATE_SW_COUNTER;
                                /* use HW only clear */
                            spci.counter_base_mem = INVALIDm;
                            LOG_WARN(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "Mem %s (sbus_addr: 0x%08x), will use hw_only_clear, "
                                "inst %d, pipe %d\n"),
                                SOC_MEM_NAME(unit, spci.mem), addr,
                                spci.inst, spci.pipe_num));
                            break;
                        }
                    }
                    rv = soc_ser_correction(unit, &spci);
                    if (SOC_FAILURE(rv)) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                           sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                           addr);
                        soc_ser_stat_update(unit, 0, spci.blk_type,
                                            spci.parity_type,
                                            spci.double_bit,
                                            SocSerCorrectTypeFailedToCorrect,
                                            stat);
                        /* Dont 'return' here - as it will skip clearing of
                         * interrupt and thus will result in processing of empty
                         * ser fifo */
                    }
                } else if (spci.mem == INVALIDm) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                       sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       addr);
                    soc_ser_stat_update(unit, 0, spci.blk_type,
                                        (type == 1) ? SOC_PARITY_TYPE_ECC :
                                        SOC_PARITY_TYPE_PARITY,
                                        spci.double_bit,
                                        SocSerCorrectTypeNoAction,
                                        stat);
                    LOG_ERROR(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "unit %d %s address 0x%08x: decoding of address to mem FAILED !!\n"),
                               unit, info_list->mem_str, addr));
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_FATAL,
                                       INVALIDm, 
                                       spci.index); 
                    /* Dont 'return' here - as it will skip clearing of
                     * interrupt and thus will result in processing of empty
                     * ser fifo */
                } else {
                    rv = SOC_E_NONE;
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
                                       sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       addr);
                    soc_ser_stat_update(unit, 0, spci.blk_type,
                                        (type == 1) ? SOC_PARITY_TYPE_ECC :
                                        SOC_PARITY_TYPE_PARITY,
                                        spci.double_bit,
                                        SocSerCorrectTypeNoAction,
                                        stat);
                }

                if (spci.log_id != 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                       spci.log_id, 0);
                    if (soc_property_get(unit, "ser_log_print_one", 0)) {
                        soc_ser_log_print_one(unit, spci.log_id);
                    }
                }

                if (second_pass) {
                    /* already in second_pass, move to next fifo entry */
                    SOC_IF_ERROR_RETURN
                        (_soc_td3_ser_reg32_get(unit, fifo_status_reg,
                                               REG_PORT_ANY, 0,
                                               &rval_fifo_status,
                                               mmu_base_index));
                    second_pass = 0;
                        /* start next fifo entry with second_pass=0 */
                } else { /* in first_pass */
                    SOC_IF_ERROR_RETURN
                        (_soc_td3_mmu_wred_correction(unit, &spci, &addr,
                                                  &second_pass));
                    if (!second_pass) {
                        /* second_pass not needed */
                        SOC_IF_ERROR_RETURN
                            (_soc_td3_ser_reg32_get(unit, fifo_status_reg,
                                                   REG_PORT_ANY, 0,
                                                   &rval_fifo_status,
                                                   mmu_base_index));
                    }
                    /* else, second_pass is needed, don't read next fifo entry
                     * yet */
                }
            } while (!soc_reg_field_get(unit, fifo_status_reg, rval_fifo_status, EMPTYf));
            SOC_IF_ERROR_RETURN
                (soc_td3_ser_reg_field32_modify(unit, info_list->intr_clr_reg,
                                                REG_PORT_ANY,
                                                MEM_PAR_ERR_CLRf, 1,
                                                0, mmu_base_index));
        }

        /* non_ser mmu interrupts in same status reg are asserted */
        if (rval_intr_status_reg & non_ser_intr_status_mask) {

            rv = soc_td3_mmu_non_ser_intr_handler(unit,
                                                 blocktype,
                                                    /* can be SOC_BLK_MMU_XPE, SOC_BLK_MMU_SED */
                                                 mmu_base_index,
                                                    /* specifies xpe0 in case of SOC_BLK_MMU_XPE */
                                                    /* specifies sc0 in case of SOC_BLK_MMU_SC */
                                                 rval_intr_status_reg &
                                                 non_ser_intr_status_mask);
                                                    /* value of intr_status_reg */
        }
    }   break;
    default:
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_ser_process_ecc(int unit, int block_info_idx, int pipe, int port,
                              const _soc_td3_ser_info_t *info,
                              char *prefix_str, char *mem_str,
                              soc_block_t blocktype, int idb_scan_thread)
{
    _soc_td3_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg, parity_enable_reg;
    soc_field_t ecc_field = ECC_ERRf, parity_enable_field;
    uint32 rval, minfo;
    uint32 entry_idx, idx, has_error;
    uint32 multiple = 0, double_bit = 0;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci;
    int rv;
    soc_stat_t *stat = SOC_STAT(unit);
    uint64 rval64;
    /*
     * info can be
     *      _soc_td3_pm_clp_ser_info[] for which info->mem is always INVALIDm
     *      _soc_td3_pm_xlp_ser_info[] for which info->mem is always INVALIDm
     *      _soc_td3_idb_ser_info[] for which info->mem may not be INVALIDm
     */
    if (info->intr_status_reg != INVALIDr) {
        reg_entry[0].reg = info->intr_status_reg;
        reg_entry[0].mem_str = NULL;
        reg_entry[1].reg = INVALIDr;
        reg_ptr = reg_entry;
    } else if (info->intr_status_reg_list != NULL) {
        reg_ptr = info->intr_status_reg_list;
    } else {
        return SOC_E_NONE;
    }

    if (INVALIDf != info->intr_status_field) {
        ecc_field = info->intr_status_field;
    }

    has_error = FALSE;
    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        reg = reg_ptr[idx].reg;
        if ((blocktype == SOC_BLK_IPIPE || blocktype == SOC_BLK_EPIPE) &&
            (SOC_REG_UNIQUE_ACC(unit, reg) != NULL)) {
               reg = SOC_REG_UNIQUE_ACC(unit, reg)[pipe];
        }
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ?
            reg_ptr[idx].mem_str : mem_str;

        sal_memset(&spci, 0, sizeof(spci));

        rv = soc_reg32_get(unit, reg, port, 0, &rval);
        if (SOC_FAILURE(rv)) {
            return rv;
        }

        /* LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "idb_status_reg %s, = 0x%x\n"), SOC_REG_NAME(unit, reg), rval)); */
        if (soc_reg_field_get(unit, reg, rval, ecc_field)) {
            has_error = TRUE;
            if (reg == CENTRAL_CTR_EVICTION_INTR_STATUSr) {
                rv = soc_reg32_get(unit, CENTRAL_CTR_EVICTION_FIFO_PARITY_ERRORr,
                                   port, 0, &rval);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                entry_idx = soc_reg_field_get(unit,
                                              CENTRAL_CTR_EVICTION_FIFO_PARITY_ERRORr,
                                              rval, MEMINDEXf);
            } else {
                multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
                double_bit = soc_reg_field_get(unit, reg, rval, DOUBLE_BIT_ERRf);
                entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            }
            _soc_td3_mem_parity_info(unit, block_info_idx, pipe,
                                    info->group_reg_status_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                               SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx, minfo);
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "idb_status_reg %s, = 0x%x\n"),
                 SOC_REG_NAME(unit, reg), rval));
            if (double_bit) {
                /* Entry Idx:
                 * 0-15  Port 0
                 * 16-31 Port 1
                 * 32-47 Port 2
                 * 48-63 Port 3
                 * In case of double-bit error we need to reset MIB counters
                 * for that port to recover
                 *
                 * Currently only PORT MIB errors are marked _SOC_TD3_PARITY_TYPE_ECC
                 */
                rval = 0x1 << ((entry_idx & 63) / 16);
                if (blocktype == SOC_BLK_XLPORT)  {
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, XLPORT_MIB_RESETr, port, 0, rval));
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, XLPORT_MIB_RESETr, port, 0, 0x0));
                } else if (blocktype == SOC_BLK_CLPORT) {
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, CLPORT_MIB_RESETr, port, 0, rval));
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, CLPORT_MIB_RESETr, port, 0, 0x0));
                }

                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "%s %s (pipe %0d) entry %d double-bit ECC error\n"),
                           prefix_str, mem_str_ptr, pipe, entry_idx));
            } else {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "%s %s (pipe %0d) entry %d ECC error\n"),
                           prefix_str, mem_str_ptr, pipe, entry_idx));
            }
            if (multiple) {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "%s %s (pipe %0d) has multiple ECC errors\n"),
                           prefix_str, mem_str_ptr, pipe));
            }

            if (IDB_IS_TDM_CAL_ECC_STATUSr == reg_ptr[idx].reg) {
                if (double_bit) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FATAL,
                                   INVALIDm, 
                                   entry_idx); 
                }
            }

            spci.flags = SOC_SER_SRC_MEM;
            if (multiple) {
                spci.flags |= SOC_SER_ERR_MULTI;
            }
            spci.double_bit = double_bit;
            spci.reg = INVALIDr;
            if (reg_ptr[idx].reg == IDB_IS_TDM_CAL_ECC_STATUSr) {
                if (entry_idx >= soc_mem_index_count(unit, IS_TDM_CALENDAR0m)) {
                    /* entry_idx shared between IS_TDM_CALENDAR0/1m */
                    spci.mem = IS_TDM_CALENDAR1m;
                    entry_idx -= soc_mem_index_count(unit, IS_TDM_CALENDAR0m);
                } else {
                    spci.mem = IS_TDM_CALENDAR0m;
                }
                /* programming on each pipe can be different */
                if (SOC_MEM_UNIQUE_ACC(unit, spci.mem) != NULL) {
                    spci.mem = SOC_MEM_UNIQUE_ACC(unit,
                                                  spci.mem)[pipe];
                }
                parity_enable_reg = IDB_SER_CONTROLr;
                parity_enable_field = IS_TDM_ECC_ENf;
            } else {
                spci.mem = INVALIDm; /* non sbus-accessible */
                parity_enable_reg = INVALIDr;
                parity_enable_field = INVALIDf;
            }
            spci.blk_type = blocktype;
            spci.index = entry_idx;
            spci.parity_type = SOC_PARITY_TYPE_ECC;
            spci.detect_time = sal_time_usecs();
            spci.pipe_num = pipe;
            spci.acc_type = -1;
            spci.addr = (spci.mem != INVALIDm)? SOC_MEM_BASE(unit, spci.mem) : 0;
            if (spci.mem != INVALIDm) {
                spci.flags |= SOC_SER_REG_MEM_KNOWN;
                spci.flags |= SOC_SER_LOG_WRITE_CACHE;
            }
            if (!idb_scan_thread) {
                spci.log_id = _soc_td3_populate_ser_log(unit,
                                                       parity_enable_reg,
                                                       INVALIDm,
                                                       parity_enable_field,
                                                       -1,
                                                       spci.mem,
                                                       (spci.mem != INVALIDm) ? SOC_MEM_BLOCK_ANY(unit, spci.mem) : 0,
                                                       spci.pipe_num,
                                                       spci.index,
                                                       spci.detect_time,
                                                       spci.sblk, /* 0 */
                                                       spci.addr,
                                                       1, /* disable_parity */
                                                       0, /* disable_mem_read */
                                                       0, /* disable_fill_cache_log */
                                                       1, /* force_cache_log */
                                                       FALSE, 0);
            } /* for idb_scan_thread, spci.log_id will be 0 */
            if (spci.mem != INVALIDm) {
                rv = soc_ser_correction(unit, &spci);
                if (spci.log_id != 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                       spci.log_id, 0);
                    if (soc_property_get(unit, "ser_log_print_one", 0)) {
                        soc_ser_log_print_one(unit, spci.log_id);
                    }
                }
                if (SOC_FAILURE(rv)) {
                    /* Add reporting failed to correct event flag to
                     * application */
                    soc_event_generate(unit,
                            SOC_SWITCH_EVENT_PARITY_ERROR,
                            SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                            spci.sblk | (spci.pipe_num << SOC_SER_ERROR_PIPE_BP) |
                            SOC_SER_ERROR_DATA_BLK_ADDR_SET, spci.addr);
                    soc_ser_stat_update(unit, 0, spci.blk_type,
                                        spci.parity_type,
                                        spci.double_bit,
                                        SocSerCorrectTypeFailedToCorrect,
                                        stat);
                    return rv;
                }
            } else {
                soc_ser_stat_update(unit, 0, spci.blk_type,
                                    spci.parity_type,
                                    spci.double_bit,
                                    SocSerCorrectTypeNoAction,
                                    stat);
            }
            if ((spci.mem == INVALIDm) && (spci.log_id != 0)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                   spci.log_id, 0);
                rv = _soc_td3_add_log_generic(unit, &spci);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                if (soc_property_get(unit, "ser_log_print_one", 0)) {
                    soc_ser_log_print_one(unit, spci.log_id);
                }
            }

            if (spci.mem == INVALIDm) {
                /* Add reporting failed to correct event flag to application */
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                        SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx, minfo);
            }

            /* Clear parity status */
            rv = soc_reg32_set(unit, reg, port, 0, 0);
            if (SOC_FAILURE(rv)) {
                return rv;
            }

            if ((info->intr_clr_reg != INVALIDr) && (INVALIDf != info->intr_clr_field)) {
                COMPILER_64_ZERO(rval64);
                /* write 1 to clear */
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval64, info->intr_clr_field, 1);
                rv = soc_reg_set(unit, info->intr_clr_reg, port, 0, rval64);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
            }
        }
    }

    if (!has_error && !idb_scan_thread) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "%s %s ECC hardware inconsistency (pipe %d)\n"),
                   prefix_str, mem_str, pipe));
    }
    return SOC_E_NONE;
}

int
soc_td3_scan_idb_mem_ecc_status(int unit)
{
    int pipe;
    int block_info_idx;
    int port = REG_PORT_ANY;
    char prefix_str[10];
    char *mem_str = "IP IDB memory";
    soc_block_t blocktype = SOC_BLK_IPIPE;
    static const _soc_td3_ser_info_t *info = &_soc_td3_idb_ser_info[0];
                                        /* only OBM queue buffer */
    int idb_scan_thread = 1;
    /* Scan is needed only for A0 */
    sal_sprintf(prefix_str, "\nUnit: %d ", unit);
    SOC_BLOCK_ITER(unit, block_info_idx, blocktype) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            SOC_IF_ERROR_RETURN(
                _soc_trident3_ser_process_ecc(unit,
                                              block_info_idx,
                                              pipe, port, info,
                                              prefix_str, mem_str,
                                              blocktype, idb_scan_thread));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_trident3_ser_process_parity(int unit, int block_info_idx, int pipe, int port,
                                 const _soc_td3_ser_info_t *info,
                                 char *prefix_str, char *mem_str,
                                 soc_block_t blocktype)
{
    _soc_td3_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    soc_field_t parity_error_field = PARITY_ERRORf;
    uint32 rval, minfo;
    uint32 multiple, entry_idx, idx, has_error;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci;
    soc_stat_t *stat = SOC_STAT(unit);

    /*
     * info can be
     *      _soc_td3_idb_ser_info[] for which info->mem cannot be INVALIDm
     */
    if (info->intr_status_reg != INVALIDr) {
        reg_entry[0].reg = info->intr_status_reg;
        reg_entry[0].mem_str = NULL;
        reg_entry[1].reg = INVALIDr;
        reg_ptr = reg_entry;
    } else if (info->intr_status_reg_list != NULL) {
        reg_ptr = info->intr_status_reg_list;
    } else {
        return SOC_E_NONE;
    }

    if (INVALIDf != info->intr_status_field) {
        parity_error_field = info->intr_status_field;
    }

    has_error = FALSE;
    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        reg = reg_ptr[idx].reg;
        if ((blocktype == SOC_BLK_IPIPE || blocktype == SOC_BLK_EPIPE) &&
            (SOC_REG_UNIQUE_ACC(unit, reg) != NULL)) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[pipe];
        }
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ?
            reg_ptr[idx].mem_str : mem_str;

        sal_memset(&spci, 0, sizeof(spci));

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, port, 0, &rval));
        if (soc_reg_field_get(unit, reg, rval, parity_error_field)) {
            has_error = TRUE;
            multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
            entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            _soc_td3_mem_parity_info(unit, block_info_idx, pipe,
                                    info->group_reg_status_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx, minfo);

            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "idb_status_reg %s, = 0x%x\n"),
                 SOC_REG_NAME(unit, reg), rval));
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "%s %s (pipe %0d) entry %d parity error\n"),
                       prefix_str, mem_str_ptr, pipe, entry_idx));
            if (multiple) {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "%s %s (pipe %0d) has multiple parity errors\n"),
                           prefix_str, mem_str_ptr, pipe));
            }

            spci.flags = SOC_SER_SRC_MEM;
            if (multiple) {
                spci.flags |= SOC_SER_ERR_MULTI;
            }
            spci.double_bit = 0;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            if ((spci.mem != INVALIDm) &&
                (SOC_MEM_UNIQUE_ACC(unit, spci.mem) != NULL)) {
                spci.mem = SOC_MEM_UNIQUE_ACC(unit, spci.mem)[pipe];
            }
            spci.blk_type = blocktype;
            spci.index = entry_idx;
            spci.parity_type = SOC_PARITY_TYPE_PARITY;
            spci.detect_time = sal_time_usecs();
            spci.pipe_num = pipe;
            spci.acc_type = -1;
            spci.addr = (spci.mem != INVALIDm)? SOC_MEM_BASE(unit, spci.mem) : 0;
            if (spci.mem != INVALIDm) {
                spci.flags |= SOC_SER_REG_MEM_KNOWN;
            }
            spci.log_id = _soc_td3_populate_ser_log(unit,
                                                   info->enable_reg,
                                                   INVALIDm,
                                                   info->enable_field,
                                                   -1,
                                                   spci.mem,
                                                   (spci.mem != INVALIDm)? SOC_MEM_BLOCK_ANY(unit, spci.mem) : 0,
                                                   spci.pipe_num,
                                                   spci.index,
                                                   spci.detect_time,
                                                   spci.sblk, /* 0 */
                                                   spci.addr,
                                                   1, /* disable_parity */
                                                   0, /* disable_mem_read */
                                                   0, /* disable_fill_cache_log */
                                                   0, /* force_cache_log */
                                                   FALSE, 0);
            /* for all memories in this list, ser_resonse is NONE */

            /* rv = soc_ser_correction(unit, &spci); */
            soc_ser_stat_update(unit, 0, spci.blk_type,
                                spci.parity_type,
                                spci.double_bit,
                                SocSerCorrectTypeNoAction,
                                stat);

            if (spci.log_id != 0) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                   spci.log_id, 0);
                SOC_IF_ERROR_RETURN(_soc_td3_add_log_generic(unit, &spci));
                if (soc_property_get(unit, "ser_log_print_one", 0)) {
                    soc_ser_log_print_one(unit, spci.log_id);
                }
            }

            /* Add reporting failed to correct event flag to application */
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                    SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                    SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx, minfo);

            /* Clear parity status */
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, port, 0, 0));
        }
    }

   if (!has_error) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "%s %s parity hardware inconsistency (pipe %d)\n"),
             prefix_str, mem_str, pipe));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_ser_process_mac(int unit, int block_info_idx,
                              const _soc_td3_ser_info_t *info,
                              char *prefix_str, char *mem_str, soc_block_t blocktype)
{
    uint32 rval = 0;
    uint64 rval64;
    uint32 has_error = FALSE;
    soc_stat_t *stat = SOC_STAT(unit);
    int log_port;
    int single_bit = 0, double_bit = 0;

    if ((info->intr_status_reg == INVALIDr) || (INVALIDf == info->intr_status_field)) {
        return SOC_E_NONE;
    }

    COMPILER_64_ZERO(rval64);
    PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
        single_bit = 0;
        double_bit = 0;
        if (SOC_REG_IS_64(unit, info->intr_status_reg)) {
            SOC_IF_ERROR_RETURN
                     (soc_reg_get(unit, info->intr_status_reg, log_port, 0, &rval64));
            if (soc_reg64_field32_get(unit, info->intr_status_reg,
                                          rval64, info->intr_status_field)) {
                has_error = TRUE;
                if (info->intr_status_field == SUM_RX_TS_MEM_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_TX_CDC_SINGLE_BIT_ERRf) {
                    single_bit = 1;
                }
                if (info->intr_status_field == SUM_RX_TS_MEM_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_TX_CDC_DOUBLE_BIT_ERRf) {
                    double_bit = 1;
                }
            }
            if ((info->intr_clr_reg != INVALIDr) && (INVALIDf != info->intr_clr_field)) {
                COMPILER_64_ZERO(rval64);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, info->intr_clr_reg, log_port, 0, &rval64));
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval64, info->intr_clr_field,
                                       0);
                SOC_IF_ERROR_RETURN
                      (soc_reg_set(unit, info->intr_clr_reg, log_port, 0, rval64));
            }
        } else {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, info->intr_status_reg, log_port, 0, &rval));
            if (soc_reg_field_get(unit, info->intr_status_reg, rval,
                                  info->intr_status_field)) {
                has_error = TRUE;
                if (info->intr_status_field == SUM_RX_TS_MEM_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_TX_CDC_SINGLE_BIT_ERRf) {
                    single_bit = 1;
                }
                if (info->intr_status_field == SUM_RX_TS_MEM_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_TX_CDC_DOUBLE_BIT_ERRf) {
                    double_bit = 1;
                }
            }
            if ((info->intr_clr_reg != INVALIDr) && (INVALIDf != info->intr_clr_field)) {
                 SOC_IF_ERROR_RETURN(soc_reg32_get(unit, info->intr_clr_reg, log_port, 0, &rval));
                 soc_reg_field_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field,0);
                 SOC_IF_ERROR_RETURN(soc_reg32_set(unit, info->intr_clr_reg, log_port, 0, rval));
            }
        }
        if (single_bit) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED, 0, 0);
        }
        if (double_bit) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
        }
    }

    if (has_error) {
        soc_ser_stat_update(unit, 0, blocktype, SOC_PARITY_TYPE_PARITY,
                            0, SocSerCorrectTypeNoAction, stat);
    }

    if (!has_error) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "%s %s hardware inconsistency\n"),
                   prefix_str, mem_str));
    }
    return SOC_E_NONE;
}

int
soc_td3_ser_process(int unit, int block_info_idx, int inst, int pipe,
                          int port, soc_reg_t group_reg, uint64 group_rval,
                          const _soc_td3_ser_info_t *info_list,
                          soc_block_t blocktype,
                          char *prefix_str)
{
    const _soc_td3_ser_info_t *info;
    int info_index;
    char *mem_str = "INVALIDm";
    /*uint32 minfo;*/

    /* This func (soc_td3_ser_process) is called by
     * _soc_trident3_ser_process_all which passes
     * _soc_td3_mmu_top_ser_info[] as param to this func.
     * And, _soc_td3_mmu_top_ser_info[] list has only one entry with type
     * _SOC_TD3_PARITY_TYPE_MMU_SER and 'info' for that entry pointing to
     * _soc_td3_mmu_ser_info
     */
    for (info_index = 0; ; info_index++) { /* Loop through each info entry in the mmu_top_ser_info list */
        info = &info_list[info_index]; /* LHS points to 1st entry in _soc_td3_mmu_top_ser_info[] */
        if (info->type == _SOC_TD3_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        if ((group_reg != INVALIDr) && (info->group_reg_status_field != INVALIDf))  {
            /* Check status for the info entry in the group register */
            /* for SOC_BLK_MMU, group_reg is INVALIDr */
            if (!soc_reg64_field32_get(unit, group_reg, group_rval,
                                       info->group_reg_status_field)) {
                continue;
            }
        }

        if (info->mem_str) {
            mem_str = info->mem_str; /* "MMU MME PAR" for SOC_BLK_MMU */
        } else if (info->mem != INVALIDm) {
            mem_str = SOC_MEM_NAME(unit, info->mem);
        } else if (info->group_reg_status_field != INVALIDf) {
            mem_str = SOC_FIELD_NAME(unit, info->group_reg_status_field);
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        
        case _SOC_TD3_PARITY_TYPE_MMU_SER:
            /* we can never be here for following 3 types.
             * _soc_td3_mmu_top_ser_info has only one entry with type
             * _SOC_TD3_PARITY_TYPE_MMU_SER
             */
        case _SOC_TD3_PARITY_TYPE_MMU_GLB:
        case _SOC_TD3_PARITY_TYPE_MMU_XPE:
        case _SOC_TD3_PARITY_TYPE_MMU_SC:
        case _SOC_TD3_PARITY_TYPE_MMU_SED:
            SOC_IF_ERROR_RETURN
                (_soc_trident3_ser_process_mmu_err(unit, block_info_idx,
                                                   info, prefix_str, -1));
                /* info points to _soc_td3_mmu_top_ser_info[0] */
            break;
        case _SOC_TD3_PARITY_TYPE_ECC:
            SOC_IF_ERROR_RETURN
                (_soc_trident3_ser_process_ecc(unit, block_info_idx, pipe,
                                               port, info, prefix_str,
                                               mem_str, blocktype, 0));
            break;
        case _SOC_TD3_PARITY_TYPE_PARITY:
            SOC_IF_ERROR_RETURN
                (_soc_trident3_ser_process_parity(unit, block_info_idx, pipe,
                                                  port, info, prefix_str,
                                                  mem_str, blocktype));
            break;
        case _SOC_TD3_PARITY_TYPE_XLMAC:
        case _SOC_TD3_PARITY_TYPE_CLMAC:
            SOC_IF_ERROR_RETURN
                (_soc_trident3_ser_process_mac(unit, block_info_idx, info,
                                                 prefix_str, mem_str, blocktype));
            break;
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the list */

    return SOC_E_NONE;
}

STATIC char *_soc_td3_ser_hwmem_base_info[] = {
    /* from SER_STATUS_BUS format in regsfile */
    "OBM_QUEUE_FIFO - In Idb",                                                     /* 0x0 */
    "OBM_DATA_FIFO - In Idb",                                                      /* 0x1 */
    "CELL_ASSEM_BUFFER - In Idb",                                                  /* 0x2 */
    "CPU_CELL_ASSEM_BUFFER - In Idb",                                              /* 0x3 */
    "LPBK_CELL_ASSEM_BUFFER - In Idb",                                             /* 0x4 */
    "MGMT_CELL_ASSEM_BUFFER - In Idb",                                             /* 0x5 */
    "SOP_EVENT_BUFFER - In Idb",                                                   /* 0x6 */
    "EOP_EVENT_BUFFER - In Idb",                                                   /* 0x7 */
    "INGRESS_PACKET_BUFFER - In Ipars",                                            /* 0x8 */
    "ISW3_EOP_BUFFER_B - In Isw3",                                                 /* 0x9 */
    "BUBBLE_MOP - In Isw2",                                                        /* 0xa */
    "Invalid value",                                                               /* 0xb */
    "Invalid value",                                                               /* 0xc */
    "Invalid value",                                                               /* 0xd */
    "Invalid value",                                                               /* 0xe */
    "Invalid value",                                                               /* 0xf */
    "Invalid value",                                                               /* 0x10 */
    "IPARS1_BUS - PassThrought bus parity check, MEMINDEX is 0",                   /* 0x11 */
    "VXLT1_BUS - PassThrought bus parity check, MEMINDEX is 0",                    /* 0x12 */
    "IVP_BUS - PassThrought bus parity check, MEMINDEX is 0",                      /* 0x13 */
    "IPARS2_BUS - PassThrought bus parity check, MEMINDEX is 0",                   /* 0x14 */
    "VXLT2_BUS - PassThrought bus parity check, MEMINDEX is 0",                    /* 0x15 */
    "IFWD1_BUS - PassThrought bus parity check, MEMINDEX is 0",                    /* 0x16 */
    "IFWD2_BUS - PassThrought bus parity check, MEMINDEX is 0",                    /* 0x17 */
    "IRSEL1_BUS - PassThrought bus parity check, MEMINDEX is 0",                   /* 0x18 */
    "ISW1_BUS - PassThrought bus parity check, MEMINDEX is 0",                     /* 0x19 */
    "IFP_BUS - PassThrought bus parity check, MEMINDEX is 0",                      /* 0x1a */
    "IRSEL2_BUS - PassThrought bus parity check, MEMINDEX is 0",                   /* 0x1b */
    "ISW2_BUS - PassThrought bus parity check, MEMINDEX is 0",                     /* 0x1c */
    "Invalid value",                                                               /* 0x1d */
    "Invalid value",                                                               /* 0x1e */
    "Invalid value",                                                               /* 0x1f */
    "IPARS1_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                       /* 0x20 */
    "IVXLT1_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                       /* 0x21 */
    "IVP_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                          /* 0x22 */
    "IPARS2_PT_BUF - In IPARS2, Passthru buffer for SOP_UNUSED_PT sigs",           /* 0x23 */
    "IVXLT2_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                       /* 0x24 */
    "IFWD1_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                        /* 0x25 */
    "IFWD2_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                        /* 0x26 */
    "IRSEL1_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                       /* 0x27 */
    "IFP_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                          /* 0x28 */
    "IRSEL2_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                       /* 0x29 */
    "ISW2_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                         /* 0x2a */
    "ICFG_ISW3_PT_BUF - PassThrought buffer for ALL_CELLS_ICFG_TO_ISW3_PT_HWY_BUS",/* 0x2b */
    "IRSEL1_FPEM_DELAY_BUFFER - In Irsel1_fp, delay fpem_rst_bus",                 /* 0x2c */
    "FT_PT_BUF - PassThrought buffer for FT learn module",                         /* 0x2d */
    "FT_LEARN_FIFO - In IFWD2, for FT learn module0",                              /* 0x2e */
    "Invalid value",                                                               /* 0x2f */
    "LEARN_FIFO - In L2MM",                                                        /* 0x30 */
    "Invalid value",                                                               /* 0x31 */
    "Invalid value",                                                               /* 0x32 */
    "Invalid value",                                                               /* 0x33 */
    "Invalid value",                                                               /* 0x34 */
    "Invalid value",                                                               /* 0x35 */
    "Invalid value",                                                               /* 0x36 */
    "Invalid value",                                                               /* 0x37 */
    "CENTRAL_EVICTION_FIFO - In CEV",                                              /* 0x38 */
    "IPARS2_PT_BUF1 - In IPARS2, Passthru buffer for SOP_USED_PT sigs",            /* 0x39 */
    "IRSEL1_FLEX_CTR_BUS_FIFO - In IRSEL1, passthru buffer for flex counter bus",  /* 0x3a */
    "IFWD1_NEW_HWY_BUF - PassThrought buffer for SOP_UNUSED_PT",                   /* 0x3b */
    "IFWD2_NEW_HWY_BUF - PassThrought buffer for SOP_UNUSED_PT",                   /* 0x3c */
    "IVXLT2_NEW_HWY_BUF - PassThrought buffer for SOP_UNUSED_PT",                  /* 0x3d */
    "Invalid value",                                                               /* 0x3e */
    "Invalid value",                                                               /* 0x3f */
    "EP_MPB_DATA - In El3",                                                        /* 0x40 */
    "EP_INITBUF - In Ehcpm",                                                       /* 0x41 */
    "CM_DATA_BUFFER - In Edatabuf",                                                /* 0x42 */
    "PM0_DATA_BUFFER - In Edatabuf",                                               /* 0x43 */
    "PM1_DATA_BUFFER - In Edatabuf",                                               /* 0x44 */
    "PM2_DATA_BUFFER - In Edatabuf",                                               /* 0x45 */
    "PM3_DATA_BUFFER - In Edatabuf",                                               /* 0x46 */
    "PM4_DATA_BUFFER - In Edatabuf",                                               /* 0x47 */
    "PM5_DATA_BUFFER - In Edatabuf",                                               /* 0x48 */
    "PM6_DATA_BUFFER - In Edatabuf",                                               /* 0x49 */
    "PM7_DATA_BUFFER - In Edatabuf",                                               /* 0x4a */
    "LBP_DATA_BUFFER - In Edatabuf",                                               /* 0x4b */
    "PM_RESI0_DATA_BUFFER - In Edatabuf",                                          /* 0x4c */
    "PM_RESI1_DATA_BUFFER - In Edatabuf",                                          /* 0x4d */
    "Invalid value",                                                               /* 0x4e */
    "Invalid value",                                                               /* 0x4f */
    "CM_EDB_CTRL_BUFFER - In Edatabuf",                                            /* 0x50 */
    "PM0_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x51 */
    "PM1_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x52 */
    "PM2_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x53 */
    "PM3_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x54 */
    "PM4_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x55 */
    "PM5_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x56 */
    "PM6_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x57 */
    "PM7_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x58 */
    "LBP_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x59 */
    "EGR_VLAN_BUS - PassThrought bus parity check, MEMINDEX is 0",                 /* 0x5a */
    "EGR_PMOD_BUS - PassThrought bus parity check, MEMINDEX is 0",                 /* 0x5b */
    "EFP_BUS - PassThrought bus parity check, MEMINDEX is 0",                      /* 0x5c */
    "Invalid value",                                                               /* 0x5d */
    "EGR_VLAN_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                     /* 0x5e */
    "EGR_PMOD_PT_BUF - PassThrought buffer for SOP_UNUSED_PT",                     /* 0x5f */
    "PM8_DATA_BUFFER - In Edatabuf",                                               /* 0x60 */
    "PM9_DATA_BUFFER - In Edatabuf",                                               /* 0x61 */
    "PM10_DATA_BUFFER - In Edatabuf",                                              /* 0x62 */
    "PM11_DATA_BUFFER - In Edatabuf",                                              /* 0x63 */
    "PM12_DATA_BUFFER - In Edatabuf",                                              /* 0x64 */
    "PM13_DATA_BUFFER - In Edatabuf",                                              /* 0x65 */
    "PM14_DATA_BUFFER - In Edatabuf",                                              /* 0x66 */
    "PM15_DATA_BUFFER - In Edatabuf",                                              /* 0x67 */
    "PM8_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x68 */
    "PM9_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x69 */
    "PM10_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x6a */
    "PM11_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x6b */
    "PM12_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x6c */
    "PM13_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x6d */
    "PM14_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x6e */
    "PM15_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x6f */
    "PM_RESI2_DATA_BUFFER - In Edatabuf",                                          /* 0x70 */
    "PM_RESI3_DATA_BUFFER - In Edatabuf",                                          /* 0x71 */
    "MGP_DATA_BUFFER - In Edatabuf",                                               /* 0x72 */
    "MGP_EDB_CTRL_BUFFER - In Edatabuf",                                           /* 0x73 */
    "Invalid value",                                                               /* 0x74 */
    "EFP_DATA_PT_BUF - In EFP, PassThrough buffer for DATA",                       /* 0x75 */
    "EGR_VLAN_HWY1_BUF - HWY1 buffer for misc EVLAN buses",                        /* 0x76 */
    "EGR_VLAN_HWY2_BUF - HWY2 buffer for misc EVLAN buses",                        /* 0x77 */
    "EGR_EL3_HWY1_BUF - HWY1 buffer for misc EVLAN buses",                         /* 0x78 */
    "EGR_EL3_HWY2_BUF - HWY2 buffer for misc EVLAN buses",                         /* 0x79 */
    "IRSEL1_USED_PT_BUF - PassThrought buffer for Irsel1 SOP_USED_FIELDS buses",   /* 0x7a */
    "Invalid value",                                                               /* 0x7b */
    "Invalid value",                                                               /* 0x7c */
    "Invalid value",                                                               /* 0x7d */
    "Invalid value",                                                               /* 0x7e */
    "Invalid value",                                                               /* 0x7f */
    "PM16_DATA_BUFFER - In Edatabuf",                                              /* 0x80 */
    "PM17_DATA_BUFFER - In Edatabuf",                                              /* 0x81 */
    "PM18_DATA_BUFFER - In Edatabuf",                                              /* 0x82 */
    "PM19_DATA_BUFFER - In Edatabuf",                                              /* 0x83 */
    "PM16_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x84 */
    "PM17_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x85 */
    "PM18_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x86 */
    "PM19_EDB_CTRL_BUFFER - In Edatabuf",                                          /* 0x87 */
    "PM_RESI4_DATA_BUFFER - In Edatabuf",                                          /* 0x88 */
    "Invalid value",                                                               /* 0x89 */
    "Invalid value",                                                               /* 0x8a */
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",                                                               /* 0x90 */
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "BS_KT_CONEXT_BUF - In Broadscan, Key_table context_buffer for key",           /* 0xA0 */
    "BS_DT_PKT_HDR_BUF - In Broadscan, Data_table Packet Header Buffer",           /* 0xA1 */
};

#define _SOC_TD3_SER_REG 1
#define _SOC_TD3_SER_MEM 0

STATIC void
_soc_trident3_print_ser_fifo_details(int unit, uint8 regmem, soc_block_t blk,
                                     uint32 sblk, int pipe, uint32 address,
                                     uint32 stage, uint32 base, uint32 index,
                                     uint32 type, uint8 drop, uint8 non_sbus,
                                     int l2_mgmt_ser_fifo)
{
    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityError, unit)) {
        switch (type) {
        case 0:
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "Error in: SOP cell.\n")));
            break;
        case 1:
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "Error in: MOP cell.\n")));
            break;
        case 2:
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "Error in: EOP cell.\n")));
            break;
        case 3:
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "Error in: SBUS transaction.\n")));
            break;
        case 4:
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "Error in: transaction - refresh, aging etc.\n")));
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "Invalid error reported !!\n")));
        }
        LOG_WARN(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "Blk: %d, Pipe: %d, Address: 0x%08x, base: 0x%x, "
                              "stage: %d, index: %d\n"),
                   sblk, pipe, address, base, stage, index));
        if (regmem == _SOC_TD3_SER_MEM) {
            if (l2_mgmt_ser_fifo) {
                if (non_sbus) {
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "unit %0d: L2 LEARN_FIFO parity "
                                          "error\n"),
                               unit));
                } else {
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "unit %0d: L2_MOD_FIFO parity "
                                          "error\n"),
                               unit));
                }
            } else {
                if (non_sbus) {
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "Mem hwbase: 0x%x [%s]\n"), base,
                               _soc_td3_ser_hwmem_base_info[base]));
                }
            }
        }
        if (drop) {
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "SER caused packet drop.\n")));
        }
    }
}

STATIC uint32
_soc_td3_check_flex_ctr_addr(int unit, uint32 address, soc_block_t blk, uint32 stage,
                            uint32 index)
{
    uint32 index2 = index;
    if ((SOC_BLK_IPIPE == blk) && (21 == stage)) {
        switch (address & 0xfffff000) {
        /* pool 0 */
        case 0x86800000:
        case 0x86801000:
        case 0x86802000:
        case 0x86803000:

        case 0x86804000:
        case 0x86808000:
        case 0x8680c000:
        case 0x86810000:

        /* pool 1 */
        case 0x86820000:
        case 0x86821000:
        case 0x86822000:
        case 0x86823000:

        case 0x86824000:
        case 0x86828000:
        case 0x8682c000:
        case 0x86830000:

        /* pool 2 */
        case 0x86840000:
        case 0x86841000:
        case 0x86842000:
        case 0x86843000:

        case 0x86844000:
        case 0x86848000:
        case 0x8684c000:
        case 0x86850000:

        /* pool 3 */
        case 0x86880000:
        case 0x86881000:
        case 0x86882000:
        case 0x86883000:

        case 0x86884000:
        case 0x86888000:
        case 0x8688c000:
        case 0x86890000:

        /* pool 4 */
        case 0x868a0000:
        case 0x868a1000:
        case 0x868a2000:
        case 0x868a3000:

        case 0x868a4000:
        case 0x868a8000:
        case 0x868ac000:
        case 0x868b0000:
            index2 = address & 0xfff; break; /* 4K depth */
        default:
            break;
        }
    } else if ((SOC_BLK_EPIPE == blk) && (10 == stage)) {
        switch (address & 0xfffff000) {
        /* pool 0 */
        case 0x2a800000:
        case 0x2a801000:
        case 0x2a802000:
        case 0x2a803000:

        case 0x2a804000:
        case 0x2a808000:
        case 0x2a80c000:
        case 0x2a810000:
            index2 = address & 0xfff; break; /* 4K depth */
        default:
            break;
        }
    }
    if (index2 != index) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "flex_ctr_addr_detected: index %d was remapped "
                        "to %d\n"),
             index, index2));
    }
    return index2;
}

STATIC int
_soc_td3_check_counter_with_ecc(int unit, soc_mem_t mem)
{
    int mem_is_ctr_with_ecc = 0;

    if (mem == DLB_ECMP_FLOWSETm ||
        mem == DLB_ECMP_FLOWSET_TIMESTAMP_PAGEm ||
        mem == DLB_ECMP_PORT_AVG_QUALITY_MEASUREm ||
        mem == DLB_HGT_LAG_FLOWSETm ||
        mem == DLB_HGT_LAG_FLOWSET_TIMESTAMP_PAGEm) {
        return mem_is_ctr_with_ecc;
    }

    if ((mem != INVALIDm) &&
        SOC_MEM_IS_VALID(unit, mem) &&
        (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ENTRY_CLEAR) &&
        SOC_MEM_FIELD_VALID(unit, mem, ECCf)) {
        mem_is_ctr_with_ecc = 1;
    }
    if (mem_is_ctr_with_ecc) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "counter_with_ecc_detected: mem %s \n"),
             SOC_MEM_NAME(unit, mem)));
    }
    return mem_is_ctr_with_ecc;
}

STATIC int
_soc_td3_force_unique_mode(int unit, soc_mem_t mem)
{
    int force_unique_mode = 0;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return 0;
    }
    if (((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ENTRY_CLEAR) &&
         SOC_MEM_FIELD_VALID(unit, mem, ECCf)) ||
            /* above covers
             *    ING_FLX_CTR_COUNTER_TABLEs,
             *    EFP_COUNTER_TABLE,
             *    EGR_FLEX_CTR_COUNTER_TABLEs
             */
        /* SER_ENTRY_CLEAR, but with PARITY */
        (ING_PW_TERM_SEQ_NUMm == mem) ||
        (EGR_PERQ_XMT_COUNTERSm == mem) ||
        (EGR_PW_INIT_COUNTERSm == mem) ||
        /* SER_CACHE_RESTORE */
        /* (EGR_FRAGMENT_ID_TABLEm == mem) || - same values written to all pipes
         * using global view - esw/firebolt/l3.c */
        (EGR_XMIT_START_COUNTm == mem)
       ) {
        force_unique_mode = 1;
    }
    if (force_unique_mode) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "Will force unique mode for: mem %s \n"),
             SOC_MEM_NAME(unit, mem)));
    }
    return force_unique_mode;
}

STATIC int
_soc_trident3_tcam_only_remap(int unit, soc_mem_t *mem, int *index)
{
    if (!SOC_MEM_IS_VALID(unit, *mem)) {
        return SOC_E_PARAM;
    }

    switch(*mem) {
        case L3_TUNNEL_ONLYm:
            *mem = L3_TUNNELm;
            break;
        case ING_SNAT_ONLYm:
            *mem = ING_SNATm;
            break;
        case VLAN_SUBNET_ONLYm:
            *mem = VLAN_SUBNETm;
            break;
        case MY_STATION_TCAM_ENTRY_ONLYm:
            *mem = MY_STATION_TCAMm;
            break;
        case MY_STATION_TCAM_2_ENTRY_ONLYm:
            *mem = MY_STATION_TCAM_2m;
            break;
        case L2_USER_ENTRY_ONLYm:
            *mem = L2_USER_ENTRYm;
            break;
        case SRC_COMPRESSION_TCAM_ONLYm:
            *mem = SRC_COMPRESSIONm;
            break;
        case DST_COMPRESSION_TCAM_ONLYm:
            *mem = DST_COMPRESSIONm;
            break;
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
            *mem = IFP_LOGICAL_TABLE_SELECTm;
            break;
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
            *mem = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
            break;
        case CPU_COS_MAP_ONLYm:
            *mem = CPU_COS_MAPm;
            break;
        case L3_DEFIP_ONLYm:
            *mem = L3_DEFIPm;
            break;
        case L3_DEFIP_PAIR_128_ONLYm:
            *mem = L3_DEFIP_PAIR_128m;
            break;
        case PHB_SELECT_TCAM_ONLYm:
            *mem = PHB_SELECT_TCAMm;
            break;
        case FLEX_RTAG7_HASH_TCAM_ONLYm:
            *mem = FLEX_RTAG7_HASH_TCAMm;
            break;
        case PKT_FLOW_SELECT_TCAM_0_ONLYm:
            *mem = PKT_FLOW_SELECT_TCAM_0m;
            break;
        case PKT_FLOW_SELECT_TCAM_1_ONLYm:
            *mem = PKT_FLOW_SELECT_TCAM_1m;
            break;
        case PKT_FLOW_SELECT_TCAM_2_ONLYm:
            *mem = PKT_FLOW_SELECT_TCAM_2m;
            break;
        case IP_PARSER1_MICE_TCAM_ONLY_0m:
            *mem = IP_PARSER1_MICE_TCAM_0m;
            break;
        case IP_PARSER1_MICE_TCAM_ONLY_1m:
            *mem = IP_PARSER1_MICE_TCAM_1m;
            break;
        case IP_PARSER2_MICE_TCAM_ONLY_0m:
            *mem = IP_PARSER2_MICE_TCAM_0m;
            break;
        case IP_PARSER2_MICE_TCAM_ONLY_1m:
            *mem = IP_PARSER2_MICE_TCAM_1m;
            break;
        case EGR_ZONE_0_EDITOR_CONTROL_TCAM_ONLYm:
            *mem = EGR_ZONE_0_EDITOR_CONTROL_TCAMm;
            break;
        case EGR_ZONE_1_EDITOR_CONTROL_TCAM_ONLYm:
            *mem = EGR_ZONE_1_EDITOR_CONTROL_TCAMm;
            break;
        case EGR_ZONE_2_EDITOR_CONTROL_TCAM_ONLYm:
            *mem = EGR_ZONE_2_EDITOR_CONTROL_TCAMm;
            break;
        case EGR_ZONE_3_EDITOR_CONTROL_TCAM_ONLYm:
            *mem = EGR_ZONE_3_EDITOR_CONTROL_TCAMm;
            break;
        case EGR_ZONE_4_EDITOR_CONTROL_TCAM_ONLYm:
            *mem = EGR_ZONE_4_EDITOR_CONTROL_TCAMm;
            break;
        case EGR_PKT_FLOW_SELECT_TCAM_ONLYm:
            *mem = EGR_PKT_FLOW_SELECT_TCAMm;
            break;
        case EGR_FIELD_EXTRACTION_PROFILE_1_TCAM_ONLYm:
            *mem = EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm;
            break;
        case EGR_FIELD_EXTRACTION_PROFILE_2_TCAM_ONLYm:
            *mem = EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm;
            break;
        case EGR_QOS_CTRL_TCAM_ONLYm:
            *mem = EGR_QOS_CTRL_TCAMm;
            break;
        default:
            break;
    }
    if (NUM_PIPE(unit) == 1) {
        /*
         * Map physical index to logical index
         * This is similar to what is done in _soc_ser_recovery_hw_cache for
         * multi-pipe devices. Doing this here saves us the trouble of modifying
         * SER correction event info
         */
        switch(*mem) {
            soc_mem_t report_mem;
            case L3_DEFIPm:
            case L3_DEFIP_DATA_ONLYm:
                *index = soc_trident2_l3_defip_mem_index_get(unit, *index, &report_mem);
                *mem = report_mem == L3_DEFIP_PAIR_128m ?
                       L3_DEFIP_PAIR_128_DATA_ONLYm : *mem;
                break;
            case L3_DEFIP_PAIR_128m:
            case L3_DEFIP_PAIR_128_DATA_ONLYm:
                *index = soc_trident2_l3_defip_index_remap(unit, *mem, *index);
                break;
            default: break;
        }
    }

    return SOC_E_NONE;
}

soc_field_info_t ing_ser_fifo_field_info[] = {
{ ADDRESSf, 32, 5, SOCF_LE | SOCF_GLOBAL },
{ DATAf, 50, 0, SOCF_LE | SOCF_GLOBAL },
{ DROPf, 1, 37, 0 | SOCF_GLOBAL },
{ ECC_PARITYf, 2, 38, SOCF_LE | SOCF_GLOBAL },
{ HWMEMBASEf, 8, 23, SOCF_LE | SOCF_GLOBAL },
{ INSTRUCTION_TYPEf, 4, 1, SOCF_LE | SOCF_GLOBAL },
{ MEMBASEf, 8, 23, SOCF_LE | SOCF_GLOBAL },
{ MEMINDEXf, 18, 5, SOCF_LE | SOCF_GLOBAL },
{ MEM_TYPEf, 1, 40, 0 | SOCF_GLOBAL },
{ MULTIPLEf, 1, 42, 0 | SOCF_GLOBAL },
{ NON_SBUSf, 1, 41, 0 | SOCF_GLOBAL },
{ PIPE_STAGEf, 6, 31, SOCF_LE | SOCF_GLOBAL },
{ REGBASEf, 17, 13, SOCF_LE | SOCF_GLOBAL },
{ REGINDEXf, 8, 5, SOCF_LE | SOCF_GLOBAL },
{ RESERVED_0f, 6, 44, SOCF_LE|SOCF_RES | SOCF_GLOBAL },
{ VALIDf, 1, 0, 0 | SOCF_GLOBAL },
{ INVALIDf }
};

#define FIX_MEM_ORDER_E_TEMPLE(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? \
                                BYTES2WORDS((m)->bytes)-1-(v) : \
                                (v))
void
soc_mem_field_get_temple(int unit, soc_mem_t mem, soc_field_t field, const uint32 *entbuf,
                         uint32 *fldbuf, uint32 fldbuf_size)
{
    soc_field_info_t    *fieldinfo = NULL;
    soc_mem_info_t      *meminfo = &SOC_MEM_INFO(unit, mem);
    int                 i, wp, bp, len;

    i = 0;
    while(ing_ser_fifo_field_info[i].field != INVALIDf) {
        if (ing_ser_fifo_field_info[i].field == field) {
            fieldinfo = &ing_ser_fifo_field_info[i];
            break;
        }

        i++;
    }

    if (fieldinfo == NULL) {
        return;
    }

    bp = fieldinfo->bp;
    len = fieldinfo->len;
    assert(len / 32 <= fldbuf_size); /* assert we do not write beyond the end of the output buffer */

    if (len == 1) {     /* special case single bits */
        wp = bp / 32;
        bp = bp & (32 - 1);
        if (entbuf[FIX_MEM_ORDER_E_TEMPLE(wp, meminfo)] & (1<<bp)) {
            fldbuf[0] = 1;
        } else {
            fldbuf[0] = 0;
        }
        return;
    }

    if (fieldinfo->flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (; len > 0; len -= 32) {
            if (bp) {
                fldbuf[i] =
                    entbuf[FIX_MEM_ORDER_E_TEMPLE(wp++, meminfo)] >> bp &
                    ((1 << (32 - bp)) - 1);
                if ( len > (32 - bp) ) {
                    fldbuf[i] |= entbuf[FIX_MEM_ORDER_E_TEMPLE(wp, meminfo)] <<
                        (32 - bp);
                }
            } else {
                fldbuf[i] = entbuf[FIX_MEM_ORDER_E_TEMPLE(wp++, meminfo)];
            }

            if (len < 32) {
                fldbuf[i] &= ((1 << len) - 1);
            }

            i++;
        }
    } else {
        i = (len - 1) / 32;

        while (len > 0) {
            assert(i >= 0);

            fldbuf[i] = 0;

            do {
                fldbuf[i] =
                    (fldbuf[i] << 1) |
                    ((entbuf[FIX_MEM_ORDER_E_TEMPLE(bp / 32, meminfo)] >>
                      (bp & (32 - 1))) & 1);
                len--;
                bp++;
            } while (len & (32 - 1));

            i--;
        }
    }
}

STATIC void
_soc_trident3_counter_reg_check(int unit, uint32 blk, int acc_type, uint32 addr,
                                uint32 *read_again)
{
    soc_regaddrinfo_t ainfo;

    soc_regaddrinfo_extended_get(unit, &ainfo, blk, acc_type, addr);
    if (ainfo.reg == IBCAST_64r || ainfo.reg == IUNKOPC_64r ||
        ainfo.reg == IIPMC_64r || ainfo.reg == ILTOMC_64r ||
        ainfo.reg == IMRP4_64r || ainfo.reg == IMRP6_64r ||
        ainfo.reg == RDBGC0_64r || ainfo.reg == RDBGC1_64r ||
        ainfo.reg == RDBGC2_64r || ainfo.reg == RDBGC3_64r ||
        ainfo.reg == RDBGC4_64r || ainfo.reg == RDBGC5_64r ||
        ainfo.reg == RDBGC6_64r || ainfo.reg == RDBGC7_64r ||
        ainfo.reg == RDBGC8_64r || ainfo.reg == RDBGC9_64r ||
        ainfo.reg == RDBGC10_64r || ainfo.reg == RDBGC11_64r ||
        ainfo.reg == RDBGC12_64r || ainfo.reg == RDBGC13_64r ||
        ainfo.reg == RDBGC14_64r || ainfo.reg == RDBGC15_64r ||
        ainfo.reg == RIPC4_64r || ainfo.reg == RIPC6_64r ||
        ainfo.reg == RIPD4_64r || ainfo.reg == RIPD6_64r ||
        ainfo.reg == RIPHE4_64r || ainfo.reg == RIPHE6_64r ||
        ainfo.reg == RPARITYDr || ainfo.reg == RPORTD_64r ||
        ainfo.reg == RTUNr || ainfo.reg == RUC_64r) {
        *read_again = 1;
    }
}

int
soc_trident3_process_ser_fifo(int unit, soc_block_t blk, int pipe,
                              char *prefix_str, int l2_mgmt_ser_fifo)
{
    int rv, disable_ser_correction;
    int mem_mode = 0;
    uint8 bidx;
    soc_mem_t mem;
    char blk_str[30];
    soc_reg_t reg = INVALIDr;
    _soc_ser_correct_info_t spci;
    _soc_mem_ser_en_info_t *ser_info_table = NULL;
    soc_reg_t parity_enable_reg = INVALIDr;
    soc_mem_t parity_enable_mem = INVALIDm;
    soc_field_t parity_enable_field = INVALIDf;
    int parity_enable_field_position = -1;
    uint32 ecc_parity = 0, address = 0, valid = 0, multiple = 0;
    uint32 reg_val, mask, entry[SOC_MAX_MEM_WORDS];
    uint32 stage = 0, addrbase = 0, index = 0, type = 0;
    uint32 sblk = 0, regmem = 0, non_sbus = 0, drop = 0;
    soc_mem_t ipsf[] = { ING_SER_FIFO_PIPE0m, ING_SER_FIFO_PIPE1m };
    soc_mem_t epsf[] = { EGR_SER_FIFO_PIPE0m, EGR_SER_FIFO_PIPE1m };
    soc_reg_t epsr[] = { EGR_INTR_STATUS_PIPE0r, EGR_INTR_STATUS_PIPE1r };
    uint32 stat_flags = 0;
    uint32 read_again = 0;
    soc_stat_t *stat = SOC_STAT(unit);

    LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "In process fifo.\n")));
    switch (blk) {
    case SOC_BLK_IPIPE:
        if (l2_mgmt_ser_fifo) { /* L2_MGMT_SER_FIFO is not per pipe */
            mem = L2_MGMT_SER_FIFOm;
            reg = L2_MGMT_INTRr;
            mask = 0x00000002; /* SER_FIFO_NOT_EMPTY */
            sal_sprintf(blk_str, "IPIPE_L2_MGMT_SER_FIFO");
        } else {
            mem = ipsf[pipe];
            mask = soc_td3_ip_pipe_fifo_bmask[unit][pipe];
            sal_sprintf(blk_str, "IPIPE");
        }
        ser_info_table = SOC_IP_MEM_SER_INFO(unit);
                    /* also lists L2_MOD_FIFOm */
        break;
    case SOC_BLK_EPIPE:
        mem = epsf[pipe];
        mask = 0x00000001; /* SER_FIFO_NON_EMPTYf */
        reg = epsr[pipe];
        sal_sprintf(blk_str, "EPIPE");
        ser_info_table = SOC_EP_MEM_SER_INFO(unit);
        break;
    default: return SOC_E_PARAM;
    }

    if (reg != INVALIDr) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY,
                                          0, &reg_val));
        if (!(reg_val & mask)) {
            /* SER FIFO is empty */
            return SOC_E_NONE;
        }
    }

    do {
        if (SOC_HW_ACCESS_DISABLE(unit)) {
            break;
        }
        sal_memset(entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_pop(unit, mem, MEM_BLOCK_ANY, entry));
        /* process entry */
        soc_mem_field_get_temple(unit, mem, VALIDf, entry, &valid, 1);
        if (valid) {
            disable_ser_correction = 0;
            parity_enable_reg = INVALIDr;
            parity_enable_field = INVALIDf;
            sal_memset(&spci, 0, sizeof(spci));
            soc_mem_field_get_temple(unit, mem, ECC_PARITYf, entry, &ecc_parity, 1);
            soc_mem_field_get_temple(unit, mem, MEM_TYPEf, entry, &regmem, 1);
            soc_mem_field_get_temple(unit, mem, ADDRESSf, entry, &address, 1);
            soc_mem_field_get_temple(unit, mem, PIPE_STAGEf, entry, &stage, 1);
            soc_mem_field_get_temple(unit, mem, INSTRUCTION_TYPEf, entry, &type, 1);
            soc_mem_field_get_temple(unit, mem, DROPf, entry, &drop, 1);
            SOC_BLOCK_ITER(unit, bidx, blk) {
                sblk = SOC_BLOCK2SCH(unit, bidx);
                break;
            }
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "%s\n"), prefix_str));
            soc_mem_field_get_temple(unit, mem, MULTIPLEf, entry, &multiple, 1);
            if (multiple) {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Multiple: ")));
            }
            if (regmem == _SOC_TD3_SER_REG) {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Reg: ")));
            } else {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Mem: ")));
            }
            switch (ecc_parity) {
            case 0:
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Parity error..\n")));
                break;
            case 1:
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Corrected single bit ECC error..\n")));
                /* NOTE: This is supressed by default,
                         we can choose not to supress it */
                break;
            case 2:
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Double or Multiple bit ECC error..\n")));
                spci.double_bit = 1;
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Invalid SER issue !!\n")));
                return SOC_E_INTERNAL;
            }
            if (regmem == _SOC_TD3_SER_MEM) {
                /* process mem */
                soc_mem_field_get_temple(unit, mem, NON_SBUSf, entry, &non_sbus, 1);
                soc_mem_field_get_temple(unit, mem, MEMBASEf, entry, &addrbase, 1);
                soc_mem_field_get_temple(unit, mem, MEMINDEXf, entry, &index, 1);
                /* Max index of L2/L3 is more than 256k */
                if ((stage == 26) && (addrbase == 0xdb || addrbase == 0xdf)) {
                    index |= 1 << 18;
                }
                if (non_sbus == 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                       SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       address);

                    index = _soc_td3_check_flex_ctr_addr(unit, address, blk,
                                                        stage, index);

                    _soc_trident3_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus,
                                                         l2_mgmt_ser_fifo);
                    spci.flags = SOC_SER_SRC_MEM;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blk;
                    spci.pipe_num = l2_mgmt_ser_fifo? -1 : pipe;
                    spci.sblk = sblk;
                    spci.addr = address - index;
                    spci.index = index;
                    spci.stage = stage;
                    spci.acc_type = -1; /* ignore acc_type for addr2_mem_decode */
                    spci.detect_time = sal_time_usecs();

                    /* Try to decode mem first */
                    if (l2_mgmt_ser_fifo) {
                        spci.mem = L2_MOD_FIFOm;
                    } else {
                        spci.mem = soc_addr_to_mem_extended(unit, spci.sblk,
                                                            spci.acc_type,
                                                            spci.addr);
                        rv = _soc_trident3_tcam_only_remap(unit, &spci.mem, &spci.index);
                        if (rv < 0) {
                            LOG_ERROR(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "mem %s is invalid !!\n"),
                                 SOC_MEM_NAME(unit, spci.mem)));
                        }
                    }
                    if (spci.mem != INVALIDm) {
                        spci.flags |= SOC_SER_REG_MEM_KNOWN;

                        if (!spci.double_bit) {
                            disable_ser_correction =
                                _soc_td3_check_counter_with_ecc(unit, spci.mem);
                        }

                        spci.flags |= SOC_SER_LOG_WRITE_CACHE;

                        if (SOC_MEM_INFO(unit, spci.mem).flags & SOC_MEM_FLAG_CAM) {
                            _soc_td3_ser_tcam_control_reg_get(unit, spci.mem,
                                                        &parity_enable_reg,
                                                        &parity_enable_field);
                        } else {
                            _soc_td3_ser_control_reg_get(unit, ser_info_table,
                                                        spci.mem,
                                                        &parity_enable_reg,
                                                        &parity_enable_mem,
                                                        &parity_enable_field,
                                                        &parity_enable_field_position);
                        }

                        if ((SOC_MEM_SER_CORRECTION_TYPE(unit, spci.mem) ==
                             SOC_MEM_FLAG_SER_ENTRY_CLEAR) &&
                             (spci.mem != EGR_PW_INIT_COUNTERSm) &&
                             (spci.mem != ING_PW_TERM_SEQ_NUMm)) {
                            spci.flags |= SOC_SER_ALSO_UPDATE_SW_COUNTER;
                            spci.counter_base_mem = spci.mem;
                            switch (spci.mem) {
                            case ING_FLEX_CTR_COUNTER_TABLE_1m:
                                spci.inst = 1;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_2m:
                                spci.inst = 2;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_3m:
                                spci.inst = 3;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_4m:
                                spci.inst = 4;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_5m:
                                spci.inst = 5;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_6m:
                                spci.inst = 6;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_7m:
                                spci.inst = 7;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_8m:
                                spci.inst = 8;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_9m:
                                spci.inst = 9;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_10m:
                                spci.inst = 10;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_11m:
                                spci.inst = 11;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_12m:
                                spci.inst = 12;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_13m:
                                spci.inst = 13;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_14m:
                                spci.inst = 14;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_15m:
                                spci.inst = 15;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_16m:
                                spci.inst = 16;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_17m:
                                spci.inst = 17;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_18m:
                                spci.inst = 18;
                                break;
                            case ING_FLEX_CTR_COUNTER_TABLE_19m:
                                spci.inst = 19;
                                break;

                            case EGR_FLEX_CTR_COUNTER_TABLE_1m:
                                spci.inst = 1;
                                break;
                            case EGR_FLEX_CTR_COUNTER_TABLE_2m:
                                spci.inst = 2;
                                break;
                            case EGR_FLEX_CTR_COUNTER_TABLE_3m:
                                spci.inst = 3;
                                break;

                            default:
                            /* ING_FLEX_CTR_COUNTER_TABLE_0m
                             * EGR_FLEX_CTR_COUNTER_TABLE_0m
                             * EFP_COUNTER_TABLEm
                             * EGR_PERQ_XMT_COUNTERSm */
                                spci.inst = 0;
                                break;
                            }
                        }
                        rv = soc_td3_ser_mem_mode_get(unit, spci.mem,
                                                     &mem_mode);
                        if ((rv == SOC_E_NONE) &&
                            (mem_mode == _SOC_SER_MEM_MODE_PIPE_UNIQUE) &&
                            (spci.mem != SRC_COMPRESSION_DATA_ONLYm &&
                             spci.mem != DST_COMPRESSION_DATA_ONLYm &&
                             spci.mem != IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm &&
                             spci.mem != EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm)) {
                            if (SOC_MEM_UNIQUE_ACC(unit, spci.mem) != NULL) {
                                spci.mem = SOC_MEM_UNIQUE_ACC(unit,
                                                              spci.mem)[pipe];
                            }
                        } else if ((rv != SOC_E_NONE) &&
                                   (rv != SOC_E_UNAVAIL)) {
                            LOG_ERROR(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "Could not determine unique-global mode for"
                                            "mem %s - will assume global mode !!\n"),
                                 SOC_MEM_NAME(unit, mem)));
                        } else if (_soc_td3_force_unique_mode(unit, spci.mem)) {
                            /* For counters, and other mems which have response
                             * type ENTRY_CLEAR, we must clear ONLY affected
                             * instance and not all instances - even if field module says
                             * it is global mode */
                            if (SOC_MEM_UNIQUE_ACC(unit, spci.mem) != NULL) {
                                spci.mem = SOC_MEM_UNIQUE_ACC(unit,
                                                              spci.mem)[pipe];
                            }
                        }
/* In general, for mems that can operate in both local, unique mode, we want
 * to check the current mode and if current mode is unique we want to
 * do ser_correction for _PIPE0,1 view. Thus, for such mems we need to
 * change mem to mem_PIPE0,1 here.
 *
 * For _DATA_ONLY mems are exception to above rule because for these,
 * ser_correction logic will first remap them to aggr_view and then do
 * correction. For such mems, by specifying them in above equation,
 * we dont change them to _PIPE0,1 in this function.
 * These will be remapped to aggr and then aggr_PIPE0,1 view in
 * common/ser.c
 *
 * By not specifying mems in above equation, we change them to _PIPE0,1
 * view.  Why do we not mention following mems in above equation ?
 *      L3_TUNNEL_DATA_ONLY - this mem never operateds in UNIQUE mode
 *
 *      FPEM_ECC - mem_mode_get will return GLOBAL for this mem.
 *                 common/ser.c will remap it to EXACT_MATCH_2, 4 and then
 *                 change it to _PIPE0,1
 *
 *      EXACT_MATCH_2, 4 - HW cannot report error for these views (HW will always report error with FPEM_ECC view)
 *
 *      FP_UDF_OFFSET - field module accesses data view separately
 *      VFP_POLICY_TABLE - field module accesses data view separately
 *      EFP_POLICY_TABLE - field module accesses data view separately
 *      IFP_POLICY_TABLE - field module accesses data view separately
 *      EFP_COUNTER_TABLE - field module accesses data view separately
 *      EFP_METER_TABLE - field module accesses data view separately
 *      and so on...
 */
                        spci.parity_type = _soc_td3_mem_has_ecc(unit, spci.mem)?
                                           SOC_PARITY_TYPE_ECC :
                                           SOC_PARITY_TYPE_PARITY;
                    } else {
                        spci.flags |= SOC_SER_REG_MEM_UNKNOWN;
                    }
#ifdef ALPM_ENABLE
                    if ((spci.mem == L3_DEFIP_ALPM_ECCm)) {
                        SOC_ALPM_LPM_LOCK(unit);
                    }
#endif
                    if ((spci.mem != INVALIDm) && (spci.mem != L3_DEFIP_ALPM_ECCm)) {
                        MEM_LOCK(unit,spci.mem);
                    }
                    spci.log_id = _soc_td3_populate_ser_log(unit,
                                                           parity_enable_reg,
                                                           parity_enable_mem,
                                                           parity_enable_field,
                                                           parity_enable_field_position,
                                                           spci.mem,
                                                           bidx,
                                                           spci.pipe_num,
                                                           spci.index,
                                                           spci.detect_time,
                                                           spci.sblk,
                                                           spci.addr,
                                                           1, /* disable_parity */
                                                           0, /* disable_mem_read */
                                                           0, /* disable_fill_cache_log */
                                                           1, /* force_cache_log */
                                                           TRUE, 0);

                    if ((spci.mem != INVALIDm) && (spci.mem != L3_DEFIP_ALPM_ECCm)) {
                        MEM_UNLOCK(unit,spci.mem);
                    }
#ifdef ALPM_ENABLE
                    if (spci.mem == L3_DEFIP_ALPM_ECCm) {
                        SOC_ALPM_LPM_UNLOCK(unit);
                    }
#endif
                    if (!disable_ser_correction && !l2_mgmt_ser_fifo) {
                        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                            "unit %d will try ser_correction for mem %s, pipe %d\n"),
                            unit, SOC_MEM_NAME(unit, spci.mem), spci.pipe_num));
                        rv = soc_ser_correction(unit, &spci);
                    } else {
                        rv = SOC_E_NONE;
                    }
                    if (spci.log_id != 0) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                           spci.log_id, 0);
                        if (soc_property_get(unit, "ser_log_print_one", 0)) {
                            soc_ser_log_print_one(unit, spci.log_id);
                        }
                    }
                    if (SOC_FAILURE(rv)) {
                        /* Add reporting failed to correct event flag to
                         * application */
                        soc_event_generate(unit,
                                SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                        if ((spci.mem != INVALIDm) &&
                            (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CAM)) {
                            stat_flags = SOC_SER_STAT_TCAM;
                        } else {
                            stat_flags = 0;
                        }
                        soc_ser_stat_update(unit, stat_flags, blk,
                                            ecc_parity == 0 ? SOC_PARITY_TYPE_PARITY :
                                            SOC_PARITY_TYPE_ECC,
                                            spci.double_bit,
                                            SocSerCorrectTypeFailedToCorrect,
                                            stat);
                        return rv;
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "%s SER mem address un-accessable !!\n"),
                               blk_str));
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
                    soc_ser_stat_update(unit, 0, blk,
                                        ecc_parity == 0 ? SOC_PARITY_TYPE_PARITY :
                                        SOC_PARITY_TYPE_ECC,
                                        spci.double_bit,
                                        SocSerCorrectTypeNoAction,
                                        stat);
                    _soc_trident3_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus,
                                                         l2_mgmt_ser_fifo);
                    _soc_td3_populate_ser_log(unit, INVALIDr, INVALIDm, INVALIDf,
                                              -1, INVALIDm, bidx, 0, index,
                                              sal_time_usecs(), 0, 0, 1, 0, 0, 1,
                                              FALSE, addrbase);
                }
            } else {
                /* process reg */
                soc_mem_field_get_temple(unit, mem, NON_SBUSf, entry, &non_sbus, 1);
                soc_mem_field_get_temple(unit, mem, REGBASEf, entry, &addrbase, 1);
                soc_mem_field_get_temple(unit, mem, REGINDEXf, entry, &index, 1);
                if (l2_mgmt_ser_fifo) {
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "%s SER reg address reported in "
                                          "L2_MGMT_SER_FIRO !!\n"),
                               blk_str));
                    _soc_trident3_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus,
                                                         1);
                } else if (non_sbus == 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                       SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       address);
                    _soc_trident3_print_ser_fifo_details(unit, 1, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus,
                                                         0);
                    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_UNKNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blk;
                    spci.pipe_num = pipe;
                    spci.sblk = sblk;
                    spci.addr = address;
                    spci.index = index;
                    spci.stage = stage;
                    spci.acc_type = -1; /* ignore acc_type for addr2_reg_decode */
                    spci.detect_time = sal_time_usecs();
                    spci.parity_type = SOC_PARITY_TYPE_PARITY;
                    spci.log_id = soc_ser_log_create_entry(unit,
                        sizeof(soc_ser_log_tlv_generic_t) +
                        sizeof(soc_ser_log_tlv_register_t) +
                        sizeof(soc_ser_log_tlv_hdr_t) *3);
                    _soc_trident3_counter_reg_check(unit, sblk, -1, address, &read_again);
                    if (read_again) {
                        spci.flags |= SOC_SER_REG_READ_AGAIN;
                    }
                    rv = soc_ser_correction(unit, &spci);
                    if (spci.log_id != 0) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                           spci.log_id, 0);
                        if (soc_property_get(unit, "ser_log_print_one", 0)) {
                            soc_ser_log_print_one(unit, spci.log_id);
                        }
                    }
                    if (SOC_FAILURE(rv)) {
                        /* Add reporting failed to correct event flag to
                         * application */
                        soc_event_generate(unit,
                                SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                        soc_ser_stat_update(unit, 0, blk,
                                            ecc_parity == 0 ? SOC_PARITY_TYPE_PARITY :
                                            SOC_PARITY_TYPE_ECC,
                                            spci.double_bit,
                                            SocSerCorrectTypeFailedToCorrect,
                                            stat);
                        return rv;
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "%s SER reg address un-accessable !!\n"),
                               blk_str));
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
                    soc_ser_stat_update(unit, 0, blk,
                                        ecc_parity == 0 ? SOC_PARITY_TYPE_PARITY :
                                        SOC_PARITY_TYPE_ECC,
                                        spci.double_bit,
                                        SocSerCorrectTypeNoAction,
                                        stat);
                    _soc_trident3_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus,
                                                         0);
                }
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "unit %d Got invalid mem pop from %s !!\n"),
                       unit, SOC_MEM_NAME(unit, mem)));
        }
        /* check if any more pending */
        if (reg == INVALIDr) {
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN
                (soc_iproc_getreg(unit, soc_reg_addr(unit,
                                  ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r,
                                  REG_PORT_ANY, 0),
                                  &reg_val));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
        }
    } while (reg_val & mask);
    return SOC_E_NONE;
}

STATIC int
_soc_trident3_ser_process_all(int unit, int reg_type, int bit)
{
    uint8      rbi;
    int        port = REG_PORT_ANY;
    uint32     cmic_bit;
    uint64     rb_rval64;
    const      _soc_td3_ser_route_block_t *rb;
    char       prefix_str[10];
    int        block_info_idx;
    soc_reg_t  tmp_reg;
    int        processed_an_intr = 0;
    soc_stat_t *stat = SOC_STAT(unit);
    COMPILER_64_ZERO(rb_rval64);

    sal_sprintf(prefix_str, "\nUnit: %d ", unit);

    LOG_VERBOSE(BSL_LS_SOC_SER,
             (BSL_META_U(unit,
                         "unit %d, _soc_trident3_ser_process_all called: reg_type %d, bit %d \n"),
              unit, reg_type, bit));

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_td3_ser_route_blocks[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (!((rb->cmic_reg == reg_type) && (cmic_bit == 1 << bit))) {
            continue;
        }
        if ((rb->blocktype == SOC_BLK_IPIPE || rb->blocktype == SOC_BLK_EPIPE)
            && (rb->cmic_bit != _SOC_TD3_IPIPE0_IDB_INTR)
            && (rb->cmic_bit != _SOC_TD3_IPIPE1_IDB_INTR)
            && (rb->cmic_bit != _SOC_TD3_IPIPE0_ISW4_INTR)
            && (rb->cmic_bit != _SOC_TD3_IPIPE1_ISW4_INTR)
            && (rb->cmic_bit != _SOC_TD3_CEV_INTR)) {
            /* New fifo style processing */
            (void)soc_trident3_process_ser_fifo(unit, rb->blocktype, rb->pipe,
                                                prefix_str, 0);
            stat->ser_err_fifo++;
            processed_an_intr = 1;
        } else {
            /* Legacy processing */
            /* for rb->blocktype = SOC_BLK_MMU, we will be here */
            SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
                if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                    port = SOC_BLOCK_PORT(unit, block_info_idx);
                    break;
                }
                /* ??? Because entry for mmu in _soc_td3_ser_route_blocks array uses
                 * SOC_BLK_MMU which is illegal for Tomahawk, so block_info_idx
                 * determined above is incorrect for mmu. Does it matter ?
                 */
            }
            if (rb->enable_reg != INVALIDr) {
                /* for SOC_BLK_MMU, enable_reg is INVALIDr */
                if (SOC_BLOCK_IN_LIST(SOC_REG_INFO(unit, rb->enable_reg).block,
                    SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                        /* This port block is not configured */
                        LOG_ERROR(BSL_LS_SOC_SER,
                                  (BSL_META_U(unit,
                                              "unit %d SER error on disabled "
                                              "port block %d !!\n"),
                                   unit, block_info_idx));
                        continue;
                }
            }
            /* Read per route block parity status register */
            if (rb->status_reg != INVALIDr) {
                /* for SOC_BLK_MMU, status_reg is INVALIDr */
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

                /*
                 * For SOC_BLK_MMU, rb->status_reg is INVALIDr,
                 * so rb_rval64 will always be 0, so we will skip all of
                 * following for mmu
                 * so we will never call soc_td3_ser_process,
                 * and hence process_mmu_err at all !!
                 */
                if (COMPILER_64_IS_ZERO(rb_rval64)) {
                    continue;
                }
            }
            processed_an_intr = 1;
            SOC_IF_ERROR_RETURN
                (soc_td3_ser_process(unit, block_info_idx, rb->id,
                                     rb->pipe, port, rb->status_reg,
                                     rb_rval64, rb->info, rb->blocktype,
                                     prefix_str));

            stat->ser_err_int++;
        } /* legacy processing */
    }
    if (!processed_an_intr) {
        LOG_WARN(BSL_LS_SOC_SER,
                 (BSL_META_U(unit,
                             "unit %d, reg_type %d, bit %d "
                             "could not process interrupt !!\n"),
                  unit, reg_type, bit));
        
    }
    return SOC_E_NONE;
}

void
soc_trident3_ser_error(void *unit_vp, void *d1, void *d2, void *d3,
                       void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    (void)_soc_trident3_ser_process_all(unit, PTR_TO_INT(d3), PTR_TO_INT(d4));
    sal_usleep(SAL_BOOT_QUICKTURN ? 1000000 : 10000);
    
    soc_ser_top_intr_reg_enable(unit, PTR_TO_INT(d3), PTR_TO_INT(d4), 1);
    soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
}

static _soc_td3_tcam_wrapper_info_t _soc_td3_tcam_wrapper_table[] = {
    { L3_TUNNELm, IVXLT1_SER_CONTROLr, L3_TUNNEL_ONLY_PARITY_ENf, TCAM_PARITY_MASKf},
    { ING_SNATm, RSEL2_CAM_DBGCTRLr, ING_SNAT_ONLY_PARITY_ENf, TCAM_PARITY_MASKf},
    { VLAN_SUBNETm, IVXLT2_SER_CONTROLr, VLAN_SUBNET_ONLY_PARITY_ENf, TCAM_PARITY_MASKf},
    { MY_STATION_TCAMm, IVP_MY_STATION_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { MY_STATION_TCAM_2m, IFWD1_SER_CONTROLr, MY_STATION_TCAM_2_PARITY_ENf, TCAM_PARITY_MASKf},
    { L2_USER_ENTRYm, IFWD2_SER_CONTROLr, L2_USER_ENTRY_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { SRC_COMPRESSIONm, SRC_COMPRESSION_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { DST_COMPRESSIONm, DST_COMPRESSION_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    /*{ UDF_TCAMm, UDF_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},*/
    { VFP_TCAMm, VFP_CAM_CONTROL_SLICE_3_0r, PARITY_ENf, TCAM_PARITY_MASKf},
    { IFP_LOGICAL_TABLE_SELECTm, IFP_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { IP_MULTICAST_TCAMm, IFWD1_SER_CONTROLr, IP_MULTICAST_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { CPU_COS_MAPm, CPU_COS_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { L3_DEFIPm, ILPM_SER_CONTROLr, L3_DEFIP_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { L3_DEFIP_PAIR_128m, ILPM_SER_CONTROLr, L3_DEFIP_TCAM_PARITY_ENf, TCAM_PARITY_MASK_UPRf},
    { IFP_TCAMm, IFP_TCAM_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { IFP_TCAM_WIDEm, IFP_TCAM_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { PHB_SELECT_TCAMm, IRSEL1_SER_CONTROLr, PHB_SELECT_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { FLEX_RTAG7_HASH_TCAMm, IFWD2_SER_CONTROLr, FLEX_RTAG7_HASH_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { PKT_FLOW_SELECT_TCAM_0m, ICFG_SER_CONTROLr, PKT_FLOW_SELECT_TCAM_0_PARITY_ENf, TCAM_PARITY_MASKf},
    { PKT_FLOW_SELECT_TCAM_1m, IPARS1_SER_CONTROLr, PKT_FLOW_SELECT_TCAM_1_PARITY_ENf, TCAM_PARITY_MASKf},
    { PKT_FLOW_SELECT_TCAM_2m, IFWD1_SER_CONTROLr, PKT_FLOW_SELECT_TCAM_2_PARITY_ENf, TCAM_PARITY_MASKf},
    { FORWARDING_1_LOGICAL_TBL_SEL_TCAMm, FORWARDING_1_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { FORWARDING_2_LOGICAL_TBL_SEL_TCAMm, FORWARDING_2_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { FORWARDING_3_LOGICAL_TBL_SEL_TCAMm, FORWARDING_3_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, TUNNEL_ADAPT_1_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, TUNNEL_ADAPT_2_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAMm, TUNNEL_ADAPT_3_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAMm, TUNNEL_ADAPT_4_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EFP_TCAMm, EFP_TCAM_CONTROLr, EFP_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ZONE_0_EDITOR_CONTROL_TCAMm, EGR_VLAN_TCAM_SER_CONTROLr, EGR_ZONE_0_EDITOR_CONTROL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ZONE_1_EDITOR_CONTROL_TCAMm, EGR_VLAN_TCAM_SER_CONTROLr, EGR_ZONE_1_EDITOR_CONTROL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ZONE_2_EDITOR_CONTROL_TCAMm, EGR_VLAN_TCAM_SER_CONTROLr, EGR_ZONE_2_EDITOR_CONTROL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ZONE_3_EDITOR_CONTROL_TCAMm, EGR_VLAN_TCAM_SER_CONTROLr, EGR_ZONE_3_EDITOR_CONTROL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ZONE_4_EDITOR_CONTROL_TCAMm, EGR_VLAN_TCAM_SER_CONTROLr, EGR_ZONE_4_EDITOR_CONTROL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_PKT_FLOW_SELECT_TCAMm, EGR_EL3_TCAM_SER_CONTROLr, EGR_PKT_FLOW_SELECT_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm, EGR_EL3_TCAM_SER_CONTROLr, EGR_FIELD_EXTRACTION_PROFILE_1_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm, EGR_EL3_TCAM_SER_CONTROLr, EGR_FIELD_EXTRACTION_PROFILE_2_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_QOS_CTRL_TCAMm, EGR_EL3_TCAM_SER_CONTROLr, EGR_QOS_CTRL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAMm, EGR_ADAPT_1_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAMm, EGR_ADAPT_2_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER0_HME_STAGE_TCAM_ONLY_0m, IP_PARSER0_HME_STAGE_TCAM_SER_CONTROL_0r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_0m, IP_PARSER1_HME_STAGE_TCAM_SER_CONTROL_0r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_1m, IP_PARSER1_HME_STAGE_TCAM_SER_CONTROL_1r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m, IP_PARSER1_HME_STAGE_TCAM_SER_CONTROL_2r, SER_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_3m, IP_PARSER1_HME_STAGE_TCAM_SER_CONTROL_3r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER1_HME_STAGE_TCAM_ONLY_4m, IP_PARSER1_HME_STAGE_TCAM_SER_CONTROL_4r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER1_MICE_TCAM_0m, IP_PARSER1_MICE_TCAM_SER_CONTROL_0r, SER_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER1_MICE_TCAM_1m, IP_PARSER1_MICE_TCAM_SER_CONTROL_1r, SER_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER2_HME_STAGE_TCAM_ONLY_0m, IP_PARSER2_HME_STAGE_TCAM_SER_CONTROL_0r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER2_HME_STAGE_TCAM_ONLY_1m, IP_PARSER2_HME_STAGE_TCAM_SER_CONTROL_1r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m, IP_PARSER2_HME_STAGE_TCAM_SER_CONTROL_2r, SER_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_3m, IP_PARSER2_HME_STAGE_TCAM_SER_CONTROL_3r, SER_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER2_HME_STAGE_TCAM_ONLY_4m, IP_PARSER2_HME_STAGE_TCAM_SER_CONTROL_4r, SER_ENf, TCAM_PARITY_MASK0f},
    { IP_PARSER2_MICE_TCAM_0m, IP_PARSER2_MICE_TCAM_SER_CONTROL_0r, SER_ENf, TCAM_PARITY_MASKf},
    { IP_PARSER2_MICE_TCAM_1m, IP_PARSER2_MICE_TCAM_SER_CONTROL_1r, SER_ENf, TCAM_PARITY_MASKf},
#ifdef BCM_HELIX5_SUPPORT
    { EGR_PKT_FLOW_SELECT_TCAMm, EGR_EPFS_TCAM_SER_CONTROLr, EGR_PKT_FLOW_SELECT_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { EGR_QOS_CTRL_TCAMm, EGR_EPFS_TCAM_SER_CONTROLr, EGR_QOS_CTRL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { BSK_FTFP_TCAMm, BSK_FTFP_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm, BSK_FTFP_LTS_A_KEY_GEN_SER_CONTROLr, LOGICAL_TBL_SEL_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
#endif
    { INVALIDm},
};

int
soc_trident3_mem_ser_control(int unit, soc_mem_t mem, int copyno,
                             int enable)
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
        if (soc_property_get(unit, spn_PARITY_ENABLE, FALSE)) {
            SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_all(unit, enable));
            SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table_all(unit, enable));
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_all(unit, enable));
        SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table_all(unit, enable));
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
soc_trident3_ser_register(int unit)
{
    soc_cmic_intr_handler_t handle;

#ifdef INCLUDE_MEM_SCAN
    WRITE_IP_PARSER1_HME_STAGE_CONFIG_2r(unit, 0);
    WRITE_IP_PARSER2_HME_STAGE_CONFIG_2r(unit, 0);
    WRITE_IP_PARSER2_HME_STAGE_CONFIG_3r(unit, 0);
    soc_mem_scan_ser_list_register(unit, TRUE, _soc_td3_tcam_ser_info[unit]);
#endif
    memset(&_td3_ser_functions, 0, sizeof(soc_ser_functions_t));
    _td3_ser_functions._soc_ser_parity_error_cmicx_intr_f =
        &soc_trident3_ser_error;
    soc_ser_function_register(unit, &_td3_ser_functions);

    
    handle.num = CHIP_INTR_LOW_PRIORITY;
    handle.intr_fn = soc_ser_cmicx_intr_handler;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);
}

int
soc_trident3_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                                _soc_ser_sram_info_t *sram_info)
{
    int i, base, base_index, offset, base_bucket, bkt_offset;
    int entries_per_ram = 0, entries_per_bank, contiguous = 0;

    switch (mem) {
    case L2_ENTRY_ECCm:
        sram_info->disable_mem = L2_ENTRY_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        if (index < SOC_TD3_NUM_ENTRIES_L2_BANK) {
            /* dedicated L2 entries - hash table */
            sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;
            entries_per_ram = SOC_TD3_NUM_ENTRIES_L2_DEDICATE_RAM;
            entries_per_bank = entries_per_ram * sram_info->ram_count;
            base = (index/entries_per_bank)*entries_per_bank;
            offset = index % entries_per_ram;
            base += offset;
        } else {
            /* hash_xor mems, stride 8K */
            /*
             * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
             * sram_info->force_field = FORCE_XOR_GENERATIONf;
             */
            sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;
                            /* #indexes to be corrected */
            entries_per_ram = SOC_TD3_NUM_ENTRIES_L2_SHARE_RAM;
                            /* #entries / xor_bank in a shared bank */
            entries_per_bank = entries_per_ram * sram_info->ram_count;
                            /* #entries / uft bank */
            base_index = index - SOC_TD3_NUM_ENTRIES_L2_BANK;
                            /* index in uft space */
            base = (base_index/entries_per_bank)*entries_per_bank;
                            /* base addr of uft bank */
            offset = base_index % entries_per_ram;
                            /* 1st entry within uft bank */
            base += SOC_TD3_NUM_ENTRIES_L2_BANK + offset;
                            /* 1st index for correction */
        }
        break;
    case L3_ENTRY_ECCm:
        sram_info->disable_mem = L3_ENTRY_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        if (index < SOC_TD3_NUM_ENTRIES_L3_BANK) {
            /* dedicated L3 entries - hash table */
            sram_info->ram_count = 1;
            entries_per_ram = SOC_TD3_NUM_ENTRIES_L3_BANK;
            base = index;
        } else {
            /* hash_xor mems, stride 8K */
            /*
             * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
             * sram_info->force_field = FORCE_XOR_GENERATIONf;
             */
            sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;
                            /* #indexes to be corrected */
            entries_per_ram = SOC_TD3_NUM_ENTRIES_XOR_BANK;
                            /* #entries / xor_bank in a shared bank */
            entries_per_bank = entries_per_ram * sram_info->ram_count;
                            /* #entries / uft bank */
            base_index = index - SOC_TD3_NUM_ENTRIES_L3_BANK;
                            /* index in uft space */
            base = (base_index/entries_per_bank)*entries_per_bank;
                            /* base addr of uft bank */
            offset = base_index % entries_per_ram;
                            /* 1st entry within uft bank */
            base += SOC_TD3_NUM_ENTRIES_L3_BANK + offset;
                            /* 1st index for correction */
        }
        break;
    case EXACT_MATCH_ECCm:
        /* hash_xor mems, stride 8K */
        /*
         * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
         * sram_info->force_field = FORCE_XOR_GENERATIONf;
         */
        sram_info->disable_mem = EXACT_MATCH_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TD3_NUM_ENTRIES_XOR_BANK;
                        /* #entries / xor_bank in a shared bank */
        entries_per_bank = entries_per_ram * sram_info->ram_count;
                        /* #entries / uft bank */
        base_index = index;
                        /* index - (SOC_TD3_NUM_ENTRIES_FPEM_BANK=0) */
                        /* index in uft space */
        base = (base_index/entries_per_bank)*entries_per_bank;
                        /* base addr of uft bank */
        offset = base_index % entries_per_ram;
                        /* 1st entry within uft bank */
        base += offset;
                        /* base += (SOC_TD3_NUM_ENTRIES_FPEM_BANK=0) + offset; */
                        /* 1st index for correction */
        break;
    case L3_DEFIP_ALPM_ECCm:
        if (!soc_trident3_alpm_mode_get(unit)) {
            return SOC_E_PARAM;
        }
        /*
         * sram_info->disable_reg = ILPM_SER_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
         * sram_info->force_field = FORCE_XOR_GENERATIONf;
         */
        sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;

#ifdef BCM_MAVERICK2_SUPPORT
    if (NUM_PIPE(unit) == 1) {
        entries_per_ram = SOC_TD3_NUM_ENTRIES_XOR_BANK;
        entries_per_bank = entries_per_ram * sram_info->ram_count;

        /* index in uft space */
        base_index = index;
        base = (base_index/entries_per_bank)*entries_per_bank;
        offset = base_index % entries_per_ram;
        base += offset;

        for (i = 0; i < sram_info->ram_count; i++) {
            base_bucket = base % soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);

            sram_info->view[i] = L3_DEFIP_ALPM_RAWm;
            /* 1:4 Need to fix 1 entry for each ECC entry */
            sram_info->index_count[i] = 1;
            sram_info->mem_indexes[i][0] = base_bucket;

            base += entries_per_ram;
        }
        return SOC_E_NONE;
    }
#endif
        if (soc_trident3_get_alpm_banks(unit) == 8) {
            base = index & 0x7; /* 8 uft_bank mode */
            base_bucket = ((index >> 3) & SOC_TD3_ALPM_BKT_MASK(unit));
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x, uft_bank:%d\n"),
                     base_bucket, base));
            base_bucket = base_bucket % SOC_TD3_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < SOC_TD3_NUM_EL_SHARED; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TD3_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TD3_ALPM_MODE0_BKT_MASK) |
                    (bkt_offset << 3) | base;
            }
        } else {
            base = index & 0x3; /* 4 uft_bank mode */
            base_bucket = ((index >> 2) & SOC_TD3_ALPM_BKT_MASK(unit));
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x, uft_bank:%d\n"),
                     base_bucket, base));
            base_bucket = base_bucket % SOC_TD3_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < SOC_TD3_NUM_EL_SHARED; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TD3_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TD3_ALPM_MODE1_BKT_MASK) |
                    (bkt_offset << 2) | base;
            }
        }
        return SOC_E_NONE;
    case FPEM_LPm:
        sram_info->disable_reg = FPEM_CONTROLr;
        sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
        /* sram_info->force_reg = ISS_MEMORY_CONTROL_84r; */
        /* sram_info->force_field = FORCE_XOR_GENERATIONf; */

        sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TD3_LP_ENTRIES_IN_XOR_BANK;;
                        /* #lp_entries / lp_xor_bank of uft bank */
        base = index % entries_per_ram;
                        /* 1st lp_entry within uft bank */
        entries_per_bank = SOC_TD3_NUM_ENTRIES_XOR_BANK * sram_info->ram_count;
                        /* #phy_entries / uft bank */
        offset = entries_per_bank * (index / SOC_TD3_LP_ENTRIES_IN_UFT_BANK);
                        /* of 1st_phy_entry in uft space */
        for (i = 0; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] = offset;
            if (i < 2) {
                sram_info->view[i] = EXACT_MATCH_2m;
                sram_info->mem_indexes[i][0] += 8*(base + i*entries_per_ram);
                /* indexes 0 to 4K-1 correspond to em2 type entries */
                /* each index in FPEM_LP corresponds to 4 consecutive em2
                 * entries */
            } else if (i == 2) {
                sram_info->view[i] = EXACT_MATCH_4m;
                sram_info->mem_indexes[i][0] += 16*base;
                /* indexes 4K to 6K-1 correspond to em4 type entries */
                /* each index in FPEM_LP corresponds to 4 consecutive em4
                 * entries */
            } else {
                sram_info->view[i] = mem;
                offset = SOC_TD3_LP_ENTRIES_IN_UFT_BANK *
                         (index / SOC_TD3_LP_ENTRIES_IN_UFT_BANK);
                        /* of 1st_lp_entry in uft_space */
                offset += base;
                        /* of 1st_lp_entry to be corrected */
                sram_info->mem_indexes[i][0] = offset + i*entries_per_ram;
                        /* of 4th_lp_entry to be corrected */
            }
        }
        return SOC_E_NONE;
    case VLAN_XLATE_LPm:
    case EGR_VLAN_XLATE_LPm:
        sram_info->view[0] = mem;
        sram_info->index_count[0] = 1;
        sram_info->ram_count = 1;
        entries_per_ram = 0; /* x */
        base = index; /* index of 1st lp_entry */
        break;
    case L2_ENTRY_ISS_LPm:
    case L3_ENTRY_ISS_LPm:
        /* hash_xor mems, stride 2K */
        for (i=0; i < SOC_TD3_NUM_EL_SHARED; i++) {
           sram_info->view[i] = mem;
           sram_info->index_count[i] = 1;
        }
        sram_info->ram_count = SOC_TD3_NUM_EL_SHARED;
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TD3_LP_ENTRIES_IN_XOR_BANK;
        entries_per_bank = SOC_TD3_LP_ENTRIES_IN_UFT_BANK;
        base_index = index; /* index of lp_entry */
        base = (base_index/entries_per_bank)*entries_per_bank;
                        /* lp_entry0 in uft bank */
        offset = base_index % entries_per_ram;
                        /* lp_entry in 1st xor_bank */
        base += offset; /* index of 1st lp_entry */
        break;
    case VLAN_XLATE_1_ECCm:
        contiguous = 1;
        /*
         * sram_info->force_reg = VLAN_XLATE_DBGCTRL_0r;
         * sram_info->force_field = FORCE_XOR_GENf;
         */
        sram_info->disable_mem = VLAN_XLATE_1_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        sram_info->ram_count = SOC_TD3_NUM_EL_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case VLAN_XLATE_2_ECCm:
        contiguous = 1;
        /*
         * sram_info->force_reg = VLAN_XLATE_DBGCTRL_0r;
         * sram_info->force_field = FORCE_XOR_GENf;
         */
        sram_info->disable_mem = VLAN_XLATE_2_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        sram_info->ram_count = SOC_TD3_NUM_EL_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case EGR_VLAN_XLATE_1_ECCm:
        contiguous = 1;
        /*
         * sram_info->force_reg = EGR_VLAN_XLATE_CONTROLr;
         * sram_info->force_field = FORCE_XOR_GENf;
         */
        sram_info->disable_mem = EGR_VLAN_XLATE_1_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        sram_info->ram_count = SOC_TD3_NUM_EL_EGR_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case EGR_VLAN_XLATE_2_ECCm:
        contiguous = 1;
        /*
         * sram_info->force_reg = EGR_VLAN_XLATE_CONTROLr;
         * sram_info->force_field = FORCE_XOR_GENf;
         */
        sram_info->disable_mem = EGR_VLAN_XLATE_2_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        sram_info->ram_count = SOC_TD3_NUM_EL_EGR_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case MPLS_ENTRY_ECCm:
        contiguous = 1;
        sram_info->disable_mem = MPLS_ENTRY_HASH_CONTROLm;
        sram_info->disable_mem_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
        sram_info->disable_mem_index = 0;
        sram_info->ram_count = SOC_TD3_NUM_EL_MPLS_ENTRY;
        base = (index/4) * 4;
        break;
    case L2_ENTRY_LPm:
    case L3_ENTRY_LPm:
    case EXACT_MATCH_LPm:
    case VLAN_XLATE_1_LPm:
    case VLAN_XLATE_2_LPm:
    case EGR_VLAN_XLATE_1_LPm:
    case EGR_VLAN_XLATE_2_LPm:
    case MPLS_ENTRY_LPm:
        if (NUM_PIPE(unit) == 1) {
            /* Special handling needed for LP mems
             * No HW cache for single pipe devices
             */
            return SOC_E_UNAVAIL;
        }
        sram_info->view[0] = mem;
        sram_info->index_count[0] = 1;
        sram_info->ram_count = 1;
        entries_per_ram = 0; /* x */
        base = index; /* index of 1st lp_entry */
        break;
    case ING_DNAT_ADDRESS_TYPE_ECCm:
    case ING_VP_VLAN_MEMBERSHIP_ECCm:
    case EGR_VP_VLAN_MEMBERSHIP_ECCm:
    case SUBPORT_ID_TO_SGPP_MAP_ECCm:
        if (NUM_PIPE(unit) == 1) {
            contiguous = 1;
            sram_info->view[0] = mem;
            sram_info->index_count[0] = 1;
            sram_info->ram_count = 1;
            base = index;
        } else {
            return SOC_E_UNAVAIL;
        }
        break;
    default: return SOC_E_PARAM;
    }
    sram_info->mem_indexes[0][0] = base;
    if (contiguous) {
        for (i=1; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] = sram_info->mem_indexes[i-1][0] + 1;
        }
    } else {
        for (i=1; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] =  sram_info->mem_indexes[i-1][0] +
                                            entries_per_ram;
        }
    }
    return SOC_E_NONE;
}

void
soc_trident3_vlan_xlate_mem_remap(soc_mem_t *mem, int *index)
{
    switch(*mem) {
        case VLAN_XLATE_1_SINGLEm:
            *mem = VLAN_XLATE_1_ECCm;
            break;
        case VLAN_XLATE_1_DOUBLEm:
            *mem = VLAN_XLATE_1_ECCm;
            *index *= 2;
            break;
        case EGR_VLAN_XLATE_1_SINGLEm:
            *mem = EGR_VLAN_XLATE_1_ECCm;
            break;
        case EGR_VLAN_XLATE_1_DOUBLEm:
            *mem = EGR_VLAN_XLATE_1_ECCm;
            *index *= 2;
            break;
        case VLAN_XLATE_2_SINGLEm:
            *mem = VLAN_XLATE_2_ECCm;
            break;
        case VLAN_XLATE_2_DOUBLEm:
            *mem = VLAN_XLATE_2_ECCm;
            *index *= 2;
            break;
        case EGR_VLAN_XLATE_2_SINGLEm:
            *mem = EGR_VLAN_XLATE_2_ECCm;
            break;
        case EGR_VLAN_XLATE_2_DOUBLEm:
            *mem = EGR_VLAN_XLATE_2_ECCm;
            *index *= 2;
            break;
        default:
            break;
    }
}

int
soc_trident3_l3_memwr_parity_check(int unit, soc_mem_t mem, int disable)
{
    int rv = SOC_E_NONE;
    soc_field_t disable_field = DISABLE_SBUS_MEMWR_ECC_CHECKf;
    soc_mem_t disable_mem = INVALIDm;
    l3_entry_hash_control_entry_t hash_control_entry;

    switch(mem) {
        case L3_ENTRY_ONLY_SINGLEm:
        case L3_ENTRY_ONLY_DOUBLEm:
        case L3_ENTRY_ONLY_QUADm:
        case L3_ENTRY_SINGLEm:
        case L3_ENTRY_DOUBLEm:
        case L3_ENTRY_QUADm:
            disable_mem = L3_ENTRY_HASH_CONTROLm;
            break;
        default:
            return SOC_E_NONE;
    }

    sal_memset(&hash_control_entry, 0, sizeof(hash_control_entry));
    rv = soc_mem_read(unit, disable_mem, MEM_BLOCK_ANY, 0, (void *)&hash_control_entry);
    soc_mem_field32_set(unit, disable_mem , (void *)&hash_control_entry, disable_field,
                        disable ? 1 : 0);
    rv = soc_mem_write(unit, disable_mem, MEM_BLOCK_ALL, 0, (void *)&hash_control_entry);

    return rv;
}

int
soc_trident3_ifp_slice_mode_hw_get(int unit, int pipe, int slice_num, int *slice_type,
                             int *slice_enabled)
{
    soc_reg_t ifp_cfg_r;
    uint32 ifp_cfg_rval = 0;
    static soc_reg_t ifp_cfg_r_list[] = {
        IFP_CONFIG_PIPE0r,
        IFP_CONFIG_PIPE1r};
    int rv = 0;

    rv = soc_th_ifp_slice_mode_get(unit, pipe, slice_num,
                                   slice_type, slice_enabled);
    if (rv != SOC_E_INIT) {
       return rv;
    }

    /* coverity[Logically dead code:FALSE]*/
    if ((slice_type == NULL) || (slice_enabled == NULL) ||
        (pipe >= NUM_PIPE(unit))) {
        return SOC_E_PARAM;
    }

    if (pipe < 0) { /* global mode */
        ifp_cfg_r = IFP_CONFIGr;
    } else {
        ifp_cfg_r = ifp_cfg_r_list[pipe];
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, ifp_cfg_r, REG_PORT_ANY, slice_num, &ifp_cfg_rval));

    if (soc_feature(unit, soc_feature_ifp_no_narrow_mode_support)) {
        *slice_type = _SOC_TD3_IFP_SLICE_MODE_WIDE; 
    } else {
    *slice_type = soc_reg_field_get(unit, ifp_cfg_r, ifp_cfg_rval,
                                    IFP_SLICE_MODEf);
    } 
    *slice_enabled = soc_reg_field_get(unit, ifp_cfg_r, ifp_cfg_rval,
                                       IFP_SLICE_LOOKUP_ENABLEf);
    return SOC_E_NONE;
}

/* Returns FALSE if mem is not one of the dual_mode mems tracked by SER engine
 * Else returns TRUE, base_mem, pipe
 * Note: pipe will be  -1 when mem == base_mem
 *       pipe will be >=0 when mem != base_mem
 */
int soc_td3_mem_is_dual_mode(int unit, soc_mem_t mem, soc_mem_t *base_mem, int *pipe)
{
    int i, p;
    int rv_dual_mode = FALSE;
    static const soc_mem_t mem_list[] = {
        SRC_COMPRESSIONm,
        DST_COMPRESSIONm,
        FP_UDF_TCAMm,
        VFP_TCAMm,
        IFP_LOGICAL_TABLE_SELECTm,
        EXACT_MATCH_LOGICAL_TABLE_SELECTm,
        EFP_TCAMm,
        IFP_TCAMm,
        IFP_TCAM_WIDEm,
        INVALIDm};
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return FALSE;
    }
    for (i = 0; mem_list[i] != INVALIDm; i++) {
        /* test for base_view */
        p = -1;
        if (mem == mem_list[i]) {
            rv_dual_mode = TRUE;
            break;
        }
        /* test for base_PIPE0,1 views */
        if (SOC_MEM_UNIQUE_ACC(unit, mem_list[i]) != NULL) {
            for (p = 0; p < NUM_PIPE(unit); p++) {
                if (mem == SOC_MEM_UNIQUE_ACC(unit, mem_list[i])[p]) {
                    rv_dual_mode = TRUE;
                    break; /* from p loop */
                }
            }
            if (rv_dual_mode) {
                break; /* from i loop */
            }
        }
    }
    if (FALSE != rv_dual_mode) {
        if (NULL != pipe) {
            *pipe = p;
        }
        if (NULL != base_mem) {
            *base_mem = mem_list[i];
        }
    }
    return rv_dual_mode;
}

int
soc_td3_mem_index_invalid(int unit, soc_mem_t mem, int index, uint8 dual_mode,
                         soc_mem_t in_base_mem, int in_pipe)
{
    int slice_num, slice_type, rv;
    int cfg_slice_enabled = 0;
    int cfg_slice_type = 0;
    int mem_mode = _SOC_SER_MEM_MODE_GLOBAL;
    soc_mem_t base_mem = INVALIDm;
    int pipe = -1, mode_ok = 0;
    int8 dual_mode_mem;

    if (0xFF == dual_mode) {
        dual_mode_mem = soc_td3_mem_is_dual_mode(unit, mem, &base_mem, &pipe);
    } else {
        base_mem = in_base_mem;
        pipe = in_pipe;
        dual_mode_mem = dual_mode;
    }

    if (dual_mode_mem) {
        if (!SOC_FAILURE(soc_td3_check_hw_global_mode(unit, base_mem,
                                                     &mem_mode))) {
            mode_ok = ((_SOC_SER_MEM_MODE_GLOBAL == mem_mode) && (pipe < 0)) ||
                      ((_SOC_SER_MEM_MODE_GLOBAL != mem_mode) && (pipe >= 0));
            if (!mode_ok) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "Denied access to mem %s, pipe %d, index %d in %s mode !!\n"),
                    SOC_MEM_NAME(unit, mem), pipe, index, mem_mode? "UNIQUE" : "GLOBAL"));
                return TRUE; /* skip: dual mode, but !mode_ok */
            }

            if (base_mem != IFP_TCAMm && base_mem != IFP_TCAM_WIDEm) {
                return FALSE; /* allow: dual_mode && mode_ok */
            }

            /* IFP_TCAMm or IFP_TCAM_WIDEm */
            if (base_mem == IFP_TCAMm) {
                slice_num = index/512; slice_type = 0;
            } else {
                slice_num = index/256; slice_type = 1;
            }
            if ((slice_num < 0) || (slice_num >= 12)) {
                return TRUE; /* invalid - skip mem_acc */
            }

            rv = soc_trident3_ifp_slice_mode_hw_get(unit, pipe, slice_num,
                                           &cfg_slice_type, &cfg_slice_enabled);
            if (SOC_FAILURE(rv)) {
                /* only case where this can happen:
                 * When slice is in global mode and we pass 'pipe != -1' or
                 * invalid input parameters.
                 * For example, if we are trying to access location 5 in IFP_TCAM_WIDE_PIPE2,
                 * but if slice_0 is part of IFP_TCAM then field module will return
                 * SOC_E_CONFIG - to indicate that slice must be skipped.
                 */
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "unit %d, slice_mode_get failed: mem %s, "
                    "pipe: %d, index %d, slice_num %d !!\n"),
                           unit, SOC_MEM_NAME(unit, mem), pipe, index, slice_num));
                return TRUE; /* invalid - skip mem_acc */
            } else if (slice_type != cfg_slice_type) {
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "Denied access to mem %s, pipe %d, index %d slice %d in %s slice_mode !!\n"),
                    SOC_MEM_NAME(unit, mem), pipe, index, slice_num, cfg_slice_type? "WIDE" : "NARROW"));
                return TRUE; /* invalid - skip mem_acc */
            } else {
                return FALSE; /* proceed with mem_acc */
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                "Denied access to mem %s, pipe %d, index %d in %s mode !!\n"),
                SOC_MEM_NAME(unit, mem), pipe, index, "UNKNOWN_MODE"));
            return TRUE; /* skip: dual mode, but could not find mode */
        }
    }
    return FALSE; /* allow: !dual_mode */
}

/* For use during WB.
 * Looks at SER_RANGE_ENABLEr and determines if mem is in global mode / unique
 * mode.
 * Works ONLY for mems listed in mem_list[] below.
 * Cannot use: soc_td3_field_mem_mode_get during WB
 * Note: Bit positions are HARD-CODED to match with order of entries in
         _soc_generic_ser_info_t _soc_td3_tcam_ser_info_template[]
         MUST change these values if we change above array !!
 */
int
soc_td3_check_hw_global_mode(int unit, soc_mem_t mem, int *mem_mode)
{
    int i = 0;
    uint32 range_enable;
    static const struct {
        soc_mem_t mem;
        int bit_position;
    } mem_list[] = {
        {SRC_COMPRESSIONm, 10},
        {DST_COMPRESSIONm, 12},
        {FP_UDF_TCAMm, 14},
        {VFP_TCAMm, 16},
        {IFP_LOGICAL_TABLE_SELECTm, 18},
        {EXACT_MATCH_LOGICAL_TABLE_SELECTm, 20},
        {EFP_TCAMm, 22},
        {IFP_TCAMm, 24},
        {IFP_TCAM_WIDEm, 26},
        {INVALIDm, 0 }
    };
    if (!SOC_MEM_IS_VALID(unit,mem)) {
       return SOC_E_PARAM;
    }
    if (NULL == mem_mode) {
       return SOC_E_PARAM;
    }
    *mem_mode = _SOC_SER_MEM_MODE_GLOBAL;
    for (i = 0; mem_list[i].mem != INVALIDm; i++) {
        if (mem_list[i].mem == mem) {
            SOC_IF_ERROR_RETURN(
                READ_SER_RANGE_ENABLEr(unit, &range_enable));
            if ((range_enable >> mem_list[i].bit_position) & 0x1) {
                *mem_mode = _SOC_SER_MEM_MODE_PIPE_UNIQUE;
            }
            return SOC_E_NONE;
        }
    }
    return SOC_E_NONE;
}

int
soc_td3_ser_mem_mode_get(int unit, soc_mem_t mem, int *mem_mode)
{
    int rv = SOC_E_NONE;

    /* first query field module */
    /* trident3/field.c didn't implement. Waiting*/
    rv = soc_th_field_mem_mode_get(unit, mem, mem_mode);

    if (rv == SOC_E_INIT) {
        rv = soc_trident3_tcam_mode_get(unit, mem, mem_mode);
        if (rv == SOC_E_NOT_FOUND) {
            *mem_mode = _SOC_SER_MEM_MODE_GLOBAL;
        }
        rv = SOC_E_NONE;
    }

    return rv;
}

int
soc_td3_ifp_slice_mode_check(int unit, soc_mem_t mem, int slice, int *slice_skip)
{
#define _SOC_TD3_IFP_SLICE_MODE_NARROW 0
#define _SOC_TD3_IFP_SLICE_MODE_WIDE 1
    soc_reg_t reg;
    uint32 rval;
    int exp_slice_width_narrow, slice_width_narrow, slice_enabled;

    if (NULL == slice_skip) {
        return SOC_E_PARAM;
    }
    *slice_skip = 0;
    switch (mem) {
    case IFP_TCAMm:
        exp_slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIGr;
        break;
    case IFP_TCAM_PIPE0m:
        exp_slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE0r;
        break;
    case IFP_TCAM_PIPE1m:
        exp_slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE1r;
        break;
    case IFP_TCAM_WIDEm:
        exp_slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIGr;
        break;
    case IFP_TCAM_WIDE_PIPE0m:
        exp_slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIG_PIPE0r;
        break;
    case IFP_TCAM_WIDE_PIPE1m:
        exp_slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIG_PIPE1r;
        break;
    default:
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, slice, &rval));
    if (soc_feature(unit, soc_feature_ifp_no_narrow_mode_support)) {
       slice_width_narrow = _SOC_TD3_IFP_SLICE_MODE_WIDE; 
    } else {
    slice_width_narrow = soc_reg_field_get(unit, reg, rval, IFP_SLICE_MODEf);
    }

    slice_enabled = soc_reg_field_get(unit, reg, rval, IFP_SLICE_ENABLEf);
    if (!slice_enabled || (exp_slice_width_narrow != slice_width_narrow)) {
        *slice_skip = 1;
    }
    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit, "soc_td3_ifp_slice_mode_check: For mem %s, read reg %s, slice_enabled = %0d, slice_width = %0d, slice_skip = %0d\n"),
        SOC_MEM_NAME(unit, mem), SOC_REG_NAME(unit, reg), slice_enabled,
        slice_width_narrow, *slice_skip));
    return SOC_E_NONE;
}

/*
 * 0. Must not read IFP_TCAM_WIDEm for slices that are in 80b mode.
 *    Must not read IFP_TCAMm for slices in 160b mode.
 *    Must breakdown dma read into read of slices
 *
 * 1. Issues dma_read for each slice of mem and collects data into *table.
 *
 * 2. used for reloading sw_cache from hw_tables during WB.
 *    This requires reading of IFP_CONFIGr to determine slice_mode. Thus
 *    we cannot use soc_td3_ifp_slice_mode_get() which will be eventually
 *    replaced by field' module.
 *
 * 3. used for scrubbing IFP_TCAM, IFP_TCAM_WIDE views before CPU goes offline
 *    We still cannot use standard soc_td3_ifp_slice_mode_get() as defined earlier,
 *    because exit_clean in appl/diag/shell.c first invokes _bcm_shutdown
 *    (which de-inits field module) and then soc_shutdown (which calls this
 *    function)
 */
int
soc_td3_ifp_tcam_dma_read(int unit, soc_mem_t mem, int blk, uint32 *table,
                         int en_per_pipe_read)
{
    int slice, slice_skip = 0, slice_size;
    int slice_start_addr, slice_end_addr, entry_dw;
    int rv = SOC_E_NONE;

    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return SOC_E_PARAM;
    }
    if (NULL == table) {
        return SOC_E_PARAM;
    }

    switch (mem) {
    case IFP_TCAMm:
    case IFP_TCAM_PIPE0m:
    case IFP_TCAM_PIPE1m:
        slice_size = 512;
        break;
    case IFP_TCAM_WIDEm:
    case IFP_TCAM_WIDE_PIPE0m:
    case IFP_TCAM_WIDE_PIPE1m:
        slice_size = 256;
        break;
    default:
        return SOC_E_PARAM;
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    slice_start_addr = soc_mem_index_min(unit, mem);
    for (slice = 0; slice < 12; slice++) {
        slice_end_addr = slice_start_addr + slice_size - 1;
        SOC_IF_ERROR_RETURN(
            soc_td3_ifp_slice_mode_check(unit, mem, slice,
                                         &slice_skip));
        if (!slice_skip) {
            if (en_per_pipe_read) {
                int i;
                uint32 ser_flags;
                int acc_type_list[] = {
                    _SOC_TD3_ACC_TYPE_UNIQUE_PIPE0,
                    _SOC_TD3_ACC_TYPE_UNIQUE_PIPE1};
                for (i = 0; i < NUM_PIPE(unit); i++) {
                    ser_flags = 0;
                    ser_flags |= _SOC_SER_FLAG_DISCARD_READ;
                    ser_flags |= _SOC_SER_FLAG_MULTI_PIPE;
                    ser_flags |= acc_type_list[i];
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "soc_td3_ifp_tcam_dma_read: will read slice %0d for mem %s, pipe %d\n"),
                             slice, SOC_MEM_NAME(unit, mem), acc_type_list[i]));
                    if (soc_mem_ser_read_range(unit, mem, blk,
                                               slice_start_addr, slice_end_addr,
                                               ser_flags,
                                               &table[slice_start_addr * entry_dw]
                                              ) < 0) {
                        rv = SOC_E_FAIL;
                    }
                }
            } else {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "soc_td3_ifp_tcam_dma_read: will read slice %0d for mem %s\n"),
                         slice, SOC_MEM_NAME(unit, mem)));
                if (soc_mem_read_range(unit, mem, blk,
                                       slice_start_addr, slice_end_addr,
                                       &table[slice_start_addr * entry_dw]) < 0) {
                    rv = SOC_E_FAIL;
                }
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "soc_td3_ifp_tcam_dma_read: skipped slice %0d for mem %s\n"),
                     slice, SOC_MEM_NAME(unit, mem)));
        }
        slice_start_addr = slice_end_addr + 1;
    }
    return rv;
}

int
soc_trident3_check_cache_skip(int unit, soc_mem_t mem)
{
    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return 1;
    }
    
    switch (mem) {
        case L3_TUNNEL_DATA_ONLYm:

        case DST_COMPRESSION_DATA_ONLYm:
        case DST_COMPRESSION_DATA_ONLY_PIPE0m:
        case DST_COMPRESSION_DATA_ONLY_PIPE1m:

        case SRC_COMPRESSION_DATA_ONLYm:
        case SRC_COMPRESSION_DATA_ONLY_PIPE0m:
        case SRC_COMPRESSION_DATA_ONLY_PIPE1m:

        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m:

        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m:

        case EXACT_MATCH_2_ENTRY_ONLYm:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE1m:

        case EXACT_MATCH_4_ENTRY_ONLYm:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE1m:

        case L2_ENTRY_ONLY_TILEm:

        case L2_ENTRY_ONLY_ECCm:
        case L3_ENTRY_ONLY_ECCm:
        case L3_DEFIP_ALPM_ECCm:
        case EXACT_MATCH_ECCm:
        case EXACT_MATCH_ECC_PIPE0m:
        case EXACT_MATCH_ECC_PIPE1m:
        case MPLS_ENTRY_ECCm:
        case VLAN_XLATE_ECCm:
        case EGR_VLAN_XLATE_ECCm:
        case SUBPORT_ID_TO_SGPP_MAP_ECCm:
        case ING_DNAT_ADDRESS_TYPE_ECCm:
        case EGR_VP_VLAN_MEMBERSHIP_ECCm:
        case ING_VP_VLAN_MEMBERSHIP_ECCm:

        case EXACT_MATCH_LPm:
        case L2_ENTRY_ISS_LPm:
        case L3_ENTRY_ISS_LPm:

        case EGR_FRAGMENT_ID_TABLEm:
        case EGR_FRAGMENT_ID_TABLE_PIPE0m:
        case EGR_FRAGMENT_ID_TABLE_PIPE1m:
        case L3_ECMP_RRLB_CNTm:
        case L3_ECMP_RRLB_CNT_PIPE0m:
        case L3_ECMP_RRLB_CNT_PIPE1m:

#ifdef BCM_FLOWTRACKER_SUPPORT
    /* Flowtracker Group counter memories */
        case BSC_KG_FLOW_AGE_OUT_COUNTERm:
        case BSC_KG_FLOW_LEARNED_COUNTERm:
        case BSC_KG_FLOW_EXCEED_COUNTERm:
        case BSC_KG_FLOW_MISSED_COUNTERm:
        case BSC_DG_FLOW_METER_EXCEED_COUNTERm:
        case BSC_AG_AGE_TABLEm:
#endif

            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "cache_skip: skipping mem %s (soc_trident3_check_cache_skip)\n"), SOC_MEM_NAME(unit, mem)));
            return 1;

        default:
            return 0;
    }
/*
        case L2_ENTRY_TILEm:

        case L3_DEFIP_ALPM_IPV4m:
        case L3_DEFIP_ALPM_IPV4_1m:
        case L3_DEFIP_ALPM_IPV6_64m:
        case L3_DEFIP_ALPM_IPV6_64_1m:
        case L3_DEFIP_ALPM_IPV6_128m:
        case L3_DEFIP_ALPM_RAWm:
*/
}

/*
 * following implement assist functions for soc_system_scrub() in
 * src/soc/esw/drv.c
 *
 * 1. Must not scrub 80b slices with _WIDE view and 160b slices with narrow view
 * 2. Must not scrub IFP_TCAM, IFP_TCAM_WIDE when in pipe_unique mode
 * 3. Must NOT scrub read_clear counters
 * 3. Can remove scrubbing of MEMm, if we are already scrubbing MEM_PIPE0-3m
 * 4. Are all mmu instances being scrubbed
 * 5. Views for which cachable=0 will not be scrubbed - so don't need to worry
 * about
 */
int
soc_td3_check_scrub_skip(int unit, soc_mem_t mem, int check_hw_global_mode)
{
    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return 1; /* skip */
    }
    if (soc_mem_is_cam(unit, mem) &&
        SOC_TD3_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem)) {
        /* For tcams that can operate in both global, unique modes,
         * we must skip read of global views when in unique mode.
         * This is because SER_ENGINE when configured to track tcam in unique
         * mode may not know which parity bits to use for comparison when it
         * sees read of global_view (with acc_type = 9)
         * Note: for srams, this skip is not necessary, because these are
         *       not covered by SER_ENGINE
         *
         * We cannot skip read of global views when in global mode.
         * This is because, even though we scrub _PIPE0,1,2,3 views - these
         * will result in detection of error - but not correction !!
         * So we must scrub global views.
         * We can optimize by skipping scrub of _PIPE0,1,2,3 views when in
         * global mode. But this is not needed - because this will happen only
         * once during exit_clean and not during non_tcam_iterations (because
         * when doing non_tcam_iterations, we will be skipping all tcams) */
        int rv;
        int mem_mode = _SOC_SER_MEM_MODE_GLOBAL;
        if (check_hw_global_mode) {
            rv = soc_td3_check_hw_global_mode(unit, mem, &mem_mode);
                /* field module may already be de-initialized */
        } else {
            rv = SOC_E_NONE;/*soc_td3_field_mem_mode_get(unit, mem, &mem_mode);*/
                /* field module is active */
        }
        if ((rv == SOC_E_NONE) && (mem_mode == _SOC_SER_MEM_MODE_PIPE_UNIQUE)) {
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (cannot read this mem in pipe_unique_mode)\n"), SOC_MEM_NAME(unit, mem)));
            return 1; /* skip */
        } else {
            return 0; /* we know for sure this mem is not in 'skip_list' */
        }
    }
    
    switch (mem) {
    /* ECC views need not be scrubbed */
        case L2_ENTRY_ONLY_ECCm:
            /* 1:1 mapping with L2Xm, so scan L2Xm */

        /* case L3_ENTRY_ONLY_ECCm: */
        case L3_ENTRY_IPV4_UNICASTm:
        case L3_ENTRY_IPV4_MULTICASTm:
        case L3_ENTRY_IPV6_UNICASTm:
        case L3_ENTRY_IPV6_MULTICASTm:
        case L3_ENTRY_ONLYm:

        /* case L3_DEFIP_ALPM_ECCm: */
        case L3_DEFIP_ALPM_IPV4m:
        case L3_DEFIP_ALPM_IPV4_1m:
        case L3_DEFIP_ALPM_IPV6_64m:
        case L3_DEFIP_ALPM_IPV6_64_1m:
        case L3_DEFIP_ALPM_IPV6_128m:
        case L3_DEFIP_ALPM_RAWm:

        case FPEM_ECCm:
            /* Has unique acc type, so only pipe0 will be scanned.
             * Instead, scan the _pipe0-3 views */
        /* case FPEM_ECC_PIPE0m: */
        /* case FPEM_ECC_PIPE1m: */
        /* case FPEM_ECC_PIPE2m: */
        /* case FPEM_ECC_PIPE3m: */

        case EXACT_MATCH_2m:
        case EXACT_MATCH_2_PIPE0m:
        case EXACT_MATCH_2_PIPE1m:

        case EXACT_MATCH_4m:
        case EXACT_MATCH_4_PIPE0m:
        case EXACT_MATCH_4_PIPE1m:

        case MPLS_ENTRY_ECCm:
        case VLAN_XLATE_ECCm:
        case EGR_VLAN_XLATE_ECCm:

    /* DATA_ONLY, ENTRY_ONLY views need not be scrubbed */
    /*
        case L2_USER_ENTRY_DATA_ONLYm:
        case L2_USER_ENTRY_ONLYm:
    */
        case L3_TUNNEL_DATA_ONLYm:
        case L3_TUNNEL_ONLYm:

        case VLAN_SUBNET_DATA_ONLYm:
        case VLAN_SUBNET_ONLYm:

        case MY_STATION_TCAM_DATA_ONLYm:
        case MY_STATION_TCAM_ENTRY_ONLYm:

        case ING_SNAT_DATA_ONLYm:
        case ING_SNAT_ONLYm:

        case DST_COMPRESSION_DATA_ONLYm:
        case DST_COMPRESSION_DATA_ONLY_PIPE0m:
        case DST_COMPRESSION_DATA_ONLY_PIPE1m:
    /*
        case DST_COMPRESSION_TCAM_ONLYm:
        case DST_COMPRESSION_TCAM_ONLY_PIPE0m:
        case DST_COMPRESSION_TCAM_ONLY_PIPE1m:
        case DST_COMPRESSION_TCAM_ONLY_PIPE2m:
        case DST_COMPRESSION_TCAM_ONLY_PIPE3m:
    */

        case SRC_COMPRESSION_DATA_ONLYm:
        case SRC_COMPRESSION_DATA_ONLY_PIPE0m:
        case SRC_COMPRESSION_DATA_ONLY_PIPE1m:
    /*
        case SRC_COMPRESSION_TCAM_ONLYm:
        case SRC_COMPRESSION_TCAM_ONLY_PIPE0m:
        case SRC_COMPRESSION_TCAM_ONLY_PIPE1m:
        case SRC_COMPRESSION_TCAM_ONLY_PIPE2m:
        case SRC_COMPRESSION_TCAM_ONLY_PIPE3m:

        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m:

        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m:
    */

        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m:
    /*
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m:
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m:
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m:
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m:
    */

    /* specials which need not be scrubbed */
        case L2_BULKm:
        case L2_ENTRY_ONLY_TILEm:
        case L2_ENTRY_TILEm:
        case L2_LEARN_INSERT_FAILUREm:

        case EXACT_MATCH_2_ENTRY_ONLYm:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE1m:

        case EXACT_MATCH_4_ENTRY_ONLYm:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE1m:

    /* global views which already have _PIPE0-PIPE3 views */

        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (soc_td3_check_scrub_skip)\n"), SOC_MEM_NAME(unit, mem)));
        return 1;
    default:
    /*
     * Must NOT BE skipped
     * -------------------
     * LP views - they could have parity errors
     * Mems scanned by memscan - we need to scrub them
     * MEMm if scanning MEM_PIPE0-3m
 */
        break;
    }
    return 0;
}

int
soc_td3_mem_is_eligible_for_scan(int unit, soc_mem_t mem)
{
    switch (mem) {
        case L3_ENTRY_ONLY_ECCm:
        case L3_DEFIP_ALPM_ECCm:
        case FPEM_ECC_PIPE0m:
        case FPEM_ECC_PIPE1m:
        case FPEM_ECC_PIPE2m:
        case FPEM_ECC_PIPE3m:
        case L2_ENTRY_ISS_LPm:
        case L2_ENTRY_LPm:
        case L3_ENTRY_ISS_LPm:
        case L3_ENTRY_LPm:
        case FPEM_LPm:
        case VLAN_XLATE_LPm:
        case EGR_VLAN_XLATE_LPm:
        return 1;
        default:
        break;
    }
    return 0;
}

void
soc_td3_mem_scan_info_get(int unit, soc_mem_t mem, int block,
                         int *num_pipe, int *ser_flags)
{
    int k, num_inst_to_scrub = 0, acc_type_list[8], copyno = block;
    (void) soc_td3_check_scrub_info(unit, mem, block, copyno,
                                   &num_inst_to_scrub,
                                   acc_type_list);
    /* in case of SOC_FAILURE(rv), num_inst_to_scrub = 0, so
     * will be handled below as if we need to scrub only one
     * pipe */
    assert(num_inst_to_scrub <= 8);
    if (num_inst_to_scrub == 0) {
        *num_pipe = 1;
        ser_flags[0] = 0;
        if (!soc_mem_dmaable(unit, mem, block)) {
            ser_flags[0] = _SOC_SER_FLAG_NO_DMA;
        }
    } else {
        if (1 == NUM_PIPE(unit)) {
            
            num_inst_to_scrub = 1;
        }
        *num_pipe = num_inst_to_scrub;
        for (k = 0; k < num_inst_to_scrub; k++) {
            ser_flags[k] = 0;
            ser_flags[k] |= _SOC_SER_FLAG_DISCARD_READ;
            if (num_inst_to_scrub > 1) {
                ser_flags[k] |= _SOC_SER_FLAG_MULTI_PIPE;
            }
            ser_flags[k] |= acc_type_list[k];
            if (!soc_mem_dmaable(unit, mem, block)) {
                ser_flags[k] |= _SOC_SER_FLAG_NO_DMA;
            }
        }
    }
}

int
soc_td3_check_scrub_info(int unit, soc_mem_t mem, int blk, int copyno,
                        int *num_inst_to_scrub, int *acc_type_list)
{
    int mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    int mmu_base_type;
    soc_block_t blk_type;

    if (copyno == COPYNO_ALL) {
        return SOC_E_PARAM;
    }
    blk_type = SOC_BLOCK_TYPE(unit, copyno);

    if ((NULL == acc_type_list) || (NULL == num_inst_to_scrub)) {
        return SOC_E_PARAM;
    }

    *num_inst_to_scrub = 0;

    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return SOC_E_NONE;
    }

    if (((mem_acc_type == _SOC_TD3_ACC_TYPE_DUPLICATE) &&
         ((blk_type == SOC_BLK_IPIPE) || (blk_type == SOC_BLK_EPIPE)) &&
         (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL)) ||
        SOC_TD3_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem)) {
        /* 2nd last term: sw has not already created per-instance unique views */
        *num_inst_to_scrub = NUM_PIPE(unit);
        acc_type_list[0] = _SOC_TD3_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TD3_ACC_TYPE_UNIQUE_PIPE1;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d\n"),
                    SOC_MEM_NAME(unit,mem), blk, blk_type,
                    _SOC_TD3_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                    *num_inst_to_scrub));
        return SOC_E_NONE;
    } /* need_per_pipe_scrub */

    if ((mem_acc_type == _SOC_TD3_ACC_TYPE_DUPLICATE) &&
        ((blk_type == SOC_BLK_MMU_GLB) ||
         (blk_type == SOC_BLK_MMU_XPE) ||
         (blk_type == SOC_BLK_MMU_SC))) {

        mmu_base_type = SOC_MEM_BASE_TYPE(unit, mem);

        
        switch (blk_type) {
        case SOC_BLK_MMU_XPE:
            switch (mmu_base_type) {
            case SOC_TD3_MMU_BASE_TYPE_IPIPE:
                    *num_inst_to_scrub = 1;
                    acc_type_list[0] = 0;
                break;
            case SOC_TD3_MMU_BASE_TYPE_EPIPE:
                    *num_inst_to_scrub = 1;
                    acc_type_list[0] = 0;
                break;
            case SOC_TD3_MMU_BASE_TYPE_CHIP:
                /* must read each xpe instance */
                *num_inst_to_scrub = 1;
                acc_type_list[0] = 0;
                break;
            case SOC_TD3_MMU_BASE_TYPE_SLICE:
                    *num_inst_to_scrub = 1;
                    acc_type_list[0] = 0;
                break;
            case SOC_TD3_MMU_BASE_TYPE_LAYER:
                    *num_inst_to_scrub = 1;
                    acc_type_list[0] = 0;
                break;
            default: /* IPORT, EPORT, XPE */
                /* should not find any TD3 memories here */
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                          SOC_MEM_NAME(unit,mem), blk, blk_type,
                          _SOC_TD3_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                          *num_inst_to_scrub, mmu_base_type));
                return SOC_E_FAIL;
            } /* mmu_base_type */
            break; /* case: SOC_BLK_MMU_XPE */

        default: /* SOC_BLK_MMU_GLB,  SOC_BLK_MMU_SC */
            /* should not find any TD3 memories here */
            LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                      SOC_MEM_NAME(unit,mem), blk, blk_type,
                      _SOC_TD3_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                      *num_inst_to_scrub, mmu_base_type));
            return SOC_E_FAIL;
        } /* blk_type */
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                    SOC_MEM_NAME(unit,mem), blk, blk_type,
                    _SOC_TD3_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                    *num_inst_to_scrub, mmu_base_type));
        return SOC_E_NONE;
    } /* need_per_mmu_membase_scrub */
    return SOC_E_NONE;
}

/* For lookup operations to folded hash tables, SER event could be on LP view or
 * functional view (fv), even when search is issued to functional view.
 * In case when there is error on LP view, rsp_word
 * carries index to LP view and not the functinal view.
 * Following function helps remap the fv_index to lp_index.
 * It assumes that err_info received by caller is '1' meaning HW is indicating
 * that error is in LP view. Because this can happen only for folded hash
 * tables, any other mem passed as param to this function will result in return
 * with error from this function.
 */
int
soc_td3_lp_to_fv_index_remap(int unit, soc_mem_t mem, int *rpt_index)
{
    int em_lp_bank, em_lp_addr, index, rv = SOC_E_NONE;

    if (NULL == rpt_index) {
        return SOC_E_PARAM;
    }
    index = *rpt_index; /* default */
    switch(mem) {
    case VLAN_MACm:
    case VLAN_XLATEm:
        /* Error in VLAN_XLATE_LPm */
        index = index << 2; /* = 4*index; for cache read */
        break;
    case EGR_VLAN_XLATEm:
        /* Error in EGR_VLAN_XLATE_LPm */
        index = index << 2; /* = 4*index; for cache read */
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
        /* Error in FPEM_LPm */
        assert(index < 32768);
        em_lp_bank = (index >> 13) & 0x3; /* = index/8K; can be 0,1,2,3 */
        em_lp_addr = index & 0x1FFF; /* = index%8K; adr within lp bank */
        assert(em_lp_addr < 4096); /* For EM_2 */
        index = (16384 * em_lp_bank) + (4 * em_lp_addr);
                /* each lp bank is 8K deep, total of 4 lp banks, 1 per each uft_bank */
                /* each uft bank can have 16K EM_2 entries */
                /* each lp_entry covers 4 consec EM_2 entries */
                /* index is the addr of first em2_entry in the bkt pointed by
                 * lp_adr reported in err_rsp */
        index = 2*index;
                /* Each em2 entry has 2 FPEM_ECC entries.
                   Now index is addr of first fpem_ecc entry in the bkt pointed
                   by lp_adr reported in err_rsp */
        LOG_WARN(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, td3_lp_to_fv_index_remap: "
                        "mem %s, em_lp_bank %d, em_lp_addr %0d \n"),
             unit, SOC_MEM_NAME(unit, mem), em_lp_bank, em_lp_addr));
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        /* Error in FPEM_LPm */
        assert(index < 32768);
        em_lp_bank = (index >> 13) & 0x3; /* = index/8K; can be 0,1,2,3 */
        em_lp_addr = index & 0x1FFF; /* = index%8K; adr within lp bank */
        assert(em_lp_addr >= 4096); /* For EM_4 */
        assert(em_lp_addr < 6144); /* For EM_4 */
        index = (8192 * em_lp_bank) + (4 * (em_lp_addr - 4096));
                /* each lp bank is 8K deep, total of 4 lp banks, 1 per each uft_bank */
                /* each uft bank can have 8K EM_4 entries */
                /* each lp_entry covers 4 consec EM_4 entries */
                /* index is the addr of first em4_entry in the bkt pointed by
                 * lp_adr reported in err_rsp */
        index = 4*index;
                /* Each em4 entry has 4 FPEM_ECC entries.
                   Now index is addr of first fpem_ecc entry in the bkt pointed
                   by lp_adr reported in err_rsp */
        LOG_WARN(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, td3_lp_to_fv_index_remap: "
                        "mem %s, em_lp_bank %d, em_lp_addr %0d \n"),
             unit, SOC_MEM_NAME(unit, mem), em_lp_bank, em_lp_addr));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "soc_td3_lp_to_fv_index_remap: mem %s is not folded hash table !! index %d\n"),
                   SOC_MEM_NAME(unit, mem), index));
        rv = SOC_E_PARAM;
    }

    LOG_WARN(BSL_LS_SOC_SER,
        (BSL_META_U(unit,
                    "unit %d, td3_lp_to_fv_index_remap: "
                    "mem %s, rpt_lp_index %0d, fv_index %0d \n"),
         unit, SOC_MEM_NAME(unit, mem), *rpt_index, index));
    *rpt_index = index;
    return rv;
}

STATIC
int
_soc_td3_reg32_par_en_modify(int unit, soc_reg_t reg, soc_field_t field,
                            int enable)
{
    uint64 rval64;
    uint32 rval;
    int port = REG_PORT_ANY;
    static char *mmu_base_type[8] = {
        "IPORT", "EPORT", "IPIPE", "EPIPE", "CHIP", "XPE", "SLICE", "LAYER"};

    if (SOC_REG_IS_VALID(unit, reg) && INVALIDf != field) {
        if (SOC_REG_IS_64(unit, reg)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, port, 0, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, field, enable? 1 : 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, port, 0, rval64));
        } else {
            int at, bt;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, port, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, field,
                              enable ? 1 : 0);
            at = SOC_REG_ACC_TYPE(unit, reg);
            bt = SOC_REG_BASE_TYPE(unit, reg);
            if (20 == at) { /* ACC_TYPE_SINGLE - used only in MMU blocks */
                int inst, pipe, max_pipe;
                switch (bt) {
                case 2: /* IPIPE */
                case 3: /* EPIPE */
                    max_pipe = NUM_PIPE(unit); break;
                case 4: /* CHIP */
                    max_pipe = 1; break;
                case 5: /* XPE */
                    max_pipe = NUM_XPE(unit); break;
                case 6: /* SLICE */
                    max_pipe = NUM_SLICE(unit); break;
                case 7: /* LAYER */
                    max_pipe = NUM_LAYER(unit); break;
                default:
                    /* 0: IPORT */
                    /* 1: EPORT */
                    max_pipe = -1;
                    LOG_ERROR(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "unit %d, reg %s has unexpected "
                                    "base_type %s, will skip enable of "
                                    "1b err reporting\n"),
                         unit, SOC_REG_NAME(unit, reg),
                         mmu_base_type[bt]));
                    break;
                }
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "unit %d, reg %s has base_type %s\n"),
                     unit, SOC_REG_NAME(unit, reg),
                     mmu_base_type[bt]));
                for (pipe = 0; pipe < max_pipe; pipe++) {
                    inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, inst, 0, rval));
                    LOG_WARN(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "HF detected: setting reg.field[%s.%s]=0, mmu_base_type %s, pipe %d, using acc_type %0d\n"),
                        SOC_REG_NAME(unit, reg), SOC_FIELD_NAME(unit, field),
                        mmu_base_type[bt], pipe, at));
                }
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
                LOG_WARN(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "HF detected: setting reg.field[%s.%s]=0, using acc_type %0d (default)\n"),
                    SOC_REG_NAME(unit, reg), SOC_FIELD_NAME(unit, field),
                    at));
            }
        } /* reg32 */
        return SOC_E_NONE;
    }
    return SOC_E_UNAVAIL;
}

STATIC int
soc_td3_mem_parity_set(int unit, soc_mem_t mem, int copyno,
                      _soc_mem_ser_en_info_t *ser_info_list,
                      int en, int en_1b)
{
    int i = 0;

    for (i = 0; INVALIDm != ser_info_list[i].mem; i++) {
        if (ser_info_list[i].mem == mem) {
            if (ser_info_list[i].en_ctrl.flag_reg_mem == 0) {
                SOC_IF_ERROR_RETURN(
                    _soc_td3_reg32_par_en_modify(unit,
                                                ser_info_list[i].en_ctrl.ctrl_type.en_reg,
                                                ser_info_list[i].en_ctrl.en_fld,
                                                en));
                SOC_IF_ERROR_RETURN(
                    _soc_td3_reg32_par_en_modify(unit,
                                                ser_info_list[i].ecc1b_ctrl.ctrl_type.en_reg,
                                                ser_info_list[i].ecc1b_ctrl.en_fld,
                                                en_1b));
            }
            return SOC_E_NONE;
        }
    }

    return SOC_E_UNAVAIL;

}

int soc_td3_mem_parity_control(int unit, soc_mem_t mem, int copyno,
                              int en, int en_1b)
{
    int rv = SOC_E_NONE;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return SOC_E_UNAVAIL;
    }

    rv = soc_td3_mem_parity_set(unit, mem, copyno, SOC_IP_MEM_SER_INFO(unit), en, en_1b);
    if (rv == SOC_E_NONE) {
        return rv;
    }

    rv = soc_td3_mem_parity_set(unit, mem, copyno, SOC_EP_MEM_SER_INFO(unit), en, en_1b);
    if (rv == SOC_E_NONE) {
        return rv;
    }

    rv = soc_td3_mem_parity_set(unit, mem, copyno, SOC_MMU_MEM_SER_INFO(unit), en, en_1b);
    if (rv == SOC_E_NONE) {
        return rv;
    }

    return SOC_E_UNAVAIL;
}

#if defined(SER_TR_TEST_SUPPORT)

#define TR_MEM_NAME_SIZE_MAX    100
#ifdef SOC_MEM_NAME
#define TR_TEST_MEM_NAME_GET(_unit, _msg, _mem) do {                    \
        const char *_mem_name = SOC_MEM_NAME(_unit_, _mem);             \
        if (sal_strlen(_mem_name) < TR_MEM_NAME_SIZE_MAX) {             \
            sal_strcpy(_msg, _mem_name);                                \
        }                                                               \
    } while (0)
#else
#define TR_TEST_MEM_NAME_GET(_unit, _msg, _mem)
#endif

#define TR_TEST_MEM_PRINT(_unit, _msg, _mem) do {                       \
        sal_sprintf(_msg, "Mem ID: %d", _mem);                          \
        TR_TEST_MEM_NAME_GET(_unit, _msg, _mem);                        \
        LOG_WARN(BSL_LS_SOC_SER, (BSL_META_U(_unit,                     \
                            "Memory %s is valid, but not currently testable.\n"), \
                 _msg));                                                \
    } while (0)

#define _SER_TEST_MEM_TD3_ALPM_MASK(num_uft_banks) ((num_uft_banks) == 8? 0xFFFF : 0x7FFF)
#define _SER_TEST_MEM_TD3_ALPM_SHIFT(num_uft_banks) ((num_uft_banks) == 8? 16 : 15)

#define _SOC_TD3_TR144_LONG_SLEEP_TIME_US 200000

int
ser_test_td3_mem_index_remap(int unit, ser_test_data_t *test_data,
                            int *mem_has_ecc)
{
    int num_uft_banks = 0;
    int uft_bkt_bank = 0;
    int uft_le_fv = 0;
    int uft_le_ecc = 0;

    int remap_status = 0; /* no error */

    if ((NULL == test_data) || (NULL == mem_has_ecc)) {
        return SOC_E_PARAM;
    }
    if (!SOC_MEM_IS_VALID(unit, test_data->mem_fv)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d, mem %d is INVALID or not valid "
                              "for this unit !!\n"),
                   unit, test_data->mem_fv));
        return SOC_E_UNAVAIL;
    }
    *mem_has_ecc = 0;
    test_data->mem = test_data->mem_fv;
    test_data->index = test_data->index_fv;

    num_uft_banks = soc_trident3_get_alpm_banks(unit);
    switch (test_data->mem_fv) {
    case VLAN_XLATE_1_SINGLEm:
        test_data->mem = VLAN_XLATE_1_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case VLAN_XLATE_2_SINGLEm:
        test_data->mem = VLAN_XLATE_2_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case VLAN_XLATE_1_DOUBLEm:
        test_data->mem = VLAN_XLATE_1_ECCm;
        test_data->index = test_data->index_fv * 2;
        *mem_has_ecc = 1;
        break;
    case VLAN_XLATE_2_DOUBLEm:
        test_data->mem = VLAN_XLATE_2_ECCm;
        test_data->index = test_data->index_fv * 2;
        *mem_has_ecc = 1;
        break;
    case EGR_VLAN_XLATE_1_SINGLEm:
        test_data->mem = EGR_VLAN_XLATE_1_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case EGR_VLAN_XLATE_2_SINGLEm:
        test_data->mem = EGR_VLAN_XLATE_2_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case EGR_VLAN_XLATE_1_DOUBLEm:
        test_data->mem = EGR_VLAN_XLATE_1_ECCm;
        test_data->index = test_data->index_fv * 2;
        *mem_has_ecc = 1;
        break;
    case EGR_VLAN_XLATE_2_DOUBLEm:
        test_data->mem = EGR_VLAN_XLATE_2_ECCm;
        test_data->index = test_data->index_fv * 2;
        *mem_has_ecc = 1;
        break;
    case MPLS_ENTRYm:
        test_data->mem = MPLS_ENTRY_ECCm;
        test_data->index = test_data->index_fv * 2;
        *mem_has_ecc = 1;
        break;
    case MPLS_ENTRY_SINGLEm:
        test_data->mem = MPLS_ENTRY_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case L2Xm:
    case L2_ENTRY_ONLY_SINGLEm:
    case L2_ENTRY_SINGLEm:
        test_data->mem = L2_ENTRY_ECCm;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
        test_data->mem = L3_ENTRY_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_DOUBLEm:
        test_data->mem = L3_ENTRY_ECCm;
        test_data->index = test_data->index_fv*2;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_ONLY_QUADm:
    case L3_ENTRY_QUADm:
        test_data->mem = L3_ENTRY_ECCm;
        test_data->index = test_data->index_fv*4;
        *mem_has_ecc = 1;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
        test_data->mem = EXACT_MATCH_ECCm;
        test_data->index = test_data->index_fv*2;
        *mem_has_ecc = 1;
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
        test_data->mem = EXACT_MATCH_ECCm;
        test_data->index = test_data->index_fv*4;
        *mem_has_ecc = 1;
        break;
    case EXACT_MATCH_LPm:
        test_data->mem = EXACT_MATCH_LPm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case L3_DEFIP_ALPM_IPV4m: /* 6:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv %
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        uft_le_fv = test_data->index_fv /
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break;
        case 1: uft_le_ecc = 0; break; /* or 1 */
        case 2: uft_le_ecc = 1; break;
        case 3: uft_le_ecc = 2; break;
        case 4: uft_le_ecc = 2; break; /* or 3 */
        case 5: uft_le_ecc = 3; break;
        default: remap_status = -1; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index =
            uft_le_ecc * soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm) +
            uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_IPV4_1m: /* 4:4 */
    case L3_DEFIP_ALPM_IPV6_64m:
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        test_data->index = test_data->index_fv;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m: /* 3:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv %
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        uft_le_fv = test_data->index_fv /
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1 */
        case 1: uft_le_ecc = 1; break; /* or 2 */
        case 2: uft_le_ecc = 2; break; /* or 3 */
        default: remap_status = -1; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index =
            uft_le_ecc * soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm) +
            uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_IPV6_128m: /* 2:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv %
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        uft_le_fv = test_data->index_fv /
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1 */
        case 1: uft_le_ecc = 2; break; /* or 3 */
        default: remap_status = -1; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index =
            uft_le_ecc * soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm) +
            uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_RAWm: /* 1:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv %
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        uft_le_fv = test_data->index_fv /
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1,2,3 */
        default: remap_status = -1; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index =
            uft_le_ecc * soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm) +
            uft_bkt_bank;
        break;
    case MMU_INTFO_TC2PRI_MAPPINGm:
        test_data->mem = MMU_INTFO_TC2PRI_MAPPING0m;
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        test_data->mem = ING_DNAT_ADDRESS_TYPE_ECCm;
        test_data->index = test_data->index_fv / 2;
        *mem_has_ecc = 1;
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        test_data->mem = EGR_VP_VLAN_MEMBERSHIP_ECCm;
        test_data->index = test_data->index_fv / 2;
        *mem_has_ecc = 1;
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
        test_data->mem = ING_VP_VLAN_MEMBERSHIP_ECCm;
        test_data->index = test_data->index_fv / 2;
        *mem_has_ecc = 1;
        break;
    case SUBPORT_ID_TO_SGPP_MAPm:
        test_data->mem = SUBPORT_ID_TO_SGPP_MAP_ECCm;
        test_data->index = test_data->index_fv / 2;
        *mem_has_ecc = 1;
        break;
    default:
        test_data->mem = test_data->mem_fv;
        test_data->index = test_data->index_fv;
        break;
    }
    if ((test_data->mem != test_data->mem_fv) ||
        (test_data->index != test_data->index_fv)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, ser_test_td3_mem_index_remap: "
                        "mem_fv %s, index_fv %0d, mem %s, index %0d, "
                        "#uft_banks %0d, uft_bkt_bank %0d, uft_le_fv %0d, "
                        "uft_le_ecc %0d, remap_status %0d \n"),
             unit, SOC_MEM_NAME(unit, test_data->mem_fv), test_data->index_fv,
             SOC_MEM_NAME(unit, test_data->mem), test_data->index, num_uft_banks,
             uft_bkt_bank, uft_le_fv, uft_le_ecc, remap_status));
    }
    return remap_status;
}

static soc_ser_test_functions_t ser_trident3_test_fun;

typedef struct ser_td3_skipped_mem_s {
    soc_mem_t mem;
    soc_acc_type_t acc_type;
    int allow_error_inj;
} ser_td3_skipped_mem_t;

const ser_td3_skipped_mem_t td3_skipped_mems[] = {
    /* Report only views */
    { VLAN_XLATE_1_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { VLAN_XLATE_2_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { MPLS_ENTRY_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECC_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECC_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L2_ENTRY_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L3_ENTRY_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L3_DEFIP_ALPM_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { EGR_VLAN_XLATE_1_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { EGR_VLAN_XLATE_2_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { EXACT_MATCH_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { EXACT_MATCH_ECC_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 0 },

    /* FIFO Supports only POP operation */
    { CENTRAL_CTR_EVICTION_FIFOm, _SOC_ACC_TYPE_PIPE_ANY, 0 },

    { L2_BULKm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L2_LEARN_INSERT_FAILUREm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { MMU_THDU_UCQ_STATS_TABLEm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { MMU_THDU_UCQ_STATS_TABLE_XPE0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { MMU_THDU_UCQ_STATS_TABLE_XPE0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L2_LEARN_INSERT_FAILUREm, _SOC_ACC_TYPE_PIPE_ANY, 0 },

    { L3_DEFIP_ALPM_IPV4m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { L3_DEFIP_ALPM_IPV4_1m, _SOC_ACC_TYPE_PIPE_ANY, 1},
    { L3_DEFIP_ALPM_IPV6_64m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { L3_DEFIP_ALPM_IPV6_64_1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { L3_DEFIP_ALPM_IPV6_128m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { L3_DEFIP_ALPM_RAWm, _SOC_ACC_TYPE_PIPE_ANY, 1 },

    /* 2b error in IS_TDM_CALENDAR0,1 is fatal
     * Allow error_injection - but not tr 144 testing
     */
    { IS_TDM_CALENDAR0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },

    { TDM_CALENDAR0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK14m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK14_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK15m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK15_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK16m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK16_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK17m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK17_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK18m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK18_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK19m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK19_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK20m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK20_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK21m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK21_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK22m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK22_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK23m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK23_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK24m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK24_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK25m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK25_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK26m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK26_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK27m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK27_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK28m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK28_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK29m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK29_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK30m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK30_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK31m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK31_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DBm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED1_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED1_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_ECMP_FLOWSETm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_ECMP_FLOWSET_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_ECMP_FLOWSET_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_HGT_FLOWSETm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_HGT_FLOWSET_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_HGT_FLOWSET_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_LAG_FLOWSETm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_LAG_FLOWSET_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { RH_LAG_FLOWSET_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
#ifdef BCM_HELIX5_SUPPORT
    /* Read-only memory */
    { BSC_AG_AGE_TABLEm , _SOC_ACC_TYPE_PIPE_ANY, 0 },

    /* Not cached and SER_RESPONSE => NONE */
    { BSC_DT_FLEX_SESSION_DATA_SINGLEm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { BSC_DT_FLEX_SESSION_DATA_DOUBLEm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
#endif

    /* End */
    { INVALIDm }
};

STATIC soc_acc_type_t
_soc_trident3_pipe_to_acc_type(int pipe)
{
    soc_acc_type_t rv;
    switch (pipe) {
        case SOC_PIPE_SELECT_ANY:
        case SOC_PIPE_SELECT_0: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_0; break;
        case SOC_PIPE_SELECT_1: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_1; break;
        default: rv =_SOC_ACC_TYPE_PIPE_ALL; break;
    }
    return rv;
    /* For TD3, ONLY possible values of test_data->acc_type
     * are: ACC_TYPE_PIPE_0, 1, ALL
     */
}

STATIC soc_pipe_select_t
_soc_trident3_acc_type_to_pipe(soc_acc_type_t acc_type)
{
    soc_pipe_select_t rv;
        switch (acc_type) {
        case _SOC_UNIQUE_ACC_TYPE_PIPE_0: rv = SOC_PIPE_SELECT_0; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_1: rv = SOC_PIPE_SELECT_1; break;
        case _SOC_ACC_TYPE_PIPE_ANY:  rv = SOC_PIPE_SELECT_ANY; break;
        default:                      rv = SOC_PIPE_SELECT_ALL; break;
        }
    return rv;
}

int
soc_trident3_pipe_select(int unit, int egress, int pipe)
{
    soc_reg_t reg;

    reg = egress ? EGR_SBS_CONTROLr : SBS_CONTROLr;
    if ((pipe >= 0) && (pipe <= 1)) {
        return soc_reg_field32_modify(unit, reg, REG_PORT_ANY, PIPE_SELECTf,
                                      pipe);
    } else {
        return soc_reg_field32_modify(unit, reg, REG_PORT_ANY, PIPE_SELECTf, 0);
    }
}

soc_error_t
soc_td3_ser_parity_control_reg_set(int unit, ser_test_data_t *test_data, int enable)
{
    
    uint32 rval;
    uint64 regData;

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, test_data->parity_enable_reg, test_data->port, 0,
                       &rval));
    LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "soc_td3_ser_parity_control_reg_set READ: reg %s, rval_rdat = 0x%x, bit %d \n"),
                   SOC_REG_NAME(unit, test_data->parity_enable_reg),
                   rval,
                   test_data->tcam_parity_bit
                   ));
    if (enable) {
        rval |= (1 << test_data->tcam_parity_bit);
    } else {
        rval &= ~(1 << test_data->tcam_parity_bit);
    }
    LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "soc_td3_ser_parity_control_reg_set WROTE: reg %s, rval_wdat = 0x%x, bit %d \n"),
                   SOC_REG_NAME(unit, test_data->parity_enable_reg),
                   rval,
                   test_data->tcam_parity_bit
                   ));
    COMPILER_64_SET(regData, 0, rval);
    return soc_reg_set(unit, test_data->parity_enable_reg, test_data->port, 0,
                       regData);
}

STATIC soc_error_t
_ser_td3_ser_support_mem_found(int unit, soc_mem_t mem,
                                      _soc_mem_ser_en_info_t *ser_info_list)
{
    int i = 0;

    for (i = 0; ser_info_list[i].mem != INVALIDm; i++) {
        if ((ser_info_list[i].mem == mem) &&
           (soc_mem_index_count(unit, mem))) {
            return SOC_E_NONE; /* found */
        }
    }

    return SOC_E_UNAVAIL;
}

soc_error_t
soc_td3_ser_error_injection_support(int unit, soc_mem_t mem,
                                    int pipe_target)
{
    int rv = SOC_E_UNAVAIL;
    int i, p;
    _soc_td3_tcam_wrapper_info_t *tcams = _soc_td3_tcam_wrapper_table;
    soc_acc_type_t soc_acc_type_target =
        _soc_trident3_pipe_to_acc_type(pipe_target);

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d, mem %s is INVALID or not valid "
                              "or parity is disabled for this mem !!\n"),
                   unit, SOC_MEM_NAME(unit,mem)));
        return rv;
    }

    /* Check if memory is listed in skipped mems structure */
    for (i = 0; td3_skipped_mems[i].mem != INVALIDm; i++) {
        if ((td3_skipped_mems[i].mem == mem) &&
            ((_SOC_ACC_TYPE_PIPE_ANY == td3_skipped_mems[i].acc_type) ||
             (td3_skipped_mems[i].acc_type == soc_acc_type_target))) {
            if (td3_skipped_mems[i].allow_error_inj) {
                return SOC_E_NONE;
            } else {
                return rv;
            }
        }
    }

    /* Search TCAMs */
    for (i = 0; tcams[i].mem != INVALIDm; i++) {
        if(!SOC_REG_FIELD_VALID(unit, tcams[i].enable_reg, tcams[i].enable_field)) {
           continue;
        }
        if (tcams[i].mem == mem) {
            return SOC_E_NONE; /* found */
        }
        if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem) != NULL) {
            for (p = 0; p < NUM_PIPE(unit); p++) {
                if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem)[p] == mem) {
                    return SOC_E_NONE; /* found */
                }
            }
        }
    }

   /* Search for mems listed in:
    * _soc_bcm56870_a0_ip_mem_ser_info,
    * _soc_bcm56870_a0_ep_mem_ser_info,
    * _soc_bcm56870_a0_mmu_mem_ser_info,
    */
   rv = _ser_td3_ser_support_mem_found(unit, mem, SOC_IP_MEM_SER_INFO(unit));
   if(rv == SOC_E_NONE) {
       return rv;
   }
   rv = _ser_td3_ser_support_mem_found(unit, mem, SOC_EP_MEM_SER_INFO(unit));
   if(rv == SOC_E_NONE) {
       return rv;
   }
   rv = _ser_td3_ser_support_mem_found(unit, mem, SOC_MMU_MEM_SER_INFO(unit));
   if(rv == SOC_E_NONE) {
       return rv;
   }

   return rv;
}

soc_error_t
soc_td3_ser_correction_info_get(int unit,
                                ser_correction_info_t *corr_info)
{
    if (!SOC_MEM_IS_VALID(unit, corr_info->inject_mem)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d, mem %d is INVALID or not valid "
                              "for this unit !!\n"),
                   unit, corr_info->inject_mem));
        return SOC_E_UNAVAIL;
    }
    switch (corr_info->inject_mem) {
     /* compare index only */
    case VLAN_MACm:
    case VLAN_XLATEm:
    case EGR_VLAN_XLATEm:
    case MPLS_ENTRYm:
    case MODPORT_MAP_M0m:
    case MODPORT_MAP_M1m:
    case MODPORT_MAP_M2m:
    case MODPORT_MAP_M3m:
    case L2_ENTRY_TILEm:
    case L2_ENTRY_ONLY_TILEm:
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
        corr_info->compare_mem = 0;
        corr_info->compare_index = 1;
        break;

     /* must NOT compare view or index */
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
    case L3_DEFIP_ALPM_IPV4m:
    case L3_DEFIP_ALPM_IPV4_1m:
    case L3_DEFIP_ALPM_IPV6_64m:
    case L3_DEFIP_ALPM_IPV6_64_1m:
    case L3_DEFIP_ALPM_IPV6_128m:
    case L3_DEFIP_ALPM_RAWm:
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_ENTRY_ONLYm:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_ENTRY_ONLYm:
    case FPEM_LPm:
        corr_info->compare_mem = 0;
        corr_info->compare_index = 0;
        break;

    /* can compare both view and index */
    default:
        corr_info->compare_mem = 1;
        corr_info->compare_index = 1;
        break;
    }

    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit,
                    "==== predict: mem = %s, "
                    "compare_mem,index = %d,%d\n"),
         SOC_MEM_NAME(unit, corr_info->inject_mem),
         corr_info->compare_mem,
         corr_info->compare_index));

    return SOC_E_NONE;
}

typedef struct ser_td3_mems_test_field_s {
    soc_mem_t mem;
    soc_acc_type_t acc_type;
    soc_field_t test_field;
} ser_td3_mems_test_field_t;

const ser_td3_mems_test_field_t td3_mems_test_field[] = {
    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP0f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP2f },
    { EGR_VLAN_VFI_MEMBERSHIPm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP0f },
    { EGR_VLAN_VFI_MEMBERSHIPm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP1f },
    { EGR_VLAN_VFI_UNTAGm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP0f },
    { EGR_VLAN_VFI_UNTAGm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP1f },

    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP_P0f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP_P1f },

    { ING_L3_NEXT_HOPm, _SOC_ACC_TYPE_PIPE_ANY, ECC_1f},

    { INVALIDm, _SOC_ACC_TYPE_PIPE_ANY, INVALIDf }
};

STATIC void
_soc_td3_ser_find_test_field(int unit, soc_mem_t mem,
                         soc_acc_type_t soc_acc_type_target,
                         soc_field_t *test_field)
{
    int i;
    for (i = 0; td3_mems_test_field[i].mem != INVALIDm; i++) {
        if ((td3_mems_test_field[i].mem == mem) &&
            ((td3_mems_test_field[i].acc_type == soc_acc_type_target) ||
             (td3_mems_test_field[i].acc_type == _SOC_ACC_TYPE_PIPE_ANY))
           ) {
            if (!SOC_MEM_FIELD_VALID(unit, mem, td3_mems_test_field[i].test_field)) {
                continue;
            }
            *test_field = td3_mems_test_field[i].test_field;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "===== Selected field %s for Memory %s \n"),
                 SOC_FIELD_NAME(unit, *test_field),
                 SOC_MEM_NAME(unit,mem)));
            break;
        }
    }
    return;
}

/*
 * Function:
 *      _soc_trident3_perform_ser_test
 * Purpose:
 *      Performs test operations common to TCAM and FIFO memory tests before
 *      invoking common SER test.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_data   - (IN) Test data required for SER test
 *      test_type   - (IN) How many indices to test for each memory
 *      mem_failed  - (OUT) Incremented when memories fail the test.
 * Returns:
 *  SOC_E_NONE if test passes, an error otherwise (multiple types of errors are
 *      possible.)
 */
STATIC soc_error_t
_soc_trident3_perform_ser_test(int unit, uint32 flags, ser_test_data_t *test_data,
                               _soc_ser_test_t test_type,
                               int *mem_skipped, int *mem_failed)
{
    int k, skip_mem = FALSE;
    soc_error_t rv = SOC_E_NONE;
    soc_pipe_select_t pipe_target;
    uint32 mem_acc_type;

    if (!SOC_MEM_IS_VALID(unit, test_data->mem_fv)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d, mem %d is INVALID or not valid "
                              "for this unit !!\n"),
                   unit, test_data->mem_fv));
        skip_mem = TRUE;
    }
    for (k = 0; td3_skipped_mems[k].mem != INVALIDm; k++) {
        if ((td3_skipped_mems[k].mem == test_data->mem_fv) &&
            ((_SOC_ACC_TYPE_PIPE_ANY == td3_skipped_mems[k].acc_type) ||
             (td3_skipped_mems[k].acc_type == test_data->acc_type))) {
            skip_mem = TRUE;
            break;
        }
    }
    /* skip mems for which SW does not need to do error correction */
    if (!(SOC_MEM_INFO(unit, test_data->mem_fv).flags & SOC_MEM_SER_FLAGS)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "SOC_MEM_SER_FLAGS is not set for %s "
                        "(flags 0x%8x). Skipping.\n"),
             SOC_MEM_NAME(unit, test_data->mem_fv),
             SOC_MEM_INFO(unit,test_data->mem_fv).flags));
        skip_mem = TRUE;
    }
    /* skip mems which could be of 0 depth due to config properties */
    /* coverity[overrun-call] */
    if (!soc_mem_index_count(unit, test_data->mem_fv))	{
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "Index_Count is 0 for %s (flags 0x%8x). Skipping.\n"),
             SOC_MEM_NAME(unit, test_data->mem_fv),
             SOC_MEM_INFO(unit,test_data->mem_fv).flags));
        skip_mem = TRUE;
    }
    /* skip mems which have addr_split */
    mem_acc_type = SOC_MEM_ACC_TYPE(unit, test_data->mem_fv);
    if ((mem_acc_type == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_DIST) ||
        (mem_acc_type == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_SPLIT)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "Memory %s has acc_type ADDR_SPLIT (%0d) \n"),
             SOC_MEM_NAME(unit,test_data->mem_fv), mem_acc_type));
        /* skip_mem = TRUE; */
    }

    if (!skip_mem) {
        pipe_target = _soc_trident3_acc_type_to_pipe(test_data->acc_type);
        _soc_td3_ser_find_test_field(unit, test_data->mem_fv, test_data->acc_type,
                                 &test_data->test_field);
        soc_trident3_pipe_select(unit, TRUE, pipe_target);
        soc_trident3_pipe_select(unit, FALSE, pipe_target);
/*
        if (_soc_td3_check_counter_with_ecc(unit, test_data->mem_fv)) {
            flags = flags | SOC_INJECT_ERROR_2BIT_ECC;
        }
*/
        rv = ser_test_mem(unit, flags, test_data, test_type, mem_failed);
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "===== ser_test for Memory %s, Acc_type %d FAILED !!"
                            " \n"),
                 SOC_MEM_NAME(unit,test_data->mem_fv), test_data->acc_type));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "===== ser_test for Memory %s, Acc_type %d PASSED !!"
                            " \n"),
                 SOC_MEM_NAME(unit,test_data->mem_fv), test_data->acc_type));
        }
        soc_trident3_pipe_select(unit, TRUE, 0);
        soc_trident3_pipe_select(unit, FALSE, 0);
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "===== ser_test for Memory %s, Acc_type %d "
                        "SKIPPED \n"),
             SOC_MEM_NAME(unit,test_data->mem_fv), test_data->acc_type));
        (*mem_skipped)++;
    }
    return rv;
}

STATIC int
_soc_td3_refresh_modify(int unit, int enable) {
    struct {
        soc_reg_t ref_reg;
        soc_field_t en_field;
    } rf_list[] = {
        {AUX_ARB_CONTROLr, FP_REFRESH_ENABLEf},
        {EFP_METER_CONTROLr, EFP_REFRESH_ENABLEf},
        {MMU_GCFG_MISCCONFIGr, REFRESH_ENf},
        {INVALIDr, INVALIDf}
    };
    uint32 rval;
    int i;
    for (i = 0; rf_list[i].ref_reg != INVALIDr; i++) {
        if (!SOC_REG_IS_VALID(unit, rf_list[i].ref_reg)) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, rf_list[i].ref_reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, rf_list[i].ref_reg, &rval, rf_list[i].en_field,
                          enable ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, rf_list[i].ref_reg, REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

soc_error_t
soc_td3_test_mem_create(int unit, soc_mem_t mem, int block, int index,
                        uint32 *flags, soc_acc_type_t soc_acc_type_target,
                        uint32 * tmp_entry, uint32 * field_data,
                        ser_test_data_t * p_test_data,
                        _soc_mem_ser_en_info_t *ser_info_list)
{
    int i = 0 ,found_mem = FALSE;
    soc_field_t test_field = INVALIDf;
    int mem_has_ecc = 0;
    soc_mem_t en_mem;
    soc_reg_t en_reg;

    for (i = 0; ser_info_list[i].mem != INVALIDm; i++) {
        if ((ser_info_list[i].mem == mem) &&
            (soc_mem_index_count(unit, mem))) {
            test_field = ECCf;
            _soc_td3_ser_find_test_field(unit, mem, soc_acc_type_target,
                                     &test_field);

            en_reg = ser_info_list[i].en_ctrl.flag_reg_mem ? INVALIDr :
                     ser_info_list[i].en_ctrl.ctrl_type.en_reg;
            en_mem = ser_info_list[i].en_ctrl.flag_reg_mem ? 
                     ser_info_list[i].en_ctrl.ctrl_type.en_mem : INVALIDm;
            soc_ser_create_test_data_with_new_format(unit, tmp_entry, field_data,
                                             en_reg,
                                             en_mem,
                                             SOC_INVALID_TCAM_PARITY_BIT,
                                             ser_info_list[i].en_ctrl.en_fld,
                                             ser_info_list[i].en_ctrl.en_fld_position,
                                             mem, test_field, block,
                                             REG_PORT_ANY,
                                             soc_acc_type_target,
                                             index, p_test_data);
            found_mem = TRUE;
            if ((SOC_REG_IS_VALID(unit, ser_info_list[i].ecc1b_ctrl.ctrl_type.en_reg) ||
                 SOC_MEM_IS_VALID(unit, ser_info_list[i].ecc1b_ctrl.ctrl_type.en_mem)) &&
                (INVALIDf != ser_info_list[i].ecc1b_ctrl.en_fld)) {
                *flags |= SOC_INJECT_ERROR_2BIT_ECC;
            }

            SOC_IF_ERROR_RETURN
                     (ser_test_mem_index_remap(unit, p_test_data, &mem_has_ecc));
            if (mem_has_ecc) {
                *flags |= SOC_INJECT_ERROR_2BIT_ECC;
            }
            break;
        }
    }

    if (found_mem) {
        return SOC_E_NONE;
    }else {
        return SOC_E_UNAVAIL;
    }

}

/*
 * Function:
 *      soc_td3_ser_inject_or_test_mem
 * Purpose:
 *      Provide a common function for injecting errors and testing single
 *      single memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe (x/y) to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 *      test_type   - (IN) How many indices to test in the passes memory
 *      cmd         - (IN) TRUE if a command-line test is desired.
 * Returns:
 *      SOC_E_NONE if test passes, an error otherwise (multiple types of errors
 *      are possible.)
 */
soc_error_t
soc_td3_ser_inject_or_test_mem(int unit, soc_mem_t mem, int pipe_target, int block,
                              int index, _soc_ser_test_t test_type,
                              int inject_only, int cmd, uint32 flags)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], field_data[SOC_MAX_MEM_FIELD_WORDS];
    ser_test_data_t test_data;
    _soc_td3_tcam_wrapper_info_t *tcams = _soc_td3_tcam_wrapper_table;
    int i, p, error_count = 0, skip_count = 0, found_mem = FALSE;
    char fail_message[TR_MEM_NAME_SIZE_MAX + 1];
    soc_error_t rv = SOC_E_NONE;
    soc_acc_type_t soc_acc_type_target, mem_acc_type;
    ser_correction_info_t corr_info;

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d, mem %s is INVALID or not valid "
                              "or parity is disabled for this mem !!\n"),
                   unit, SOC_MEM_NAME(unit,mem)));
        return SOC_E_UNAVAIL;
    }

    mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    if (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_1) {
        soc_acc_type_target = mem_acc_type;
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "soc_td3_ser_inject_or_test_mem: unit %d, mem %s, "
                        "pipe_target %d, BUT mem_acc_type %d will be "
                        "used\n"),
             unit, SOC_MEM_NAME(unit, mem),
             pipe_target, soc_acc_type_target));
    } else if ((mem_acc_type == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_DIST) ||
               (mem_acc_type == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_SPLIT)) {
        /* 1. ignore pipe_target, because pipe is determined by index param
         * 2. For such mem, ser_en_info in bcm56870_a0.c file will not have
         *    _PIPE0,1 views (these are created only for mems with UNIQUE
         *    acc type.
         */
        soc_acc_type_target = _SOC_ACC_TYPE_PIPE_ANY;
    } else {
        soc_acc_type_target = _soc_trident3_pipe_to_acc_type(pipe_target);
    }

    corr_info.inject_mem = mem;
    soc_td3_ser_correction_info_get(unit, &corr_info);

    /* Check if the memory is to be skipped */
    for (i = 0; td3_skipped_mems[i].mem != INVALIDm; i++) {
        if ((td3_skipped_mems[i].mem == mem) &&
            ((_SOC_ACC_TYPE_PIPE_ANY == td3_skipped_mems[i].acc_type) ||
             (td3_skipped_mems[i].acc_type == soc_acc_type_target))) {
            /* coverity[secure_coding] */
            if (inject_only && td3_skipped_mems[i].allow_error_inj) {
                break;
            }
            TR_TEST_MEM_PRINT(unit, fail_message, mem);
            return SOC_E_UNAVAIL;
        }
    }

    /* Search for TCAM memories */
    for (i = 0; tcams[i].mem != INVALIDm; i++) {
        if (!SOC_REG_FIELD_VALID(unit, tcams[i].enable_reg, tcams[i].enable_field)) {
            continue;
        }
        if (tcams[i].mem == mem) {
            soc_ser_create_test_data_with_new_format(unit, tmp_entry, field_data,
                                                     tcams[i].enable_reg,
                                                     INVALIDm,
                                                     SOC_INVALID_TCAM_PARITY_BIT,
                                                     tcams[i].enable_field,
                                                     -1, mem,
                                                     tcams[i].parity_field, block,
                                                     REG_PORT_ANY,
                                                     soc_acc_type_target,
                                                     index, &test_data);
            found_mem = TRUE;
            break; /*A memory was found that matched, so stop searching.*/
        }
        if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem) != NULL) {
            for (p = 0; p < NUM_PIPE(unit); p++) {
                if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem)[p] == mem) {
                    /*Note: Need to confirm if the enable register's acc_type is unique,
                    * once set the parity enable with global view whether the each pipe's enable
                    * register can be set correctly
                    */
                    soc_ser_create_test_data_with_new_format(unit, tmp_entry,
                                                             field_data,
                                                             tcams[i].enable_reg,
                                                             INVALIDm,
                                                             SOC_INVALID_TCAM_PARITY_BIT,
                                                             tcams[i].enable_field,
                                                             -1, mem,
                                                             tcams[i].parity_field,
                                                             block, REG_PORT_ANY,
                                                             soc_acc_type_target,
                                                             index, &test_data);
                    found_mem = TRUE;
                    break; /* from pipe loop */
                }
            }
        }
        if (found_mem) {
            break;
        }
    }

   /* Search for mems listed in:
    * _soc_bcm56870_a0_ip_mem_ser_info,
    * _soc_bcm56870_a0_ep_mem_ser_info
    */
    if (!found_mem) {
        rv = soc_td3_test_mem_create(unit, mem, block, index, &flags,
                                     soc_acc_type_target, tmp_entry,
                                     field_data, &test_data,
                                     SOC_IP_MEM_SER_INFO(unit));
        if (rv == SOC_E_NONE) {
            found_mem = TRUE;
        }else {
            rv = soc_td3_test_mem_create(unit, mem, block, index, &flags,
                                         soc_acc_type_target, tmp_entry,
                                         field_data, &test_data,
                                         SOC_EP_MEM_SER_INFO(unit));
            if (rv == SOC_E_NONE) {
                found_mem = TRUE;
            } else {
                rv = soc_td3_test_mem_create(unit, mem, block, index, &flags, _SOC_ACC_TYPE_PIPE_ANY,
                tmp_entry, field_data, &test_data, SOC_MMU_MEM_SER_INFO(unit));
                if (rv == SOC_E_NONE) {
                    found_mem = TRUE;
                }
            }
        }
    }

    if (found_mem) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
           (BSL_META_U(unit,
                       "soc_td3_ser_inject_or_test_mem: unit %d, mem %s, "
                       "pipe_target %d, block %d, index %d, test_type %d, "
                       "inject_only %d, cmd %d, flags 0x%x \n"),
            unit, SOC_MEM_NAME(unit, mem),
            pipe_target, block, index, test_type, inject_only, cmd, flags));
        if (inject_only) {
            /* Set pipe select */
            soc_trident3_pipe_select(unit, TRUE, pipe_target);
            soc_trident3_pipe_select(unit, FALSE, pipe_target);

            /* Disable parity */
            SOC_IF_ERROR_RETURN(_ser_test_parity_control(unit, &test_data, 0));

            /* Read the memory (required for successful injection) */
            rv = ser_test_mem_read(unit, 0, &test_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                   "soc_td3_ser_inject_or_test_mem: unit %d, Error_inject: Read for mem %s FAILED\n"),
                   unit, SOC_MEM_NAME(unit, mem)));
                return rv;
            }

            /* Inject error */
            rv = soc_ser_test_inject_full(unit, flags, &test_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                   "soc_td3_ser_inject_or_test_mem: unit %d, Error_inject: error inject for mem %s FAILED\n"),
                   unit, SOC_MEM_NAME(unit, mem)));
                return rv;
            }

            /* Enable parity */
            SOC_IF_ERROR_RETURN(_ser_test_parity_control(unit, &test_data, 1));

            soc_trident3_pipe_select(unit, TRUE, 0);
            soc_trident3_pipe_select(unit, FALSE, 0);
        } else {
            if (cmd) {
                ser_test_cmd_generate(unit, &test_data);
            } else {
                int def_soc_ser_test_long_sleep;
                int def_soc_ser_test_long_sleep_time_us;

                /* save default values, choose TD3 specific values  */
                def_soc_ser_test_long_sleep = soc_ser_test_long_sleep;
                def_soc_ser_test_long_sleep_time_us = soc_ser_test_long_sleep_time_us;
                soc_ser_test_long_sleep = TRUE;
                soc_ser_test_long_sleep_time_us =
                    soc_property_get(unit, "tr144_long_sleep_time_us",
                                    _SOC_TD3_TR144_LONG_SLEEP_TIME_US);
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                               "tr 144 test will use:long_sleep = %d,"
                               "long_sleep_time = %d uS\n"),
                     soc_ser_test_long_sleep,
                     soc_ser_test_long_sleep_time_us));

                (void) _soc_td3_refresh_modify(unit, 0); /* disable refresh */
                rv = _soc_trident3_perform_ser_test(unit, flags, &test_data, test_type,
                                                   &skip_count, &error_count);
                (void) _soc_td3_refresh_modify(unit, 1); /* re-enable refresh */

                /* restore default values */
                soc_ser_test_long_sleep = def_soc_ser_test_long_sleep;
                soc_ser_test_long_sleep_time_us = def_soc_ser_test_long_sleep_time_us;

                if (!SOC_FAILURE(rv)) {
                    if ((error_count == 0) && !skip_count) {
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                       "SER test PASSED for memory"
                                       "%s\n"),
                             test_data.mem_name));
                    }
                    if (skip_count !=0) {
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                       "SER test SKIPPED for memory"
                                       "%s\n"),
                             test_data.mem_name));
                    }
                }
            }
        }
    } else {
        TR_TEST_MEM_PRINT(unit, fail_message, mem);
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}

/*
 * Function:
 *      soc_td3_ser_inject_error
 * Purpose:
 *      Injects an error into a single td3 memory
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 */
STATIC soc_error_t
soc_td3_ser_inject_error(int unit, uint32 flags, soc_mem_t mem, int pipe_target,
                        int block, int index)
{
    return soc_td3_ser_inject_or_test_mem(unit, mem, pipe_target, block, index,
                                         SER_SINGLE_INDEX, TRUE, FALSE, flags);
}

/*
 * Function:
 *      soc_td3_ser_test_mem
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
soc_td3_ser_test_mem(int unit, soc_mem_t mem, _soc_ser_test_t test_type,
                    int cmd)
{
    return soc_td3_ser_inject_or_test_mem(unit, mem, -1, MEM_BLOCK_ANY, 0,
                                         test_type, FALSE, cmd, FALSE);
}

/*
 * Function:
 *      soc_td3_ser_tcam_test
 * Purpose:
 *      Tests that SER is working for fifo and SRAM memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  the number of tests which are failed
 */
STATIC int
soc_td3_ser_tcam_test(int unit, _soc_ser_test_t test_type)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], field_data[SOC_MAX_MEM_FIELD_WORDS];
    ser_test_data_t test_data;
    int mem_failed = 0, mem_tests = 0, mem_skipped = 0;
    int i;
    int mem_index = 0; /* dont_care */
    soc_acc_type_t wrapper_at;

    for (i = 0; _soc_td3_tcam_wrapper_table[i].mem != INVALIDm; i++) {
        if(!SOC_REG_FIELD_VALID(unit, _soc_td3_tcam_wrapper_table[i].enable_reg,
                                _soc_td3_tcam_wrapper_table[i].enable_field)) {
           continue;
        }
        for (wrapper_at = _SOC_UNIQUE_ACC_TYPE_PIPE_0;
             wrapper_at < NUM_PIPE(unit);
             wrapper_at++) {
            mem_tests++;
            if (!SOC_MEM_IS_VALID(unit, _soc_td3_tcam_wrapper_table[i].mem) ||
                (SOC_MEM_INFO(unit, _soc_td3_tcam_wrapper_table[i].mem).flags &
                 SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "unit %d, mem %s is INVALID or not valid "
                                      "or parity is disabled for this mem !!\n"),
                           unit, SOC_MEM_NAME(unit, _soc_td3_tcam_wrapper_table[i].mem)));
                mem_skipped++;
                continue;
            }

            soc_ser_create_test_data_with_new_format(unit, tmp_entry, field_data,
                                                     _soc_td3_tcam_wrapper_table[i].enable_reg,
                                                     INVALIDm, SOC_INVALID_TCAM_PARITY_BIT,
                                                     _soc_td3_tcam_wrapper_table[i].enable_field,
                                                     -1, _soc_td3_tcam_wrapper_table[i].mem,
                                                     _soc_td3_tcam_wrapper_table[i].parity_field,
                                                     MEM_BLOCK_ANY, REG_PORT_ANY,
                                                     wrapper_at,
                                                     mem_index, &test_data);

            _soc_trident3_perform_ser_test(unit, 0, &test_data, test_type,
                                           &mem_skipped, &mem_failed);
        }
    }

    LOG_CLI((BSL_META_U(unit, "\nTCAM memories tested on unit %d: %d\n"),
             unit, mem_tests - mem_skipped));
    LOG_CLI((BSL_META_U(unit, "TCAM tests passed:\t%d\n"),
             mem_tests - mem_failed - mem_skipped));
    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit, "TCAM tests skipped:\t%d \n"),
         0));
    LOG_CLI((BSL_META_U(unit, "TCAM tests failed:\t%d\n\n"), mem_failed));
    return mem_failed;
}

STATIC int
soc_td3_ser_block_test(int unit, _soc_ser_test_t test_type,
                       int *p_mem_failed, int *p_mem_tests, int *p_mem_skipped,
                       _soc_mem_ser_en_info_t *ser_info_list, soc_block_t blocktype)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], field_data[SOC_MAX_MEM_FIELD_WORDS];
    ser_test_data_t test_data;
    int num_inst_to_test = 0;
    int i;
    uint32 mem_acc_type, flags = 0;
    soc_acc_type_t acc_type;
    int mem_index = 0;
    soc_mem_t en_mem = INVALIDm;
    soc_reg_t en_reg = INVALIDr;
#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
    int rv = SOC_E_NONE;
#endif

    for (i = 0; ser_info_list[i].mem != INVALIDm; i++) {
        if (!SOC_MEM_IS_VALID(unit, ser_info_list[i].mem) ||
            !soc_mem_index_count(unit, ser_info_list[i].mem) ||
            (SOC_MEM_INFO(unit, ser_info_list[i].mem).flags &
             SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "unit %d, mem %s is INVALID or not valid "
                                  "or parity is disabled for this mem !!\n"),
                       unit, SOC_MEM_NAME(unit, ser_info_list[i].mem)));
            continue;
        }
        flags = 0; /* for every mem */
        mem_acc_type = SOC_MEM_ACC_TYPE(unit, ser_info_list[i].mem);
        if (blocktype == SOC_BLK_MMU_GLB) {
            num_inst_to_test = 1;
            acc_type = _SOC_ACC_TYPE_PIPE_ANY;
        } else if (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_1) {
            num_inst_to_test = 1;
            acc_type = mem_acc_type;
        } else if ((mem_acc_type == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_DIST) ||
                   (mem_acc_type == _SOC_TD3_ACC_TYPE_ADDR_SPLIT_SPLIT)) {
            /*
             * For such mems, pipe is determined by address.
             * Ideally, we want to test all 4 pipes, but as
             * simplification, we may test only 3 instances if tr test
             * is being run with T=spread option (because addr=0 means
             * pipe0, addr=last means pipe3 and addr=last/2 means
             * pipe1/pipe2
             */
            num_inst_to_test = 1;
            acc_type = _SOC_ACC_TYPE_PIPE_ANY;
        } else {
            /* test all instances */
            num_inst_to_test = NUM_PIPE(unit);
            acc_type = _SOC_UNIQUE_ACC_TYPE_PIPE_0;
        }
        *p_mem_tests += num_inst_to_test;
        test_data.test_field = ECCf;
        en_reg = ser_info_list[i].en_ctrl.flag_reg_mem ? INVALIDr :
                 ser_info_list[i].en_ctrl.ctrl_type.en_reg;
        en_mem = ser_info_list[i].en_ctrl.flag_reg_mem ?
                 ser_info_list[i].en_ctrl.ctrl_type.en_mem : INVALIDm;
        soc_ser_create_test_data_with_new_format(unit, tmp_entry, field_data,
                                 en_reg, en_mem, SOC_INVALID_TCAM_PARITY_BIT,
                                 ser_info_list[i].en_ctrl.en_fld,
                                 ser_info_list[i].en_ctrl.en_fld_position,
                                 ser_info_list[i].mem,
                                 test_data.test_field, MEM_BLOCK_ANY,
                                 REG_PORT_ANY, acc_type, mem_index,
                                 &test_data);
        if (test_data.mem_info == NULL) {
            *p_mem_skipped += num_inst_to_test;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "Memory %s skipped due to lack of"
                                    " mem_info structure.\n"),
                         SOC_MEM_NAME(unit,test_data.mem)));
            continue;
        }
        if (SOC_REG_IS_VALID(unit, ser_info_list[i].ecc1b_ctrl.ctrl_type.en_reg) &&
            (INVALIDf != ser_info_list[i].ecc1b_ctrl.en_fld)) {
            flags |= SOC_INJECT_ERROR_2BIT_ECC;
        }
#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
        rv = _soc_trident3_perform_ser_test(unit, flags, &test_data, test_type,
                                            p_mem_skipped, p_mem_failed);
        if (SOC_FAILURE(rv)) {
            return SOC_E_FAIL;
        }
#else
        _soc_trident3_perform_ser_test(unit, flags, &test_data, test_type,
                                       p_mem_skipped, p_mem_failed);
#endif
        if (num_inst_to_test > 1) {
            for (test_data.acc_type = _SOC_UNIQUE_ACC_TYPE_PIPE_0;
                 test_data.acc_type < NUM_PIPE(unit);
                 test_data.acc_type++) {
#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
                rv = _soc_trident3_perform_ser_test(unit, flags, &test_data,
                                                    test_type,
                                                    p_mem_skipped,
                                                    p_mem_failed);
                if (SOC_FAILURE(rv)) {
                    return SOC_E_FAIL;
                }
#else
                _soc_trident3_perform_ser_test(unit, flags, &test_data,
                                               test_type,
                                               p_mem_skipped,
                                               p_mem_failed);
#endif
            }
        }
    }

    return SOC_E_NONE;
}

/* #define _SER_DBG_EXIT_ON_FIRST_FAILURE */
/*
 * Function:
 *      soc_td3_ser_hardware_test
 * Purpose:
 *      Tests that SER is working for fio and SRAM memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  the number of tests which are failed
 */
STATIC int
soc_td3_ser_hardware_test(int unit, _soc_ser_test_t test_type) {
    int mem_failed = 0, mem_tests = 0, mem_skipped = 0;
    int rv = SOC_E_NONE;

    (void) _soc_td3_refresh_modify(unit, 0); /* disable refresh */

    rv = soc_td3_ser_block_test(unit, test_type, &mem_failed, &mem_tests, &mem_skipped,
        SOC_IP_MEM_SER_INFO(unit), SOC_BLK_IPIPE);


#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
    if (SOC_SUCCESS(rv))
#endif
    {
        rv = soc_td3_ser_block_test(unit, test_type, &mem_failed, &mem_tests, &mem_skipped,
            SOC_EP_MEM_SER_INFO(unit), SOC_BLK_EPIPE);
    }

#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
    if (SOC_SUCCESS(rv))
#endif
    {
        rv = soc_td3_ser_block_test(unit, test_type, &mem_failed, &mem_tests, &mem_skipped,
            SOC_MMU_MEM_SER_INFO(unit), SOC_BLK_MMU_GLB);
    }

    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit, "H/W memories test fail.\n")));
    } else {
        LOG_CLI((BSL_META_U(unit, "H/W memories test success.\n")));
    }

    LOG_CLI((BSL_META_U(unit, "\nH/W memories tested on unit %d: %d\n"),
             unit, mem_tests - mem_skipped));
    LOG_CLI((BSL_META_U(unit, "H/W tests passed:\t%d\n"),
             mem_tests - mem_failed - mem_skipped));
    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit, "H/W tests skipped:\t%d \n"),
         0));
    LOG_CLI((BSL_META_U(unit, "H/W tests failed:\t%d\n\n"), mem_failed));
    (void) _soc_td3_refresh_modify(unit, 1); /* re-enable refresh */
    return mem_failed;
}

#define TD3_XLPORT_INFO 0
#define TD3_CLPORT_INFO 1

soc_error_t
soc_td3_ser_port_parity_control_check(int unit, _soc_td3_ser_info_t *info_list, int type)
{
    int i;
    int match = 0;
    int port;
    soc_info_t *si = &SOC_INFO(unit);
     _soc_td3_ser_info_t *info;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 value = 0;
    uint32 rval = 0;
    uint64 rval64;
    uint32 total_count = 0;
    uint32 pass_count = 0;

    for (i = 0; ; i++) {
        info = &info_list[i];
        if (info->type == _SOC_TD3_PARITY_TYPE_NONE) {
            break;
        }

        reg = info->enable_reg;
        field = info->enable_field;
        if (SOC_REG_IS_VALID(unit, reg)) {
            PBMP_PORT_ITER(unit, port) {
                if ((SOC_PBMP_MEMBER(si->management_pbm, port) && (type != TD3_XLPORT_INFO)) ||
                    (!SOC_PBMP_MEMBER(si->management_pbm, port) && (type == TD3_XLPORT_INFO))) {
                    continue;
                }
                if (SOC_REG_IS_64(unit,reg)) {
                    COMPILER_64_ZERO(rval64);
                    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
                    value = soc_reg64_field32_get(unit, reg, rval64, field);
                } else {
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
                    value = soc_reg_field_get(unit, reg, rval, field);
                }
                if (0 == value) {
                    match = 0;
                    LOG_WARN(BSL_LS_SOC_SER,
                             (BSL_META_U(unit,
                                      "parity control %s.%s is disabled !!\n"),
                                      SOC_REG_NAME(unit, reg),
                                      SOC_FIELD_NAME(unit, field)));
                } else {
                    match = 1;
                }
                total_count++;
                if (match) {
                    pass_count++;
                }
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "\nPort parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
}

void
soc_td3_ser_parity_control_check_all(int unit)
{
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_MEM,
                                            (void*) SOC_IP_MEM_SER_INFO(unit));
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_MEM,
                                            (void*) SOC_EP_MEM_SER_INFO(unit));
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_MEM,
                                            (void*) SOC_MMU_MEM_SER_INFO(unit));
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_REG,
                                            (void*) SOC_IP_REG_SER_INFO(unit));
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_REG,
                                            (void*) SOC_EP_REG_SER_INFO(unit));
#if defined(BCM_56870_A0)
if (SOC_IS_TRIDENT3(unit)) {
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                            (void*) _soc_bcm56870_a0_ip_bus_ser_info);
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                            (void*) _soc_bcm56870_a0_ep_bus_ser_info);
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                            (void*) _soc_bcm56870_a0_ip_buffer_ser_info);
    (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                            (void*) _soc_bcm56870_a0_ep_buffer_ser_info);
    (void)soc_td3_ser_port_parity_control_check(unit, _soc_td3_pm_clp_ser_info, TD3_CLPORT_INFO);
    (void)soc_td3_ser_port_parity_control_check(unit, _soc_td3_pm_xlp_ser_info, TD3_XLPORT_INFO);
}
#endif

}

/*
 * Function:
 *      soc_td3_ser_test
 * Purpose:
 *      Tests that SER is working for all supported memories.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  SOC_E_NONE if test passes, an error otherwise (multiple types of errors are
 *      possible.)
 */
soc_error_t
soc_td3_ser_test(int unit, _soc_ser_test_t test_type) {
    int errors = 0;
    int def_soc_ser_test_long_sleep;
    int def_soc_ser_test_long_sleep_time_us;

    /* save default values, choose TD3 specific values  */
    def_soc_ser_test_long_sleep = soc_ser_test_long_sleep;
    def_soc_ser_test_long_sleep_time_us = soc_ser_test_long_sleep_time_us;
    soc_ser_test_long_sleep = TRUE;
    soc_ser_test_long_sleep_time_us =
        soc_property_get(unit, "tr144_long_sleep_time_us",
                         _SOC_TD3_TR144_LONG_SLEEP_TIME_US);

    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit,
                    "tr 144 test will use:long_sleep = %d,"
                    "long_sleep_time = %d uS\n"),
         soc_ser_test_long_sleep,
         soc_ser_test_long_sleep_time_us));

    /* Check parity controls before testing */
    soc_td3_ser_parity_control_check_all(unit);
    /*Test TCAM memories*/
    errors += soc_td3_ser_tcam_test(unit, test_type);
    /*Test for FIFO memories*/
    errors += soc_td3_ser_hardware_test(unit, test_type);

    /* restore default values */
    soc_ser_test_long_sleep = def_soc_ser_test_long_sleep;
    soc_ser_test_long_sleep_time_us = def_soc_ser_test_long_sleep_time_us;

    if (errors == 0) {
        return SOC_E_NONE;
    } else {
        LOG_CLI((BSL_META_U(unit,
                    "TR 144 test failed.\n")));
        return SOC_E_FAIL;
    }
}

void
soc_td3_ser_test_register(int unit)
{
    /*Initialize chip-specific functions for SER testing*/
    memset(&ser_trident3_test_fun, 0, sizeof(soc_ser_test_functions_t));
    ser_trident3_test_fun.inject_error_f = &soc_td3_ser_inject_error;
    ser_trident3_test_fun.test_mem = &soc_td3_ser_test_mem;
    ser_trident3_test_fun.test = &soc_td3_ser_test;
    ser_trident3_test_fun.parity_control = &soc_td3_ser_parity_control_reg_set;
    ser_trident3_test_fun.injection_support = &soc_td3_ser_error_injection_support;
    ser_trident3_test_fun.correction_info_get = &soc_td3_ser_correction_info_get;
    soc_ser_test_functions_register(unit, &ser_trident3_test_fun);
}

#endif /*defined(SER_TR_TEST_SUPPORT)*/

STATIC soc_error_t
_soc_td3_ser_tcam_control_reg_get(int unit, soc_mem_t mem,
                                  soc_reg_t   *ser_control_reg,
                                  soc_field_t *ser_enable_field)
{
    int i;
    for (i = 0; _soc_td3_tcam_wrapper_table[i].mem != INVALIDm; i++) {
        if(!SOC_REG_FIELD_VALID(unit, _soc_td3_tcam_wrapper_table[i].enable_reg,
                                _soc_td3_tcam_wrapper_table[i].enable_field)) {
            continue;
        }
        if (_soc_td3_tcam_wrapper_table[i].mem == mem ) {
            *ser_control_reg = _soc_td3_tcam_wrapper_table[i].enable_reg;
            *ser_enable_field = _soc_td3_tcam_wrapper_table[i].enable_field;
            return SOC_E_NONE;  /* found */
        }
    }
    return SOC_E_UNAVAIL;
}

int
soc_td3_tcam_engine_enable(int unit, int enable)
{
    uint64 config;
    COMPILER_64_ZERO(config);

    if (enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify
            (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, 1));

        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, INTERVALf, 64);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, IDB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));

        /* config EP HW ser scan engine */
        COMPILER_64_ZERO(config);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, INTERVALf, 64);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, EGR_ARB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));
    } else {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify
            (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, 0));

        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, INTERVALf, 64);
        soc_reg64_field32_set(unit, IDB_SER_SCAN_CONFIGr, &config, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, IDB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));

        /* config EP HW ser scan engine */
        COMPILER_64_ZERO(config);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, INTERVALf, 64);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, EGR_ARB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));
    }

    return SOC_E_NONE;
}

soc_error_t
soc_td3_ser_tcam_wrapper_enable(int unit, int enable)
{
    int i;
    uint32 val = 0;
    uint64 rval64;
    soc_reg_t   ser_control_reg;
    soc_field_t ser_enable_field;

    for (i = 0; _soc_td3_tcam_wrapper_table[i].mem != INVALIDm; i++) {

        if(!SOC_REG_FIELD_VALID(unit, _soc_td3_tcam_wrapper_table[i].enable_reg,
                                _soc_td3_tcam_wrapper_table[i].enable_field)) {
            continue;
        }

        if (!SOC_MEM_IS_VALID(unit, _soc_td3_tcam_wrapper_table[i].mem)) {
            continue;
        }
        ser_control_reg = _soc_td3_tcam_wrapper_table[i].enable_reg;
        ser_enable_field = _soc_td3_tcam_wrapper_table[i].enable_field;

        if (SOC_REG_IS_64(unit,ser_control_reg)) {
            COMPILER_64_ZERO(rval64);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, ser_control_reg, REG_PORT_ANY, 0, &rval64));
            soc_reg64_field32_set(unit, ser_control_reg, &rval64, ser_enable_field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, ser_control_reg, REG_PORT_ANY, 0, rval64));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, ser_control_reg, REG_PORT_ANY, 0, &val));
            soc_reg_field_set(unit, ser_control_reg, &val, ser_enable_field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, ser_control_reg, REG_PORT_ANY, 0, val));
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_TRIDENT3_SUPPORT */

