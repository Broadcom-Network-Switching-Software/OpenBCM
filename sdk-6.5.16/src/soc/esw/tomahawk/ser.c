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
#include <soc/register.h>
#include <soc/iproc.h>
#include <soc/intr.h>
#ifdef ALPM_ENABLE
#include <soc/esw/alpm_int.h>
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
#include <soc/tomahawk2.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif

#ifdef BCM_TOMAHAWK_SUPPORT

#include <soc/soc_ser_log.h>
#include <soc/tomahawk.h>

#define READ_CMICx_INTC_INTR(unit, reg, rvp) \
        soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), rvp)


#define _SOC_MMU_BASE_INDEX_GLB 0

#define _SOC_MMU_BASE_INDEX_XPE0 0
#define _SOC_MMU_BASE_INDEX_XPE1 1
#define _SOC_MMU_BASE_INDEX_XPE2 2
#define _SOC_MMU_BASE_INDEX_XPE3 3

#define _SOC_MMU_BASE_INDEX_SC0 0
#define _SOC_MMU_BASE_INDEX_SC1 1

#define _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD 0xfffc7fff /* [17:15] = 3'b000 */
#define _SOC_TH_MMU_ADDR_SEG0 0x00000000 /* [17:15] = 3'b000 */
#define _SOC_TH_MMU_ADDR_SEG1 0x00008000 /* [17:15] = 3'b001 */
#define _SOC_TH_MMU_ADDR_SEG2 0x00010000 /* [17:15] = 3'b010 */
#define _SOC_TH_MMU_ADDR_SEG3 0x00018000 /* [17:15] = 3'b011 */


#define _SOC_TH2_MMU_ADDR_ZERO_OUT_SEG_FIELD 0xfffcffff /* [17:16] = 2'b00 */
#define _SOC_TH2_MMU_ADDR_SEG0 0x00000000 /* [17:16] = 2'b00 */
#define _SOC_TH2_MMU_ADDR_SEG1 0x00010000 /* [17:16] = 2'b01 */
#define _SOC_TH2_MMU_ADDR_SEG2 0x00020000 /* [17:16] = 2'b10 */
#define _SOC_TH2_MMU_ADDR_SEG3 0x00030000 /* [17:16] = 2'b11 */



/* tomahawk [15:16]   tomahawk2 [16:17] */
#define _SOC_MMU_BASE_INDEX(unit, mem, bit_mask) (SOC_IS_TOMAHAWK2(unit)?((soc_mem_base(unit, mem) >> 16) & bit_mask):\
                                        ((soc_mem_base(unit, mem) >> 15) & bit_mask))


#define _SOC_FIND_MMU_XPE_SER_FIFO_MEM(mmu_base_index) \
            ((mmu_base_index == 0)? MMU_XCFG_MEM_FAIL_ADDR_64_XPE0m : \
             (mmu_base_index == 1)? MMU_XCFG_MEM_FAIL_ADDR_64_XPE1m : \
             (mmu_base_index == 2)? MMU_XCFG_MEM_FAIL_ADDR_64_XPE2m : \
                                    MMU_XCFG_MEM_FAIL_ADDR_64_XPE3m)

#define _SOC_FIND_MMU_SC_SER_FIFO_MEM(mmu_base_index) \
            ((mmu_base_index == 0)? MMU_SCFG_MEM_FAIL_ADDR_64_SC0m : \
                                    MMU_SCFG_MEM_FAIL_ADDR_64_SC1m)

#define _SOC_FIND_MMU_SED_SER_FIFO_MEM(mmu_base_index) \
            ((mmu_base_index == 0)? MMU_SEDCFG_MEM_FAIL_ADDR_64_SC0m : \
                                    MMU_SEDCFG_MEM_FAIL_ADDR_64_SC1m)

#ifdef BCM_TOMAHAWK3_SUPPORT
#define _SOC_MMU_BASE_INDEX_ITM0 0
#define _SOC_MMU_BASE_INDEX_ITM1 1

#define _SOC_MMU_BASE_INDEX_EB0 0
#define _SOC_MMU_BASE_INDEX_EB1 1
#define _SOC_MMU_BASE_INDEX_EB2 2
#define _SOC_MMU_BASE_INDEX_EB3 3
#define _SOC_MMU_BASE_INDEX_EB4 4
#define _SOC_MMU_BASE_INDEX_EB5 5
#define _SOC_MMU_BASE_INDEX_EB6 6
#define _SOC_MMU_BASE_INDEX_EB7 7

#define _SOC_FIND_MMU_ITM_SER_FIFO_MEM(mmu_base_index) \
                ((mmu_base_index == 0)? MMU_ITMCFG_MEM_FAIL_ADDR_64_ITM0m : \
                                        MMU_ITMCFG_MEM_FAIL_ADDR_64_ITM1m)

#define _SOC_FIND_MMU_EB_SER_FIFO_MEM(mmu_base_index) \
                ((mmu_base_index == 0)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE0m : \
                 (mmu_base_index == 1)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE1m : \
                 (mmu_base_index == 2)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE2m : \
                 (mmu_base_index == 3)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE3m : \
                 (mmu_base_index == 4)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE4m : \
                 (mmu_base_index == 5)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE5m : \
                 (mmu_base_index == 6)? MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE6m : \
                                        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE7m)
#endif


#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE0   0
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE1   1
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE2   2
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE3   3
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE4   4
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE5   5
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE6   6
#define _SOC_TH_ACC_TYPE_UNIQUE_PIPE7   7
#define _SOC_TH_ACC_TYPE_SINGLE   20
#define _SOC_TH_ACC_TYPE_DUPLICATE   9
#define _SOC_TH_ACC_TYPE_ADDR_SPLIT_DIST   10
#define _SOC_TH_ACC_TYPE_ADDR_SPLIT_SPLIT  12
#define _SOC_TH_ACC_TYPE_DATA_SPLIT   14
#define _SOC_TH_ACC_TYPE_NAME(at) \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE0) ? "AT_UNIQUE_PIPE0" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE1) ? "AT_UNIQUE_PIPE1" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE2) ? "AT_UNIQUE_PIPE2" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE3) ? "AT_UNIQUE_PIPE3" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE4) ? "AT_UNIQUE_PIPE4" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE5) ? "AT_UNIQUE_PIPE5" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE6) ? "AT_UNIQUE_PIPE6" : \
            (at == _SOC_TH_ACC_TYPE_UNIQUE_PIPE7) ? "AT_UNIQUE_PIPE7" : \
            (at == _SOC_TH_ACC_TYPE_SINGLE) ? "AT_SINGLE" : \
            (at == _SOC_TH_ACC_TYPE_DUPLICATE) ? "AT_DUPLICATE" : \
            (at == _SOC_TH_ACC_TYPE_ADDR_SPLIT_DIST) ? "AT_ADDR_SPLIT_DIST" : \
            (at == _SOC_TH_ACC_TYPE_ADDR_SPLIT_SPLIT) ? "AT_ADDR_SPLIT_SPLIT" : \
            (at == _SOC_TH_ACC_TYPE_DATA_SPLIT) ? "AT_DATA_SPLIT" : \
            "AT_UNKNOWN"

#define _SOC_TH_IFP_SLICE_MODE_NARROW 0
#define _SOC_TH_IFP_SLICE_MODE_WIDE 1

typedef struct _soc_th_ser_mmu_intr_info_s {
    soc_reg_t   int_statf;
    int         mmu_base_index;
    int         ser_info_index;
} _soc_th_ser_mmu_intr_info_t;

typedef enum {
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_PARITY,
    _SOC_PARITY_TYPE_MMU_SER,
    _SOC_PARITY_TYPE_MMU_GLB,
    _SOC_PARITY_TYPE_MMU_XPE,
    _SOC_PARITY_TYPE_MMU_SC,
    
#ifdef BCM_TOMAHAWK2_SUPPORT
    _SOC_PARITY_TYPE_MMU_SED,
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    _SOC_PARITY_TYPE_MMU_ITMCFG,
    _SOC_PARITY_TYPE_MMU_EBCFG,
#endif
#if defined (BCM_TOMAHAWKPLUS_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT)
    _SOC_PARITY_TYPE_CLMAC,
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    _SOC_PARITY_TYPE_XLMAC,
    _SOC_PARITY_TYPE_CDMAC,
#endif
    _SOC_PARITY_TYPE_SER,
    _SOC_PARITY_TYPE_MAC_RX_CDC,
    _SOC_PARITY_TYPE_MAC_TX_CDC,
    _SOC_PARITY_TYPE_MAC_RX_TS
} _soc_th_ser_info_type_t;

typedef struct _soc_th_ser_route_block_s {
    uint8               cmic_reg; /* 3: intr3, 4: intr4, 5: intr5 */
    uint32              cmic_bit;
    soc_block_t         blocktype;
    int                 pipe;
    soc_reg_t           enable_reg;
    soc_reg_t           status_reg;
    soc_field_t         enable_field;
    void                *info;
    uint8               id;
} _soc_th_ser_route_block_t;

/* These are now generated from the regsfile processing into bcm56960_a0.c */
#if defined(BCM_TOMAHAWK3_SUPPORT)
extern _soc_tcam_ser_en_info_t _soc_bcm56980_a0_tcam_mem_ser_info[];
#endif
/* Above structures are defined in chip.h */

_soc_bus_ser_en_info_t _soc_th_ip_bus_ser_info[] = {
    { "IPARS", IPARS_SER_CONTROLr, IPARS_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "IVXLT", IVXLT_SER_CONTROLr, IVXLT_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "ISW1", ISW1_SER_CONTROLr, ISW1_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "IRSEL1", IRSEL1_SER_CONTROLr, IRSEL1_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "IFP", IFP_PARITY_CONTROLr, IFP_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "IRSEL2", IRSEL2_SER_CONTROLr, IRSEL2_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "ISW2", ISW2_SER_CONTROL_1r, ISW2_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "IFWD", IFWD_SER_CONTROLr, IFWD_BUS_PAR_ENf, INVALIDr, INVALIDf },
    { "VP", VP_SER_CONTROLr, VP_BUS_PAR_ENf, INVALIDr, INVALIDf },
    { "IPARS_IVP_PT", IVXLT_SER_CONTROLr, PT_HWY_PARITY_ENf, INVALIDr, INVALIDf },
    { "IVP_ISW1_PT", IRSEL1_SER_CONTROLr, IVP_TO_ISW1_PT_HWY_PARITY_ENf, INVALIDr, INVALIDf },
    { "IFWD_ISW1_PT", IRSEL1_SER_CONTROLr, IFWD_TO_ISW1_PT_HWY_PARITY_ENf, INVALIDr, INVALIDf },
    { "ISW1_ISW2_PT", IRSEL2_SER_CONTROLr, ISW1_TO_ISW2_PT_HWY_PARITY_ENf, INVALIDr, INVALIDf },
    { "", INVALIDr }
};
_soc_bus_ser_en_info_t _soc_th_ep_bus_ser_info[] = {
    { "EVLAN", EGR_VLAN_SER_CONTROLr, EVLAN_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "EHCPM", EGR_EHCPM_SER_CONTROLr, EHCPM_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "EPMOD", EGR_EPMOD_SER_CONTROLr, EPMOD_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "EFPPARS", EGR_EFPPARS_SER_CONTROLr, EFPPARS_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "EFP", EFP_PARITY_CONTROLr, EFP_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "EP_PT", EGR_VLAN_SER_CONTROLr, PT_HWY_PAR_ENf, INVALIDr, INVALIDf },
    { "", INVALIDr }
};

_soc_buffer_ser_en_info_t _soc_th_ip_buffer_ser_info[] = {
    { "ISW3_EOP_BUFFER_A", ISW3_SER_CONTROL_1r, SW3_EOP_BUFFER_A_PARITY_ENf, INVALIDr, INVALIDf },
    { "ISW3_EOP_BUFFER_B", ISW3_SER_CONTROL_1r, SW3_EOP_BUFFER_B_PARITY_ENf, INVALIDr, INVALIDf },
    { "BUBBLE_MOP", ISW2_SER_CONTROL_0_64r, CPB_PARITY_ENf, INVALIDr, INVALIDf },
    { "METER_STAGING", IFP_PARITY_CONTROLr, METER_MUX_DATA_STAGING_PARITY_ENf, INVALIDr, INVALIDf },
    { "COUNTER_STAGING", IFP_PARITY_CONTROLr,
      COUNTER_MUX_DATA_STAGING_PARITY_ENf, INVALIDr, INVALIDf },
    /* No other good place for these for now */

    /* OBM packet buffer - DBE */
    { "OBM0_DATA_FIFO", IDB_OBM0_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM1_DATA_FIFO", IDB_OBM1_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM2_DATA_FIFO", IDB_OBM2_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM3_DATA_FIFO", IDB_OBM3_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM4_DATA_FIFO", IDB_OBM4_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM5_DATA_FIFO", IDB_OBM5_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM6_DATA_FIFO", IDB_OBM6_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM7_DATA_FIFO", IDB_OBM7_SER_CONTROLr, DATA_ECC_ENABLEf, INVALIDr, INVALIDf },

    /* OBM queue buffer - DBE */
    { "OBM0_QUEUE_FIFO", IDB_OBM0_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM1_QUEUE_FIFO", IDB_OBM1_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM2_QUEUE_FIFO", IDB_OBM2_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM3_QUEUE_FIFO", IDB_OBM3_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM4_QUEUE_FIFO", IDB_OBM4_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM5_QUEUE_FIFO", IDB_OBM5_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM6_QUEUE_FIFO", IDB_OBM6_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },
    { "OBM7_QUEUE_FIFO", IDB_OBM7_QUEUE_SER_CONTROLr, ECC_ENABLEf, INVALIDr, INVALIDf },

    /* Data port CA buffer (CA-PM packet buffer) - DBE */
    { "CELL_ASSEM_BUFFER0", IDB_OBM0_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER1", IDB_OBM1_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER2", IDB_OBM2_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER3", IDB_OBM3_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER4", IDB_OBM4_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER5", IDB_OBM5_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER6", IDB_OBM6_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },
    { "CELL_ASSEM_BUFFER7", IDB_OBM7_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, INVALIDr, INVALIDf },

    /* CPU port CA buffer (CA-CM packet buffer) - DBE */
    { "CPU_CELL_ASSEM_BUFFER", IDB_SER_CONTROLr, CA_CPU_ECC_ENABLEf, INVALIDr, INVALIDf },

    /* Loopback port CA buffer (CA-LPBK packet buffer) - DBE */
    { "LPBK_CELL_ASSEM_BUFFER", IDB_SER_CONTROLr, CA_LPBK_ECC_ENABLEf, INVALIDr, INVALIDf },

    /* Ingress Scheduler - DBE */
    { "IS_TDM_CALENDAR", IDB_SER_CONTROLr, IS_TDM_ECC_ENf, INVALIDr, INVALIDf },
    /* OBM Stats - parity */
    { "IDB_OBMn_STATS_WIN_RES", IDB_SER_CONTROLr, OBM_MON_PAR_ENf, INVALIDr, INVALIDf },

    /* L2_Mgmt - not part of ipipe, epipe */
    { "L2_MOD_FIFO", L2_MOD_FIFO_PARITY_CONTROLr, L2_MOD_FIFO_PARITY_ENf, INVALIDr, INVALIDf },
    { "LEARN_FIFO", LEARN_FIFO_ECC_CONTROLr, ECC_ENf, INVALIDr, INVALIDf },


    /* OBM packet buffer - disable SBE
    { "OBM0_DATA_FIFO_1b", IDB_OBM0_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM1_DATA_FIFO_1b", IDB_OBM1_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM2_DATA_FIFO_1b", IDB_OBM2_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM3_DATA_FIFO_1b", IDB_OBM3_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM4_DATA_FIFO_1b", IDB_OBM4_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM5_DATA_FIFO_1b", IDB_OBM5_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM6_DATA_FIFO_1b", IDB_OBM6_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    { "OBM7_DATA_FIFO_1b", IDB_OBM7_SER_CONTROLr, DATA_EN_COR_ERR_RPTf },
    */

    /* OBM queue buffer - enable SBE */
    { "OBM0_QUEUE_FIFO_1b", IDB_OBM0_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM1_QUEUE_FIFO_1b", IDB_OBM1_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM2_QUEUE_FIFO_1b", IDB_OBM2_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM3_QUEUE_FIFO_1b", IDB_OBM3_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM4_QUEUE_FIFO_1b", IDB_OBM4_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM5_QUEUE_FIFO_1b", IDB_OBM5_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM6_QUEUE_FIFO_1b", IDB_OBM6_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "OBM7_QUEUE_FIFO_1b", IDB_OBM7_QUEUE_SER_CONTROLr, EN_COR_ERR_RPTf, INVALIDr, INVALIDf },

    /* Data port CA buffer (CA-PM packet buffer) - disable SBE
    { "CELL_ASSEM_BUFFER0_1b", IDB_OBM0_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER1_1b", IDB_OBM1_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER2_1b", IDB_OBM2_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER3_1b", IDB_OBM3_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER4_1b", IDB_OBM4_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER5_1b", IDB_OBM5_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER6_1b", IDB_OBM6_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    { "CELL_ASSEM_BUFFER7_1b", IDB_OBM7_SER_CONTROLr, CA_FIFO_EN_COR_ERR_RPTf },
    */

    /* CPU port CA buffer (CA-CM packet buffer) - disable SBE
    { "CPU_CELL_ASSEM_BUFFER_1b", IDB_SER_CONTROLr, CA_CPU_ERR1_RPT_EN },
    */

    /* Loopback port CA buffer (CA-LPBK packet buffer) - disable SBE
    { "LPBK_CELL_ASSEM_BUFFER_1b", IDB_SER_CONTROLr, CA_LPBK_ERR1_RPT_EN },
    */

    /* Ingress Scheduler - disable SBE
    { "IS_TDM_CALENDAR_1b", IDB_SER_CONTROLr, IS_TDM_ERR1_RPT_ENf },
    */

    /* end of list */
    { "", INVALIDr }
};

_soc_buffer_ser_en_info_t _soc_th_ep_buffer_ser_info[] = {
    { "EDB_PM0_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM0_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM1_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM1_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM2_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM2_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM3_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM3_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM4_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM4_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM5_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM5_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM6_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM6_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM7_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM7_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_CM_MEM", EGR_EDATABUF_PARITY_CONTROLr, CM_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_LBP_MEM", EGR_EDATABUF_PARITY_CONTROLr, LBP_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM0_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM0_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM1_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM1_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM2_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM2_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM3_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM3_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM4_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM4_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM5_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM5_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM6_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM6_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_PM7_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM7_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_CM_CTRL", EGR_EDB_CTRL_PARITY_ENr, CM_ECC_ENf, INVALIDr, INVALIDf },
    { "EDB_LBP_CTRL", EGR_EDB_CTRL_PARITY_ENr, LBP_ECC_ENf, INVALIDr, INVALIDf },
    { "INITBUF", EGR_EL3_ECC_PARITY_CONTROLr, INITBUF_ECC_ENf, INVALIDr, INVALIDf },
    { "EGR_MPB", EGR_EL3_ECC_PARITY_CONTROLr, EGR_MPB_ECC_ENf, INVALIDr, INVALIDf },
    { "EP_EDB_PM_RESI_BUFF", EGR_EDATABUF_PARITY_CONTROLr,
      PM_RESI_BUFFER_PAR_ENf, INVALIDr, INVALIDf },
    { "", INVALIDr }
};

_soc_bus_buffer_ser_force_info_t _soc_bcm56960_a0_bus_buffer_ser_force_info[] = {
    { "BUBBLE_MOP", ISW2_SER_CONTROL_0_64r, CPB_PARITY_FORCE_ERRf },
    { "IPARS_BUS", IPARS_SER_CONTROLr, IPARS_FORCE_ERRORf },
    { "IVXLT_BUS", IVXLT_SER_CONTROLr, IVXLT_BUS_PARITY_FORCE_ERRORf },
    { "IFWD_BUS", IFWD_SER_CONTROLr, IFWD_BUS_CORRUPT_ENf },
    { "IRSEL1_BUS", IRSEL1_SER_CONTROLr, IRSEL1_BUS_PARITY_FORCE_ERRORf },
    { "ISW1_BUS", ISW1_SER_CONTROLr, ISW1_BUS_PARITY_FORCE_ERRORf },
    { "ISW2_BUS", ISW2_SER_CONTROL_1r, ISW2_BUS_PARITY_FORCE_ERRORf },
    { "IFP_BUS", IFP_PARITY_CONTROLr, IFP_BUS_PARITY_FORCE_ERRORf },
    { "IRSEL2_BUS", IRSEL2_SER_CONTROLr, IRSEL2_BUS_PARITY_FORCE_ERRORf },
    { "VP_BUS", VP_SER_CONTROLr, VP_BUS_CORRUPT_ENf },
    { "EP_INITBUF", EGR_EL3_ECC_PARITY_CONTROLr, EP_INITBUF_ECC_CORRUPTf },
    { "IP_PACKET_BUFFER", IPARS_SER_CONTROLr, INGRESS_PACKET_BUFFER_FORCE_ERRORf },
    { "EFP_BUS", EFP_PARITY_CONTROLr, EFP_BUS_PARITY_FORCE_ERRORf },
    { "EVLAN_BUS", EGR_VLAN_SER_CONTROLr, EVLAN_FORCE_ERRORf },
    { "EHCPM_BUS", EGR_EHCPM_SER_CONTROLr, EHCPM_FORCE_ERRORf },
    { "EPMOD_BUS", EGR_EPMOD_SER_CONTROLr, EPMOD_FORCE_ERRORf },
    { "EFPPARS_BUS", EGR_EFPPARS_SER_CONTROLr, EFPPARS_FORCE_ERRORf },
    { "EP_PT_BUS", EGR_VLAN_SER_CONTROLr, PT_HWY_PAR_CORf },
    { "IPARS_IVP_PT_BUS", IVXLT_SER_CONTROLr, PT_HWY_PARITY_FORCE_ERRORf },
    { "IVP_ISW1_PT_BUS", IRSEL1_SER_CONTROLr, IVP_TO_ISW1_PT_HWY_FORCE_ERRORf },
    { "IFWD_ISW1_PT_BUS", IRSEL1_SER_CONTROLr, IFWD_TO_ISW1_PT_HWY_FORCE_ERRORf },
    { "ISW1_ISW2_PT_BUS", IRSEL2_SER_CONTROLr, ISW1_TO_ISW2_PT_HWY_FORCE_ERRORf },
    { "", INVALIDr } /* end */
};

uint32 ser_th_bus_buffer_hwbase[] = {
    0xa,  /* BUBBLE_MOP */
    0x11, /* IPARS_BUS */
    0x12, /* IVXLT_BUS */
    0x14, /* IFWD_BUS */
    0x15, /* IRSEL1_BUS */
    0x16, /* ISW1_BUS */
    0x19, /* ISW2_BUS */
    0xf,  /* IFP_BUS */
    0x18, /* IRSEL2_BUS */
    0x13, /* VP_BUS */
    0x41, /* EP_INITBUF */
    0x40, /* IP_PACKET_BUFFER */
    0x5e, /* EFP_BUS */
    0x5a, /* EVLAN_BUS */
    0x5b, /* EHCPM_BUS */
    0x5c, /* EPMOD_BUS */
    0x5d, /* EFPPARS_BUS */
    0x5f, /* EP_PT_BUS */
    0xb,  /* IPARS_IVP_PT_BUS */
    0xc,  /* IVP_ISW1_PT_BUS */
    0xd,  /* IFWD_ISW1_PT_BUS */
    0xf   /* ISW1_ISW2_PT_BUS */
};

typedef struct _soc_th_ser_reg_s {
    soc_reg_t reg;
    char      *mem_str;
} _soc_th_ser_reg_t;

typedef struct _soc_th_ser_info_s {
    _soc_th_ser_info_type_t   type;
    struct _soc_th_ser_info_s *info;
    int                       id;
    soc_field_t               group_reg_enable_field;
    soc_field_t               group_reg_status_field;
    soc_mem_t                 mem;
    char                      *mem_str;
    soc_reg_t                 enable_reg;
    soc_field_t               enable_field;
    soc_field_t               *enable_field_list;
    soc_reg_t                 intr_enable_reg;
    soc_field_t               intr_enable_field;
    soc_field_t               *intr_enable_field_list;
    soc_reg_t                 intr_status_reg;
    _soc_th_ser_reg_t         *intr_status_reg_list;
    soc_reg_t                 intr_status_field;
    soc_field_t               *intr_status_field_list;
    soc_reg_t                 intr_clr_reg;
    soc_field_t               intr_clr_field;
    soc_field_t               *intr_clr_field_list;
} _soc_th_ser_info_t;

typedef struct _soc_th_ser_block_info_s {
    int         type;
    soc_block_t blocktype;
    char        name[16];
    soc_reg_t   fifo_reset_reg;
    void        *info;
} _soc_th_ser_block_info_t;

static soc_ser_functions_t _th_ser_functions;


static soc_field_t _soc_th_mmu_xpe_enable_fields[] = {
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

static soc_field_t _soc_th_mmu_sc_enable_fields[] = {
    MB_A_PARITY_ENf,
    MB_B_PARITY_ENf,
    TDM_PARITY_ENf,
    ENQS_PARITY_ENf,
    MTRO_PARITY_ENf,
    VBS_PARITY_ENf,
    DQS_PARITY_ENf,
    TOQ_B_PARITY_ENf,
    TOQ_A_PARITY_ENf,
    CFAP_B_PARITY_ENf,
    CFAP_A_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_th_mmu_sc_int_enable_fields[] = {
    CFAP_A_MEM_FAIL_ENf,
    CFAP_B_MEM_FAIL_ENf,
    MEM_PAR_ERR_ENf,
    INVALIDf
};
static soc_field_t _soc_th_mmu_sc_status_fields[] = {
    CFAP_A_MEM_FAIL_STATf,
    CFAP_B_MEM_FAIL_STATf,
    MEM_PAR_ERR_STATf,
    INVALIDf
};
static soc_field_t _soc_th_mmu_sc_clr_fields[] = {
    CFAP_A_MEM_FAIL_CLRf,
    CFAP_B_MEM_FAIL_CLRf,
    MEM_PAR_ERR_CLRf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th_mmu_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_GLB, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU GLB MEM PAR",
        MMU_GCFG_PARITY_ENr, INTFO_PARITY_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_GCFG_GLB_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_XPE, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU XPE MEM PAR",
        MMU_XCFG_PARITY_ENr, INVALIDf, _soc_th_mmu_xpe_enable_fields,
        MMU_XCFG_XPE_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_XCFG_XPE_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_XCFG_XPE_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_SC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SC MEM PAR",
        MMU_SCFG_PARITY_ENr, INVALIDf, _soc_th_mmu_sc_enable_fields,
        MMU_SCFG_SC_CPU_INT_ENr, INVALIDf, _soc_th_mmu_sc_int_enable_fields,
        MMU_SCFG_SC_CPU_INT_STATr, NULL, INVALIDf/*INVALIDf*/,
        _soc_th_mmu_sc_status_fields/*_soc_th_mmu_sc_status_fields*/,
        MMU_SCFG_SC_CPU_INT_CLEARr, INVALIDf/*INVALIDf*/,
        _soc_th_mmu_sc_clr_fields/*_soc_th_mmu_sc_clr_fields*/
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static soc_field_t _soc_th_mmu_top_int_enable_fields[] = {
    SC_R_CPU_INT_ENf,
    SC_S_CPU_INT_ENf,
    XPE_R_B_CPU_INT_ENf,
    XPE_R_A_CPU_INT_ENf,
    XPE_S_B_CPU_INT_ENf,
    XPE_S_A_CPU_INT_ENf,
    GLB_CPU_INT_ENf,
    INVALIDf
};

static soc_field_t _soc_th_mmu_top_int_status_fields[] = {
    SC_R_CPU_INT_STATf,
    SC_S_CPU_INT_STATf,
    XPE_R_B_CPU_INT_STATf,
    XPE_R_A_CPU_INT_STATf,
    XPE_S_B_CPU_INT_STATf,
    XPE_S_A_CPU_INT_STATf,
    GLB_CPU_INT_STATf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th_mmu_top_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_SER, _soc_th_mmu_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GCFG_ALL_CPU_INT_ENr, INVALIDf,
        _soc_th_mmu_top_int_enable_fields,
        MMU_GCFG_ALL_CPU_INT_STATr,
        NULL, INVALIDf, _soc_th_mmu_top_int_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static const
_soc_th_ser_mmu_intr_info_t mmu_intr_info[] = {
    { GLB_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_GLB, 0 },
    { XPE_R_A_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE0, 1 },
    { XPE_S_A_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE1, 1 },
    { XPE_R_B_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE2, 1 },
    { XPE_S_B_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE3, 1 },
    { SC_R_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC0, 2 },
    { SC_S_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC1, 2},
    { INVALIDf, -1, -1 }
};

static soc_field_t _soc_th_pm_rx_cdc_clear_field[] = {
    CLEAR_RX_CDC_SINGLE_BIT_ERRf, CLEAR_RX_CDC_DOUBLE_BIT_ERRf
};
static soc_field_t _soc_th_pm_tx_cdc_clear_field[] = {
    CLEAR_TX_CDC_SINGLE_BIT_ERRf, CLEAR_TX_CDC_DOUBLE_BIT_ERRf
};
static soc_field_t _soc_th_pm_rx_ts_clear_field[] = {
    CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf, CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf
};

static _soc_th_ser_reg_t _soc_th_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC" },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_reg_t _soc_th_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC" },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_reg_t _soc_th_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_reg_t _soc_th_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_info_t _soc_th_pm_clp_ser_info[] = {
    { _SOC_PARITY_TYPE_MAC_RX_TS, NULL, 0,
        MAC_RX_TS_CDC_MEM_ERRf, MAC_RX_TS_CDC_MEM_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        CLMAC_ECC_CTRLr, RX_TS_MEM_ECC_CTRL_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_RX_TS_MEM_ECC_STATUSr, NULL, INVALIDf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, INVALIDf, _soc_th_pm_rx_ts_clear_field
    },
    { _SOC_PARITY_TYPE_MAC_RX_CDC, NULL, 0,
        MAC_RX_CDC_MEM_ERRf, MAC_RX_CDC_MEM_ERRf,
        INVALIDm, "MAC RX CDC memory",
        CLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_RX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, INVALIDf, _soc_th_pm_rx_cdc_clear_field
    },
    { _SOC_PARITY_TYPE_MAC_TX_CDC, NULL, 0,
        MAC_TX_CDC_MEM_ERRf, MAC_TX_CDC_MEM_ERRf,
        INVALIDm, "MAC TX CDC memory",
        CLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_TX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, INVALIDf, _soc_th_pm_tx_cdc_clear_field
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_clp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_clp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_th_ser_info_t _soc_th_pm_xlp_ser_info[] = {
    { _SOC_PARITY_TYPE_MAC_RX_CDC, NULL, 0,
        MAC_RX_CDC_MEM_ERRf, MAC_RX_CDC_MEM_ERRf,
        INVALIDm, "MAC RX CDC memory",
        XLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_RX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, INVALIDf, _soc_th_pm_rx_cdc_clear_field
    },
    { _SOC_PARITY_TYPE_MAC_TX_CDC, NULL, 0,
        MAC_TX_CDC_MEM_ERRf, MAC_TX_CDC_MEM_ERRf,
        INVALIDm, "MAC TX CDC memory",
        XLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_TX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, INVALIDf, _soc_th_pm_tx_cdc_clear_field
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_xlp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_xlp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

#ifdef BCM_TOMAHAWKPLUS_SUPPORT
static _soc_th_ser_info_t _soc_thplus_pm_clp_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_clp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_clp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC single bit",
        CLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC double bits",
        CLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC single bit",
        CLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC double bits",
        CLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX TS MEM single bit",
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX TS MEM double bits",
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};
#endif

static _soc_th_ser_reg_t _soc_th_idb_status_reg[] = {
    { IDB_OBM0_DATA_ECC_STATUSr, "IDB OBM0 packet data memory" },
    { IDB_OBM1_DATA_ECC_STATUSr, "IDB OBM1 packet data memory" },
    { IDB_OBM2_DATA_ECC_STATUSr, "IDB OBM2 packet data memory" },
    { IDB_OBM3_DATA_ECC_STATUSr, "IDB OBM3 packet data memory" },
    { IDB_OBM4_DATA_ECC_STATUSr, "IDB OBM4 packet data memory" },
    { IDB_OBM5_DATA_ECC_STATUSr, "IDB OBM5 packet data memory" },
    { IDB_OBM6_DATA_ECC_STATUSr, "IDB OBM6 packet data memory" },
    { IDB_OBM7_DATA_ECC_STATUSr, "IDB OBM7 packet data memory" },
    { IDB_OBM0_CA_ECC_STATUSr, "IDB OBM0 CA packet buffer memory" },
    { IDB_OBM1_CA_ECC_STATUSr, "IDB OBM1 CA packet buffer memory" },
    { IDB_OBM2_CA_ECC_STATUSr, "IDB OBM2 CA packet buffer memory" },
    { IDB_OBM3_CA_ECC_STATUSr, "IDB OBM3 CA packet buffer memory" },
    { IDB_OBM4_CA_ECC_STATUSr, "IDB OBM4 CA packet buffer memory" },
    { IDB_OBM5_CA_ECC_STATUSr, "IDB OBM5 CA packet buffer memory" },
    { IDB_OBM6_CA_ECC_STATUSr, "IDB OBM6 CA packet buffer memory" },
    { IDB_OBM7_CA_ECC_STATUSr, "IDB OBM7 CA packet buffer memory" },
    { IDB_CA_CPU_ECC_STATUSr, "CA-CPU packet buffer memory" },
    { IDB_CA_LPBK_ECC_STATUSr, "CA-LPBK packet buffer memory" },
    { IDB_IS_TDM_CAL_ECC_STATUSr, "IDB IS_TDM0,1 calendar" },
    { INVALIDr }
};

/* OBM queue buffer */
static _soc_th_ser_reg_t _soc_th_idb_status_reg2[] = {
    { IDB_OBM0_QUEUE_ECC_STATUSr, "IDB OBM0 queue FIFO memory" },
    { IDB_OBM1_QUEUE_ECC_STATUSr, "IDB OBM1 queue FIFO memory" },
    { IDB_OBM2_QUEUE_ECC_STATUSr, "IDB OBM2 queue FIFO memory" },
    { IDB_OBM3_QUEUE_ECC_STATUSr, "IDB OBM3 queue FIFO memory" },
    { IDB_OBM4_QUEUE_ECC_STATUSr, "IDB OBM4 queue FIFO memory" },
    { IDB_OBM5_QUEUE_ECC_STATUSr, "IDB OBM5 queue FIFO memory" },
    { IDB_OBM6_QUEUE_ECC_STATUSr, "IDB OBM6 queue FIFO memory" },
    { IDB_OBM7_QUEUE_ECC_STATUSr, "IDB OBM7 queue FIFO memory" },
    { INVALIDr }
};

static _soc_th_ser_info_t _soc_th_idb_ser_info[] = {
    /* OBM queue buffer - keep this as first entry */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_idb_status_reg2, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_idb_status_reg, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

/* Setup dynamically in enable ser */
static uint32 _soc_th_ip_pipe_fifo_bmask[SOC_MAX_NUM_DEVICES][4];

#define _SOC_TH_IPIPE0_IP0_INTR 0x00000200
#define _SOC_TH_IPIPE1_IP0_INTR 0x00000400
#define _SOC_TH_IPIPE2_IP0_INTR 0x00000800
#define _SOC_TH_IPIPE3_IP0_INTR 0x00001000
static const
_soc_th_ser_route_block_t  _soc_th_ser_route_blocks[] = {
    { 3, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTR */
      SOC_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      _soc_th_mmu_top_ser_info, 0 },
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    { 3, 0x00000020, /* PIPE0_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000040, /* PIPE1_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000080, /* PIPE2_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 2, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000100, /* PIPE3_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 3, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, _SOC_TH_IPIPE0_IP0_INTR, /* PIPE0_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, _SOC_TH_IPIPE1_IP0_INTR, /* PIPE1_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, _SOC_TH_IPIPE2_IP0_INTR, /* PIPE2_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 2, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, _SOC_TH_IPIPE3_IP0_INTR, /* PIPE3_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 3, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, 0x00002000, /* PIPE0_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00004000, /* PIPE1_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00008000, /* PIPE2_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 2, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00010000, /* PIPE3_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 3, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 4, 0x00000001, /* PM0_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 0 },
    { 4, 0x00000002, /* PM1_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 1 },
    { 4, 0x00000004, /* PM2_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 2 },
    { 4, 0x00000008, /* PM3_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 3 },
    { 4, 0x00000010, /* PM4_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 4 },
    { 4, 0x00000020, /* PM5_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 5 },
    { 4, 0x00000040, /* PM6_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 6 },
    { 4, 0x00000080, /* PM7_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 7 },
    { 4, 0x00000100, /* PM8_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 8 },
    { 4, 0x00000200, /* PM9_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 9 },
    { 4, 0x00000400, /* PM10_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 10 },
    { 4, 0x00000800, /* PM11_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 11 },
    { 4, 0x00001000, /* PM12_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 12 },
    { 4, 0x00002000, /* PM13_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 13 },
    { 4, 0x00004000, /* PM14_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 14 },
    { 4, 0x00008000, /* PM15_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 15 },
    { 4, 0x00010000, /* PM16_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 16 },
    { 4, 0x00020000, /* PM17_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 17 },
    { 4, 0x00040000, /* PM18_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 18 },
    { 4, 0x00080000, /* PM19_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 19 },
    { 4, 0x00100000, /* PM20_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 20 },
    { 4, 0x00200000, /* PM21_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 21 },
    { 4, 0x00400000, /* PM22_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 22 },
    { 4, 0x00800000, /* PM23_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 23 },
    { 4, 0x01000000, /* PM24_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 24 },
    { 4, 0x02000000, /* PM25_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 25 },
    { 4, 0x04000000, /* PM26_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 26 },
    { 4, 0x08000000, /* PM27_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 27 },
    { 4, 0x10000000, /* PM28_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 28 },
    { 4, 0x20000000, /* PM29_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 29 },
    { 4, 0x40000000, /* PM30_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 30 },
    { 4, 0x80000000, /* PM31_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 31 },
    { 5, 0x00000001, /* PM32_INTR */
      SOC_BLK_XLPORT, 1, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_xlp_ser_info, 0 },
    { 0 } /* table terminator */
};

static soc_mem_t _soc_th_skip_populating[] = {
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
    INVALIDm
};

#ifdef BCM_TOMAHAWK2_SUPPORT
extern _soc_bus_ser_en_info_t _soc_bcm56970_a0_ip_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56970_a0_ip_buffer_ser_info[];
extern _soc_bus_ser_en_info_t _soc_bcm56970_a0_ep_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56970_a0_ep_buffer_ser_info[];

_soc_buffer_ser_en_info_t _soc_th2_ifp_buffer_ser_info[] = {
    { "METER MUX DATA STAGING", IFP_PARITY_CONTROLr, METER_MUX_DATA_STAGING_PARITY_ENf, INVALIDr, INVALIDf },
    { "PORT METER MAP", IFP_PARITY_CONTROLr, PORT_METER_MAP_PARITY_ENf, INVALIDr, INVALIDf },
    { "COUNTER MUX DATA STAGING", IFP_PARITY_CONTROLr, COUNTER_MUX_DATA_STAGING_PARITY_ENf, INVALIDr, INVALIDf },
    { "IFP COUNTER", IFP_PARITY_CONTROLr, COUNTER_PARITY_ENf, INVALIDr, INVALIDf },
    { "", INVALIDr }
};

_soc_bus_buffer_ser_force_info_t ser_th2_bus_buffer_skip_test[] = {
    { "DPP_OUTPUT_EVENT_FIFO", SW4_SER_CONTROLr, EVENT_FIFO_ECC_FORCE_ERRf },
    { "SLOT_PIPELINE", SW4_SER_CONTROLr, SLOT_PIPELINE_ECC_FORCE_ERRf },
    { "CELL_QUEUE", SW4_SER_CONTROLr, CELL_QUEUE_ECC_FORCE_ERRf },
    { "", INVALIDr } /* end */
};

uint32 ser_th2_bus_buffer_hwbase[] = {
    0x18, /* IRSEL2_BUS */
    0x15, /* IRSEL1_BUS */
    0,    /* DPP_OUTPUT_EVENT_FIFO */
    0,    /* SLOT_PIPELINE */
    0x5b, /* EHCPM_BUS */
    0x11, /* IPARS_BUS */
    0x41, /* EP_INITBUF_RAM_WRAPPER */
    0x12, /* IVXLT_BUS */
    0x5a, /* EVLAN_BUS */
    0x16, /* ISW1_BUS */
    0x13, /* VP_BUS */
    0xb,  /* IPARS_IVP_PT_BUS */
    0x17, /* IFP_BUS */
    0xf,  /* ISW1_ISW2_PT_BUS */
    0xc,  /* IVP_ISW1_PT_BUS */
    0x5e, /* EFP_BUS */
    0x5f, /* EP_PT_BUS */
    0x14, /* IFWD_BUS */
    0x19, /* ISW2_BUS */
    0x5c, /* EPMOD_BUS */
    0x40, /* IP_PACKET_BUFFER */
    0x72, /* EP_EFPBUF */
    0,    /* CELL_QUEUE */
    0xd,  /* IFWD_ISW1_PT_BUS */
    0x5d, /* EFPPARS_BUS */
    0     /* EOP_BUFFER_MTU */
};

soc_field_t _soc_th2_idb_ca_lpbk_enable_fields[] = {
    CA_LPBK_ECC_ENABLEf,
    CA_LPBK_ERR1_RPT_ENf,
    INVALIDf
};

soc_field_t _soc_th2_idb_ca_cpu_enable_fields[] = {
    CA_CPU_ECC_ENABLEf,
    CA_CPU_ERR1_RPT_ENf,
    INVALIDf
};

soc_field_t _soc_th2_idb_sop_event_enable_fields[] = {
    SOP_EVENT_ECC_ENABLEf,
    SOP_EVENT_ERR1_RPT_ENf,
    INVALIDf
};

soc_field_t _soc_th2_idb_eop_event_enable_fields[] = {
    EOP_EVENT_ECC_ENABLEf,
    EOP_EVENT_ERR1_RPT_ENf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th2_idb_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM0 DATA FIFO",
        IDB_OBM0_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM0_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM1 DATA FIFO",
        IDB_OBM1_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM1_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM2 DATA FIFO",
        IDB_OBM2_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM2_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM3 DATA FIFO",
        IDB_OBM3_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM3_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM4 DATA FIFO",
        IDB_OBM4_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM4_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM5 DATA FIFO",
        IDB_OBM5_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM5_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM6 DATA FIFO",
        IDB_OBM6_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM6_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM7 DATA FIFO",
        IDB_OBM7_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM7_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM8 DATA FIFO",
        IDB_OBM8_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM8_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM9 DATA FIFO",
        IDB_OBM9_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM9_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM10 DATA FIFO",
        IDB_OBM10_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM10_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM11 DATA FIFO",
        IDB_OBM11_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM11_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM12 DATA FIFO",
        IDB_OBM12_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM12_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM13 DATA FIFO",
        IDB_OBM13_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM13_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM14 DATA FIFO",
        IDB_OBM14_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM14_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM15 DATA FIFO",
        IDB_OBM15_SER_CONTROLr, DATA_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM15_DATA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM0 CELL ASSEM BUFFER",
        IDB_OBM0_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM0_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM1 CELL ASSEM BUFFER",
        IDB_OBM1_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM1_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM2 CELL ASSEM BUFFER",
        IDB_OBM2_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM2_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM3 CELL ASSEM BUFFER",
        IDB_OBM3_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM3_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM4 CELL ASSEM BUFFER",
        IDB_OBM4_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM4_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM5 CELL ASSEM BUFFER",
        IDB_OBM5_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM5_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM6 CELL ASSEM BUFFER",
        IDB_OBM6_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM6_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM7 CELL ASSEM BUFFER",
        IDB_OBM7_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM7_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM8 CELL ASSEM BUFFER",
        IDB_OBM8_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM8_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM9 CELL ASSEM BUFFER",
        IDB_OBM9_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM9_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM10 CELL ASSEM BUFFER",
        IDB_OBM10_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM10_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM11 CELL ASSEM BUFFER",
        IDB_OBM11_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM11_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM12 CELL ASSEM BUFFER",
        IDB_OBM12_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM12_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM13 CELL ASSEM BUFFER",
        IDB_OBM13_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM13_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM14 CELL ASSEM BUFFER",
        IDB_OBM14_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM14_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM15 CELL ASSEM BUFFER",
        IDB_OBM15_SER_CONTROLr, CA_FIFO_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM15_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM0 OBM QUEUE FIFO",
        IDB_OBM0_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM0_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM1 OBM QUEUE FIFO",
        IDB_OBM1_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM1_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM2 OBM QUEUE FIFO",
        IDB_OBM2_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM2_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM3 OBM QUEUE FIFO",
        IDB_OBM3_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM3_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM4 OBM QUEUE FIFO",
        IDB_OBM4_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM4_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM5 OBM QUEUE FIFO",
        IDB_OBM5_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM5_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM6 OBM QUEUE FIFO",
        IDB_OBM6_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM6_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM7 OBM QUEUE FIFO",
        IDB_OBM7_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM7_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM8 OBM QUEUE FIFO",
        IDB_OBM8_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM8_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM9 OBM QUEUE FIFO",
        IDB_OBM9_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM9_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM10 OBM QUEUE FIFO",
        IDB_OBM10_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM10_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM11 OBM QUEUE FIFO",
        IDB_OBM11_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM11_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM12 OBM QUEUE FIFO",
        IDB_OBM12_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM12_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM13 OBM QUEUE FIFO",
        IDB_OBM13_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM13_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM14 OBM QUEUE FIFO",
        IDB_OBM14_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM14_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "OBM15 OBM QUEUE FIFO",
        IDB_OBM15_SER_CONTROLr, QUEUE_ECC_ENABLEf, NULL,
        INVALIDr, INVALIDf, NULL,
        IDB_OBM15_QUEUE_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "CA-LPBK memory ",
        IDB_SER_CONTROLr, INVALIDf, _soc_th2_idb_ca_lpbk_enable_fields,
        INVALIDr, INVALIDf, NULL,
        IDB_CA_LPBK_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "CA-CPU memory",
        IDB_SER_CONTROLr, INVALIDf, _soc_th2_idb_ca_cpu_enable_fields,
        INVALIDr, INVALIDf, NULL,
        IDB_CA_CPU_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "SOP_EVENT memory",
        IDB_SER_CONTROLr, INVALIDf, _soc_th2_idb_sop_event_enable_fields,
        INVALIDr, INVALIDf, NULL,
        IDB_SOP_EVENT_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        IDB_SER_CONTROLr, INVALIDf, _soc_th2_idb_eop_event_enable_fields,
        INVALIDr, INVALIDf, NULL,
        IDB_EOP_EVENT_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    

    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_th_ser_info_t _soc_th2_ip7_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0, 
        SLOT_PIPELINE_ECC_ERRf, SLOT_PIPELINE_ECC_ERRf,
        INVALIDm, "Slot pipeline",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        SLOT_PIPELINE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0, 
        CELL_QUEUE_ECC_ERRf, CELL_QUEUE_ECC_ERRf,
        INVALIDm, "Cell queue",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CELL_QUEUE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0, 
        EOP_BUFFER_MTU_ECC_ERRf, EOP_BUFFER_MTU_ECC_ERRf,
        INVALIDm, "EOP_BUFFER_MTU",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        EOP_BUFFER_MTU_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0, 
        EVENT_FIFO_ECC_ERRf, EVENT_FIFO_ECC_ERRf,
        INVALIDm, "Event FIFO",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        EVENT_FIFO_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};



static _soc_th_ser_reg_t _soc_th2_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC" },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_reg_t _soc_th2_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC" },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_info_t _soc_th2_pm_clp_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th2_pm_clp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th2_pm_clp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC single bit",
        CLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC TX CDC double bits",
        CLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC single bit",
        CLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX CDC double bits",
        CLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX TS MEM single bit",
        CLMAC_ECC_CTRLr, RX_TS_MEM_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_TS_MEM_SINGLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_CLMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CLMAC RX TS MEM double bits",
        CLMAC_ECC_CTRLr, RX_TS_MEM_ECC_CTRL_ENf, NULL,
        CLMAC_INTR_ENABLEr, EN_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL,
        CLMAC_INTR_STATUSr, NULL, SUM_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL,
        CLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf, NULL
    },

    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_th_ser_reg_t _soc_th2_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_reg_t _soc_th2_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" },
    { INVALIDr, "INVALID"}
};


static _soc_th_ser_info_t _soc_th2_pm_xlp_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th2_pm_xlp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th2_pm_xlp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static soc_field_t _soc_th2_mmu_top_int_enable_fields[] = {
    SED_R_CPU_INT_ENf,
    SED_S_CPU_INT_ENf,
    SC_R_CPU_INT_ENf,
    SC_S_CPU_INT_ENf,
    XPE_R_B_CPU_INT_ENf,
    XPE_R_A_CPU_INT_ENf,
    XPE_S_B_CPU_INT_ENf,
    XPE_S_A_CPU_INT_ENf,
    GLB_CPU_INT_ENf,
    INVALIDf
};

static soc_field_t _soc_th2_mmu_top_int_status_fields[] = {
    SED_R_CPU_INT_STATf,
    SED_S_CPU_INT_STATf,
    SC_R_CPU_INT_STATf,
    SC_S_CPU_INT_STATf,
    XPE_R_B_CPU_INT_STATf,
    XPE_R_A_CPU_INT_STATf,
    XPE_S_B_CPU_INT_STATf,
    XPE_S_A_CPU_INT_STATf,
    GLB_CPU_INT_STATf,
    INVALIDf
};

static soc_field_t _soc_th2_mmu_xpe_enable_fields[] = {
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

static soc_field_t _soc_th2_mmu_sc_enable_fields[] = {
    TDM_PARITY_ENf,
    VBS_PARITY_ENf,
    TOQ_B_PARITY_ENf,
    TOQ_A_PARITY_ENf,
    INVALIDf
};


static soc_field_t _soc_th2_mmu_sed_enable_fields[] = {
    MB_A_PARITY_ENf,
    MB_B_PARITY_ENf,
    ENQS_PARITY_ENf,
    MTRO_PARITY_ENf,
    DQS_PARITY_ENf,
    CFAP_B_PARITY_ENf,
    CFAP_A_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_th2_mmu_sed_int_enable_fields[] = {
    CFAP_A_MEM_FAIL_ENf,
    CFAP_B_MEM_FAIL_ENf,
    MEM_PAR_ERR_ENf,
    INVALIDf
};
static soc_field_t _soc_th2_mmu_sed_int_status_fields[] = {
    CFAP_A_MEM_FAIL_STATf,
    CFAP_B_MEM_FAIL_STATf,
    MEM_PAR_ERR_STATf,
    INVALIDf
};
static soc_field_t _soc_th2_mmu_sed_int_clr_fields[] = {
    CFAP_A_MEM_FAIL_CLRf,
    CFAP_B_MEM_FAIL_CLRf,
    MEM_PAR_ERR_CLRf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th2_mmu_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_GLB, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU GLB MEM PAR",
        MMU_GCFG_PARITY_ENr, INTFO_PARITY_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_GCFG_GLB_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_XPE, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU XPE MEM PAR",
        MMU_XCFG_PARITY_ENr, INVALIDf, _soc_th2_mmu_xpe_enable_fields,
        MMU_XCFG_XPE_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_XCFG_XPE_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_XCFG_XPE_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_SC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SC MEM PAR",
        MMU_SCFG_PARITY_ENr, INVALIDf, _soc_th2_mmu_sc_enable_fields,
        MMU_SCFG_SC_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_SCFG_SC_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, 
        NULL,
        MMU_SCFG_SC_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, 
        NULL
    },
    { _SOC_PARITY_TYPE_MMU_SED, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SED MEM PAR",
        MMU_SEDCFG_PARITY_ENr, INVALIDf, _soc_th2_mmu_sed_enable_fields,
        MMU_SEDCFG_SED_CPU_INT_ENr, INVALIDf, _soc_th2_mmu_sed_int_enable_fields,
        MMU_SEDCFG_SED_CPU_INT_STATr, NULL, INVALIDf,
        _soc_th2_mmu_sed_int_status_fields,
        MMU_SEDCFG_SED_CPU_INT_CLEARr, INVALIDf,
        _soc_th2_mmu_sed_int_clr_fields
    },

    
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


static _soc_th_ser_info_t _soc_th2_mmu_top_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_SER, _soc_th2_mmu_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GCFG_ALL_CPU_INT_ENr, INVALIDf,
        _soc_th2_mmu_top_int_enable_fields,
        MMU_GCFG_ALL_CPU_INT_STATr,
        NULL, INVALIDf, _soc_th2_mmu_top_int_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static const
_soc_th_ser_route_block_t  _soc_th2_ser_route_blocks[] = {
    /* CMIC_CMC_IRQ_STAT3 */
    { 3, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTRf */
      SOC_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      _soc_th2_mmu_top_ser_info, 0 },  
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    
    { 3, 0x00000020, /* PIPE0_EP2_TO_CMIC_PERR_INTRf */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000040, /* PIPE1_EP2_TO_CMIC_PERR_INTRf */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000080, /* PIPE2_EP2_TO_CMIC_PERR_INTRf */
      SOC_BLK_EPIPE, 2, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000100, /* PIPE3_EP2_TO_CMIC_PERR_INTRf */
      SOC_BLK_EPIPE, 3, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },

    
    /* Note: This is an exception for being legacy style in IP0 not fifo mode.
        * These mask bit are set for IP0 IDB Mem reset complete or IDB memories SER
        *  interrupt, please check IP0_INTR_STATUS
        */
    { 3, 0x00000200, /* PIPE0_IP0_TO_CMIC_PERR_INTRf */
      SOC_BLK_IPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th2_idb_ser_info, 0 },
    { 3, 0x00000400, /* PIPE1_IP0_TO_CMIC_PERR_INTRf */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th2_idb_ser_info, 0 },
    { 3, 0x00000800, /* PIPE2_IP0_TO_CMIC_PERR_INTRf */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 2, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th2_idb_ser_info, 0 },
    { 3, 0x000001000, /* PIPE3_IP0_TO_CMIC_PERR_INTRf */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 3, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th2_idb_ser_info, 0 },
      
    { 3, 0x00002000, /* PIPE0_IP6_TO_CMIC_PERR_INTRf */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00004000, /* PIPE1_IP6_TO_CMIC_PERR_INTRf */
      SOC_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00008000, /* PIPE2_IP6_TO_CMIC_PERR_INTRf */
      SOC_BLK_IPIPE, 2, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00010000, /* PIPE3_IP6_TO_CMIC_PERR_INTRf */
      SOC_BLK_IPIPE, 3, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },

    /* IP7 interrupt signal for any parity/ecc error events, 
         * as well as some fatal fifo full/empty events in Isw4 (IP) */         
    /* Note: This is an exception for being legacy style in IP */
    { 3, 0x04000000, /* PIPE0_IP7_TO_CMIC_INTRf */
      SOC_BLK_IPIPE, 0, IP7_INTR_ENABLEr, IP7_INTR_STATUSr, INVALIDf,
      _soc_th2_ip7_ser_info, 0 },
    { 3, 0x08000000, /* PIPE1_IP7_TO_CMIC_INTRf */
      SOC_BLK_IPIPE, 1, IP7_INTR_ENABLEr, IP7_INTR_STATUSr, INVALIDf,
      _soc_th2_ip7_ser_info, 0 },
    { 3, 0x10000000, /* PIPE2_IP7_TO_CMIC_INTRf */
      SOC_BLK_IPIPE, 2, IP7_INTR_ENABLEr, IP7_INTR_STATUSr, INVALIDf,
      _soc_th2_ip7_ser_info, 0 },
    { 3, 0x20000000, /* PIPE3_IP7_TO_CMIC_INTRf */
      SOC_BLK_IPIPE, 3, IP7_INTR_ENABLEr, IP7_INTR_STATUSr, INVALIDf,
      _soc_th2_ip7_ser_info, 0 },
    

    /* CMIC_CMC_IRQ_STAT4 */
    { 4, 0x00000001, /* PM0_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 0 },
    { 4, 0x00000002, /* PM1_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 1 },
    { 4, 0x00000004, /* PM2_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 2 },
    { 4, 0x00000008, /* PM3_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 3 },
    { 4, 0x00000010, /* PM4_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 4 },
    { 4, 0x00000020, /* PM5_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 5 },
    { 4, 0x00000040, /* PM6_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 6 },
    { 4, 0x00000080, /* PM7_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 7 },
    { 4, 0x00000100, /* PM8_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 8 },
    { 4, 0x00000200, /* PM9_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 9 },
    { 4, 0x00000400, /* PM10_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 10 },
    { 4, 0x00000800, /* PM11_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 11 },
    { 4, 0x00001000, /* PM12_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 12 },
    { 4, 0x00002000, /* PM13_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 13 },
    { 4, 0x00004000, /* PM14_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 14 },
    { 4, 0x00008000, /* PM15_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 15 },
    { 4, 0x00010000, /* PM16_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 16 },
    { 4, 0x00020000, /* PM17_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 17 },
    { 4, 0x00040000, /* PM18_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 18 },
    { 4, 0x00080000, /* PM19_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 19 },
    { 4, 0x00100000, /* PM20_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 20 },
    { 4, 0x00200000, /* PM21_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 21 },
    { 4, 0x00400000, /* PM22_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 22 },
    { 4, 0x00800000, /* PM23_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 23 },
    { 4, 0x01000000, /* PM24_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 24 },
    { 4, 0x02000000, /* PM25_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 25 },
    { 4, 0x04000000, /* PM26_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 26 },
    { 4, 0x08000000, /* PM27_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 27 },
    { 4, 0x10000000, /* PM28_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 28 },
    { 4, 0x20000000, /* PM29_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 29 },
    { 4, 0x40000000, /* PM30_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 30 },
    { 4, 0x80000000, /* PM31_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 31 },

    /* CMIC_CMC_IRQ_STAT6 */
    { 6, 0x00000001, /* PM32_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 32 },
    { 6, 0x00000002, /* PM33_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 33 },
    { 6, 0x00000004, /* PM34_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 34 },
    { 6, 0x00000008, /* PM35_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 35 },
    { 6, 0x00000010, /* PM36_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 36 },
    { 6, 0x00000020, /* PM37_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 37 },
    { 6, 0x00000040, /* PM38_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 38 },
    { 6, 0x00000080, /* PM39_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 39 },
    { 6, 0x00000100, /* PM40_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 40 },
    { 6, 0x00000200, /* PM41_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 41 },
    { 6, 0x00000400, /* PM42_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 42 },
    { 6, 0x00000800, /* PM43_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 43 },
    { 6, 0x00001000, /* PM44_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 44 },
    { 6, 0x00002000, /* PM45_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 45 },
    { 6, 0x00004000, /* PM46_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 46 },
    { 6, 0x00008000, /* PM47_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 47 },
    { 6, 0x00010000, /* PM48_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 48 },
    { 6, 0x00020000, /* PM49_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 49 },
    { 6, 0x00040000, /* PM50_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 50 },
    { 6, 0x00080000, /* PM51_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 51 },
    { 6, 0x00100000, /* PM52_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 52 },
    { 6, 0x00200000, /* PM53_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 53 },
    { 6, 0x00400000, /* PM54_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 54 },
    { 6, 0x00800000, /* PM55_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 55 },
    { 6, 0x01000000, /* PM56_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 56 },
    { 6, 0x02000000, /* PM57_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 57 },
    { 6, 0x04000000, /* PM58_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 58 },
    { 6, 0x08000000, /* PM59_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 59 },
    { 6, 0x10000000, /* PM60_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 60 },
    { 6, 0x20000000, /* PM61_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 61 },
    { 6, 0x40000000, /* PM62_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 62 },
    { 6, 0x80000000, /* PM63_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_clp_ser_info, 63 },

    /* CMIC_CMC_IRQ_STAT5 */
    { 5, 0x00000001, /* PM64_INTR */
      SOC_BLK_XLPORT, 1, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_th2_pm_xlp_ser_info, 0 },

    { 0 } /* table terminator */
};

#define _SOC_MMU_BASE_INDEX_SED0 0
#define _SOC_MMU_BASE_INDEX_SED1 1

static const
_soc_th_ser_mmu_intr_info_t th2_mmu_intr_info[] = {
    { GLB_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_GLB, 0 },
    { XPE_R_A_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE0, 1 },
    { XPE_S_A_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE1, 1 },
    { XPE_R_B_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE2, 1 },
    { XPE_S_B_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE3, 1 },
    { SC_R_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC0, 2 },
    { SC_S_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC1, 2},    
    { SED_R_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SED0, 3 },
    { SED_S_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SED1, 3},
    
    { INVALIDf, -1, -1 } 
};

static _soc_tcam_ser_en_info_t _soc_th2_tcam_wrapper_table[] = {
    { L3_TUNNELm, L3_TUNNEL_CAM_SER_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { ING_SNATm, ING_SNAT_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { VLAN_SUBNETm, VLAN_SUBNET_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { MY_STATION_TCAMm, MY_STATION_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { L2_USER_ENTRYm, L2_USER_ENTRY_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { SRC_COMPRESSIONm, SRC_COMPRESSION_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { DST_COMPRESSIONm, DST_COMPRESSION_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { FP_UDF_TCAMm, UDF_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { VFP_TCAMm, VFP_CAM_BIST_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},   
    { IP_MULTICAST_TCAMm, IP_MULTICAST_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { CPU_COS_MAPm, CPU_COS_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { L3_DEFIPm, ILPM_SER_CONTROLr, L3_DEFIP_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { L3_DEFIP_PAIR_128m, ILPM_SER_CONTROLr, L3_DEFIP_TCAM_PARITY_ENf, TCAM_PARITY_MASK_UPRf},
    { IFP_TCAMm, IFP_TCAM_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { IFP_TCAM_WIDEm, IFP_TCAM_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { INVALIDm},
};

static soc_mem_t _soc_th2_skip_populating[] = {
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

    /*Shared MMU_SCFG_PARITY_EN.TDM_PARITY_EN*/
    TDM_CALENDAR0m,
    TDM_CALENDAR1m,

    INVALIDm
};

#endif

#ifdef BCM_TOMAHAWK2_SUPPORT
static uint32 _soc_th2_ip_pipe_fifo_bmask[SOC_MAX_NUM_DEVICES][4];

STATIC soc_error_t _soc_th2_ser_tcam_wrapper_found(soc_mem_t mem);
STATIC soc_error_t  _soc_th2_tcam_wrapper_enable(int unit, int enable);
STATIC soc_error_t
_soc_th2_ser_tcam_control_reg_get(soc_mem_t mem, soc_reg_t *ser_control_reg,
                                  soc_field_t *ser_enable_field);
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
STATIC int
_soc_th3_reg32_par_en_modify(int unit, soc_reg_t reg, soc_field_t field,
                             int enable);
STATIC int
_soc_tomahawk3_idb_enable(int unit, soc_reg_t reg_name, int enable, int *is_idb);

extern _soc_bus_ser_en_info_t _soc_bcm56980_a0_ip_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56980_a0_ip_buffer_ser_info[];
extern _soc_bus_ser_en_info_t _soc_bcm56980_a0_ep_bus_ser_info[];
extern _soc_buffer_ser_en_info_t _soc_bcm56980_a0_ep_buffer_ser_info[];

uint32 ser_th3_bus_buffer_hwbase[] = {
    0x5e, /* EFP_BUS */
    0x64, /* EPOST_OUTPUT_EVENT_DATA_FIFO */
    0x5c, /* EPMOD_BUS */
    0xb,  /* PT_HWY_IADAPT */
    0x65, /* EPOST_SLOT_PIPELINE */
    0,    /* DPP_OUTPUT_EVENT_FIFO */
    0x42, /* EARB_SOP_EVENT_FIFO */
    0x10, /* IFWD1_PASSTHRU_BUS */
    0x9,  /* SW_EOP_BUFFER_B */
    0x19, /* BUS_IRSEL */
    0x60, /* EP_EFPBUF */
    0xf,  /* ALL_CELLS_IPARS_TO_ISW_PT_HWY_FIFO */
    0x17, /* PT_HWY_IFP */
    0x5d, /* EFPPARS_BUS */
    0,    /* CELL_QUEUE */
    0x1,  /* IARB_EVENT_FIFO */
    0,    /* SLOT_PIPELINE */
    0x66, /* EPOST_PKT_BUF */
    0x64, /* EPOST_OUTPUT_EVENT_FIFO */
    0x41, /* EP_PACKET_DATA_BUS */
    0x40, /* EP_MPB_BUS */
    0x18  /* PT_HWY_IRSEL */
};

_soc_bus_buffer_ser_force_info_t ser_th3_bus_buffer_skip_test[] = {
    { "EPOST_SLOT_PIPELINE", EPOST_SER_CONTROLr, SLOT_PIPELINE_ECC_CORRUPTf },
    { "DPP_OUTPUT_EVENT_FIFO", IPOST_SER_CONTROLr, EVENT_FIFO_ECC_FORCE_ERRf },
    { "EARB_SOP_EVENT_FIFO", EGR_EARB_SER_CONTROLr, EARB_SOP_EVENT_FIFO_ECC_FORCE_ERRf },
    { "IARB_EVENT_FIFO", IARB_SER_CONTROLr, EVENT_FIFO_ECC_CORRUPTf },
    { "SLOT_PIPELINE", IPOST_SER_CONTROLr, SLOT_PIPELINE_ECC_FORCE_ERRf },
    { "EPOST_OUTPUT_EVENT_FIFO", EPOST_SER_CONTROLr, OUTPUT_FIFO_ECC_CORRUPTf },
    { "CELL_QUEUE", IPOST_SER_CONTROLr, CELL_QUEUE_ECC_FORCE_ERRf },
    { "", INVALIDr } /* end */
};

_soc_buffer_ser_en_info_t _soc_th3_no_mapping_ser_info[] = {
    { "TCAM POOL0", IFP_TCAM_POOL0_BISTCTRLr, PARITY_ENf, INVALIDr, INVALIDf },
    { "TCAM POOL1", IFP_TCAM_POOL1_BISTCTRLr, PARITY_ENf, INVALIDr, INVALIDf },
    { "TCAM POOL2", IFP_TCAM_POOL2_BISTCTRLr, PARITY_ENf, INVALIDr, INVALIDf },
    { "TCAM POOL3", IFP_TCAM_POOL3_BISTCTRLr, PARITY_ENf, INVALIDr, INVALIDf },
    { "VFP CAM BIST CONTROL", VFP_CAM_BIST_CONTROLr, PARITY_ENf, INVALIDr, INVALIDf },
    { "METER MUX DATA STAGING", IFP_PARITY_CONTROLr, METER_MUX_DATA_STAGING_PARITY_ENf, INVALIDr, INVALIDf },
    { "PORT METER MAP", IFP_PARITY_CONTROLr, PORT_METER_MAP_PARITY_ENf, INVALIDr, INVALIDf },
    { "COUNTER MUX DATA STAGING", IFP_PARITY_CONTROLr, COUNTER_MUX_DATA_STAGING_PARITY_ENf, INVALIDr, INVALIDf },
    { "IFP COUNTER", IFP_PARITY_CONTROLr, COUNTER_PARITY_ENf, INVALIDr, INVALIDf },
    { "EGR PW INIT COUNTER", EGR_EFP_EN_COR_ERR_RPTr, EGR_PW_INIT_COUNTER_EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "EGR METER TABLE", EGR_EFP_EN_COR_ERR_RPTr, EGR_METER_TABLE_EN_COR_ERR_RPTf, INVALIDr, INVALIDf },
    { "EINITBUF", EGR_ESW_EN_COR_ERR_RPTr, EINITBUFf, INVALIDr, INVALIDf },
    { "ESW_BUS", EGR_ESW_SER_CONTROLr, ESW_BUS_PARITY_ENf, INVALIDr, INVALIDf },
    { "", INVALIDr }
};

static soc_field_t _soc_th3_idb_obm_enable_fields[] = {
    MON_EN_COR_ERR_RPTf,
    MON_ECC_ENABLEf,
    CTRL_EN_COR_ERR_RPTf,
    CTRL_ECC_ENABLEf,
    DATA_EN_COR_ERR_RPTf,
    DATA_ECC_ENABLEf,
    INVALIDf
};

static _soc_th_ser_reg_t _soc_th3_idb_obm0_intr_status_regs[] = {
    { IDB_OBM0_DATA_ECC_STATUSr, "IDB_OBM0_DATA_ECC_STATUS" },
    { IDB_OBM0_CTRL_ECC_STATUSr, "IDB_OBM0_CTRL_ECC_STATUS" },
    { IDB_OBM0_OVERSUB_MON_ECC_STATUSr, "IDB_OBM0_OVERSUB_MON_ECC_STATUS" },
    { INVALIDr, "INVALID" }
};
static _soc_th_ser_reg_t _soc_th3_idb_obm1_intr_status_regs[] = {
    { IDB_OBM1_DATA_ECC_STATUSr, "IDB_OBM1_DATA_ECC_STATUS" },
    { IDB_OBM1_CTRL_ECC_STATUSr, "IDB_OBM1_CTRL_ECC_STATUS" },
    { IDB_OBM1_OVERSUB_MON_ECC_STATUSr, "IDB_OBM1_OVERSUB_MON_ECC_STATUS" },
    { INVALIDr, "INVALID" }
};
static _soc_th_ser_reg_t _soc_th3_idb_obm2_intr_status_regs[] = {
    { IDB_OBM2_DATA_ECC_STATUSr, "IDB_OBM2_DATA_ECC_STATUS" },
    { IDB_OBM2_CTRL_ECC_STATUSr, "IDB_OBM2_CTRL_ECC_STATUS" },
    { IDB_OBM2_OVERSUB_MON_ECC_STATUSr, "IDB_OBM2_OVERSUB_MON_ECC_STATUS" },
    { INVALIDr, "INVALID" }
};
static _soc_th_ser_reg_t _soc_th3_idb_obm3_intr_status_regs[] = {
    { IDB_OBM3_DATA_ECC_STATUSr, "IDB_OBM3_DATA_ECC_STATUS" },
    { IDB_OBM3_CTRL_ECC_STATUSr, "IDB_OBM3_CTRL_ECC_STATUS" },
    { IDB_OBM3_OVERSUB_MON_ECC_STATUSr, "IDB_OBM3_OVERSUB_MON_ECC_STATUS" },
    { INVALIDr, "INVALID" }
};

soc_field_t _soc_th3_idb_ca_lpbk_enable_fields[] = {
    CA_LPBK_FIFO_ECC_ENABLEf,
    CA_LPBK_FIFO_EN_COR_ERR_RPTf,
    INVALIDf
};

soc_field_t _soc_th3_idb_ca_cpu_enable_fields[] = {
    CA_CPU_FIFO_ECC_ENABLEf,
    CA_CPU_FIFO_EN_COR_ERR_RPTf,
    INVALIDf
};

soc_field_t _soc_th3_idb_ca_fifo_enable_fields[] = {
    CA_FIFO_EN_COR_ERR_RPTf,
    CA_FIFO_ECC_ENABLEf,
    INVALIDf
};


static _soc_th_ser_info_t _soc_th3_idb_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        LPBK_FIFO_ECC_ERRf, LPBK_FIFO_ECC_ERRf,
        INVALIDm, "CA-LPBK packet buffer memory",
        IDB_CA_LPBK_SER_CONTROLr, INVALIDf, _soc_th3_idb_ca_lpbk_enable_fields,
        INVALIDr, INVALIDf, NULL,
        IDB_CA_LPBK_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        CPU_FIFO_ECC_ERRf, CPU_FIFO_ECC_ERRf,
        INVALIDm, "CA-CPU packet buffer memory",
        IDB_CA_CPU_SER_CONTROLr, INVALIDf, _soc_th3_idb_ca_cpu_enable_fields,
        INVALIDr, INVALIDf, NULL,
        IDB_CA_CPU_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        CA3_FIFO_ECC_ERRf, CA3_FIFO_ECC_ERRf,
        INVALIDm, "CA3 FIFO",
        IDB_CA3_SER_CONTROLr, INVALIDf, _soc_th3_idb_ca_fifo_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        IDB_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        OBM3_ECC_ERRf, OBM3_ECC_ERRf,
        INVALIDm, "IDB OBM3 packet data memory",
        IDB_OBM3_SER_CONTROLr, INVALIDf, _soc_th3_idb_obm_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        INVALIDr, _soc_th3_idb_obm3_intr_status_regs, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        CA2_FIFO_ECC_ERRf, CA2_FIFO_ECC_ERRf,
        INVALIDm, "CA2 FIFO",
        IDB_CA2_SER_CONTROLr, INVALIDf, _soc_th3_idb_ca_fifo_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        IDB_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        OBM2_ECC_ERRf, OBM2_ECC_ERRf,
        INVALIDm, "IDB OBM2 packet data memory",
        IDB_OBM2_SER_CONTROLr, INVALIDf, _soc_th3_idb_obm_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        INVALIDr, _soc_th3_idb_obm2_intr_status_regs, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        CA1_FIFO_ECC_ERRf, CA1_FIFO_ECC_ERRf,
        INVALIDm, "CA1 FIFO",
        IDB_CA1_SER_CONTROLr, INVALIDf, _soc_th3_idb_ca_fifo_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        IDB_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        OBM1_ECC_ERRf, OBM1_ECC_ERRf,
        INVALIDm, "IDB OBM1 packet data memory",
        IDB_OBM1_SER_CONTROLr, INVALIDf, _soc_th3_idb_obm_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        INVALIDr, _soc_th3_idb_obm1_intr_status_regs, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        CA0_FIFO_ECC_ERRf, CA0_FIFO_ECC_ERRf,
        INVALIDm, "CA0 FIFO",
        IDB_CA0_SER_CONTROLr, INVALIDf, _soc_th3_idb_ca_fifo_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        IDB_CA_ECC_STATUSr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        OBM0_ECC_ERRf, OBM0_ECC_ERRf,
        INVALIDm, "IDB OBM0 packet data memory",
        IDB_OBM0_SER_CONTROLr, INVALIDf, _soc_th3_idb_obm_enable_fields,
        INVALIDr, INVALIDf, NULL,
        /* need to confirm the status reg*/
        INVALIDr, _soc_th3_idb_obm0_intr_status_regs, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    

    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

soc_field_t _soc_th3_edb_ctrl_buffer_enable_fields[] = {
    PM3_ECC_ENf,
    PM2_ECC_ENf,
    PM1_ECC_ENf,
    PM0_ECC_ENf,
    CM_ECC_ENf,
    LPBK_ECC_ENf,
    XMIT_START_COUNT_ECC_ENf,
    INVALIDf
};

soc_field_t _soc_th3_edb_ctrl_buffer_1b_enable_fields[] = {
    PM3_ENf,
    PM2_ENf,
    PM1_ENf,
    PM0_ENf,
    CM_ENf,
    LPBK_ENf,
    XMIT_START_COUNT_ENf,
    INVALIDf
};

soc_field_t _soc_th3_edb_data_buffer_enable_fields[] = {
    PM3_ECC_ENf,
    PM2_ECC_ENf,
    PM1_ECC_ENf,
    PM0_ECC_ENf,
    RESIDUAL_ECC_ENf,
    CM_ECC_ENf,
    LPBK_ECC_ENf,
    INVALIDf
};

soc_field_t _soc_th3_edb_data_buffer_1b_enable_fields[] = {
    PM3_ENf,
    PM2_ENf,
    PM1_ENf,
    PM0_ENf,
    RESIDUAL_ENf,
    CM_ENf,
    LPBK_ENf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th3_edb_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "EDB control buffer",
        EDB_CONTROL_BUFFER_ECC_ENr, INVALIDf, _soc_th3_edb_ctrl_buffer_enable_fields,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "EDB data buffer",
        EDB_DATA_BUFFER_ECC_ENr, INVALIDf, _soc_th3_edb_data_buffer_enable_fields,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "EDB control buffer 1-bit",
        EDB_CONTROL_BUFFER_EN_COR_ERR_RPTr, INVALIDf, _soc_th3_edb_ctrl_buffer_1b_enable_fields,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "EDB data buffer 1-bit",
        EDB_DATA_BUFFER_EN_COR_ERR_RPTr, INVALIDf, _soc_th3_edb_data_buffer_1b_enable_fields,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_PARITY_TYPE_NONE }
};

static _soc_th_ser_info_t _soc_th3_ipost_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        SLOT_PIPELINE_ECC_ERRf, SLOT_PIPELINE_ECC_ERRf,
        INVALIDm, "Slot pipeline",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        SLOT_PIPELINE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        CELL_QUEUE_ECC_ERRf, CELL_QUEUE_ECC_ERRf,
        INVALIDm, "Cell queue",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CELL_QUEUE_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    /*{ _SOC_PARITY_TYPE_ECC, NULL, 0,
        EOP_BUFFER_MTU_ECC_ERRf, EOP_BUFFER_MTU_ECC_ERRf,
        INVALIDm, "EOP_BUFFER_MTU",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },*/
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        EVENT_FIFO_ECC_ERRf, EVENT_FIFO_ECC_ERRf,
        INVALIDm, "Event FIFO",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        EVENT_FIFO_ECC_STATUS_INTRr, NULL, ECC_ERRf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


static _soc_th_ser_reg_t _soc_th3_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_reg_t _soc_th3_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC" },
    { INVALIDr, "INVALID"}
};

static _soc_th_ser_info_t _soc_th3_pm_xlp_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th3_pm_xlp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th3_pm_xlp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_PARITY_TYPE_MAC_TX_CDC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "XLMAC TX CDC single bit",
        XLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_TX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_MAC_TX_CDC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "XLMAC TX CDC double bits",
        XLMAC_ECC_CTRLr, TX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_TX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_TX_CDC_DOUBLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_MAC_RX_CDC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "XLMAC RX CDC single bit",
        XLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_RX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_SINGLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_SINGLE_BIT_ERRf, NULL
    },
    { _SOC_PARITY_TYPE_MAC_RX_CDC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "XLMAC RX CDC double bits",
        XLMAC_ECC_CTRLr, RX_CDC_ECC_CTRL_ENf, NULL,
        XLMAC_INTR_ENABLEr, EN_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_INTR_STATUSr, NULL, SUM_RX_CDC_DOUBLE_BIT_ERRf, NULL,
        XLMAC_CLEAR_ECC_STATUSr, CLEAR_RX_CDC_DOUBLE_BIT_ERRf, NULL
    },

    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

soc_field_t _soc_th3_cdmac_ecc_enable_fields[] = {
    TX_CDC_ECC_CTRL_ENf,
    MIB_COUNTER_ECC_CTRL_ENf,
    INVALIDf
};

soc_field_t _soc_th3_cdmac_intr_enable_fields[] = {
    TX_CDC_SINGLE_BIT_ERRf,
    TX_CDC_DOUBLE_BIT_ERRf,
    MIB_COUNTER_SINGLE_BIT_ERRf,
    MIB_COUNTER_DOUBLE_BIT_ERRf,
    MIB_COUNTER_MULTIPLE_ERRf,
    INVALIDf
};

soc_field_t _soc_th3_cdmac_ecc_status_fields[] = {
    TX_CDC_SINGLE_BIT_ERRf,
    TX_CDC_DOUBLE_BIT_ERRf,
    MIB_COUNTER_SINGLE_BIT_ERRf,
    MIB_COUNTER_DOUBLE_BIT_ERRf,
    MIB_COUNTER_MULTIPLE_ERRf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th3_pm_cdp_ser_info[] = {
    { _SOC_PARITY_TYPE_CDMAC, NULL, 0,
        MAC_ERRf, MAC_ERRf,
        INVALIDm, "CDMAC",
        CDMAC_ECC_CTRLr, INVALIDf, _soc_th3_cdmac_ecc_enable_fields,
        CDMAC_INTR_ENABLEr, INVALIDf, _soc_th3_cdmac_intr_enable_fields,
        CDMAC_ECC_STATUSr, NULL, INVALIDf, _soc_th3_cdmac_ecc_status_fields,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


static soc_field_t _soc_th3_mmu_top_int_enable_fields[] = {
    ITMCFG1_INT_ENf,
    ITMCFG0_INT_ENf,
    EBCFG7_INT_ENf,
    EBCFG6_INT_ENf,
    EBCFG5_INT_ENf,
    EBCFG4_INT_ENf,
    EBCFG3_INT_ENf,
    EBCFG2_INT_ENf,
    EBCFG1_INT_ENf,
    EBCFG0_INT_ENf,
    MEM_PAR_ERR_INT_ENf,
    INVALIDf
};

static soc_field_t _soc_th3_mmu_top_int_status_fields[] = {
    ITMCFG1_INT_STATf,
    ITMCFG0_INT_STATf,
    EBCFG7_INT_STATf,
    EBCFG6_INT_STATf,
    EBCFG5_INT_STATf,
    EBCFG4_INT_STATf,
    EBCFG3_INT_STATf,
    EBCFG2_INT_STATf,
    EBCFG1_INT_STATf,
    EBCFG0_INT_STATf,
    MEM_PAR_ERR_INT_STATf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th3_mmu_ser_info[] = {
    /* SW is allowed to clear an interrupt by writing 1'b1 to that status bit. */
    { _SOC_PARITY_TYPE_MMU_GLB, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU GLB MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GLBCFG_CPU_INT_ENr, MEM_PAR_ERR_INT_ENf, NULL,
        MMU_GLBCFG_CPU_INT_STATr, NULL, MEM_PAR_ERR_INT_STATf, NULL,
        MMU_GLBCFG_CPU_INT_STATr, MEM_PAR_ERR_INT_STATf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_ITMCFG, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU ITM MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_ITMCFG_CPU_INT_ENr, MEM_PAR_ERR_INT_ENf, NULL,
        MMU_ITMCFG_CPU_INT_STATr, NULL, MEM_PAR_ERR_INT_STATf, NULL,
        MMU_ITMCFG_CPU_INT_STATr, MEM_PAR_ERR_INT_STATf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_EBCFG, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU EB MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_EBCFG_CPU_INT_ENr, MEM_PAR_ERR_INT_ENf, NULL,
        MMU_EBCFG_CPU_INT_STATr, NULL, MEM_PAR_ERR_INT_STATf, NULL,
        MMU_EBCFG_CPU_INT_STATr, MEM_PAR_ERR_INT_STATf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


static _soc_th_ser_info_t _soc_th3_mmu_top_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_SER, _soc_th3_mmu_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GLBCFG_CPU_INT_ENr, INVALIDf,
        _soc_th3_mmu_top_int_enable_fields,
        MMU_GLBCFG_CPU_INT_STATr,
        NULL, INVALIDf, _soc_th3_mmu_top_int_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static soc_field_t _soc_th3_cev_ser_enable_fields[] = {
    PARITY_ENf,
    EN_COR_ERR_RPTf,
    INVALIDf
};
static _soc_th_ser_info_t _soc_th3_cev_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        FIFO_PARITY_ERRORf, FIFO_PARITY_ERRORf,
        INVALIDm, "Central fifo",
        CENTRAL_CTR_EVICTION_FIFO_PARITY_CONTROLr, INVALIDf, _soc_th3_cev_ser_enable_fields,
        INVALIDr, INVALIDf, NULL,
        CENTRAL_CTR_EVICTION_INTR_STATUSr, NULL, FIFO_PARITY_ERRORf, NULL,
        INVALIDr, INVALIDf, NULL
    },

    { _SOC_PARITY_TYPE_NONE} /* table terminator */
};

#define _SOC_TH3_CEV_INTR 0x00000001

static const
_soc_th_ser_route_block_t  _soc_th3_ser_route_blocks[] = {
    { 0, 0x00000001, /* CEV_TO_CMIC_INTR */
      SOC_BLK_IPIPE, -1, CENTRAL_CTR_EVICTION_INTR_ENABLEr, CENTRAL_CTR_EVICTION_INTR_STATUSr,
      INVALIDf, _soc_th3_cev_ser_info, 0 },
    { 0, 0x00000002, /* DLB_CREDIT_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 0, DLB_ECMP_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000010, /* MMU_TO_CMIC_INTR */
      SOC_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      _soc_th3_mmu_top_ser_info, 0 },
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    { 0, 0x00000020, /* PIPE0_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000040, /* PIPE1_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000080, /* PIPE2_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 2, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000100, /* PIPE3_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 3, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000200, /* PIPE4_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 4, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000400, /* PIPE5_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 5, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00000800, /* PIPE6_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 6, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00001000, /* PIPE7_EFPMOD_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 7, EGR_INTR_ENABLE_2r, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00002000, /* PIPE0_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00004000, /* PIPE1_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00008000, /* PIPE2_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 2, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00010000, /* PIPE3_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 3, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00020000, /* PIPE4_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 4, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00040000, /* PIPE5_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 5, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00080000, /* PIPE6_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 6, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 0, 0x00100000, /* PIPE7_EPOST_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 7, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
#if 0
    { 1, 0x00000001, /* PIPE0_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000002, /* PIPE1_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000004, /* PIPE2_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 2, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000008, /* PIPE3_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 3, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000010, /* PIPE4_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 4, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000020, /* PIPE5_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 5, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000040, /* PIPE6_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 6, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
    { 1, 0x00000080, /* PIPE7_IARB_TO_CMIC_INTR */
      SOC_BLK_EPIPE, 7, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      NULL, 0 },
#endif
    { 1, 0x00010000, /* PIPE0_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00020000, /* PIPE1_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00040000, /* PIPE2_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 2, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00080000, /* PIPE3_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 3, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00100000, /* PIPE4_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 4, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00200000, /* PIPE5_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 5, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00400000, /* PIPE6_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 6, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x00800000, /* PIPE7_IP_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 7, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 1, 0x01000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 0, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x02000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 1, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x04000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 2, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x08000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 3, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x10000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 4, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x20000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 5, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x40000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 6, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 1, 0x80000000, /* PIPE0_IPOST_TO_CMIC_INTR */
      SOC_BLK_IPIPE, 7, IPOST_INTR_ENABLEr, IPOST_INTR_STATUSr, INVALIDf,
      _soc_th3_ipost_ser_info, 0 },
    { 2, 0x00000001, /* PM0_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 0 },
    { 2, 0x00000002, /* PM1_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 1 },
    { 2, 0x00000004, /* PM2_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 2 },
    { 2, 0x00000008, /* PM3_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 3 },
    { 2, 0x00000010, /* PM4_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 4 },
    { 2, 0x00000020, /* PM5_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 5 },
    { 2, 0x00000040, /* PM6_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 6 },
    { 2, 0x00000080, /* PM7_INTR */
      SOC_BLK_CDPORT, 0, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 7 },
    { 2, 0x00000100, /* PM8_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 8 },
    { 2, 0x00000200, /* PM9_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 9 },
    { 2, 0x00000400, /* PM10_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 10 },
    { 2, 0x00000800, /* PM11_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 11 },
    { 2, 0x00001000, /* PM12_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 12 },
    { 2, 0x00002000, /* PM13_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 13 },
    { 2, 0x00004000, /* PM14_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 14 },
    { 2, 0x00008000, /* PM15_INTR */
      SOC_BLK_CDPORT, 1, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 15 },
    { 2, 0x00010000, /* PM16_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 16 },
    { 2, 0x00020000, /* PM17_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 17 },
    { 2, 0x00040000, /* PM18_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 18 },
    { 2, 0x00080000, /* PM19_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 19 },
    { 2, 0x00100000, /* PM20_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 20 },
    { 2, 0x00200000, /* PM21_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 21 },
    { 2, 0x00400000, /* PM22_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 22 },
    { 2, 0x00800000, /* PM23_INTR */
      SOC_BLK_CDPORT, 2, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 23 },
    { 2, 0x01000000, /* PM24_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 24 },
    { 2, 0x02000000, /* PM25_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 25 },
    { 2, 0x04000000, /* PM26_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 26 },
    { 2, 0x08000000, /* PM27_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 27 },
    { 2, 0x10000000, /* PM28_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 28 },
    { 2, 0x20000000, /* PM29_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 29 },
    { 2, 0x40000000, /* PM30_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 30 },
    { 2, 0x80000000, /* PM31_INTR */
      SOC_BLK_CDPORT, 3, CDPORT_INTR_ENABLEr, CDPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_cdp_ser_info, 31 },
    { 3, 0x00000001, /* PIPE0_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_XLPORT, 0, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_th3_pm_xlp_ser_info, 0 },
    { 3, 0x00000002, /* PIPE0_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 0, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000004, /* PIPE1_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 1, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000008, /* PIPE2_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 2, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000010, /* PIPE3_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 3, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000020, /* PIPE4_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 4, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000040, /* PIPE5_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 5, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000080, /* PIPE6_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 6, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000100, /* PIPE7_EDB_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 7, EDB_INTR_ENABLEr, EDB_INTR_STATUSr, SER_FIFO_NON_EMPTYf,
      _soc_th3_edb_ser_info, 0 },
    { 3, 0x00000200, /* PIPE0_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00000400, /* PIPE1_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00000800, /* PIPE2_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 2, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00001000, /* PIPE3_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 3, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00002000, /* PIPE4_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 4, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00004000, /* PIPE5_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 5, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00008000, /* PIPE6_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 6, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },
    { 3, 0x00010000, /* PIPE7_IDB_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 7, IDB_INTR_ENABLEr, IDB_INTR_STATUSr, INVALIDf,
      _soc_th3_idb_ser_info, 0 },

    { 0 } /* table terminator */
};

#if 0
static _soc_tcam_ser_en_info_t _soc_th3_tcam_wrapper_table[12] = {
    { MY_STATION_TCAMm, MY_STATION_TCAM_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { L2_USER_ENTRYm, L2_USER_ENTRY_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { FP_UDF_TCAMm, UDF_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { VFP_TCAMm, VFP_CAM_SER_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},   
    { CPU_COS_MAPm, CPU_COS_CAM_SERCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { L3_DEFIP_TCAM_LEVEL1m, ILPM_SER_CONTROLr, L3_DEFIP_TCAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { L3_DEFIP_PAIR_LEVEL1m, ILPM_SER_CONTROLr, L3_DEFIP_TCAM_PARITY_ENf, TCAM_PARITY_MASK_UPRf},
    { IFP_TCAMm, IFP_TCAM_CAM_CONTROLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { EFP_TCAMm, EFP_RAM_CONTROL_2_64r, EFP_CAM_PARITY_ENf, TCAM_PARITY_MASKf},
    { IFP_LOGICAL_TABLE_SELECTm, IFP_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, PARITY_ENf, TCAM_PARITY_MASKf},
    { INVALIDm},
};
#endif

static const
_soc_th_ser_mmu_intr_info_t th3_mmu_intr_info[] = {
    { MEM_PAR_ERR_INT_STATf, _SOC_MMU_BASE_INDEX_GLB, 0},
    { ITMCFG1_INT_STATf, _SOC_MMU_BASE_INDEX_ITM1, 1 },
    { ITMCFG0_INT_STATf, _SOC_MMU_BASE_INDEX_ITM0, 1 },
    { EBCFG7_INT_STATf, _SOC_MMU_BASE_INDEX_EB7, 2 },
    { EBCFG6_INT_STATf, _SOC_MMU_BASE_INDEX_EB6, 2 },
    { EBCFG5_INT_STATf, _SOC_MMU_BASE_INDEX_EB5, 2 },
    { EBCFG4_INT_STATf, _SOC_MMU_BASE_INDEX_EB4, 2 },
    { EBCFG3_INT_STATf, _SOC_MMU_BASE_INDEX_EB3, 2 },
    { EBCFG2_INT_STATf, _SOC_MMU_BASE_INDEX_EB2, 2 },
    { EBCFG1_INT_STATf, _SOC_MMU_BASE_INDEX_EB1, 2 },
    { EBCFG0_INT_STATf, _SOC_MMU_BASE_INDEX_EB0, 2 },
    { INVALIDf, -1, -1 }
};

static soc_mem_t _soc_th3_skip_populating[] = {
    /* Shared  MMU_WRED_ENABLE_ECCP_MEM.MMU_WRED_DROP_CURVE_PROFILE_ECCP_EN */
    MMU_WRED_DROP_CURVE_PROFILE_0_0m,
    MMU_WRED_DROP_CURVE_PROFILE_0_1m,
    MMU_WRED_DROP_CURVE_PROFILE_0_2m,
    MMU_WRED_DROP_CURVE_PROFILE_0_3m,
    MMU_WRED_DROP_CURVE_PROFILE_0_4m,
    MMU_WRED_DROP_CURVE_PROFILE_0_5m,
    MMU_WRED_DROP_CURVE_PROFILE_1_0m,
    MMU_WRED_DROP_CURVE_PROFILE_1_1m,
    MMU_WRED_DROP_CURVE_PROFILE_1_2m,
    MMU_WRED_DROP_CURVE_PROFILE_1_3m,
    MMU_WRED_DROP_CURVE_PROFILE_1_4m,
    MMU_WRED_DROP_CURVE_PROFILE_1_5m,
    MMU_WRED_DROP_CURVE_PROFILE_2_0m,
    MMU_WRED_DROP_CURVE_PROFILE_2_1m,
    MMU_WRED_DROP_CURVE_PROFILE_2_2m,
    MMU_WRED_DROP_CURVE_PROFILE_2_3m,
    MMU_WRED_DROP_CURVE_PROFILE_2_4m,
    MMU_WRED_DROP_CURVE_PROFILE_2_5m,
    MMU_WRED_DROP_CURVE_PROFILE_3_0m,
    MMU_WRED_DROP_CURVE_PROFILE_3_1m,
    MMU_WRED_DROP_CURVE_PROFILE_3_2m,
    MMU_WRED_DROP_CURVE_PROFILE_3_3m,
    MMU_WRED_DROP_CURVE_PROFILE_3_4m,
    MMU_WRED_DROP_CURVE_PROFILE_3_5m,
    MMU_WRED_DROP_CURVE_PROFILE_4_0m,
    MMU_WRED_DROP_CURVE_PROFILE_4_1m,
    MMU_WRED_DROP_CURVE_PROFILE_4_2m,
    MMU_WRED_DROP_CURVE_PROFILE_4_3m,
    MMU_WRED_DROP_CURVE_PROFILE_4_4m,
    MMU_WRED_DROP_CURVE_PROFILE_4_5m,
    MMU_WRED_DROP_CURVE_PROFILE_5_0m,
    MMU_WRED_DROP_CURVE_PROFILE_5_1m,
    MMU_WRED_DROP_CURVE_PROFILE_5_2m,
    MMU_WRED_DROP_CURVE_PROFILE_5_3m,
    MMU_WRED_DROP_CURVE_PROFILE_5_4m,
    MMU_WRED_DROP_CURVE_PROFILE_5_5m,
    MMU_WRED_DROP_CURVE_PROFILE_6_0m,
    MMU_WRED_DROP_CURVE_PROFILE_6_1m,
    MMU_WRED_DROP_CURVE_PROFILE_6_2m,
    MMU_WRED_DROP_CURVE_PROFILE_6_3m,
    MMU_WRED_DROP_CURVE_PROFILE_6_4m,
    MMU_WRED_DROP_CURVE_PROFILE_6_5m,
    MMU_WRED_DROP_CURVE_PROFILE_7_0m,
    MMU_WRED_DROP_CURVE_PROFILE_7_1m,
    MMU_WRED_DROP_CURVE_PROFILE_7_2m,
    MMU_WRED_DROP_CURVE_PROFILE_7_3m,
    MMU_WRED_DROP_CURVE_PROFILE_7_4m,
    MMU_WRED_DROP_CURVE_PROFILE_7_5m,
    MMU_WRED_DROP_CURVE_PROFILE_8_0m,
    MMU_WRED_DROP_CURVE_PROFILE_8_1m,
    MMU_WRED_DROP_CURVE_PROFILE_8_2m,
    MMU_WRED_DROP_CURVE_PROFILE_8_3m,
    MMU_WRED_DROP_CURVE_PROFILE_8_4m,
    MMU_WRED_DROP_CURVE_PROFILE_8_5m,
    INVALIDm
};

static uint32 _soc_th3_ip_pipe_fifo_bmask[SOC_MAX_NUM_DEVICES][8];
STATIC soc_error_t _soc_th3_ser_tcam_wrapper_found(soc_mem_t mem);
STATIC soc_error_t
_soc_th3_ser_tcam_control_reg_get(soc_mem_t mem, soc_reg_t *ser_control_reg,
                                  soc_field_t *ser_enable_field);
STATIC soc_error_t  _soc_th3_tcam_wrapper_enable(int unit, int enable);
int soc_th3_tcam_engine_enable(int unit, int enable);

#endif


#ifdef BCM_TOMAHAWKPLUS_SUPPORT
static const
_soc_th_ser_route_block_t  _soc_thplus_ser_route_blocks[] = {
    { 3, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTR */
      SOC_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      _soc_th_mmu_top_ser_info, 0 },
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    { 3, 0x00000020, /* PIPE0_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000040, /* PIPE1_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000080, /* PIPE2_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 2, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000100, /* PIPE3_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 3, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, _SOC_TH_IPIPE0_IP0_INTR, /* PIPE0_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, _SOC_TH_IPIPE1_IP0_INTR, /* PIPE1_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, _SOC_TH_IPIPE2_IP0_INTR, /* PIPE2_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 2, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, _SOC_TH_IPIPE3_IP0_INTR, /* PIPE3_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 3, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, 0x00002000, /* PIPE0_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00004000, /* PIPE1_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00008000, /* PIPE2_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 2, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00010000, /* PIPE3_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 3, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 4, 0x00000001, /* PM0_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 0 },
    { 4, 0x00000002, /* PM1_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 1 },
    { 4, 0x00000004, /* PM2_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 2 },
    { 4, 0x00000008, /* PM3_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 3 },
    { 4, 0x00000010, /* PM4_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 4 },
    { 4, 0x00000020, /* PM5_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 5 },
    { 4, 0x00000040, /* PM6_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 6 },
    { 4, 0x00000080, /* PM7_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 7 },
    { 4, 0x00000100, /* PM8_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 8 },
    { 4, 0x00000200, /* PM9_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 9 },
    { 4, 0x00000400, /* PM10_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 10 },
    { 4, 0x00000800, /* PM11_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 11 },
    { 4, 0x00001000, /* PM12_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 12 },
    { 4, 0x00002000, /* PM13_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 13 },
    { 4, 0x00004000, /* PM14_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 14 },
    { 4, 0x00008000, /* PM15_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 15 },
    { 4, 0x00010000, /* PM16_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 16 },
    { 4, 0x00020000, /* PM17_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 17 },
    { 4, 0x00040000, /* PM18_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 18 },
    { 4, 0x00080000, /* PM19_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 19 },
    { 4, 0x00100000, /* PM20_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 20 },
    { 4, 0x00200000, /* PM21_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 21 },
    { 4, 0x00400000, /* PM22_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 22 },
    { 4, 0x00800000, /* PM23_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 23 },
    { 4, 0x01000000, /* PM24_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 24 },
    { 4, 0x02000000, /* PM25_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 25 },
    { 4, 0x04000000, /* PM26_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 26 },
    { 4, 0x08000000, /* PM27_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 27 },
    { 4, 0x10000000, /* PM28_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 28 },
    { 4, 0x20000000, /* PM29_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 29 },
    { 4, 0x40000000, /* PM30_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 30 },
    { 4, 0x80000000, /* PM31_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_thplus_pm_clp_ser_info, 31 },
    { 5, 0x00000001, /* PM32_INTR */
      SOC_BLK_XLPORT, 1, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_xlp_ser_info, 0 },
    { 0 } /* table terminator */
};

soc_mem_t _soc_thp_skip_populating[] = {
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

    /* Shared MMU_SCFG_PARITY_EN.TDM_PARITY_EN */
    TDM_CALENDAR0m,
    TDM_CALENDAR1m,

    INVALIDm
};

#endif

STATIC int
_soc_tomahawk_mmu_ser_config(int unit, _soc_th_ser_info_t *info_list,
                             int enable)
{
    int info_index, f, rv;
    soc_field_t *fields;
    uint32 rval;
    _soc_th_ser_info_t *info;

    switch(info_list->type) {
    case _SOC_PARITY_TYPE_MMU_SER:
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
            if (info->type == _SOC_PARITY_TYPE_NONE) {
                /* End of table */
                break;
            }
            rv = _soc_tomahawk_mmu_ser_config(unit, info, enable);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Error configuring %s !!\n"),
                         info->mem_str));
                return rv;
            }
        }
        break;
    case _SOC_PARITY_TYPE_MMU_GLB:
    case _SOC_PARITY_TYPE_MMU_XPE:
    case _SOC_PARITY_TYPE_MMU_SC:
#ifdef BCM_TOMAHAWK2_SUPPORT
    case _SOC_PARITY_TYPE_MMU_SED:
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case _SOC_PARITY_TYPE_MMU_ITMCFG:
    case _SOC_PARITY_TYPE_MMU_EBCFG:
#endif
        /* Parity enable */
        if (info_list->enable_reg != INVALIDr) {
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
        }
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

STATIC int
_soc_tomahawk_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                              soc_reg_t group_reg, uint64 *group_rval,
                              _soc_th_ser_info_t *info_list,
                              soc_mem_t mem, int enable)
{
    _soc_th_ser_info_t *info;
    int info_index, rv, rv1;
    uint32 rval;
    uint32 value = 0;
    soc_reg_t reg;
    uint64 rval64;
    soc_field_t *field;
    int i = 0;
    int log_port;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int is_idb = 0;
    int idx = 0;
#endif
    rv = SOC_E_NOT_FOUND;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        rv = SOC_E_NONE;

        if (group_reg != INVALIDr && info->group_reg_enable_field != INVALIDf) {
            /* Enable the info entry in the group register */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit) && (group_reg == CDPORT_INTR_ENABLEr)) {
                value = 0xff;
            } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit) && (group_reg == CLPORT_INTR_ENABLEr)) {
                value = 0xf;
            } else
#endif
            {
                value = 1;
            }
            soc_reg64_field32_set(unit, group_reg, group_rval,
                                  info->group_reg_enable_field, enable ? value : 0);
        }
        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_MMU_SER:
            rv1 = _soc_tomahawk_mmu_ser_config(unit, info, enable);
            if (SOC_FAILURE(rv1)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Error configuring MMU SER !!\n")));
                return rv1;
            }
            break;
        case _SOC_PARITY_TYPE_ECC:
        case _SOC_PARITY_TYPE_PARITY:
            reg = info->enable_reg;
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                (void)_soc_tomahawk3_idb_enable(unit, reg, enable, &is_idb);
                if (is_idb) {
                    continue;
                }
            }
#endif
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                if (info->enable_field == INVALIDf) {
                    if (info->enable_field_list != NULL) {
                        field = info->enable_field_list;
                        i = 0;
                        while(field[i] != INVALIDf) {
                            soc_reg64_field32_set(unit, reg, &rval64, field[i],
                                                  enable ? 1 : 0);
                            i++;
                        }
                    }
                } else {
                    soc_reg64_field32_set(unit, reg, &rval64, info->enable_field,
                                          enable ? 1 : 0);
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                if (info->enable_field == INVALIDf) {
                    if (info->enable_field_list != NULL) {
                        field = info->enable_field_list;
                        i = 0;
                        while(field[i] != INVALIDf) {
                            soc_reg_field_set(unit, reg, &rval, field[i],
                                              enable ? 1 : 0);
                            i++;
                        }
                    }
                } else {
                    soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                      enable ? 1 : 0);
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "SER enable for: %s\n"),
                         (info->mem == INVALIDm) ? info->mem_str :
                         SOC_MEM_NAME(unit, info->mem)));
            break;
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TOMAHAWKPLUS_SUPPORT)
        case _SOC_PARITY_TYPE_CLMAC:
            reg = info->enable_reg;
            /* At this moment, port mapping info is not ready on PCID server */
            if (SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM) {
                break;
            }
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            if (SOC_BLOCK_INFO(unit, block_info_idx).type < 0) {
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
#endif
#if defined (BCM_TOMAHAWK3_SUPPORT)
        case _SOC_PARITY_TYPE_CDMAC:
            reg = info->enable_reg;
            /* At this moment, port mapping info is not ready on PCID server */
            if (SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM) {
                break;
            }
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            if (SOC_BLOCK_INFO(unit, block_info_idx).type < 0) {
                break;
            }
            PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
                for (idx = 0; idx < 2; idx++) {
                    if (SOC_REG_IS_64(unit, reg)) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg_get(unit, reg, log_port, idx, &rval64));
                        if (info->enable_field == INVALIDf) {
                            if (info->enable_field_list != NULL) {
                                field = info->enable_field_list;
                                i = 0;
                                while(field[i] != INVALIDf) {
                                    soc_reg64_field32_set(unit, reg, &rval64, field[i],
                                                          enable ? 1 : 0);
                                    i++;
                                }
                            }
                        } else {
                            soc_reg64_field32_set(unit, reg, &rval64, info->enable_field,
                                                  enable ? 1 : 0);
                        }
                        SOC_IF_ERROR_RETURN
                            (soc_reg_set(unit, reg, log_port, idx, rval64));
                    } else {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, reg, log_port, idx, &rval));
                        if (info->enable_field == INVALIDf) {
                            if (info->enable_field_list != NULL) {
                                field = info->enable_field_list;
                                i = 0;
                                while(field[i] != INVALIDf) {
                                    soc_reg_field_set(unit, reg, &rval, field[i],
                                                      enable ? 1 : 0);
                                    i++;
                                }
                            }
                        } else {
                            soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                              enable ? 1 : 0);
                        }
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, reg, log_port, idx, rval));
                    }
                }
            }

            if (SOC_REG_IS_VALID(unit, info->intr_enable_reg)) {
                reg = info->intr_enable_reg;
                if (SOC_BLOCK_INFO(unit, block_info_idx).type < 0) {
                    break;
                }
                PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
                    for (idx = 0; idx < 2; idx++) {
                        if (SOC_REG_IS_64(unit, reg)) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg_get(unit, reg, log_port, idx, &rval64));
                            if (info->intr_enable_field == INVALIDf) {
                                if (info->intr_enable_field_list != NULL) {
                                    field = info->intr_enable_field_list;
                                    i = 0;
                                    while(field[i] != INVALIDf) {
                                        soc_reg64_field32_set(unit, reg, &rval64, field[i],
                                                              enable ? 1 : 0);
                                        i++;
                                    }
                                }
                            } else {
                                soc_reg64_field32_set(unit, reg, &rval64, info->intr_enable_field,
                                                      enable ? 1 : 0);
                            }
                            SOC_IF_ERROR_RETURN
                                (soc_reg_set(unit, reg, log_port, idx, rval64));
                        } else {
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_get(unit, reg, log_port, idx, &rval));
                            if (info->intr_enable_field == INVALIDf) {
                                if (info->intr_enable_field_list != NULL) {
                                    field = info->intr_enable_field_list;
                                    i = 0;
                                    while(field[i] != INVALIDf) {
                                        soc_reg_field_set(unit, reg, &rval, field[i],
                                                          enable ? 1 : 0);
                                        i++;
                                    }
                                }
                            } else {
                                soc_reg_field_set(unit, reg, &rval, info->intr_enable_field,
                                                  enable ? 1 : 0);
                            }
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg, log_port, idx, rval));
                        }
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "SER enable for: %s\n"),
                         (info->mem == INVALIDm) ? info->mem_str :
                         SOC_MEM_NAME(unit, info->mem)));
            break;
#endif
        case _SOC_PARITY_TYPE_MAC_TX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_TS:
            reg = info->enable_reg;
            /* At this moment, port mapping info is not ready on PCID server */
            if (SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM) {
                break;
            }
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            if (SOC_BLOCK_INFO(unit, block_info_idx).type < 0) {
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

            if (SOC_REG_IS_VALID(unit, info->intr_enable_reg)) {
                reg = info->intr_enable_reg;
                if (SOC_BLOCK_INFO(unit, block_info_idx).type < 0) {
                    break;
                }
                PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
                    if (SOC_REG_IS_64(unit, reg)) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg_get(unit, reg, log_port, 0, &rval64));
                        soc_reg64_field32_set(unit, reg, &rval64,
                                              info->intr_enable_field,
                                              enable ? 1 : 0);
                        SOC_IF_ERROR_RETURN
                            (soc_reg_set(unit, reg, log_port, 0, rval64));
                    } else {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, reg, log_port, 0, &rval));
                        soc_reg_field_set(unit, reg, &rval,
                                          info->intr_enable_field,
                                          enable ? 1 : 0);
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, reg, log_port, 0, rval));
                    }
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


static int
soc_tomahawk_ser_block_enable(int unit, int enable, uint32 *p_cmic_bit,
                              int type, soc_block_t blocktype,
                              soc_reg_t fifo_reset_reg, void *info)
{
    uint8                           rbi;
    uint8                           fld_val = 0;
    uint16                          pcount;
    uint32                          cmic_bit;
    int                             port = REG_PORT_ANY;
    soc_reg_t                       reg, ecc1b_reg;
    soc_field_t                     field, ecc1b_field;
    uint8                           flag = 0;
    uint8                           ecc1b_flag = 0;
    _soc_reg_ser_en_info_t          *reg_info;
    _soc_mem_ser_en_info_t          *mem_info;
    _soc_bus_ser_en_info_t          *bus_info;
    _soc_buffer_ser_en_info_t       *buf_info;
    char                            *str_type;
    char                            *str_name;
    static char *parity_module_str[4] = {"REG", "MEM", "BUS", "BUF"};
    int latency = 0;

    const _soc_th_ser_route_block_t *rb;

    int done = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int is_idb = 0;
#endif

    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

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
                ecc1b_reg = reg_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
                ecc1b_flag = reg_info[pcount].ecc1b_ctrl.flag_reg_mem;
                ecc1b_field = reg_info[pcount].ecc1b_ctrl.en_fld;
                str_name = SOC_REG_NAME(unit, reg_info[pcount].reg);
            }
            break;
        case _SOC_SER_TYPE_MEM:
            if (mem_info[pcount].mem == INVALIDm) {
                done = 1;
            } else if (!SOC_MEM_IS_VALID(unit, mem_info[pcount].mem) ||
                       (SOC_IS_TOMAHAWK(unit) &&
                       ((mem_info[pcount].mem == TRUNK_RR_CNTm) ||
                       (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE0m) ||
                       (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE1m) ||
                       (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE2m) ||
                       (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE3m))) ||
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
                ecc1b_reg = mem_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
                ecc1b_flag = mem_info[pcount].ecc1b_ctrl.flag_reg_mem;
                ecc1b_field = mem_info[pcount].ecc1b_ctrl.en_fld;
                str_name = SOC_MEM_NAME(unit, mem_info[pcount].mem);
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    (void)_soc_tomahawk3_idb_enable(unit, reg, enable, &is_idb);
                    if (is_idb) {
                        continue;
                    }
                }
#endif
                
            }
            break;
        case _SOC_SER_TYPE_BUS:
            if (bus_info[pcount].en_reg == INVALIDr) { 
                done = 1;
            } else {
                reg = bus_info[pcount].en_reg;
                field = bus_info[pcount].en_fld;
                str_name = bus_info[pcount].bus_name;
                ecc1b_reg = bus_info[pcount].ecc1b_en_reg;
                ecc1b_field = bus_info[pcount].ecc1b_en_fld;
            }
            break;
        case _SOC_SER_TYPE_BUF:
            if (buf_info[pcount].en_reg == INVALIDr) {
                done = 1;
            } else {
                reg = buf_info[pcount].en_reg;
                field = buf_info[pcount].en_fld;
                str_name = buf_info[pcount].buffer_name;
                if (SOC_IS_TOMAHAWKPLUS(unit) &&
                    (field == EN_COR_ERR_RPTf) &&
                    (reg == IDB_OBM0_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM1_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM2_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM3_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM4_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM5_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM6_QUEUE_SER_CONTROLr ||
                     reg == IDB_OBM7_QUEUE_SER_CONTROLr)) {
                    /* skip enable of SBE for IDB_OBMn_QUEUE mems */
                    reg = INVALIDr;
                    field = INVALIDf;
                }
                ecc1b_reg = buf_info[pcount].ecc1b_en_reg;
                ecc1b_field = buf_info[pcount].ecc1b_en_fld;
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

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (enable) {
                if (reg == IFP_TCAM_POOL0_BISTCTRLr ||
                    reg == IFP_TCAM_POOL1_BISTCTRLr ||
                    reg == IFP_TCAM_POOL2_BISTCTRLr ||
                    reg == IFP_TCAM_POOL3_BISTCTRLr) {
                    fld_val = 0x7;
                } else if (reg == VFP_CAM_BIST_CONTROLr) {
                    fld_val = 0xff;
                } else {
                    fld_val = 1;
                }
            } else {
                fld_val = 0;
            }
        } else
#endif
        {
            fld_val = enable ? 1 : 0;
        }
        if (SOC_REG_IS_VALID(unit, reg) && (flag == 0)) {
            /* NOTE: Do not use the field modify routine in the following as
                     some regs do not return the correct value due to which the
                     modify routine skips the write */
            if (SOC_REG_IS_64(unit,reg)) {
                soc_th_ser_reg_field_modify(unit, reg, port, field,
                                            fld_val, 0, -1);
            } else {
                soc_th_ser_reg_field32_modify(unit, reg, port, field,
                                              fld_val, 0, -1);
            }
        }
        if (ecc1b_reg != INVALIDr && ecc1b_field != INVALIDf &&
            SOC_REG_IS_VALID(unit, ecc1b_reg) && (ecc1b_flag == 0)) {
            if (SOC_REG_IS_64(unit,reg)) {
                soc_th_ser_reg_field_modify(unit, ecc1b_reg, port, ecc1b_field,
                                            fld_val, 0, -1);
            } else {
                soc_th_ser_reg_field32_modify(unit, ecc1b_reg, port, ecc1b_field,
                                              fld_val, 0, -1);
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "SER enable for %s: %s\n"),
                     str_type, str_name));
    } /* walk thro each entry of block_info */

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            rb = &_soc_th3_ser_route_blocks[rbi];
        } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            rb = &_soc_th2_ser_route_blocks[rbi];
        } else
#endif
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
            if (SOC_IS_TOMAHAWKPLUS(unit)) {
                rb = &_soc_thplus_ser_route_blocks[rbi];
            } else
#endif
        {
            rb = &_soc_th_ser_route_blocks[rbi];
        }

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
             * these cmic reg is always cmic_rval3 - see block_info[] array
             */
            if (rb->enable_reg != INVALIDr &&
                rb->enable_field != INVALIDf) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, rb->enable_reg,
                        REG_PORT_ANY, rb->enable_field, enable ? 1 : 0));
            }
            if ((rb->blocktype == SOC_BLK_IPIPE)
                && (rb->cmic_bit != _SOC_TH_IPIPE0_IP0_INTR)
                && (rb->cmic_bit != _SOC_TH_IPIPE1_IP0_INTR)
                && (rb->cmic_bit != _SOC_TH_IPIPE2_IP0_INTR)
                && (rb->cmic_bit != _SOC_TH_IPIPE3_IP0_INTR)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit) && (rb->cmic_bit != _SOC_TH3_CEV_INTR)) {
                    _soc_th3_ip_pipe_fifo_bmask[unit][rb->pipe] |= cmic_bit;
                } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
                if (SOC_IS_TOMAHAWK2(unit)) {
                    _soc_th2_ip_pipe_fifo_bmask[unit][rb->pipe] |= cmic_bit;
                } else
#endif
                {
                    _soc_th_ip_pipe_fifo_bmask[unit][rb->pipe] |= cmic_bit;
                }
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
soc_tomahawk3_parity_bit_enable(int unit, soc_reg_t enreg, soc_mem_t enmem,
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
                if (enreg == IFP_TCAM_CAM_CONTROLr) {
                    soc_reg_field_set(unit, enreg, &rval, enfld, enable ? 0xf : 0);
                } else {
                    soc_reg_field_set(unit, enreg, &rval, enfld, enable ? 1 : 0);
                }
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

int
soc_tomahawk3_ser_enable_parity_table(int unit, int type, void *info, int enable)
{
    uint32       i = 0;
    uint32      entry[SOC_MAX_MEM_WORDS];
    uint32      copyno;
    uint32      done = 0;
    uint8       flag = 0;
    soc_mem_t   en_mem = INVALIDm;
    soc_mem_t   ecc1b_en_mem = INVALIDm;
    soc_field_t en_fld = INVALIDf;
    soc_field_t ecc1b_en_fld = INVALIDf;
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
                ecc1b_en_mem = reg_info[i].ecc1b_ctrl.ctrl_type.en_mem;
                ecc1b_en_fld = reg_info[i].ecc1b_ctrl.en_fld;
                break;
            case _SOC_SER_TYPE_MEM:
                if (mem_info[i].mem == INVALIDm) {
                    done = 1;
                }
                en_mem = mem_info[i].en_ctrl.ctrl_type.en_mem;
                flag = mem_info[i].en_ctrl.flag_reg_mem;
                en_fld = mem_info[i].en_ctrl.en_fld;
                ecc1b_en_mem = mem_info[i].ecc1b_ctrl.ctrl_type.en_mem;
                ecc1b_en_fld = mem_info[i].ecc1b_ctrl.en_fld;
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
            soc_mem_field32_set(unit, en_mem, entry, en_fld, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, en_mem, copyno, 0, entry));
        }

        if (ecc1b_en_mem != INVALIDm) {
            SOC_MEM_BLOCK_ITER(unit, ecc1b_en_mem, copyno) {
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, ecc1b_en_mem, copyno,
                                                 0, entry));
                soc_mem_field32_set(unit, ecc1b_en_mem, entry,
                                    ecc1b_en_fld, enable ? 1 : 0);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, ecc1b_en_mem, copyno,
                                                  0, entry));
            }
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_ser_enable_parity_table_all(int unit, int enable)
{
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_MEM, (void*) SOC_IP_MEM_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_REG, (void*) SOC_IP_REG_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_MEM, (void*) SOC_EP_MEM_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_REG, (void*) SOC_EP_REG_SER_INFO(unit), enable));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table(unit,
        _SOC_SER_TYPE_MEM, (void*) SOC_MMU_MEM_SER_INFO(unit), enable));

    return SOC_E_NONE;
}


int
soc_tomahawk_ser_enable_all(int unit, int enable)
{
    uint8                           rbi;
    uint32                          cmic_bit;
    uint32                          cmic_rval3, cmic_rval4, cmic_rval5;
    int                             port = REG_PORT_ANY;
    int                             rv, block_info_idx;
    uint64                          rval64;

    const _soc_th_ser_route_block_t *rb;

#ifdef BCM_TOMAHAWK2_SUPPORT
    uint32                          cmic_rval6 = 0;
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32                          cmicx_rval0, cmicx_rval1, cmicx_rval2, cmicx_rval3;
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), &cmicx_rval0));
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                            REG_PORT_ANY, 0), &cmicx_rval1));
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                            REG_PORT_ANY, 0), &cmicx_rval2));
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG3r,
                            REG_PORT_ANY, 0), &cmicx_rval3));
    } else
#endif
    {

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK3r(unit, &cmic_rval3));
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK4r(unit, &cmic_rval4));
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK5r(unit, &cmic_rval5));

#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK6r(unit, &cmic_rval6));
        }
#endif
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        _soc_th2_ip_pipe_fifo_bmask[unit][0] = 0;
        _soc_th2_ip_pipe_fifo_bmask[unit][1] = 0;
        _soc_th2_ip_pipe_fifo_bmask[unit][2] = 0;
        _soc_th2_ip_pipe_fifo_bmask[unit][3] = 0;
    } else
#endif
    {
        _soc_th_ip_pipe_fifo_bmask[unit][0] = 0;
        _soc_th_ip_pipe_fifo_bmask[unit][1] = 0;
        _soc_th_ip_pipe_fifo_bmask[unit][2] = 0;
        _soc_th_ip_pipe_fifo_bmask[unit][3] = 0;
    }

    SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
        _SOC_SER_TYPE_REG, SOC_BLK_IPIPE, INVALIDr,
        (void*) SOC_IP_REG_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
        _SOC_SER_TYPE_MEM, SOC_BLK_IPIPE, ING_SER_FIFO_CTRLr,
        (void*) SOC_IP_MEM_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
        _SOC_SER_TYPE_REG, SOC_BLK_EPIPE, INVALIDr,
        (void*) SOC_EP_REG_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
        _SOC_SER_TYPE_MEM, SOC_BLK_EPIPE, EGR_SER_FIFO_CTRLr,
        (void*) SOC_EP_MEM_SER_INFO(unit)));

    SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
        _SOC_SER_TYPE_MEM, SOC_BLK_MMU_GLB, INVALIDr,
        (void*) SOC_MMU_MEM_SER_INFO(unit)));

    if (SOC_IS_TOMAHAWK(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUS, SOC_BLK_IPIPE, INVALIDr,
            (void*) &_soc_th_ip_bus_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_IPIPE, INVALIDr,
            (void*) &_soc_th_ip_buffer_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUS, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_th_ep_bus_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_th_ep_buffer_ser_info));
    }

#if defined(BCM_TOMAHAWK2_SUPPORT) && defined(BCM_56970_A0)
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUS, SOC_BLK_IPIPE, INVALIDr,
            (void*) &_soc_bcm56970_a0_ip_bus_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_IPIPE, INVALIDr,
            (void*) &_soc_bcm56970_a0_ip_buffer_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUS, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_bcm56970_a0_ep_bus_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_bcm56970_a0_ep_buffer_ser_info));
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_th2_ifp_buffer_ser_info));
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT) && defined(BCM_56980_A0)
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUS, SOC_BLK_IPIPE, INVALIDr,
            (void*) &_soc_bcm56980_a0_ip_bus_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_IPIPE, INVALIDr,
            (void*) &_soc_bcm56980_a0_ip_buffer_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUS, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_bcm56980_a0_ep_bus_ser_info));

        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_bcm56980_a0_ep_buffer_ser_info));
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_block_enable(unit, enable, &cmic_rval3,
            _SOC_SER_TYPE_BUF, SOC_BLK_EPIPE, INVALIDr,
            (void*) &_soc_th3_no_mapping_ser_info));
    }
#endif

    /* Enable 1B error reporting for some special items */

    /* Loop through each place-and-route block entry to enable legacy
       style SER stuff */
    /* BLK_MMU, BLK_CLPORT will be handled here */
    for (rbi = 0; ; rbi++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            rb = &_soc_th3_ser_route_blocks[rbi];
        } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            rb = &_soc_th2_ser_route_blocks[rbi];
        } else
#endif
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
        if (SOC_IS_TOMAHAWKPLUS(unit)) {
            rb = &_soc_thplus_ser_route_blocks[rbi];
        } else
#endif
        {
            rb = &_soc_th_ser_route_blocks[rbi];
        }

        port = REG_PORT_ANY;
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        if (enable) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                switch (rb->cmic_reg) {
                case 0: cmicx_rval0 |= cmic_bit; break;
                case 1: cmicx_rval1 |= cmic_bit; break;
                case 2: cmicx_rval2 |= cmic_bit; break;
                case 3: cmicx_rval3 |= cmic_bit; break;
                default: break;
                }
            } else
#endif
            {
                switch (rb->cmic_reg) {
                case 3: cmic_rval3 |= cmic_bit; break;
                case 4: cmic_rval4 |= cmic_bit; break;
                case 5: cmic_rval5 |= cmic_bit; break;
#ifdef BCM_TOMAHAWK2_SUPPORT
                case 6: cmic_rval6 |= cmic_bit; break;
#endif
                default: break;
                }
            }
        } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                switch (rb->cmic_reg) {
                case 0: cmicx_rval0 &= ~cmic_bit; break;
                case 1: cmicx_rval1 &= ~cmic_bit; break;
                case 2: cmicx_rval2 &= ~cmic_bit; break;
                case 3: cmicx_rval3 &= ~cmic_bit; break;
                default: break;
                }
            }
#endif
        }

        if (rb->info == NULL) {
            continue;
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

        rv = _soc_tomahawk_ser_enable_info(unit, block_info_idx, rb->id, port,
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

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), cmicx_rval0));
        cmicx_rval1 &= (~0xc300);
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                            REG_PORT_ANY, 0), cmicx_rval1));
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                            REG_PORT_ANY, 0), cmicx_rval2));
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG3r,
                            REG_PORT_ANY, 0), cmicx_rval3));
        _soc_th3_tcam_wrapper_enable(unit, enable);
        (void)soc_th3_tcam_engine_enable(unit, enable);
    }
#endif

    if (enable) {
        /* MMU enables */
        /* check trident2.c - may have to add some similar code here */
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            (void)soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
        }
#endif
#ifdef BCM_CMICM_SUPPORT
        /* Write CMIC enable register */
        if (soc_feature(unit, soc_feature_cmicm)) {
            (void)soc_cmicm_intr3_enable(unit, cmic_rval3);
            (void)soc_cmicm_intr4_enable(unit, cmic_rval4);
            (void)soc_cmicm_intr5_enable(unit, cmic_rval5);
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                (void)soc_cmicm_intr6_enable(unit, cmic_rval6);
                if (soc_feature(unit, soc_feature_tcam_scan_engine)) {
                    (void)soc_th2_tcam_engine_enable(unit, TRUE);
                    (void) _soc_th2_tcam_wrapper_enable(unit, TRUE);
                }
            }
#endif
        }
#endif
    } else {
        /* MMU disables */
        /* check trident2.c - may have to add some similar code here */

        /* Write CMIC disable register */
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            (void)soc_cmic_intr_disable(unit, CHIP_INTR_LOW_PRIORITY);
        }
#endif
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            (void)soc_cmicm_intr3_disable(unit, cmic_rval3);
            (void)soc_cmicm_intr4_disable(unit, cmic_rval4);
            (void)soc_cmicm_intr5_disable(unit, cmic_rval5);
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                (void)soc_cmicm_intr6_disable(unit, cmic_rval6);
            }
#endif
        }
#endif
    }
    return SOC_E_NONE;
}

STATIC int soc_tomahawk_pipe_select(int unit, int egress, int pipe);

STATIC void
_soc_th_ser_control_reg_get(int unit,
                            const _soc_mem_ser_en_info_t *ser_en_info_list,
                            soc_mem_t   mem,
                            soc_reg_t   *ser_control_reg,
                            soc_mem_t   *ser_control_mem,
                            soc_field_t *ser_enable_field)
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
            return;
        }
    }
#ifdef BCM_TOMAHAWK2_SUPPORT
    if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CAM) &&
         SOC_IS_TOMAHAWK2(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
        for (i = 0; _soc_th2_tcam_wrapper_table[i].mem != INVALIDm; i++) {
            if (_soc_th2_tcam_wrapper_table[i].mem == mem) {
                *ser_control_reg = _soc_th2_tcam_wrapper_table[i].enable_reg;
                *ser_enable_field = _soc_th2_tcam_wrapper_table[i].enable_field;
                return;  /* found */
            }
        }
    }
#endif
    LOG_ERROR(BSL_LS_SOC_SER,
              (BSL_META_U(unit,
                          "unit %d, mem %s reported in ser_fifo but not "
                          "listed in ip, ep_mem_ser_info list !!\n"),
               unit, SOC_MEM_NAME(unit,mem) ));
    return;
}

STATIC void
_soc_th_mem_parity_info(int unit, int block_info_idx, int pipe,
                        soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | ((pipe & 0xff) << SOC_ERROR_PIPE_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}

STATIC int
_soc_th_mem_has_ecc(int unit, soc_mem_t mem)
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
_soc_th_mem_is_dyn(int unit, soc_mem_t mem)
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

STATIC int
_soc_thx_mem_log_content_skip(int unit, soc_mem_t mem){
    soc_mem_t *mem_list = NULL;
    int i = 0;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        mem_list = _soc_th3_skip_populating;
    } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        mem_list = _soc_th2_skip_populating;
    } else
#endif
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        mem_list = _soc_thp_skip_populating;
    } else
#endif
    {
        mem_list = _soc_th_skip_populating;
    }

    i = 0;
    while(mem_list[i] != INVALIDm) {
        if (mem == mem_list[i]) {
            return TRUE;
        }
        i++;
    }

    return FALSE;
}

STATIC int
_soc_th_populate_ser_log(int unit, soc_reg_t parity_enable_reg,
                         soc_mem_t parity_enable_mem,
                         soc_field_t parity_enable_field,
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
                                 _soc_th_mem_is_dyn(unit, mem) ||
                                 _soc_thx_mem_log_content_skip(unit, mem) ||
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
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (soc_tomahawk3_parity_bit_enable(unit, parity_enable_reg,
                                            parity_enable_mem,
                                            parity_enable_field,
                                            -1, 0) < 0) {
                    return 0;
                }
            } else
#endif
            {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                    REG_PORT_ANY, parity_enable_field, 0) < 0) {
                    return 0;
                }
            }
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
                /*Enable NACK on read */
                flags |= SOC_MEM_DONT_USE_CACHE | SOC_MEM_SCHAN_ERR_RETURN;
                soc_mem_read_extended(unit,
                                      flags,
                                      mem, 0, mem_block,
                                      index, tmp_entry);
            }
        }

        /* Re_enable Parity */
        if (final_disable_parity) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (soc_tomahawk3_parity_bit_enable(unit, parity_enable_reg,
                                            parity_enable_mem,
                                            parity_enable_field,
                                            -1, 1) < 0) {
                    return 0;
                }
            } else
#endif
            {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                    REG_PORT_ANY, parity_enable_field, 1) < 0) {
                    return 0;
                }
            }
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
_soc_add_log_generic(int unit, _soc_ser_correct_info_t *spci)
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
_soc_th_ser_reg32_get(int unit, soc_reg_t reg, int port, int index,
                      uint32 *data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
       return soc_tomahawk_xpe_reg32_get(unit, reg, block_num,
                                         base_type_index, index, data);
    case SOC_BLK_MMU_SC:
       return soc_tomahawk_sc_reg32_get(unit, reg, block_num,
                                        base_type_index, index, data);
#ifdef BCM_TOMAHAWK2_SUPPORT
    case SOC_BLK_MMU_SED:
       return soc_tomahawk2_sed_reg32_get(unit, reg, block_num,
                                          base_type_index, index, data);
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case SOC_BLK_MMU_ITM:
        return soc_tomahawk3_itm_reg32_get(unit, reg, block_num,
                                           base_type_index, index, data);
    case SOC_BLK_MMU_EB:
        return soc_tomahawk3_eb_reg32_get(unit, reg, block_num,
                                          base_type_index, index, data);
#endif
    default:
       return soc_reg32_get(unit, reg, port, index, data);
    }
}

STATIC int
_soc_th_ser_reg32_set(int unit, soc_reg_t reg, int port, int index,
                      uint32 data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
        return soc_tomahawk_xpe_reg32_set(unit, reg, block_num,
                                          base_type_index, index, data);
    case SOC_BLK_MMU_SC:
        return soc_tomahawk_sc_reg32_set(unit, reg, block_num,
                                         base_type_index, index, data);
#ifdef BCM_TOMAHAWK2_SUPPORT
    case SOC_BLK_MMU_SED:
        return soc_tomahawk2_sed_reg32_set(unit, reg, block_num,
                                           base_type_index, index, data);
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case SOC_BLK_MMU_ITM:
        return soc_tomahawk3_itm_reg32_set(unit, reg, block_num,
                                           base_type_index, index, data);
    case SOC_BLK_MMU_EB:
        return soc_tomahawk3_eb_reg32_set(unit, reg, block_num,
                                          base_type_index, index, data);
#endif
    default:
       return soc_reg32_set(unit, reg, port, index, data);
    }
}

int
soc_th_ser_reg_field32_modify(int unit, soc_reg_t reg, soc_port_t port,
                               soc_field_t field, uint32 value,
                               int index, int mmu_base_index)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN
        (_soc_th_ser_reg32_get(unit, reg, port, index, &rval, mmu_base_index));
    soc_reg_field_set(unit, reg, &rval, field, value);
    SOC_IF_ERROR_RETURN
        (_soc_th_ser_reg32_set(unit, reg, port, index, rval, mmu_base_index));
    return SOC_E_NONE;
}

STATIC int
_soc_th_ser_reg_get(int unit, soc_reg_t reg, int port, int index,
                      uint64 *data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
       return soc_tomahawk_xpe_reg_get(unit, reg, block_num,
                                         base_type_index, index, data);
    case SOC_BLK_MMU_SC:
       return soc_tomahawk_sc_reg_get(unit, reg, block_num,
                                        base_type_index, index, data);
#ifdef BCM_TOMAHAWK2_SUPPORT
    case SOC_BLK_MMU_SED:
       return soc_tomahawk2_sed_reg_get(unit, reg, block_num,
                                          base_type_index, index, data);
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case SOC_BLK_MMU_ITM:
        return soc_tomahawk3_itm_reg_get(unit, reg, block_num,
                                           base_type_index, index, data);
    case SOC_BLK_MMU_EB:
        return soc_tomahawk3_eb_reg_get(unit, reg, block_num,
                                          base_type_index, index, data);
#endif
    default:
       return soc_reg_get(unit, reg, port, index, data);
    }
}

STATIC int
_soc_th_ser_reg_set(int unit, soc_reg_t reg, int port, int index,
                      uint64 data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
        return soc_tomahawk_xpe_reg_set(unit, reg, block_num,
                                          base_type_index, index, data);
    case SOC_BLK_MMU_SC:
        return soc_tomahawk_sc_reg_set(unit, reg, block_num,
                                         base_type_index, index, data);
#ifdef BCM_TOMAHAWK2_SUPPORT
    case SOC_BLK_MMU_SED:
        return soc_tomahawk2_sed_reg_set(unit, reg, block_num,
                                           base_type_index, index, data);
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case SOC_BLK_MMU_ITM:
        return soc_tomahawk3_itm_reg_set(unit, reg, block_num,
                                           base_type_index, index, data);
    case SOC_BLK_MMU_EB:
        return soc_tomahawk3_eb_reg_set(unit, reg, block_num,
                                          base_type_index, index, data);
#endif
    default:
       return soc_reg_set(unit, reg, port, index, data);
    }
}

int
soc_th_ser_reg_field_modify(int unit, soc_reg_t reg, soc_port_t port,
                               soc_field_t field, uint32 value,
                               int index, int mmu_base_index)
{
    uint64 rval;

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (_soc_th_ser_reg_get(unit, reg, port, index, &rval, mmu_base_index));
    soc_reg64_field32_set(unit, reg, &rval, field, value);
    SOC_IF_ERROR_RETURN
        (_soc_th_ser_reg_set(unit, reg, port, index, rval, mmu_base_index));
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_mmu_mem_remap(_soc_ser_correct_info_t *spci)
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
    case MMU_THDU_RESUME_PORT0_PIPE2m:
    case MMU_THDU_RESUME_PORT1_PIPE2m:
    case MMU_THDU_RESUME_PORT2_PIPE2m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE2m; break;
    case MMU_THDU_RESUME_PORT0_PIPE3m:
    case MMU_THDU_RESUME_PORT1_PIPE3m:
    case MMU_THDU_RESUME_PORT2_PIPE3m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE3m; break;

    case MMU_THDU_CONFIG_PORT0_PIPE0m:
    case MMU_THDU_CONFIG_PORT1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE0m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE1m:
    case MMU_THDU_CONFIG_PORT1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE1m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE2m:
    case MMU_THDU_CONFIG_PORT1_PIPE2m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE2m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE3m:
    case MMU_THDU_CONFIG_PORT1_PIPE3m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE3m; break;

    case MMU_THDU_CONFIG_QGROUP0_PIPE0m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE0m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE1m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE1m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE2m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE2m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE2m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE3m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE3m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE3m; break;

    case MMU_THDU_OFFSET_QGROUP0_PIPE0m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE0m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE0m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE1m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE1m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE1m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE2m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE2m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE2m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE3m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE3m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE3m; break;

    case MMU_THDU_CONFIG_QUEUE0_PIPE0m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE0m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE1m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE1m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE2m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE2m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE2m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE3m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE3m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE3m; break;

    case MMU_THDU_OFFSET_QUEUE0_PIPE0m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE0m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE0m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE1m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE1m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE1m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE2m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE2m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE2m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE3m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE3m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE3m; break;

    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE0m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE0m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE0m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE0m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE1m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE1m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE1m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE1m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE2m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE2m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE2m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE2m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE3m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE3m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE3m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE3m; break;


    /* THDI */
    case THDI_PORT_SP_CONFIG0_PIPE0m:
    case THDI_PORT_SP_CONFIG1_PIPE0m:
    case THDI_PORT_SP_CONFIG2_PIPE0m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE0m; break;
    case THDI_PORT_SP_CONFIG0_PIPE1m:
    case THDI_PORT_SP_CONFIG1_PIPE1m:
    case THDI_PORT_SP_CONFIG2_PIPE1m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE1m; break;
    case THDI_PORT_SP_CONFIG0_PIPE2m:
    case THDI_PORT_SP_CONFIG1_PIPE2m:
    case THDI_PORT_SP_CONFIG2_PIPE2m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE2m; break;
    case THDI_PORT_SP_CONFIG0_PIPE3m:
    case THDI_PORT_SP_CONFIG1_PIPE3m:
    case THDI_PORT_SP_CONFIG2_PIPE3m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE3m; break;


    /* MTRO */
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE0m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE0m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE1m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE1m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE2m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE2m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE2m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE3m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE3m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE3m; break;


    /* THDM */
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE1m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE2m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE3m; break;

    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE0m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE0m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE0m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE1m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE1m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE1m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE2m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE2m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE2m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE3m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE3m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE3m; break;

    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE1m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE1m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE1m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE2m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE2m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE2m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE3m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE3m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE3m; break;

    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE0m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE0m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE1m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE1m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE1m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE2m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE2m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE2m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE3m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE3m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE3m; break;

    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE0m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE1m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE2m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE2m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE3m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE3m; break;

    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE0m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE0m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE0m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE0m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE1m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE1m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE1m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE1m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE2m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE2m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE2m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE2m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE3m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE3m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE3m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE3m; break;


    /* RQE */
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m:
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m:
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m; break;

    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC1m:
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC1m:
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m; break;

#ifdef BCM_TOMAHAWK2_SUPPORT
    case TCB_THRESHOLD_PROFILE_MAP_A_XPE0m:
    case TCB_THRESHOLD_PROFILE_MAP_B_XPE0m:
        spci->mem = TCB_THRESHOLD_PROFILE_MAP_XPE0m; break;
    case TCB_THRESHOLD_PROFILE_MAP_A_XPE1m:
    case TCB_THRESHOLD_PROFILE_MAP_B_XPE1m:
        spci->mem = TCB_THRESHOLD_PROFILE_MAP_XPE1m; break;
    case TCB_THRESHOLD_PROFILE_MAP_A_XPE2m:
    case TCB_THRESHOLD_PROFILE_MAP_B_XPE2m:
        spci->mem = TCB_THRESHOLD_PROFILE_MAP_XPE2m; break;
    case TCB_THRESHOLD_PROFILE_MAP_A_XPE3m:
    case TCB_THRESHOLD_PROFILE_MAP_B_XPE3m:
        spci->mem = TCB_THRESHOLD_PROFILE_MAP_XPE3m; break;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case MMU_MTRO_CPU_L1_Bm:
    case MMU_MTRO_CPU_L1_Cm:
        spci->mem = MMU_MTRO_CPU_L1_Am; break;
    case MMU_MTRO_L0_Bm:
    case MMU_MTRO_L0_Cm:
        spci->mem = MMU_MTRO_L0_Am; break;
    case MMU_MTRO_L0_B_PIPE0m:
    case MMU_MTRO_L0_C_PIPE0m:
        spci->mem = MMU_MTRO_L0_A_PIPE0m; break;
    case MMU_MTRO_L0_B_PIPE1m:
    case MMU_MTRO_L0_C_PIPE1m:
        spci->mem = MMU_MTRO_L0_A_PIPE1m; break;
    case MMU_MTRO_L0_B_PIPE2m:
    case MMU_MTRO_L0_C_PIPE2m:
        spci->mem = MMU_MTRO_L0_A_PIPE2m; break;
    case MMU_MTRO_L0_B_PIPE3m:
    case MMU_MTRO_L0_C_PIPE3m:
        spci->mem = MMU_MTRO_L0_A_PIPE3m; break;
    case MMU_MTRO_L0_B_PIPE4m:
    case MMU_MTRO_L0_C_PIPE4m:
        spci->mem = MMU_MTRO_L0_A_PIPE4m; break;
    case MMU_MTRO_L0_B_PIPE5m:
    case MMU_MTRO_L0_C_PIPE5m:
        spci->mem = MMU_MTRO_L0_A_PIPE5m; break;
    case MMU_MTRO_L0_B_PIPE6m:
    case MMU_MTRO_L0_C_PIPE6m:
        spci->mem = MMU_MTRO_L0_A_PIPE6m; break;
    case MMU_MTRO_L0_B_PIPE7m:
    case MMU_MTRO_L0_C_PIPE7m:
        spci->mem = MMU_MTRO_L0_A_PIPE7m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIGm; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE0m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE0m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE1m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE1m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE2m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE2m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE3m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE3m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE4m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE4m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE5m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE5m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE6m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE6m; break;
    case MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE7m:
        spci->mem = MMU_THDI_PORT_PG_MIN_CONFIG_PIPE7m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIGm; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE0m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE0m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE1m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE1m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE2m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE2m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE3m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE3m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE4m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE4m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE5m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE5m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE6m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE6m; break;
    case MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE7m:
        spci->mem = MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE7m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIGm; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE0m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE0m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE1m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE1m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE2m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE2m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE3m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE3m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE4m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE4m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE5m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE5m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE6m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE6m; break;
    case MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE7m:
        spci->mem = MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE7m; break;
    case MMU_THDI_PORTSP_CONFIG1m:
        spci->mem = MMU_THDI_PORTSP_CONFIGm; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE0m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE1m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE1m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE2m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE2m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE3m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE3m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE4m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE4m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE5m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE5m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE6m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE6m; break;
    case MMU_THDI_PORTSP_CONFIG1_PIPE7m:
        spci->mem = MMU_THDI_PORTSP_CONFIG_PIPE7m; break;
    case MMU_THDO_Q_TO_QGRP_MAPD1m:
    case MMU_THDO_Q_TO_QGRP_MAPD2m:
        spci->mem = MMU_THDO_Q_TO_QGRP_MAPD0m; break;
    case MMU_THDO_QUEUE_CONFIG1m:
        spci->mem = MMU_THDO_QUEUE_CONFIGm; break;
    case MMU_THDO_QUEUE_RESUME_OFFSET1m:
        spci->mem = MMU_THDO_QUEUE_RESUME_OFFSETm; break;
    case MMU_THDO_CONFIG_UC_QGROUP1m:
    case MMU_THDO_CONFIG_UC_QGROUP2m:
        spci->mem = MMU_THDO_CONFIG_UC_QGROUP0m; break;
    case MMU_THDO_CONFIG_PORT_UC1m:
    case MMU_THDO_CONFIG_PORT_UC2m:
        spci->mem = MMU_THDO_CONFIG_PORT_UC0m; break;
    case MMU_THDO_RESUME_PORT_UC1m:
    case MMU_THDO_RESUME_PORT_UC2m:
        spci->mem = MMU_THDO_RESUME_PORT_UC0m; break;
    case MMU_THDO_RESUME_PORT_MC1m:
    case MMU_THDO_RESUME_PORT_MC2m:
        spci->mem = MMU_THDO_RESUME_PORT_MC0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_0_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_0_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_0_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_0_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_0_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_0_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_1_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_1_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_1_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_1_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_1_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_1_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_2_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_2_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_2_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_2_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_2_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_2_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_3_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_3_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_3_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_3_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_3_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_3_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_4_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_4_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_4_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_4_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_4_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_4_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_5_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_5_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_5_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_5_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_5_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_5_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_6_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_6_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_6_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_6_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_6_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_6_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_7_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_7_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_7_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_7_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_7_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_7_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_8_1m:
    case MMU_WRED_DROP_CURVE_PROFILE_8_2m:
    case MMU_WRED_DROP_CURVE_PROFILE_8_3m:
    case MMU_WRED_DROP_CURVE_PROFILE_8_4m:
    case MMU_WRED_DROP_CURVE_PROFILE_8_5m:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_8_0m; break;
    case MMU_REPL_MEMBER_ICC_SC0m:
    case MMU_REPL_MEMBER_ICC_SC1m:
    case MMU_REPL_MEMBER_ICC_SC2m:
    case MMU_REPL_MEMBER_ICC_SC3m:
        spci->mem = MMU_REPL_MEMBER_ICCm; break;
    case MMU_REPL_GROUP_INFO_TBL0m:
    case MMU_REPL_GROUP_INFO_TBL1m:
    case MMU_REPL_GROUP_INFO_TBL2m:
    case MMU_REPL_GROUP_INFO_TBL3m:
        spci->mem = MMU_REPL_GROUP_INFO_TBLm; break;
    case MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDO1m:
        spci->mem = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDO0m; break;
#endif

    default:
        break;
    }
    return SOC_E_NONE;
}

/* correction routine for MMU_REPL_GROUP_INITIAL_COPY_COUNT mem */
STATIC int
_soc_mmu_repl_group_correction(int mmu_base_index,
                               _soc_ser_correct_info_t *spci, uint32 *addr)
{
    if ((spci == NULL) || (addr == NULL)) {
        return SOC_E_PARAM;
    }
    if (spci->mem == MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m &&
        ((mmu_base_index == _SOC_MMU_BASE_INDEX_XPE1) ||
         (mmu_base_index == _SOC_MMU_BASE_INDEX_XPE3))) {
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC1m;
        *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_TH_MMU_ADDR_SEG1;
        spci->addr = *addr;
    }
    if (spci->mem == MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m &&
        ((mmu_base_index == _SOC_MMU_BASE_INDEX_XPE1) ||
         (mmu_base_index == _SOC_MMU_BASE_INDEX_XPE3))) {
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC1m;
        *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_TH_MMU_ADDR_SEG1;
        spci->addr = *addr;
    }
    return SOC_E_NONE;
}

/* correction routine for MMU_WRED_CONFIG mem */
STATIC int
_soc_mmu_wred_correction(int unit, _soc_ser_correct_info_t *spci,
                         uint32 *addr, int *second_pass)
{
    if ((spci == NULL) || (addr == NULL) || (second_pass == NULL)) {
        return SOC_E_PARAM;
    }
    switch (spci->mem) {
    case MMU_WRED_CONFIG_XPE0_PIPE0m:
        spci->mem = MMU_WRED_CONFIG_XPE0_PIPE1m;
        spci->mem_reported = MMU_WRED_CONFIG_XPE0_PIPE1m;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            *addr &= _SOC_TH2_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH2_MMU_ADDR_SEG1;
        } else
#endif
        {
            *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH_MMU_ADDR_SEG1;
        }

        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_CONFIG_XPE2_PIPE0m:
        spci->mem = MMU_WRED_CONFIG_XPE2_PIPE1m;
        spci->mem_reported = MMU_WRED_CONFIG_XPE2_PIPE1m;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            *addr &= _SOC_TH2_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH2_MMU_ADDR_SEG1;
        } else
#endif
        {
            *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH_MMU_ADDR_SEG1;
        }

        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_CONFIG_XPE1_PIPE2m:
        spci->mem = MMU_WRED_CONFIG_XPE1_PIPE3m;
        spci->mem_reported = MMU_WRED_CONFIG_XPE1_PIPE3m;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            *addr &= _SOC_TH2_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH2_MMU_ADDR_SEG3;
        } else
#endif
        {
            *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH_MMU_ADDR_SEG3;
        }

        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_CONFIG_XPE3_PIPE2m:
        spci->mem = MMU_WRED_CONFIG_XPE3_PIPE3m;
        spci->mem_reported = MMU_WRED_CONFIG_XPE3_PIPE3m;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            *addr &= _SOC_TH2_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH2_MMU_ADDR_SEG3;
        } else
#endif
        {
            *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
            *addr |= _SOC_TH_MMU_ADDR_SEG3;
        }

        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE0m:
        spci->mem = MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE1m;
        spci->mem_reported = MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE1m;
        *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_TH_MMU_ADDR_SEG1;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE2m:
        spci->mem = MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE3m;
        spci->mem_reported = MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE3m;
        *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_TH_MMU_ADDR_SEG3;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE0m:
        spci->mem = MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE1m;
        spci->mem_reported = MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE1m;
        *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_TH_MMU_ADDR_SEG1;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE2m:
        spci->mem = MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE3m;
        spci->mem_reported = MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE3m;
        *addr &= _SOC_TH_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_TH_MMU_ADDR_SEG3;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    default:
        *second_pass = 0;
        break;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_process_mmu_err(int unit, int block_info_idx,
                                  const _soc_th_ser_info_t *info_list,
                                  char *prefix_str, int mmu_base_index)
{
    int rv, type = 0, multi = 0;
    uint32 addr = 0, rval, rval_fifo_status, rval_intr_status_reg;
    uint32 non_ser_intr_status_mask;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_reg_t fifo_status_reg, mem_fail_ctr_reg;
    soc_reg_t parity_enable_reg = INVALIDr;
    soc_field_t parity_enable_field = INVALIDf;
    soc_field_t *fields_stat;
    soc_field_t intr_stat_clr = INVALIDf;
    soc_mem_t ser_fifo_mem;
    soc_mem_t parity_enable_mem = INVALIDm;
    soc_block_t blocktype = SOC_BLK_NONE;
    uint8 blk_idx;
    uint32 sblk = 0;
    _soc_ser_correct_info_t spci;
    int second_pass = 0;
    int i;
    int f, parity_error = 0;
    static int acc_type[] = {_SOC_ACC_TYPE_PIPE_ANY, _SOC_ACC_TYPE_PIPE_ANY};
    _soc_th_ser_mmu_intr_info_t *p_ser_mmu_intr_info;
    soc_stat_t *stat = SOC_STAT(unit);

#ifdef BCM_TOMAHAWK3_SUPPORT
     if (SOC_IS_TOMAHAWK3(unit)) {
         p_ser_mmu_intr_info = (_soc_th_ser_mmu_intr_info_t *) &th3_mmu_intr_info;
         intr_stat_clr = MEM_PAR_ERR_INT_STATf;
     } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
     if (SOC_IS_TOMAHAWK2(unit)) {
        p_ser_mmu_intr_info = (_soc_th_ser_mmu_intr_info_t *) &th2_mmu_intr_info;
        intr_stat_clr = MEM_PAR_ERR_CLRf;
     } else
#endif
    {
        p_ser_mmu_intr_info = (_soc_th_ser_mmu_intr_info_t *) &mmu_intr_info;
        intr_stat_clr = MEM_PAR_ERR_CLRf;
    }

    switch(info_list->type) {
    case _SOC_PARITY_TYPE_MMU_SER: {
        /* When we are here, we have been called by _soc_tomahawk_process_ser.
         * info_list points to 1st entry in _soc_th_mmu_top_ser_info[].
         * Goal is to find src_sub_block in mmu: MMU_GLB, XPE0,1,2,3, SC0,1 */
        _soc_th_ser_info_t *info = NULL;

        /* find src of interrupt in mmu */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_status_reg, REG_PORT_ANY, 0,
                           &rval)); /* status_reg has BT,AT = CHIP,SINGLE */
        for (i = 0; p_ser_mmu_intr_info[i].int_statf != INVALIDf; i++) {
            if (soc_reg_field_get(unit, info_list->intr_status_reg, rval,
                                  p_ser_mmu_intr_info[i].int_statf)) {
#define MMU_INTR_SRC(i)\
                ((i) == 0? "MMU_GLB" : \
                 (i) == 1? "MMU_XPE0" : \
                 (i) == 2? "MMU_XPE1" : \
                 (i) == 3? "MMU_XPE2" : \
                 (i) == 4? "MMU_XPE3" : \
                 (i) == 5? "MMU_SC0" : \
                 (i) == 6? "MMU_SC1" : \
                 (i) == 7? "MMU_SED0" : \
                 (i) == 8? "MMU_SED1" : \
                 "MMU_UNNOWN")
#ifdef BCM_TOMAHAWK3_SUPPORT
#define MMU_TH3_INTR_SRC(i)\
                ((i) == 0? "MMU_GLB" : \
                 (i) == 1? "MMU_ITM0" : \
                 (i) == 2? "MMU_ITM1" : \
                 (i) == 3? "MMU_EB0" : \
                 (i) == 4? "MMU_EB1" : \
                 (i) == 5? "MMU_EB2" : \
                 (i) == 6? "MMU_EB3" : \
                 (i) == 7? "MMU_EB4" : \
                 (i) == 8? "MMU_EB5" : \
                 (i) == 9? "MMU_EB6" : \
                 (i) == 10? "MMU_EB7" : \
                 "MMU_UNNOWN")

                if (SOC_IS_TOMAHAWK3(unit)) {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "intr_status_reg = 0x%08x, "
                                    "int_statf = %d is set, mmu_intr_info_idx "
                                    "= %d (%s) !!\n"),
                         rval, p_ser_mmu_intr_info[i].int_statf, i, MMU_TH3_INTR_SRC(i)));
                } else
#endif
                {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "intr_status_reg = 0x%08x, "
                                    "int_statf = %d is set, mmu_intr_info_idx "
                                    "= %d (%s) !!\n"),
                         rval, p_ser_mmu_intr_info[i].int_statf, i, MMU_INTR_SRC(i)));
                }
                info = &(info_list->info[p_ser_mmu_intr_info[i].ser_info_index]);
                    /* 'info' now points to entry info_index[i]
                     * in _soc_th_mmu_ser_info[] */
                rv = _soc_tomahawk_ser_process_mmu_err(unit, block_info_idx,
                                                       info, prefix_str,
                                                       p_ser_mmu_intr_info[i].mmu_base_index);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "process_mmu_err: Error processing %s !!\n"),
                             info->mem_str));
                    return rv;
                }
            }
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, info_list->intr_status_reg,
                                              REG_PORT_ANY, 0, rval));
        }
#endif
        if (info == NULL) {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                "In MMU SER processing with no error bit set "
                                "0x%08x !!\n"), rval));
            return SOC_E_NONE;
        }
    }   break;
    case _SOC_PARITY_TYPE_MMU_GLB:
    case _SOC_PARITY_TYPE_MMU_XPE:
#ifdef BCM_TOMAHAWK2_SUPPORT
    case _SOC_PARITY_TYPE_MMU_SED:
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case _SOC_PARITY_TYPE_MMU_ITMCFG:
    case _SOC_PARITY_TYPE_MMU_EBCFG:
#endif
    case _SOC_PARITY_TYPE_MMU_SC: {
        /* we are here when we were recursively called by process_mmu_ser itself
         * and info_list is pointing to one of the entries in
         * _soc_th_mmu_ser_info[]
         */
        switch(info_list->type) {
        case _SOC_PARITY_TYPE_MMU_GLB: /* all of these regs,mem have BT,AT = CHIP,SINGLE */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                fifo_status_reg = MMU_GLBCFG_MEM_SER_FIFO_STSr;
                ser_fifo_mem = MMU_GLBCFG_MEM_FAIL_ADDR_64m;
                mem_fail_ctr_reg = MMU_GLBCFG_MEM_FAIL_INT_CTRr;
                blocktype = SOC_BLK_MMU_GLB;
                acc_type[0] = _SOC_ACC_TYPE_PIPE_ANY;
                non_ser_intr_status_mask = ~0x1;
            } else
#endif
            {
                fifo_status_reg = MMU_GCFG_MEM_SER_FIFO_STSr;
                ser_fifo_mem = MMU_GCFG_MEM_FAIL_ADDR_64m;
                mem_fail_ctr_reg = MMU_GCFG_MEM_FAIL_INT_CTRr;
                blocktype = SOC_BLK_MMU_GLB;
                acc_type[0] = _SOC_ACC_TYPE_PIPE_ANY;
#ifdef BCM_TOMAHAWK2_SUPPORT
                if (SOC_IS_TOMAHAWK2(unit)) {
                    non_ser_intr_status_mask = ~0x1;
                } else
#endif
                {
                    non_ser_intr_status_mask = 0x0;
                }
            }
                /* MMU_GCFG_GLB_CPU_INT_STAT has only MEM_PAR_ERR_STATf */
            break;
        case _SOC_PARITY_TYPE_MMU_XPE: /* all of these regs,mem have BT,AT = XPE,SINGLE */
            fifo_status_reg = MMU_XCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_XPE_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_XCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_XPE;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x1;
                /* bit 0 in MMU_XCFG_XPE_CPU_INT_STAT is MEM_PAR_ERR_STATf */
            break;
        case _SOC_PARITY_TYPE_MMU_SC: /* all of these regs,mem have BT,AT = SLICE,SINGLE */
            fifo_status_reg = MMU_SCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_SC_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_SCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_SC;
            acc_type[0] = mmu_base_index;
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                non_ser_intr_status_mask = ~0x1;
            } else
#endif
            {
                non_ser_intr_status_mask = ~0x4;
            }
                /* bit 2 in MMU_SCFG_SC_CPU_INT_STAT is MEM_PAR_ERR_STATf */
            break;
#ifdef BCM_TOMAHAWK2_SUPPORT
        case _SOC_PARITY_TYPE_MMU_SED: /* all of these regs,mem have BT,AT = SLICE,SINGLE */
            fifo_status_reg = MMU_SEDCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_SED_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_SEDCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_SED;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x4;
            /* bit 2 in MMU_SEDCFG_SED_CPU_INT_STAT is MEM_PAR_ERR_STATf */
            break;
#endif     
#ifdef BCM_TOMAHAWK3_SUPPORT
        case _SOC_PARITY_TYPE_MMU_ITMCFG:
            fifo_status_reg = MMU_ITMCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_ITM_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_ITMCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_ITM;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x1;
            break;
        case _SOC_PARITY_TYPE_MMU_EBCFG:
            fifo_status_reg = MMU_EBCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_EB_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_EBCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_EB;
            acc_type[0] = mmu_base_index;
            non_ser_intr_status_mask = ~0x1;
            break;
#endif
        default:
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN
            (_soc_th_ser_reg32_get(unit, info_list->intr_status_reg,
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
        } else
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (info_list->intr_status_field == MEM_PAR_ERR_INT_STATf) {
            if (soc_reg_field_get(unit, info_list->intr_status_reg,
                                   rval_intr_status_reg, info_list->intr_status_field)) {
                parity_error = 1;
            }
        } else
#endif
        if (info_list->intr_status_field == MEM_PAR_ERR_STATf) {
            if (soc_reg_field_get(unit, info_list->intr_status_reg,
                                   rval_intr_status_reg, info_list->intr_status_field)) {
                parity_error = 1;
            }
        }

        if (parity_error == 1) {
            /* Now we know that one of the reasons for interrupt from MMU is
             * parity error for sure, so mmu_ser_fifo has to be non-empty */
            SOC_IF_ERROR_RETURN
                (_soc_th_ser_reg32_get(unit, fifo_status_reg, REG_PORT_ANY, 0,
                                       &rval_fifo_status, mmu_base_index));
            if (soc_reg_field_get(unit, fifo_status_reg, rval_fifo_status, EMPTYf)) {
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "unit %d %s SER interrupt with empty fifo !!\n"),
                           unit, info_list->mem_str));
                SOC_IF_ERROR_RETURN
                    (soc_th_ser_reg_field32_modify(unit,
                                                    info_list->intr_clr_reg,
                                                    REG_PORT_ANY,
                                                    intr_stat_clr,
                                                    1, 0, mmu_base_index));
                return SOC_E_NONE;
            }
            SOC_IF_ERROR_RETURN
                (_soc_th_ser_reg32_get(unit, mem_fail_ctr_reg, REG_PORT_ANY, 0,
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
#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (SOC_IS_TOMAHAWK3(unit)) {
                            (void)soc_th3_mmu_mem_blk_remap(spci.mem, &spci.sblk);
                        }
#endif
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
                        SOC_IF_ERROR_RETURN
                            (_soc_mmu_repl_group_correction(mmu_base_index,
                                                            &spci, &addr));
                        spci.index = addr - soc_mem_base(unit, spci.mem);
                        spci.flags |= SOC_SER_REG_MEM_KNOWN;
                        spci.flags |= SOC_SER_LOG_WRITE_CACHE;
                            /* ser_correction will fill cache part in ser_log */
                        spci.mem_reported = spci.mem; /* before remap */
                        spci.flags |= SOC_SER_LOG_MEM_REPORTED;
                        SOC_IF_ERROR_RETURN
                            (_soc_tomahawk_ser_mmu_mem_remap(&spci));
                        spci.parity_type = _soc_th_mem_has_ecc(unit, spci.mem)?
                                           SOC_PARITY_TYPE_ECC :
                                           SOC_PARITY_TYPE_PARITY;
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "unit %d mem for ser_correction ="
                                        " %s \n"),
                             unit, SOC_MEM_NAME(unit,spci.mem)));

                        _soc_th_ser_control_reg_get(unit,
                                                    (const _soc_mem_ser_en_info_t *)SOC_MMU_MEM_SER_INFO(unit),
                                                    spci.mem, /* after remap */
                                                    &parity_enable_reg,
                                                    &parity_enable_mem,
                                                    &parity_enable_field);
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

                spci.log_id = _soc_th_populate_ser_log(unit,
                                                       parity_enable_reg,
                                                       parity_enable_mem,
                                                       parity_enable_field,
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
                        (info_list->type == _SOC_PARITY_TYPE_MMU_XPE)) {

                        spci.flags |= SOC_SER_ALSO_UPDATE_SW_COUNTER;
                                            /* xpe0, 1, 2, 3 */
                        spci.pipe_num = (spci.addr >> 15) & 0x3;
                                            /* bits 16:15 */
                        switch (spci.mem) {
                        case MMU_CTR_COLOR_DROP_MEM_XPE0m:
                        case MMU_CTR_COLOR_DROP_MEM_XPE1m:
                        case MMU_CTR_COLOR_DROP_MEM_XPE2m:
                        case MMU_CTR_COLOR_DROP_MEM_XPE3m:
                            spci.counter_base_mem = MMU_CTR_COLOR_DROP_MEMm;
                            spci.pipe_num = -1; /* Note */
                            break;

                        case MMU_CTR_ING_DROP_MEM_XPE0_PIPE0m:
                        case MMU_CTR_ING_DROP_MEM_XPE1_PIPE0m:
                            spci.counter_base_mem = MMU_CTR_ING_DROP_MEMm;
                            spci.pipe_num = 0;
                            break;
                        case MMU_CTR_ING_DROP_MEM_XPE2_PIPE1m:
                        case MMU_CTR_ING_DROP_MEM_XPE3_PIPE1m:
                            spci.counter_base_mem = MMU_CTR_ING_DROP_MEMm;
                            spci.pipe_num = 1;
                            break;
                        case MMU_CTR_ING_DROP_MEM_XPE2_PIPE2m:
                        case MMU_CTR_ING_DROP_MEM_XPE3_PIPE2m:
                            spci.counter_base_mem = MMU_CTR_ING_DROP_MEMm;
                            spci.pipe_num = 2;
                            break;
                        case MMU_CTR_ING_DROP_MEM_XPE0_PIPE3m:
                        case MMU_CTR_ING_DROP_MEM_XPE1_PIPE3m:
                            spci.counter_base_mem = MMU_CTR_ING_DROP_MEMm;
                            spci.pipe_num = 3;
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
                        (_soc_th_ser_reg32_get(unit, fifo_status_reg,
                                               REG_PORT_ANY, 0,
                                               &rval_fifo_status,
                                               mmu_base_index));
                    second_pass = 0;
                        /* start next fifo entry with second_pass=0 */
                } else { /* in first_pass */
                    SOC_IF_ERROR_RETURN
                        (_soc_mmu_wred_correction(unit, &spci, &addr,
                                                  &second_pass));
                    if (!second_pass) {
                        /* second_pass not needed */
                        SOC_IF_ERROR_RETURN
                            (_soc_th_ser_reg32_get(unit, fifo_status_reg,
                                                   REG_PORT_ANY, 0,
                                                   &rval_fifo_status,
                                                   mmu_base_index));
                    }
                    /* else, second_pass is needed, don't read next fifo entry
                     * yet */
                }
            } while (!soc_reg_field_get(unit, fifo_status_reg, rval_fifo_status, EMPTYf));
            SOC_IF_ERROR_RETURN
                (soc_th_ser_reg_field32_modify(unit, info_list->intr_clr_reg,
                                                REG_PORT_ANY,
                                                intr_stat_clr, 1,
                                                0, mmu_base_index));
        }

        /* non_ser mmu interrupts in same status reg are asserted */
        if (rval_intr_status_reg & non_ser_intr_status_mask) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                rv = soc_th3_mmu_non_ser_intr_handler(unit,
                                                      blocktype,
                                                        /* can be SOC_BLK_MMU_ITM,
                                                         * SOC_BLK_MMU_EB */
                                                      mmu_base_index,
                                                        /* specifies xpe0,1,2,3
                                                         * in case of SOC_BLK_MMU_XPE */
                                                        /* specifies sc0,1
                                                         * in case of SOC_BLK_MMU_SC,
                                                         * SOC_BLK_MMU_SED */
                                                      rval_intr_status_reg &
                                                      non_ser_intr_status_mask);
                                                        /* value of intr_status_reg */
            } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                rv = soc_th2_mmu_non_ser_intr_handler(unit,
                                                      blocktype,
                                                        /* can be SOC_BLK_MMU_XPE,
                                                         * SOC_BLK_MMU_SC,
                                                         * SOC_BLK_MMU_SED */
                                                      mmu_base_index,
                                                        /* specifies xpe0,1,2,3
                                                         * in case of SOC_BLK_MMU_XPE */
                                                        /* specifies sc0,1
                                                         * in case of SOC_BLK_MMU_SC,
                                                         * SOC_BLK_MMU_SED */
                                                      rval_intr_status_reg &
                                                      non_ser_intr_status_mask);
                                                        /* value of intr_status_reg */
            } else
#endif
            {
                rv = soc_th_mmu_non_ser_intr_handler(unit,
                                                     blocktype,
                                                        /* can be SOC_BLK_MMU_XPE, SOC_BLK_MMU_SC */
                                                     mmu_base_index,
                                                        /* specifies xpe0,1,2,3 in case of SOC_BLK_MMU_XPE */
                                                        /* specifies sc0,1 in case of SOC_BLK_MMU_SC */
                                                     rval_intr_status_reg &
                                                     non_ser_intr_status_mask);
                                                        /* value of intr_status_reg */
            }
        }
    }   break;
    default:
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_th_idb_counters_clear(int unit, int pipe, soc_reg_t obm_status_reg)
{
    int i, j, obm_queue_num;
    uint64 tmp64;
    static const soc_reg_t idb_obm_reg_list[] = {
        IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM0_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM0_MAX_USAGEr,

        IDB_OBM1_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM1_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM1_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM1_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM1_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM1_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM1_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM1_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM1_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM1_MAX_USAGEr,

        IDB_OBM2_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM2_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM2_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM2_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM2_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM2_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM2_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM2_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM2_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM2_MAX_USAGEr,

        IDB_OBM3_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM3_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM3_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM3_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM3_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM3_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM3_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM3_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM3_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM3_MAX_USAGEr,

        IDB_OBM4_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM4_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM4_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM4_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM4_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM4_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM4_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM4_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM4_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM4_MAX_USAGEr,

        IDB_OBM5_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM5_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM5_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM5_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM5_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM5_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM5_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM5_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM5_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM5_MAX_USAGEr,

        IDB_OBM6_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM6_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM6_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM6_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM6_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM6_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM6_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM6_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM6_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM6_MAX_USAGEr,

        IDB_OBM7_LOSSY_LO_PKT_DROP_COUNTr,
        IDB_OBM7_LOSSY_HI_PKT_DROP_COUNTr,
        IDB_OBM7_LOSSLESS0_PKT_DROP_COUNTr,
        IDB_OBM7_LOSSLESS1_PKT_DROP_COUNTr,
        IDB_OBM7_LOSSY_LO_BYTE_DROP_COUNTr,
        IDB_OBM7_LOSSY_HI_BYTE_DROP_COUNTr,
        IDB_OBM7_LOSSLESS0_BYTE_DROP_COUNTr,
        IDB_OBM7_LOSSLESS1_BYTE_DROP_COUNTr,
        IDB_OBM7_FLOW_CONTROL_EVENT_COUNTr,
        IDB_OBM7_MAX_USAGEr
    };

#define _SOC_IDB_OBM_NUM_DISABLE_REGS 10
#define _SOC_IDB_BM_NUM_GEN_CLEAR_REGS 8
    switch (obm_status_reg) {
    case IDB_OBM0_QUEUE_ECC_STATUSr:
        obm_queue_num = 0;
        break;
    case IDB_OBM1_QUEUE_ECC_STATUSr:
        obm_queue_num = 1;
        break;
    case IDB_OBM2_QUEUE_ECC_STATUSr:
        obm_queue_num = 2;
        break;
    case IDB_OBM3_QUEUE_ECC_STATUSr:
        obm_queue_num = 3;
        break;
    case IDB_OBM4_QUEUE_ECC_STATUSr:
        obm_queue_num = 4;
        break;
    case IDB_OBM5_QUEUE_ECC_STATUSr:
        obm_queue_num = 5;
        break;
    case IDB_OBM6_QUEUE_ECC_STATUSr:
        obm_queue_num = 6;
        break;
    case IDB_OBM7_QUEUE_ECC_STATUSr:
        obm_queue_num = 7;
        break;
    default:
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(tmp64);
    for (i = 0; i < _SOC_IDB_OBM_NUM_DISABLE_REGS; i++) {
        soc_reg_t reg =
            idb_obm_reg_list[(obm_queue_num*_SOC_IDB_OBM_NUM_DISABLE_REGS) + i];
        soc_reg_t base_reg = reg;
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
           reg = SOC_REG_UNIQUE_ACC(unit, reg)[pipe];
        }
        for (j = 0; j < SOC_REG_NUMELS(unit, reg); j++) {
            /* For counts collected by sw, clear both hw, sw counts */
            if (i < _SOC_IDB_BM_NUM_GEN_CLEAR_REGS) {
                SOC_IF_ERROR_RETURN(
                    soc_ser_update_counter(unit,
                                              1,            /* is_reg */
                                              reg,          /* restore_reg */
                                              INVALIDm,     /* restore_mem */
                                              j,            /* index */
                                              REG_PORT_ANY, /* port */
                                              base_reg,     /* base_reg */
                                              INVALIDm,     /* base_mem */
                                              obm_queue_num,/* inst_num */
                                              pipe,         /* pipe_num */
                                              0));          /* restore_last */
                LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "Cleared sw, hw reg %s, %s, pipe=%0d, numels=%0d\n\n\n"),
                     SOC_REG_NAME(unit, reg),
                     SOC_REG_IS_64(unit, reg)? "64b" : "32b",
                     pipe, j));
                continue;
            }

            /* For counts not collected by sw, clear only hw count */
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN(
                    soc_reg_set(unit, reg, REG_PORT_ANY, j, tmp64));
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Cleared reg %s, 64b, pipe=%0d, numels=%0d\n"),
                     SOC_REG_NAME(unit, reg), pipe, j));
            } else {
                SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg, REG_PORT_ANY, j, 0));
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Cleared reg %s, 32b, pipe=%0d, numels=%0d\n"),
                     SOC_REG_NAME(unit, reg), pipe, j));
            }
        }
    }
    return SOC_E_NONE;
}

#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TOMAHAWKPLUS_SUPPORT)
STATIC int
_soc_tomahawk_ser_process_clmac(int unit, int block_info_idx,
                              const _soc_th_ser_info_t *info,
                              char *prefix_str, char *mem_str, soc_block_t blocktype)
{
    uint64 rval64;
    uint32 has_error = FALSE, rval = 0;
    int log_port;
    int single_bit = 0, double_bit = 0;
    soc_stat_t *stat = SOC_STAT(unit);

    COMPILER_64_ZERO(rval64);
    if ((info->intr_status_reg == INVALIDr) || (INVALIDf == info->intr_status_field)) {
        return SOC_E_NONE;
    }

    PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
        single_bit = 0;
        double_bit = 0;
        if (SOC_REG_IS_64(unit, info->intr_status_reg)) {
            SOC_IF_ERROR_RETURN
                     (soc_reg_get(unit, info->intr_status_reg, log_port, 0, &rval64));
            if (soc_reg64_field32_get(unit, info->intr_status_reg,
                                          rval64, info->intr_status_field)) {
                has_error = TRUE;
                if (info->intr_status_field == SUM_TX_CDC_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_TS_MEM_SINGLE_BIT_ERRf) {
                    single_bit = 1;
                }
                if (info->intr_status_field == SUM_TX_CDC_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_TS_MEM_DOUBLE_BIT_ERRf) {
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
                if (info->intr_status_field == SUM_TX_CDC_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_SINGLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_TS_MEM_SINGLE_BIT_ERRf) {
                    single_bit = 1;
                }
                if (info->intr_status_field == SUM_TX_CDC_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_CDC_DOUBLE_BIT_ERRf ||
                    info->intr_status_field == SUM_RX_TS_MEM_DOUBLE_BIT_ERRf) {
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
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
STATIC int
_soc_tomahawk_ser_process_cdmac(int unit, int block_info_idx,
                              const _soc_th_ser_info_t *info,
                              char *prefix_str, char *mem_str,
                              soc_block_t blocktype)
{
    uint32 has_error = FALSE, rval = 0;
    int log_port;
    int i = 0;
    int single_bit = 0, double_bit = 0;
    /* Mask is based on the bits in intr_status_reg */
    uint32 single_bit_mask = 0x5, double_bit_mask = 0x80a;
    soc_stat_t *stat = SOC_STAT(unit);

    if ((info->intr_status_reg == INVALIDr) ||
        ((INVALIDf == info->intr_status_field) &&
         (NULL == info->intr_status_field_list))) {
        return SOC_E_NONE;
    }

    PBMP_ITER(SOC_BLOCK_BITMAP(unit, block_info_idx), log_port) {
        single_bit = 0;
        double_bit = 0;
        for(i = 0; i < 2; i++) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, info->intr_status_reg, log_port, i, &rval));
            if (rval != 0) {
                has_error = TRUE;
                if (rval & single_bit_mask) {
                    single_bit = 1;
                }
                if (rval & double_bit_mask) {
                    double_bit = 1;
                }
            }
            if ((info->intr_clr_reg != INVALIDr) && (INVALIDf != info->intr_clr_field)) {
                 SOC_IF_ERROR_RETURN(soc_reg32_get(unit, info->intr_clr_reg, log_port, i, &rval));
                 soc_reg_field_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field,i);
                 SOC_IF_ERROR_RETURN(soc_reg32_set(unit, info->intr_clr_reg, log_port, i, rval));
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
#endif

STATIC int
_soc_tomahawk_ser_process_ecc(int unit, int block_info_idx, int pipe, int port,
                              const _soc_th_ser_info_t *info,
                              char *prefix_str, char *mem_str,
                              soc_block_t blocktype, int idb_scan_thread)
{
#define _SOC_TH_IF_ERROR_UNLOCK_IDB_RETURN(rv) \
        if (SOC_FAILURE(rv)) { \
            if (idb_lock) { \
                SOC_IDB_UNLOCK(unit); \
                idb_lock = 0; \
            } \
            return rv; \
        }
    _soc_th_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg, parity_enable_reg;
    soc_field_t ecc_field = ECC_ERRf, parity_enable_field;
    uint32 rval, minfo;
    uint32 multiple, double_bit, entry_idx, idx, has_error;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci;
    int rv, idb_lock = 0;
    soc_stat_t *stat = SOC_STAT(unit);
    /*
     * info can be
     *      _soc_th_pm_clp_ser_info[] for which info->mem is always INVALIDm
     *      _soc_th_pm_xlp_ser_info[] for which info->mem is always INVALIDm
     *      _soc_th_idb_ser_info[] for which info->mem may not be INVALIDm
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

    sal_memset(&spci, 0, sizeof(spci));
    has_error = FALSE;
    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        switch (reg_ptr[idx].reg) {
        case IDB_OBM0_QUEUE_ECC_STATUSr:
        case IDB_OBM1_QUEUE_ECC_STATUSr:
        case IDB_OBM2_QUEUE_ECC_STATUSr:
        case IDB_OBM3_QUEUE_ECC_STATUSr:
        case IDB_OBM4_QUEUE_ECC_STATUSr:
        case IDB_OBM5_QUEUE_ECC_STATUSr:
        case IDB_OBM6_QUEUE_ECC_STATUSr:
        case IDB_OBM7_QUEUE_ECC_STATUSr:
            SOC_IDB_LOCK(unit);
            idb_lock = 1;
            break;
#ifdef BCM_TOMAHAWK3_SUPPORT
        case IDB_OBM0_OVERSUB_MON_ECC_STATUSr:
            spci.mem = IDB_OBM0_IOM_STATS_WINDOW_RESULTSm;
            break;
        case IDB_OBM1_OVERSUB_MON_ECC_STATUSr:
            spci.mem = IDB_OBM1_IOM_STATS_WINDOW_RESULTSm;
            break;
        case IDB_OBM2_OVERSUB_MON_ECC_STATUSr:
            spci.mem = IDB_OBM2_IOM_STATS_WINDOW_RESULTSm;
            break;
        case IDB_OBM3_OVERSUB_MON_ECC_STATUSr:
            spci.mem = IDB_OBM3_IOM_STATS_WINDOW_RESULTSm;
            break;
        case CENTRAL_CTR_EVICTION_INTR_STATUSr:
            spci.mem = CENTRAL_CTR_EVICTION_FIFOm;
            break;
#endif
        default:
            idb_lock = 0;
            break;
        }
        reg = reg_ptr[idx].reg;
        if ((blocktype == SOC_BLK_IPIPE || blocktype == SOC_BLK_EPIPE) &&
            (SOC_REG_UNIQUE_ACC(unit, reg) != NULL)) {
               reg = SOC_REG_UNIQUE_ACC(unit, reg)[pipe];
        }
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ?
            reg_ptr[idx].mem_str : mem_str;

        rv = soc_reg32_get(unit, reg, port, 0, &rval);
        _SOC_TH_IF_ERROR_UNLOCK_IDB_RETURN(rv);

        multiple = 0;
        double_bit = 0;
        entry_idx = 0;

        /* LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "idb_status_reg %s, = 0x%x\n"), SOC_REG_NAME(unit, reg), rval)); */
        if (SOC_REG_FIELD_VALID(unit, reg, ecc_field) &&
            soc_reg_field_get(unit, reg, rval, ecc_field)) {
            has_error = TRUE;
            if (reg == CENTRAL_CTR_EVICTION_INTR_STATUSr) {
                reg = CENTRAL_CTR_EVICTION_FIFO_PARITY_ERRORr;
            }
            if (SOC_REG_FIELD_VALID(unit, reg, MULTIPLE_ERRf)) {
                multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
            }
            if (SOC_REG_FIELD_VALID(unit, reg, DOUBLE_BIT_ERRf)) {
                double_bit = soc_reg_field_get(unit, reg, rval, DOUBLE_BIT_ERRf);
            }
            if (SOC_REG_FIELD_VALID(unit, reg, ENTRY_IDXf)) {
                entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            } else if (SOC_REG_FIELD_VALID(unit, reg, MEMINDEXf)) {
                entry_idx = soc_reg_field_get(unit, reg, rval, MEMINDEXf);
            }
            _soc_th_mem_parity_info(unit, block_info_idx, pipe,
                                    info->group_reg_status_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                               SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx,
                               minfo);
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "idb_status_reg %s, = 0x%x\n"),
                 SOC_REG_NAME(unit, reg), rval));
            if (double_bit) {
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

            if (idb_lock) { /* one of the IDB_OBMn_QUEUE_ECC_STATUSr */
                if (SOC_IS_TOMAHAWKPLUS(unit)) {
                    if (double_bit) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_FATAL,
                                           INVALIDm, 
                                           entry_idx); 
                    }
                    /* SBE is turned off - so we cannot be here for SBE */
                } else if (soc_feature(unit, soc_feature_th_a0_sw_war)) {
                    /* for both SBE, DBE */
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FATAL,
                                   INVALIDm, 
                                   entry_idx); 
                } else {
                    /* for both SBE, DBE
                     * clear CMIC accessible OBM/CA counters */
                    rv = _soc_th_idb_counters_clear(unit, pipe, reg_ptr[idx].reg);
                    _SOC_TH_IF_ERROR_UNLOCK_IDB_RETURN(rv);
                }
            } else if (IDB_IS_TDM_CAL_ECC_STATUSr == reg_ptr[idx].reg) {
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
                if (entry_idx & 0x80 ) { /* MSB is set */
                    spci.mem = IS_TDM_CALENDAR1m;
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
            }
#ifdef BCM_TOMAHAWK3_SUPPORT
            else if(SOC_IS_TOMAHAWK3(unit) &&
                   ((reg_ptr[idx].reg == IDB_OBM0_OVERSUB_MON_ECC_STATUSr) ||
                    (reg_ptr[idx].reg == IDB_OBM1_OVERSUB_MON_ECC_STATUSr) ||
                    (reg_ptr[idx].reg == IDB_OBM2_OVERSUB_MON_ECC_STATUSr) ||
                    (reg_ptr[idx].reg == IDB_OBM3_OVERSUB_MON_ECC_STATUSr))) {
                if (SOC_MEM_UNIQUE_ACC(unit, spci.mem) != NULL) {
                    spci.mem = SOC_MEM_UNIQUE_ACC(unit, spci.mem)[pipe];
                }
                parity_enable_reg = IDB_OBM0_SER_CONTROLr;
                parity_enable_field = MON_ECC_ENABLEf;
            }
#endif
            else {
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
                spci.log_id = _soc_th_populate_ser_log(unit,
                                                       parity_enable_reg,
                                                       INVALIDm,
                                                       parity_enable_field,
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
                    if (idb_lock) {
                        SOC_IDB_UNLOCK(unit);
                        idb_lock = 0;
                    }
                    soc_ser_stat_update(unit, 0, spci.blk_type,
                                        spci.parity_type,
                                        spci.double_bit,
                                        SocSerCorrectTypeFailedToCorrect,
                                        stat);
                    return rv;
                }
            }
            if ((spci.mem == INVALIDm) && (spci.log_id != 0)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                   spci.log_id, 0);
                rv = _soc_add_log_generic(unit, &spci);
                _SOC_TH_IF_ERROR_UNLOCK_IDB_RETURN(rv);
                if (soc_property_get(unit, "ser_log_print_one", 0)) {
                    soc_ser_log_print_one(unit, spci.log_id);
                }
            }

            if (spci.mem == INVALIDm) {
                /* Add reporting failed to correct event flag to application */
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                        SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx, minfo);
                soc_ser_stat_update(unit, 0, spci.blk_type,
                                    spci.parity_type,
                                    spci.double_bit,
                                    SocSerCorrectTypeNoAction,
                                    stat);
            }

            /* Clear parity status */
            rv = soc_reg32_set(unit, reg, port, 0, 0);
            _SOC_TH_IF_ERROR_UNLOCK_IDB_RETURN(rv);
        }
        if (idb_lock) {
            SOC_IDB_UNLOCK(unit);
            idb_lock = 0;
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
soc_th_scan_idb_mem_ecc_status(int unit)
{
    int pipe;
    int block_info_idx;
    int port = REG_PORT_ANY;
    char prefix_str[10];
    char *mem_str = "IP IDB memory";
    soc_block_t blocktype = SOC_BLK_IPIPE;
    static const _soc_th_ser_info_t *info = &_soc_th_idb_ser_info[0];
                                        /* only OBM queue buffer */
    int idb_scan_thread = 1;
    /* Scan is needed only for A0 */
    sal_sprintf(prefix_str, "\nUnit: %d ", unit);
    SOC_BLOCK_ITER(unit, block_info_idx, blocktype) {
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            SOC_IF_ERROR_RETURN(
                _soc_tomahawk_ser_process_ecc(unit,
                                              block_info_idx,
                                              pipe, port, info,
                                              prefix_str, mem_str,
                                              blocktype, idb_scan_thread));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_process_parity(int unit, int block_info_idx, int pipe, int port,
                                 const _soc_th_ser_info_t *info,
                                 char *prefix_str, char *mem_str,
                                 soc_block_t blocktype)
{
    _soc_th_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    soc_field_t parity_error_field = PARITY_ERRORf;
    uint32 rval, minfo;
    uint32 multiple, entry_idx, idx, has_error;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci;
    soc_stat_t *stat = SOC_STAT(unit);

    /*
     * info can be
     *      _soc_th_idb_ser_info[] for which info->mem cannot be INVALIDm
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
            _soc_th_mem_parity_info(unit, block_info_idx, pipe,
                                    info->group_reg_status_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               SOC_SER_ERROR_ENTRY_ID_MINFO_SET | entry_idx,
                               minfo);

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
            spci.log_id = _soc_th_populate_ser_log(unit,
                                                   info->enable_reg,
                                                   INVALIDm,
                                                   info->enable_field,
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
                SOC_IF_ERROR_RETURN(_soc_add_log_generic(unit, &spci));
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
_soc_tomahawk_ser_process_mac(int unit, int block_info_idx, int pipe, int port,
                              const _soc_th_ser_info_t *info,
                              int schan, char *prefix_str, char *mem_str, soc_block_t blocktype)
{
    _soc_th_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    soc_field_t single_bit_f, double_bit_f;
    uint64 rval;
    uint32 single_bit = 0, double_bit = 0, idx;
    char *mem_str_ptr;
    uint32 has_error = FALSE, db = FALSE;
    soc_stat_t *stat = SOC_STAT(unit);

    if (schan) {
        /* Some table does not have NACK register */
        return SOC_E_NONE;
    } else {
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
    }

    switch (info->type) {
        case _SOC_PARITY_TYPE_MAC_TX_CDC:
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                single_bit_f = SUM_TX_CDC_SINGLE_BIT_ERRf;
                double_bit_f = SUM_TX_CDC_DOUBLE_BIT_ERRf;
            } else
#endif
            {
                single_bit_f = TX_CDC_SINGLE_BIT_ERRf;
                double_bit_f = TX_CDC_DOUBLE_BIT_ERRf;
            }
            break;
        case _SOC_PARITY_TYPE_MAC_RX_CDC:
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                single_bit_f = SUM_RX_CDC_SINGLE_BIT_ERRf;
                double_bit_f = SUM_RX_CDC_DOUBLE_BIT_ERRf;
            } else
#endif
            {
                single_bit_f = RX_CDC_SINGLE_BIT_ERRf;
                double_bit_f = RX_CDC_DOUBLE_BIT_ERRf;
            }
            break;
        case _SOC_PARITY_TYPE_MAC_RX_TS:
            single_bit_f = RX_TS_MEM_SINGLE_BIT_ERRf;
            double_bit_f = RX_TS_MEM_DOUBLE_BIT_ERRf;
            break;
        default:
            return SOC_E_PARAM;
    }

    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        reg = reg_ptr[idx].reg;
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ? reg_ptr[idx].mem_str : mem_str;

        if (!(SOC_REG_FIELD_VALID(unit, reg, single_bit_f) &&
              SOC_REG_FIELD_VALID(unit, reg, double_bit_f))) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval));

        single_bit = soc_reg64_field32_get(unit, reg, rval, single_bit_f);
        double_bit = soc_reg64_field32_get(unit, reg, rval, double_bit_f);
        if (single_bit || double_bit) {
            has_error = TRUE;
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 0, 0);
            if (double_bit) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "%s %s double-bit ECC error on port %d\n"),
                           prefix_str, mem_str_ptr, port));
                db = TRUE;
            } else {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED, 0, 0);
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "%s %s single-bit ECC error on port %d\n"),
                           prefix_str, mem_str_ptr, port));
            }
        }

        if (info->intr_clr_reg == INVALIDr) {
            continue;
        }

        /* Clear parity status by a rising edge on intr_clr_field in intr_clr_reg */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, info->intr_clr_reg, port, 0, &rval));
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
        } else
#endif
        {
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, info->intr_clr_reg, port, 0, &rval));
            if (single_bit) {
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[0], 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[0], 1);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[0], 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
            }
            if (double_bit) {
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[1], 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[1], 1);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
                soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[1], 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
            }
        }
    }

    if (has_error) {
        soc_ser_stat_update(unit, 0, blocktype, SOC_PARITY_TYPE_PARITY,
                            db, SocSerCorrectTypeNoAction, stat);
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_process_ser(int unit, int block_info_idx, int inst, int pipe,
                          int port, soc_reg_t group_reg, uint64 group_rval,
                          const _soc_th_ser_info_t *info_list,
                          soc_block_t blocktype,
                          char *prefix_str)
{
    const _soc_th_ser_info_t *info;
    int info_index;
    char *mem_str;
    uint64 rval64;
    uint32 rval;
    /*uint32 minfo;*/

    /* This func (_soc_tomahawk_process_ser) is called by
     * _soc_tomahawk_ser_process_all which passes
     * _soc_th_mmu_top_ser_info[] as param to this func.
     * And, _soc_th_mmu_top_ser_info[] list has only one entry with type
     * _SOC_PARITY_TYPE_MMU_SER and 'info' for that entry pointing to
     * _soc_th_mmu_ser_info
     */
    for (info_index = 0; ; info_index++) { /* Loop through each info entry in the mmu_top_ser_info list */
        info = &info_list[info_index]; /* LHS points to 1st entry in _soc_th_mmu_top_ser_info[] */
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        if (group_reg != INVALIDr) {
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
        } else {
            mem_str = SOC_FIELD_NAME(unit, info->group_reg_status_field);
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        
        case _SOC_PARITY_TYPE_MMU_SER:
            /* we can never be here for following 3 types.
             * _soc_th_mmu_top_ser_info has only one entry with type
             * _SOC_PARITY_TYPE_MMU_SER
             */
        case _SOC_PARITY_TYPE_MMU_GLB:
        case _SOC_PARITY_TYPE_MMU_XPE:
        case _SOC_PARITY_TYPE_MMU_SC:
#ifdef BCM_TOMAHAWK2_SUPPORT
        case _SOC_PARITY_TYPE_MMU_SED:
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        case _SOC_PARITY_TYPE_MMU_ITMCFG:
        case _SOC_PARITY_TYPE_MMU_EBCFG:
#endif
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_mmu_err(unit, block_info_idx,
                                                   info, prefix_str, -1));
                /* info points to _soc_th_mmu_top_ser_info[0] */
            break;
        case _SOC_PARITY_TYPE_ECC:
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_ecc(unit, block_info_idx, pipe,
                                               port, info, prefix_str,
                                               mem_str, blocktype, 0));
            if ((info->group_reg_status_field == MIB_RX_MEM_ERRf) || 
                (info->group_reg_status_field == MIB_TX_MEM_ERRf)) {
                COMPILER_64_ZERO(rval64);
                soc_reg64_field32_set(unit, group_reg, &rval64,
                                      info->group_reg_status_field, 1);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, group_reg, port, 0, rval64));
            }
            break;
        case _SOC_PARITY_TYPE_PARITY:
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_parity(unit, block_info_idx, pipe,
                                                  port, info, prefix_str,
                                                  mem_str, blocktype));
            break;
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TOMAHAWKPLUS_SUPPORT)
        case _SOC_PARITY_TYPE_CLMAC:
           SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_clmac(unit, block_info_idx, info,
                                                 prefix_str, mem_str, blocktype));
            break;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        case _SOC_PARITY_TYPE_CDMAC:
            SOC_IF_ERROR_RETURN
                 (_soc_tomahawk_ser_process_cdmac(unit, block_info_idx, info,
                                                  prefix_str, mem_str, blocktype));
            break;
#endif
        case _SOC_PARITY_TYPE_MAC_TX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_TS:
            /* xxx_SINGLE_BIT_ERRf, xxx_DOUBLE_BIT_ERRf */
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_mac(unit, block_info_idx, pipe,
                                               port, info, FALSE,
                                               prefix_str, mem_str, blocktype));
            if ((info->group_reg_status_field == MAC_RX_CDC_MEM_ERRf) ||
                (info->group_reg_status_field == MAC_TX_CDC_MEM_ERRf) ||
                (info->group_reg_status_field == MAC_RX_TS_CDC_MEM_ERRf)) {
                rval = 0;
                soc_reg_field_set(unit, group_reg, &rval,
                                  info->group_reg_status_field, 1);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, group_reg, port, 0, rval));
            }
            break;
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the list */

    return SOC_E_NONE;
}

#ifdef BCM_TOMAHAWK3_SUPPORT
STATIC char *_soc_th3_ser_hwmem_base_info[] = {
    /* from SER_STATUS_BUS format in regsfile */
    "Invalid value",                        /* 0x0 */
    "IARB_EVENT_FIFO",                      /* 0x1 */
    "Invalid value",                        /* 0x2 */
    "Invalid value",                        /* 0x3 */
    "Invalid value",                        /* 0x4 */
    "Invalid value",                        /* 0x5 */
    "Invalid value",                        /* 0x6 */
    "INGRESS_PACKET_BUFFER",                /* 0x7 */
    "Invalid value",                        /* 0x8 */
    "ISW_EOP_BUFFER_B",                     /* 0x9 */
    "Invalid value",                        /* 0xa */
    "IADAPT_PASSTHRU",                      /* 0xb */
    "Invalid value",                        /* 0xc */
    "Invalid value",                        /* 0xd */
    "Invalid value",                        /* 0xe */
    "IPARS_ISW_PT_BUF",                     /* 0xf */
    "IFWD1_PASSTHRU",                       /* 0x10 */
    "Invalid value",                        /* 0x11 */
    "Invalid value",                        /* 0x12 */
    "Invalid value",                        /* 0x13 */
    "Invalid value",                        /* 0x14 */
    "Invalid value",                        /* 0x15 */
    "Invalid value",                        /* 0x16 */
    "IFP_BUS",                              /* 0x17 */
    "IRSEL_PT_BUF",                         /* 0x18 */
    "IRSEL_BUS",                            /* 0x19 */
    "Invalid value",                        /* 0x1a */
    "Invalid value",                        /* 0x1b */
    "Invalid value",                        /* 0x1c */
    "Invalid value",                        /* 0x1d */
    "Invalid value",                        /* 0x1e */
    "Invalid value",                        /* 0x1f */
    "Invalid value",                        /* 0x20 */
    "Invalid value",                        /* 0x21 */
    "Invalid value",                        /* 0x22 */
    "Invalid value",                        /* 0x23 */
    "Invalid value",                        /* 0x24 */
    "Invalid value",                        /* 0x25 */
    "Invalid value",                        /* 0x26 */
    "Invalid value",                        /* 0x27 */
    "Invalid value",                        /* 0x28 */
    "Invalid value",                        /* 0x29 */
    "Invalid value",                        /* 0x2a */
    "Invalid value",                        /* 0x2b */
    "Invalid value",                        /* 0x2c */
    "Invalid value",                        /* 0x2d */
    "Invalid value",                        /* 0x2e */
    "Invalid value",                        /* 0x2f */
    "Invalid value",                        /* 0x30 */
    "Invalid value",                        /* 0x31 */
    "Invalid value",                        /* 0x32 */
    "Invalid value",                        /* 0x33 */
    "Invalid value",                        /* 0x34 */
    "Invalid value",                        /* 0x35 */
    "Invalid value",                        /* 0x36 */
    "Invalid value",                        /* 0x37 */
    "CENTRAL_EVICTION_FIFO",                /* 0x38 */
    "Invalid value",                        /* 0x39 */
    "Invalid value",                        /* 0x3a */
    "Invalid value",                        /* 0x3b */
    "Invalid value",                        /* 0x3c */
    "Invalid value",                        /* 0x3d */
    "Invalid value",                        /* 0x3e */
    "Invalid value",                        /* 0x3f */
    "EP_MPB_DATA",                          /* 0x40 */
    "EP_PACKET_DATA",                       /* 0x41 */
    "EARB_SOP_EVENT_FIFO",                  /* 0x42 */
    "Invalid value",                        /* 0x43 */
    "Invalid value",                        /* 0x44 */
    "Invalid value",                        /* 0x45 */
    "Invalid value",                        /* 0x46 */
    "Invalid value",                        /* 0x47 */
    "Invalid value",                        /* 0x48 */
    "Invalid value",                        /* 0x49 */
    "Invalid value",                        /* 0x4a */
    "Invalid value",                        /* 0x4b */
    "Invalid value",                        /* 0x4c */
    "Invalid value",                        /* 0x4d */
    "Invalid value",                        /* 0x4e */
    "Invalid value",                        /* 0x4f */
    "Invalid value",                        /* 0x50 */
    "Invalid value",                        /* 0x51 */
    "Invalid value",                        /* 0x52 */
    "Invalid value",                        /* 0x53 */
    "Invalid value",                        /* 0x54 */
    "Invalid value",                        /* 0x55 */
    "Invalid value",                        /* 0x56 */
    "Invalid value",                        /* 0x57 */
    "Invalid value",                        /* 0x58 */
    "Invalid value",                        /* 0x59 */
    "Invalid value",                        /* 0x5a */
    "Invalid value",                        /* 0x5b */
    "EGR_PMOD_BUS",                         /* 0x5c */
    "EGR_FPPARS_BUS",                       /* 0x5d */
    "EFP_BUS",                              /* 0x5e */
    "Invalid value",                        /* 0x5f */
    "EP_EFPBUF",                            /* 0x60 */
    "Invalid value",                        /* 0x61 */
    "Invalid value",                        /* 0x62 */
    "Invalid value",                        /* 0x63 */
    "EPOST_OUTPUT_FIFO",                    /* 0x64 */
    "EPOST_SLOT_PIPELINE",                  /* 0x65 */
    "EPOST_PKT_BUF",                        /* 0x66 */
    "EDB_RESIDUAL_DATA_BUF",                /* 0x67 */
    "EDB_XMIT_START_COUNT",                 /* 0x68 */
    "EDB_PM0_EOP_STATUS",                   /* 0x69 */
    "EDB_PM0_CTRL_FIFO",                    /* 0x6a */
    "EDB_PM0_DATA_FIFO",                    /* 0x6b */
    "EDB_PM1_EOP_STATUS",                   /* 0x6c */
    "EDB_PM1_CTRL_FIFO",                    /* 0x6d */
    "EDB_PM1_DATA_FIFO",                    /* 0x6e */
    "EDB_PM2_EOP_STATUS",                   /* 0x6f */
    "EDB_PM2_CTRL_FIFO",                    /* 0x70 */
    "EDB_PM2_DATA_FIFO",                    /* 0x71 */
    "EDB_PM3_EOP_STATUS",                   /* 0x72 */
    "EDB_PM3_CTRL_FIFO",                    /* 0x73 */
    "EDB_PM3_DATA_FIFO",                    /* 0x74 */
    "EDB_CM_EOP_STATUS",                    /* 0x75 */
    "EDB_CM_CTRL_FIFO",                     /* 0x76 */
    "EDB_CM_DATA_FIFO",                     /* 0x77 */
    "EDB_LPBK_EOP_STATUS",                  /* 0x78 */
    "EDB_LPBK_CTRL_FIFO",                   /* 0x79 */
    "EDB_LPBK_DATA_FIFO"                    /* 0x7a */
};
#endif
STATIC char *_soc_th_ser_hwmem_base_info[] = {
    /* from SER_STATUS_BUS format in regsfile */
    "OBM_QUEUE_FIFO - In Idb",                                       /* 0x0 */
    "OBM_DATA_FIFO - In Idb",                                        /* 0x1 */
    "CELL_ASSEM_BUFFER - In Idb",                                    /* 0x2 */
    "CPU_CELL_ASSEM_BUFFER - In Idb",                                /* 0x3 */
    "LPBK_CELL_ASSEM_BUFFER - In Idb",                               /* 0x4 */
    "Invalid value",                                                 /* 0x5 */
    "Invalid value",                                                 /* 0x6 */
    "INGRESS_PACKET_BUFFER - In Ipars",                              /* 0x7 */
    "ISW3_EOP_BUFFER_A - In Isw3",                                   /* 0x8 */
    "ISW3_EOP_BUFFER_B - In Isw3",                                   /* 0x9 */
    "BUBBLE_MOP - In Isw2",                                          /* 0xa */
    "IPARS_IVP_PT_BUS - In Ivp, Passthru Highway bus",               /* 0xb */
    "IVP_ISW1_PT_BUS - In Isw1, Passthru Highway bus",               /* 0xc */
    "IFWD_ISW1_PT_BUS - In Isw1, Passthru Highway bus",              /* 0xd */
    "Invalid value",                                                 /* 0xe */
    "ISW1_ISW2_PT_BUS - In Isw2, Passthru Highway bus",              /* 0xf */
    "ICFG_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x10 */
    "IPARS_BUS - PassThrought bus parity check, MEMINDEX is 0",      /* 0x11 */
    "IVXLT_BUS - PassThrought bus parity check, MEMINDEX is 0",      /* 0x12 */
    "IVP_BUS - PassThrought bus parity check, MEMINDEX is 0",        /* 0x13 */
    "IFWD_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x14 */
    "IRSEL1_BUS - PassThrought bus parity check, MEMINDEX is 0",     /* 0x15 */
    "ISW1_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x16 */
    "IFP_BUS - PassThrought bus parity check, MEMINDEX is 0",        /* 0x17 */
    "IRSEL2_BUS - PassThrought bus parity check, MEMINDEX is 0",     /* 0x18 */
    "ISW2_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x19 */
    "Invalid value",                                                 /* 0x1a */
    "Invalid value",                                                 /* 0x1b */
    "Invalid value",                                                 /* 0x1c */
    "Invalid value",                                                 /* 0x1d */
    "Invalid value",                                                 /* 0x1e */
    "Invalid value",                                                 /* 0x1f */
    "Invalid value",                                                 /* 0x20 */
    "Invalid value",                                                 /* 0x21 */
    "Invalid value",                                                 /* 0x22 */
    "Invalid value",                                                 /* 0x23 */
    "Invalid value",                                                 /* 0x24 */
    "Invalid value",                                                 /* 0x25 */
    "Invalid value",                                                 /* 0x26 */
    "Invalid value",                                                 /* 0x27 */
    "Invalid value",                                                 /* 0x28 */
    "Invalid value",                                                 /* 0x29 */
    "Invalid value",                                                 /* 0x2a */
    "Invalid value",                                                 /* 0x2b */
    "Invalid value",                                                 /* 0x2c */
    "Invalid value",                                                 /* 0x2d */
    "Invalid value",                                                 /* 0x2e */
    "Invalid value",                                                 /* 0x2f */
    "LEARN_FIFO - In L2MM",                                          /* 0x30 */
    "Invalid value",                                                 /* 0x31 */
    "Invalid value",                                                 /* 0x32 */
    "Invalid value",                                                 /* 0x33 */
    "Invalid value",                                                 /* 0x34 */
    "Invalid value",                                                 /* 0x35 */
    "Invalid value",                                                 /* 0x36 */
    "Invalid value",                                                 /* 0x37 */
    "CENTRAL_EVICTION_FIFO - In CEV",                                /* 0x38 */
    "Invalid value",                                                 /* 0x39 */
    "Invalid value",                                                 /* 0x3a */
    "Invalid value",                                                 /* 0x3b */
    "Invalid value",                                                 /* 0x3c */
    "Invalid value",                                                 /* 0x3d */
    "Invalid value",                                                 /* 0x3e */
    "Invalid value",                                                 /* 0x3f */
    "EP_MPB_DATA - In El3",                                          /* 0x40 */
    "EP_INITBUF - In Ehcpm",                                         /* 0x41 */
    "CM_DATA_BUFFER - In Edatabuf",                                  /* 0x42 */
    "PM0_DATA_BUFFER - In Edatabuf",                                 /* 0x43 */
    "PM1_DATA_BUFFER - In Edatabuf",                                 /* 0x44 */
    "PM2_DATA_BUFFER - In Edatabuf",                                 /* 0x45 */
    "PM3_DATA_BUFFER - In Edatabuf",                                 /* 0x46 */
    "PM4_DATA_BUFFER - In Edatabuf",                                 /* 0x47 */
    "PM5_DATA_BUFFER - In Edatabuf",                                 /* 0x48 */
    "PM6_DATA_BUFFER - In Edatabuf",                                 /* 0x49 */
    "PM7_DATA_BUFFER - In Edatabuf",                                 /* 0x4a */
    "LBP_DATA_BUFFER - In Edatabuf",                                 /* 0x4b */
    "PM_RESI0_DATA_BUFFER - In Edatabuf",                            /* 0x4c */
    "PM_RESI1_DATA_BUFFER - In Edatabuf",                            /* 0x4d */
    "Invalid value",                                                 /* 0x4e */
    "Invalid value",                                                 /* 0x4f */
    "CM_EDB_CTRL_BUFFER - In Edatabuf",                              /* 0x50 */
    "PM0_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x51 */
    "PM1_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x52 */
    "PM2_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x53 */
    "PM3_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x54 */
    "PM4_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x55 */
    "PM5_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x56 */
    "PM6_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x57 */
    "PM7_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x58 */
    "LBP_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x59 */
    "EGR_VLAN_BUS - PassThrought bus parity check, MEMINDEX is 0",   /* 0x5a */
    "EGR_HCPM_BUS - PassThrought bus parity check, MEMINDEX is 0",   /* 0x5b */
    "EGR_PMOD_BUS - PassThrought bus parity check, MEMINDEX is 0",   /* 0x5c */
    "EGR_FPPARS_BUS - PassThrought bus parity check, MEMINDEX is 0", /* 0x5d */
    "EFP_BUS - PassThrought bus parity check, MEMINDEX is 0",        /* 0x5e */
    "EP_PT_BUS - In Esw, Passthru Highway bus"                       /* 0x5f */
};

#define _SOC_TH_SER_REG 1
#define _SOC_TH_SER_MEM 0

STATIC void
_soc_tomahawk_print_ser_fifo_details(int unit, uint8 regmem, soc_block_t blk,
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
        if (regmem == _SOC_TH_SER_MEM) {
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
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        LOG_WARN(BSL_LS_SOC_SER,
                                  (BSL_META_U(unit,
                                              "Mem hwbase: 0x%x [%s]\n"), base,
                                   _soc_th3_ser_hwmem_base_info[base]));
                    } else
#endif
                    {
                        LOG_WARN(BSL_LS_SOC_SER,
                                  (BSL_META_U(unit,
                                              "Mem hwbase: 0x%x [%s]\n"), base,
                                   _soc_th_ser_hwmem_base_info[base]));
                    }
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
_soc_th_check_flex_ctr_addr(int unit, uint32 address, soc_block_t blk, uint32 stage,
                            uint32 index)
{
    uint32 index2 = index;
    if ((SOC_BLK_IPIPE == blk) && (21 == stage)) {
        switch (address & 0xfffff000) {
        /* pool 0 */
        case 0x56800000:
        case 0x56801000:
        case 0x56802000:
        case 0x56803000:

        case 0x56804000:
        case 0x56808000:
        case 0x5680c000:
        case 0x56810000:

        /* pool 1 */
        case 0x56820000:
        case 0x56821000:
        case 0x56822000:
        case 0x56823000:

        case 0x56824000:
        case 0x56828000:
        case 0x5682c000:
        case 0x56830000:

        /* pool 2 */
        case 0x56840000:
        case 0x56841000:
        case 0x56842000:
        case 0x56843000:

        case 0x56844000:
        case 0x56848000:
        case 0x5684c000:
        case 0x56850000:

        /* pool 3 */
        case 0x56880000:
        case 0x56881000:
        case 0x56882000:
        case 0x56883000:

        case 0x56884000:
        case 0x56888000:
        case 0x5688c000:
        case 0x56890000:

        /* pool 4 */
        case 0x568a0000:
        case 0x568a1000:
        case 0x568a2000:
        case 0x568a3000:

        case 0x568a4000:
        case 0x568a8000:
        case 0x568ac000:
        case 0x568b0000:
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
_soc_th_check_counter_with_ecc(int unit, soc_mem_t mem)
{
    int mem_is_ctr_with_ecc = 0;

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
_soc_th_force_unique_mode(int unit, soc_mem_t mem)
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
#ifdef BCM_TOMAHAWK3_SUPPORT
        (EDB_XMIT_START_COUNTm == mem) ||
#endif
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

#ifdef BCM_TOMAHAWK2_SUPPORT
STATIC int
_soc_tomahawk2_tcam_only_remap(int unit, soc_mem_t *mem)
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
        case L2_USER_ENTRY_ONLYm:
            *mem = L2_USER_ENTRYm;
            break;
        case SRC_COMPRESSION_TCAM_ONLYm:
            *mem = SRC_COMPRESSIONm;
            break;
        case DST_COMPRESSION_TCAM_ONLYm:
            *mem = DST_COMPRESSIONm;
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
        default:
            break;
    }

    return SOC_E_NONE;
}
#endif

int
soc_tomahawk_process_ser_fifo(int unit, soc_block_t blk, int pipe,
                              char *prefix_str, int l2_mgmt_ser_fifo, int reg_type, int cmic_bit)
{
    int rv, mem_mode, disable_ser_correction;
    int ret = SOC_E_NONE;
    uint8 bidx;
    soc_mem_t mem;
    char blk_str[30];
    soc_reg_t reg = INVALIDr;
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_reg_t intr_reg = INVALIDr;
#endif
    _soc_ser_correct_info_t spci;
    _soc_mem_ser_en_info_t *ser_info_table = NULL;
    soc_reg_t parity_enable_reg;
    soc_mem_t parity_enable_mem = INVALIDm;
    soc_field_t parity_enable_field;
    uint32 ecc_parity = 0, address = 0;
    uint32 reg_val, mask, entry[SOC_MAX_MEM_WORDS];
    uint32 stage = 0, addrbase = 0, index = 0, type = 0;
    uint32 sblk = 0, regmem = 0, non_sbus = 0, drop = 0;
    soc_mem_t ipsf[] = { ING_SER_FIFO_PIPE0m, ING_SER_FIFO_PIPE1m,
                         ING_SER_FIFO_PIPE2m, ING_SER_FIFO_PIPE3m,
#ifdef BCM_TOMAHAWK3_SUPPORT
                         ING_SER_FIFO_PIPE4m, ING_SER_FIFO_PIPE5m,
                         ING_SER_FIFO_PIPE6m, ING_SER_FIFO_PIPE7m
#endif
                         };
    soc_mem_t epsf[] = { EGR_SER_FIFO_PIPE0m, EGR_SER_FIFO_PIPE1m,
                         EGR_SER_FIFO_PIPE2m, EGR_SER_FIFO_PIPE3m,
#ifdef BCM_TOMAHAWK3_SUPPORT
                         EGR_SER_FIFO_PIPE4m, EGR_SER_FIFO_PIPE5m,
                         EGR_SER_FIFO_PIPE6m, EGR_SER_FIFO_PIPE7m
#endif
                         };
    soc_reg_t epsr[] = {
                         EGR_INTR_STATUS_PIPE0r, EGR_INTR_STATUS_PIPE1r,
                         EGR_INTR_STATUS_PIPE2r, EGR_INTR_STATUS_PIPE3r,
#ifdef BCM_TOMAHAWK3_SUPPORT
                         EGR_INTR_STATUS_PIPE4r, EGR_INTR_STATUS_PIPE5r,
                         EGR_INTR_STATUS_PIPE6r, EGR_INTR_STATUS_PIPE7r
#endif
                         };
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_mem_t epsf2[] = { EGR_SER_FIFO_2_PIPE0m, EGR_SER_FIFO_2_PIPE1m,
                          EGR_SER_FIFO_2_PIPE2m, EGR_SER_FIFO_2_PIPE3m,
                          EGR_SER_FIFO_2_PIPE4m, EGR_SER_FIFO_2_PIPE5m,
                          EGR_SER_FIFO_2_PIPE6m, EGR_SER_FIFO_2_PIPE7m, };
    soc_reg_t epsr2[] = { EGR_INTR_STATUS_2_PIPE0r, EGR_INTR_STATUS_2_PIPE1r,
                          EGR_INTR_STATUS_2_PIPE2r, EGR_INTR_STATUS_2_PIPE3r,
                          EGR_INTR_STATUS_2_PIPE4r, EGR_INTR_STATUS_2_PIPE5r,
                          EGR_INTR_STATUS_2_PIPE6r, EGR_INTR_STATUS_2_PIPE7r };
    soc_mem_t edbsf[] = { EDB_SER_FIFO_PIPE0m, EDB_SER_FIFO_PIPE1m,
                          EDB_SER_FIFO_PIPE2m, EDB_SER_FIFO_PIPE3m,
                          EDB_SER_FIFO_PIPE4m, EDB_SER_FIFO_PIPE5m,
                          EDB_SER_FIFO_PIPE6m, EDB_SER_FIFO_PIPE7m };
    soc_reg_t edbsr[] = { EDB_INTR_STATUS_PIPE0r, EDB_INTR_STATUS_PIPE1r,
                          EDB_INTR_STATUS_PIPE2r, EDB_INTR_STATUS_PIPE3r,
                          EDB_INTR_STATUS_PIPE4r, EDB_INTR_STATUS_PIPE5r,
                          EDB_INTR_STATUS_PIPE6r, EDB_INTR_STATUS_PIPE7r };
#endif
    uint32 stat_flags = 0;
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
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit) && cmic_bit & 0x2) {
                mem = DLB_ECMP_SER_FIFOm;
                reg = DLB_ECMP_INTR_STATUSr;
                mask = 0x00000001; /* SER_FIFO_NOT_EMPTY */
                sal_sprintf(blk_str, "IPIPE_DLB_SER_FIFO");
            } else
#endif
            {
                mem = ipsf[pipe];
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    mask = _soc_th3_ip_pipe_fifo_bmask[unit][pipe];
                } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
                if (SOC_IS_TOMAHAWK2(unit)) {
                    mask = _soc_th2_ip_pipe_fifo_bmask[unit][pipe];
                } else
#endif
                {
                    mask = _soc_th_ip_pipe_fifo_bmask[unit][pipe];
                }
            }
            sal_sprintf(blk_str, "IPIPE");
        }

        ser_info_table = SOC_IP_MEM_SER_INFO(unit);
        break;
    case SOC_BLK_EPIPE:
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (reg_type == 3) {
                /* EDB interrupt belongs to reg3 */
                mem = edbsf[pipe];
                reg = edbsr[pipe];
            } else if (cmic_bit & 0x1fe0) {
                /* EFPMOD interrupt uses bit 5-12 */
                mem = epsf2[pipe];
                reg = epsr2[pipe];
            } else {
                mem = epsf[pipe];
                reg = epsr[pipe];
            }
            mask = 0x00000001; /* SER_FIFO_NON_EMPTYf */
            sal_sprintf(blk_str, "EPIPE");
            ser_info_table = SOC_EP_MEM_SER_INFO(unit);
        } else
#endif
        {
            mem = epsf[pipe];
            mask = 0x00000001; /* SER_FIFO_NON_EMPTYf */
            reg = epsr[pipe];
            sal_sprintf(blk_str, "EPIPE");
            ser_info_table = SOC_EP_MEM_SER_INFO(unit);
        }
        break;
    default: return SOC_E_PARAM;
    }

    do {
        sal_memset(entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_pop(unit, mem, MEM_BLOCK_ANY, entry));
        /* process entry */
        if (soc_mem_field32_get(unit, mem, entry, VALIDf)) {
            disable_ser_correction = 0;
            parity_enable_reg = INVALIDr;
            parity_enable_field = INVALIDf;
            sal_memset(&spci, 0, sizeof(spci));
            ecc_parity = soc_mem_field32_get(unit, mem, entry, ECC_PARITYf);
            regmem = soc_mem_field32_get(unit, mem, entry, MEM_TYPEf);
            address = soc_mem_field32_get(unit, mem, entry, ADDRESSf);
            stage = soc_mem_field32_get(unit, mem, entry, PIPE_STAGEf);
            type = soc_mem_field32_get(unit, mem, entry, INSTRUCTION_TYPEf);
            drop = soc_mem_field32_get(unit, mem, entry, DROPf);

            SOC_BLOCK_ITER(unit, bidx, blk) {
                sblk = SOC_BLOCK2SCH(unit, bidx);
                break;
            }
            LOG_WARN(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                                  "%s\n"), prefix_str));
            if (soc_mem_field32_get(unit, mem, entry, MULTIPLEf)) {
                LOG_WARN(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "Multiple: ")));
            }
            if (regmem == _SOC_TH_SER_REG) {
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
            if (regmem == _SOC_TH_SER_MEM) {
                /* process mem */
                non_sbus = soc_mem_field32_get(unit, mem, entry, NON_SBUSf);
                addrbase = soc_mem_field32_get(unit, mem, entry, MEMBASEf);
                index = soc_mem_field32_get(unit, mem, entry, MEMINDEXf);
#ifdef BCM_TOMAHAWK3_SUPPORT
                /* For memory EDB_XMIT_START_COUNT, we should remap the address
                   from 0x5da00000 to 0x5c280000
                */
                if (SOC_IS_TOMAHAWK3(unit)) {
                    if (((address - index) == 0x5da00000) && (mem == edbsf[pipe])) {
                        address = 0x5c280000 + index;
                    }
                }
#endif
/*
 * S/W WAR for TH-4666. When we inject an error into ISW2 buffer on TH/TH+ and
 * trigger it, H/W fails to set NON_SBUS in the fifo memory
 * and S/W wrongly interprets it to HG_TRUNK_GROUPm
 */
                if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWKPLUS(unit)) {
                    if (!l2_mgmt_ser_fifo && (non_sbus == 0)) {
                        uint32 err_index = 0;
                        soc_mem_t err_mem = INVALIDm;

                        err_index = _soc_th_check_flex_ctr_addr(unit, address,
                                                                blk, stage,
                                                                index);
                        err_mem = soc_addr_to_mem_extended(unit, sblk, -1,
                                                           address - err_index);
                        if (err_mem == HG_TRUNK_GROUPm) {
                            non_sbus = 1;
                        }
                    }
                }

                if (non_sbus == 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                       SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       address);
                    if (!SOC_IS_TOMAHAWK2(unit)) {
                        index = _soc_th_check_flex_ctr_addr(unit, address, blk,
                                                            stage, index);
                    }

                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk,
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
                    }
#ifdef BCM_TOMAHAWK3_SUPPORT
                    /* In TH3, tcam scan engine still reports the ser error on IFP_TCAM
                       with narrow view, even the narrow view is removed. So need to remap
                       the index for IFP_TCAM*/
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        if ((spci.mem == IFP_TCAMm || spci.mem == IFP_TCAM_PIPE0m ||
                             spci.mem == IFP_TCAM_PIPE1m || spci.mem == IFP_TCAM_PIPE2m ||
                             spci.mem == IFP_TCAM_PIPE3m || spci.mem == IFP_TCAM_PIPE4m ||
                             spci.mem == IFP_TCAM_PIPE5m || spci.mem == IFP_TCAM_PIPE6m ||
                             spci.mem == IFP_TCAM_PIPE7m) && (type == 4)) {
                            spci.index = index / 2;
                        }
                    }
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
                    if ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit)) &&
                         soc_feature(unit, soc_feature_tcam_scan_engine)) {
                        _soc_tomahawk2_tcam_only_remap(unit, &spci.mem);
                    }
#endif
                    if (spci.mem != INVALIDm) {
                        spci.flags |= SOC_SER_REG_MEM_KNOWN;
                        if (!spci.double_bit) {
                            disable_ser_correction =
                                _soc_th_check_counter_with_ecc(unit, spci.mem);
                        }

                        spci.flags |= SOC_SER_LOG_WRITE_CACHE;

                        if ((SOC_MEM_INFO(unit, spci.mem).flags & SOC_MEM_FLAG_CAM) &&
                             soc_feature(unit, soc_feature_tcam_scan_engine)) {
#ifdef BCM_TOMAHAWK2_SUPPORT
                            if (SOC_IS_TOMAHAWK2(unit)) {
                                ret = _soc_th2_ser_tcam_control_reg_get(spci.mem,
                                                        &parity_enable_reg,
                                                        &parity_enable_field);
                            }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
                            if (SOC_IS_TOMAHAWK3(unit)) {
                                ret = _soc_th3_ser_tcam_control_reg_get(spci.mem,
                                                        &parity_enable_reg,
                                                        &parity_enable_field);
                            }
#endif
                            if ( SOC_E_NONE == ret) {
                                LOG_VERBOSE(BSL_LS_SOC_SER,
                                          (BSL_META_U(unit,
                                          "unit %d, tcam %s reported in ser_fifo!\n"),
                                           unit, SOC_MEM_NAME(unit,mem) ));
                                }
                        } else {

                            _soc_th_ser_control_reg_get(unit, ser_info_table,
                                                        spci.mem,
                                                        &parity_enable_reg,
                                                        &parity_enable_mem,
                                                        &parity_enable_field);
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
                        rv = soc_th_ser_mem_mode_get(unit, spci.mem,
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
                            LOG_WARN(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "Could not determine unique-global mode for"
                                            "mem %s - will assume global mode !!\n"),
                                 SOC_MEM_NAME(unit, mem)));
                        } else if (_soc_th_force_unique_mode(unit, spci.mem)) {
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
 * do ser_correction for _PIPE0,1,2,3 view. Thus, for such mems we need to
 * change mem to mem_PIPE01,2,3 here.
 *
 * For _DATA_ONLY mems are exception to above rule because for these,
 * ser_correction logic will first remap them to aggr_view and then do
 * correction. For such mems, by specifying them in above equation,
 * we dont change them to _PIPE0,1,2,3 in this function.
 * These will be remapped to aggr and then aggr_PIPE0,1,2,3 view in
 * common/ser.c
 *
 * By not specifying mems in above equation, we change them to _PIPE0,1,2,3
 * view.  Why do we not mention following mems in above equation ?
 *      L3_TUNNEL_DATA_ONLY - this mem never operateds in UNIQUE mode
 *
 *      FPEM_ECC - mem_mode_get will return GLOBAL for this mem.
 *                 common/ser.c will remap it to EXACT_MATCH_2, 4 and then
 *                 change it to _PIPE0,1,2,3
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
                        spci.parity_type = _soc_th_mem_has_ecc(unit, spci.mem)?
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
                    spci.log_id = _soc_th_populate_ser_log(unit,
                                                           parity_enable_reg,
                                                           parity_enable_mem,
                                                           parity_enable_field,
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
                        soc_ser_stat_update(unit, stat_flags, spci.blk_type,
                                            spci.parity_type,
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
                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus,
                                                         l2_mgmt_ser_fifo);
                    _soc_th_populate_ser_log(unit, INVALIDr, INVALIDm, INVALIDf,
                                             INVALIDm, bidx, 0, index,
                                             sal_time_usecs(), 0, 0, 1, 0, 0, 1,
                                             FALSE, addrbase);
                }
            } else {
                /* process reg */
                non_sbus = soc_mem_field32_get(unit, mem, entry, NON_SBUSf);
                addrbase = soc_mem_field32_get(unit, mem, entry, REGBASEf);
                index = soc_mem_field32_get(unit, mem, entry, REGINDEXf);
                if (l2_mgmt_ser_fifo) {
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "%s SER reg address reported in "
                                          "L2_MGMT_SER_FIRO !!\n"),
                               blk_str));
                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk,
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
                    _soc_tomahawk_print_ser_fifo_details(unit, 1, blk, sblk,
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
                        soc_ser_stat_update(unit, 0, spci.blk_type,
                                            spci.parity_type,
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
                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk,
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
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (blk == SOC_BLK_EPIPE) {
                    intr_reg = ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r;
                } else {
                    intr_reg = ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r;
                }
                SOC_IF_ERROR_RETURN
                    (soc_iproc_getreg(unit, soc_reg_addr(unit, intr_reg,
                                      REG_PORT_ANY, 0), &reg_val));
            } else
#endif
            {
                SOC_IF_ERROR_RETURN
                    (READ_CMIC_CMC0_IRQ_STAT3r(unit, &reg_val));
            }
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
        }
    } while (reg_val & mask);
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_check_fifo_intr(int unit, uint32 cmic_bit, uint8 cmic_reg)
{
    if (SOC_IS_TOMAHAWK3(unit) &&
        ((cmic_bit & 0x1fffe2 && cmic_reg == 0) ||
         (cmic_bit & 0xff0000 && cmic_reg == 1) ||
         (cmic_bit & 0x1fe && cmic_reg == 3))) {
        return TRUE;
    }
    if (SOC_IS_TOMAHAWK2(unit) && (cmic_bit & 0x1e1e0)) {
        return TRUE;
    }
    if (((cmic_bit != _SOC_TH_IPIPE0_IP0_INTR)
          && (cmic_bit != _SOC_TH_IPIPE1_IP0_INTR)
          && (cmic_bit != _SOC_TH_IPIPE2_IP0_INTR)
          && (cmic_bit != _SOC_TH_IPIPE3_IP0_INTR)) 
          && (! SOC_IS_TOMAHAWK2(unit))
          && (!SOC_IS_TOMAHAWK3(unit))) {
        return TRUE;
    }

    return FALSE;
}

STATIC int
_soc_tomahawk_ser_process_all(int unit, int reg_type, int bit)
{
    uint8      rbi;
    int        port = REG_PORT_ANY;
    uint32     cmic_rval = 0, cmic_bit;
    uint64     rb_rval64;
    const      _soc_th_ser_route_block_t *rb;
    char       prefix_str[10];
    int        block_info_idx;
    soc_reg_t  tmp_reg;
    int        processed_an_intr = 0;
    soc_stat_t *stat = SOC_STAT(unit);
    COMPILER_64_ZERO(rb_rval64);

    sal_sprintf(prefix_str, "\nUnit: %d ", unit);

    LOG_VERBOSE(BSL_LS_SOC_SER,
             (BSL_META_U(unit,
                         "unit %d, _soc_tomahawk_ser_process_all called: reg_type %d, bit %d \n"),
              unit, reg_type, bit));

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        switch (reg_type) {
        case 3:
            /* Read CMIC parity status register */
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN
                (READ_CMIC_CMC0_IRQ_STAT3r(unit, &cmic_rval));
            if (cmic_rval == 0) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                     unit));
                return SOC_E_NONE;
            }
            break;
        case 4:
            /* Read PM0-31 parity status register */
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN
                (READ_CMIC_CMC0_IRQ_STAT4r(unit, &cmic_rval));
            if (cmic_rval == 0) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                     unit));
                return SOC_E_NONE;
            }
            break;
        case 5:
            /* Read PM32 parity status register */
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN
                (READ_CMIC_CMC0_IRQ_STAT5r(unit, &cmic_rval));
            if (cmic_rval == 0) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                     unit));
                return SOC_E_NONE;
            }
            break;
#ifdef BCM_TOMAHAWK2_SUPPORT
        case 6:
            /* Read PM32-63 parity status register */
            /* coverity[result_independent_of_operands] */     
            SOC_IF_ERROR_RETURN
                (READ_CMIC_CMC0_IRQ_STAT6r(unit, &cmic_rval));
            if (cmic_rval == 0) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                     unit));
                return SOC_E_NONE;
            }        
            break;
#endif
        default:
            return SOC_E_NONE;
        }
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        switch(reg_type) {
            case 0:
                READ_CMICx_INTC_INTR(unit, ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r,
                                     &cmic_rval);
                if (cmic_rval == 0) {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                         unit));
                    return SOC_E_NONE;
                }
                break;
            case 1:
                READ_CMICx_INTC_INTR(unit, ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r,
                                     &cmic_rval);
                if (cmic_rval == 0) {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                         unit));
                    return SOC_E_NONE;
                }
                break;
            case 2:
                READ_CMICx_INTC_INTR(unit, ICFG_CHIP_LP_INTR_RAW_STATUS_REG2r,
                                     &cmic_rval);
                if (cmic_rval == 0) {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                         unit));
                    return SOC_E_NONE;
                }
                break;
            case 3:
                READ_CMICx_INTC_INTR(unit, ICFG_CHIP_LP_INTR_RAW_STATUS_REG3r,
                                     &cmic_rval);
                if (cmic_rval == 0) {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "unit %d, _soc_tomahawk_ser_process_all: will not process intr because cmic_rval is 0\n"),
                         unit));
                    return SOC_E_NONE;
                }
                break;
            default:
                return SOC_E_NONE;
        }
    }
#endif

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            rb = &_soc_th3_ser_route_blocks[rbi];
        } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            rb = &_soc_th2_ser_route_blocks[rbi];
        } else
#endif
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
        if (SOC_IS_TOMAHAWKPLUS(unit)) {
            rb = &_soc_thplus_ser_route_blocks[rbi];
        } else
#endif
        {
            rb = &_soc_th_ser_route_blocks[rbi];
        }
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (!((rb->cmic_reg == reg_type) && (cmic_bit == 1<<bit))) {
            continue;
        }
        if (!(cmic_rval & cmic_bit)) {
            /* Indicated bit not asserted for the route block */
            LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "unit %d, _soc_tomahawk_ser_process_all: cmic_rval %8x, indicated cmic_bit %8x not asserted for the route block %d, so exiting\n"),
                 unit, cmic_rval, cmic_bit, rbi));
            break;
        }
        /* For TH family only IP/EP memories use soc_tomahawk_process_ser_fifo
           to recover the error. For TH2, IP/EP interrupt bits are in a same intr
           register, so just need to do AND operation with related mask. But for
           TH3, the new fifo EDB_SER_FIFO is added, and the interrupt bits for
           IP/EP belong to different intr register, so besides checking the
           cmic_bit, we should also check the cmic_reg number.*/
        if ((rb->blocktype == SOC_BLK_IPIPE || rb->blocktype == SOC_BLK_EPIPE) &&
             _soc_tomahawk_ser_check_fifo_intr(unit, rb->cmic_bit, rb->cmic_reg)) {
            /* New fifo style processing */
            (void)soc_tomahawk_process_ser_fifo(unit, rb->blocktype, rb->pipe,
                                                prefix_str, 0, reg_type, rb->cmic_bit);
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
                /* ??? Because entry for mmu in _soc_th_ser_route_blocks array uses
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
                 * so we will never call _soc_tomahawk_process_ser,
                 * and hence process_mmu_err at all !!
                 */
                if (COMPILER_64_IS_ZERO(rb_rval64)) {
                    continue;
                }
            }
            processed_an_intr = 1;
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_process_ser(unit, block_info_idx, rb->id,
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
soc_tomahawk_ser_error(void *unit_vp, void *d1, void *d2, void *d3,
                       void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    (void)_soc_tomahawk_ser_process_all(unit, PTR_TO_INT(d3), PTR_TO_INT(d4));
    sal_usleep(SAL_BOOT_QUICKTURN ? 1000000 : 10000);

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        soc_ser_top_intr_reg_enable(unit, PTR_TO_INT(d3), PTR_TO_INT(d4), 1);
        soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
    } else
#endif
    {
        switch (PTR_TO_INT(d3)) {
#ifdef BCM_CMICM_SUPPORT
        case 3: (void)soc_cmicm_intr3_enable(unit, PTR_TO_INT(d2));
            break;
        case 4: (void)soc_cmicm_intr4_enable(unit, PTR_TO_INT(d2));
            break;
        case 5: (void)soc_cmicm_intr5_enable(unit, PTR_TO_INT(d2));
            break;
#ifdef BCM_TOMAHAWK2_SUPPORT
        case 6: (void)soc_cmicm_intr6_enable(unit, PTR_TO_INT(d2));
            break;
#endif
        default:
            break;
#endif
        }
    }
}


static _soc_generic_ser_info_t _soc_th_tcam_ser_info_template[] = {
    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* Global view */
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* End of list */
    { INVALIDm },
};

#ifdef BCM_TOMAHAWK2_SUPPORT
static _soc_generic_ser_info_t _soc_th2_tcam_ser_info_template[] = {
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DUPLICATE, so need to scan each pipe separately */
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* default_acc_type = DATA_SPLIT, but still need to scan each pipe separately */
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },

    /* Global view */
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {1, 145}, {146, 289}, {147, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 236}, {1, 236}, {237, 472}, {238, 472} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 236}, {1, 236}, {237, 472}, {238, 472} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 236}, {1, 236}, {237, 472}, {238, 472} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 236}, {1, 236}, {237, 472}, {238, 472} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 236}, {1, 236}, {237, 472}, {238, 472} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_0},
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* End of list */
    { INVALIDm },
};

static _soc_generic_ser_info_t _soc_th2_b0_tcam_ser_info_template[] = {
    /* Global view */
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* Global view */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_MEM_ADDR_ACC_TYPE_PIPE_0 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_1 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_2 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_CONFIG_SKIP |
      _SOC_MEM_ADDR_ACC_TYPE_PIPE_3 },
    /* Unique view - initially disabled */
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_DISCARD_READ | _SOC_SER_FLAG_MULTI_PIPE |
      _SOC_SER_FLAG_XY_READ |
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },

    /* End of list */
    { INVALIDm },
};
#endif

static _soc_generic_ser_info_t *_soc_th_tcam_ser_info[SOC_MAX_NUM_DEVICES];
#if defined(BCM_TOMAHAWK2_SUPPORT)
STATIC void
_soc_th2_ser_info_flag_update(_soc_generic_ser_info_t *ser_info,
                              soc_mem_t mem, uint32 flag, int enable)
{
    int ser_idx = 0;
    /*This will update the flag of per-pipe memory too.*/
    while (INVALIDm != ser_info[ser_idx].mem) {
        if (ser_info[ser_idx].mem == mem) {
            if (enable) {
                ser_info[ser_idx].ser_flags |= flag;
            } else {
                ser_info[ser_idx].ser_flags &= ~flag;
            }
        }
        ser_idx++;
    }
}
#endif /* BCM_TOMAHAWK2_SUPPORT */

int
soc_tomahawk_tcam_ser_init(int unit)
{
    int alloc_size = 0;
    uint32 map[] = {0, 1, 2, 3};

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        /* First, make per-unit copy of the master TCAM list */
        if (!soc_feature(unit, soc_feature_tcam_scan_engine)) {
            alloc_size = sizeof(_soc_th2_tcam_ser_info_template);

            if (NULL == _soc_th_tcam_ser_info[unit]) {
                if ((_soc_th_tcam_ser_info[unit] =
                     sal_alloc(alloc_size, "th tcam list")) == NULL) {
                    return SOC_E_MEMORY;
                }
            }

            /* Make a fresh copy of the TCAM template info */
            sal_memcpy(_soc_th_tcam_ser_info[unit],
                       &(_soc_th2_tcam_ser_info_template),
                       alloc_size);
        } else {
            alloc_size = sizeof(_soc_th2_b0_tcam_ser_info_template);

            if (NULL == _soc_th_tcam_ser_info[unit]) {
                if ((_soc_th_tcam_ser_info[unit] =
                     sal_alloc(alloc_size, "th tcam list")) == NULL) {
                    return SOC_E_MEMORY;
                }
            }

            /* Make a fresh copy of the TCAM template info */
            sal_memcpy(_soc_th_tcam_ser_info[unit],
                       &(_soc_th2_b0_tcam_ser_info_template),
                       alloc_size);
        }

        if (soc_feature(unit, soc_feature_field_stage_half_slice) ||
            soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
            _soc_th2_ser_info_flag_update(_soc_th_tcam_ser_info[unit], IFP_TCAMm,
                                          _SOC_SER_FLAG_NO_DMA, TRUE);
            _soc_th2_ser_info_flag_update(_soc_th_tcam_ser_info[unit], IFP_TCAM_WIDEm,
                                          _SOC_SER_FLAG_NO_DMA, TRUE);
        }
    } else 
#endif

    {
        /* First, make per-unit copy of the master TCAM list */
        alloc_size = sizeof(_soc_th_tcam_ser_info_template);

        if (NULL == _soc_th_tcam_ser_info[unit]) {
            if ((_soc_th_tcam_ser_info[unit] =
                 sal_alloc(alloc_size, "th tcam list")) == NULL) {
                return SOC_E_MEMORY;
            }
        }

        /* Make a fresh copy of the TCAM template info */
        sal_memcpy(_soc_th_tcam_ser_info[unit],
                   &(_soc_th_tcam_ser_info_template),
                   alloc_size);
    }

    SOC_IF_ERROR_RETURN(soc_generic_ser_at_map_init(unit, map, COUNTOF(map)));
    return soc_generic_ser_init(unit, _soc_th_tcam_ser_info[unit]);
}

void
soc_tomahawk_ser_fail(int unit)
{
    soc_generic_ser_process_error(unit, _soc_th_tcam_ser_info[unit],
                                  _SOC_PARITY_TYPE_SER);
}

int
soc_tomahawk_mem_ser_control(int unit, soc_mem_t mem, int copyno,
                             int enable)
{
    if (enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                           REG_PORT_ANY, TOP_MMU_RST_Lf, 0));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                           REG_PORT_ANY, TOP_MMU_RST_Lf, 1));
        sal_usleep(1000);
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN(soc_tomahawk2_clear_mmu_memory(unit));
        } else
#endif
        {
            SOC_IF_ERROR_RETURN(soc_tomahawk_clear_mmu_memory(unit));
        }
    }

    if (enable) {
        if (soc_property_get(unit, spn_PARITY_ENABLE, FALSE)) {
            SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, enable));
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, enable));
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
soc_tomahawk_ser_register(int unit)
{
#ifdef INCLUDE_MEM_SCAN
    soc_mem_scan_ser_list_register(unit, TRUE,
                                   _soc_th_tcam_ser_info[unit]);
#endif
    memset(&_th_ser_functions, 0, sizeof(soc_ser_functions_t));
    _th_ser_functions._soc_ser_fail_f = &soc_tomahawk_ser_fail;
    _th_ser_functions._soc_ser_parity_error_cmicm_intr_f =
        &soc_tomahawk_ser_error;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    _th_ser_functions._soc_ser_parity_error_cmicx_intr_f =
        &soc_tomahawk_ser_error;
#endif
    soc_ser_function_register(unit, &_th_ser_functions);
}

int
soc_tomahawk_mem_sram_info_get(int unit, soc_mem_t mem, int index,
                                _soc_ser_sram_info_t *sram_info)
{
    int i, base, base_index, offset, base_bucket, bkt_offset;
    int entries_per_ram = 0, entries_per_bank, contiguous = 0;

    switch (mem) {
    case L2_ENTRY_ONLY_ECCm:
#ifdef BCM_TOMAHAWK3_SUPPORT
    case L2_ENTRY_ECCm:
#endif
        if (index < SOC_TH_NUM_ENTRIES_L2_BANK) {
            /* dedicated L2 entries - hash table */
            sram_info->ram_count = 1;
            entries_per_ram = SOC_TH_NUM_ENTRIES_L2_BANK;
            base = index;
        } else {
            /* hash_xor mems, stride 8K */
            /*
             * sram_info->disable_reg = L2_ENTRY_PARITY_CONTROLr;
             * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
             */
            sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
            sram_info->force_field = FORCE_XOR_GENERATIONf;
            sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITEf;
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                sram_info->ram_count = SOC_TH2_NUM_EL_SHARED;
            } else
#endif
            {
                sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
            }
                            /* #indexes to be corrected */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                entries_per_ram = SOC_TH3_NUM_ENTRIES_XOR_BANK;
            } else
#endif
            {
                entries_per_ram = SOC_TH_NUM_ENTRIES_XOR_BANK;
            }
                            /* #entries / xor_bank in a shared bank */
            entries_per_bank = entries_per_ram * sram_info->ram_count;
                            /* #entries / uft bank */
            base_index = index - SOC_TH_NUM_ENTRIES_L2_BANK;
                            /* index in uft space */
            base = (base_index/entries_per_bank)*entries_per_bank;
                            /* base addr of uft bank */
            offset = base_index % entries_per_ram;
                            /* 1st entry within uft bank */
            base += SOC_TH_NUM_ENTRIES_L2_BANK + offset;
                            /* 1st index for correction */
        }
        break;
    case L3_ENTRY_ONLY_ECCm:
#ifdef BCM_TOMAHAWK3_SUPPORT
    case L3_ENTRY_ECCm:
#endif
        if (index < SOC_TH_NUM_ENTRIES_L3_BANK(unit)) {
            /* dedicated L3 entries - hash table */
            sram_info->ram_count = 1;
            entries_per_ram = SOC_TH_NUM_ENTRIES_L3_BANK(unit);
            base = index;
        } else {
            /* hash_xor mems, stride 8K */
            /*
             * sram_info->disable_reg = L3_ENTRY_PARITY_CONTROLr;
             * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
             */
            sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
            sram_info->force_field = FORCE_XOR_GENERATIONf;
            sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITEf;
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                sram_info->ram_count = SOC_TH2_NUM_EL_SHARED;
            } else
#endif
            {
                sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
            }
                            /* #indexes to be corrected */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                entries_per_ram = SOC_TH3_NUM_ENTRIES_XOR_BANK;
            } else
#endif
            {
                entries_per_ram = SOC_TH_NUM_ENTRIES_XOR_BANK;
            }
                            /* #entries / xor_bank in a shared bank */
            entries_per_bank = entries_per_ram * sram_info->ram_count;
                            /* #entries / uft bank */
            base_index = index - SOC_TH_NUM_ENTRIES_L3_BANK(unit);
                            /* index in uft space */
            base = (base_index/entries_per_bank)*entries_per_bank;
                            /* base addr of uft bank */
            offset = base_index % entries_per_ram;
                            /* 1st entry within uft bank */
            base += SOC_TH_NUM_ENTRIES_L3_BANK(unit) + offset;
                            /* 1st index for correction */
        }
        break;
    case FPEM_ECCm:
#ifdef BCM_TOMAHAWK3_SUPPORT
    case EXACT_MATCH_ECCm:
#endif
        /* hash_xor mems, stride 8K */
        /*
         * sram_info->disable_reg = FPEM_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         */
        sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
        sram_info->force_field = FORCE_XOR_GENERATIONf;
        sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITEf;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            sram_info->ram_count = SOC_TH2_NUM_EL_SHARED;
        } else
#endif
        {
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
        }
                        /* #indexes to be corrected */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            entries_per_ram = SOC_TH3_NUM_ENTRIES_XOR_BANK;
        } else
#endif
        {
            entries_per_ram = SOC_TH_NUM_ENTRIES_XOR_BANK;
        }
                        /* #entries / xor_bank in a shared bank */
        entries_per_bank = entries_per_ram * sram_info->ram_count;
                        /* #entries / uft bank */
        base_index = index;
                        /* index - (SOC_TH_NUM_ENTRIES_FPEM_BANK=0) */
                        /* index in uft space */
        base = (base_index/entries_per_bank)*entries_per_bank;
                        /* base addr of uft bank */
        offset = base_index % entries_per_ram;
                        /* 1st entry within uft bank */
        base += offset;
                        /* base += (SOC_TH_NUM_ENTRIES_FPEM_BANK=0) + offset; */
                        /* 1st index for correction */
        break;
    case L3_DEFIP_ALPM_ECCm:
        if (!soc_tomahawk_alpm_mode_get(unit)) {
            return SOC_E_PARAM;
        }
        /*
         * sram_info->disable_reg = ILPM_SER_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         */
        sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
        sram_info->force_field = FORCE_XOR_GENERATIONf;
        sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITEf;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            sram_info->ram_count = SOC_TH2_NUM_EL_SHARED;
        } else
#endif
        {
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
        }

        if (soc_th_get_alpm_banks(unit) == 2) {
            base = index & 0x1; /* 2 uft_bank mode */
            base_bucket = ((index >> 1) & SOC_TH_ALPM_BKT_MASK(unit));
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x, uft_bank:%d\n"),
                     base_bucket, base));
            base_bucket = base_bucket % SOC_TH_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < sram_info->ram_count; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TH_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TH_ALPM_MODE1_BKT_MASK) |
                    (bkt_offset << 1) | base;
            }
        } else {
            base = index & 0x3; /* 4 uft_bank mode */
            base_bucket = ((index >> 2) & SOC_TH_ALPM_BKT_MASK(unit));
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x, uft_bank:%d\n"),
                     base_bucket, base));
            base_bucket = base_bucket % SOC_TH_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < sram_info->ram_count; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TH_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TH_ALPM_MODE0_BKT_MASK) |
                    (bkt_offset << 2) | base;
            }
        }
        return SOC_E_NONE;
#ifdef BCM_TOMAHAWK3_SUPPORT
    case L3_DEFIP_ALPM_LEVEL3_ECCm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (!soc_tomahawk3_alpm_mode_get(unit)) {
                return SOC_E_PARAM;
            }
            sram_info->force_mem = UFT_SHARED_BANKS_CONTROLm;
            sram_info->force_field = FORCE_XOR_GENf;
            sram_info->disable_xor_write_field = DISABLE_XOR_WRITEf;
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;

            base_bucket = ((index >> 2) & SOC_TH3_ALPM_BKT_MASK(unit));
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x\n"),
                     base_bucket));
            base_bucket = base_bucket % SOC_TH3_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < SOC_TH3_NUM_EL_SHARED; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TH3_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TH3_ALPM_MODE_BKT_MASK) | (bkt_offset << 2);
            }
        }
        return SOC_E_NONE;
#endif
    case FPEM_LPm:
        sram_info->disable_reg = FPEM_CONTROLr;
        sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
        /* sram_info->force_reg = ISS_MEMORY_CONTROL_84r; */
        /* sram_info->force_field = FORCE_XOR_GENERATIONf; */

#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            sram_info->ram_count = SOC_TH2_NUM_EL_SHARED;
        } else
#endif
        {
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
        }
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TH_LP_ENTRIES_IN_XOR_BANK;;
                        /* #lp_entries / lp_xor_bank of uft bank */
        base = index % entries_per_ram;
                        /* 1st lp_entry within uft bank */
        entries_per_bank = SOC_TH_NUM_ENTRIES_XOR_BANK * sram_info->ram_count;
                        /* #phy_entries / uft bank */
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            offset = entries_per_bank * (index / SOC_TH2_LP_ENTRIES_IN_UFT_BANK);
        } else
#endif
        {
            offset = entries_per_bank * (index / SOC_TH_LP_ENTRIES_IN_UFT_BANK);
        }
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
                offset = SOC_TH_LP_ENTRIES_IN_UFT_BANK *
                         (index / SOC_TH_LP_ENTRIES_IN_UFT_BANK);
                        /* of 1st_lp_entry in uft_space */
                offset += base;
                        /* of 1st_lp_entry to be corrected */
                sram_info->mem_indexes[i][0] = offset + i*entries_per_ram;
                        /* of 4th_lp_entry to be corrected */
            }
        }
        return SOC_E_NONE;
    case L2_ENTRY_LPm:
    case L3_ENTRY_LPm:
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
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            sram_info->ram_count = SOC_TH2_NUM_EL_SHARED;
        } else
#endif
        {
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
        }
        
        for (i=0; i < sram_info->ram_count; i++) {
           sram_info->view[i] = mem;
           sram_info->index_count[i] = 1;
        }
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TH_LP_ENTRIES_IN_XOR_BANK;
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            entries_per_bank = SOC_TH2_LP_ENTRIES_IN_UFT_BANK;
        } else
#endif
        {
            entries_per_bank = SOC_TH_LP_ENTRIES_IN_UFT_BANK;
        }
        base_index = index; /* index of lp_entry */
        base = (base_index/entries_per_bank)*entries_per_bank;
                        /* lp_entry0 in uft bank */
        offset = base_index % entries_per_ram;
                        /* lp_entry in 1st xor_bank */
        base += offset; /* index of 1st lp_entry */
        break;
    case VLAN_XLATE_ECCm:
    case VLAN_MACm:
        contiguous = 1;
        /*
         * sram_info->disable_reg = VLAN_XLATE_DBGCTRL_0r;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         */
        sram_info->force_reg = VLAN_XLATE_DBGCTRL_0r;
        sram_info->force_field = FORCE_XOR_GENf;
        sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITEf;
        sram_info->ram_count = SOC_TH_NUM_EL_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case EGR_VLAN_XLATE_ECCm:
        contiguous = 1;
        /*
         * sram_info->disable_reg = EGR_VLAN_XLATE_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         */
        sram_info->force_reg = EGR_VLAN_XLATE_CONTROLr;
        sram_info->force_field = FORCE_XOR_GENf;
        sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITEf;
        sram_info->ram_count = SOC_TH_NUM_EL_EGR_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case ING_L3_NEXT_HOPm:
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            sram_info->ram_count = SOC_TH2_NUM_EL_ING_L3_NEXT_HOP;
        } else
#endif
        {
            sram_info->ram_count = SOC_TH_NUM_EL_ING_L3_NEXT_HOP;
        }
        sram_info->force_reg = RSEL2_RAM_DBGCTRL5r;
        sram_info->force_field = FORCE_XOR_GEN_NHOPf;
        sram_info->disable_xor_write_field = DEBUG_DISABLE_XOR_WRITE_NHOPf;
        entries_per_ram = SOC_TH_RAM_OFFSET_ING_L3_NEXT_HOP;
        base = index % entries_per_ram;
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

int
soc_th_ifp_slice_mode_hw_get(int unit, int pipe, int slice_num, int *slice_type,
                             int *slice_enabled)
{
    soc_reg_t ifp_cfg_r;
    uint32 ifp_cfg_rval = 0;
    static soc_reg_t ifp_cfg_r_list[] = {
        IFP_CONFIG_PIPE0r,
        IFP_CONFIG_PIPE1r,
        IFP_CONFIG_PIPE2r,
        IFP_CONFIG_PIPE3r,
        IFP_CONFIG_PIPE4r,
        IFP_CONFIG_PIPE5r,
        IFP_CONFIG_PIPE6r,
        IFP_CONFIG_PIPE7r};
    int rv = 0;

    rv = soc_th_ifp_slice_mode_get(unit, pipe, slice_num,
                                   slice_type, slice_enabled);
    if (rv != SOC_E_INIT) {
       return rv;
    }

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
       *slice_type = _SOC_TH_IFP_SLICE_MODE_WIDE;
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
int soc_th_mem_is_dual_mode(int unit, soc_mem_t mem, soc_mem_t *base_mem, int *pipe)
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
        /* test for base_PIPE0,1,2,3 views */
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
soc_th_mem_index_invalid(int unit, soc_mem_t mem, int index, uint8 dual_mode,
                         soc_mem_t in_base_mem, int in_pipe)
{
    int slice_num = 0, slice_type, rv;
    int cfg_slice_enabled = 0;
    int cfg_slice_type = 0;
    int mem_mode = _SOC_SER_MEM_MODE_GLOBAL;
    soc_mem_t base_mem = INVALIDm;
    int pipe = -1, mode_ok = 0;
    int8 dual_mode_mem;

    if (0xFF == dual_mode) {
        dual_mode_mem = soc_th_mem_is_dual_mode(unit, mem, &base_mem, &pipe);
    } else {
        base_mem = in_base_mem;
        pipe = in_pipe;
        dual_mode_mem = dual_mode;
    }

    if (dual_mode_mem) {
        if (!SOC_FAILURE(soc_th_check_hw_global_mode(unit, base_mem,
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
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                if ((index >= 0) && (index < 1536)) {
                    slice_num = index/256;
                } else if ((index >= 1536) && (index < 3072)) {
                    slice_num = ((index - 1536) / 512) + 5;
                }
                slice_type = _SOC_TH_IFP_SLICE_MODE_WIDE;

                if ((slice_num < 0) || (slice_num >= 9)) {
                    return TRUE; /* invalid - skip mem_acc */
                }
            } else
#endif
            {
                /* IFP_TCAMm or IFP_TCAM_WIDEm */
                if (base_mem == IFP_TCAMm) {
                    slice_num = index/512; slice_type = 0;
                } else {
                    slice_num = index/256; slice_type = 1;
                }
                if ((slice_num < 0) || (slice_num >= 12)) {
                    return TRUE; /* invalid - skip mem_acc */
                }
            }

            rv = soc_th_ifp_slice_mode_hw_get(unit, pipe, slice_num,
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
                    "pipe: %d, index %d, slice_num %d rv %d!!\n"),
                           unit, SOC_MEM_NAME(unit, mem), pipe, index, slice_num, rv));
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
 * Cannot use: soc_th_field_mem_mode_get during WB
 * Note: Bit positions are HARD-CODED to match with order of entries in
         _soc_generic_ser_info_t _soc_th_tcam_ser_info_template[]
         MUST change these values if we change above array !!
 */
int
soc_th_check_hw_global_mode(int unit, soc_mem_t mem, int *mem_mode)
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
soc_th_ser_mem_mode_get(int unit, soc_mem_t mem, int *mem_mode)
{
    int rv = SOC_E_NONE;

    /* first query field module */
    rv = soc_th_field_mem_mode_get(unit, mem, mem_mode);

    switch (rv) {
    case SOC_E_INIT: /* BCM_E_INIT: field module is de-initialized */
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, field_mem_mode_get failed: mem %s "
                        "Will consult soc_th_check_hw_global_mode !!\n"),
             unit, SOC_MEM_NAME(unit, mem)));
        switch (mem) {
        case VFP_TCAMm:
        case VFP_POLICY_TABLEm:
            rv = soc_th_check_hw_global_mode(unit,
                                             VFP_TCAMm,
                                             mem_mode);
            break;

        case EFP_TCAMm:
        case EFP_POLICY_TABLEm:
        case EFP_COUNTER_TABLEm:
        case EFP_METER_TABLEm:
            rv = soc_th_check_hw_global_mode(unit,
                                             EFP_TCAMm,
                                             mem_mode);
            break;

        case IFP_LOGICAL_TABLE_SELECTm:
        case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
        case IFP_TCAMm:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case IFP_LOGICAL_TABLE_ACTION_PRIORITYm:
        case IFP_KEY_GEN_PROGRAM_PROFILEm:
        case IFP_KEY_GEN_PROGRAM_PROFILE2m:
        case IFP_POLICY_TABLEm:
        case IFP_METER_TABLEm:
            rv = soc_th_check_hw_global_mode(unit,
                                             IFP_LOGICAL_TABLE_SELECTm, /* or IFP_TCAMm */
                                             mem_mode);
            break;

        case FP_UDF_OFFSETm:
        case FP_UDF_TCAMm:
            rv = soc_th_check_hw_global_mode(unit,
                                             FP_UDF_TCAMm,
                                             mem_mode);
            break;

        case EXACT_MATCH_LOGICAL_TABLE_SELECTm:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
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
        case FPEM_ECCm:
            rv = soc_th_check_hw_global_mode(unit,
                                             EXACT_MATCH_LOGICAL_TABLE_SELECTm,
                                             mem_mode);
            break;

        case SRC_COMPRESSIONm:
        case SRC_COMPRESSION_TCAM_ONLYm:
        case SRC_COMPRESSION_DATA_ONLYm:
            /* stage = bcmFieldQualifyStageClass; */
            /* stage_id = _BCM_FIELD_STAGE_CLASS */
            rv = soc_th_check_hw_global_mode(unit,
                                             SRC_COMPRESSIONm,
                                             mem_mode);
            break;

        case DST_COMPRESSIONm:
        case DST_COMPRESSION_TCAM_ONLYm:
        case DST_COMPRESSION_DATA_ONLYm:
            /* stage = bcmFieldQualifyStageClass; */
            /* stage_id = _BCM_FIELD_STAGE_CLASS */
            rv = soc_th_check_hw_global_mode(unit,
                                             DST_COMPRESSIONm,
                                             mem_mode);
            break;

        case TTL_FNm:
        case TOS_FNm:
        case TCP_FNm:
        case IP_PROTO_MAPm:
            /* stage = bcmFieldQualifyStageClassExactMatch; */
            /* stage_id = _BCM_FIELD_STAGE_CLASS */
            rv = soc_th_check_hw_global_mode(unit,
                                             DST_COMPRESSIONm,
                                             mem_mode);
            break;

        case IFP_TCAM_WIDEm:
            rv = soc_th_check_hw_global_mode(unit,
                                             IFP_TCAM_WIDEm,
                                             mem_mode);
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
        }
        break; /*field module is de-initialized */

    default: /* field module response - may/may_not_be SOC_E_NONE */
        break;
    }

    return rv;
}

STATIC int
_soc_th_ifp_slice_mode_check(int unit, soc_mem_t mem, int slice, int *slice_skip)
{

    soc_reg_t reg;
    uint32 rval;
    int exp_slice_width_narrow, slice_width_narrow, slice_enabled;

    if (NULL == slice_skip) {
        return SOC_E_PARAM;
    }
    *slice_skip = 0;
    switch (mem) {
    case IFP_TCAMm:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIGr;
        break;
    case IFP_TCAM_PIPE0m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE0r;
        break;
    case IFP_TCAM_PIPE1m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE1r;
        break;
    case IFP_TCAM_PIPE2m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE2r;
        break;
    case IFP_TCAM_PIPE3m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE3r;
        break;
    case IFP_TCAM_PIPE4m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE4r;
        break;
    case IFP_TCAM_PIPE5m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE5r;
        break;
    case IFP_TCAM_PIPE6m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE6r;
        break;
    case IFP_TCAM_PIPE7m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_NARROW;
        reg = IFP_CONFIG_PIPE7r;
        break;
    case IFP_TCAM_WIDEm:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIGr;
        break;
    case IFP_TCAM_WIDE_PIPE0m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIG_PIPE0r;
        break;
    case IFP_TCAM_WIDE_PIPE1m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIG_PIPE1r;
        break;
    case IFP_TCAM_WIDE_PIPE2m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIG_PIPE2r;
        break;
    case IFP_TCAM_WIDE_PIPE3m:
        exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
        reg = IFP_CONFIG_PIPE3r;
        break;
    default:
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, slice, &rval));

    if (soc_feature(unit, soc_feature_ifp_no_narrow_mode_support)) {
       slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
       exp_slice_width_narrow = _SOC_TH_IFP_SLICE_MODE_WIDE;
    } else {
       slice_width_narrow = soc_reg_field_get(unit, reg, rval, IFP_SLICE_MODEf);
    }

    slice_enabled = soc_reg_field_get(unit, reg, rval, IFP_SLICE_ENABLEf);
    if (!slice_enabled || (exp_slice_width_narrow != slice_width_narrow)) {
        *slice_skip = 1;
    }
    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit, "_soc_th_ifp_slice_mode_check: For mem %s, read reg %s, slice_enabled = %0d, slice_width = %0d, slice_skip = %0d\n"),
        SOC_MEM_NAME(unit, mem), SOC_REG_NAME(unit, reg), slice_enabled,
        slice_width_narrow, *slice_skip));
    return SOC_E_NONE;
}

/*
 * This function creates an array of slice sizes and also returns
 * the slice_count per device
 */
int
soc_th_ifp_tcam_slice_size_calculate(int unit, soc_mem_t mem,
                                     int *slice_count,
                                     int *slice_size_arr)
{
    int slice_idx = 0;
    int rv = SOC_E_NONE;

    if (NULL == slice_count) {
        return SOC_E_PARAM;
    }

    if (NULL == slice_size_arr) {
        return SOC_E_PARAM;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        *slice_count = 9;
        for (slice_idx = 0; slice_idx < *slice_count; slice_idx++) {
             slice_size_arr[slice_idx] = (slice_idx < 6) ? 256 : 512;
        }
    } else
#endif
    {
        *slice_count = 12;
        switch (mem) {
            case IFP_TCAMm:
            case IFP_TCAM_PIPE0m:
            case IFP_TCAM_PIPE1m:
            case IFP_TCAM_PIPE2m:
            case IFP_TCAM_PIPE3m:
            case IFP_TCAM_PIPE4m:
            case IFP_TCAM_PIPE5m:
            case IFP_TCAM_PIPE6m:
            case IFP_TCAM_PIPE7m:
                for (slice_idx = 0; slice_idx < *slice_count; slice_idx++) {
                     slice_size_arr[slice_idx] = 512;
                }
                break;
            case IFP_TCAM_WIDEm:
            case IFP_TCAM_WIDE_PIPE0m:
            case IFP_TCAM_WIDE_PIPE1m:
            case IFP_TCAM_WIDE_PIPE2m:
            case IFP_TCAM_WIDE_PIPE3m:
                for (slice_idx = 0; slice_idx < *slice_count; slice_idx++) {
                     slice_size_arr[slice_idx] = 256;
                }
                break;
            default:
                rv = SOC_E_PARAM;
        }
    }
    return rv;
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
 *    we cannot use soc_th_ifp_slice_mode_get() which will be eventually
 *    replaced by field' module.
 *
 * 3. used for scrubbing IFP_TCAM, IFP_TCAM_WIDE views before CPU goes offline
 *    We still cannot use standard soc_th_ifp_slice_mode_get() as defined earlier,
 *    because exit_clean in appl/diag/shell.c first invokes _bcm_shutdown
 *    (which de-inits field module) and then soc_shutdown (which calls this
 *    function)
 */
int
soc_th_ifp_tcam_dma_read(int unit, soc_mem_t mem, int blk, uint32 *table,
                         int en_per_pipe_read, uint8 *vmap)
{
    int slice, slice_skip = 0;
    int slice_start_addr, slice_end_addr, entry_dw;
    int rv = SOC_E_NONE;
    int slice_count = 0;
    int slice_size_arr[12] = { 0 };

    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return SOC_E_PARAM;
    }
    if (NULL == table) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
       soc_th_ifp_tcam_slice_size_calculate(unit, mem,
                                            &slice_count,
                                            slice_size_arr));

    entry_dw = soc_mem_entry_words(unit, mem);

    slice_start_addr = soc_mem_index_min(unit, mem);
    for (slice = 0; slice < slice_count; slice++) {
        slice_end_addr = slice_start_addr + slice_size_arr[slice] - 1;
        SOC_IF_ERROR_RETURN(
            _soc_th_ifp_slice_mode_check(unit, mem, slice,
                                         &slice_skip));
        if (!slice_skip) {
            if (en_per_pipe_read) {
                int i;
                uint32 ser_flags;
                int acc_type_list[] = {
                    _SOC_TH_ACC_TYPE_UNIQUE_PIPE0,
                    _SOC_TH_ACC_TYPE_UNIQUE_PIPE1,
                    _SOC_TH_ACC_TYPE_UNIQUE_PIPE2,
                    _SOC_TH_ACC_TYPE_UNIQUE_PIPE3};
                for (i = 0; i < NUM_PIPE(unit); i++) {
                    ser_flags = 0;
                    ser_flags |= _SOC_SER_FLAG_DISCARD_READ;
                    ser_flags |= _SOC_SER_FLAG_MULTI_PIPE;
                    ser_flags |= acc_type_list[i];
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "soc_th_ifp_tcam_dma_read: will read slice %0d for mem %s, pipe %d\n"),
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
                                    "soc_th_ifp_tcam_dma_read: will read slice %0d for mem %s\n"),
                         slice, SOC_MEM_NAME(unit, mem)));
                if (soc_mem_read_range(unit, mem, blk,
                                       slice_start_addr, slice_end_addr,
                                       &table[slice_start_addr * entry_dw]) < 0) {
                    rv = SOC_E_FAIL;
                }
            }
        } else {
            if (vmap != NULL) {
                sal_memset(&vmap[slice_start_addr / 8], 0, (slice_end_addr + 7 - slice_start_addr) / 8);
            }
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "soc_th_ifp_tcam_dma_read: skipped slice %0d for mem %s\n"),
                     slice, SOC_MEM_NAME(unit, mem)));
        }
        slice_start_addr = slice_end_addr + 1;
    }

    return rv;
}

static int
_soc_th_ifp_tcam_index_valid(int unit, soc_mem_t mem, int index) {
    int rv = SOC_E_NONE;
    int slice_start_addr, slice_end_addr;
    int slice, slice_skip = 1;
    int slice_count = 0;
    int slice_size_arr[12] = { 0 };

    SOC_IF_ERROR_RETURN(
       soc_th_ifp_tcam_slice_size_calculate(unit, mem,
                                            &slice_count,
                                            slice_size_arr));

    slice_start_addr = soc_mem_index_min(unit, mem);

    for (slice = 0; slice < slice_count && index >= slice_start_addr; slice++) {

        slice_end_addr = slice_start_addr + slice_size_arr[slice] - 1;

        if (index >= slice_start_addr && index <= slice_end_addr) {
            rv = _soc_th_ifp_slice_mode_check(unit, mem, slice, &slice_skip);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "There is failure during slice mode check, slice %0d for mem %s(start 0x%x, end 0x%x)\n"),
                         slice, SOC_MEM_NAME(unit, mem), slice_start_addr, slice_end_addr));
                return FALSE;
            }
            if (slice_skip == 0) { /* slice can be read */
                return TRUE;
            }
        }
        slice_start_addr = slice_end_addr + 1;
    }

    return FALSE;
}

int
soc_th_dual_tcam_index_valid(int unit, soc_mem_t mem, int index) {
    soc_mem_t base_mem;
    int pipe;

    if (FALSE == soc_th_mem_is_dual_mode(unit, mem, &base_mem, &pipe)) {
        return TRUE; /* valid index */
    }

    if (base_mem == IFP_TCAMm || base_mem == IFP_TCAM_WIDEm) {
        return _soc_th_ifp_tcam_index_valid(unit, mem, index);
    }
    /* add other dual mode memories here */

    return TRUE; /* valid index */
}

int
soc_th_ifp_tcam_range_dma_read(int unit, soc_mem_t mem, int blk,
                         int start_addr, uint32 num_entry, uint32 *table)
{
    int slice, slice_skip = 0;
    int slice_start_addr, slice_end_addr, entry_dw;
    int min_addr, max_addr, end_addr;
    int rv = SOC_E_NONE;
    int slice_size_arr[12] = { 0 };
    int slice_count = 0;

    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return SOC_E_PARAM;
    }
    if (NULL == table) {
        return SOC_E_PARAM;
    }
    min_addr = soc_mem_index_min(unit, mem);
    max_addr = soc_mem_index_max(unit, mem);
    end_addr = start_addr + num_entry;
    if (min_addr > start_addr || end_addr > max_addr) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
       soc_th_ifp_tcam_slice_size_calculate(unit, mem,
                                            &slice_count,
                                            slice_size_arr));

    entry_dw = soc_mem_entry_words(unit, mem);

    slice_start_addr = min_addr;

    for (slice = 0; slice < slice_count; slice++) {
        slice_end_addr = slice_start_addr + slice_size_arr[slice] - 1;

        if (start_addr > slice_end_addr) {
            slice_start_addr = slice_end_addr + 1;
            continue;
        }

        SOC_IF_ERROR_RETURN(
            _soc_th_ifp_slice_mode_check(unit, mem, slice,
                                         &slice_skip));
        if (slice_skip) {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Skipped slice %0d for mem %s(start 0x%x, end 0x%x)\n"),
                     slice, SOC_MEM_NAME(unit, mem), slice_start_addr, slice_end_addr));
            slice_start_addr = slice_end_addr + 1;
            continue;
        }

        if (start_addr > slice_start_addr) {
            slice_start_addr = start_addr;
        }
        if (end_addr < slice_end_addr) {
            slice_end_addr = end_addr;
        }

        if (soc_mem_read_range(unit, mem, blk,
                               slice_start_addr, slice_end_addr,
                               &table[(slice_start_addr - start_addr) * entry_dw]) < 0) {
            rv = SOC_E_FAIL;
            LOG_ERROR(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "Read slice %0d of mem %s(start 0x%x, end 0x%x) unsuccessfully\n"),
                 slice, SOC_MEM_NAME(unit, mem), slice_start_addr, slice_end_addr));
            return rv;
        } else {
            LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "Read slice %0d of mem %s(start 0x%x, end 0x%x) successfully\n"),
                     slice, SOC_MEM_NAME(unit, mem), slice_start_addr, slice_end_addr));
        }
        if (end_addr <= slice_end_addr) {
            break;
        }
        slice_start_addr = slice_end_addr + 1;
    }

    return rv;
}

int
soc_th_check_cache_skip(int unit, soc_mem_t mem)
{
    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return 1;
    }
    
    switch (mem) {
        case L3_TUNNEL_DATA_ONLYm:

        case DST_COMPRESSION_DATA_ONLYm:
        case DST_COMPRESSION_DATA_ONLY_PIPE0m:
        case DST_COMPRESSION_DATA_ONLY_PIPE1m:
        case DST_COMPRESSION_DATA_ONLY_PIPE2m:
        case DST_COMPRESSION_DATA_ONLY_PIPE3m:

        case SRC_COMPRESSION_DATA_ONLYm:
        case SRC_COMPRESSION_DATA_ONLY_PIPE0m:
        case SRC_COMPRESSION_DATA_ONLY_PIPE1m:
        case SRC_COMPRESSION_DATA_ONLY_PIPE2m:
        case SRC_COMPRESSION_DATA_ONLY_PIPE3m:

        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m:

        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m:
        case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m:

        case EXACT_MATCH_2_ENTRY_ONLYm:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE1m:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE2m:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE3m:

        case EXACT_MATCH_4_ENTRY_ONLYm:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE1m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE2m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE3m:

        case L2_ENTRY_ONLY_TILEm:
        case L2_ENTRY_TILEm:
        case L2_ENTRY_ONLY_ECCm:
        case L3_ENTRY_ONLY_ECCm:
        case L3_DEFIP_ALPM_ECCm:
        case FPEM_ECCm:
        case FPEM_ECC_PIPE0m:
        case FPEM_ECC_PIPE1m:
        case FPEM_ECC_PIPE2m:
        case FPEM_ECC_PIPE3m:
        case MPLS_ENTRY_ECCm:
        case VLAN_XLATE_ECCm:
        case EGR_VLAN_XLATE_ECCm:

        case FPEM_LPm:
        case L2_ENTRY_ISS_LPm:
        case L3_ENTRY_ISS_LPm:

        case EGR_FRAGMENT_ID_TABLEm:
        case EGR_FRAGMENT_ID_TABLE_PIPE0m:
        case EGR_FRAGMENT_ID_TABLE_PIPE1m:
        case EGR_FRAGMENT_ID_TABLE_PIPE2m:
        case EGR_FRAGMENT_ID_TABLE_PIPE3m:
#ifdef BCM_TOMAHAWK3_SUPPORT
        case L3_DEFIP_ALPM_LEVEL2_ECCm:
        case L3_DEFIP_ALPM_LEVEL3_ECCm:
#endif

            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "cache_skip: skipping mem %s (soc_th_check_cache_skip)\n"), SOC_MEM_NAME(unit, mem)));
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
soc_th_check_scrub_skip(int unit, soc_mem_t mem, int check_hw_global_mode)
{
    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return 1; /* skip */
    }
    if (soc_mem_is_cam(unit, mem) &&
        SOC_TH_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem)) {
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
            rv = soc_th_check_hw_global_mode(unit, mem, &mem_mode);
                /* field module may already be de-initialized */
        } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
            /* This function is invoked during system initialization and shutdown.
               Before calling this function, field module is already detached. So
               don't need to get the mem_mode, just use default GLOBAL mode.
            */
            if (SOC_IS_TOMAHAWK3(unit)) {
                rv = SOC_E_NONE;
            } else
#endif
            {
                rv = soc_th_field_mem_mode_get(unit, mem, &mem_mode);
                /* field module is active */
            }
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
        case EXACT_MATCH_2_PIPE2m:
        case EXACT_MATCH_2_PIPE3m:

        case EXACT_MATCH_4m:
        case EXACT_MATCH_4_PIPE0m:
        case EXACT_MATCH_4_PIPE1m:
        case EXACT_MATCH_4_PIPE2m:
        case EXACT_MATCH_4_PIPE3m:

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
        case DST_COMPRESSION_DATA_ONLY_PIPE2m:
        case DST_COMPRESSION_DATA_ONLY_PIPE3m:
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
        case SRC_COMPRESSION_DATA_ONLY_PIPE2m:
        case SRC_COMPRESSION_DATA_ONLY_PIPE3m:
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
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m:
        case IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m:
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
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE2m:
        case EXACT_MATCH_2_ENTRY_ONLY_PIPE3m:

        case EXACT_MATCH_4_ENTRY_ONLYm:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE0m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE1m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE2m:
        case EXACT_MATCH_4_ENTRY_ONLY_PIPE3m:

    /* global views which already have _PIPE0-PIPE3 views */

        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: skipping mem %s (soc_th_check_scrub_skip)\n"), SOC_MEM_NAME(unit, mem)));
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
soc_th_mem_is_eligible_for_scan(int unit, soc_mem_t mem)
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
soc_th_mem_scan_info_get(int unit, soc_mem_t mem, int block,
                         int *num_pipe, int *ser_flags)
{
    int k, num_inst_to_scrub = 0, acc_type_list[8], copyno = block;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        (void)soc_th3_check_scrub_info(unit, mem, block, copyno,
                                     &num_inst_to_scrub, acc_type_list);
    } else
#endif
    {
        (void) soc_th_check_scrub_info(unit, mem, block, copyno,
                                       &num_inst_to_scrub,
                                       acc_type_list);
    }
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
        *num_pipe = num_inst_to_scrub;
        for (k = 0; k < num_inst_to_scrub; k++) {
            ser_flags[k] = 0;
            ser_flags[k] |= _SOC_SER_FLAG_DISCARD_READ;
            ser_flags[k] |= _SOC_SER_FLAG_MULTI_PIPE;
            ser_flags[k] |= acc_type_list[k];
            if (!soc_mem_dmaable(unit, mem, block)) {
                ser_flags[k] |= _SOC_SER_FLAG_NO_DMA;
            }
        }
    }
}

int
soc_th_check_scrub_info(int unit, soc_mem_t mem, int blk, int copyno,
                        int *num_inst_to_scrub, int *acc_type_list)
{
    int mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    int mmu_base_type, mmu_base_index;
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

    /* L3_DEFIPm/L3_DEFIP_PAIR_128m should be scrubed */
    if ((mem == L3_DEFIPm) || (mem == L3_DEFIP_PAIR_128m)) {
        *num_inst_to_scrub = NUM_PIPE(unit);
        acc_type_list[0] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE1;
        acc_type_list[2] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE2;
        acc_type_list[3] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE3;
    }

    if (((mem_acc_type == _SOC_TH_ACC_TYPE_DUPLICATE) &&
         ((blk_type == SOC_BLK_IPIPE) || (blk_type == SOC_BLK_EPIPE)) &&
         (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL)) ||
        SOC_TH_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem)) {
        /* 2nd last term: sw has not already created per-instance unique views */
        *num_inst_to_scrub = NUM_PIPE(unit);
        acc_type_list[0] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE1;
        acc_type_list[2] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE2;
        acc_type_list[3] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE3;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d\n"),
                    SOC_MEM_NAME(unit,mem), blk, blk_type,
                    _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                    *num_inst_to_scrub));
        return SOC_E_NONE;
    } /* need_per_pipe_scrub */

    if ((mem_acc_type == _SOC_TH_ACC_TYPE_DUPLICATE) &&
        ((blk_type == SOC_BLK_MMU_GLB) ||
         (blk_type == SOC_BLK_MMU_XPE) ||
         (blk_type == SOC_BLK_MMU_SC))) {

        mmu_base_type = SOC_MEM_BASE_TYPE(unit, mem);

        
        switch (blk_type) {
        case SOC_BLK_MMU_XPE:
            switch (mmu_base_type) {
            case SOC_TH_MMU_BASE_TYPE_IPIPE:
                mmu_base_index = _SOC_MMU_BASE_INDEX(unit, mem, 3);
                if ((mmu_base_index == 0) || (mmu_base_index == 3)) {
                    /* ipipe0,3 serviced by xpe0,1 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 0;
                    acc_type_list[1] = 1;
                } else {
                    /* ipipe1,2 serviced by xpe2,3 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 2;
                    acc_type_list[1] = 3;
                }
                break;
            case SOC_TH_MMU_BASE_TYPE_EPIPE:
                mmu_base_index = _SOC_MMU_BASE_INDEX(unit, mem, 3);
                if ((mmu_base_index == 0) || (mmu_base_index == 1)) {
                    /* epipe0,1 serviced by xpe0,2 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 0;
                    acc_type_list[1] = 2;
                } else {
                    /* epipe2,3 serviced by xpe1,3 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 1;
                    acc_type_list[1] = 3;
                }
                break;
            case SOC_TH_MMU_BASE_TYPE_CHIP:
                /* must read each xpe instance */
                *num_inst_to_scrub = 4;
                acc_type_list[0] = 0;
                acc_type_list[1] = 1;
                acc_type_list[2] = 2;
                acc_type_list[3] = 3;
                break;
            case SOC_TH_MMU_BASE_TYPE_SLICE:
                mmu_base_index = _SOC_MMU_BASE_INDEX(unit, mem, 1);
                if (mmu_base_index == 0) {
                    /* slice0 serviced by xpe0,2 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 0;
                    acc_type_list[1] = 2;
                } else {
                    /* slice1 serviced by xpe1,3 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 1;
                    acc_type_list[1] = 3;
                }
                break;
            case SOC_TH_MMU_BASE_TYPE_LAYER:
                mmu_base_index = _SOC_MMU_BASE_INDEX(unit, mem, 1);
                if (mmu_base_index == 0) {
                    /* layer_A serviced by xpe0,1 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 0;
                    acc_type_list[1] = 1;
                } else {
                    /* layer_B serviced by xpe2,3 */
                    *num_inst_to_scrub = 2;
                    acc_type_list[0] = 2;
                    acc_type_list[1] = 3;
                }
                break;
            default: /* IPORT, EPORT, XPE */
                /* should not find any TH memories here */
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                          SOC_MEM_NAME(unit,mem), blk, blk_type,
                          _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                          *num_inst_to_scrub, mmu_base_type));
                return SOC_E_FAIL;
            } /* mmu_base_type */
            break; /* case: SOC_BLK_MMU_XPE */

        default: /* SOC_BLK_MMU_GLB,  SOC_BLK_MMU_SC */
            /* should not find any TH memories here */
            LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                      SOC_MEM_NAME(unit,mem), blk, blk_type,
                      _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                      *num_inst_to_scrub, mmu_base_type));
            return SOC_E_FAIL;
        } /* blk_type */
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                    SOC_MEM_NAME(unit,mem), blk, blk_type,
                    _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
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
soc_th_lp_to_fv_index_remap(int unit, soc_mem_t mem, int *rpt_index)
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
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
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
                        "unit %d, th_lp_to_fv_index_remap: "
                        "mem %s, em_lp_bank %d, em_lp_addr %0d \n"),
             unit, SOC_MEM_NAME(unit, mem), em_lp_bank, em_lp_addr));
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:
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
                        "unit %d, th_lp_to_fv_index_remap: "
                        "mem %s, em_lp_bank %d, em_lp_addr %0d \n"),
             unit, SOC_MEM_NAME(unit, mem), em_lp_bank, em_lp_addr));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "soc_th_lp_to_fv_index_remap: mem %s is not folded hash table !! index %d\n"),
                   SOC_MEM_NAME(unit, mem), index));
        rv = SOC_E_PARAM;
    }

    LOG_WARN(BSL_LS_SOC_SER,
        (BSL_META_U(unit,
                    "unit %d, th_lp_to_fv_index_remap: "
                    "mem %s, rpt_lp_index %0d, fv_index %0d \n"),
         unit, SOC_MEM_NAME(unit, mem), *rpt_index, index));
    *rpt_index = index;
    return rv;
}

STATIC
int
_soc_th_reg32_par_en_modify(int unit, soc_reg_t reg, soc_field_t field,
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
                    LOG_WARN(BSL_LS_SOC_SER,
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

static int
soc_th_mem_parity_set(int unit, soc_mem_t mem, int copyno,
                      _soc_mem_ser_en_info_t *ser_info_list,
                      int en, int en_1b)
{
    int i = 0;

    for (i = 0; INVALIDm != ser_info_list[i].mem; i++) {
        if (ser_info_list[i].mem == mem) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                SOC_IF_ERROR_RETURN(
                    _soc_th3_reg32_par_en_modify(unit,
                                                ser_info_list[i].en_ctrl.ctrl_type.en_reg,
                                                ser_info_list[i].en_ctrl.en_fld,
                                                en));
                SOC_IF_ERROR_RETURN(
                    _soc_th3_reg32_par_en_modify(unit,
                                                ser_info_list[i].ecc1b_ctrl.ctrl_type.en_reg,
                                                ser_info_list[i].ecc1b_ctrl.en_fld,
                                                en_1b));
            } else
#endif
            {
                SOC_IF_ERROR_RETURN(
                    _soc_th_reg32_par_en_modify(unit,
                                                ser_info_list[i].en_ctrl.ctrl_type.en_reg,
                                                ser_info_list[i].en_ctrl.en_fld,
                                                en));
                SOC_IF_ERROR_RETURN(
                    _soc_th_reg32_par_en_modify(unit,
                                                ser_info_list[i].ecc1b_ctrl.ctrl_type.en_reg,
                                                ser_info_list[i].ecc1b_ctrl.en_fld,
                                                en_1b));
            }
            return SOC_E_NONE;
        }
    }

    return SOC_E_UNAVAIL;

}


int soc_th_mem_parity_control(int unit, soc_mem_t mem, int copyno,
                              int en, int en_1b)
{
    int ret = SOC_E_NONE;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return SOC_E_UNAVAIL;
    }

    ret = soc_th_mem_parity_set(unit, mem, copyno, SOC_IP_MEM_SER_INFO(unit), en, en_1b);
    if (ret == SOC_E_NONE) {
        return ret;
    }

    ret = soc_th_mem_parity_set(unit, mem, copyno, SOC_EP_MEM_SER_INFO(unit), en, en_1b);
    if (ret == SOC_E_NONE) {
        return ret;
    }

    ret = soc_th_mem_parity_set(unit, mem, copyno, SOC_MMU_MEM_SER_INFO(unit), en, en_1b);
    if (ret == SOC_E_NONE) {
        return ret;
    }

    return SOC_E_UNAVAIL;
}

#define MEM_SER_INFO_COUNT 3
/*
 * Function:
 *      soc_th_ser_single_bit_error_enable_set
 * Purpose:
 *      Enable/Disable single bit eror reporting for the memories which are
 *      protected by ECC logic and whoes SER_RESPONSE is SER_NONE on TD2/TD2p chips
 * Parameters:
 *      unit        - (IN) Device Number
 *      enable         - (IN) Enable/Disable
 * Returns:
 *      SOC_E_NONE if set successfully, an error otherwise
 */
int soc_th_ser_single_bit_error_enable_set(int unit, int enable)
{
    int i, value, k;
    soc_reg_t ecc_reg;
    soc_field_t ecc_fld;
    _soc_mem_ser_en_info_t *mem_ser_info;
    _soc_mem_ser_en_info_t *mem_ser_info_all[MEM_SER_INFO_COUNT] = {NULL};

    value = enable ? 1:0;

    mem_ser_info_all[0] = SOC_IP_MEM_SER_INFO(unit);
    mem_ser_info_all[1] = SOC_EP_MEM_SER_INFO(unit);
    mem_ser_info_all[2] = SOC_MMU_MEM_SER_INFO(unit);

    if (NULL == mem_ser_info_all[0]) {
        /* If this is not TH or TH+, we won't be able to get the mem info */
        return SOC_E_UNAVAIL;
    }

    for (k = 0; k < MEM_SER_INFO_COUNT; k++) {
        mem_ser_info = mem_ser_info_all[k];
        for (i = 0; mem_ser_info[i].mem != INVALIDm; i++) {
            if (!SOC_MEM_IS_VALID(unit, mem_ser_info[i].mem)) {
                continue;
            }
            /* Skip the memory whose SER_RESPONSE not equal SER_NONE*/
            if (SOC_MEM_SER_CORRECTION_TYPE(unit, mem_ser_info[i].mem) != 0) {
                continue;
            }

            ecc_reg = mem_ser_info[i].ecc1b_ctrl.ctrl_type.en_reg;
            ecc_fld = mem_ser_info[i].ecc1b_ctrl.en_fld;

            if (!soc_reg_field_valid(unit, ecc_reg, ecc_fld)) {
                continue;
            }

            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "unit %d, MEM %50s REG %30s FIELD %40s\n"),
                         unit, SOC_MEM_NAME(unit, mem_ser_info[i].mem),
                         SOC_REG_NAME(unit, ecc_reg),
                         SOC_FIELD_NAME(unit, ecc_fld)));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ecc_reg, REG_PORT_ANY,
                                        ecc_fld, value));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th_ser_single_bit_error_enable_get
 * Purpose:
 *      Get the status of single bit eror reporting for the memories which are
 *      protected by ECC logic and whoes SER_RESPONSE is SER_NONE on TD2/TD2p chips
 * Parameters:
 *      unit        - (IN) Device Number
 *      enable         - (OUT) Enable/Disable
 * Returns:
 *      SOC_E_NONE if get successfully, an error otherwise
 */
int soc_th_ser_single_bit_error_enable_get(int unit, int *enable)
{
    soc_reg_t ecc_reg;
    soc_field_t ecc_fld;
    _soc_mem_ser_en_info_t *mem_ser_info = NULL;
    uint32 rval = 0, fld_value = 0, i;

    mem_ser_info = SOC_MMU_MEM_SER_INFO(unit);

    if (NULL == mem_ser_info) {
        /* If this is not TH or TH+, we won't be able to get the mem info */
        return SOC_E_UNAVAIL;
    }

    for (i = 0; mem_ser_info[i].mem != INVALIDm; i++) {
        if (!SOC_MEM_IS_VALID(unit, mem_ser_info[i].mem)) {
            continue;
        }
        /* Skip the memory whose SER_RESPONSE not equal SER_NONE*/
        if (SOC_MEM_SER_CORRECTION_TYPE(unit, mem_ser_info[i].mem) != 0) {
            continue;
        }

        ecc_reg = mem_ser_info[i].ecc1b_ctrl.ctrl_type.en_reg;
        ecc_fld = mem_ser_info[i].ecc1b_ctrl.en_fld;

        if (soc_reg_field_valid(unit, ecc_reg, ecc_fld)) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, ecc_reg, REG_PORT_ANY, 0, &rval));
            fld_value = soc_reg_field_get(unit, ecc_reg, rval, ecc_fld);
            break;
        }
    }

    *enable = fld_value ? TRUE:FALSE;
    return SOC_E_NONE;
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

#define _SER_TEST_MEM_TH_ALPM_MASK(num_uft_banks)                             \
        (SOC_IS_TOMAHAWK2(unit) ? ((num_uft_banks) == 4? 0xFFFF : 0x7FFF) :  \
         ((num_uft_banks) == 4? 0x7FFF : 0x3FFF))
#define _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks)                            \
        (SOC_IS_TOMAHAWK2(unit) ? ((num_uft_banks) == 4? 16 : 15) :          \
         ((num_uft_banks) == 4? 15 : 14))

#define _SOC_TH_TR144_LONG_SLEEP_TIME_US 200000

int
ser_test_th_mem_index_remap(int unit, ser_test_data_t *test_data,
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

    num_uft_banks = soc_th_get_alpm_banks(unit);
    switch (test_data->mem_fv) {
    case VLAN_XLATEm:
    case VLAN_MACm:
        test_data->mem = VLAN_XLATE_ECCm;
        *mem_has_ecc = 1;
        break;
    case EGR_VLAN_XLATEm:
        test_data->mem = EGR_VLAN_XLATE_ECCm;
        *mem_has_ecc = 1;
        break;
    case MPLS_ENTRYm:
        test_data->mem = MPLS_ENTRY_ECCm;
        *mem_has_ecc = 1;
        break;
    case L2Xm:
    case L2_ENTRY_ONLY_TILEm:
    case L2_ENTRY_TILEm:
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L2_ENTRY_ECCm;
        } else
#endif
        {
            test_data->mem = L2_ENTRY_ONLY_ECCm;
        }
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
        test_data->mem = L3_ENTRY_ONLY_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
        test_data->mem = L3_ENTRY_ONLY_ECCm;
        test_data->index = test_data->index_fv*2;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_IPV6_MULTICASTm:
        test_data->mem = L3_ENTRY_ONLY_ECCm;
        test_data->index = test_data->index_fv*4;
        *mem_has_ecc = 1;
        break;
    case EXACT_MATCH_2m:
    case EXACT_MATCH_2_PIPE0m:
    case EXACT_MATCH_2_PIPE1m:
    case EXACT_MATCH_2_PIPE2m:
    case EXACT_MATCH_2_PIPE3m:
    case EXACT_MATCH_2_ENTRY_ONLYm:
    case EXACT_MATCH_2_ENTRY_ONLY_PIPE0m:
    case EXACT_MATCH_2_ENTRY_ONLY_PIPE1m:
    case EXACT_MATCH_2_ENTRY_ONLY_PIPE2m:
    case EXACT_MATCH_2_ENTRY_ONLY_PIPE3m:
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = EXACT_MATCH_ECCm;
        } else
#endif
        {
            test_data->mem = FPEM_ECCm;
        }
        test_data->index = test_data->index_fv*2;
        *mem_has_ecc = 1;
        break;
    case EXACT_MATCH_4m:
    case EXACT_MATCH_4_PIPE0m:
    case EXACT_MATCH_4_PIPE1m:
    case EXACT_MATCH_4_PIPE2m:
    case EXACT_MATCH_4_PIPE3m:
    case EXACT_MATCH_4_ENTRY_ONLYm:
    case EXACT_MATCH_4_ENTRY_ONLY_PIPE0m:
    case EXACT_MATCH_4_ENTRY_ONLY_PIPE1m:
    case EXACT_MATCH_4_ENTRY_ONLY_PIPE2m:
    case EXACT_MATCH_4_ENTRY_ONLY_PIPE3m:
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = EXACT_MATCH_ECCm;
        } else
#endif
        {
            test_data->mem = FPEM_ECCm;
        }
        test_data->index = test_data->index_fv*4;
        *mem_has_ecc = 1;
        break;
    case L3_DEFIP_ALPM_IPV4m: /* 6:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv &
            _SER_TEST_MEM_TH_ALPM_MASK(num_uft_banks);
        uft_le_fv = test_data->index_fv >>
            _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break;
        case 1: uft_le_ecc = 0; break; /* or 1 */
        case 2: uft_le_ecc = 1; break;
        case 3: uft_le_ecc = 2; break;
        case 4: uft_le_ecc = 2; break; /* or 3 */
        case 5: uft_le_ecc = 3; break;
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
                _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
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
        uft_bkt_bank = test_data->index_fv &
            _SER_TEST_MEM_TH_ALPM_MASK(num_uft_banks);
        uft_le_fv = test_data->index_fv >>
            _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1 */
        case 1: uft_le_ecc = 1; break; /* or 2 */
        case 2: uft_le_ecc = 2; break; /* or 3 */
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
                _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_IPV6_128m: /* 2:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv &
            _SER_TEST_MEM_TH_ALPM_MASK(num_uft_banks);
        uft_le_fv = test_data->index_fv >>
            _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1 */
        case 1: uft_le_ecc = 2; break; /* or 3 */
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
                _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_RAWm: /* 1:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv &
            _SER_TEST_MEM_TH_ALPM_MASK(num_uft_banks);
        uft_le_fv = test_data->index_fv >>
            _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1,2,3 */
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
                _SER_TEST_MEM_TH_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
#ifdef BCM_TOMAHAWK2_SUPPORT
    case MMU_INTFO_TC2PRI_MAPPINGm:
        if (SOC_IS_TOMAHAWK2(unit)) {
            test_data->mem = MMU_INTFO_TC2PRI_MAPPING0m;
        }
        break;
    case ING_DNAT_ADDRESS_TYPEm:
        if (SOC_IS_TOMAHAWK2(unit)) {
            test_data->mem = ING_DNAT_ADDRESS_TYPE_ECCm;
            *mem_has_ecc = 1;
        }
        break;
    case EGR_VP_VLAN_MEMBERSHIPm:
        if (SOC_IS_TOMAHAWK2(unit)) {
            test_data->mem = EGR_VP_VLAN_MEMBERSHIP_ECCm;
            *mem_has_ecc = 1;
        }
        break;
    case ING_VP_VLAN_MEMBERSHIPm:
        if (SOC_IS_TOMAHAWK2(unit)) {
            test_data->mem = ING_VP_VLAN_MEMBERSHIP_ECCm;
            *mem_has_ecc = 1;
        }
        break;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case L3_DEFIP_ALPM_LEVEL3m:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_DEFIP_ALPM_LEVEL3_ECCm;
            test_data->index = test_data->index_fv * 4;
            *mem_has_ecc = 1;
        }
        break;
    case L3_DEFIP_ALPM_LEVEL3_SINGLEm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_DEFIP_ALPM_LEVEL3_ECCm;
            test_data->index = test_data->index_fv;
            *mem_has_ecc = 1;
        }
        break;
    case L3_DEFIP_ALPM_LEVEL2m:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_DEFIP_ALPM_LEVEL2_ECCm;
            test_data->index = test_data->index_fv * 4;
            *mem_has_ecc = 1;
        }
        break;
    case L3_DEFIP_ALPM_LEVEL2_SINGLEm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_DEFIP_ALPM_LEVEL2_ECCm;
            test_data->index = test_data->index_fv;
            *mem_has_ecc = 1;
        }
        break;
    case L3_ENTRY_ONLY_SINGLEm:
    case L3_ENTRY_SINGLEm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_ENTRY_ECCm;
            test_data->index = test_data->index_fv;
            *mem_has_ecc = 1;
        }
        break;
    case L3_ENTRY_ONLY_DOUBLEm:
    case L3_ENTRY_DOUBLEm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_ENTRY_ECCm;
            test_data->index = test_data->index_fv*2;
            *mem_has_ecc = 1;
        }
        break;
    case L3_ENTRY_ONLY_QUADm:
    case L3_ENTRY_QUADm:
        if (SOC_IS_TOMAHAWK3(unit)) {
            test_data->mem = L3_ENTRY_ECCm;
            test_data->index = test_data->index_fv*4;
            *mem_has_ecc = 1;
        }
        break;
    case MPLS_ENTRY_SINGLEm:
        test_data->mem = MPLS_ENTRY_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
#endif
    default:
        test_data->mem = test_data->mem_fv;
        test_data->index = test_data->index_fv;
        break;
    }
    if ((test_data->mem != test_data->mem_fv) ||
        (test_data->index != test_data->index_fv)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, ser_test_th_mem_index_remap: "
                        "mem_fv %s, index_fv %0d, mem %s, index %0d, "
                        "#uft_banks %0d, uft_bkt_bank %0d, uft_le_fv %0d, "
                        "uft_le_ecc %0d, remap_status %0d \n"),
             unit, SOC_MEM_NAME(unit, test_data->mem_fv), test_data->index_fv,
             SOC_MEM_NAME(unit, test_data->mem), test_data->index, num_uft_banks,
             uft_bkt_bank, uft_le_fv, uft_le_ecc, remap_status));
    }
    return remap_status;
}

static soc_ser_test_functions_t ser_tomahawk_test_fun;

typedef struct ser_th_skipped_mem_s {
    soc_mem_t mem;
    soc_acc_type_t acc_type;
    int allow_error_inj;
} ser_th_skipped_mem_t;

const ser_th_skipped_mem_t th_skipped_mems[] = {
    { VLAN_XLATE_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { MPLS_ENTRY_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECC_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECC_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECC_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { FPEM_ECC_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L2_ENTRY_ONLY_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L3_ENTRY_ONLY_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L3_DEFIP_ALPM_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { EGR_VLAN_XLATE_ECCm, _SOC_ACC_TYPE_PIPE_ANY, 0 },

    { L2_BULKm, _SOC_ACC_TYPE_PIPE_ANY, 0 },
    { L2_LEARN_INSERT_FAILUREm, _SOC_ACC_TYPE_PIPE_ANY, 0 },

    { L3_DEFIP_ALPM_IPV4m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { L3_DEFIP_ALPM_IPV6_64_1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { L3_DEFIP_ALPM_RAWm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
#ifdef BCM_TOMAHAWK3_SUPPORT
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, 1},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 1},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_4, 1},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { L3_DEFIP_ALPM_LEVEL2_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 1},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_0, 1},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 1},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_4, 1},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { L3_DEFIP_ALPM_LEVEL2m, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 1},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, 1},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 1},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_4, 1},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { L3_DEFIP_ALPM_LEVEL2_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 1},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, 1},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 0},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_4, 1},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { L3_DEFIP_ALPM_LEVEL3_SINGLEm, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 0},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_0, 1},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 0},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_4, 1},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { L3_DEFIP_ALPM_LEVEL3m, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 0},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, 1},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 0},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_4, 1},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { L3_DEFIP_ALPM_LEVEL3_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 0},
    { EGR_L3_NEXT_HOPm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { EGR_L3_NEXT_HOPm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { EGR_L3_NEXT_HOPm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { EGR_L3_NEXT_HOPm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { EXACT_MATCH_2m, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { EXACT_MATCH_2m, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { EXACT_MATCH_2m, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 0},
    { EXACT_MATCH_2m, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { EXACT_MATCH_2m, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { EXACT_MATCH_2m, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 0},
    { EXACT_MATCH_4m, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { EXACT_MATCH_4m, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { EXACT_MATCH_4m, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 0},
    { EXACT_MATCH_4m, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { EXACT_MATCH_4m, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { EXACT_MATCH_4m, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 0},
    { EXACT_MATCH_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, 0},
    { EXACT_MATCH_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, 0},
    { EXACT_MATCH_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, 0},
    { EXACT_MATCH_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, 0},
    { EXACT_MATCH_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, 0},
    { EXACT_MATCH_ECCm, _SOC_UNIQUE_ACC_TYPE_PIPE_7, 0},
#endif
    { IS_TDM_CALENDAR0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR0_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR0_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { IS_TDM_CALENDAR1_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },


    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },

    /* These memories will be accessed by HW, so skipped for TR 144 */
    { TDM_CALENDAR0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR0_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR0_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
#ifdef BCM_TOMAHAWK2_SUPPORT
    { TDM_CALENDAR1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { TDM_CALENDAR1_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK0_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK1_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK2_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK3_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK4_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK5_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK6_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK7_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK8_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK9_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK10_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK11_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK12_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_CFAP_BANK13_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK0_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1_XPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1_XPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1_XPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { REPLICATION_FIFO_BANK1_XPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DBm, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED0_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED0_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED0_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED0_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED1_PIPE0m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED1_PIPE1m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED1_PIPE2m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
    { MMU_ENQS_PBI_DB_SED1_PIPE3m, _SOC_ACC_TYPE_PIPE_ANY, 1 },
#endif


    /* End */
    { INVALIDm }
};

STATIC soc_acc_type_t
_soc_tomahawk_pipe_to_acc_type(int pipe)
{
    soc_acc_type_t rv;
    switch (pipe) {
        case SOC_PIPE_SELECT_ANY:
        case SOC_PIPE_SELECT_0: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_0; break;
        case SOC_PIPE_SELECT_1: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_1; break;
        case SOC_PIPE_SELECT_2: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_2; break;
        case SOC_PIPE_SELECT_3: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_3; break;
        case SOC_PIPE_SELECT_4: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_4; break;
        case SOC_PIPE_SELECT_5: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_5; break;
        case SOC_PIPE_SELECT_6: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_6; break;
        case SOC_PIPE_SELECT_7: rv = _SOC_UNIQUE_ACC_TYPE_PIPE_7; break;
        default: rv =_SOC_ACC_TYPE_PIPE_ALL; break;
    }
    return rv;
    /* For TH, ONLY possible values of test_data->acc_type
     * are: ACC_TYPE_PIPE_0, 1, 2, 3, ALL
     */
}

STATIC soc_pipe_select_t
_soc_tomahawk_acc_type_to_pipe(soc_acc_type_t acc_type)
{
    soc_pipe_select_t rv;
        switch (acc_type) {
        case _SOC_UNIQUE_ACC_TYPE_PIPE_0: rv = SOC_PIPE_SELECT_0; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_1: rv = SOC_PIPE_SELECT_1; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_2: rv = SOC_PIPE_SELECT_2; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_3: rv = SOC_PIPE_SELECT_3; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_4: rv = SOC_PIPE_SELECT_4; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_5: rv = SOC_PIPE_SELECT_5; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_6: rv = SOC_PIPE_SELECT_6; break;
        case _SOC_UNIQUE_ACC_TYPE_PIPE_7: rv = SOC_PIPE_SELECT_7; break;
        case _SOC_ACC_TYPE_PIPE_ANY:  rv = SOC_PIPE_SELECT_ANY; break;
        default:                      rv = SOC_PIPE_SELECT_ALL; break;
        }
    return rv;
}

STATIC int
soc_tomahawk_pipe_select(int unit, int egress, int pipe)
{
    soc_reg_t reg;

    reg = egress ? EGR_SBS_CONTROLr : SBS_CONTROLr;
    if ((pipe >= 0) && (pipe <= 3)) {
        return soc_reg_field32_modify(unit, reg, REG_PORT_ANY, PIPE_SELECTf,
                                      pipe);
    } else {
        return soc_reg_field32_modify(unit, reg, REG_PORT_ANY, PIPE_SELECTf, 0);
    }
}

STATIC soc_error_t
soc_th_parity_control_reg_set(int unit, ser_test_data_t *test_data, int enable)
{
    
    uint32 rval;
    uint64 regData;

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, test_data->parity_enable_reg, test_data->port, 0,
                       &rval));
    LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "soc_th_parity_control_reg_set READ: reg %s, rval_rdat = 0x%x, bit %d \n"),
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
                              "soc_th_parity_control_reg_set WROTE: reg %s, rval_wdat = 0x%x, bit %d \n"),
                   SOC_REG_NAME(unit, test_data->parity_enable_reg),
                   rval,
                   test_data->tcam_parity_bit
                   ));
    COMPILER_64_SET(regData, 0, rval);
    return soc_reg_set(unit, test_data->parity_enable_reg, test_data->port, 0,
                       regData);
}

STATIC soc_error_t
_ser_th_ser_support_mem_found(int unit, soc_mem_t mem,
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


STATIC soc_error_t
_ser_th_ser_error_injection_support(int unit, soc_mem_t mem,
                                    int pipe_target)
{
    int rv = SOC_E_UNAVAIL;
    int i, p;
    uint32 rval;
    _soc_generic_ser_info_t *tcams = _soc_th_tcam_ser_info[unit];

    soc_acc_type_t soc_acc_type_target =
        _soc_tomahawk_pipe_to_acc_type(pipe_target);

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
    for (i = 0; th_skipped_mems[i].mem != INVALIDm; i++) {
        if ((th_skipped_mems[i].mem == mem) &&
            ((_SOC_ACC_TYPE_PIPE_ANY == th_skipped_mems[i].acc_type) ||
             (th_skipped_mems[i].acc_type == soc_acc_type_target))) {
            if (th_skipped_mems[i].allow_error_inj) {
                return SOC_E_NONE;
            } else {
                return rv;
            }
        }
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
        rv = _soc_th2_ser_tcam_wrapper_found(mem);
        if(rv == SOC_E_NONE) {
            return rv;
        }
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
        rv = _soc_th3_ser_tcam_wrapper_found(mem);
        if(rv == SOC_E_NONE) {
            return rv;
        }
    }
#endif
    /* Search TCAMs */
    if (SOC_REG_IS_VALID(unit, SER_RANGE_ENABLEr)) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, SER_RANGE_ENABLEr, REG_PORT_ANY, 0, &rval));
        for (i = 0; tcams[i].mem != INVALIDm; i++) {
            if (tcams[i].mem == mem) {
                if (((rval >> tcams[i].ser_hw_index) & 0x1) == 0) {
                    return rv; /* matched mem but ser_detection is disabled */
                }
                return SOC_E_NONE; /* found */
            }
            if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem) != NULL) {
                for (p = 0; p < NUM_PIPE(unit); p++) {
                    if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem)[p] == mem) {
                        if (((rval >> (tcams[i].ser_hw_index + 1)) & 0x1) == 0) {
                            return rv; /* matched mem but ser_detection is disabled */
                        }
                        return SOC_E_NONE; /* found */
                    }
                }
            }
        }
    }

   /* Search for mems listed in:
    * _soc_bcm56960_a0_ip_mem_ser_info,
    * _soc_bcm56960_a0_ep_mem_ser_info,
    * _soc_bcm56960_a0_mmu_mem_ser_info,
    */
    rv = _ser_th_ser_support_mem_found(unit, mem, SOC_IP_MEM_SER_INFO(unit));
    if(rv == SOC_E_NONE) {
        return rv;
    }
    rv = _ser_th_ser_support_mem_found(unit, mem, SOC_EP_MEM_SER_INFO(unit));
    if(rv == SOC_E_NONE) {
        return rv;
    }
    rv = _ser_th_ser_support_mem_found(unit, mem, SOC_MMU_MEM_SER_INFO(unit));
    if(rv == SOC_E_NONE) {
        return rv;
    }
    return rv;
}

STATIC soc_error_t
_ser_th_correction_info_get(int unit,
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

typedef struct ser_th_mems_test_field_s {
    soc_mem_t mem;
    soc_acc_type_t acc_type;
    soc_field_t test_field;
} ser_th_mems_test_field_t;

const ser_th_mems_test_field_t th_mems_test_field[] = {
    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, EVEN_EGR_VLAN_STG_PARITY_P0f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, EVEN_EGR_VLAN_STG_PARITY_P1f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, EVEN_EGR_VLAN_STG_PARITY_P2f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, EVEN_EGR_VLAN_STG_PARITY_P3f },

    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, EVEN_PBM_PARITY_P0f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, EVEN_PBM_PARITY_P1f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, EVEN_PBM_PARITY_P2f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, EVEN_PBM_PARITY_P3f },

    { INVALIDm, _SOC_ACC_TYPE_PIPE_ANY, INVALIDf }
};

#ifdef BCM_TOMAHAWK2_SUPPORT
const ser_th_mems_test_field_t th2_mems_test_field[] = {
    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP_0f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP_1f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, ECCP_2f },
    { EGR_VLAN_STGm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, ECCP_3f },

    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP_P0f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP_P1f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, ECCP_P2f },
    { SRC_MODID_INGRESS_BLOCKm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, ECCP_P3f },

    { INVALIDm, _SOC_ACC_TYPE_PIPE_ANY, INVALIDf }
};
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
const ser_th_mems_test_field_t th3_mems_test_field[] = {
    /* mem with acc_type=data_split, has separate parity field for each pipe */
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_0, ECCP_P0f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_1, ECCP_P1f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_2, ECCP_P2f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_3, ECCP_P3f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_4, ECCP_P4f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_5, ECCP_P5f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_6, ECCP_P6f },
    { STG_TABm, _SOC_UNIQUE_ACC_TYPE_PIPE_7, ECCP_P7f },

    { INVALIDm, _SOC_ACC_TYPE_PIPE_ANY, INVALIDf }
};
#endif
STATIC void
_soc_ser_find_test_field(int unit, soc_mem_t mem,
                         soc_acc_type_t soc_acc_type_target,
                         soc_field_t *test_field)
{
    int i;
    const ser_th_mems_test_field_t *mems_test_field = th_mems_test_field;

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        mems_test_field = th2_mems_test_field;
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        mems_test_field = th3_mems_test_field;
    }
#endif
    for (i = 0; mems_test_field[i].mem != INVALIDm; i++) {
        if ((mems_test_field[i].mem == mem) &&
            ((mems_test_field[i].acc_type == soc_acc_type_target) ||
             (mems_test_field[i].acc_type == _SOC_ACC_TYPE_PIPE_ANY))
           ) {
            *test_field = mems_test_field[i].test_field;
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
 *      _soc_tomahawk_perform_ser_test
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
_soc_tomahawk_perform_ser_test(int unit, uint32 flags, ser_test_data_t *test_data,
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
    for (k = 0; th_skipped_mems[k].mem != INVALIDm; k++) {
        if ((th_skipped_mems[k].mem == test_data->mem_fv) &&
            ((_SOC_ACC_TYPE_PIPE_ANY == th_skipped_mems[k].acc_type) ||
             (th_skipped_mems[k].acc_type == test_data->acc_type))) {
            skip_mem = TRUE;
        }
    }

    if (soc_feature(unit, soc_feature_no_vfp) &&
       (test_data->mem_fv == VFP_TCAMm || test_data->mem_fv == VFP_POLICY_TABLEm ||
        test_data->mem_fv == VFP_POLICY_TABLE_PIPE0m ||
        test_data->mem_fv == VFP_POLICY_TABLE_PIPE1m ||
        test_data->mem_fv == VFP_POLICY_TABLE_PIPE2m ||
        test_data->mem_fv == VFP_POLICY_TABLE_PIPE3m)) {
        skip_mem = TRUE;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit) &&
        soc_th3_ser_test_skip_by_at(unit, test_data->mem_fv, test_data->acc_type)) {
        skip_mem = TRUE;
    }
#endif

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
    if ((mem_acc_type == _SOC_TH_ACC_TYPE_ADDR_SPLIT_DIST) ||
        (mem_acc_type == _SOC_TH_ACC_TYPE_ADDR_SPLIT_SPLIT)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "Memory %s has acc_type ADDR_SPLIT (%0d) \n"),
             SOC_MEM_NAME(unit,test_data->mem_fv), mem_acc_type));
        /* skip_mem = TRUE; */
    }

    if (!skip_mem) {
        pipe_target = _soc_tomahawk_acc_type_to_pipe(test_data->acc_type);
        _soc_ser_find_test_field(unit, test_data->mem_fv, test_data->acc_type,
                                 &test_data->test_field);
        soc_tomahawk_pipe_select(unit, TRUE, pipe_target);
        soc_tomahawk_pipe_select(unit, FALSE, pipe_target);
/*
        if (_soc_th_check_counter_with_ecc(unit, test_data->mem_fv)) {
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
        soc_tomahawk_pipe_select(unit, TRUE, 0);
        soc_tomahawk_pipe_select(unit, FALSE, 0);
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

int
_soc_th_refresh_modify(int unit, int enable) {
    struct {
        soc_reg_t ref_reg;
        soc_field_t en_field;
    } rf_list[] = {
        {AUX_ARB_CONTROLr, FP_REFRESH_ENABLEf},
        {EFP_METER_CONTROLr, EFP_REFRESH_ENABLEf},
        {MMU_GCFG_MISCCONFIGr, REFRESH_ENf},
#ifdef BCM_TOMAHAWK3_SUPPORT
        {AUX_ARB_CONTROLr, DLB_HGT_256NS_REFRESH_ENABLEf},
        {MMU_MTRO_CONFIGr, REFRESH_DISABLEf},
        {MMU_WRED_REFRESH_CONTROLr, REFRESH_DISABLEf},
        {IFP_REFRESH_CONFIGr, IFP_REFRESH_DISABLEf},
        {DLB_ECMP_REFRESH_DISABLEr, DLB_REFRESH_DISABLEf},
#endif
        {INVALIDr, INVALIDf}
    };
    uint32 rval;
    uint32 fld_val = 0;
    int i;
    for (i = 0; rf_list[i].ref_reg != INVALIDr; i++) {
        if (!SOC_REG_IS_VALID(unit, rf_list[i].ref_reg) ||
            !soc_reg_field_valid(unit, rf_list[i].ref_reg, rf_list[i].en_field)) {
            continue;
        }

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit) && rf_list[i].en_field == FP_REFRESH_ENABLEf) {
            continue;
        }
        if (SOC_IS_TOMAHAWK3(unit) && (rf_list[i].ref_reg == MMU_MTRO_CONFIGr ||
            rf_list[i].ref_reg == MMU_WRED_REFRESH_CONTROLr ||
            rf_list[i].ref_reg == IFP_REFRESH_CONFIGr ||
            rf_list[i].ref_reg == DLB_ECMP_REFRESH_DISABLEr)) {
                fld_val = enable ? 0 : 1;
        } else
#endif
        {
            fld_val = enable ? 1 : 0;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, rf_list[i].ref_reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, rf_list[i].ref_reg, &rval, rf_list[i].en_field,
                          fld_val);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, rf_list[i].ref_reg, REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

soc_error_t
soc_th_test_mem_create(int unit, soc_mem_t mem, int block, int index,
                              uint32* p_flags, soc_acc_type_t soc_acc_type_target,
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
            _soc_ser_find_test_field(unit, mem, soc_acc_type_target,
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
                *p_flags |= SOC_INJECT_ERROR_2BIT_ECC;
            }

            SOC_IF_ERROR_RETURN
                     (ser_test_mem_index_remap(unit, p_test_data, &mem_has_ecc));
            if (mem_has_ecc) {
                *p_flags |= SOC_INJECT_ERROR_2BIT_ECC;
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

STATIC int
soc_th_ser_pipe_check(int unit, int pipe)
{
    int rv = SOC_E_NONE;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (pipe != SOC_PIPE_SELECT_0 &&
            pipe != SOC_PIPE_SELECT_1 &&
            pipe != SOC_PIPE_SELECT_2 &&
            pipe != SOC_PIPE_SELECT_3 &&
            pipe != SOC_PIPE_SELECT_4 &&
            pipe != SOC_PIPE_SELECT_5 &&
            pipe != SOC_PIPE_SELECT_6 &&
            pipe != SOC_PIPE_SELECT_7 &&
            pipe != SOC_PIPE_SELECT_ANY &&
            pipe != _SOC_ACC_TYPE_PIPE_ALL) {
            rv = SOC_E_PARAM;
        }
    } else
#endif
    {
        if (pipe != SOC_PIPE_SELECT_0 &&
            pipe != SOC_PIPE_SELECT_1 &&
            pipe != SOC_PIPE_SELECT_2 &&
            pipe != SOC_PIPE_SELECT_3 &&
            pipe != SOC_PIPE_SELECT_ANY &&
            pipe != _SOC_ACC_TYPE_PIPE_ALL) {
            rv = SOC_E_PARAM;
        }
    }

    return rv;
}


/*
 * Function:
 *      soc_th_ser_inject_or_test_mem
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
soc_th_ser_inject_or_test_mem(int unit, soc_mem_t mem, int pipe_target, int block,
                              int index, _soc_ser_test_t test_type,
                              int inject_only, int cmd, uint32 flags)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], field_data[SOC_MAX_MEM_FIELD_WORDS];
    ser_test_data_t test_data;
    _soc_generic_ser_info_t *tcams = _soc_th_tcam_ser_info[unit];
    int i, p, error_count = 0, skip_count = 0, found_mem = FALSE;
    int fld_len;
    char fail_message[TR_MEM_NAME_SIZE_MAX + 1];
    soc_error_t rv = SOC_E_NONE;
    soc_acc_type_t soc_acc_type_target, mem_acc_type;
    soc_field_t test_field = INVALIDf;
    uint32 rval;
    uint32 *entry = NULL;
    uint32 fld_value = 0;
    ser_correction_info_t corr_info;
    uint8 xor_bank = FALSE;
    _soc_ser_sram_info_t *sram_info = NULL;

    SOC_IF_ERROR_RETURN(soc_th_ser_pipe_check(unit, pipe_target));

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "unit %d, mem %s is INVALID or not valid "
                              "or parity is disabled for this mem !!\n"),
                   unit, SOC_MEM_NAME(unit,mem)));
        return SOC_E_UNAVAIL;
    }
/***
    if (_soc_th_check_counter_with_ecc(unit, mem)) {
        flags |= SOC_INJECT_ERROR_2BIT_ECC;
    }
 */

    mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit) && (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_7)) {
        soc_acc_type_target = mem_acc_type;
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "soc_th_ser_inject_or_test_mem: unit %d, mem %s, "
                        "pipe_target %d, BUT mem_acc_type %d will be "
                        "used\n"),
             unit, SOC_MEM_NAME(unit, mem),
             pipe_target, soc_acc_type_target));
    } else
#endif
    if (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_3) {
        soc_acc_type_target = mem_acc_type;
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "soc_th_ser_inject_or_test_mem: unit %d, mem %s, "
                        "pipe_target %d, BUT mem_acc_type %d will be "
                        "used\n"),
             unit, SOC_MEM_NAME(unit, mem),
             pipe_target, soc_acc_type_target));
    } else if ((mem_acc_type == _SOC_TH_ACC_TYPE_ADDR_SPLIT_DIST) ||
               (mem_acc_type == _SOC_TH_ACC_TYPE_ADDR_SPLIT_SPLIT)) {
        /* 1. ignore pipe_target, because pipe is determined by index param
         * 2. For such mem, ser_en_info in bcm56960_a0.c file will not have
         *    _PIPE0,1,2,3 views (these are created only for mems with UNIQUE
         *    acc type.
         */
        soc_acc_type_target = _SOC_ACC_TYPE_PIPE_ANY;
    } else {
        soc_acc_type_target = _soc_tomahawk_pipe_to_acc_type(pipe_target);
    }

    corr_info.inject_mem = mem;
    _ser_th_correction_info_get(unit, &corr_info);

    /* Check if the memory is to be skipped */
    for (i = 0; th_skipped_mems[i].mem != INVALIDm; i++) {
        if ((th_skipped_mems[i].mem == mem) &&
            ((_SOC_ACC_TYPE_PIPE_ANY == th_skipped_mems[i].acc_type) ||
             (th_skipped_mems[i].acc_type == soc_acc_type_target))) {
            /* coverity[secure_coding] */
            if (inject_only && th_skipped_mems[i].allow_error_inj) {
                break;
            }
            TR_TEST_MEM_PRINT(unit, fail_message, mem);
            return SOC_E_UNAVAIL;
        }
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
        for (i = 0; _soc_th2_tcam_wrapper_table[i].mem != INVALIDm; i++) {
            if (_soc_th2_tcam_wrapper_table[i].mem == mem) {
                /* found */
                found_mem = TRUE;
                soc_ser_create_test_data(unit, tmp_entry, field_data,
                                         _soc_th2_tcam_wrapper_table[i].enable_reg,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         _soc_th2_tcam_wrapper_table[i].enable_field,
                                         mem, _soc_th2_tcam_wrapper_table[i].parity_field,
                                         block, REG_PORT_ANY,
                                         soc_acc_type_target, index, &test_data);
                break;
            }
        }
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
        for (i = 0; _soc_bcm56980_a0_tcam_mem_ser_info[i].mem != INVALIDm; i++) {
            if (_soc_bcm56980_a0_tcam_mem_ser_info[i].mem == mem) {
                /* found */
                found_mem = TRUE;
                soc_ser_create_test_data_with_new_format(unit, tmp_entry, field_data,
                                         _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_reg,
                                         INVALIDm, SOC_INVALID_TCAM_PARITY_BIT,
                                         _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_field,
                                         -1, mem,
                                         _soc_bcm56980_a0_tcam_mem_ser_info[i].parity_field,
                                         MEM_BLOCK_ANY, REG_PORT_ANY, soc_acc_type_target,
                                         index, &test_data);
                break;
            }
        }
    }
#endif

    if (!found_mem) {
        /* Search for TCAM memories */
        if (SOC_REG_IS_VALID(unit, SER_RANGE_ENABLEr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, SER_RANGE_ENABLEr, REG_PORT_ANY, 0, &rval));
            for (i = 0; tcams[i].mem != INVALIDm; i++) {
                if (tcams[i].mem == mem) {
                    if (mem == L3_DEFIPm ||
                        mem == L3_DEFIP_PAIR_128m ||
                        mem == ING_SNATm ||
                        mem == L3_TUNNELm) {
                        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                            "soc_th_ser_inject_or_test_mem: mem %s, will not inject error on TCAM.valid bit, but on data portion of this table\n"),
                            SOC_MEM_NAME(unit, mem)));
                        break; /* from tcam loop (inject error to data view) */
                    }
                    switch (tcams[i].mem) {
                    case L3_DEFIPm:
                        test_field = VALID0f;
                        break;
                    case L3_DEFIP_PAIR_128m:
                        test_field = VALID0_LWRf;
                        break;
                    default:
                        test_field = VALIDf;
                        break;
                    }

                    /* For mems that can operate in either global/unique mode
                     * make sure we find the correct enable bit position in
                     * SER_RANGE_ENABLEr
                     */
                    if (((rval >> tcams[i].ser_hw_index) & 0x1) == 0) {
                        break; /* we found the matching mem in list, BUT ser_detection
                                  is disabled - so no more searching */
                    }
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "soc_th_ser_inject_or_test_mem: unit %d, mem %s, "
                                    "i %d, ser_hw_index %d, ser_range_enable=%8x\n"),
                         unit, SOC_MEM_NAME(unit, mem), i, tcams[i].ser_hw_index,
                         rval));
                    soc_ser_create_test_data(unit, tmp_entry, field_data,
                                             SER_RANGE_ENABLEr, tcams[i].ser_hw_index,
                                             INVALIDf, mem,
                                             test_field, block, REG_PORT_ANY,
                                             soc_acc_type_target, index, &test_data);
                    found_mem = TRUE;
                    break; /*A memory was found that matched, so stop searching.*/
                }
                if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem) != NULL) {
                    for (p = 0; p < NUM_PIPE(unit); p++) {
                        if (SOC_MEM_UNIQUE_ACC(unit, tcams[i].mem)[p] == mem) {
                            if (((rval >> (tcams[i].ser_hw_index + 1)) & 0x1) == 0) {
                                break; /* from pipe loop */
                                       /* we found matching mem, BUT ser_detection is
                                        * disabled - so no more searching */
                            }
                            test_field = VALIDf;
                            LOG_VERBOSE(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "soc_th_ser_inject_or_test_mem: unit %d, mem %s, "
                                            "i %d, ser_hw_index+1 %d, ser_range_enable=%8x\n"),
                                 unit, SOC_MEM_NAME(unit, mem), i,
                                 tcams[i].ser_hw_index + 1, rval));
                            soc_ser_create_test_data(unit, tmp_entry, field_data,
                                                     SER_RANGE_ENABLEr,
                                                     (tcams[i].ser_hw_index + 1),
                                                     INVALIDf, mem,
                                                     test_field, block, REG_PORT_ANY,
                                                     soc_acc_type_target, index,
                                                     &test_data);
                            found_mem = TRUE;
                            break; /* from pipe loop */
                        }
                    }
                }
                if (found_mem) {
                    break;
                }
            }
        }
    }

    /* Search for mems listed in:
     * _soc_bcm56960_a0_ip_mem_ser_info,
     * _soc_bcm56960_a0_ep_mem_ser_info
     */
    if (!found_mem) {
        rv = soc_th_test_mem_create(unit, mem, block, index, &flags, soc_acc_type_target,
             tmp_entry, field_data, &test_data, SOC_IP_MEM_SER_INFO(unit));
        if (rv == SOC_E_NONE) {
            found_mem = TRUE;
        }else {
            rv = soc_th_test_mem_create(unit, mem, block, index, &flags, soc_acc_type_target,
                 tmp_entry, field_data, &test_data, SOC_EP_MEM_SER_INFO(unit));
            if (rv == SOC_E_NONE) {
                found_mem = TRUE;
            } else {
                rv = soc_th_test_mem_create(unit, mem, block, index, &flags, _SOC_ACC_TYPE_PIPE_ANY,
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
                        "soc_th_ser_inject_or_test_mem: unit %d, mem %s, "
                        "pipe_target %d, block %d, index %d, test_type %d, "
                        "inject_only %d, cmd %d, flags 0x%x \n"),
             unit, SOC_MEM_NAME(unit, mem),
             pipe_target, block, index, test_type, inject_only, cmd, flags));
        if (inject_only) {
            /* index check */
            if (!soc_mem_index_valid(unit, mem, index)) {
                TR_TEST_MEM_PRINT(unit, fail_message, mem);
                rv = SOC_E_PARAM;
                return rv;
            }

            xor_bank = (flags & SOC_INJECT_ERROR_XOR_BANK) ? TRUE : FALSE;
            if (xor_bank) {
                /*
                * To inject parity error to XOR Bank
                * 1) Disable parity_en of table
                * 2) Set DEBUG_DISABLE_XOR_WRITE==0
                * 3) Issue MEMWR to table with corrupt PARITY field
                * 4) Set DEBUG_DISABLE_XOR_WRITE==1
                * 5) Issue MEMWR to table with correct PARITY field
                * 6) Set DEBUG_DISABLE_XOR_WRITE==0
                * 7) Enable parity_en of table
                */
                sram_info = sal_alloc(sizeof(_soc_ser_sram_info_t), "TH sram info");
                if (NULL == sram_info) {
                    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                                "soc_th_ser_inject_or_test_mem: mem %s, alloc "
                                "memory for sram_info failed!\n"),
                                SOC_MEM_NAME(unit, mem)));
                    return (SOC_E_MEMORY);
                }
                _soc_sram_info_init(unit, sram_info);
                (void)soc_tomahawk_mem_sram_info_get(unit,
                          test_data.mem, test_data.index, sram_info);
            }

            /* Set pipe select */
            soc_tomahawk_pipe_select(unit, TRUE, pipe_target);
            soc_tomahawk_pipe_select(unit, FALSE, pipe_target);

            /* Disable parity */
            rv = _ser_test_parity_control(unit, &test_data, 0);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "soc_th_ser_inject_or_test_mem: unit %d,"
                    "Error_inject: Disable parity for mem %s FAILED\n"),
                    unit, SOC_MEM_NAME(unit, mem)));
                soc_tomahawk_pipe_select(unit, TRUE, 0);
                soc_tomahawk_pipe_select(unit, FALSE, 0);
                if (xor_bank && sram_info != NULL) {
                    sal_free(sram_info);
                }
                return rv;
            }

            /* Read the memory (required for successful injection) */
            rv = ser_test_mem_read(unit, 0, &test_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "soc_th_ser_inject_or_test_mem: unit %d, Error_inject: Read for mem %s FAILED\n"),
                    unit, SOC_MEM_NAME(unit, mem)));
                (void)_ser_test_parity_control(unit, &test_data, 1);
                soc_tomahawk_pipe_select(unit, TRUE, 0);
                soc_tomahawk_pipe_select(unit, FALSE, 0);
                if (xor_bank && sram_info != NULL) {
                    sal_free(sram_info);
                }
                return rv;
            }

            /* Inject error */
            rv = soc_ser_test_inject_full(unit, flags, &test_data);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "soc_th_ser_inject_or_test_mem: unit %d, Error_inject: error inject for mem %s FAILED\n"),
                    unit, SOC_MEM_NAME(unit, mem)));
                (void)_ser_test_parity_control(unit, &test_data, 1);
                soc_tomahawk_pipe_select(unit, TRUE, 0);
                soc_tomahawk_pipe_select(unit, FALSE, 0);
                if (xor_bank && sram_info != NULL) {
                    sal_free(sram_info);
                }
                return rv;
            }

            if (xor_bank) {
                if (SOC_REG_IS_VALID(unit, sram_info->force_reg)) {
                    /* Disable xor bank writing */
                    fld_len = soc_reg_field_length(unit, sram_info->force_reg,
                                                   sram_info->disable_xor_write_field);
                    fld_value = (1 << fld_len) - 1;
                    soc_reg_field32_modify(unit, sram_info->force_reg,
                                           REG_PORT_ANY,
                                           sram_info->disable_xor_write_field, fld_value);

                    /* Call it again to write correct data */
                    rv = soc_ser_test_inject_full(unit, flags, &test_data);

                    /* Enable xor bank writing */
                    soc_reg_field32_modify(unit, sram_info->force_reg,
                                           REG_PORT_ANY,
                                           sram_info->disable_xor_write_field, 0);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                            "soc_th_ser_inject_or_test_mem: unit %d,"
                            "write correct data into mem %s FAILED\n"),
                            unit, SOC_MEM_NAME(unit, mem)));
                        (void)_ser_test_parity_control(unit, &test_data, 1);
                        soc_tomahawk_pipe_select(unit, TRUE, 0);
                        soc_tomahawk_pipe_select(unit, FALSE, 0);
                        sal_free(sram_info);
                        return rv;
                    }

                    cli_out("Set %sr.%sf before dump to trigger XOR bank error!\n",
                        SOC_REG_NAME(unit, sram_info->force_reg),
                        SOC_FIELD_NAME(unit, sram_info->force_field));
                } else if(SOC_MEM_IS_VALID(unit, sram_info->force_mem)) {
                    entry = sal_alloc((4 * SOC_MAX_MEM_WORDS), "sram_entry");
                    if (entry == NULL) {
                        return SOC_E_MEMORY;
                    }
                    sal_memset(entry, 0, 4 * soc_mem_entry_words(unit, sram_info->force_mem));

                    /* Disable xor bank writing */
                    soc_mem_read(unit, sram_info->force_mem, MEM_BLOCK_ALL, 0, entry);
                    fld_len = soc_mem_field_length(unit, sram_info->force_mem,
                                                   sram_info->disable_xor_write_field);
                    fld_value = (1 << fld_len) - 1;
                    soc_mem_field32_set(unit, sram_info->force_mem, entry,
                                        sram_info->disable_xor_write_field, fld_value);
                    soc_mem_write(unit, sram_info->force_mem, MEM_BLOCK_ALL, 0, entry);

                    /* Call it again to write correct data */
                    rv = soc_ser_test_inject_full(unit, flags, &test_data);

                    /* Enable xor bank writing */
                    soc_mem_field32_set(unit, sram_info->force_mem, entry,
                                        sram_info->disable_xor_write_field, 0);
                    soc_mem_write(unit, sram_info->force_mem, MEM_BLOCK_ALL, 0, entry);

                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                            "soc_th_ser_inject_or_test_mem: unit %d,"
                            "write correct data into mem %s FAILED\n"),
                            unit, SOC_MEM_NAME(unit, mem)));
                        (void)_ser_test_parity_control(unit, &test_data, 1);
                        soc_tomahawk_pipe_select(unit, TRUE, 0);
                        soc_tomahawk_pipe_select(unit, FALSE, 0);
                        sal_free(sram_info);
                        sal_free(entry);
                        return rv;
                    }
                    LOG_CLI((BSL_META_U(unit, "Set %sm.%sf before dump to trigger XOR bank error!\n"),
                                        SOC_MEM_NAME(unit, sram_info->force_mem),
                                        SOC_FIELD_NAME(unit, sram_info->force_field)));
                    sal_free(entry);
                } else {
                    LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "soc_th_ser_inject_or_test_mem: unit %d,"
                        "Don't support injecting error into XOR bank of "
                        "mem %s index %d FAILED\n"),
                        unit, SOC_MEM_NAME(unit, mem), index));
                    rv = SOC_E_UNAVAIL;
                }
                sal_free(sram_info);
            }

            /* Enable parity */
            SOC_IF_ERROR_RETURN(_ser_test_parity_control(unit, &test_data, 1));

            soc_tomahawk_pipe_select(unit, TRUE, 0);
            soc_tomahawk_pipe_select(unit, FALSE, 0);
        } else {
            if (cmd) {
                ser_test_cmd_generate(unit, &test_data);
            } else {
                int def_soc_ser_test_long_sleep;
                int def_soc_ser_test_long_sleep_time_us;

                /* save default values, choose TH specific values  */
                def_soc_ser_test_long_sleep = soc_ser_test_long_sleep;
                def_soc_ser_test_long_sleep_time_us = soc_ser_test_long_sleep_time_us;
                soc_ser_test_long_sleep = TRUE;
                soc_ser_test_long_sleep_time_us =
                    soc_property_get(unit, "tr144_long_sleep_time_us",
                                     _SOC_TH_TR144_LONG_SLEEP_TIME_US);
                LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "tr 144 test will use:long_sleep = %d,"
                                "long_sleep_time = %d uS\n"),
                     soc_ser_test_long_sleep,
                     soc_ser_test_long_sleep_time_us));

                (void) _soc_th_refresh_modify(unit, 0); /* disable refresh */
                rv = _soc_tomahawk_perform_ser_test(unit, flags, &test_data, test_type,
                                                    &skip_count, &error_count);
                (void) _soc_th_refresh_modify(unit, 1); /* re-enable refresh */

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
 *      soc_th_ser_inject_error
 * Purpose:
 *      Injects an error into a single th memory
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 */
STATIC soc_error_t
soc_th_ser_inject_error(int unit, uint32 flags, soc_mem_t mem, int pipe_target,
                        int block, int index)
{
    return soc_th_ser_inject_or_test_mem(unit, mem, pipe_target, block, index,
                                         SER_SINGLE_INDEX, TRUE, FALSE, flags);
}

/*
 * Function:
 *      soc_th_ser_test_mem
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
soc_th_ser_test_mem(int unit, soc_mem_t mem, _soc_ser_test_t test_type,
                    int cmd)
{
    return soc_th_ser_inject_or_test_mem(unit, mem, -1, MEM_BLOCK_ANY, 0,
                                         test_type, FALSE, cmd, FALSE);
}

const soc_mem_t th3_tcam_test_skipped_mems[] = {
    EFP_TCAM_PIPE0m,
    EFP_TCAM_PIPE1m,
    EFP_TCAM_PIPE2m,
    EFP_TCAM_PIPE3m,
    EFP_TCAM_PIPE4m,
    EFP_TCAM_PIPE5m,
    EFP_TCAM_PIPE6m,
    EFP_TCAM_PIPE7m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE0m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE1m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE2m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE3m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE4m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE5m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE6m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE7m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE4m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE5m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE6m,
    EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE7m,
    FP_UDF_TCAM_PIPE0m,
    FP_UDF_TCAM_PIPE1m,
    FP_UDF_TCAM_PIPE2m,
    FP_UDF_TCAM_PIPE3m,
    FP_UDF_TCAM_PIPE4m,
    FP_UDF_TCAM_PIPE5m,
    FP_UDF_TCAM_PIPE6m,
    FP_UDF_TCAM_PIPE7m,
    IFP_LOGICAL_TABLE_SELECT_PIPE0m,
    IFP_LOGICAL_TABLE_SELECT_PIPE1m,
    IFP_LOGICAL_TABLE_SELECT_PIPE2m,
    IFP_LOGICAL_TABLE_SELECT_PIPE3m,
    IFP_LOGICAL_TABLE_SELECT_PIPE4m,
    IFP_LOGICAL_TABLE_SELECT_PIPE5m,
    IFP_LOGICAL_TABLE_SELECT_PIPE6m,
    IFP_LOGICAL_TABLE_SELECT_PIPE7m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE4m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE5m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE6m,
    IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE7m,
    IFP_TCAM_PIPE0m,
    IFP_TCAM_PIPE1m,
    IFP_TCAM_PIPE2m,
    IFP_TCAM_PIPE3m,
    IFP_TCAM_PIPE4m,
    IFP_TCAM_PIPE5m,
    IFP_TCAM_PIPE6m,
    IFP_TCAM_PIPE7m,
    VFP_TCAM_PIPE0m,
    VFP_TCAM_PIPE1m,
    VFP_TCAM_PIPE2m,
    VFP_TCAM_PIPE3m,
    VFP_TCAM_PIPE4m,
    VFP_TCAM_PIPE5m,
    VFP_TCAM_PIPE6m,
    VFP_TCAM_PIPE7m,
    INVALIDm
};
/*
 * Function:
 *      soc_th_ser_tcam_test
 * Purpose:
 *      Tests that SER is working for fifo and SRAM memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  the number of tests which are failed
 */
STATIC int
soc_th_ser_tcam_test(int unit, _soc_ser_test_t test_type)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], field_data[SOC_MAX_MEM_FIELD_WORDS];
    ser_test_data_t test_data;
    int mem_failed = 0, mem_tests = 0, mem_skipped = 0;
    _soc_generic_ser_info_t *tcams = _soc_th_tcam_ser_info[unit];
    int i;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int j;
    int skip = FALSE;
    int num_inst_to_test = 0;
    soc_acc_type_t start_at = _SOC_MEM_ADDR_ACC_TYPE_PIPE_0;
#endif
    soc_acc_type_t acc_type;
#ifdef BCM_TOMAHAWK2_SUPPORT
    soc_acc_type_t wrapper_at;
#endif
    soc_field_t test_field = VALIDf;
    int tcam_parity_bit;
    int mem_index = 0; /* dont_care */

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
        for (i = 0; _soc_th2_tcam_wrapper_table[i].mem != INVALIDm; i++) {
            for (wrapper_at = _SOC_MEM_ADDR_ACC_TYPE_PIPE_0; wrapper_at < NUM_PIPE(unit); wrapper_at++) {
                mem_tests++;
                if (!SOC_MEM_IS_VALID(unit, _soc_th2_tcam_wrapper_table[i].mem) ||
                    (SOC_MEM_INFO(unit, _soc_th2_tcam_wrapper_table[i].mem).flags &
                     SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
                    LOG_WARN(BSL_LS_SOC_SER,
                              (BSL_META_U(unit,
                                          "unit %d, mem %s is INVALID or not valid "
                                          "or parity is disabled for this mem !!\n"),
                               unit, SOC_MEM_NAME(unit, _soc_th2_tcam_wrapper_table[i].mem)));
                    mem_skipped++;
                    continue;
                }

                soc_ser_create_test_data(unit, tmp_entry, field_data,
                                         _soc_th2_tcam_wrapper_table[i].enable_reg,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         _soc_th2_tcam_wrapper_table[i].enable_field,
                                         _soc_th2_tcam_wrapper_table[i].mem,
                                         _soc_th2_tcam_wrapper_table[i].parity_field,
                                         MEM_BLOCK_ANY, REG_PORT_ANY,
                                         wrapper_at,
                                         mem_index, &test_data);

                _soc_tomahawk_perform_ser_test(unit, 0, &test_data, test_type,
                                               &mem_skipped, &mem_failed);
            }
        }
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit) && soc_feature(unit, soc_feature_tcam_scan_engine)) {
            for (i = 0; _soc_bcm56980_a0_tcam_mem_ser_info[i].mem != INVALIDm; i++) {
                for (j = 0; th3_tcam_test_skipped_mems[j] != INVALIDm; j++) {
                    if (th3_tcam_test_skipped_mems[j] == _soc_bcm56980_a0_tcam_mem_ser_info[i].mem) {
                        skip = TRUE;
                        break;
                    }
                }
                if (skip == TRUE) {
                    skip = FALSE;
                    continue;
                }
                num_inst_to_test = NUM_PIPE(unit);
                if (test_type == SER_FIRST_HALF_PIPES) {
                    start_at = _SOC_MEM_ADDR_ACC_TYPE_PIPE_0;
                    num_inst_to_test /= 2;
                }
                if (test_type == SER_SECOND_HALF_PIPES) {
                    start_at = _SOC_MEM_ADDR_ACC_TYPE_PIPE_4;
                    num_inst_to_test /= 2;
                }
                for (wrapper_at = start_at; wrapper_at < (start_at + num_inst_to_test); wrapper_at++) {
                    mem_tests++;
                    if (!SOC_MEM_IS_VALID(unit, _soc_bcm56980_a0_tcam_mem_ser_info[i].mem) ||
                        (SOC_MEM_INFO(unit, _soc_bcm56980_a0_tcam_mem_ser_info[i].mem).flags &
                         SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
                        LOG_WARN(BSL_LS_SOC_SER,
                                  (BSL_META_U(unit,
                                              "unit %d, mem %s is INVALID or not valid "
                                              "or parity is disabled for this mem !!\n"),
                                   unit, SOC_MEM_NAME(unit, _soc_bcm56980_a0_tcam_mem_ser_info[i].mem)));
                        mem_skipped++;
                        continue;
                    }

                    soc_ser_create_test_data_with_new_format(unit, tmp_entry, field_data,
                                             _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_reg,
                                             INVALIDm, SOC_INVALID_TCAM_PARITY_BIT,
                                             _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_field,
                                             -1, _soc_bcm56980_a0_tcam_mem_ser_info[i].mem,
                                             _soc_bcm56980_a0_tcam_mem_ser_info[i].parity_field,
                                             MEM_BLOCK_ANY, REG_PORT_ANY, wrapper_at, mem_index,
                                             &test_data);

                    _soc_tomahawk_perform_ser_test(unit, 0, &test_data, test_type,
                                                   &mem_skipped, &mem_failed);
                }
            }
        }
#endif

    if (tcams != NULL) {
        for (i = 0; tcams[i].mem != INVALIDm; i++) {
            mem_tests++;
            if (!SOC_MEM_IS_VALID(unit, tcams[i].mem) ||
                (SOC_MEM_INFO(unit, tcams[i].mem).flags &
                 SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)) {
                LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "unit %d, mem %s is INVALID or not valid "
                                        "or parity is disabled for this mem !!\n"),
                            unit, SOC_MEM_NAME(unit, tcams[i].mem)));
                mem_skipped++;
                continue;
            }
            /* Avoid re-testing tcams that are in dual mode - they are listed
             * twice in the tcams list */
            if (tcams[i].ser_flags & _SOC_SER_FLAG_VIEW_DISABLE) {
                mem_skipped++;
                continue;
            }

            acc_type = (tcams[i].ser_flags & _SOC_SER_FLAG_ACC_TYPE_MASK);

            switch (tcams[i].mem) {
            case L3_DEFIPm:
                test_field = VALID0f;
                break;
            case L3_DEFIP_PAIR_128m:
                test_field = VALID0_LWRf;
                break;
            default:
                test_field = VALIDf;
                break;
            }

            /* For tcams that can operate in either global/unique mode
             * find correct enable bit in SER_RANGE_ENABLEr */
            tcam_parity_bit =
                (tcams[i].ser_dynamic_state == _SOC_SER_STATE_PIPE_MODE_UNIQUE)?
                (tcams[i].ser_hw_index + 1) : tcams[i].ser_hw_index;

            soc_ser_create_test_data(unit, tmp_entry, field_data, SER_RANGE_ENABLEr,
                                     tcam_parity_bit, INVALIDf, tcams[i].mem,
                                     test_field, MEM_BLOCK_ANY, REG_PORT_ANY,
                                     acc_type, mem_index, &test_data);

            _soc_tomahawk_perform_ser_test(unit, 0, &test_data, test_type,
                                           &mem_skipped, &mem_failed);

            /* Note: for each entry in template[], above will perform 4 tests (one
             *       per pipe)
             */
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
soc_th_ser_block_test(int unit, _soc_ser_test_t test_type,
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
                        (BSL_META_U(
                             unit,
                             "unit %d, mem %s is INVALID or not valid "
                             "or parity is disabled for this mem !!\n"),
                        unit, SOC_MEM_NAME(unit, ser_info_list[i].mem)));
            continue;
        }

        if (!soc_feature(unit, soc_feature_advanced_flex_counter)) {
            if ((ser_info_list[i].mem >= ING_FLEX_CTR_COUNTER_TABLE_0m &&
                 ser_info_list[i].mem <= ING_FLEX_CTR_COUNTER_TABLE_9_PIPE3m) ||
                (ser_info_list[i].mem >= EGR_FLEX_CTR_COUNTER_TABLE_0m &&
                 ser_info_list[i].mem <= EGR_FLEX_CTR_COUNTER_TABLE_3_Ym)) {
                continue;
            }
        }

        flags = 0; /* for every mem */
        mem_acc_type = SOC_MEM_ACC_TYPE(unit, ser_info_list[i].mem);
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit) && (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_7)) {
            if ((test_type == SER_FIRST_HALF_PIPES) &&
                (mem_acc_type > _SOC_UNIQUE_ACC_TYPE_PIPE_3)) {
                continue;
            }
            if ((test_type == SER_SECOND_HALF_PIPES) &&
                (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_3)) {
                continue;
            }
            num_inst_to_test = 1;
            acc_type = mem_acc_type;
        } else
#endif
        if (blocktype == SOC_BLK_MMU_GLB) {
            num_inst_to_test = 1;
            acc_type = _SOC_ACC_TYPE_PIPE_ANY;
        } else if (mem_acc_type <= _SOC_UNIQUE_ACC_TYPE_PIPE_3) {
            num_inst_to_test = 1;
            acc_type = mem_acc_type;
        } else if ((mem_acc_type == _SOC_TH_ACC_TYPE_ADDR_SPLIT_DIST) ||
                   (mem_acc_type == _SOC_TH_ACC_TYPE_ADDR_SPLIT_SPLIT)) {
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
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (test_type == SER_FIRST_HALF_PIPES) {
                    num_inst_to_test /= 2;
                }
                if (test_type == SER_SECOND_HALF_PIPES) {
                    num_inst_to_test /= 2;
                    acc_type = _SOC_UNIQUE_ACC_TYPE_PIPE_4;
                }
            }
#endif
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
        if ((SOC_REG_IS_VALID(unit, ser_info_list[i].ecc1b_ctrl.ctrl_type.en_reg) ||
             SOC_MEM_IS_VALID(unit, ser_info_list[i].ecc1b_ctrl.ctrl_type.en_mem)) &&
            (INVALIDf != ser_info_list[i].ecc1b_ctrl.en_fld)) {
            flags |= SOC_INJECT_ERROR_2BIT_ECC;
        }
#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
        rv = _soc_tomahawk_perform_ser_test(unit, flags, &test_data, test_type,
                                            p_mem_skipped, p_mem_failed);
        if (SOC_FAILURE(rv)) {
            return SOC_E_FAIL;
        }
#else
        _soc_tomahawk_perform_ser_test(unit, flags, &test_data, test_type,
                                       p_mem_skipped, p_mem_failed);
#endif
        if (num_inst_to_test > 1) {
            for (test_data.acc_type = (acc_type + 1);
                 test_data.acc_type < (acc_type + num_inst_to_test);
                 test_data.acc_type++) {
#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
                rv = _soc_tomahawk_perform_ser_test(unit, flags, &test_data,
                                                    test_type,
                                                    p_mem_skipped,
                                                    p_mem_failed);
                if (SOC_FAILURE(rv)) {
                    return SOC_E_FAIL;
                }
#else
                _soc_tomahawk_perform_ser_test(unit, flags, &test_data,
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
 *      soc_th_ser_hardware_test
 * Purpose:
 *      Tests that SER is working for fio and SRAM memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  the number of tests which are failed
 */
STATIC int
soc_th_ser_hardware_test(int unit, _soc_ser_test_t test_type) {
    int mem_failed = 0, mem_tests = 0, mem_skipped = 0 ;
    int rv = SOC_E_NONE;

    (void) _soc_th_refresh_modify(unit, 0); /* disable refresh */

    rv = soc_th_ser_block_test(unit, test_type, &mem_failed, &mem_tests, &mem_skipped,
         SOC_IP_MEM_SER_INFO(unit), SOC_BLK_IPIPE);


#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
    if (SOC_SUCCESS(rv))
#endif
    {
        rv = soc_th_ser_block_test(unit, test_type, &mem_failed, &mem_tests, &mem_skipped,
             SOC_EP_MEM_SER_INFO(unit), SOC_BLK_EPIPE);
    }

#if defined(_SER_DBG_EXIT_ON_FIRST_FAILURE)
    if (SOC_SUCCESS(rv))
#endif
    {
        rv = soc_th_ser_block_test(unit, test_type, &mem_failed, &mem_tests, &mem_skipped,
             SOC_MMU_MEM_SER_INFO(unit), SOC_BLK_MMU_GLB);
    }

    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit, "H/W memories test fail.\n")));
    } else {
        LOG_CLI((BSL_META_U(unit, "H/W memories test success.\n")));
    }

    LOG_CLI((BSL_META_U(unit, "\nH/W memories tested on unit %d: %d\n"),
             unit, mem_tests - mem_skipped));
    LOG_CLI((BSL_META_U(unit, "H/W memories tests passed:\t%d\n"),
             mem_tests - mem_failed - mem_skipped));
    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit, "H/W memories tests skipped:\t%d \n"),
         0));
    LOG_CLI((BSL_META_U(unit, "H/W memories tests failed:\t%d\n\n"), mem_failed));
    (void) _soc_th_refresh_modify(unit, 1); /* re-enable refresh */
    return mem_failed;
}

soc_error_t
soc_thx_ser_idb_parity_control_check(int unit, _soc_th_ser_info_t *info_list)
{
    _soc_th_ser_info_t *info;
    int i, j;
    int match = 0;
    uint32 value = 0;
    uint32 rval = 0;
    uint64 rval64;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 total_count = 0;
    uint32 pass_count = 0;

    for (i = 0; ; i++) {
        info = &info_list[i];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            break;
        }

        reg = info->enable_reg;
        if (info->enable_field_list == NULL) {
            field = info->enable_field;
            if (!soc_reg_field_valid(unit, reg, field)) {
                continue;
            }
            if (SOC_REG_IS_64(unit,reg)) {
                COMPILER_64_ZERO(rval64);
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                value = soc_reg64_field32_get(unit, reg, rval64, field);
            } else {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
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
        } else {
            if (SOC_REG_IS_VALID(unit, reg)) {
                if (SOC_REG_IS_64(unit, reg)) {
                    COMPILER_64_ZERO(rval64);
                    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                } else {
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                }
                for (j = 0; ; j++) {
                    if (info->enable_field_list[j] == INVALIDf) {
                        break;
                    }
                    field = info->enable_field_list[j];
                    if (!soc_reg_field_valid(unit, reg, field)) {
                        continue;
                    }
                    if (SOC_REG_IS_64(unit,reg)) {
                        value = soc_reg64_field32_get(unit, reg, rval64, field);
                    } else {
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
    }
    LOG_CLI((BSL_META_U(unit, "\nIDB parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
}

#define THx_XLPORT_INFO 0
#define THx_CLPORT_INFO 1
#define THx_CDPORT_INFO 2
soc_error_t
soc_thx_ser_port_parity_control_check(int unit, _soc_th_ser_info_t *info_list, int type)
{
    int i, j;
    int match = 0;
    int port;
    soc_info_t *si = &SOC_INFO(unit);
     _soc_th_ser_info_t *info;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 value = 0;
    uint32 rval = 0;
    uint64 rval64;
    uint32 total_count = 0;
    uint32 pass_count = 0;

    for (i = 0; ; i++) {
        info = &info_list[i];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            break;
        }

        reg = info->enable_reg;
        if (SOC_REG_IS_VALID(unit, reg)) {
            PBMP_PORT_ITER(unit, port) {
                if ((SOC_PBMP_MEMBER(si->management_pbm, port) && (type != THx_XLPORT_INFO)) ||
                    (!SOC_PBMP_MEMBER(si->management_pbm, port) && (type == THx_XLPORT_INFO))) {
                    continue;
                }
                if (info->enable_field_list == NULL) {
                    field = info->enable_field;
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
                } else {
                    if (SOC_REG_IS_64(unit,reg)) {
                        COMPILER_64_ZERO(rval64);
                        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
                    } else {
                        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
                    }
                    for (j = 0; ; j++) {
                        if (info->enable_field_list[j] == INVALIDf) {
                            break;
                        }
                        field = info->enable_field_list[j];
                        if (SOC_REG_IS_64(unit,reg)) {
                            value = soc_reg64_field32_get(unit, reg, rval64, field);
                        } else {
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
        }
    }
    LOG_CLI((BSL_META_U(unit, "\nPort parity control checked on unit %d: %d\n"),
             unit, total_count));
    LOG_CLI((BSL_META_U(unit, "Passed fields:\t%d\n"), pass_count));
    LOG_CLI((BSL_META_U(unit, "Failed fields::\t%d\n\n"), (total_count - pass_count)));

    return SOC_E_NONE;
}

void
soc_thx_ser_parity_control_check_all(int unit)
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
#if defined(BCM_TOMAHAWK3_SUPPORT) && defined(BCM_56980_A0)
    if (SOC_IS_TOMAHAWK3(unit)) {
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                                (void*) _soc_bcm56980_a0_ip_bus_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                                (void*) _soc_bcm56980_a0_ep_bus_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                                (void*) _soc_bcm56980_a0_ip_buffer_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                                (void*) _soc_bcm56980_a0_ep_buffer_ser_info);
        (void)soc_thx_ser_idb_parity_control_check(unit, _soc_th3_idb_ser_info);
        (void)soc_thx_ser_port_parity_control_check(unit, _soc_th3_pm_cdp_ser_info, THx_CDPORT_INFO);
        (void)soc_thx_ser_port_parity_control_check(unit, _soc_th3_pm_xlp_ser_info, THx_XLPORT_INFO);
    }
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT) && defined(BCM_56970_A0)
    if (SOC_IS_TOMAHAWK2(unit)) {
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                                (void*) _soc_bcm56970_a0_ip_bus_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                                (void*) _soc_bcm56970_a0_ep_bus_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                                (void*) _soc_bcm56970_a0_ip_buffer_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                                (void*) _soc_bcm56970_a0_ep_buffer_ser_info);
        (void)soc_thx_ser_idb_parity_control_check(unit, _soc_th2_idb_ser_info);
        (void)soc_thx_ser_port_parity_control_check(unit, _soc_th2_pm_clp_ser_info, THx_CLPORT_INFO);
        (void)soc_thx_ser_port_parity_control_check(unit, _soc_th2_pm_xlp_ser_info, THx_XLPORT_INFO);
    }
#endif
    if (SOC_IS_TOMAHAWK(unit)) {
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                                (void*) _soc_th_ip_bus_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUS,
                                                (void*) _soc_th_ep_bus_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                                (void*) _soc_th_ip_buffer_ser_info);
        (void)soc_ser_test_parity_control_check(unit, _SOC_SER_TYPE_BUF,
                                                (void*) _soc_th_ep_buffer_ser_info);
        (void)soc_thx_ser_port_parity_control_check(unit, _soc_th_pm_clp_ser_info, THx_CLPORT_INFO);
        (void)soc_thx_ser_port_parity_control_check(unit, _soc_th_pm_xlp_ser_info, THx_XLPORT_INFO);
    }
}

/*
 * Function:
 *      soc_th_ser_test
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
soc_th_ser_test(int unit, _soc_ser_test_t test_type) {
    int errors = 0;
    int def_soc_ser_test_long_sleep;
    int def_soc_ser_test_long_sleep_time_us;

    /* save default values, choose TH specific values  */
    def_soc_ser_test_long_sleep = soc_ser_test_long_sleep;
    def_soc_ser_test_long_sleep_time_us = soc_ser_test_long_sleep_time_us;
    soc_ser_test_long_sleep = TRUE;
    soc_ser_test_long_sleep_time_us =
        soc_property_get(unit, "tr144_long_sleep_time_us",
                         _SOC_TH_TR144_LONG_SLEEP_TIME_US);

    LOG_VERBOSE(BSL_LS_SOC_SER,
        (BSL_META_U(unit,
                    "tr 144 test will use:long_sleep = %d,"
                    "long_sleep_time = %d uS\n"),
         soc_ser_test_long_sleep,
         soc_ser_test_long_sleep_time_us));

    /* Check parity controls before testing */
    soc_thx_ser_parity_control_check_all(unit);
    /*Test TCAM memories*/
    errors += soc_th_ser_tcam_test(unit, test_type);
    /*Test for FIFO memories*/
    errors += soc_th_ser_hardware_test(unit, test_type);

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
soc_th_ser_test_register(int unit)
{
    /*Initialize chip-specific functions for SER testing*/
    memset(&ser_tomahawk_test_fun, 0, sizeof(soc_ser_test_functions_t));
    ser_tomahawk_test_fun.inject_error_f = &soc_th_ser_inject_error;
    ser_tomahawk_test_fun.test_mem = &soc_th_ser_test_mem;
    ser_tomahawk_test_fun.test = &soc_th_ser_test;
    ser_tomahawk_test_fun.parity_control = &soc_th_parity_control_reg_set;
    ser_tomahawk_test_fun.injection_support = &_ser_th_ser_error_injection_support;
    ser_tomahawk_test_fun.correction_info_get = &_ser_th_correction_info_get;
    soc_ser_test_functions_register(unit, &ser_tomahawk_test_fun);
}

#endif /*defined(SER_TR_TEST_SUPPORT)*/

#ifdef BCM_TOMAHAWK2_SUPPORT
int
soc_th2_tcam_engine_enable(int unit, int enable)
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
    }

    return SOC_E_NONE;
}

STATIC soc_error_t _soc_th2_ser_tcam_wrapper_found(soc_mem_t mem)
{
    int i;
    for (i = 0; _soc_th2_tcam_wrapper_table[i].mem != INVALIDm; i++) {
        if (_soc_th2_tcam_wrapper_table[i].mem == mem) {
            return SOC_E_NONE;  /* found */
        }
    }
    return SOC_E_UNAVAIL;
}

STATIC soc_error_t
_soc_th2_ser_tcam_control_reg_get(soc_mem_t mem, soc_reg_t   *ser_control_reg,
                                  soc_field_t *ser_enable_field)
{
    int i;
    for (i = 0; _soc_th2_tcam_wrapper_table[i].mem != INVALIDm; i++) {
        if (_soc_th2_tcam_wrapper_table[i].mem == mem) {
            *ser_control_reg = _soc_th2_tcam_wrapper_table[i].enable_reg;
            *ser_enable_field = _soc_th2_tcam_wrapper_table[i].enable_field;
            return SOC_E_NONE;  /* found */
        }
    }
    return SOC_E_UNAVAIL;
}

STATIC soc_error_t  _soc_th2_tcam_wrapper_enable(int unit, int enable)
{
    int i;
    uint32 val = 0;
    soc_reg_t   ser_control_reg;
    soc_field_t ser_enable_field;

    for (i = 0; _soc_th2_tcam_wrapper_table[i].mem != INVALIDm; i++) {
        ser_control_reg = _soc_th2_tcam_wrapper_table[i].enable_reg;
        ser_enable_field = _soc_th2_tcam_wrapper_table[i].enable_field;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, ser_control_reg, REG_PORT_ANY, 0, &val));
        soc_reg_field_set(unit, ser_control_reg, &val, ser_enable_field, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, ser_control_reg, REG_PORT_ANY, 0, val));
    }
    return SOC_E_NONE;
}

int
soc_th2_vfp_ser_correction(int unit, soc_mem_t mem, int index, int copyno, int entry_dw)
{
    int rv = SOC_E_NONE;
    int i = 0;
    uint32 entry[SOC_MAX_MEM_WORDS], *cache = NULL;
    uint8 *vmap;
    void *null_entry = NULL;

    if (copyno < 0) {
        return SOC_E_INTERNAL;
    }
    cache = SOC_MEM_STATE(unit, mem).cache[copyno];
    vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];

    index = index % 128;
    for (i = 0; i < 8; i++) {
        MEM_LOCK(unit, mem);
        if (cache != NULL && CACHE_VMAP_TST(vmap, index)) {
            sal_memcpy(entry, cache + index * entry_dw, entry_dw * 4);
            if ((rv = soc_mem_write(unit, mem, copyno, index, entry)) < 0) {
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "CACHE_RESTORE %s.%s[%d] failed: %s\n"),
                           SOC_MEM_NAME(unit, mem),
                           SOC_BLOCK_NAME(unit, copyno),
                           index, soc_errmsg(rv)));
            }
        } else {
            null_entry = soc_mem_entry_null(unit, mem);
            if ((rv = soc_mem_write(unit, mem, copyno, index, null_entry)) < 0) {
                LOG_ERROR(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "CLEAR_RESTORE %s.%s[%d] failed: %s\n"),
                           SOC_MEM_NAME(unit, mem),
                           SOC_BLOCK_NAME(unit, copyno),
                           index, soc_errmsg(rv)));
            }
        }
        MEM_UNLOCK(unit, mem);
        index += 128;
    }
    return rv;}
#endif /* BCM_TOMAHAWK2_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
int
soc_th3_check_scrub_info(int unit, soc_mem_t mem, int blk, int copyno,
                        int *num_inst_to_scrub, int *acc_type_list)
{
    int mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    int mmu_base_type, mmu_base_index;
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

    /* L3_DEFIPm/L3_DEFIP_PAIR_128m should be scrubed */
    if ((mem == L3_DEFIP_LEVEL1m) || (mem == L3_DEFIP_PAIR_LEVEL1m)) {
        *num_inst_to_scrub = NUM_PIPE(unit);
        acc_type_list[0] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE1;
        acc_type_list[2] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE2;
        acc_type_list[3] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE3;
        acc_type_list[4] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE4;
        acc_type_list[5] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE5;
        acc_type_list[6] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE6;
        acc_type_list[7] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE7;
    }

    if (mem == EXACT_MATCH_2m || mem == EXACT_MATCH_4m ||
        mem == EXACT_MATCH_ECCm || mem == L3_DEFIP_ALPM_LEVEL3_ECCm ||
        mem == L3_DEFIP_ALPM_LEVEL3_SINGLEm || mem == L3_DEFIP_ALPM_LEVEL3m) {
        *num_inst_to_scrub = 2;
        acc_type_list[0] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE4;
    }

    if (mem == EGR_L3_NEXT_HOPm || mem == L3_DEFIP_ALPM_LEVEL2_SINGLEm ||
        mem == L3_DEFIP_ALPM_LEVEL2m || mem == L3_DEFIP_ALPM_LEVEL2_ECCm) {
        *num_inst_to_scrub = 4;
        acc_type_list[0] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE3;
        acc_type_list[2] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE4;
        acc_type_list[3] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE7;
    }

    if (((mem_acc_type == _SOC_TH_ACC_TYPE_DUPLICATE) &&
         ((blk_type == SOC_BLK_IPIPE) || (blk_type == SOC_BLK_EPIPE)) &&
         (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL)) ||
        SOC_TH_MEM_CHK_TCAM_GLOBAL_UNIQUE_MODE(mem)) {
        /* 2nd last term: sw has not already created per-instance unique views */
        *num_inst_to_scrub = NUM_PIPE(unit);
        acc_type_list[0] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE0;
        acc_type_list[1] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE1;
        acc_type_list[2] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE2;
        acc_type_list[3] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE3;
        acc_type_list[4] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE4;
        acc_type_list[5] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE5;
        acc_type_list[6] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE6;
        acc_type_list[7] = _SOC_TH_ACC_TYPE_UNIQUE_PIPE7;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d\n"),
                    SOC_MEM_NAME(unit,mem), blk, blk_type,
                    _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                    *num_inst_to_scrub));
        return SOC_E_NONE;
    } /* need_per_pipe_scrub */

    if ((mem_acc_type == _SOC_TH_ACC_TYPE_DUPLICATE) &&
        ((blk_type == SOC_BLK_MMU_GLB) ||
         (blk_type == SOC_BLK_MMU_ITM) ||
         (blk_type == SOC_BLK_MMU_EB))) {

        mmu_base_type = SOC_MEM_BASE_TYPE(unit, mem);

        
        switch (blk_type) {
        case SOC_BLK_MMU_ITM:
            switch (mmu_base_type) {
            case SOC_TH3_MMU_BASE_TYPE_IPIPE:
                mmu_base_index = _SOC_MMU_BASE_INDEX(unit, mem, 0x7);
                if ((mmu_base_index == 0) || (mmu_base_index == 1) ||
                    (mmu_base_index == 2) || (mmu_base_index == 3)) {
                    /* ipipe0-3 serviced by ITM0 */
                    *num_inst_to_scrub = 1;
                    acc_type_list[0] = 0;
                } else {
                    /* ipipe4-7 serviced by ITM1 */
                    *num_inst_to_scrub = 1;
                    acc_type_list[0] = 1;
                }
                break;
            case SOC_TH3_MMU_BASE_TYPE_EPIPE:
                /* epipe0-7 serviced by both ITM0,1 */
                *num_inst_to_scrub = 2;
                acc_type_list[0] = 0;
                acc_type_list[1] = 1;
                break;
            case SOC_TH3_MMU_BASE_TYPE_CHIP:
                /* must read each xpe instance */
                *num_inst_to_scrub = 2;
                acc_type_list[0] = 0;
                acc_type_list[1] = 1;
                break;
            default: /* IPORT, EPORT, ITM */
                /* should not find any TH3 MMU memories here */
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                          SOC_MEM_NAME(unit,mem), blk, blk_type,
                          _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                          *num_inst_to_scrub, mmu_base_type));
                return SOC_E_FAIL;
            } /* mmu_base_type */
            break; /* case: SOC_BLK_MMU_ITM */
        case SOC_BLK_MMU_EB:
            switch (mmu_base_type) {
            case SOC_TH3_MMU_BASE_TYPE_IPIPE:
                *num_inst_to_scrub = 8;
                acc_type_list[0] = 0;
                acc_type_list[1] = 1;
                acc_type_list[2] = 2;
                acc_type_list[3] = 3;
                acc_type_list[4] = 4;
                acc_type_list[5] = 5;
                acc_type_list[6] = 6;
                acc_type_list[7] = 7;
                break;
            case SOC_TH3_MMU_BASE_TYPE_EPIPE:
                /* epipe0-7 serviced EB0-7 */
                mmu_base_index = _SOC_MMU_BASE_INDEX(unit, mem, 0x7);
                *num_inst_to_scrub = 1;
                acc_type_list[0] = mmu_base_index;
                break;
            case SOC_TH3_MMU_BASE_TYPE_CHIP:
                /* must read each EB instance */
                *num_inst_to_scrub = 8;
                acc_type_list[0] = 0;
                acc_type_list[1] = 1;
                acc_type_list[2] = 2;
                acc_type_list[3] = 3;
                acc_type_list[4] = 4;
                acc_type_list[5] = 5;
                acc_type_list[6] = 6;
                acc_type_list[7] = 7;
                break;
            case SOC_TH3_MMU_BASE_TYPE_ITM:
                /* must read each EB instance */
                *num_inst_to_scrub = 8;
                acc_type_list[0] = 0;
                acc_type_list[1] = 1;
                acc_type_list[2] = 2;
                acc_type_list[3] = 3;
                acc_type_list[4] = 4;
                acc_type_list[5] = 5;
                acc_type_list[6] = 6;
                acc_type_list[7] = 7;
                break;
            default: /* IPORT, EPORT, EB */
                /* should not find any TH3 MMU memories here */
                LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                          SOC_MEM_NAME(unit,mem), blk, blk_type,
                          _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                          *num_inst_to_scrub, mmu_base_type));
                return SOC_E_FAIL;
            } /* mmu_base_type */
            break; /* case: SOC_BLK_MMU_EB */

        default: /* SOC_BLK_MMU_GLB */
            /* should not find any TH memories here */
            LOG_ERROR(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub_ERROR: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                      SOC_MEM_NAME(unit,mem), blk, blk_type,
                      _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                      *num_inst_to_scrub, mmu_base_type));
            return SOC_E_FAIL;
        } /* blk_type */
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit, "scrub: mem.blk.blk_type %s.%d.%d has acc_type = %s (%d), num_inst_to_scrub = %0d, mmu_base_type = %0d\n"),
                    SOC_MEM_NAME(unit,mem), blk, blk_type,
                    _SOC_TH_ACC_TYPE_NAME(mem_acc_type), mem_acc_type,
                    *num_inst_to_scrub, mmu_base_type));
        return SOC_E_NONE;
    } /* need_per_mmu_membase_scrub */
    return SOC_E_NONE;
}

int
soc_th3_tcam_engine_enable(int unit, int enable)
{
    uint64 config;
    COMPILER_64_ZERO(config);

    if (enable) {

        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, INTERVALf, 64);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, IARB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));

        /* config EP HW ser scan engine */
        COMPILER_64_ZERO(config);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, INTERVALf, 1000000);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, EGR_ARB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));
    } else {

        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, INTERVALf, 64);
        soc_reg64_field32_set(unit, IARB_SER_SCAN_CONFIGr, &config, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, IARB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));

        /* config EP HW ser scan engine */
        COMPILER_64_ZERO(config);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, START_IDXf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, MAX_IDXf, 1023);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, OP_BLOCKf, 0);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, INTERVALf, 1000000);
        soc_reg64_field32_set(unit, EGR_ARB_SER_SCAN_CONFIGr, &config, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg64_set
            (unit, EGR_ARB_SER_SCAN_CONFIGr, REG_PORT_ANY, 0, config));
    }

    return SOC_E_NONE;
}

STATIC soc_error_t _soc_th3_ser_tcam_wrapper_found(soc_mem_t mem)
{
    int i;
    for (i = 0; _soc_bcm56980_a0_tcam_mem_ser_info[i].mem != INVALIDm; i++) {
        if (_soc_bcm56980_a0_tcam_mem_ser_info[i].mem == mem) {
            return SOC_E_NONE;  /* found */
        }
    }
    return SOC_E_UNAVAIL;
}

STATIC soc_error_t
_soc_th3_ser_tcam_control_reg_get(soc_mem_t mem, soc_reg_t
                                  *ser_control_reg,
                                  soc_field_t *ser_enable_field)
{
    int i;
    for (i = 0; _soc_bcm56980_a0_tcam_mem_ser_info[i].mem != INVALIDm; i++) {
        if (_soc_bcm56980_a0_tcam_mem_ser_info[i].mem == mem) {
            *ser_control_reg = _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_reg;
            *ser_enable_field = _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_field;
            return SOC_E_NONE;  /* found */
        }
    }
    return SOC_E_UNAVAIL;
}

STATIC soc_error_t  _soc_th3_tcam_wrapper_enable(int unit, int enable)
{
    int i;
    uint8 fld_val = 0;
    uint32 val = 0;
    uint64 rval64;
    soc_reg_t   ser_control_reg;
    soc_field_t ser_enable_field;

    for (i = 0; _soc_bcm56980_a0_tcam_mem_ser_info[i].mem != INVALIDm; i++) {
        ser_control_reg = _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_reg;
        ser_enable_field = _soc_bcm56980_a0_tcam_mem_ser_info[i].enable_field;

        if (SOC_REG_IS_VALID(unit, ser_control_reg)) {
            if (enable) {
                if (ser_control_reg == IFP_TCAM_CAM_CONTROLr) {
                    fld_val = 0xf;
                } else {
                    fld_val = 1;
                }
            } else {
                fld_val = 0;
            }

            if (SOC_REG_IS_64(unit, ser_control_reg)) {
                COMPILER_64_ZERO(rval64);
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, ser_control_reg, REG_PORT_ANY, 0, &rval64));
                soc_reg64_field32_set(unit, ser_control_reg, &rval64, ser_enable_field,
                                      fld_val);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, ser_control_reg, REG_PORT_ANY, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, ser_control_reg, REG_PORT_ANY, 0, &val));
                soc_reg_field_set(unit, ser_control_reg, &val, ser_enable_field, fld_val);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, ser_control_reg, REG_PORT_ANY, 0, val));
            }
        }
    }
    return SOC_E_NONE;
}

int
soc_tomahawk3_mem_ser_control(int unit, soc_mem_t mem, int copyno,
                             int enable)
{
    soc_reg_t reg = INVALIDr;
    uint32 rval;

    if (!enable) {
        /* disable FP_REFRESH */
        reg = AUX_ARB_CONTROLr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, FP_REFRESH_ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    if (enable) {
        if (soc_property_get(unit, spn_PARITY_ENABLE, FALSE)) {
            SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, enable));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table_all(unit, enable));
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, enable));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table_all(unit, enable));
    }

    if (enable) {
        /*Initialize MMU memory*/
        rval = 0;
        reg = MMU_GLBCFG_MISCCONFIGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, ECCP_ENf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        sal_usleep(20);
        soc_reg_field_set(unit, reg, &rval, INIT_MEMf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        soc_reg_field_set(unit, reg, &rval, INIT_MEMf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        /* enable FP_REFRESH */
        reg = AUX_ARB_CONTROLr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, FP_REFRESH_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

int
soc_th3_mmu_mem_blk_remap(soc_mem_t mem, uint32 *block)
{
    /* In mmu QSCH memories belong to EB block, but if there is ser error on
       these table, the ser event is reported to GLB ser fifo. So need to
       remap the block back to MMU_EB */
    if (mem == MMU_QSCH_L0_ACCUM_COMP_MEMm || mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE0m ||
        mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE1m || mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE2m ||
        mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE3m || mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE4m ||
        mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE5m || mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE6m ||
        mem == MMU_QSCH_L0_ACCUM_COMP_MEM_PIPE7m || mem == MMU_QSCH_L0_CREDIT_MEMm ||
        mem == MMU_QSCH_L0_CREDIT_MEM_PIPE0m || mem == MMU_QSCH_L0_CREDIT_MEM_PIPE1m ||
        mem == MMU_QSCH_L0_CREDIT_MEM_PIPE2m || mem == MMU_QSCH_L0_CREDIT_MEM_PIPE3m ||
        mem == MMU_QSCH_L0_CREDIT_MEM_PIPE4m || mem == MMU_QSCH_L0_CREDIT_MEM_PIPE5m ||
        mem == MMU_QSCH_L0_CREDIT_MEM_PIPE6m || mem == MMU_QSCH_L0_CREDIT_MEM_PIPE7m ||
        mem == MMU_QSCH_L0_FIRST_MEMm || mem == MMU_QSCH_L0_FIRST_MEM_PIPE0m ||
        mem == MMU_QSCH_L0_FIRST_MEM_PIPE1m || mem == MMU_QSCH_L0_FIRST_MEM_PIPE2m ||
        mem == MMU_QSCH_L0_FIRST_MEM_PIPE3m || mem == MMU_QSCH_L0_FIRST_MEM_PIPE4m ||
        mem == MMU_QSCH_L0_FIRST_MEM_PIPE5m || mem == MMU_QSCH_L0_FIRST_MEM_PIPE6m ||
        mem == MMU_QSCH_L0_FIRST_MEM_PIPE7m || mem == MMU_QSCH_L0_WEIGHT_MEMm ||
        mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE0m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE1m ||
        mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE2m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE3m ||
        mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE4m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE5m ||
        mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE6m || mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE7m ||
        mem == MMU_QSCH_L1_ACCUM_COMP_MEMm || mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE0m ||
        mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE1m || mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE2m ||
        mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE3m || mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE4m ||
        mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE5m || mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE6m ||
        mem == MMU_QSCH_L1_ACCUM_COMP_MEM_PIPE7m || mem == MMU_QSCH_L1_CREDIT_MEMm ||
        mem == MMU_QSCH_L1_CREDIT_MEM_PIPE0m || mem == MMU_QSCH_L1_CREDIT_MEM_PIPE1m ||
        mem == MMU_QSCH_L1_CREDIT_MEM_PIPE2m || mem == MMU_QSCH_L1_CREDIT_MEM_PIPE3m ||
        mem == MMU_QSCH_L1_CREDIT_MEM_PIPE4m || mem == MMU_QSCH_L1_CREDIT_MEM_PIPE5m ||
        mem == MMU_QSCH_L1_CREDIT_MEM_PIPE6m || mem == MMU_QSCH_L1_CREDIT_MEM_PIPE7m ||
        mem == MMU_QSCH_L1_FIRST_MEMm || mem == MMU_QSCH_L1_FIRST_MEM_PIPE0m ||
        mem == MMU_QSCH_L1_FIRST_MEM_PIPE1m || mem == MMU_QSCH_L1_FIRST_MEM_PIPE2m ||
        mem == MMU_QSCH_L1_FIRST_MEM_PIPE3m || mem == MMU_QSCH_L1_FIRST_MEM_PIPE4m ||
        mem == MMU_QSCH_L1_FIRST_MEM_PIPE5m || mem == MMU_QSCH_L1_FIRST_MEM_PIPE6m ||
        mem == MMU_QSCH_L1_FIRST_MEM_PIPE7m || mem == MMU_QSCH_L1_WEIGHT_MEMm ||
        mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE0m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE1m ||
        mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE2m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE3m ||
        mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE4m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE5m ||
        mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE6m || mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE7m ||
        mem == MMU_QSCH_L2_ACCUM_COMP_MEMm || mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE0m ||
        mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE1m || mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE2m ||
        mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE3m || mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE4m ||
        mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE5m || mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE6m ||
        mem == MMU_QSCH_L2_ACCUM_COMP_MEM_PIPE7m || mem == MMU_QSCH_L2_CREDIT_MEMm ||
        mem == MMU_QSCH_L2_CREDIT_MEM_PIPE0m || mem == MMU_QSCH_L2_CREDIT_MEM_PIPE1m ||
        mem == MMU_QSCH_L2_CREDIT_MEM_PIPE2m || mem == MMU_QSCH_L2_CREDIT_MEM_PIPE3m ||
        mem == MMU_QSCH_L2_CREDIT_MEM_PIPE4m || mem == MMU_QSCH_L2_CREDIT_MEM_PIPE5m ||
        mem == MMU_QSCH_L2_CREDIT_MEM_PIPE6m || mem == MMU_QSCH_L2_CREDIT_MEM_PIPE7m ||
        mem == MMU_QSCH_L2_WEIGHT_MEMm || mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE0m ||
        mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE1m || mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE2m ||
        mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE3m || mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE4m ||
        mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE5m || mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE6m ||
        mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE7m ) {
        *block = 4; /* MMU_EB */
        return SOC_E_NONE;
    }

    if (mem == MMU_RL_RQE_FIFO_MEMm || mem == MMU_RL_RQE_FIFO_MEM_ITM0m ||
        mem == MMU_RL_RQE_FIFO_MEM_ITM1m || mem == MMU_RL_FBANK0m ||
        mem == MMU_RL_FBANK0_ITM0m || mem == MMU_RL_FBANK0_ITM1m ||
        mem == MMU_RL_FBANK1m || mem == MMU_RL_FBANK1_ITM0m ||
        mem == MMU_RL_FBANK1_ITM1m || mem == MMU_RL_FBANK2m ||
        mem == MMU_RL_FBANK2_ITM0m || mem == MMU_RL_FBANK2_ITM1m ||
        mem == MMU_RL_FBANK3m || mem == MMU_RL_FBANK3_ITM0m ||
        mem == MMU_RL_FBANK3_ITM1m || mem == MMU_RL_FBANK4m ||
        mem == MMU_RL_FBANK4_ITM0m || mem == MMU_RL_FBANK4_ITM1m ||
        mem == MMU_RL_FBANK5m || mem == MMU_RL_FBANK5_ITM0m ||
        mem == MMU_RL_FBANK5_ITM1m || mem == MMU_RL_FBANK6m ||
        mem == MMU_RL_FBANK6_ITM0m || mem == MMU_RL_FBANK6_ITM1m ||
        mem == MMU_RL_FBANK7m || mem == MMU_RL_FBANK7_ITM0m ||
        mem == MMU_RL_FBANK7_ITM1m || mem == MMU_RL_FBANK8m ||
        mem == MMU_RL_FBANK8_ITM0m || mem == MMU_RL_FBANK8_ITM1m ||
        mem == MMU_RL_FBANK9m || mem == MMU_RL_FBANK9_ITM0m ||
        mem == MMU_RL_FBANK9_ITM1m || mem == MMU_RL_FBANK10m ||
        mem == MMU_RL_FBANK10_ITM0m || mem == MMU_RL_FBANK10_ITM1m ||
        mem == MMU_RL_FBANK11m || mem == MMU_RL_FBANK11_ITM0m ||
        mem == MMU_RL_FBANK11_ITM1m || mem == MMU_RL_FBANK12m ||
        mem == MMU_RL_FBANK12_ITM0m || mem == MMU_RL_FBANK12_ITM1m ||
        mem == MMU_RL_FBANK13m || mem == MMU_RL_FBANK13_ITM0m ||
        mem == MMU_RL_FBANK13_ITM1m || mem == MMU_RL_FBANK14m ||
        mem == MMU_RL_FBANK14_ITM0m || mem == MMU_RL_FBANK14_ITM1m ||
        mem == MMU_RL_FBANK15m || mem == MMU_RL_FBANK15_ITM0m ||
        mem == MMU_RL_FBANK15_ITM1m || mem == MMU_THDI_PORT_PG_MIN_COUNTERm ||
        mem == MMU_THDI_PORT_PG_SHARED_BSTm ||
        mem == MMU_THDI_PORT_PG_HDRM_BSTm ||
        mem == MMU_THDO_BST_TOTAL_QUEUEm ||
        mem == MMU_THDO_BST_SHARED_PORTSP_MCm ||
        mem == MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm ||
        mem == MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTAT_ITM0m ||
        mem == MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTAT_ITM1m ||
        mem == MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm ||
        mem == MMU_THDO_BST_TOTAL_QUEUE_PKTSTAT_ITM0m ||
        mem == MMU_THDO_BST_TOTAL_QUEUE_PKTSTAT_ITM1m ||
        mem == MMU_THDO_QUEUE_DROP_COUNTm ||
        mem == MMU_THDO_QUEUE_DROP_COUNT_ITM0m ||
        mem == MMU_THDO_QUEUE_DROP_COUNT_ITM1m ||
        mem == MMU_THDO_PORT_DROP_COUNT_UCm ||
        mem == MMU_THDO_PORT_DROP_COUNT_UC_ITM0m ||
        mem == MMU_THDO_PORT_DROP_COUNT_UC_ITM1m ||
        mem == MMU_THDO_PORT_DROP_COUNT_MCm ||
        mem == MMU_THDO_PORT_DROP_COUNT_MC_ITM0m ||
        mem == MMU_THDO_PORT_DROP_COUNT_MC_ITM1m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNTm ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE0m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE1m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE2m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE3m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE4m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE5m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE6m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNT_PIPE7m ||
        mem == MMU_THDO_SRC_PORT_DROP_COUNTm) {
        *block = 3; /* MMU_ITM */
        return SOC_E_NONE;
    }

    return SOC_E_NOT_FOUND;
}

STATIC int
_soc_th3_reg32_par_en_modify(int unit, soc_reg_t reg, soc_field_t field, int enable)
{
    uint64 rval64;
    uint32 rval;
    int port = REG_PORT_ANY;
    static char *mmu_base_type[8] = {
        "IPORT", "EPORT", "IPIPE", "EPIPE", "CHIP", "ITM"};

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
                case 5: /* ITM */
                    max_pipe = NUM_ITM(unit); break;
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

#ifdef BCM_CMICX_SUPPORT
void soc_th3_ser_intr_handler(int unit,uint32 cmic_val,int reg_num)
{
    soc_reg_t ser_intr_reg[4] = {ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG3r};
    uint32 mask = 0;
    int i = 0;

    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, ser_intr_reg[reg_num], REG_PORT_ANY, 0),
                           &mask);
    cmic_val &= mask;

    for(i = 0; i < 32; i++) {
        switch(reg_num){
            case 0:
                if(cmic_val & 1 << i) {
                    if((i >= 4 && i <= 20) || (i == 1) || (i == 0)) {
                        (void)soc_ser_top_intr_reg_enable(unit, 0, i, 0);
                        if (soc_ser_parity_error_cmicx_intr(INT_TO_PTR(unit), 0, 0,
                                                            INT_TO_PTR(0),
                                                            INT_TO_PTR(i))) {
                        } else {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "soc_ser_cmicx_intr_handler unit %d: "
                                                  "Disabling unhandled interrupt(s): %d\n"),
                                                  unit, PTR_TO_INT(i)));
                            (void)soc_ser_top_intr_reg_enable(unit, 0, i, 0);
                        }
                    }
                }
                break;
            case 1:
                if (cmic_val & 1 << i) {
                    if (i >= 16 && i <= 31) {
                        (void)soc_ser_top_intr_reg_enable(unit, 1, i, 0);
                        if (soc_ser_parity_error_cmicx_intr(INT_TO_PTR(unit), 0, 0,
                                                            INT_TO_PTR(1),
                                                            INT_TO_PTR(i))) {
                        } else {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "soc_ser_cmicx_intr_handler unit %d: "
                                                  "Disabling unhandled interrupt(s): %d\n"),
                                       unit, PTR_TO_INT(i)));
                            (void)soc_ser_top_intr_reg_enable(unit, 1, i, 0);
                        }
                    }
                }
                break;
            case 2:
                if (cmic_val & 1 << i) {
                    (void)soc_ser_top_intr_reg_enable(unit, 2, i, 0);
                    if (soc_ser_parity_error_cmicx_intr(INT_TO_PTR(unit), 0, 0,
                                                        INT_TO_PTR(2),
                                                        INT_TO_PTR(i))) {
                    } else {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_ser_cmicx_intr_handler unit %d: "
                                              "Disabling unhandled interrupt(s): %d\n"),
                                   unit, PTR_TO_INT(i)));
                        (void)soc_ser_top_intr_reg_enable(unit, 2, i, 0);
                    }
                }
                break;
            case 3:
                if (cmic_val & 1 << i) {
                    if (i >= 0 && i <= 16) {
                        (void)soc_ser_top_intr_reg_enable(unit, 3, i, 0);
                        if (soc_ser_parity_error_cmicx_intr(INT_TO_PTR(unit), 0, 0,
                                                            INT_TO_PTR(3),
                                                            INT_TO_PTR(i))) {
                        } else {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "soc_ser_cmicx_intr_handler unit %d: "
                                                  "Disabling unhandled interrupt(s): %d\n"),
                                       unit, PTR_TO_INT(i)));
                            (void)soc_ser_top_intr_reg_enable(unit, 3, i, 0);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}
#endif

STATIC int _soc_tomahawk3_idb_enable(int unit, soc_reg_t reg_name, int enable, int *is_idb)
{
    int i, j, pipe, pm_num;
    uint64 rval64;
    uint32 rval,rval1;
    uint32 addr;
    int block;
    uint8 acc_type;
    soc_reg_t reg = TOP_TSC_ENABLEr;
    soc_reg_t obm_ctrl_enable[] =
                { IDB_OBM0_SER_CONTROLr,
                  IDB_OBM1_SER_CONTROLr,
                  IDB_OBM2_SER_CONTROLr,
                  IDB_OBM3_SER_CONTROLr };
    soc_reg_t ca_ctrl_enable[] =
                { IDB_CA0_SER_CONTROLr,
                  IDB_CA1_SER_CONTROLr,
                  IDB_CA2_SER_CONTROLr,
                  IDB_CA3_SER_CONTROLr };

    for (i = 0; i < _TH3_PBLKS_PER_PIPE; i++) {
        if ((reg_name == obm_ctrl_enable[i]) || (reg_name == ca_ctrl_enable[i])) {
            *is_idb = 1;
            break;
        }
    }

    if (i == _TH3_PBLKS_PER_PIPE) {
        *is_idb = 0;
        return SOC_E_NOT_FOUND;
    }

    addr = soc_reg_addr_get(unit, reg_name, REG_PORT_ANY, 0,
                             SOC_REG_ADDR_OPTION_NONE,
                             &block, &acc_type);
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval1));
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        pm_num = pipe * _TH3_PBLKS_PER_PIPE + i;

        if (rval1 & (1 << pm_num)) {
            if (SOC_REG_IS_64(unit, reg_name)) {
                COMPILER_64_ZERO(rval64);
                SOC_IF_ERROR_RETURN
                    (_soc_reg64_get(unit, block, pipe, addr, &rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (_soc_reg32_get(unit, block, pipe, addr, &rval));
            }
            if (reg_name == obm_ctrl_enable[i]) {
                j = 0;
                while(_soc_th3_idb_obm_enable_fields[j] != INVALIDf) {
                    soc_reg_field_set(unit, reg_name, &rval,
                                      _soc_th3_idb_obm_enable_fields[j],
                                      enable ? 1 : 0);
                    j++;
                }
            } else if (reg_name == ca_ctrl_enable[i]){
                j = 0;
                while(_soc_th3_idb_ca_fifo_enable_fields[j] != INVALIDf) {
                    soc_reg64_field32_set(unit, reg_name, &rval64,
                                          _soc_th3_idb_ca_fifo_enable_fields[j],
                                          enable ? 1 : 0);
                    j++;
                }
            }

            if (SOC_REG_IS_64(unit, reg_name)) {
                SOC_IF_ERROR_RETURN
                    (_soc_reg64_set(unit, block, pipe, addr, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (_soc_reg32_set(unit, block, pipe, addr, rval));
            }
        }
    }

    return SOC_E_NONE;
}

int soc_th3_ser_test_skip_by_at(int unit, soc_mem_t mem, soc_acc_type_t at)
{
    uint16 dev_id;
    uint8 rev_id;
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56983_DEVICE_ID) {
        if ((at == _SOC_UNIQUE_ACC_TYPE_PIPE_2) ||
            (at == _SOC_UNIQUE_ACC_TYPE_PIPE_3) ||
            (at == _SOC_UNIQUE_ACC_TYPE_PIPE_4) ||
            (at == _SOC_UNIQUE_ACC_TYPE_PIPE_5)) {
            return TRUE;
        }

        if ((mem == MMU_MTRO_L0_A_PIPE2m) || (mem == MMU_MTRO_L0_A_PIPE3m) ||
            (mem == MMU_MTRO_L0_A_PIPE4m) || (mem == MMU_MTRO_L0_A_PIPE5m) ||
            (mem == MMU_MTRO_L0_B_PIPE2m) || (mem == MMU_MTRO_L0_B_PIPE3m) ||
            (mem == MMU_MTRO_L0_B_PIPE4m) || (mem == MMU_MTRO_L0_B_PIPE5m) ||
            (mem == MMU_MTRO_L0_C_PIPE2m) || (mem == MMU_MTRO_L0_C_PIPE3m) ||
            (mem == MMU_MTRO_L0_C_PIPE4m) || (mem == MMU_MTRO_L0_C_PIPE5m) ||
            (mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE2m) ||
            (mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE3m) ||
            (mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE4m) ||
            (mem == MMU_QSCH_L0_WEIGHT_MEM_PIPE5m) ||
            (mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE2m) ||
            (mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE3m) ||
            (mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE4m) ||
            (mem == MMU_QSCH_L1_WEIGHT_MEM_PIPE5m) ||
            (mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE2m) ||
            (mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE3m) ||
            (mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE4m) ||
            (mem == MMU_QSCH_L2_WEIGHT_MEM_PIPE5m) ||
            (mem == MMU_THDI_PORTSP_CONFIG1_PIPE2m) ||
            (mem == MMU_THDI_PORTSP_CONFIG1_PIPE3m) ||
            (mem == MMU_THDI_PORTSP_CONFIG1_PIPE4m) ||
            (mem == MMU_THDI_PORTSP_CONFIG1_PIPE5m) ||
            (mem == MMU_THDI_PORTSP_CONFIG_PIPE2m) ||
            (mem == MMU_THDI_PORTSP_CONFIG_PIPE3m) ||
            (mem == MMU_THDI_PORTSP_CONFIG_PIPE4m) ||
            (mem == MMU_THDI_PORTSP_CONFIG_PIPE5m) ||
            (mem == MMU_THDI_PORT_BST_CONFIG_PIPE2m) ||
            (mem == MMU_THDI_PORT_BST_CONFIG_PIPE3m) ||
            (mem == MMU_THDI_PORT_BST_CONFIG_PIPE4m) ||
            (mem == MMU_THDI_PORT_BST_CONFIG_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_HDRM_CONFIG_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_HDRM_CONFIG_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_HDRM_CONFIG_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_HDRM_CONFIG_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG1_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_MIN_CONFIG_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG1_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_RESUME_CONFIG_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG1_PIPE5m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE2m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE3m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE4m) ||
            (mem == MMU_THDI_PORT_PG_SHARED_CONFIG_PIPE5m)) {
            return TRUE;
        }
    }

    return FALSE;
}

int soc_th3_ser_pipe_skip(int unit, soc_pipe_select_t pipe)
{
    int skip = FALSE;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (dev_id == BCM56983_DEVICE_ID) {
        if ((pipe == SOC_PIPE_SELECT_2) ||
            (pipe == SOC_PIPE_SELECT_3) ||
            (pipe == SOC_PIPE_SELECT_4) ||
            (pipe == SOC_PIPE_SELECT_5)) {
            skip = TRUE;
        }
    }

    return skip;
}
#endif
#endif /* BCM_TOMAHAWK_SUPPORT */
