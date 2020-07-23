/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        triumph.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/firebolt.h>
#include <soc/bradley.h>
#include <soc/triumph.h>
#include <soc/triumph2.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>
#include <soc/devids.h>
#include <soc/l2x.h>
#include <soc/pbsmh.h>
#include <soc/dcbformats.h>

#ifdef BCM_TRIUMPH_SUPPORT

/*
 * Triumph chip driver functions.  
 */
soc_functions_t soc_triumph_drv_funs = {
    soc_triumph_misc_init,
    soc_triumph_mmu_init,
    soc_triumph_age_timer_get,
    soc_triumph_age_timer_max_get,
    soc_triumph_age_timer_set,
};

typedef enum {
    _SOC_PARITY_INFO_TYPE_GENERIC,
    _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
    _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
    _SOC_PARITY_INFO_TYPE_DUAL_TABLE1,
    _SOC_PARITY_INFO_TYPE_DUAL_TABLE2,
    _SOC_PARITY_INFO_TYPE_MMUIPMC,
    _SOC_PARITY_INFO_TYPE_MMUWRED,
    _SOC_PARITY_INFO_TYPE_MMUMTRO
} _soc_parity_info_type_t;

typedef struct _soc_parity_info_s {
    soc_field_t             enable_field;
    soc_field_t             error_field;
    char                    *msg;
    soc_mem_t               mem;
    _soc_parity_info_type_t type;
    soc_reg_t               control_reg;
    soc_reg_t               status0_reg;
    soc_reg_t               status1_reg;
} _soc_parity_info_t;

STATIC _soc_parity_info_t _soc_tr_ip0_parity_info[] = {
    { VXLT_PAR_ERRf, VXLT_PAR_ERRf, NULL,
      VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE1,
      VLAN_XLATE_PARITY_CONTROLr,
      VLAN_XLATE_PARITY_STATUSr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_tr_ip1_parity_info[] = {
    { VLAN_PAR_ERRf, VLAN_PAR_ERRf, NULL,
      VLAN_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      VLAN_PARITY_CONTROLr,
      VLAN_PARITY_STATUSr, INVALIDr },
    { SVP_PAR_ERRf, SVP_PAR_ERRf, NULL,
      SOURCE_VPm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      SOURCE_VP_PARITY_CONTROLr,
      SOURCE_VP_PARITY_STATUSr, INVALIDr },
    { L3_IIF_PAR_ERRf, L3_IIF_PAR_ERRf, NULL,
      L3_IIFm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      L3_IIF_PARITY_CONTROLr,
      L3_IIF_PARITY_STATUSr, INVALIDr },
    { MPLS_ENTRY_PAR_ERRf, MPLS_ENTRY_PAR_ERRf, NULL,
      MPLS_ENTRYm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE1,
      MPLS_ENTRY_PARITY_CONTROLr,
      MPLS_ENTRY_PARITY_STATUSr, INVALIDr },
    { L2_ENTRY_PAR_ERRf, L2_ENTRY_PAR_ERRf, NULL,
      L2Xm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE2,
      L2_ENTRY_PARITY_CONTROLr,
      L2_ENTRY_PARITY_STATUS_0r, L2_ENTRY_PARITY_STATUS_1r },
    { L2MC_PAR_ERRf, L2MC_PAR_ERRf, NULL,
      L2MCm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      L2MC_PARITY_CONTROLr,
      L2MC_PARITY_STATUSr, INVALIDr },
    { L3_ENTRY_PAR_ERRf, L3_ENTRY_PAR_ERRf, NULL,
      L3_ENTRY_ONLYm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE2,
      L3_ENTRY_PARITY_CONTROLr,
      L3_ENTRY_PARITY_STATUS_0r, L3_ENTRY_PARITY_STATUS_1r },
    { L3_DEFIP_DATA_PAR_ERRf, L3_DEFIP_DATA_PAR_ERRf, NULL,
      L3_DEFIP_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      L3_DEFIP_PARITY_CONTROLr,
      L3_DEFIP_PARITY_STATUSr, INVALIDr },
 /* { IESMIF_INTRf }, */
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_tr_ip2_parity_info[] = {
    { L3_IPMC_PAR_ERRf, L3_IPMC_PAR_ERRf, NULL,
      L3_IPMCm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      L3_IPMC_PARITY_CONTROLr,
      L3_IPMC_PARITY_STATUSr, INVALIDr },
    { INITIAL_ING_NHOP_PAR_ERRf, INITIAL_ING_NHOP_PAR_ERRf, NULL,
      INITIAL_ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      INITIAL_NHOP_PARITY_CONTROLr,
      INITIAL_NHOP_PARITY_STATUSr, INVALIDr },
 /* { IL3MC_ERB_INTRf }, */
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_tr_ip3_parity_info[] = {
    { ING_NHOP_PAR_ERRf, ING_NHOP_PAR_ERRf, NULL,
      ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      ING_L3_NEXT_HOP_PARITY_CONTROLr,
      ING_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr },
    { ING_IPFIX_SESS_PAR_ERRf, ING_IPFIX_SESS_PAR_ERRf, NULL,
      ING_IPFIX_SESSION_TABLEm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE1,
      ING_IPFIX_SESSION_PARITY_CONTROLr,
      ING_IPFIX_SESSION_PARITY_STATUSr, INVALIDr },
    { ING_IPFIX_EXPORT_PAR_ERRf, ING_IPFIX_EXPORT_PAR_ERRf, NULL,
      ING_IPFIX_EXPORT_FIFOm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      ING_IPFIX_EXPORT_FIFO_PARITY_CONTROLr,
      ING_IPFIX_EXPORT_FIFO_PARITY_STATUSr, INVALIDr },
    { L3_MTU_VALUES_PAR_ERRf, L3_MTU_VALUES_PAR_ERRf, NULL,
      L3_MTU_VALUESm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      L3_MTU_VALUES_PARITY_CONTROLr,
      L3_MTU_VALUES_PARITY_STATUSr, INVALIDr },
    { EGR_MASK_PAR_ERRf, EGR_MASK_PAR_ERRf, NULL,
      EGR_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_MASK_PARITY_CONTROLr,
      EGR_MASK_PARITY_STATUSr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_tr_ep_parity_info[] = {
    { EGR_NHOP_PAR_ERRf, EGR_NHOP_PAR_ERRf, NULL,
      EGR_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_L3_NEXT_HOP_PARITY_CONTROLr,
      EGR_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr },
    { EGR_L3_INTF_PAR_ERRf, EGR_L3_INTF_PAR_ERRf, NULL,
      EGR_L3_INTFm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_L3_INTF_PARITY_CONTROLr,
      EGR_L3_INTF_PARITY_STATUSr, INVALIDr },
    { EGR_MPLS_LABEL_PAR_ERRf, EGR_MPLS_LABEL_PAR_ERRf, NULL,
      EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_CONTROLr,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_STATUSr, INVALIDr },
    { EGR_VLAN_PAR_ERRf, EGR_VLAN_PAR_ERRf, NULL,
      EGR_VLANm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_VLAN_PARITY_CONTROLr,
      EGR_VLAN_PARITY_STATUSr, INVALIDr },
    { EGR_VXLT_PAR_ERRf, EGR_VXLT_PAR_ERRf, NULL,
      EGR_VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE1,
      EGR_VLAN_XLATE_PARITY_CONTROLr,
      EGR_VLAN_XLATE_PARITY_STATUSr, INVALIDr },
    { EGR_IP_TUNNEL_PAR_ERRf, EGR_IP_TUNNEL_PAR_ERRf, NULL,
      EGR_IP_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_IP_TUNNEL_PARITY_CONTROLr,
      EGR_IP_TUNNEL_PARITY_STATUSr, INVALIDr },
    { EGR_PW_COUNT_PAR_ERRf, EGR_PW_COUNT_PAR_ERRf, NULL,
      EGR_PW_INIT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_PW_INIT_COUNTERS_PARITY_CONTROLr,
      EGR_PW_INIT_COUNTERS_PARITY_STATUSr, INVALIDr },
    { EGR_IPFIX_SESS_PAR_ERRf, EGR_IPFIX_SESS_PAR_ERRf, NULL,
      EGR_IPFIX_SESSION_TABLEm, _SOC_PARITY_INFO_TYPE_DUAL_TABLE1,
      EGR_IPFIX_SESSION_PARITY_CONTROLr,
      EGR_IPFIX_SESSION_PARITY_STATUSr, INVALIDr },
    { EGR_IPFIX_EXPORT_PAR_ERRf, EGR_IPFIX_EXPORT_PAR_ERRf, NULL,
      EGR_IPFIX_EXPORT_FIFOm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_IPFIX_EXPORT_FIFO_PARITY_CONTROLr,
      EGR_IPFIX_EXPORT_FIFO_PARITY_STATUSr, INVALIDr },
    { EP_INITBUF_DBEf, EP_INITBUF_DBEf, "EP_INITBUF",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_DBEr, INVALIDr },
 /* { EP_INITBUF_SBEf, EP_INITBUF_SBEf, NULL,
      EGR_INITBUFm, _SOC_PARITY_INFO_TYPE_SPECIAL,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_SBEr, INVALIDr }, */
    { INVALIDf, INVALIDf }, /* table terminator */
};

/*
 * Is DEQ_CBPERRPTRr for MMU_CBPDATA0 - MMU_CBPDATA31 (15-bit address)?
 * How does MMU_CELLCHKm (15-bit address) report parity error?
 */
STATIC _soc_parity_info_t _soc_tr_mmu_parity_info[] = {
    { AGING_CTR_PAR_ERR_ENf, AGING_CTR_PAR_ERRf, NULL,
      MMU_AGING_CTRm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { AGING_EXP_PAR_ERR_ENf, AGING_EXP_PAR_ERRf, NULL,
      MMU_AGING_EXPm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { CCP_PAR_ERR_ENf, CCP_PAR_ERRf, NULL,
      MMU_CCP_MEMm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      CCPPARITYERRORPTRr, INVALIDr },
    { CFAP_PAR_ERR_ENf, CFAP_PAR_ERRf, NULL,
      MMU_CFAP_MEMm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      CFAPPARITYERRORPTRr, INVALIDr },
    { CFAP_MEM_FAIL_ENf, CFAP_MEM_FAILf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR_CPU_ERR_ENf, DEQ_PKTHDR_CPU_ERRf, NULL,
      MMU_CBPPKTHEADERCPUm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      DEQ_PKTHDRCPUERRPTRr, INVALIDr },
    { DEQ_PKTHDR0_ERR_ENf, DEQ_PKTHDR0_ERRf, NULL,
      MMU_CBPPKTHEADER0_MEM0m, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      DEQ_PKTHDR0ERRPTRr, INVALIDr },
    { DEQ0_NOT_IP_ERR_ENf, DEQ0_NOT_IP_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ0_CELLCRC_ERR_ENf, DEQ0_CELLCRC_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { START_BY_START_ERR_ENf, START_BY_START_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { MEM1_IPMC_TBL_PAR_ERR_ENf, MEM1_IPMC_TBL_PAR_ERRf, "MMU_IPMC_GROUP_TBL",
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUIPMC,
      INVALIDr,
      MEM1_IPMCGRP_TBL_PARITYERRORPTRr, MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr },
    { MEM1_VLAN_TBL_PAR_ERR_ENf, MEM1_VLAN_TBL_PAR_ERRf, NULL,
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      MEM1_IPMCVLAN_TBL_PARITYERRORPTRr, INVALIDr },
    { TOQ0_CELLHDR_PAR_ERR_ENf, TOQ0_CELLHDR_PAR_ERRf, NULL,
      MMU_CBPCELLHEADERm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      TOQ_CELLHDRERRPTRr, INVALIDr },
    { TOQ0_PKTHDR1_PAR_ERR_ENf, TOQ0_PKTHDR1_PAR_ERRf, NULL,
      MMU_CBPPKTHEADER1_MEM0m, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      TOQ_PKTHDR1ERRPTRr, INVALIDr },
    { TOQ0_PKTLINK_PAR_ERR_ENf, TOQ0_PKTLINK_PAR_ERRf, NULL,
      MMU_PKTLINK0m, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      TOQ_PKTLINKERRPTRr, INVALIDr },
    { TOQ0_CELLLINK_PAR_ERR_ENf, TOQ0_CELLLINK_PAR_ERRf, NULL,
      MMU_CELLLINKm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      TOQ_CELLLINKERRPTRr, INVALIDr },
    { TOQ0_IPMC_TBL_PAR_ERR_ENf, TOQ0_IPMC_TBL_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUIPMC,
      INVALIDr,
      TOQ_IPMCGRPERRPTR0r, TOQ_ERRINTR1r },
    { TOQ0_VLAN_TBL_PAR_ERR_ENf, TOQ0_VLAN_TBL_PAR_ERRf, NULL,
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      TOQ_IPMCVLANERRPTRr, INVALIDr },
    { MTRO_PAR_ERR_ENf, MTRO_PAR_ERRf, NULL,
      INVALIDm,_SOC_PARITY_INFO_TYPE_MMUMTRO,
      INVALIDr,
      EGRSHAPEPARITYERRORPTRr, INVALIDr },
    { PFAP_PAR_ERR_ENf, PFAP_PAR_ERRf, NULL,
      MMU_PFAP_MEMm, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      PFAPPARITYERRORPTRr, INVALIDr },
    { PFAP_MEM_FAIL_ENf, PFAP_MEM_FAILf, NULL,
      MMU_PFAP_MEMm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { WRED_PAR_ERR_ENf, WRED_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUWRED,
      INVALIDr,
      WRED_PARITY_ERROR_INFOr, WRED_PARITY_ERROR_BITMAPr },
    { DEQ_PKTHDR2_ERR_ENf, DEQ_PKTHDR2_PAR_ERRf, NULL,
      MMU_CBPPKTHEADER2m, _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1,
      INVALIDr,
      DEQ_PKTHDR2ERRPTRr, INVALIDr },
    /* what memory is this for? use E2EFC_PARITYERRORPTRr.PTRf for index */
    { E2EFC_ERR_ENf, E2EFC_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC struct {
    uint32             cpi_bit;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    _soc_parity_info_t *info;
} _soc_tr_parity_block_info[] = {
    { 0x01, MEM_FAIL_INT_ENr, MEM_FAIL_INT_STATr, _soc_tr_mmu_parity_info },
    { 0x02, EP_INTR_ENABLEr, EP_INTR_STATUSr, _soc_tr_ep_parity_info },
    { 0x04, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, _soc_tr_ip0_parity_info },
    { 0x08, IP1_INTR_ENABLEr, IP1_INTR_STATUSr, _soc_tr_ip1_parity_info },
    { 0x10, IP2_INTR_ENABLEr, IP2_INTR_STATUSr, _soc_tr_ip2_parity_info },
    { 0x20, IP3_INTR_ENABLEr, IP3_INTR_STATUSr, _soc_tr_ip3_parity_info },
 /* { 0x40, PCIE_REPLAY_PERR, }, */
    { 0 }, /* table terminator */
};

/* MMU MTRO conversion structure*/
#define MMU_MTRO_MAX_GROUP         19
#define MMU_MTRO_MAX_PORT_IN_GROUP 6
#define   SOC_TRIUMPH_MEM_PARITY_INFO(unit, block_info_idx, field_enum)    \
          ((SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)       \
          | (field_enum & SOC_ERROR_FIELD_ENUM_MASK))

STATIC struct {
    int    num_port;
    int    port_list[MMU_MTRO_MAX_PORT_IN_GROUP];
    int    base_queue;
    int    numq;
} _soc_tr_mtro_group_info[MMU_MTRO_MAX_GROUP] = {
    {1, {1,-1,-1,-1,-1,-1},            0,  8},
    {2, {2, 3,-1,-1,-1,-1},            0, 24},
    {5, {4, 5, 6, 7, 8,-1},            0,  8},
    {5, {9, 10, 11, 12, 13,-1},        0,  8},
    {2, {14, 15,-1,-1,-1,-1},          0, 24},
    {5, {16, 17, 18, 19, 20, -1},      0,  8},
    {5, {21, 22, 23, 24, 25, -1},      0,  8},
    {2, {26, 27,-1,-1,-1,-1},          0, 24},
    {2, {28, 29,-1,-1,-1,-1},          0, 24},
    {2, {30, 31,-1,-1,-1,-1},          0, 24},
    {1, {32, -1,-1,-1,-1,-1},          0, 24},
    {5, {33, 34, 35, 36, 37,-1},       0,  8},
    {5, {38, 39, 40, 41, 42,-1},       0,  8},
    {1, {43,-1,-1,-1,-1,-1},           0, 24},
    {5, {44, 45, 46, 47, 48,-1},       0,  8},
    {5, {49, 50, 51, 52, 53,-1},       0,  8},
    {6, {2, 3, 14, 15, 26, 27},       24,  2},
    {6, {28, 29, 30, 31, 32, 43},     24,  2},
    {1, {REG_PORT_ANY,-1,-1,-1,-1,-1}, 0, 48}
};

static soc_ser_functions_t _tr_ser_functions;

STATIC int
_soc_triumph_parity_enable(int unit, int enable)
{
    int block, table;
    uint32 addr, cmic_enable, block_enable, regval, misc_cfg;
    _soc_parity_info_t *info;
    soc_reg_t block_reg, reg;

    cmic_enable = 0;
    /* loop through each block */
    for (block = 0; _soc_tr_parity_block_info[block].cpi_bit; block++) {
        cmic_enable |= _soc_tr_parity_block_info[block].cpi_bit;
        info = _soc_tr_parity_block_info[block].info;
        block_reg = _soc_tr_parity_block_info[block].enable_reg;
        block_enable = 0;

        /* loop through each table in the block */
        for (table = 0; info[table].enable_field != INVALIDf; table++) {
            /* handle different parity error reporting style */
            switch (info[table].type) {
            case _SOC_PARITY_INFO_TYPE_GENERIC:
            case _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1:
                break;
            case _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2:
            case _SOC_PARITY_INFO_TYPE_DUAL_TABLE1:
            case _SOC_PARITY_INFO_TYPE_DUAL_TABLE2:
                reg = info[table].control_reg;
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr,
                                                    enable ? 1 : 0));
                break;
            default:
                break;
            } /* handle different parity error reporting style */
            soc_reg_field_set(unit, block_reg, &block_enable,
                              info[table].enable_field, enable ? 1 : 0);
        } /* loop through each table in the block */

        addr = soc_reg_addr(unit, block_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, block_enable));
    } /* loop through each block */

    /* Disable parity error support on specific tables */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                               ING_L3_NEXT_HOP_PARITY_CONTROLr,
                                               REG_PORT_ANY, PARITY_ENf, 0x0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, L3_IPMC_PARITY_CONTROLr,
                                               REG_PORT_ANY, PARITY_ENf, 0x0));

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN
        (WRITE_CMIC_CHIP_PARITY_INTR_ENABLEr(unit, cmic_enable));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &regval));
    soc_intr_enable(unit, IRQ_MEM_FAIL);

    return SOC_E_NONE;
}

/*
 * status0_reg has entry index of the parity error
 * or
 * status0_reg == INVALIDr (no furthur information)
 */
STATIC int
_soc_triumph_parity_process_single_table1(int unit, char *msg,
                                          _soc_parity_info_t *info, int block,
                                          int * intr_enable)
{
    _soc_ser_correct_info_t spci;
    soc_reg_t reg;
    uint32 addr, entry_idx;
    uint32 rval;
    int rv = 0;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    reg = info->status0_reg;
    if (reg == INVALIDr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s parity error\n"),
                              unit, msg));
        *intr_enable = 0;
        return SOC_E_NONE;
    }
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &entry_idx));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s entry %d parity error\n"),
               unit, msg, entry_idx));

    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
            SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
            info->error_field);

    if (info->mem != INVALIDm) {
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
        spci.reg = INVALIDr;
        spci.mem = info->mem;
        spci.blk_type = block;
        spci.index = entry_idx;
        rv = soc_ser_correction(unit, &spci);
        if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               info->mem, info->error_field);
        }
    } else {
        *intr_enable = 0;
    }

    /* update soc stats for some cases */
    if (info->error_field == CCP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cpcrc++;
    } else if (info->error_field == CFAP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cfap++;
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    return SOC_E_NONE;
}

/*
 * IP EP parity error info format for single table:
 * XXX_CONTROLr: PARITY_ENf
 * XXX_STATUSr: PARITY_ERRf, MULTIPLE_ERRf, ENTRY_IDXf
 * or
 * XXX_STATUSr: DOUBLE_BIT_ERRf, MULTUPLE_ERRf, ENTRY_IDXf
 */
STATIC int
_soc_triumph_parity_process_single_table2(int unit, char *msg,
                                          _soc_parity_info_t *info, int block,
                                          int * intr_enable)
{
    soc_reg_t reg;
    uint32 addr, entry_idx, status;
    _soc_ser_correct_info_t spci;
    int rv = 0;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    reg = info->status0_reg;
    if (reg == INVALIDr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s parity error\n"),
                              unit, msg));
        *intr_enable = 0;
        return SOC_E_NONE;
    }
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
    if (soc_reg_field_valid(unit, reg, PARITY_ERRf)) {
        if (!soc_reg_field_get(unit, reg, status, PARITY_ERRf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s parity hardware inconsistency\n"),
                       unit, msg));
            *intr_enable = 0;
            return SOC_E_NONE;
        }
        entry_idx = soc_reg_field_get(unit, reg, status,
                                      ENTRY_IDXf);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, entry_idx));

        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                info->error_field);

        if (info->mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            spci.blk_type = block;
            spci.index = entry_idx;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   info->mem, info->error_field);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        } else {
            *intr_enable = 0;
        }
    } else if (soc_reg_field_valid(unit, reg, DOUBLE_BIT_ERRf)) {
        if (!soc_reg_field_get(unit, reg, status,
                               DOUBLE_BIT_ERRf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s parity hardware inconsistency\n"),
                       unit, msg));
            *intr_enable = 0;
            return SOC_E_NONE;
        }
        entry_idx = soc_reg_field_get(unit, reg, status,
                                      ENTRY_IDXf);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d double bit error\n"),
                   unit, msg, entry_idx));

        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                info->error_field);

        if (info->mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            spci.blk_type = block;
            spci.index = entry_idx;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   info->mem, info->error_field);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        } else { 
            *intr_enable = 0;
        } 
    } 

    return SOC_E_NONE;
}

/*
 * IP EP parity error info format 1 for dual table:
 * XXX_CONTROLr: PARITY_ENf
 * XXX_STATUSr: PARITY_ERR_BMf, MULTIPLE_ERRf, BUCKET_IDX_0f,
 *              BUCKET_IDX_1f
 * lower half of PARITY_ERR_BMf is for BUCKET_IDX_0f
 * upper half of PARITY_ERR_BMf is for BUCKET_IDX_1f
 */
STATIC int
_soc_triumph_parity_process_dual_table1(int unit, char *msg,
                                        _soc_parity_info_t *info, int block,
                                        int * intr_enable)
{
    int size;
    int err_count = 0;
    int index, rv = 0;
    soc_reg_t reg;
    uint32 addr, entry_idx, status, bitmap0, bitmap1;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    reg = info->status0_reg;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
    size = soc_reg_field_length(unit, reg, PARITY_ERR_BMf) / 2;
    bitmap0 = soc_reg_field_get(unit, reg, status, PARITY_ERR_BMf);
    bitmap1 = (bitmap0 >> size) & ((1 << size) - 1);
    bitmap0 &= (1 << size) - 1;
    entry_idx =
        soc_reg_field_get(unit, reg, status, BUCKET_IDX_0f) *
        size * 2;
    for (index = 0; index < size; index++, bitmap0 >>= 1) {
        if (!bitmap0) {
            break;
        }
        if (!(bitmap0 & 1)) {
            continue;
        }
        err_count++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, entry_idx + index));

        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                info->error_field);

        if (info->mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            spci.blk_type = block;
            spci.index = entry_idx + index;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   info->mem, info->error_field);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        } else {
            *intr_enable = 0;
        }
    }
    entry_idx =
        soc_reg_field_get(unit, reg, status, BUCKET_IDX_1f) *
        size * 2 + size;
    for (index = 0; index < size; index++, bitmap1 >>= 1) {
        if (!bitmap1) {
            break;
        }
        if (!(bitmap1 & 1)) {
            continue;
        }
        err_count++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, entry_idx + index));

        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                info->error_field);

        if (info->mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            spci.blk_type = block;
            spci.index = entry_idx + index;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   info->mem, info->error_field);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        } else {
            *intr_enable = 0;
        }
    }

    if (err_count == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s parity hardware inconsistency\n"),
                   unit, msg));
    }

    return SOC_E_NONE;
}

/*
 * IP EP parity error info format 2 for dual table:
 * XXX_CONTROLr: PARITY_ENf
 * XXX_STATUS0r: PARITY_ERR_BM_0f, MULTIPLE_ERR_0f,
 *               BUCKET_IDX_0f
 * XXX_STATUS1r: PARITY_ERR_BM_1f, MULTIPLE_ERR_1f,
 *               BUCKET_IDX_1f
 */
STATIC int
_soc_triumph_parity_process_dual_table2(int unit, char *msg,
                                        _soc_parity_info_t *info, int block,
                                        int * intr_enable)
{
    int size, index, rv = 0;
    int err_count = 0;
    soc_reg_t reg;
    uint32 addr, entry_idx, status, bitmap0, bitmap1;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    reg = info->status0_reg;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
    size = soc_reg_field_length(unit, reg, PARITY_ERR_BM_0f);
    bitmap0 = soc_reg_field_get(unit, reg, status,
                                PARITY_ERR_BM_0f);
    entry_idx =
        soc_reg_field_get(unit, reg, status, BUCKET_IDX_0f) *
        size * 2;
    for (index = 0; index < size; index++, bitmap0 >>= 1) {
        if (!bitmap0) {
            break;
        }
        if (!(bitmap0 & 1)) {
            continue;
        }
        err_count++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, entry_idx + index));

        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                info->error_field);

        if (info->mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            spci.blk_type = block;
            spci.index = entry_idx + index;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   info->mem, info->error_field);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        } else {
            *intr_enable = 0;
        }
    }
    reg = info->status1_reg;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
    size = soc_reg_field_length(unit, reg, PARITY_ERR_BM_1f);
    bitmap1 = soc_reg_field_get(unit, reg, status,
                                PARITY_ERR_BM_1f);
    entry_idx =
        soc_reg_field_get(unit, reg, status, BUCKET_IDX_1f) *
        size * 2 + size;
    for (index = 0; index < size; index++, bitmap1 >>= 1) {
        if (!bitmap1) {
            break;
        }
        if (!(bitmap1 & 1)) {
            continue;
        }
        err_count++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, entry_idx + index));

        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                info->error_field);

        if (info->mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info->mem;
            spci.blk_type = block;
            spci.index = entry_idx + index;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   info->mem, info->error_field);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        } else {
            *intr_enable = 0;
        }
    }

    if (err_count == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s parity hardware inconsistency\n"),
                   unit, msg));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_parity_mmu_clear(int unit, soc_field_t status_field)
{
    uint32 rval;
    SOC_IF_ERROR_RETURN
        (READ_MEM_FAIL_INT_STATr(unit, &rval));
    if (soc_reg_field_valid(unit, MEM_FAIL_INT_STATr, status_field)) {
        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &rval, status_field, 0);
    } else {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_MEM_FAIL_INT_STATr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    return SOC_E_NONE;
}

/*
 * status0 (MEM1_IPMCGRP_TBL_PARITYERRORPTRr) is index
 * status1 (MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr) is table id
 *         MMU_IPMC_GROUP_TBL0m - MMU_IPMC_GROUP_TBL8m
 */
STATIC int
_soc_triumph_parity_process_mmuipmc(int unit, char *msg,
                                        _soc_parity_info_t *info, int block,
                                        int * intr_enable)
{
    int size, index, rv = 0;
    soc_reg_t reg;
    uint32 addr, entry_idx, status;
    soc_reg_t mem_id_reg, index_reg;
    uint32 mem_bitmap, minfo;
    _soc_ser_correct_info_t spci;
    static soc_mem_t ipmc_mems[] =
    {
        MMU_IPMC_GROUP_TBL0m,
        MMU_IPMC_GROUP_TBL1m,
        MMU_IPMC_GROUP_TBL2m,
        MMU_IPMC_GROUP_TBL3m,
        MMU_IPMC_GROUP_TBL4m,
        MMU_IPMC_GROUP_TBL5m,
        MMU_IPMC_GROUP_TBL6m,
        MMU_IPMC_GROUP_TBL7m,
        MMU_IPMC_GROUP_TBL8m
    };
    static soc_field_t ipmc_mem_fields[] =
    {
        IGPERR0f,
        IGPERR1f,
        IGPERR2f,
        IGPERR3f,
        IGPERR4f,
        IGPERR5f,
        IGPERR6f,
        IGPERR7f,
        IGPERR8f
    };

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    if((SOC_IS_TRIUMPH(unit)) &&
            (TOQ0_IPMC_TBL_PAR_ERRf == info->error_field)){
        SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &status));
        if (!soc_reg_field_get(unit, MEM_FAIL_INT_STATr, status,
                    TOQ0_IPMC_TBL_PAR_ERRf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d IPMC: parity hardware inconsistency\n"),
                     unit));
            return SOC_E_NONE;
        }
        /*
         * status0 (TOQ_IPMCGRPERRPTR0r) is index
         * status1 (TOQ_ERRINTR1r) is table id
         * MMU_IPMC_GROUP_TBL0m - MMU_IPMC_GROUP_TBL8m
         */
        index_reg = info->status0_reg;
        addr = soc_reg_addr(unit, index_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &entry_idx));
        mem_id_reg = info->status1_reg;
        addr = soc_reg_addr(unit, mem_id_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &mem_bitmap));
        for (index = 0; index < COUNTOF(ipmc_mem_fields); index++) {
            if (!soc_reg_field_get(unit, mem_id_reg, mem_bitmap,
                        ipmc_mem_fields[index])) {
                continue;
            }
            minfo = SOC_TRIUMPH_MEM_PARITY_INFO(unit, block,
                    info->error_field);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                    SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                    entry_idx, minfo);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d IPMC_GROUP%d entry %d parity error\n"),
                     unit, index, entry_idx));
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = ipmc_mems[index];
            spci.blk_type = SOC_BLK_MMU;
            spci.index = entry_idx;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                        entry_idx, minfo);
            }
            /* Clear memory id register explicitly */
            soc_reg_field_set(unit, mem_id_reg, &mem_bitmap, ipmc_mem_fields[index], 0);
            addr = soc_reg_addr(unit, mem_id_reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, mem_bitmap));
            break;
        }
        SOC_IF_ERROR_RETURN(_soc_triumph_parity_mmu_clear(unit, TOQ0_IPMC_TBL_PAR_ERRf));

    } else {

        reg = info->status1_reg;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
        size = soc_reg_field_length(unit, reg, STATUSf);
        reg = info->status0_reg;
        for (index = 0; index < size; index++, status >>= 1) {
            if (!(status & 1)) {
                continue;
            }
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, index);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr,
                        &entry_idx));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d %s%d entry %d parity error\n"),
                     unit, msg, index, entry_idx));

            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                    SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
                    info->error_field);

            if (info->mem != INVALIDm) {
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = info->mem;
                spci.blk_type = block;
                spci.index = entry_idx;
                rv = soc_ser_correction(unit, &spci);
                if (SOC_FAILURE(rv)) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                            SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                            info->mem, info->error_field);
                }
                SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
            } else {
                *intr_enable = 0;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * status0 (WRED_PARITY_ERROR_INFOr) is index
 * status1 (WRED_PARITY_ERROR_BITMAPr) is table id
 */
STATIC int
_soc_triumph_parity_process_mmuwred(int unit, char *msg,
                                        _soc_parity_info_t *info, int block,
                                        int * intr_enable)
{
    soc_reg_t reg;
    uint32 addr0, entry_idx, status, addr1, mconf, mconf_addr;
    _soc_ser_correct_info_t spci;
    int rv = 0;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    reg = info->status0_reg;
    addr0 = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr0, &entry_idx));
    reg = info->status1_reg;
    addr1 = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr1, &status));
    spci.mem = info->mem;
    if (status & 0x000003) {
        msg = "WRED_CFG_CELL";
        spci.mem = MMU_WRED_CFG_CELLm; 
    } else if (status & 0x00000c) {
        msg = "WRED_THD_0_CELL";
        spci.mem = MMU_WRED_THD_0_CELLm; 
    } else if (status & 0x000030) {
        msg = "WRED_THD_1_CELL";
        spci.mem = MMU_WRED_THD_1_CELLm; 
    } else if (status & 0x0000c0) {
        msg = "WRED_CFG_PACKET";
        spci.mem = MMU_WRED_CFG_PACKETm; 
    } else if (status & 0x000300) {
        msg = "WRED_THD_0_PACKET";
        spci.mem = MMU_WRED_THD_0_PACKETm; 
    } else if (status & 0x000c00) {
        msg = "WRED_THD_1_PACKET";
        spci.mem = MMU_WRED_THD_1_PACKETm; 
    } else if (status & 0x003000) {
        msg = "WRED_PORT_CFG_CELL";
        spci.mem = MMU_WRED_PORT_CFG_CELLm; 
    } else if (status & 0x00c000) {
        msg = "WRED_PORT_THD_0_CELL";
        spci.mem = MMU_WRED_PORT_THD_0_CELLm; 
    } else if (status & 0x030000) {
        msg = "WRED_PORT_THD_1_CELL";
        spci.mem = MMU_WRED_PORT_THD_1_CELLm; 
    } else if (status & 0x0c0000) {
        msg = "WRED_PORT_CFG_PACKET";
        spci.mem = MMU_WRED_PORT_CFG_PACKETm; 
    } else if (status & 0x300000) {
        msg = "WRED_PORT_THD_0_PACKET";
        spci.mem = MMU_WRED_PORT_THD_0_PACKETm; 
    } else if (status & 0xc00000) {
        msg = "WRED_PORT_THD_1_PACKET";
        spci.mem = MMU_WRED_PORT_THD_1_PACKETm; 
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s parity hardware inconsistency\n"),
                   unit, msg));
        *intr_enable = 0;
        return SOC_E_NONE;
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s entry %d parity error\n"),
               unit, msg, entry_idx));

    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
            SOC_SWITCH_EVENT_DATA_ERROR_PARITY, info->mem,
            info->error_field);

    if (spci.mem != INVALIDm) {
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
        spci.reg = INVALIDr;
        spci.blk_type = block;
        spci.index = entry_idx;
        rv = soc_ser_correction(unit, &spci);
        if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               info->mem, info->error_field);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr0, 0));
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr1, 0));

        mconf_addr = soc_reg_addr(unit, MISCCONFIGr, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, mconf_addr, &mconf));
        soc_reg_field_set(unit, MISCCONFIGr, &mconf, PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, mconf_addr, mconf));
        soc_reg_field_set(unit, MISCCONFIGr, &mconf, PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, mconf_addr, mconf));
    } else {
        *intr_enable = 0;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_mtro_mmu_port_index_get(int instance_num, int ptr, 
                                     int *port, int *queue)
{
    int  num_port, numq, base_queue, port_index;
    
    /* Check if instance_num is valid */ 
    if ((instance_num < 0) || 
        (instance_num >= MMU_MTRO_MAX_GROUP)) {
        return SOC_E_INTERNAL;
    }
 
    num_port = _soc_tr_mtro_group_info[instance_num].num_port;
    numq = _soc_tr_mtro_group_info[instance_num].numq;
    base_queue = _soc_tr_mtro_group_info[instance_num].base_queue;
    
    /* Check if ptr is valid */
    if ((ptr < 0) || (ptr >= (num_port * numq))) {
        return SOC_E_INTERNAL;
    }
 
    port_index = ptr / numq;
    *port = _soc_tr_mtro_group_info[instance_num].port_list[port_index];
    *queue = base_queue + ptr % numq;
   
    return SOC_E_NONE;
} 

STATIC int
_soc_triumph_parity_process_mmumtro(int unit, _soc_parity_info_t *info,
                                    soc_block_t block)
{
    static soc_reg_t mtro_regs[] = {
        MINBUCKETCONFIG_64r,
        MINBUCKETr,
        MAXBUCKETCONFIG_64r,
        MAXBUCKETr
    };
    static soc_reg_t cpu_mtro_regs[] = {
        CPUMAXBUCKETCONFIG_64r,
        CPUMAXBUCKETr
    };
    uint32 i, rval, status, val, minfo;
    _soc_ser_correct_info_t spci;
    int rv, port, index, instance_num, ptr;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
    spci.mem = INVALIDm;
    spci.blk_type = SOC_BLK_MMU;
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &status));
    if (!soc_reg_field_get(unit, MEM_FAIL_INT_STATr, status, 
                           MTRO_PAR_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d MTRO: parity hardware"
                              " inconsistency\n"),
                              unit));
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, EGRSHAPEPARITYERRORPTRr,
                                      REG_PORT_ANY, 0, &rval));
    instance_num = soc_reg_field_get(unit, EGRSHAPEPARITYERRORPTRr, 
                                     rval, INSTANCE_NUMf);
    ptr = soc_reg_field_get(unit, EGRSHAPEPARITYERRORPTRr, rval, PTRf);
    SOC_IF_ERROR_RETURN(
        _soc_triumph_mtro_mmu_port_index_get(instance_num, ptr, 
                                             &port, &index)); 
    /* Disable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    if (port == REG_PORT_ANY) {
        for (i = 0; i < COUNTOF(cpu_mtro_regs); i++) {
            spci.port = port;
            spci.index = index;
            spci.reg = cpu_mtro_regs[i];
            minfo = SOC_TRIUMPH_MEM_PARITY_INFO(unit, block, info->error_field); 
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               index, minfo);
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   index, minfo);
            }
        }
    } else if (port >=1 && port <= 53) {
        for (i = 0; i < COUNTOF(mtro_regs); i++) {
            spci.port = port;
            spci.index = index;
            spci.reg = mtro_regs[i];
            minfo = SOC_TRIUMPH_MEM_PARITY_INFO(unit, block, info->error_field); 
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               index, minfo);
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   index, minfo);
            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d MTRO: parity hardware inconsistency\n"),
                              unit));
    }
    SOC_IF_ERROR_RETURN(_soc_triumph_parity_mmu_clear(unit, MTRO_PAR_ERRf));
    /* Enable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));
    return SOC_E_NONE;
}

STATIC int
_soc_triumph_process_parity_error(int unit)
{
    int block, table, intr_enable;
    uint32 addr, cmic_status, block_status, block_enable;
    uint32 enable_addr;
    _soc_parity_info_t *info;
    soc_reg_t block_reg, enable_reg;
    char *msg;
    
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CHIP_PARITY_INTR_STATUSr(unit,
                                                           &cmic_status));

    /* loop through each block */
    for (block = 0; _soc_tr_parity_block_info[block].cpi_bit; block++) {
        if (!(cmic_status & _soc_tr_parity_block_info[block].cpi_bit)) {
            continue;
        }
        info = _soc_tr_parity_block_info[block].info;
        block_reg = _soc_tr_parity_block_info[block].status_reg;
        addr = soc_reg_addr(unit, block_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &block_status));

        enable_reg = _soc_tr_parity_block_info[block].enable_reg;
        enable_addr = soc_reg_addr(unit, enable_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, enable_addr, &block_enable));

        /* loop through each table in the block */
        for (table = 0; info[table].error_field != INVALIDf; table++) {
            if (!soc_reg_field_get(unit, block_reg, block_status,
                                   info[table].error_field)) {
                continue;
            }
            if (info[table].msg) {
                msg = info[table].msg;
            } else if (info[table].mem != INVALIDm) {
                msg = SOC_MEM_NAME(unit, info[table].mem);
            } else {
                msg = SOC_FIELD_NAME(unit, info[table].error_field);
            }

            intr_enable = 1;

            /* handle different parity error reporting style */
            switch (info[table].type) {
            case _SOC_PARITY_INFO_TYPE_SINGLE_TABLE1:
                SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_single_table1(unit, msg, 
                        &info[table], block, &intr_enable));
                break;
            case _SOC_PARITY_INFO_TYPE_SINGLE_TABLE2:
                SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_single_table2(unit, msg, 
                        &info[table], block, &intr_enable));
                break;
            case _SOC_PARITY_INFO_TYPE_DUAL_TABLE1:
                SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_dual_table1(unit, msg, 
                        &info[table], block, &intr_enable));
                break;
            case _SOC_PARITY_INFO_TYPE_DUAL_TABLE2:
                SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_dual_table2(unit, msg, 
                        &info[table], block, &intr_enable));
                break;
            case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_mmuipmc(unit, msg, 
                        &info[table], block, &intr_enable));
                break;
            case _SOC_PARITY_INFO_TYPE_MMUWRED:
                SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_mmuwred(unit, msg, 
                        &info[table], block, &intr_enable));
                break;
            case _SOC_PARITY_INFO_TYPE_MMUMTRO:
                 SOC_IF_ERROR_RETURN(
                    _soc_triumph_parity_process_mmumtro(unit,
                                                        &info[table], block));
                break; 
            /* Coverity:- This is intentional and not a bug.
             * The generic case is hit only when we are not able
             * to fix an issue, there is no analysis/correction code,
             * just an err msg - same as the default.
             * This is an old device for which we do not have
             * full SER correction implemented.
             */
            /* coverity[unterminated_case] */
            case _SOC_PARITY_INFO_TYPE_GENERIC:
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                        info[table].mem,
                        info[table].error_field);
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s asserted\n"),
                                      unit, msg));
            default:
                intr_enable = 0;
                break; 
            } /* handle different parity error reporting style */
            /* Disable error reporting on this memory to avoid continuous 
               interrupts */
            soc_reg_field_set(unit, enable_reg, &block_enable,
                                 info[table].enable_field, intr_enable);
        } /* loop through each table in the block */

        /* Exclude non-parity status bits */
        block_status &= block_enable;  
        block_enable &= ~block_status;
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, enable_addr, block_enable));
        block_enable |= block_status;
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, enable_addr, block_enable));
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    } /* loop through each block */

    return SOC_E_NONE;
}

void
soc_triumph_parity_error(void *unit_vp, void *d1, void *d2,
                         void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    _soc_triumph_process_parity_error(unit);
    soc_intr_enable(unit, IRQ_MEM_FAIL);
}

typedef struct _soc_esm_intr_reg_info_s {
    soc_field_t error_field;
    char        *msg;
    soc_reg_t   detail_reg[3];
} _soc_esm_intr_reg_info_t;

STATIC _soc_esm_intr_reg_info_t _soc_tr_etu_intr_reg_info[] = {
    { ILL_SRAM_ACCf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { SBUS_REQACK_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { BKGND_PROC_SECf, NULL,
      { ETU_BKGND_PROC_SEC_INFOr, INVALIDr, INVALIDr } },
    { BKGND_PROC_ERRf, NULL,
      { ETU_BKGND_PROC_ERR_INFOr, INVALIDr, INVALIDr } },
    { SBUS_CMD_SECf, NULL, { ETU_SBUS_CMD_SEC_INFOr, INVALIDr, INVALIDr } },
    { SBUS_CMD_ERRf, NULL,
      { ETU_SBUS_CMD_ERR_INFO1r, ETU_SBUS_CMD_ERR_INFO2r, INVALIDr} },
    { ET_DPEO_PERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { ET_S1_MWS_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { ET_S0_MWS_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { ET_S1_NORV_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { ET_S0_NORV_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { ET_S1_RBUS_PERRf, NULL, { ETU_S1_RBUS_PERR_INFOr, INVALIDr, INVALIDr } },
    { ET_S0_RBUS_PERRf, NULL, { ETU_S0_RBUS_PERR_INFOr, INVALIDr, INVALIDr } },
    /* PPA_COMPLETEf */
    /* ET_INST_REQ_COMPLETEf */
    /* MRST_COMPLETEf */
    { CIF_LOST2_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_LUREQFIFO */
    { LUREQFIFOCOUNT_GTE_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_LUREQFIFO */
    { ADREQ1FIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } },  /* chk_ADREQ1FIFO */
    { ADREQ1FIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } },  /* chk_ADREQ1FIFO */
    { ADREQ0FIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } },  /* chk_ADREQ0FIFO */
    { ADREQ0FIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } },  /* chk_ADREQ0FIFO */
    { L2MODFIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { L2MODFIFO_UNDERRUNf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { L2MODFIFO_NOTEMPTYf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { EXT_TCAM_SE_O_L_PINf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { INVALIDf } /* table terminator */
};

STATIC _soc_esm_intr_reg_info_t _soc_tr_es01_intr_reg_info[] = {
    { CIF_LOST2_HITHR1f, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ADREQ1FIFO */
    { ADREQ1FIFOCOUNT_GTE_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ADREQ1FIFO */
    { CIF_LOST2_HITHR0f, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ADREQ0FIFO */
    { ADREQ0FIFOCOUNT_GTE_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ADREQ0FIFO */
    { ADL3DST_SECf, NULL, { ES01_ADL3DST_SEC_INFOr, INVALIDr, INVALIDr } },
    { ADL3DST_DEDf, NULL, { ES01_ADL3DST_DED_INFOr, INVALIDr, INVALIDr } },
    { ADL3SRC_SECf, NULL, { ES01_ADL3SRC_SEC_INFOr, INVALIDr, INVALIDr } },
    { ADL3SRC_DEDf, NULL, { ES01_ADL3SRC_DED_INFOr, INVALIDr, INVALIDr } },
    { ADL2DST_SECf, NULL, { ES01_ADL2DST_SEC_INFOr, INVALIDr, INVALIDr } },
    { ADL2DST_DEDf, NULL, { ES01_ADL2DST_DED_INFOr, INVALIDr, INVALIDr } },
    { ADL2SRC_SECf, NULL, { ES01_ADL2SRC_SEC_INFOr, INVALIDr, INVALIDr } },
    { ADL2SRC_DEDf, NULL, { ES01_ADL2SRC_DED_INFOr, INVALIDr, INVALIDr } },
    { L2L3RSPFIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_L2L3RSPFIFO */
    { L2L3RSPFIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_L2L3RSPFIFO */
    { FP0RSPFIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_FP0RSPFIFO */
    { FP0RSPFIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_FP0RSPFIFO */
    { FP1RSPFIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_FP1RSPFIFO */
    { FP1RSPFIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_FP1RSPFIFO */
    { PKT_CNT_WRAPf, NULL, { ES01_PKT_CNT_WRAP_INFOr, INVALIDr, INVALIDr } },
    { BYT_CNT_WRAPf, NULL, { ES01_BYT_CNT_WRAP_INFOr, INVALIDr, INVALIDr } },
    { ADFPCNTR_SECf, NULL, { ES01_ADFPCNTR_SEC_INFOr, INVALIDr, INVALIDr } },
    { ADFPCNTR_DEDf, NULL, { ES01_ADFPCNTR_DED_INFOr, INVALIDr, INVALIDr } },
    { INVALIDf } /* table terminator */
};

STATIC _soc_esm_intr_reg_info_t _soc_tr_iesmif_intr_reg_info[] = {
    { AGE_IPFIX_ERRf, NULL, { IPFIX_AGE_CONTROLr, INVALIDr, INVALIDr } },
    { FP_REFRESH_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { BK2BK_ESM_ELIGIBLEf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { ESU_ASF_HI_ACCUMf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_L2L3RSPFIFO, chk_FP0RSPFIFO, chk_FP1RSPFIFO */
    { LUREQFIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_LUREQFIFO */
    { ECB_HI_ACCUMf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ECBFIFO */
    { ECB_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ECBFIFO */
    { DROPPED_CELLf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { CNP_ES_COUNT_WRAPf, NULL, { IESMIF_STATUS7r, INVALIDr, INVALIDr } },
    { CNP_EXT_SEARCHf, NULL, { IESMIF_STATUS7r, INVALIDr, INVALIDr } },
    { NEW_MAX_LATENCYf, NULL, { IESMIF_STATUS3r, INVALIDr, INVALIDr } },
    { ECB_SEC_COUNT_WRAPf, NULL, { IESMIF_STATUS5r, INVALIDr, INVALIDr } },
    { ECB_SEC_ERRf, NULL,
      { IESMIF_STATUS5r, IESMIF_ECB_ECC_STATUS_SBEr,
        IESMIF_ECB_SBE_SYNDROME12r } },
    { ECB_DED_COUNT_WRAPf, NULL, { IESMIF_STATUS6r, INVALIDr, INVALIDr } },
    { ECB_DED_ERRf, NULL,
      { IESMIF_STATUS6r, IESMIF_ECB_ECC_STATUS_DBEr, INVALIDr } },
    { INVALIDf } /* table terminator */
};

STATIC _soc_esm_intr_reg_info_t _soc_tr_il3mc_erb_intr_reg_info[] = {
    { EXTFP_POLICY_SECf, NULL,
      { IL3MC_EXTFP_POLICY_SEC_INFOr, INVALIDr, INVALIDr } },
    { EXTFP_POLICY_DEDf, NULL,
      { IL3MC_EXTFP_POLICY_DED_INFOr, INVALIDr, INVALIDr } },
    { EXT_LU_ERRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { FPCREQFIFO_BK2BK_WRf, NULL, { INVALIDr, INVALIDr, INVALIDr } },
    { FPCREQFIFO_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /*chk_FPCREQFIFO */
    { FPCREQFIFO_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /*chk_FPCREQFIFO */
    { FP1RSPFIFOCOUNT_GTE_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_FP1RSPFIFO */
    { FP0RSPFIFOCOUNT_GTE_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_FP0RSPFIFO */
    { L2L3RSPFIFOCOUNT_GTE_HITHRf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_L2L3RSPFIFO */
    { IPCF_PTR_MISMATCHf, NULL,
      { IL3MC_IPCF_PTR_MISMATCH_INFOr, INVALIDr, INVALIDr } },
    { ERB_OVERFLOWf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ERBFIFO */
    { ERB_FULLf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ERBFIFO */
    { ERB_UNDERRUNf, NULL, { INVALIDr, INVALIDr, INVALIDr } }, /* chk_ERBFIFO */
    { INVALIDf } /* table terminator */
};

STATIC struct {
    soc_reg_t enable_reg;
    soc_reg_t clear_reg;
    soc_reg_t status_reg;
    _soc_esm_intr_reg_info_t *reg_info;
} _soc_tr_esm_intr_info[] = {
    { ETU_INTR_ENABLEr, ETU_INTR_CLEARr, ETU_INTR_STATUSr,
      _soc_tr_etu_intr_reg_info },
    { ES01_INTR_ENABLEr, ES01_INTR_CLEARr, ES01_INTR_STATUSr,
      _soc_tr_es01_intr_reg_info },
    { IESMIF_INTR_ENABLEr, IESMIF_INTR_CLEARr, IESMIF_INTR_STATUSr,
      _soc_tr_iesmif_intr_reg_info },
    { IL3MC_ERB_INTR_ENABLEr, IL3MC_ERB_INTR_CLEARr, IL3MC_ERB_INTR_STATUSr,
      _soc_tr_il3mc_erb_intr_reg_info },
    { INVALIDr, INVALIDr, INVALIDr } /* table terminator */
};

STATIC int
_soc_triumph_esm_intr_enable(int unit, int enable)
{
    int reg_index;
    uint32 addr, rval;
    soc_reg_t reg;
    _soc_esm_intr_reg_info_t *intr_reg_info;

    if (SOC_CONTROL(unit)->tcam_info == NULL) {
        return SOC_E_NONE;
    }

    for (reg_index = 0;
         _soc_tr_esm_intr_info[reg_index].status_reg != INVALIDf;
         reg_index++) {
        reg = _soc_tr_esm_intr_info[reg_index].status_reg;
        rval = 0;

        for (intr_reg_info = _soc_tr_esm_intr_info[reg_index].reg_info;
             intr_reg_info->error_field != INVALIDf; intr_reg_info++) {
            soc_reg_field_set(unit, reg, &rval, intr_reg_info->error_field,
                              enable ? 1 : 0);
        }
        reg = _soc_tr_esm_intr_info[reg_index].enable_reg;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    }

    soc_intr_enable(unit, IRQ_CHIP_FUNC_3);

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_process_intr_status(int unit)
{
    int reg_index, field_index, i;
    uint32 addr, status, rval;
    soc_reg_t reg, detail_reg;
    _soc_esm_intr_reg_info_t *intr_reg_info;
    soc_reg_info_t *reg_info;
    soc_field_info_t *field_info;
    char *msg;
    char line_buf[80], field_buf[80];
    int	line_len, field_len;

    for (reg_index = 0; _soc_tr_esm_intr_info[reg_index].clear_reg != INVALIDf;
         reg_index++) {
        reg = _soc_tr_esm_intr_info[reg_index].status_reg;

        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
        for (intr_reg_info = _soc_tr_esm_intr_info[reg_index].reg_info;
             intr_reg_info->error_field != INVALIDf; intr_reg_info++) {
            if (!soc_reg_field_get(unit, reg, status,
                                   intr_reg_info->error_field)) {
                continue;
            }
            msg = intr_reg_info->msg;
            if (msg == NULL) {
                msg = SOC_FIELD_NAME(unit, intr_reg_info->error_field);
            }
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s asserted\n"), unit, msg));

            for (i = 0; intr_reg_info->detail_reg[i] != INVALIDr; i++) {
                detail_reg = intr_reg_info->detail_reg[i];
                addr = soc_reg_addr(unit, detail_reg, REG_PORT_ANY, 0);
                SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
                sal_sprintf(line_buf, "%s=0x%08x: ",
                            SOC_REG_NAME(unit, detail_reg), rval);
                line_len = sal_strlen(line_buf);
                reg_info = &SOC_REG_INFO(unit, detail_reg);
                for (field_index = reg_info->nFields - 1; field_index >= 0;
                     field_index--) {
                    field_info = &reg_info->fields[field_index];
                    sal_sprintf(field_buf, "%s%s=0x%x",
                                field_index == reg_info->nFields - 1 ?
                                "<" : " ",
                                SOC_FIELD_NAME(unit, field_info->field),
                                soc_reg_field_get(unit, detail_reg, rval,
                                                  field_info->field));
                    field_len = sal_strlen(field_buf);
                    if (line_len + field_len >= 64) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "    %s\n"), line_buf));
                        line_len = 0;
                    }
                    sal_sprintf(&line_buf[line_len], "%s", field_buf);
                    line_len += field_len;
                }
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "    %s>\n"), line_buf));
            }
        }
        reg = _soc_tr_esm_intr_info[reg_index].clear_reg;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0xffffffff));
    }

    return SOC_E_NONE;
}

void
soc_triumph_esm_intr_status(void *unit_vp, void *d1, void *d2,
                            void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    _soc_triumph_esm_process_intr_status(unit);
    soc_intr_enable(unit, IRQ_CHIP_FUNC_3);
}

STATIC int
soc_triumph_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec;
    soc_timeout_t       to;

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table, L2_ENTRYm */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x8000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table, EGR_L3_NEXT_HOPm */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    if (!SAL_BOOT_QUICKTURN) {
        /* ING_PW_TERM_COUNTERS is not handled by reset control */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, ING_PW_TERM_COUNTERSm, COPYNO_ALL, TRUE));
        /* Egress IPFIX tables are not properly handled by reset control */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, EGR_IPFIX_SESSION_TABLEm, COPYNO_ALL, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, EGR_IPFIX_EXPORT_FIFOm, COPYNO_ALL, TRUE));
    }

    return SOC_E_NONE;
}

/* SER processing for TCAMs */
STATIC _soc_ser_parity_info_t _soc_triumph_ser_parity_info[] = {
    { EFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        430, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { EGR_ERSPANm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        339, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
/*    { FP_SLICE_MAPm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        510, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},*/
    { FP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        430, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { ING_IPFIX_PROFILEm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        14, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { MPLS_STATION_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        121, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { VFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        430, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { L3_TUNNELm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        223, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { VLAN_SUBNETm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        162, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { L3_DEFIPm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        235, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { L3_DEFIP_128m, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1, -1,
        308, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},                 
    { INVALIDm, _SOC_SER_PARITY_MODE_NUM},
};


int
soc_triumph_misc_init(int unit)
{
    uint16              dev_id;
    uint8               rev_id;
    uint32              rval;
    uint64              reg64;
    int                 first_valid_xgport[4], gxport_12g;
    int                 port, blk, i;
    uint32              mode[4] = {0};
    soc_pbmp_t          pbmp_gport_block;
    soc_field_t         fields[3];
    uint32              values[3];
    int                 cache_fpmem;
    soc_mem_t           fp_tcams[3] = {FP_TCAMm, EFP_TCAMm, VFP_TCAMm};
    uint8               fp_tcam_len = 3;
    uint8               fp_tcam_iter = 0;

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /*
     * XGPORT0: 2-13
     * XGPORT1: 14-25
     * XGPORT2: 26, 32-42
     * XGPORT3: 27, 43-53
     */
    for (i = 0; i < 4; i++) {
        first_valid_xgport[i] = -1;
    }
    PBMP_XG_ITER(unit, port) {
        blk = SOC_PORT_BLOCK(unit, port);
        if (first_valid_xgport[SOC_BLOCK_INFO(unit, blk).number] == -1) {
            first_valid_xgport[SOC_BLOCK_INFO(unit, blk).number] = port;
        }
    }

    gxport_12g = FALSE;
    switch (dev_id) {
    case BCM56684_DEVICE_ID:
    case BCM56680_DEVICE_ID:
        mode[0] = mode[1] = mode[2] = mode[3] = 3; /* 2.5G-mode */
        gxport_12g = TRUE;

        SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP0_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP1_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP2_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, HG_16GBPS_BMPf, 0x0f);
        SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));
        break;

    case BCM56686_DEVICE_ID:
        mode[0] = mode[3] = 2; /* xport-mode */

        SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP0_MODEf, 2);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 2);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, HG_16GBPS_BMPf, 0xf9);
        SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));

        /* use 7 requestors mode instead of the default 8 requestors mode */
        SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

        /*
         * move the XPORT3 (XGOIRT3) to within 7 slots
         * (i.e. swap w/ unused block XGPORT1 or XGPORT2)
         */
        SOC_IF_ERROR_RETURN(READ_IARB_TDM_MAPr(unit, &rval));
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT2f, 0x7);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT3f, 0x5);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_MAPr(unit, rval));

        soc_port_cmap_set(unit, 2, SOC_CTR_TYPE_XE);
        soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
        break;

    case BCM56620_DEVICE_ID:
        mode[0] = mode[1] = mode[2] = mode[3] = 3; /* 2.5G-mode */
        SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP0_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP1_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP2_MODEf, 3);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 3);
        SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));
        break;

    case BCM56624_DEVICE_ID:
        mode[0] = mode[1] = mode[2] = mode[3] = 1; /* gport-mode */
        break;

    case BCM56626_DEVICE_ID:
        mode[0] = mode[1] = 1; /* gport-mode */
        for (i = 2; i < 4; i++) {
            port = first_valid_xgport[i];
            if (SOC_INFO(unit).port_speed_max[port] >= 10000) {
                mode[i] = 2; /* xport-mode */
            } else {
                mode[i] = 1; /* gport-mode */
            }
        }

        SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP2_MODEf, 2);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 2);
        SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));

        soc_port_cmap_set(unit, 26, SOC_CTR_TYPE_XE);
        soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
        break;

    case BCM56628_DEVICE_ID:
        for (i = 0; i < 4; i++) {
            port = first_valid_xgport[i];
            if (SOC_INFO(unit).port_speed_max[port] >= 10000) {
                mode[i] = 2; /* xport-mode */
            } else {
                mode[i] = 1; /* gport-mode */
            }
        }

        SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP0_MODEf, 2);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP1_MODEf, 2);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP2_MODEf, 2);
        soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 2);
        SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));

        soc_port_cmap_set(unit, 2, SOC_CTR_TYPE_XE);
        soc_port_cmap_set(unit, 14, SOC_CTR_TYPE_XE);
        soc_port_cmap_set(unit, 26, SOC_CTR_TYPE_XE);
        soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
        break;

    case BCM56629_DEVICE_ID:
        if (soc_feature(unit, soc_feature_xgport_one_xe_six_ge)) {
            gxport_12g = TRUE;
            if (soc_property_get(unit, spn_BCM56629_40GE, 0)) {
                mode[0] = mode[1] = mode[2] = mode[3] = 1; /* gport-mode */
                break;
            } else {
                mode[0] = mode[1] = mode[2] = mode[3] = 4; /* spank-mode */

                SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
                soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP0_MODEf, 1);
                soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP1_MODEf, 1);
                soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP2_MODEf, 1);
                soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 1);
                soc_reg_field_set(unit, ESTDMCONFIGr, &rval, HG_16GBPS_BMPf,
                                  0x0f);
                SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));

                soc_port_cmap_set(unit, 2,  SOC_CTR_TYPE_XE);
                soc_port_cmap_set(unit, 14, SOC_CTR_TYPE_XE);
                soc_port_cmap_set(unit, 26, SOC_CTR_TYPE_XE);
                soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
            }
        } else {
            mode[0] = mode[1] = 1; /* gport-mode */
            mode[2] = mode[3] = 2; /* xport-mode */

            SOC_IF_ERROR_RETURN(READ_ESTDMCONFIGr(unit, &rval));
            soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP2_MODEf, 2);
            soc_reg_field_set(unit, ESTDMCONFIGr, &rval, GROUP3_MODEf, 2);
            SOC_IF_ERROR_RETURN(WRITE_ESTDMCONFIGr(unit, rval));

            soc_port_cmap_set(unit, 26, SOC_CTR_TYPE_XE);
            soc_port_cmap_set(unit, 27, SOC_CTR_TYPE_XE);
        }
        break;

    default:
        break;
    }

    SOC_PBMP_CLEAR(pbmp_gport_block);

    /* SPORT */
    /* coverity[result_independent_of_operands] */
    if (SOC_PORT_VALID(unit, 1)) {
        SOC_IF_ERROR_RETURN(READ_SPORT_CTL_REGr(unit, 1, &rval));
        soc_reg_field_set(unit, SPORT_CTL_REGr, &rval, SPORT_EN_BITf, 1);
        SOC_IF_ERROR_RETURN(WRITE_SPORT_CTL_REGr(unit, 1, rval));
        SOC_PBMP_PORT_ADD(pbmp_gport_block, 1);
    }

    /* For FP_TCAMs, we don't have the logic necessary to
     * set the software cache until now.  This is the time to check for
     * the cache permission of this table.
     */
    cache_fpmem = soc_feature(unit, soc_feature_mem_cache) &&
            soc_property_get(unit, spn_MEM_CACHE_ENABLE,
                             (SAL_BOOT_RTLSIM || SAL_BOOT_XGSSIM) ? 0 : 1);

    for (fp_tcam_iter=0; fp_tcam_iter < fp_tcam_len; fp_tcam_iter++) {

         if (soc_property_get(unit, spn_MEM_CHECK_NOCACHE_OVERRIDE, 0)) {
             char mem_name[100];
             char *mptr;

             sal_memset(mem_name, 0, sizeof(mem_name));
             sal_strlcpy(mem_name, "mem_nocache_", sizeof(mem_name));
             mptr = &mem_name[sal_strlen(mem_name)];
             sal_strlcpy(mptr, SOC_MEM_NAME(unit, fp_tcams[fp_tcam_iter]),
                         sizeof(mem_name) - sal_strlen(mem_name) - 1);
             if (soc_property_get(unit, mem_name, 0)) {
                 cache_fpmem = 0;
             }
         }

         if (cache_fpmem) {
             SOC_MEM_INFO(unit, fp_tcams[fp_tcam_iter]).flags |=
                 SOC_MEM_FLAG_CACHABLE;
             SOC_IF_ERROR_RETURN
                 (soc_mem_cache_set(unit, fp_tcams[fp_tcam_iter],
                                    MEM_BLOCK_ALL, TRUE));
         }

         if (!SOC_WARM_BOOT(unit)) {
             /* Must clear FP_TCAM after port to pipe mappings
              * are initialized. */
             SOC_IF_ERROR_RETURN
                 (soc_mem_clear(unit, fp_tcams[fp_tcam_iter],
                                 COPYNO_ALL, TRUE));
         }
    }

    /* set the mode for XGPORT blocks */
    for (i = 0; i < 4; i++) {
        port = first_valid_xgport[i];
        if (port == -1) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_XGPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XGPORT_MODE_REGr, &rval, XGPORT_MODE_BITSf,
                          mode[i]);
        SOC_IF_ERROR_RETURN(WRITE_XGPORT_MODE_REGr(unit, port, rval));
        SOC_PBMP_PORT_ADD(pbmp_gport_block, port);
    }

    /* set the mode for GXPORT blocks */
    PBMP_GX_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, XPORT_MODE_BITSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
        SOC_PBMP_PORT_ADD(pbmp_gport_block, port);
    }

    /*
     * program IARB TDM request slot
     * IARB XPORT0 to XPORT3 represent XGPORT0 to XGPORT3
     * IARB XPORT4 to XPORT7 represent GXPORT0 to GXPORT3
     * each port block in slot 0, 2, 4, 6 has its own 16G TDM bandwidth
     * port blocks in slot 1, 3, 5, 7 share 3 16G TDM bandwidth in round robin
     * manner to behave like 4 12G slots
     */
    if (gxport_12g) {
        SOC_IF_ERROR_RETURN(READ_IARB_TDM_MAPr(unit, &rval));
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT0f, 0x0);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT1f, 0x2);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT2f, 0x4);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT3f, 0x6);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT4f, 0x1);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT5f, 0x3);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT6f, 0x5);
        soc_reg_field_set(unit, IARB_TDM_MAPr, &rval, XPORT7f, 0x7);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_MAPr(unit, rval));
    }

    if (soc_feature(unit, soc_feature_esm_support)) {
        SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
        /* description for ESM_ELIGIBLE_MODE in regsfile is wrong,
         * it should be 1 for odd slots and 2 for even slots */
        if (soc_property_get(unit, spn_EXT_LOOKUP_ON_XPORT, 0)) {
            /* Enable ESM lookup on back-panel ports (XPORT blocks) */
            soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval,
                              ESM_ELIGIBLE_MODEf, 0x2); /* use even slot */
        } else {
            /* Enable ESM lookup on front-panel ports (XGPORT blocks) */
            soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval,
                              ESM_ELIGIBLE_MODEf, 0x1); /* use odd slot */
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));
    }

    /*
     * Remap LED if XGPORT block is configured as single 10G port
     * use the last port of the block instead of the first port of the block
     * (swap the first and the last port number of the block in remap table)
     * Default REMAP_PORT_x to port mapping (hardware reset value):
     * REMAP_PORT_0 .. REMAP_PORT_3: GXPORT3 .. GXPORT0
     * REMAP_PORT_4: SPORT
     * REMAP_PORT_5 .. REMAP_PORT_16: 12th .. 1st port of XGPORT3
     * REMAP_PORT_17 .. REMAP_PORT_28: 12th .. 1st port of XGPORT2
     * REMAP_PORT_29 .. REMAP_PORT_40: 12th .. 1st port of XGPORT1
     * REMAP_PORT_41 .. REMAP_PORT_52: 12th .. 1st port of XGPORT0
     */
    if (mode[0] == 2) {
        /* use the last port of the block (swap the first and the last port) */
        READ_CMIC_LED_PORT_ORDER_REMAP_50_54r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_50_54r, &rval,
                          REMAP_PORT_52f, 13);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_50_54r(unit, rval);
        READ_CMIC_LED_PORT_ORDER_REMAP_40_44r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_40_44r, &rval,
                          REMAP_PORT_41f, 2);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_40_44r(unit, rval);
    }
    if (mode[1] == 2) {
        /* use the last port of the block (swap the first and the last port) */
        READ_CMIC_LED_PORT_ORDER_REMAP_40_44r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_40_44r, &rval,
                          REMAP_PORT_40f, 25);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_40_44r(unit, rval);
        READ_CMIC_LED_PORT_ORDER_REMAP_25_29r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval,
                          REMAP_PORT_29f, 14);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_25_29r(unit, rval);
    }
    if (mode[2] == 2) {
        /* use the last port of the block (swap the first and the last port) */
        READ_CMIC_LED_PORT_ORDER_REMAP_25_29r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval,
                          REMAP_PORT_28f, 42);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_25_29r(unit, rval);
        READ_CMIC_LED_PORT_ORDER_REMAP_15_19r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_15_19r, &rval,
                          REMAP_PORT_17f, 26);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_15_19r(unit, rval);
    }
    if (mode[3] == 2) {
        READ_CMIC_LED_PORT_ORDER_REMAP_15_19r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_15_19r, &rval,
                          REMAP_PORT_16f, 53);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_15_19r(unit, rval);
        READ_CMIC_LED_PORT_ORDER_REMAP_5_9r(unit, &rval);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_5_9r, &rval,
                          REMAP_PORT_5f, 27);
        WRITE_CMIC_LED_PORT_ORDER_REMAP_5_9r(unit, rval);
    }

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_triumph_pipe_mem_clear(unit));
    }

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));

    /* Turn on ingress/egress/mmu/esm parity */
    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        _soc_triumph_parity_enable(unit, TRUE);
        if (soc_feature(unit, soc_feature_esm_support)) {
            SOC_IF_ERROR_RETURN(_soc_triumph_esm_intr_enable(unit, TRUE));
        }
#ifdef INCLUDE_MEM_SCAN
        if (soc_feature(unit, soc_feature_ser_parity)) {
            soc_mem_scan_ser_list_register(unit, FALSE,
                                           _soc_triumph_ser_parity_info);
        }
#endif /* INCLUDE_MEM_SCAN */

        memset(&_tr_ser_functions, 0, sizeof(soc_ser_functions_t));
        _tr_ser_functions._soc_ser_parity_error_intr_f =
            &soc_triumph_parity_error;
        soc_ser_function_register(unit, &_tr_ser_functions);
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /* Enable dual hash on L2, L3 and MPLS_ENTRY tables */
    fields[0] = ENABLEf;
    values[0] = 1;
    fields[1] = HASH_SELECTf;
    values[1] = FB_HASH_CRC32_LOWER;
    fields[2] = INSERT_LEAST_FULL_HALFf;
    values[2] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MPLS_ENTRY_HASH_CONTROLr, REG_PORT_ANY,
                                INSERT_LEAST_FULL_HALFf, 1));

    /*
     * Egress Enable
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_64r, &reg64, PORT_BITMAP_LOf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_64r, &reg64, PORT_BITMAP_HIf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 1));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_64r(unit, reg64));

    /* Clear GPORT blocks port stats */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    PBMP_ITER(pbmp_gport_block, port) {
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_ITER(pbmp_gport_block, port) {
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }

    /* Drop the packet when traffic exceeds available IP bandwidth */
    rval = 0;
    soc_reg_field_set(unit, PKT_DROP_ENABLEr, &rval, PKT_DROP_ENABLEf, 1);
    PBMP_ITER(pbmp_gport_block, port) {
        SOC_IF_ERROR_RETURN(WRITE_PKT_DROP_ENABLEr(unit, port, rval));
    }

    /* XMAC init should be moved to mac */
    if (SOC_PBMP_NOT_NULL(PBMP_XE_ALL(unit)) ||
        SOC_PBMP_NOT_NULL(PBMP_HG_ALL(unit))) {
        rval = 0;
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf, 1);
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        PBMP_HG_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
        }
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
        PBMP_XE_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
        }
    }

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    /* Backwards compatible mirroring by default */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, 
                      DRACO_1_5_MIRRORING_MODE_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          DRACO1_5_MIRRORf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIGr, &rval, DRACO1_5_MIRRORf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));


    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /*
     * Set reference clock (based on 200MHz core clock)
     * to be 200MHz * (1/40) = 5MHz
     */
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, 40);
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

    /* Match the Internal MDC freq with above for External MDC */
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, 40);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    /*
     * Set reference clock (based on 200MHz core clock)
     * to be 200MHz * (1/8) = 25MHz
     */
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVISORf,  8);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVIDENDf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_I2Cr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval, DIVISORf,  8);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval, DIVIDENDf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_STDMAr(unit, rval));

    rval = 0x01; /* 125KHz I2C sampling rate based on 5Mhz reference clock */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_I2C_STATr(unit, rval));

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));

    /* Enable vrf based l3 lookup by default. */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, VRF_MASKr, REG_PORT_ANY, MASKf, 0));

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    fields[1] = LAG_RES_ENf;
    values[0] = values[1] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                 REG_PORT_ANY, 2, fields, values));

    /* Use same value as CAMBIST screen test */
    rval = 0;
    soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r, &rval,
                      ALL_TCAMS_TM_7_0f, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_TM_7_THRU_0r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval, CAM0_TMf, 0x10);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval, CAM1_TMf, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL0r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL1r, &rval, CAM2_TMf, 0x10);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL1r, &rval, CAM3_TMf, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL1r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL2r, &rval, CAM4_TMf, 0x10);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL2r, &rval, CAM5_TMf, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL2r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL3r, &rval, CAM6_TMf, 0x10);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL3r, &rval, CAM7_TMf, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL3r(unit, rval));

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
    return SOC_E_NONE;
}

int
soc_triumph_esm_init_read_config(int unit)
{
    soc_control_t *soc;
    soc_tcam_info_t *tcam_info;
    int table_size[TCAM_PARTITION_COUNT];
    int tcam_freq, sram_freq, i, count;

    soc = SOC_CONTROL(unit);

    tcam_freq = soc_property_get(unit, spn_EXT_TCAM_FREQ, 500);
    sram_freq = soc_property_get(unit, spn_EXT_SRAM_FREQ, 334);
    if (!tcam_freq || !sram_freq) {
        return SOC_E_NONE;
    }

    sal_memset(table_size, 0, sizeof(table_size));

    table_size[TCAM_PARTITION_FWD_L2] =
        soc_property_get(unit, spn_EXT_L2_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP4] =
        soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP6U] =
        soc_property_get(unit, spn_EXT_IP6U_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_FWD_IP6] =
        soc_property_get(unit, spn_EXT_IP6_FWD_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2] =
        soc_property_get(unit, spn_EXT_L2_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP4] =
        soc_property_get(unit, spn_EXT_IP4_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP6S] =
        soc_property_get(unit, spn_EXT_IP6S_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP6F] =
        soc_property_get(unit, spn_EXT_IP6F_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2C] =
        soc_property_get(unit, spn_EXT_L2C_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP4C] =
        soc_property_get(unit, spn_EXT_IP4C_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_IP6C] =
        soc_property_get(unit, spn_EXT_IP6C_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2IP4] =
        soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_SIZE, 0);
    table_size[TCAM_PARTITION_ACL_L2IP6] =
        soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_SIZE, 0);

    count = 0;
    for (i = TCAM_PARTITION_RAW + 1; i < TCAM_PARTITION_COUNT; i++) {
        count += table_size[i];
    }

    /* No table is configured, done! */
    if (!count) {
        return SOC_E_NONE;
    }

    if (soc->tcam_info == NULL) {
        soc->tcam_info = sal_alloc(sizeof(soc_tcam_info_t), "tcam info");
        if (soc->tcam_info == NULL) {
            return SOC_E_MEMORY;
        }
        /* Clear ESM information */
        sal_memset(soc->tcam_info, 0, sizeof(soc_tcam_info_t));
    }

    tcam_info = soc->tcam_info;

    for (i = TCAM_PARTITION_RAW + 1; i < TCAM_PARTITION_COUNT; i++) {
        tcam_info->partitions[i].num_entries = table_size[i];
    }
    tcam_info->tcam_freq = tcam_freq;
    tcam_info->sram_freq = sram_freq;

    return SOC_E_NONE;
}

int
soc_triumph_esm_init_set_tcam_freq(int unit, int freq)
{
    uint32 rval, vco_rng, mdiv, ndiv_int;

    /* freq = (NDIV_INT / MDIV) * 25 */
    if (freq <= 28) { mdiv = 64;
    } else if (freq <= 56) { mdiv = 32;
    } else if (freq <= 113) { mdiv = 16;
    } else if (freq <= 225) { mdiv = 8;
    } else if (freq <= 425) { mdiv = 4;
    } else { mdiv = 2; }

    if (freq * mdiv < 1600 ) { vco_rng = 0; /* 800 - 1600MHz */
    } else { vco_rng = 1; } /* 1600 - 2400MHz */

    ndiv_int = freq * mdiv / 25;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r(unit,
                                                                    &rval));
    soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r, &rval,
                      VCO_RNGf, vco_rng);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_1r(unit,
                                                                     rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit,
                                                                    &rval));
    soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r, &rval,
                      M1DIVf, mdiv);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_2r(unit,
                                                                     rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_3r(unit,
                                                                    &rval));
    soc_reg_field_set(unit, CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_3r, &rval,
                      NDIV_INTf, ndiv_int);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CORE_PLL2_CTRL_STATUS_REGISTER_3r(unit,
                                                                     rval));

    return SOC_E_NONE;
}

int
soc_triumph_esm_init_set_sram_freq(int unit, int freq)
{
    uint32 rval, vco_rng, mdiv, ndiv_int;

    if (soc_feature(unit, soc_feature_use_double_freq_for_ddr_pll)) {
        freq *= 2;
    }

    /* freq = (NDIV_INT / MDIV) * 25 */
    if (freq <= 28) { mdiv = 64;
    } else if (freq <= 56) { mdiv = 32;
    } else if (freq <= 113) { mdiv = 16;
    } else if (freq <= 225) { mdiv = 8;
    } else if (freq <= 425) { mdiv = 4;
    } else { mdiv = 2; }

    if (freq * mdiv < 1600 ) { vco_rng = 0; /* 800 - 1600MHz */
    } else { vco_rng = 1; } /* 1600 - 2400MHz */

    ndiv_int = freq * mdiv / 25;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r(unit,
                                                                    &rval));
    soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r, &rval,
                      VCO_RNGf, vco_rng);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_1r(unit,
                                                                     rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit,
                                                                    &rval));
    soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r, &rval,
                      M1DIVf, mdiv);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_2r(unit,
                                                                     rval));
    
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_3r(unit,
                                                                    &rval));
    soc_reg_field_set(unit, CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_3r, &rval,
                      NDIV_INTf, ndiv_int);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CORE_PLL1_CTRL_STATUS_REGISTER_3r(unit,
                                                                     rval));

    return SOC_E_NONE;
}

void
soc_triumph_esm_init_mem_config(int unit)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;

    sop = SOC_PERSIST(unit);
    memState = sop->memState;

    /* L2 forwarding partition */
    memState[EXT_L2_ENTRYm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_L2_ENTRY_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_L2_ENTRY_DATAm].index_max = -1; /* SRAM */
    memState[EXT_SRC_HIT_BITS_L2m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_L2m].index_max = -1; /* DST hit bit */

    /* IP4 forwarding partition */
    memState[EXT_IPV4_DEFIPm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV4_DEFIP_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_DEFIP_DATA_IPV4m].index_max = -1; /* SRAM */
    memState[EXT_SRC_HIT_BITS_IPV4m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV4m].index_max = -1; /* DST hit bit */

    /* IP6 64-bit prefix forwarding partition */
    memState[EXT_IPV6_64_DEFIPm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_64_DEFIP_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_DEFIP_DATA_IPV6_64m].index_max = -1; /* SRAM */
    memState[EXT_SRC_HIT_BITS_IPV6_64m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV6_64m].index_max = -1; /* DST hit bit */

    /* IP6 128-bit prefix forwarding partition */
    memState[EXT_IPV6_128_DEFIPm].index_max = -1; /* TCAM + SRAM */
    memState[EXT_IPV6_128_DEFIP_TCAMm].index_max = -1; /* TCAM */
    memState[EXT_DEFIP_DATA_IPV6_128m].index_max = -1; /* SRAM */
    memState[EXT_SRC_HIT_BITS_IPV6_128m].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITS_IPV6_128m].index_max = -1; /* DST hit bit */

    /* L2 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_L2m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL288_L2m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL288_L2m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL288_L2m].index_max = -1; /* SRAM counter */

    /* IP4 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_IPV4m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL288_IPV4m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL288_IPV4m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL288_IPV4m].index_max = -1; /* SRAM counter */

    /* IP6 short form 360-bit ACL partition */
    memState[EXT_ACL360_TCAM_DATA_IPV6_SHORTm].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL360_IPV6_SHORTm].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL360_IPV6_SHORTm].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL360_IPV6_SHORTm].index_max = -1; /* SRAM counter */

    /* IP6 full form 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_IPV6_LONGm].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL432_IPV6_LONGm].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL432_IPV6_LONGm].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL432_IPV6_LONGm].index_max = -1; /* SRAM counter */

    /* L2 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_L2m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL144_L2m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL144_L2m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL144_L2m].index_max = -1; /* SRAM counter */

    /* IP4 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV4m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL144_IPV4m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL144_IPV4m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL144_IPV4m].index_max = -1; /* SRAM counter */

    /* IP6 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV6m].index_max = -1; /* TCAM */
    memState[EXT_FP_POLICY_ACL144_IPV6m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL144_IPV6m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL144_IPV6m].index_max = -1; /* SRAM counter */

    /* L2+IP4 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV4m].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL432_L2_IPV4m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL432_L2_IPV4m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL432_L2_IPV4m].index_max = -1; /* SRAM counter */

    /* L2+IP6 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV6m].index_max = -1; /* TCAM data */
    memState[EXT_FP_POLICY_ACL432_L2_IPV6m].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTR_ACL432_L2_IPV6m].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8_ACL432_L2_IPV6m].index_max = -1; /* SRAM counter */

    /* All L2+L3 forwarding entries */
    memState[EXT_SRC_HIT_BITSm].index_max = -1; /* SRC hit bit */
    memState[EXT_DST_HIT_BITSm].index_max = -1; /* DST hit bit */

    /* All L3 forwarding entries */
    memState[EXT_DEFIP_DATAm].index_max = -1; /* SRAM */

    /* All 144-bit ACL entries */
    memState[EXT_ACL144_TCAMm].index_max = -1; /* TCAM */

    /* All 288-bit ACL entries */
    memState[EXT_ACL288_TCAMm].index_max = -1; /* TCAM */

    /* All 360-bit ACL entries */
    memState[EXT_ACL360_TCAM_DATAm].index_max = -1; /* TCAM data */

    /* All 432-bit ACL entries */
    memState[EXT_ACL432_TCAM_DATAm].index_max = -1; /* TCAM data */

    /* All ACL entries */
    memState[EXT_FP_POLICYm].index_max = -1; /* SRAM */
    memState[EXT_FP_CNTRm].index_max = -1; /* SRAM counter */
    memState[EXT_FP_CNTR8m].index_max = -1; /* SRAM counter */
}

/*
 * AD_MODE:
 * 1: 250 MHz, l2 only, require sram0 installed
 * 2: 250 MHz, l2 only, require sram1 installed
 * 3: 250 MHz, l3 only, require sram0 installed
 * 4: 250 MHz, l3 only, require sram1 installed
 * 5: 250 MHz, no acl, require sram0 installed
 * 6: 250 MHz, no acl, require sram1 installed
 * 7: 334 MHz, acl only, require sram0 installed
 * 8: 334 MHz, acl only, require sram1 installed
 * 9: 250 MHz, acl only, require both sram installed
 * 10: 250 MHz, no l3, require both sram installed
 * 11: 250 MHz, no l2, require both sram installed
 * 12: 334 MHz, any table, require both sram installed
 */
STATIC int
_soc_triumph_esm_init_select_ad_mode(int unit, int *ad_mode)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    uint32 l2_flags, l3_flags, acl_flags;
    int l2_entries, l3_entries, acl_entries;
    int cfg_ad_mode, cfg_sram_speed, cfg_sram0_present, cfg_sram1_present;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    partitions = tcam_info->partitions;
    l2_entries = partitions[TCAM_PARTITION_FWD_L2].num_entries;
    l3_entries = partitions[TCAM_PARTITION_FWD_IP4].num_entries +
                 partitions[TCAM_PARTITION_FWD_IP6U].num_entries +
                 partitions[TCAM_PARTITION_FWD_IP6].num_entries;
    acl_entries = partitions[TCAM_PARTITION_ACL_L2].num_entries +
                  partitions[TCAM_PARTITION_ACL_IP4].num_entries +
                  partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
                  partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
                  partitions[TCAM_PARTITION_ACL_L2C].num_entries +
                  partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
                  partitions[TCAM_PARTITION_ACL_IP6C].num_entries +
                  partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
                  partitions[TCAM_PARTITION_ACL_L2IP6].num_entries;

    l2_flags = TCAM_PARTITION_FLAGS_TYPE_L2;
    l3_flags = TCAM_PARTITION_FLAGS_TYPE_L3;
    acl_flags = TCAM_PARTITION_FLAGS_TYPE_ACL;

    cfg_ad_mode = soc_property_get(unit, spn_EXT_AD_MODE, 0);
    if (cfg_ad_mode < 0 || cfg_ad_mode > 12) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "ESM init: unit %d bad %s %d\n"),
                              unit, spn_EXT_AD_MODE, cfg_ad_mode));
        return SOC_E_PARAM;
    }

    /* it AD_MODE is not specified in config, figure it out from SRAM config */
    if (!cfg_ad_mode) {
        /* 0: 250 MHz, 1: 334 Mhz, default to 334 MHz */
        cfg_sram_speed = soc_property_get(unit, spn_EXT_SRAM_SPEED, 1);

        /* default set to both ES0 and ES1 installed */
        cfg_sram0_present = soc_property_get(unit, spn_EXT_SRAM0_PRESENT, 1);
        cfg_sram1_present = soc_property_get(unit, spn_EXT_SRAM1_PRESENT, 1);
        if (!cfg_sram0_present && !cfg_sram1_present) { /* no SRAM at all */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "ESM init: unit %d both ES0 and ES1 are configured "
                                  "as not present\n"),
                       unit));
            return SOC_E_PARAM;
        }

        if (cfg_sram_speed) { /* 334 Mhz DDR2+ */
            if (!l2_entries && !l3_entries) { /* ACL only */
                /* mode 7 ES0:ACL, mode 8 ES1:ACL */
                *ad_mode = cfg_sram0_present ? 7 : 8;
            } else {  /* any L2/L3/ACL combination */
                if (cfg_sram0_present && cfg_sram1_present) {
                    *ad_mode = 12; /* ES0:ACL, ES1:L2/L3 */
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "ESM init: unit %d require both ES0 and ES1 "
                                          "in configuration\n"),
                               unit));
                    return SOC_E_PARAM;
                }
            }
        } else { /* 250 Mhz DDR2 */
            if (!acl_entries) {
                if (!l3_entries) { /* L2 only */
                    /* mode 1 ES0:L2, mode 2 ES1:L2 */
                    *ad_mode = cfg_sram0_present ? 1 : 2;
                } else if (!l2_entries) { /* L3 only */
                    /* mode 3 ES0:L3, mode 4 ES1:L3 */
                    *ad_mode = cfg_sram0_present ? 3 : 4;
                } else { /* L2+L3 only */
                    /* mode 5 ES0:L2+L3, mode 6 ES1:L2+L3 */
                    *ad_mode = cfg_sram0_present ? 5 : 6;
                }
            } else {
                if (!cfg_sram0_present || !cfg_sram1_present) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "ESM init: unit %d require both ES0 and ES1 "
                                          "in configuration\n"),
                               unit));
                    return SOC_E_PARAM;
                }
                if (!l2_entries && !l3_entries) { /* ACL only */
                    *ad_mode = 9; /* ES0:counter, ES1:policy */
                } else {
                    if (!l3_entries) { /* L2+ACL only */
                        *ad_mode = 10; /* ES0:counter/policy, ES1:L2/policy */
                    } else if (!l2_entries) { /* L3+ACL only */
                        *ad_mode = 11; /* ES0:counter/policy, ES1:L3/policy */
                    } else { /* can't do L2+L3+ACL */
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "ESM init: unit %d can't support L2 + L3 "
                                              "+ ACL on current sram speed %d MHz\n"),
                                   unit,
                                   cfg_sram_speed ? 334 : 250));
                        return SOC_E_PARAM;
                    }
                }
            }
        }
    } else {
        *ad_mode = cfg_ad_mode;
    }

    switch (*ad_mode) {
    case 1: /* ES0:L2 */
        l2_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM0;
        break;
    case 2: /* ES1:L2 */
        l2_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        break;
    case 3: /* ES0:L3 */
        l3_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM0;
        break;
    case 4: /* ES1:L3 */
        l3_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        break;
    case 5: /* ES0:L2/L3 */
        l2_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM0;
        l3_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM0;
        break;
    case 6: /* ES1:L2/L3 */
        l2_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        l3_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        break;
    case 7: /* ES0:ACL */
        acl_flags |= (TCAM_PARTITION_FLAGS_AD_IN_SRAM0 |
                      TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0);
        break;
    case 8: /* ES1:ACL */
        acl_flags |= (TCAM_PARTITION_FLAGS_AD_IN_SRAM1 |
                      TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM1);
        break;
    case 9: /* ES0:counter, ES1:policy */
        acl_flags |= (TCAM_PARTITION_FLAGS_AD_IN_SRAM1 |
                      TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0);
        break;
    case 10: /* ES0:counter/policy, ES1:L2/policy */
        l2_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        acl_flags |= (TCAM_PARTITION_FLAGS_AD_IN_SRAM0 |
                      TCAM_PARTITION_FLAGS_AD_IN_SRAM1 |
                      TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0);
        break;
    case 11: /* ES0:counter/policy, ES1:L3/policy */
        l3_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        acl_flags |= (TCAM_PARTITION_FLAGS_AD_IN_SRAM0 |
                      TCAM_PARTITION_FLAGS_AD_IN_SRAM1 |
                      TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0);
        break;
    case 12: /* ES0:ACL, ES1:L2/L3 */
        l2_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        l3_flags |= TCAM_PARTITION_FLAGS_AD_IN_SRAM1;
        acl_flags |= (TCAM_PARTITION_FLAGS_AD_IN_SRAM0 |
                      TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0);
        break;
    default:
        break;
    }

    partitions[TCAM_PARTITION_FWD_L2].flags = l2_flags;
    partitions[TCAM_PARTITION_FWD_IP4].flags = l3_flags;
    partitions[TCAM_PARTITION_FWD_IP6U].flags = l3_flags;
    partitions[TCAM_PARTITION_FWD_IP6].flags = l3_flags;
    partitions[TCAM_PARTITION_ACL_L2].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_IP4].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_IP6S].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_IP6F].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_L2C].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_IP4C].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_IP6C].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_L2IP4].flags = acl_flags;
    partitions[TCAM_PARTITION_ACL_L2IP6].flags = acl_flags;

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_set_sram_tuning_result(int unit, int intf_num)
{
    soc_tcam_info_t *tcam_info;
    soc_reg_t reg;
    uint32 tune_data, addr, rval, fval;
    int freq;
    char name_str[20];
    static const struct {
        soc_reg_t sram_ctl_reg;
        soc_reg_t tmode0_reg;
        soc_reg_t config_reg1;
        soc_reg_t config_reg2;
        soc_reg_t config_reg3;
    } ddr_reg[2] = {
        {
            ES0_SRAM_CTLr,
            ES0_DTU_LTE_TMODE0r,
            ES0_DDR36_CONFIG_REG1_ISr,
            ES0_DDR36_CONFIG_REG2_ISr,
            ES0_DDR36_CONFIG_REG3_ISr
        },
        {
            ES1_SRAM_CTLr,
            ES1_DTU_LTE_TMODE0r,
            ES1_DDR36_CONFIG_REG1_ISr,
            ES1_DDR36_CONFIG_REG2_ISr,
            ES1_DDR36_CONFIG_REG3_ISr
        }
    };

    tcam_info = SOC_CONTROL(unit)->tcam_info;

    sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING, intf_num);
    tune_data = soc_property_get(unit, name_str, 0);
    if (tune_data) {
        freq = (tune_data >> SOC_TR_MEMTUNE_DDR_FREQ_SHIFT) &
            SOC_TR_MEMTUNE_DDR_FREQ_MASK;
        if (freq != tcam_info->sram_freq) {
            tune_data = 0;
            LOG_CLI((BSL_META_U(unit,
                                "SRAM%d: Ignore %s, config was tuned at %d MHz, "
                                "current frequency is %d MHz\n"),
                     intf_num, name_str, freq, tcam_info->sram_freq));
        }
    }

    if (!tune_data) {
        /* ESx_SRAM_CTL register */
        reg = ddr_reg[intf_num].sram_ctl_reg;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
        soc_reg_field_set(unit, reg, &rval, NUM_R2W_NOPSf, 3);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
        return SOC_E_NOT_FOUND;
    }

    /* ESx_SRAM_CTL register */
    reg = ddr_reg[intf_num].sram_ctl_reg;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    fval = (tune_data >> SOC_TR_MEMTUNE_W2R_NOPS_SHIFT) &
        SOC_TR_MEMTUNE_W2R_NOPS_MASK;
    soc_reg_field_set(unit, reg, &rval, NUM_W2R_NOPSf, fval);
    fval = (tune_data >> SOC_TR_MEMTUNE_R2W_NOPS_SHIFT) &
        SOC_TR_MEMTUNE_R2W_NOPS_MASK;
    if (!fval) {
        fval = 1; /* min r2w_nops encoding is 1, 0 means 4 nops */
    }
    soc_reg_field_set(unit, reg, &rval, NUM_R2W_NOPSf, fval);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* ESx_DTU_LTE_TMODE0 register */
    reg = ddr_reg[intf_num].tmode0_reg;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    fval = (tune_data >> SOC_TR_MEMTUNE_EM_LATENCY_SHIFT) &
        SOC_TR_MEMTUNE_EM_LATENCY_MASK;
    soc_reg_field_set(unit, reg, &rval, EM_LATENCYf, fval);
    if (soc_reg_field_valid(unit, reg, EM_LATENCY8f)) {
        soc_reg_field_set(unit, reg, &rval, EM_LATENCY8f, fval == 0 ? 1 : 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* ESx_DDR36_CONFIG_REG1_IS register */
    reg = ddr_reg[intf_num].config_reg1;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    fval = (tune_data >> SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT) &
        SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK;
    soc_reg_field_set(unit, reg, &rval, DLL90_OFFSET_TXf, fval & 0x0f);
    soc_reg_field_set(unit, reg, &rval, DLL90_OFFSET_TX4f, fval >> 4);
    fval = (tune_data >> SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT) &
        SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK;
    soc_reg_field_set(unit, reg, &rval, DLL90_OFFSET_QKf, fval & 0xf);
    soc_reg_field_set(unit, reg, &rval, DLL90_OFFSET_QK4f, fval >> 4);
    soc_reg_field_set(unit, reg, &rval, DLL90_OFFSET_QKBf, fval & 0xf);
    soc_reg_field_set(unit, reg, &rval, DLL90_OFFSET_QKB4f, fval >> 4);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* ESx_DDR36_CONFIG_REG2_IS register */
    reg = ddr_reg[intf_num].config_reg2;
    if (soc_reg_field_valid(unit, reg, SEL_EARLY1_0f)) {
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
        fval = (tune_data >> SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT) &
            SOC_TR_MEMTUNE_DDR_LATENCY_MASK;
        soc_reg_field_set(unit, reg, &rval, SEL_EARLY2_0f, fval == 0 ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, SEL_EARLY1_0f, fval == 1 ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, SEL_EARLY2_1f, fval == 0 ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, SEL_EARLY1_1f, fval == 1 ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    }

    /* ESx_DDR36_CONFIG_REG3_IS register */
    reg = ddr_reg[intf_num].config_reg3;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    fval = (tune_data >> SOC_TR_MEMTUNE_OVRD_SM_SHIFT) &
        SOC_TR_MEMTUNE_OVRD_SM_MASK;
    if (fval) {
        fval = (tune_data >> SOC_TR_MEMTUNE_PHASE_SEL_SHIFT) &
            SOC_TR_MEMTUNE_PHASE_SEL_MASK;
        soc_reg_field_set(unit, reg, &rval, PHASE_SELf, fval);
        soc_reg_field_set(unit, reg, &rval, OVRD_SM_ENf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_set_tcam_tuning_result(int unit)
{
    soc_tcam_info_t *tcam_info;
    uint32 tune_data[2], rval, fval;
    int freq, use_midl, cur_use_midl;
    char name_str[20];

    tcam_info = SOC_CONTROL(unit)->tcam_info;

    sal_sprintf(name_str, "%s0", spn_EXT_TCAM_TUNING);
    tune_data[0] = soc_property_get(unit, name_str, 0);
    sal_sprintf(name_str, "%s1", spn_EXT_TCAM_TUNING);
    tune_data[1] = soc_property_get(unit, name_str, 0);
    if (tune_data[1]) {
        freq = (tune_data[1] >> SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT) &
            SOC_TR_MEMTUNE_TCAM_FREQ_MASK;
        if (freq != tcam_info->tcam_freq) {
            tune_data[0] = tune_data[1] = 0;
            LOG_CLI((BSL_META_U(unit,
                                "TCAM: Ignore %s0 and %s1, config was tuned at %d "
                                "MHz, current frequency is %d MHz\n"),
                     spn_EXT_TCAM_TUNING, spn_EXT_TCAM_TUNING,
                     freq, tcam_info->tcam_freq));
        }
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
        cur_use_midl =
            soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              MIDL_TX_ENf) &&
            soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              SEL_TX_CLKDLY_BLKf) ? 1 : 0;
        use_midl = (tune_data[1] >> SOC_TR_MEMTUNE_USE_MIDL_SHIFT) &
            SOC_TR_MEMTUNE_USE_MIDL_MASK;
        if (use_midl != cur_use_midl) {
            tune_data[0] = tune_data[1] = 0;
            LOG_CLI((BSL_META_U(unit,
                                "TCAM: Ignore %s0 and %s1, config was tuned using %s "
                                "current setting is %s\n"),
                     spn_EXT_TCAM_TUNING, spn_EXT_TCAM_TUNING,
                     use_midl ? "MIDL" : "VCDL",
                     cur_use_midl ? "MIDL" : "VCDL"));
        }
    }

    if (tune_data[0]) {
        SOC_IF_ERROR_RETURN(READ_ETC_CTLr(unit, &rval));
        fval = (tune_data[0] >> SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT) &
            SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK;
        soc_reg_field_set(unit, ETC_CTLr, &rval, RBUS_SYNC_DLYf, fval);
        fval = (tune_data[0] >> SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT) &
            SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK;
        soc_reg_field_set(unit, ETC_CTLr, &rval, DPEO_SYNC_DLYf, fval);
        SOC_IF_ERROR_RETURN(WRITE_ETC_CTLr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG3_ISr(unit, &rval));
        fval = (tune_data[0] >> SOC_TR_MEMTUNE_FCD_DPEO_SHIFT) &
            SOC_TR_MEMTUNE_FCD_DPEO_MASK;
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval, FCD_DPEO_0f,
                          fval);
#if 0 /* this is for 2nd device */
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval, FCD_DPEO_1f,
                          fval);
#endif
        fval = (tune_data[0] >> SOC_TR_MEMTUNE_FCD_RBUS_SHIFT) &
            SOC_TR_MEMTUNE_FCD_RBUS_MASK;
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval, FCD_SMFL_0f,
                          fval + 1);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval, FCD_SMFL_1f,
                          fval + 1);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval, FCD_RBUSf,
                          fval);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval, FCD_RVf,
                          fval);
        SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG3_ISr(unit, rval));
    }
    if (tune_data[1]) {
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
        fval = (tune_data[1] >> SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT) &
            SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK;
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          VCDL_RX_OFFSETf, fval & 0x1f);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          MSBMIDL_OFFSET_RXf, fval >> 5);
        fval = (tune_data[1] >> SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT) &
            SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK;
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          VCDL_TX_OFFSETf, fval & 0x1f);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          MSBMIDL_OFFSET_TXf, fval >> 5);
        SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval));
        if (soc_reg_field_valid(unit, ETU_DDR72_CONFIG_REG3_ISr,
                                INVERT_TXCLKf)) {
            SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG3_ISr(unit, &rval));
            fval = (tune_data[1] >> SOC_TR_MEMTUNE_INVERT_TXCLK_SHIFT) &
                SOC_TR_MEMTUNE_INVERT_TXCLK_MASK;
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval,
                              INVERT_TXCLKf, fval & 0x1f);
            fval = (tune_data[1] >> SOC_TR_MEMTUNE_INVERT_RXCLK_SHIFT) &
                SOC_TR_MEMTUNE_INVERT_RXCLK_MASK;
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval,
                              INVERT_RXCLKf, fval & 0x1f);
            SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG3_ISr(unit, rval));
        }
    }

    if (!tune_data[0] || !tune_data[1]) {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_set_sram_dac(int unit, int intf_num)
{
    soc_reg_t reg;
    uint32 addr, rval, fval;
    char name_str[20];
    static const struct {
        soc_reg_t config_reg3;
    } ddr_reg[2] = {
        {
            ES0_DDR36_CONFIG_REG3_ISr
        },
        {
            ES1_DDR36_CONFIG_REG3_ISr
        }
    };

    sal_sprintf(name_str, "sram%d_dac_value", intf_num);
    fval = soc_property_get(unit, name_str, 4);
    if (fval == -1) {
        return SOC_E_NOT_FOUND;
    }

    reg = ddr_reg[intf_num].config_reg3;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, BIASGEN_DAC_ENf, 1);
    soc_reg_field_set(unit, reg, &rval, BIASGEN_DAC_CTRLf, fval);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_set_tcam_dac(int unit)
{
    uint32 rval, fval;

    fval = soc_property_get(unit, spn_TCAM_PTR_DIST, 2);
    SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
    soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                      SEL_WRFIFO_PTR_CLKf, fval);
    SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval));

    fval = soc_property_get(unit, spn_TCAM_DAC_VALUE, 4);
    if (fval == -1) {
        return SOC_E_NOT_FOUND;
    }

    SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
    soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval, BIASGEN_DAC_ENf,
                      1);
    soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                      BIASGEN_DAC_CTRLf, fval);
    SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG2_ISr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_check_sram_tx_status(int unit, int intf_num,
                                           int sram_dac_en)
{
    soc_reg_t reg;
    uint32 addr, rval;
    static const struct {
        soc_reg_t status_reg2;
    } ddr_reg[2] = {
        {
            ES0_DDR36_STATUS_REG2_ISr
        },
        {
            ES1_DDR36_STATUS_REG2_ISr
        }
    };

    if (sram_dac_en) {
        return SOC_E_NONE;
    }

    reg = ddr_reg[intf_num].status_reg2;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    if (!soc_reg_field_get(unit, reg, rval, DLL_BIAS_GEN_LOCKEDf)) {
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_check_sram_rx_status(int unit, int intf_num)
{
    soc_reg_t reg;
    uint32 addr, rval;
    static const struct {
        soc_reg_t status_reg2;
    } ddr_reg[2] = {
        {
            ES0_DDR36_STATUS_REG2_ISr
        },
        {
            ES1_DDR36_STATUS_REG2_ISr
        }
    };

    reg = ddr_reg[intf_num].status_reg2;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    if (!soc_reg_field_get(unit, reg, rval, QK_DLLDSK_LOCKEDf) ||
        !soc_reg_field_get(unit, reg, rval, QKB_DLLDSKW_LOCKEDf)) {
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

/* Toggle STOP_DK on SRAM interfaces */
STATIC int
_soc_triumph_esm_init_stop_dk_old(int unit)
{
    soc_reg_t reg;
    uint32 addr, rval;
    int i;
    static const struct {
        soc_reg_t config_reg1;
    } ddr_reg[2] = {
        {
            ES0_DDR36_CONFIG_REG1_ISr
        },
        {
            ES1_DDR36_CONFIG_REG1_ISr
        },
    };

    for (i = 0; i < 2; i++) {
        reg = ddr_reg[i].config_reg1;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
        soc_reg_field_set(unit, reg, &rval, STOP_DKf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    }

    sal_usleep(1000000);

    for (i = 0; i < 2; i++) {
        reg = ddr_reg[i].config_reg1;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
        soc_reg_field_set(unit, reg, &rval, STOP_DKf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    }

    sal_usleep(1000000);

    return SOC_E_NONE;
}

/* Toggle STOP_DK on SRAM interface */
STATIC int
_soc_triumph_esm_init_stop_dk(int unit, int intf_num)
{
    soc_reg_t reg;
    uint32 addr, rval;
    static const struct {
        soc_reg_t config_reg1;
    } ddr_reg[2] = {
        {
            ES0_DDR36_CONFIG_REG1_ISr
        },
        {
            ES1_DDR36_CONFIG_REG1_ISr
        }
    };

    reg = ddr_reg[intf_num].config_reg1;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, STOP_DKf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* No delay is needed */

    soc_reg_field_set(unit, reg, &rval, STOP_DKf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* Wait for the external SRAM to lock within 100us */
    sal_usleep(100);

    return SOC_E_NONE;
}

/* Relock DLL on SRAM interface */
STATIC int
_soc_triumph_esm_init_relock_dll_old(int unit, int intf_num)
{
    soc_reg_t reg;
    uint32 addr, rval;
    static const struct {
        soc_reg_t config_reg1;
        soc_reg_t config_reg3;
    } ddr_reg[2] = {
        {
            ES0_DDR36_CONFIG_REG1_ISr,
            ES0_DDR36_CONFIG_REG3_ISr
        },
        {
            ES1_DDR36_CONFIG_REG1_ISr,
            ES1_DDR36_CONFIG_REG3_ISr
        },
    };

    reg = ddr_reg[intf_num].config_reg1;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, RELOCK_DLLf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    sal_usleep(100);
    soc_reg_field_set(unit, reg, &rval, ENABLE_DDRf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    sal_usleep(2000000);
    soc_reg_field_set(unit, reg, &rval, RELOCK_DLLf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    sal_usleep(100);
    soc_reg_field_set(unit, reg, &rval, ENABLE_DDRf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    sal_usleep(2000000);

    reg = ddr_reg[intf_num].config_reg3;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, BIASGEN_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    sal_usleep(10000);
    soc_reg_field_set(unit, reg, &rval, BIASGEN_RESETf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    sal_usleep(10000);

    return SOC_E_NONE;
}

/* Relock DLL on SRAM interface */
STATIC int
_soc_triumph_esm_init_relock_dll(int unit, int intf_num, int do_extra_delay)
{
    soc_reg_t reg;
    uint32 addr, rval;
    static const struct {
        soc_reg_t config_reg1;
        soc_reg_t config_reg3;
    } ddr_reg[2] = {
        {
            ES0_DDR36_CONFIG_REG1_ISr,
            ES0_DDR36_CONFIG_REG3_ISr
        },
        {
            ES1_DDR36_CONFIG_REG1_ISr,
            ES1_DDR36_CONFIG_REG3_ISr
        }
    };

    reg = ddr_reg[intf_num].config_reg1;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, ENABLE_DDRf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* Reset SRAM DLL for 25us */
    sal_usleep(do_extra_delay ? 1000000 : 25);

    soc_reg_field_set(unit, reg, &rval, ENABLE_DDRf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* Wait for the external SRAM to lock within 100us */
    sal_usleep(do_extra_delay ? 1000000 : 100);

    return SOC_E_NONE;
}

STATIC int
_soc_triumph_esm_init_set_pvt(int unit, int intf_num)
{
    soc_reg_t reg;
    uint32 pvt_data, addr, rval, fval;
    char name_str[20];
    static const struct {
        soc_reg_t config_reg2;
    } ddr_reg[3] = {
        {
            ES0_DDR36_CONFIG_REG2_ISr
        },
        {
            ES1_DDR36_CONFIG_REG2_ISr
        },
        {
            ETU_DDR72_CONFIG_REG2_ISr
        }
    };

    if (intf_num < 2) {
        sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_PVT, intf_num);
    } else {
        sal_sprintf(name_str, "%s", spn_EXT_TCAM_PVT);
    }

    pvt_data = soc_property_get(unit, name_str, 0);
    if (!pvt_data) {
        return SOC_E_NOT_FOUND;
    }

    reg = ddr_reg[intf_num].config_reg2;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    if ((pvt_data >> SOC_TR_MEMTUNE_OVRD_ODTRES_SHIFT) &
        SOC_TR_MEMTUNE_OVRD_ODTRES_MASK) {
        fval = (pvt_data >> SOC_TR_MEMTUNE_ODTRES_SHIFT) &
            SOC_TR_MEMTUNE_ODTRES_MASK;
        soc_reg_field_set(unit, reg, &rval, PVT_ODTRES_VALf, fval);
        soc_reg_field_set(unit, reg, &rval, OVRD_EN_ODTRES_PVTf, 1);
    }
    if ((pvt_data >> SOC_TR_MEMTUNE_OVRD_DRIVER_SHIFT) &
        SOC_TR_MEMTUNE_OVRD_DRIVER_MASK) {
        fval = (pvt_data >> SOC_TR_MEMTUNE_PDRIVE_SHIFT) &
            SOC_TR_MEMTUNE_PDRIVE_MASK;
        soc_reg_field_set(unit, reg, &rval, PVT_PDRIVE_VALf, fval);
        fval = (pvt_data >> SOC_TR_MEMTUNE_NDRIVE_SHIFT) &
            SOC_TR_MEMTUNE_NDRIVE_MASK;
        soc_reg_field_set(unit, reg, &rval, PVT_NDRIVE_VALf, fval);
        soc_reg_field_set(unit, reg, &rval, OVRD_EN_DRIVER_PVTf, 1);
    }
    if ((pvt_data >> SOC_TR_MEMTUNE_OVRD_SLEW_SHIFT) &
        SOC_TR_MEMTUNE_OVRD_SLEW_MASK) {
        fval = (pvt_data >> SOC_TR_MEMTUNE_SLEW_SHIFT) &
            SOC_TR_MEMTUNE_SLEW_MASK;
        soc_reg_field_set(unit, reg, &rval, PVT_SLEW_VALf, fval);
        soc_reg_field_set(unit, reg, &rval, OVRD_EN_SLEW_PVTf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    return SOC_E_NONE;
}

int
soc_triumph_esm_init_pvt_comp(int unit, int intf_num)
{
    uint16 dev_id;
    uint8 rev_id;
    soc_reg_t reg, status_reg;
    uint32 addr, status_addr, rval;
    uint32 odt, slew, pdrive, ndrive;
    static const struct {
        char *intf_name;
        soc_reg_t config_reg2;
        soc_reg_t status_reg1;
        soc_reg_t status_reg2;
    } ddr_reg[3] = {
        {
            "SRAM0",
            ES0_DDR36_CONFIG_REG2_ISr,
            ES0_DDR36_STATUS_REG1_ISr,
            ES0_DDR36_STATUS_REG2_ISr
        },
        {
            "SRAM1",
            ES1_DDR36_CONFIG_REG2_ISr,
            ES1_DDR36_STATUS_REG1_ISr,
            ES1_DDR36_STATUS_REG2_ISr
        },
        {
            "TCAM",
            ETU_DDR72_CONFIG_REG2_ISr,
            ETU_DDR72_STATUS_REG1_ISr,
            ETU_DDR72_STATUS_REG2_ISr
        }
    };

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* ES0 has its own PVT pads, ES1 and TCAM share the same PVT pads */
    if (intf_num != 0) {
        /* Disable "the other" PVT cell */
        reg = ddr_reg[3 - intf_num].config_reg2;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
        soc_reg_field_set(unit, reg, &rval, PVT_COMP_PAD_ENf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

        /* Enable the "target" PVT cell */
        reg = ddr_reg[intf_num].config_reg2;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
        soc_reg_field_set(unit, reg, &rval, PVT_COMP_PAD_ENf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
    }

    reg = ddr_reg[intf_num].config_reg2;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);

    /* Override ODT vaue */
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, PVT_ODTRES_VALf, 8);
    soc_reg_field_set(unit, reg, &rval, OVRD_EN_ODTRES_PVTf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    soc_reg_field_set(unit, reg, &rval, OVRD_EN_ODTRES_PVTf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* Override pdrive, ndrive value */
    soc_reg_field_set(unit, reg, &rval, PVT_PDRIVE_VALf, 8);
    soc_reg_field_set(unit, reg, &rval, PVT_NDRIVE_VALf, 8);
    soc_reg_field_set(unit, reg, &rval, OVRD_EN_DRIVER_PVTf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    soc_reg_field_set(unit, reg, &rval, OVRD_EN_DRIVER_PVTf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    sal_usleep(1000);

    /* Get PVT status */
    status_reg = ddr_reg[intf_num].status_reg2;
    status_addr = soc_reg_addr(unit, status_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, status_addr, &rval));
    odt = soc_reg_field_get(unit, status_reg, rval, PVT_ODT_VALf);
    slew = soc_reg_field_get(unit, status_reg, rval, PVT_SLEW_VALf);
    ndrive = soc_reg_field_get(unit, status_reg, rval, PVT_NDRIVE_VALf);
    pdrive = soc_reg_field_get(unit, status_reg, rval, PVT_PDRIVE_VALf);

    status_reg = ddr_reg[intf_num].status_reg1;
    status_addr = soc_reg_addr(unit, status_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, status_addr, &rval));

    /* Check ODT PVT status and auto value */
    if (!soc_reg_field_get(unit, status_reg, rval, ODT_PVT_DONEf) &&
        odt != 0 && odt != 0xf) {
        LOG_CLI((BSL_META_U(unit,
                            "%s ODT PVT compensation fail\n"),
                 ddr_reg[intf_num].intf_name));
    }

    /* Check NDRIVE PVT status and auto value */
    if (!soc_reg_field_get(unit, status_reg, rval, NDRIVER_PVT_DONEf) &&
        ndrive != 0 && ndrive != 0xf) {
        LOG_CLI((BSL_META_U(unit,
                            "%s NDRIVE PVT compensation fail\n"),
                 ddr_reg[intf_num].intf_name));
    }

    /* Check PDRIVE PVT status and auto value */
    if (!soc_reg_field_get(unit, status_reg, rval, PDRIVER_PVT_DONEf) &&
        pdrive != 0 && pdrive != 0xf) {
        LOG_CLI((BSL_META_U(unit,
                            "%s PDRIVE PVT compensation fail\n"),
                 ddr_reg[intf_num].intf_name));
    }

    /* Override ODT vaue */
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, PVT_ODTRES_VALf, odt);
    soc_reg_field_set(unit, reg, &rval, OVRD_EN_ODTRES_PVTf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* Override pdrive, ndrive value */
    soc_reg_field_set(unit, reg, &rval, PVT_PDRIVE_VALf, pdrive);
    soc_reg_field_set(unit, reg, &rval, PVT_NDRIVE_VALf, ndrive);
    soc_reg_field_set(unit, reg, &rval, OVRD_EN_DRIVER_PVTf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* override slew value */
    if ((SOC_IS_TRIUMPH(unit) && rev_id >= BCM56624_B2_REV_ID) ||
        (SOC_IS_TRIUMPH2(unit) && rev_id >= BCM56634_B0_REV_ID)) {
        if (slew < 4) {
            slew = 4;
        }
    }
    soc_reg_field_set(unit, reg, &rval, PVT_SLEW_VALf, slew);
    soc_reg_field_set(unit, reg, &rval, OVRD_EN_SLEW_PVTf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    if (reg != ES0_DDR36_CONFIG_REG2_ISr) {
        /* Enable PVT cell */
        SOC_IF_ERROR_RETURN(READ_ES1_DDR36_CONFIG_REG2_ISr(unit, &rval));
        soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                          PVT_COMP_PAD_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ES1_DDR36_CONFIG_REG2_ISr(unit, rval));

        /* Enable TCAM PVT cell */
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                          PVT_COMP_PAD_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG2_ISr(unit, rval));
    }

    return SOC_E_NONE;
}

/* Perform mini-tuning to find latency value */
STATIC int
_soc_triumph_esm_init_retune_sram(int unit, int intf_num)
{
    soc_memtune_data_t mt_data;
    soc_reg_t reg;
    uint32 tune_data, addr, rval;
    char name_str[20];
    static const struct {
        soc_reg_t tmode1_reg;
        soc_reg_t config_reg1;
    } ddr_reg[2] = {
        {
            ES0_DTU_LTE_TMODE1r,
            ES0_DDR36_CONFIG_REG1_ISr
        },
        {
            ES1_DTU_LTE_TMODE1r,
            ES1_DDR36_CONFIG_REG1_ISr
        }
    };

    if (!soc_feature(unit, soc_feature_esm_rxfifo_resync) ||
         SOC_WARM_BOOT(unit)) {
        /* No need to retune SRAM interface */
        return SOC_E_NONE;
    }

    sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING, intf_num);
    tune_data = soc_property_get(unit, name_str, 0);
    if (!tune_data) {
        /* Can not find TX/RX value from tuning result for mini-tuning */
        return SOC_E_NONE;
    }

    reg = ddr_reg[intf_num].config_reg1;
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, reg, REG_PORT_ANY, EN_RDFIFOf, 0));

    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, reg, REG_PORT_ANY, EN_RDFIFOf, 1));

    reg = ddr_reg[intf_num].tmode1_reg;
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
    soc_reg_field_set(unit, reg, &rval, WDOEBFf, 0);
    soc_reg_field_set(unit, reg, &rval, WDOEBRf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    sal_memset(&mt_data, 0, sizeof(mt_data));
    mt_data.interface = SOC_MEM_INTERFACE_SRAM;
    mt_data.sub_interface = intf_num;
    mt_data.slice_mask = SOC_TR_SRAM_SLICE_MASK;
    mt_data.config = TRUE;
    mt_data.freq = -1;
    mt_data.test_count = 10;
    mt_data.do_txrx_first = TRUE;
    mt_data.d0r_0 = 0xffffffff;
    mt_data.adr0 = -1;
    mt_data.adr1 = -1;
    mt_data.adr_mode = -1;
    mt_data.man_em_latency = -1;
    mt_data.man_ddr_latency = -1;
    mt_data.man_tx_offset =
        (tune_data >> SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT) &
        SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK;
    mt_data.man_rx_offset =
        (tune_data >> SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT) &
        SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK;

    if (SOC_FAILURE(soc_mem_interface_tune(unit, &mt_data))) {
        return SOC_E_NONE; /* return OK, expect to have manual re-tune */
    }

    return SOC_E_NONE;
}

/* Perform mini-tuning to stabilize TCAM interface */
STATIC int
_soc_triumph_esm_init_retune_tcam(int unit)
{
    soc_memtune_data_t mt_data;
    uint32 tune_data;
    char name_str[20];

    sal_sprintf(name_str, "%s1", spn_EXT_TCAM_TUNING);
    tune_data = soc_property_get(unit, name_str, 0);
    if (!tune_data) {
        /* Can not find TX/RX value from tuning result for mini-tuning */
        return SOC_E_NONE;
    }

    sal_memset(&mt_data, 0, sizeof(mt_data));
    mt_data.interface = SOC_MEM_INTERFACE_TCAM;
    mt_data.config = TRUE;
    mt_data.freq = -1;
    mt_data.test_count = 10;
    mt_data.mask[0] = 0xffffffff;
    mt_data.tcam_loop_count = 100;
    mt_data.man_tx_offset =
        (tune_data >> SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT) &
        SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK;
    mt_data.man_rx_offset =
        (tune_data >> SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT) &
        SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK;

    if (SOC_FAILURE(soc_mem_interface_tune(unit, &mt_data))) {
        return SOC_E_NONE; /* return OK, expect to have manual re-tune */
    }
    /* TCAM tuning may raise interrupt, report and clear it */
    if (SOC_IS_TRIUMPH(unit)) {
        _soc_triumph_esm_process_intr_status(unit);
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    else if (SOC_IS_TRIUMPH2(unit)) {
        _soc_triumph2_esm_process_intr_status(unit);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_triumph_esm_init_read_config(unit));

    SOC_IF_ERROR_RETURN(soc_tcam_init(unit));

    return SOC_E_NONE;
}

/* Program tcam index to sram index translation table */
STATIC int
_soc_triumph_esm_init_set_et_pa_xlate(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    et_pa_xlat_entry_t xlat_entry;
    int sram0_base, sram1_base, counter_base, hbit_base;
    int part, index, end, entries_per_block;
    int has_counter, has_hbit;
    uint32 flags;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }
    partitions = tcam_info->partitions;

    sram0_base = 0;
    sram1_base = 0;
    counter_base = 0;
    hbit_base = 0;

    /* upper layer software requires all counters to be continguous */
    for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT; part++) {
        if (!partitions[part].num_entries) {
            continue;
        }
        flags = partitions[part].flags;
        if (flags & TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0) {
            sram0_base += partitions[part].num_entries_include_pad * 2;
        } else if (flags & TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM1) {
            sram1_base += partitions[part].num_entries_include_pad * 2;
        }
    }

    index = 0;
    for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT; part++) {
        if (!partitions[part].num_entries) {
            continue;
        }

        has_hbit = FALSE;
        has_counter = FALSE;
        flags = partitions[part].flags;
        switch (flags & TCAM_PARTITION_FLAGS_TYPE_MASK) {
        case TCAM_PARTITION_FLAGS_TYPE_L2:
            partitions[part].sram_width_shift = 1;
            has_hbit = TRUE;
            break;
        case TCAM_PARTITION_FLAGS_TYPE_L3:
            partitions[part].sram_width_shift = 0;
            has_hbit = TRUE;
            break;
        case TCAM_PARTITION_FLAGS_TYPE_ACL:
            if (flags & TCAM_PARTITION_FLAGS_AD_IN_SRAM0 &&
                flags & TCAM_PARTITION_FLAGS_AD_IN_SRAM1) {
                partitions[part].sram_width_shift = 1;
                if (sram0_base > sram1_base) {
                    sram1_base = sram0_base; /* align both, waste some space */
                } else {
                    sram0_base = sram1_base; /* align both, waste some sapce */
                }
            } else {
                partitions[part].sram_width_shift = 2;
            }
            has_counter = TRUE;
            break;
        default:
            continue;
        }

        partitions[part].sram_base = flags & TCAM_PARTITION_FLAGS_AD_IN_SRAM0 ?
                                     sram0_base : sram1_base;
        partitions[part].counter_base = has_counter ? counter_base : -1;
        partitions[part].hbit_base = has_hbit ? hbit_base : -1;

        sal_memset(&xlat_entry, 0, sizeof(xlat_entry));
        soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry, ES_WIDTHf,
                            partitions[part].sram_width_shift);
        soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry, ET_WIDTHf,
                            partitions[part].tcam_width_shift);
        if (has_hbit) {
            soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry, HBIT_ENf, 1);
        }
        if (part == TCAM_PARTITION_FWD_L2) {
            soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry, PPA_ENf, 1);
            soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry, AGE_ENf, 1);
        }

        index = partitions[part].tcam_base >> 14;
        end = index + (((partitions[part].num_entries - 1) <<
                        partitions[part].tcam_width_shift) >> 14);
        for (; index <= end; index++) {
            entries_per_block = (1 << 14) >> partitions[part].tcam_width_shift;
            if (has_counter) {
                soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry,
                                    ES_CNTR_PA_BASEf, counter_base >> 10);
                counter_base += entries_per_block * 2;
            }
            if (has_hbit) {
                soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry,
                                    HBIT_PA_BASEf, hbit_base >> 11);
                hbit_base += entries_per_block;
            }
            if (flags & TCAM_PARTITION_FLAGS_AD_IN_SRAM0) {
                soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry,
                                    ES_PA_BASEf, sram0_base >> 10);
                sram0_base +=
                    entries_per_block << partitions[part].sram_width_shift;
            }
            if (flags & TCAM_PARTITION_FLAGS_AD_IN_SRAM1) {
                soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry,
                                    ES_PA_BASEf, sram1_base >> 10);
                sram1_base +=
                    entries_per_block << partitions[part].sram_width_shift;
            }

            SOC_IF_ERROR_RETURN(WRITE_ET_PA_XLATm(unit, SOC_BLOCK_ALL, index,
                                                  &xlat_entry));
        }
    }
    sal_memset(&xlat_entry, 0, sizeof(xlat_entry));
    soc_mem_field32_set(unit, ET_PA_XLATm, &xlat_entry, ES_WIDTHf, 3);
    end = soc_mem_index_max(unit, ET_PA_XLATm);
    for (; index <= end; index++) {
        SOC_IF_ERROR_RETURN(WRITE_ET_PA_XLATm(unit, SOC_BLOCK_ALL, index,
                                              &xlat_entry));
    }

    return SOC_E_NONE;
}

/* Program software memory state according to table size in user config */
STATIC int
_soc_triumph_esm_init_adjust_mem_size(int unit)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    uint16 value = 0;

    sop = SOC_PERSIST(unit);
    memState = sop->memState;
    tcam_info = SOC_CONTROL(unit)->tcam_info;
    partitions = tcam_info->partitions;

    /* L2 forwarding partition */
    memState[EXT_L2_ENTRYm].index_max = /* TCAM + SRAM */
        memState[EXT_L2_ENTRY_TCAMm].index_max = /* TCAM */
        memState[EXT_L2_ENTRY_DATAm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_L2].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_L2m].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_L2m].index_max = /* DST hit bit */
        (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 - 1;

    /* IP4 forwarding partition */
    memState[EXT_IPV4_DEFIPm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV4_DEFIP_TCAMm].index_max = /* TCAM */
        memState[EXT_DEFIP_DATA_IPV4m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP4].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV4m].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV4m].index_max = /* DST hit bit */
        (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 - 1;

    /* IP6 64-bit prefix forwarding partition */
    memState[EXT_IPV6_64_DEFIPm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV6_64_DEFIP_TCAMm].index_max = /* TCAM */
        memState[EXT_DEFIP_DATA_IPV6_64m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP6U].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV6_64m].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV6_64m].index_max = /* DST hit bit */
        (partitions[TCAM_PARTITION_FWD_IP6U].num_entries + 31) / 32 - 1;

    /* IP6 128-bit prefix forwarding partition */
    memState[EXT_IPV6_128_DEFIPm].index_max = /* TCAM + SRAM */
        memState[EXT_IPV6_128_DEFIP_TCAMm].index_max = /* TCAM */
        memState[EXT_DEFIP_DATA_IPV6_128m].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP6].num_entries - 1;
    memState[EXT_SRC_HIT_BITS_IPV6_128m].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITS_IPV6_128m].index_max = /* DST hit bit */
        (partitions[TCAM_PARTITION_FWD_IP6].num_entries + 31) / 32 - 1;

    /* L2 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_L2m].index_max = /* TCAM */
        memState[EXT_FP_POLICY_ACL288_L2m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL288_L2m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_L2].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL288_L2m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 - 1;

    /* IP4 288-bit ACL partition */
    memState[EXT_ACL288_TCAM_IPV4m].index_max = /* TCAM */
        memState[EXT_FP_POLICY_ACL288_IPV4m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL288_IPV4m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_IP4].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL288_IPV4m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 - 1;

    /* IP6 short form 360-bit ACL partition */
    memState[EXT_ACL360_TCAM_DATA_IPV6_SHORTm].index_max = /* TCAM data */
        memState[EXT_FP_POLICY_ACL360_IPV6_SHORTm].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL360_IPV6_SHORTm].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_IP6S].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL360_IPV6_SHORTm].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 - 1;

    /* IP6 full form 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_IPV6_LONGm].index_max = /* TCAM data */
        memState[EXT_FP_POLICY_ACL432_IPV6_LONGm].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL432_IPV6_LONGm].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_IP6F].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL432_IPV6_LONGm].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 - 1;

    /* L2 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_L2m].index_max = /* TCAM */
        memState[EXT_FP_POLICY_ACL144_L2m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL144_L2m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_L2C].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL144_L2m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_L2C].num_entries + 7) / 8 - 1;

    /* IP4 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV4m].index_max = /* TCAM */
        memState[EXT_FP_POLICY_ACL144_IPV4m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL144_IPV4m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_IP4C].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL144_IPV4m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_IP4C].num_entries + 7) / 8 - 1;

    /* IP6 compact form 144-bit ACL partition */
    memState[EXT_ACL144_TCAM_IPV6m].index_max = /* TCAM */
        memState[EXT_FP_POLICY_ACL144_IPV6m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL144_IPV6m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_IP6C].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL144_IPV6m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_IP6C].num_entries + 7) / 8 - 1;

    /* L2+IP4 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV4m].index_max = /* TCAM data */
        memState[EXT_FP_POLICY_ACL432_L2_IPV4m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL432_L2_IPV4m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_L2IP4].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL432_L2_IPV4m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries + 7) / 8 - 1;

    /* L2+IP6 432-bit ACL partition */
    memState[EXT_ACL432_TCAM_DATA_L2_IPV6m].index_max = /* TCAM data */
        memState[EXT_FP_POLICY_ACL432_L2_IPV6m].index_max = /* SRAM */
        memState[EXT_FP_CNTR_ACL432_L2_IPV6m].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_L2IP6].num_entries - 1;
    memState[EXT_FP_CNTR8_ACL432_L2_IPV6m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_L2IP6].num_entries + 7) / 8 - 1;

    /* All L2+L3 forwarding entries */
    memState[EXT_SRC_HIT_BITSm].index_max = /* SRC hit bit */
        memState[EXT_DST_HIT_BITSm].index_max = /* DST hit bit */
        (partitions[TCAM_PARTITION_FWD_L2].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP4].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP6U].num_entries + 31) / 32 +
        (partitions[TCAM_PARTITION_FWD_IP6].num_entries + 31) / 32 - 1;

    /* All L3 forwarding entries */
    memState[EXT_DEFIP_DATAm].index_max = /* SRAM */
        partitions[TCAM_PARTITION_FWD_IP4].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6U].num_entries +
        partitions[TCAM_PARTITION_FWD_IP6].num_entries - 1;

    /* All 144-bit ACL entries */
    memState[EXT_ACL144_TCAMm].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_L2C].num_entries +
        partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6C].num_entries - 1;

    /* All 288-bit ACL entries */
    memState[EXT_ACL288_TCAMm].index_max = /* TCAM */
        partitions[TCAM_PARTITION_ACL_L2].num_entries +
        partitions[TCAM_PARTITION_ACL_IP4].num_entries - 1;

    /* All 360-bit ACL entries */
    memState[EXT_ACL360_TCAM_DATAm].index_max = /* TCAM data */
        partitions[TCAM_PARTITION_ACL_IP6S].num_entries - 1;

    /* All 432-bit ACL entries */
    memState[EXT_ACL432_TCAM_DATAm].index_max = /* TCAM data */
        partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
        partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
        partitions[TCAM_PARTITION_ACL_L2IP6].num_entries - 1;

    /* All ACL entries */
    memState[EXT_FP_POLICYm].index_max = /* SRAM */
        memState[EXT_FP_CNTRm].index_max = /* SRAM counter */
        partitions[TCAM_PARTITION_ACL_L2].num_entries +
        partitions[TCAM_PARTITION_ACL_IP4].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6S].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6F].num_entries +
        partitions[TCAM_PARTITION_ACL_L2C].num_entries +
        partitions[TCAM_PARTITION_ACL_IP4C].num_entries +
        partitions[TCAM_PARTITION_ACL_IP6C].num_entries +
        partitions[TCAM_PARTITION_ACL_L2IP4].num_entries +
        partitions[TCAM_PARTITION_ACL_L2IP6].num_entries - 1;
    memState[EXT_FP_CNTR8m].index_max = /* SRAM counter */
        (partitions[TCAM_PARTITION_ACL_L2].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_IP4].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_IP6S].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_IP6F].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_L2C].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_IP4C].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_IP6C].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries + 7) / 8 +
        (partitions[TCAM_PARTITION_ACL_L2IP6].num_entries + 7) / 8 - 1;

    if (SOC_IS_TRIUMPH(unit) || SOC_IS_TRIUMPH2(unit)) {
        /* Set maximum supported vrf id for the device.
         * For Triumph & Triumph2 devices vrf id needs to be updated
         * after max index values are updated */
        SOC_IF_ERROR_RETURN(soc_max_supported_vrf_get(unit, &value));
        SOC_VRF_MAX_SET(unit, value);
    }

    return SOC_E_NONE;
}

/* Clear all tables to ensure sram tables have valid CRC/ECC */
STATIC int
_soc_triumph_esm_init_clear_all_entries(int unit)
{
    soc_tcam_info_t *tcam_info;
    tr_ext_sram_bist_t sram_bist;
    soc_timeout_t to;
    uint32 rval, to_usec;

    tcam_info = SOC_CONTROL(unit)->tcam_info;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    if (!SAL_BOOT_SIMULATION) {
        /* Clear SRAM in order to have correct ECC value on each entry */
        sal_memset(&sram_bist, 0, sizeof(sram_bist));
        sram_bist.adr0 = 0;
        sram_bist.adr1 = 0x3ffffe;
        sram_bist.adr_mode = 2;
        sram_bist.em_latency = -1;
        sram_bist.w2r_nops = 3;
        sram_bist.r2w_nops = 3;
        sram_bist.loop_mode = 0; /* WW */

        soc_triumph_ext_sram_enable_set(unit, 0, TRUE, TRUE);
        soc_triumph_ext_sram_bist_setup(unit, 0, &sram_bist);
        soc_triumph_ext_sram_op(unit, 0, &sram_bist, NULL);
        soc_triumph_ext_sram_enable_set(unit, 0, FALSE, TRUE);

        soc_triumph_ext_sram_enable_set(unit, 1, TRUE, TRUE);
        soc_triumph_ext_sram_bist_setup(unit, 1, &sram_bist);
        soc_triumph_ext_sram_op(unit, 1, &sram_bist, NULL);
        soc_triumph_ext_sram_enable_set(unit, 1, FALSE, TRUE);

        /* Clear hit bit */
        rval = 0;
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &rval, EXT_DST_HIT_BITSf, 1);
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &rval, EXT_SRC_HIT_BITSf, 1);
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &rval, EXT_L2_MOD_FIFOf, 1);
        soc_reg_field_set(unit, ETU_INT_MEM_RSTr, &rval, STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ETU_INT_MEM_RSTr(unit, rval));
        soc_timeout_init(&to, to_usec, 0);
        while (TRUE) {
            SOC_IF_ERROR_RETURN(READ_ETU_INT_MEM_RSTr(unit, &rval));
            if (soc_reg_field_get(unit, ETU_INT_MEM_RSTr, rval, COMPLETEf)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                return SOC_E_TIMEOUT;
            }
        }
    }

    if (tcam_info->partitions[TCAM_PARTITION_FWD_L2].num_entries) {
        /* set Free Bit for each TCAM entries */
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, EXT_L2_ENTRY_TCAMm,
                                          MEM_BLOCK_ALL, TRUE));
    }

    if (SOC_CONTROL(unit)->ext_l2_ppa_info != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_info);
        SOC_CONTROL(unit)->ext_l2_ppa_info = NULL;
    }
    if (SOC_CONTROL(unit)->ext_l2_ppa_vlan != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_vlan);
        SOC_CONTROL(unit)->ext_l2_ppa_vlan = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Forwarding key value encoding:
 *   L2 (always 72-bit)
 *   IP4 (always 72-bit)
 *   IP6 (0:72-bit, 1:144-bit)
 * ACL key value encoding:
 *   L2 (0:disable, 1:288-bit, 2:144-bit)
 *   IP4 (0:disable, 1:288-bit, 2:144-bit, 3:L2+IP4, 4:L2)
 *   IP6 (0:disable, 1:360-bit, 2:432-bit, 3:144-bit, 4:L2+IP6, 5:L2)
 */
STATIC int
_soc_triumph_esm_init_set_esm_mode_per_port(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int fwd_ip6_key, acl_l2_key, acl_ip4_key, acl_ip6_key;
    uint32 rval;
    int port;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    partitions = tcam_info->partitions;

    fwd_ip6_key = soc_property_get(unit, spn_EXT_IP6_FWD_KEY, -1);
    acl_l2_key = soc_property_get(unit, spn_EXT_L2_ACL_KEY, -1);
    acl_ip4_key = soc_property_get(unit, spn_EXT_IP4_ACL_KEY, -1);
    acl_ip6_key = soc_property_get(unit, spn_EXT_IP6_ACL_KEY, -1);

    /* IP6 FWD key is not specified in config, figure it out from table size */
    if (fwd_ip6_key == -1) {
        fwd_ip6_key = 0; /* default 72-bit key (prefix length 64) */
        if (!partitions[TCAM_PARTITION_FWD_IP6U].num_entries) {
            if (partitions[TCAM_PARTITION_FWD_IP6].num_entries) {
                fwd_ip6_key = 1; /* 144-bit key (prefix length 128) */
            }
        }
    }

    /* L2 ACL key is not specified in config, figure it out from table size */
    if (acl_l2_key == -1) { /* L2 ACL key not specified in config */
        acl_l2_key = 0; /* default disable */
        if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            acl_l2_key = 1; /* 288-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_L2C].num_entries) {
            acl_l2_key = 2; /* 144-bit key */
        }
    }

    /* IP4 ACL key is not specified in config, figure it out from table size */
    if (acl_ip4_key == -1) { /* IP4 ACL key not specified in config */
        acl_ip4_key = 0; /* default disable */
        if (partitions[TCAM_PARTITION_ACL_IP4].num_entries) {
            acl_ip4_key = 1; /* 288-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_IP4C].num_entries) {
            acl_ip4_key = 2; /* 144-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_L2IP4].num_entries) {
            acl_ip4_key = 3; /* 432-bit L2+L3 key */
        } else if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            acl_ip4_key = 4; /* 288-bit L2 key */
        }
    }

    /* IP6 ACL key is not specified in config, figure it out from table size */
    if (acl_ip6_key == -1) { /* IP6 ACL key not specified in config */
        acl_ip6_key = 0; /* default disable */
        if (partitions[TCAM_PARTITION_ACL_IP6S].num_entries) {
            acl_ip6_key = 1; /* 360-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_IP6F].num_entries) {
            acl_ip6_key = 2; /* 432-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_IP6C].num_entries) {
            acl_ip6_key = 3; /* 144-bit key */
        } else if (partitions[TCAM_PARTITION_ACL_L2IP6].num_entries) {
            acl_ip6_key = 4; /* 432-bit L2+L3 key */
        } else if (partitions[TCAM_PARTITION_ACL_L2].num_entries) {
            acl_ip6_key = 5; /* 288-bit L2 key */
        }
    }

    if (fwd_ip6_key < 0 || fwd_ip6_key > 1 ||
        acl_l2_key < 0 || acl_l2_key > 2 ||
        acl_ip4_key < 0 || acl_ip4_key > 4 ||
        acl_ip6_key < 0 || acl_ip6_key > 5) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "ESM init: unit %d incorrect key selection\n"),
                              unit));
        return SOC_E_PARAM;
    }

    /*
     * 350 MHz 6k (subtype 3):
     *   no L2 (learning) + L3 forward mode
     *   IP6 ACL only support 144 bit
     * 350 MHz 7k (subtype 4):
     *   L2 (learning) + L3 forward mode only allow 72 bit IP6
     *   IP6 ACL only support 144 bit
     */
    if (tcam_info->mode) { /* 350 MHz */
        if (acl_ip6_key == 1 || acl_ip6_key == 2) {
            acl_ip6_key = 3;
        }
    }
    rval = 0;
    if (fwd_ip6_key) {
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_128_ENf, 1);
    }
    if (acl_l2_key) {
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, L2_ACL_ENf, 1);
        if (acl_l2_key == 2) {
            soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, L2_ACL_144_ENf,
                              1);
        }
    }
    switch (acl_ip4_key) {
    case 1: /* ACL_IP4, 288-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV4_ACL_MODEf, 2);
        break;
    case 2: /* ACL_IP4C, 144-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV4_ACL_MODEf, 2);
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV4_ACL_144_ENf,
                          1);
        break;
    case 3: /* ACL_L2IP4, 432-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV4_ACL_MODEf, 3);
        break;
    case 4: /* ACL_L2 or ACL_L2C */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV4_ACL_MODEf, 1);
        break;
    default:
        break;
    }
    switch (acl_ip6_key) {
    case 1: /* ACL_IP6S, 360-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_ACL_MODEf, 2);
        break;
    case 2: /* ACL_IP6F, 432-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_ACL_MODEf, 2);
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_FULL_ACLf, 1);
        break;
    case 3: /* ACL_IP6C, 144-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_ACL_MODEf, 2);
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_ACL_144_ENf,
                          1);
        break;
    case 4: /* ACL_L2IP6, 432-bit */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_ACL_MODEf, 3);
        break;
    case 5: /* ACL_L2 or ACL_L2C */
        soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_ACL_MODEf, 1);
        break;
    default:
        break;
    }

    soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, L2_FWD_ENf, 1);
    soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV4_FWD_MODEf,
                      acl_ip4_key == 4 ? 1 : 2);
    soc_reg_field_set(unit, ESM_MODE_PER_PORTr, &rval, IPV6_FWD_MODEf,
                      acl_ip6_key == 5 ? 1 : 2);

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_ESM_MODE_PER_PORTr(unit, port, rval));
    }

    return SOC_E_NONE;
}

int
soc_triumph_esm_init(int unit)
{
    uint16 dev_id;
    uint8 rev_id;
    soc_tcam_info_t *tcam_info;
    uint32 rval, to_usec, addr;
    uint32 wval = 0;
    int rv, sram_mode, ad_mode;
    int do_tcam_tuning, do_sram_tuning[2], sram_dac_en[2], always_drive_dbus;
    int tx_locked[2], rx_locked[2];
    int intf_num, retry_count;
    et_inst_opc_table_entry_t opc_entry;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }

    always_drive_dbus = soc_feature(unit, soc_feature_always_drive_dbus);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* De-assert external TCAM reset pin */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, EXT_TCAM_RSTf, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    addr = soc_reg_addr(unit, IESMIF_INTR_CLEARr, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &wval));
    soc_reg_field_set(unit, IESMIF_INTR_STATUSr, &wval, BK2BK_ESM_ELIGIBLEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, wval));

    /* 0: 500MHz 6 cycles, 1: 350MHz 4 cycles, default to 500MHz */
    tcam_info->mode = soc_property_get(unit, spn_EXT_TCAM_MODE, 0);

    /* 0: read from device, 3: NL6K, 4: NL7K */
    tcam_info->subtype_override =
        soc_property_get(unit, spn_EXT_TCAM_DEV_TYPE, 0);
    if (tcam_info->subtype_override &&
        (tcam_info->subtype_override < 3 || tcam_info->subtype_override > 4)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "ESM init: unit %d invalid device type %d\n"),
                              unit, tcam_info->subtype_override));
        return SOC_E_PARAM;
    }

    tcam_info->num_tcams_override =
        soc_property_get(unit, spn_EXT_TCAM_BANKS, 0);
    if (tcam_info->num_tcams_override &&
        (tcam_info->num_tcams_override < 1 ||
         tcam_info->num_tcams_override > 2)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "ESM init: unit %d does not support %d TCAM banks\n"),
                   unit, tcam_info->num_tcams_override));
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_select_ad_mode(unit, &ad_mode));

    /*
     * For RX:
     *   Use MIDL: set MIDL_RX_EN and SEL_RX_CLKDLY_BLK to 1
     *   Use VCDL: set MIDL_RX_EN and SEL_RX_CLKDLY_BLK to 0
     * For TX:
     *   Use MIDL: set MIDL_TX_EN and SEL_TX_CLKDLY_BLK to 1
     *   Use VCDL: set MIDL_TX_EN and SEL_TX_CLKDLY_BLK to 0
     */
    SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
    if (SAL_BOOT_QUICKTURN) {
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          VCDL_TX_BYPASSf, 1);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          VCDL_RX_BYPASSf, 1);
    }
    if (soc_property_get(unit, spn_EXT_TCAM_USE_MIDL, 1)) {
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          SEL_TX_CLKDLY_BLKf, 1);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          SEL_RX_CLKDLY_BLKf, 1);
    } else {
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          MIDL_TX_ENf, 0);
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval,
                          MIDL_RX_ENf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
    soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval, ODT_CLK500_If,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG2_ISr(unit, rval));

    
    SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
    soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval, ENABLE_TCAMf,
                      0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval));
    soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG1_ISr, &rval, ENABLE_TCAMf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG1_ISr(unit, rval));

    if (always_drive_dbus) {
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG3_ISr(unit, &rval));
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &rval,
                          ALWAYS_DRIVE_DBUSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG3_ISr(unit, rval));
    }

    SOC_IF_ERROR_RETURN(READ_ETC_CTLr(unit, &rval));
    soc_reg_field_set(unit, ETC_CTLr, &rval, IN_DBUS_CAPT_DLYf, 63);
    soc_reg_field_set(unit, ETC_CTLr, &rval, IN_RDACK11f, 0);
    SOC_IF_ERROR_RETURN(WRITE_ETC_CTLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ESM_CTLr(unit, &rval));
    soc_reg_field_set(unit, ESM_CTLr, &rval, EXT_TCAM_MODEf, tcam_info->mode);
    soc_reg_field_set(unit, ESM_CTLr, &rval, AD_MODEf, ad_mode);
    soc_reg_field_set(unit, ESM_CTLr, &rval, SEND_RSP_WORD_DYNAMICf, 1);
    if (tcam_info->partitions[TCAM_PARTITION_FWD_IP4].num_entries != 0 ||
        tcam_info->partitions[TCAM_PARTITION_FWD_IP6U].num_entries != 0 ||
        tcam_info->partitions[TCAM_PARTITION_FWD_IP6].num_entries != 0) {
        soc_reg_field_set(unit, ESM_CTLr, &rval, WDAT36_RMWf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_ESM_CTLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ETU_LUREQFIFO_RS_CTLr(unit, &rval));
    soc_reg_field_set(unit, ETU_LUREQFIFO_RS_CTLr, &rval, COUNT_HI_THRf, 5);
    soc_reg_field_set(unit, ETU_LUREQFIFO_RS_CTLr, &rval, COUNT_LO_THRf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_LUREQFIFO_RS_CTLr(unit, rval));

    /* sram_mode 0: 1.5 clocks latency, 1: 2 clocks latency */
    sram_mode = soc_property_get(unit, spn_EXT_SRAM_MODE, 1);
    SOC_IF_ERROR_RETURN(READ_ES0_DDR36_CONFIG_REG1_ISr(unit, &rval));
    soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG1_ISr, &rval, SRAM_MODEf,
                      sram_mode);
    SOC_IF_ERROR_RETURN(WRITE_ES0_DDR36_CONFIG_REG1_ISr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_ES1_DDR36_CONFIG_REG1_ISr(unit, &rval));
    soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG1_ISr, &rval, SRAM_MODEf,
                      sram_mode);
    SOC_IF_ERROR_RETURN(WRITE_ES1_DDR36_CONFIG_REG1_ISr(unit, rval));

    /* Program SRAM tuning result from config variables */
    for (intf_num = 0; intf_num < 2; intf_num++) {
        rv = _soc_triumph_esm_init_set_sram_tuning_result(unit, intf_num);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        do_sram_tuning[intf_num] =
            SAL_BOOT_BCMSIM || rv == SOC_E_NOT_FOUND ? FALSE : TRUE;
    }

    /* Program TCAM tuning result from config variables */
    rv = _soc_triumph_esm_init_set_tcam_tuning_result(unit);
    if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
        return rv;
    }
    do_tcam_tuning = SAL_BOOT_BCMSIM || rv == SOC_E_NOT_FOUND ? FALSE : TRUE;

    if (SOC_WARM_BOOT(unit)) {
        do_tcam_tuning = FALSE;
    }

    if (SOC_IS_TRIUMPH2(unit)) {
    /* Program SRAM DAC value from config variable */
    for (intf_num = 0; intf_num < 2; intf_num++) {
        rv = _soc_triumph_esm_init_set_sram_dac(unit, intf_num);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        sram_dac_en[intf_num] = rv == SOC_E_NOT_FOUND ? FALSE : TRUE;
    }

    /* Program TCAM DAC value from config variable */
    (void)_soc_triumph_esm_init_set_tcam_dac(unit);

    /* Bring SRAM and TCAM out of reset */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_DDR0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_DDR1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_TCAM_RST_Lf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));

    sal_usleep(25); /* reset SRAM DLL for 25us */

    SOC_IF_ERROR_RETURN(READ_ES0_DDR36_CONFIG_REG1_ISr(unit, &rval));
    soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG1_ISr, &rval, ENABLE_DDRf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ES0_DDR36_CONFIG_REG1_ISr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_ES1_DDR36_CONFIG_REG1_ISr(unit, &rval));
    soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG1_ISr, &rval, ENABLE_DDRf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ES1_DDR36_CONFIG_REG1_ISr(unit, rval));

    sal_usleep(100); /* wait for SRAM DLLs to lock for 100us */

    /* Check SRAM TX status */
    for (intf_num = 0; intf_num < 2; intf_num++) {
        rv = _soc_triumph_esm_init_check_sram_tx_status
            (unit, intf_num, sram_dac_en[intf_num]);
        if (rv == SOC_E_NOT_FOUND) {
            LOG_CLI((BSL_META_U(unit,
                                "SRAM%d TX DLL not locked\n"), intf_num));
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }
    }

    /* Set ES0 PVT */
    rv = _soc_triumph_esm_init_set_pvt(unit, 0);
    if (rv == SOC_E_NOT_FOUND) {
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 0));
        /* Set ES0 IO drive strength and slew rate strong */
        if (SOC_IS_TRIUMPH(unit) && rev_id < BCM56624_B2_REV_ID) {
            SOC_IF_ERROR_RETURN(READ_ES0_DDR36_CONFIG_REG2_ISr(unit, &rval));
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_PDRIVE_VALf, 15);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_NDRIVE_VALf, 0);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_DRIVER_PVTf, 1);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_SLEW_VALf, 15);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_SLEW_PVTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES0_DDR36_CONFIG_REG2_ISr(unit, rval));
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Set ES1 PVT */
    rv = _soc_triumph_esm_init_set_pvt(unit, 1);
    if (rv == SOC_E_NOT_FOUND) {
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 1));
        /* Set ES1 IO drive strength and slew rate strong */
        if (SOC_IS_TRIUMPH(unit) && rev_id < BCM56624_B2_REV_ID) {
            SOC_IF_ERROR_RETURN(READ_ES1_DDR36_CONFIG_REG2_ISr(unit, &rval));
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_PDRIVE_VALf, 15);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_NDRIVE_VALf, 0);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_DRIVER_PVTf, 1);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_SLEW_VALf, 15);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_SLEW_PVTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES1_DDR36_CONFIG_REG2_ISr(unit, rval));
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Set TCAM PVT */
    rv = _soc_triumph_esm_init_set_pvt(unit, 2);
    if (rv == SOC_E_NOT_FOUND) {
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 2));
        /* Set TCAM IO drive strength and slew rate weak */
        if ((SOC_IS_TRIUMPH(unit) && rev_id < BCM56624_B2_REV_ID)) {
            SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              PVT_PDRIVE_VALf, 0);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              PVT_NDRIVE_VALf, 15);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_DRIVER_PVTf, 1);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              PVT_SLEW_VALf, 1);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_SLEW_PVTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG2_ISr(unit, rval));
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    tx_locked[0] = tx_locked[1] = FALSE;
    rx_locked[0] = rx_locked[1] = FALSE;
    for (retry_count = 0; retry_count < 11; retry_count++) {
        /* Toggle STOP_DK on SRAM interfaces */
        for (intf_num = 0; intf_num < 2; intf_num++) {
            if (tx_locked[intf_num] && rx_locked[intf_num]) {
                continue;
            }
            SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_stop_dk(unit, intf_num));

            SOC_IF_ERROR_RETURN
                (_soc_triumph_esm_init_relock_dll(unit, intf_num,
                                                  retry_count != 0 ? TRUE : FALSE));

            /* Check SRAM TX status */
            rv = _soc_triumph_esm_init_check_sram_tx_status
                (unit, intf_num, sram_dac_en[intf_num]);
            if (SOC_SUCCESS(rv)) {
                tx_locked[intf_num] = TRUE;
            } else if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
                return rv;
            }

            /* Check SRAM RX status */
            rv = _soc_triumph_esm_init_check_sram_rx_status(unit, intf_num);
            if (SOC_SUCCESS(rv)) {
                rx_locked[intf_num] = TRUE;
            } else if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
                return rv;
            }
        }
    }

    for (intf_num = 0; intf_num < 2; intf_num++) {
        if (!tx_locked[intf_num]) {
            LOG_CLI((BSL_META_U(unit,
                                "SRAM%d TX DLL not locked\n"), intf_num));
        }
        if (!rx_locked[intf_num]) {
            LOG_CLI((BSL_META_U(unit,
                                "SRAM%d RX DLL not locked\n"), intf_num));
        }
    }
    } else { /* Triumph */
    /* Bring SRAM and TCAM out of reset */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_DDR0_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_DDR1_RST_Lf, 1);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, CMIC_TCAM_RST_Lf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));

    /* Set ES0 PVT */
    rv = _soc_triumph_esm_init_set_pvt(unit, 0);
    if (rv == SOC_E_NOT_FOUND) {
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 0));
        /* Set ES0 IO drive strength and slew rate strong */
        if (SOC_IS_TRIUMPH(unit) && rev_id < BCM56624_B2_REV_ID) {
            SOC_IF_ERROR_RETURN(READ_ES0_DDR36_CONFIG_REG2_ISr(unit, &rval));
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_PDRIVE_VALf, 15);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_NDRIVE_VALf, 0);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_DRIVER_PVTf, 1);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_SLEW_VALf, 15);
            soc_reg_field_set(unit, ES0_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_SLEW_PVTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES0_DDR36_CONFIG_REG2_ISr(unit, rval));
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Set ES1 PVT */
    rv = _soc_triumph_esm_init_set_pvt(unit, 1);
    if (rv == SOC_E_NOT_FOUND) {
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 1));
        /* Set ES1 IO drive strength and slew rate strong */
        if (SOC_IS_TRIUMPH(unit) && rev_id < BCM56624_B2_REV_ID) {
            SOC_IF_ERROR_RETURN(READ_ES1_DDR36_CONFIG_REG2_ISr(unit, &rval));
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_PDRIVE_VALf, 15);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_NDRIVE_VALf, 0);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_DRIVER_PVTf, 1);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              PVT_SLEW_VALf, 15);
            soc_reg_field_set(unit, ES1_DDR36_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_SLEW_PVTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES1_DDR36_CONFIG_REG2_ISr(unit, rval));
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Set TCAM PVT */
    rv = _soc_triumph_esm_init_set_pvt(unit, 2);
    if (rv == SOC_E_NOT_FOUND) {
        SOC_IF_ERROR_RETURN(soc_triumph_esm_init_pvt_comp(unit, 2));
        /* Set TCAM IO drive strength and slew rate weak */
        if ((SOC_IS_TRIUMPH(unit) && rev_id < BCM56624_B2_REV_ID)) {
            SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              PVT_PDRIVE_VALf, 0);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              PVT_NDRIVE_VALf, 15);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_DRIVER_PVTf, 1);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              PVT_SLEW_VALf, 1);
            soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG2_ISr, &rval,
                              OVRD_EN_SLEW_PVTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG2_ISr(unit, rval));
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Toggle STOP_DK on SRAM interfaces */
    SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_stop_dk_old(unit));

    for (intf_num = 0; intf_num < 2; intf_num++) {
        /* Relock DLL to stabilize interfaces */
        SOC_IF_ERROR_RETURN
            (_soc_triumph_esm_init_relock_dll_old(unit, intf_num));
        (void)_soc_triumph_esm_init_set_sram_dac(unit, intf_num);
    }

    /* Program TCAM DAC value from config variable */
    (void)_soc_triumph_esm_init_set_tcam_dac(unit);
    } /* Triumph */

    /* Perform mini-tuning to find EM_LATENCY for SRAM interface */
    for (intf_num = 0; intf_num < 2; intf_num++) {
        if (do_sram_tuning[intf_num]) {
            SOC_IF_ERROR_RETURN
                (_soc_triumph_esm_init_retune_sram(unit, intf_num));
        }
    }

    /* Enable memory controller unit */
    if (ad_mode != 2 && ad_mode != 4 && ad_mode != 6 && ad_mode != 8) {
        SOC_IF_ERROR_RETURN(READ_ES0_MCU_ENr(unit, &rval));
        soc_reg_field_set(unit, ES0_MCU_ENr, &rval, MCU_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ES0_MCU_ENr(unit, rval));
    }
    if (ad_mode != 1 && ad_mode != 3 && ad_mode != 5 && ad_mode != 7) {
        SOC_IF_ERROR_RETURN(READ_ES1_MCU_ENr(unit, &rval));
        soc_reg_field_set(unit, ES1_MCU_ENr, &rval, MCU_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ES1_MCU_ENr(unit, rval));
    }

    /* Perform mini-tuning to stabilize TCAM interface */
    if (do_tcam_tuning) {
        SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_retune_tcam(unit));
    } else {
        SOC_IF_ERROR_RETURN(soc_tcam_init(unit));
    }

    /* Program tcam index to sram index translation table */
    SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_set_et_pa_xlate(unit));

    /* Program software memory state according to table size in user config */
    SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_adjust_mem_size(unit));

    if (always_drive_dbus && soc_mem_index_count(unit, EXT_L2_ENTRY_TCAMm)) {
        SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, EXT_L2_ENTRY_TCAMm,
                                              MEM_BLOCK_ALL, TRUE));
    }

    /* Clear all tables to ensure sram tables have valid CRC/ECC */
    SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_clear_all_entries(unit));

    SOC_IF_ERROR_RETURN(READ_ETU_L2SEARCH72_INSTr(unit, &rval));
    soc_reg_field_set(unit, ETU_L2SEARCH72_INSTr, &rval, OUT_DBUS_CTLf, 0xa0);
    SOC_IF_ERROR_RETURN(WRITE_ETU_L2SEARCH72_INSTr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ESM_L2_AGE_CTLr(unit, &rval));
    soc_reg_field_set(unit, ESM_L2_AGE_CTLr, &rval, EN_RD_TCAMf,
                      always_drive_dbus ? 0 : 1);
    soc_reg_field_set(unit, ESM_L2_AGE_CTLr, &rval, CPU_NOTIFYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ESM_L2_AGE_CTLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_ESM_PER_PORT_REPL_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, EN_RD_TCAMf,
                      always_drive_dbus ? 0 : 1);
    soc_reg_field_set(unit, ESM_PER_PORT_REPL_CONTROLr, &rval, CPU_NOTIFYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ESM_PER_PORT_REPL_CONTROLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, AUX_ARB_CONTROL_2r, &rval, ESM_AGE_ENABLEf, 1);
    soc_reg_field_set(unit, AUX_ARB_CONTROL_2r, &rval, ESM_BP_ENABLEf, 0x0c);
    SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROL_2r(unit, rval));

    if (tcam_info->mode && tcam_info->subtype == 4) { /* 350MHz on 7K */
        SOC_IF_ERROR_RETURN(READ_IESMIF_CONTROL2r(unit, &rval));
        soc_reg_field_set(unit, IESMIF_CONTROL2r, &rval, NL7K_350_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IESMIF_CONTROL2r(unit, rval));

        /* Replace opcode table index 34 from "write to K2 opcode" to
         * "compare with LTR 23 opcode (single L2 search with K2)"
         * to avoid missing L2 SA lookup for IPv6 L2_ACL + L2_FWD */
        sal_memset(&opc_entry, 0, sizeof(opc_entry));
        soc_mem_field32_set(unit, ET_INST_OPC_TABLEm, &opc_entry, OPCODEf,
                            0xfa);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, ET_INST_OPC_TABLEm,
                                          MEM_BLOCK_ANY, 34, &opc_entry));
    }

    SOC_IF_ERROR_RETURN(READ_ESM_KEYGEN_CTLr(unit, &rval));
    if (tcam_info->subtype == 3) { /* NL6K */
        soc_reg_field_set(unit, ESM_KEYGEN_CTLr, &rval, TCAM_TYPEf,
                          tcam_info->mode ? 0 : 1);
    } else if (tcam_info->subtype == 4) { /* NL7K */
        soc_reg_field_set(unit, ESM_KEYGEN_CTLr, &rval, TCAM_TYPEf,
                          tcam_info->mode ? 3 : 2);
    }
    if (tcam_info->partitions[TCAM_PARTITION_FWD_IP4].num_entries) {
        soc_reg_field_set(unit, ESM_KEYGEN_CTLr, &rval, IPV4_HIT_BIT_MODEf, 2);
    }
    if (tcam_info->partitions[TCAM_PARTITION_FWD_IP6U].num_entries ||
        tcam_info->partitions[TCAM_PARTITION_FWD_IP6].num_entries) {
        soc_reg_field_set(unit, ESM_KEYGEN_CTLr, &rval, IPV6_HIT_BIT_MODEf, 2);
    }
    if (tcam_info->partitions[TCAM_PARTITION_FWD_L2].num_entries) {
        soc_reg_field_set(unit, ESM_KEYGEN_CTLr, &rval, EXT_L2_FWD_ENf, 1);
        soc_reg_field_set(unit, ESM_KEYGEN_CTLr, &rval, L2_HIT_BIT_MODEf, 1);
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_esm_l2_lookup_for_ip6)) {
            SOC_IF_ERROR_RETURN(READ_IESMIF_CONTROL2r(unit, &wval));
            soc_reg_field_set(unit, IESMIF_CONTROL2r, &wval, V6_EN_L2FWD_4NONROUTABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_IESMIF_CONTROL2r(unit, wval));
        }
#endif
    }
    SOC_IF_ERROR_RETURN(WRITE_ESM_KEYGEN_CTLr(unit, rval));

    SOC_IF_ERROR_RETURN(_soc_triumph_esm_init_set_esm_mode_per_port(unit));

    /* Enable ESM for XGPORT blocks + CPU + SPORT */
    if (SOC_IS_TRIUMPH(unit)) {
        SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, ESM_ENABLEf, 0x3ff);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_PKTBUF_ESM_OFFSETr(unit, &rval));
        /* set to (4/7) * 92 = 56 for 4 non-ESM eligible slots in tdm cycle */
        soc_reg_field_set(unit, PKTBUF_ESM_OFFSETr, &rval, THRESHOLDf, 56);
        SOC_IF_ERROR_RETURN(WRITE_PKTBUF_ESM_OFFSETr(unit, rval));
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit)) {
        int tdm_count, non_esm_count, i;
        iarb_tdm_table_entry_t entry;

        SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
        tdm_count = soc_reg_field_get(unit, IARB_TDM_CONTROLr, rval,
                                      TDM_WRAP_PTRf);
        non_esm_count = tdm_count;
        for (i = 0; i <= tdm_count; i++) {
            SOC_IF_ERROR_RETURN(READ_IARB_TDM_TABLEm(unit, MEM_BLOCK_ANY, i,
                                                     &entry));
            non_esm_count -= soc_mem_field32_get(unit, IARB_TDM_TABLEm, &entry,
                                                 ESM_ELIGIBLEf);
        }

        SOC_IF_ERROR_RETURN(READ_PKTBUF_ESM_OFFSETr(unit, &rval));
        /* set threshold to roundup(non_esm_count / total_count) * 92 */
        if (tdm_count != 0) {
            soc_reg_field_set(unit, PKTBUF_ESM_OFFSETr, &rval, THRESHOLDf,
                          (92 * non_esm_count + tdm_count - 1) / tdm_count);
        } else {
            soc_reg_field_set(unit, PKTBUF_ESM_OFFSETr, &rval, THRESHOLDf,
                                tdm_count);
        }
        SOC_IF_ERROR_RETURN(WRITE_PKTBUF_ESM_OFFSETr(unit, rval));
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    addr = soc_reg_addr(unit, IESMIF_INTR_ENABLEr, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &wval));
    soc_reg_field_set(unit, IESMIF_INTR_STATUSr, &wval, BK2BK_ESM_ELIGIBLEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, wval));

    SOC_IF_ERROR_RETURN(READ_IESMIF_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IESMIF_CONTROLr, &rval, EN_EXT_SEARCH_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IESMIF_CONTROLr(unit, rval));

    return SOC_E_NONE;
}

#define TR_MMU_NUM_PG   8
#define TR_MMU_NUM_COS  8

/* Standard Ethernet MTU, 1536 bytes (1 cell = 128 bytes) */
#define TR_MMU_ETH_FRAME_CELLS  12

/* Jumbo Frame MTU, 9216 (1 cell = 128 bytes) */
#define TR_MMU_JUMBO_FRAME_CELLS  72

/* MAX Frame MTU, 16384 (1 cell = 128 bytes) */
#define TR_MMU_MAX_FRAME_CELLS  128

#define TR_MMU_IN_PORT_MIN_CELLS       72
#define TR_MMU_IN_PORT_MIN_PKTS        1
#define TR_MMU_PG_HDRM_LIMIT_CELLS     36
#define TR_MMU_PG_HDRM_LIMIT_PKTS      36
#define TR_MMU_GLOBAL_HDRM_LIMIT_CELLS 636
#define TR_MMU_PG_RESET_OFFSET_CELLS   24
#define TR_MMU_PG_RESET_OFFSET_PKTS    1

#define TR_MMU_OUT_PORT_MIN_CELLS      12
#define TR_MMU_OUT_PORT_MIN_PKTS       1
#define TR_MMU_OUT_RESET_OFFSET_CELLS  24
#define TR_MMU_OUT_RESET_OFFSET_PKTS   2
#define TR_MMU_SOP_POLICY              0
#define TR_MMU_MOP_POLICY              7

int
soc_triumph_mmu_init(int unit)
{
    uint16              dev_id;
    uint8               rev_id;
    uint64              rval64;
    uint32              rval0, rval1, cell_rval, pkt_rval;
    int                 rv = SOC_E_NONE;
    int                 port;
    int                 total_cells, total_pkts;
    int                 in_reserved_cells, in_reserved_pkts;
    int                 out_reserved_cells, out_reserved_pkts;
    int                 out_shared_cells, out_shared_pkts;
    int                 idx, count;
    soc_pbmp_t          pbmp_8pg;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Total number of cells */
    if (SOC_IS_VALKYRIE(unit) && dev_id != BCM56620_DEVICE_ID) {
        total_cells = 24 * 1024; /* 24K cells */
    } else {
        total_cells = 32 * 1024; /* 32K cells */
    }

    /* Total number of packet pointers */
    total_pkts = 11 * 1024; /* 11K packet pointers */

    /* Ports with 8PG (1PG for other ports) */
    SOC_PBMP_CLEAR(pbmp_8pg);
    SOC_PBMP_WORD_SET(pbmp_8pg, 0, 0xfc004004); /* 2, 14, 26-31 */
    SOC_PBMP_AND(pbmp_8pg, PBMP_ALL(unit));

    /*
     * Reserved space calculation:
     *   Input port:
     *     per-port minimum
     *     per-PG minimum (config to 0)
     *     per-PG headroom
     *     per-device headroom
     *     per-port minimum for SC and QM traffic (config to 0)
     *   Output port:
     *     per-port per-COS minimum space
     * Shared space calculation:
     *   Input port: total - input port reserved - output port reserved
     *   Output port: total - output port reserved
     */
    in_reserved_cells = NUM_ALL_PORT(unit) * TR_MMU_IN_PORT_MIN_CELLS +
        NUM_PORT(unit) * TR_MMU_PG_HDRM_LIMIT_CELLS +
        TR_MMU_GLOBAL_HDRM_LIMIT_CELLS;
    in_reserved_pkts = NUM_ALL_PORT(unit) * TR_MMU_IN_PORT_MIN_PKTS +
        NUM_PORT(unit) * TR_MMU_PG_HDRM_LIMIT_PKTS;
    out_reserved_cells =
        (NUM_PORT(unit) * TR_MMU_NUM_COS + SOC_INFO(unit).num_cpu_cosq) *
        TR_MMU_OUT_PORT_MIN_CELLS;
    out_reserved_pkts =
        (NUM_PORT(unit) * TR_MMU_NUM_COS + SOC_INFO(unit).num_cpu_cosq) *
        TR_MMU_OUT_PORT_MIN_PKTS;

    /*
     * Input ports threshold
     */
    /* Reserved space: Input port per-port minimum */
    cell_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &cell_rval, PORT_MINf,
                      TR_MMU_IN_PORT_MIN_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval, PORT_MINf,
                      TR_MMU_IN_PORT_MIN_PKTS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, port, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PACKETr(unit, port, pkt_rval));
    }

    /* Reserved space: Input port per-PG minimum
     * Use defalt value 0
     * With only one PG in use PORT_MIN should be sufficient */

    /* Reserved space: Input port per-PG headroom
     * Use only 1PG (highest priority PG for the port) */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &cell_rval, PG_HDRM_LIMITf,
                      TR_MMU_PG_HDRM_LIMIT_CELLS);
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &cell_rval, PG_GEf, 1);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMIT_PACKETr, &pkt_rval, PG_HDRM_LIMITf,
                      TR_MMU_PG_HDRM_LIMIT_PKTS);
    PBMP_PORT_ITER(unit, port) {
        idx = SOC_PBMP_MEMBER(pbmp_8pg, port) ? TR_MMU_NUM_PG - 1 : 0;
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, port, idx,
                                                      cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_PACKETr(unit, port, idx,
                                                        pkt_rval));
    }

    /* Reserved space: Input port per-device headroom */
    cell_rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &cell_rval, GLOBAL_HDRM_LIMITf,
                      TR_MMU_GLOBAL_HDRM_LIMIT_CELLS);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, cell_rval));

    /* Input port shared space */
    cell_rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_CELLr, &cell_rval,
                      TOTAL_SHARED_LIMITf,
                      total_cells - in_reserved_cells - out_reserved_cells);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMIT_PACKETr, &pkt_rval,
                      TOTAL_SHARED_LIMITf,
                      total_pkts - in_reserved_pkts - out_reserved_pkts);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMIT_PACKETr(unit, pkt_rval));

    /* Input port per-port shared space limit - no limit */
    cell_rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_CELLr, &cell_rval,
                      PORT_SHARED_LIMITf, total_cells - 1);
    pkt_rval = 0;
    soc_reg_field_set(unit, PORT_SHARED_LIMIT_PACKETr, &pkt_rval,
                      PORT_SHARED_LIMITf, total_pkts - 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_CELLr(unit, port,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMIT_PACKETr(unit, port,
                                                            pkt_rval));
    }

    /* Input port per-PG reset offset
     * Use only 1PG (highest priority PG for the port)
     * Use default value 0 for CPU */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &cell_rval,
                      PG_RESET_OFFSETf, TR_MMU_PG_RESET_OFFSET_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_RESET_OFFSET_PACKETr, &pkt_rval,
                      PG_RESET_OFFSETf, TR_MMU_PG_RESET_OFFSET_PKTS);
    PBMP_PORT_ITER(unit, port) {
        idx = SOC_PBMP_MEMBER(pbmp_8pg, port) ? TR_MMU_NUM_PG - 1 : 0;
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, port, idx,
                                                        cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_PACKETr(unit, port, idx,
                                                          pkt_rval));
    }

    /* Input port per-PG reset floor (cell). Use default value 0 */

    /* Reserved space: Input port per-port minimum for SC and QM traffic
     * Use default value 0 */

    /* Input port priority XON disable */

    /* Input port max packet size in cells */
    rval0 = 0;
    soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval0, PORT_MAX_PKT_SIZEf,
                      TR_MMU_JUMBO_FRAME_CELLS);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval0));
    }

    /* Input port per-PG threshold */
    rval0 = 0;
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG0_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG1_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG2_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG3_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG4_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG5_THRESH_SELf, 0x8);
    soc_reg_field_set(unit, PG_THRESH_SELr, &rval0, PG6_THRESH_SELf, 0x8);
    PBMP_ITER(pbmp_8pg, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval0));
    }

    idx = TR_MMU_NUM_PG - 1;
    rval0 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI0_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI1_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI2_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI3_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI4_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI5_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval0, PRI6_GRPf, idx);
    rval1 = 0;
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI7_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI8_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI9_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI10_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI11_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI12_GRPf, idx);
    soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval1, PRI13_GRPf, idx);
    PBMP_ITER(pbmp_8pg, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval0));
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval1));
    }

    /* Input port pause enable */
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, PORT_PAUSE_ENABLE_64r, &rval64,
                          PORT_PAUSE_ENABLE_LOf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    soc_reg64_field32_set(unit, PORT_PAUSE_ENABLE_64r, &rval64,
                          PORT_PAUSE_ENABLE_HIf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 1));
    SOC_IF_ERROR_RETURN(WRITE_PORT_PAUSE_ENABLE_64r(unit, rval64));

    /*
     * Output ports threshold
     */
    /* Reserved space: Output port per-port per-COS minimum space */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval, Q_MIN_CELLf,
                      TR_MMU_OUT_PORT_MIN_CELLS);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_LIMIT_ENABLE_CELLf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_LIMIT_DYNAMIC_CELLf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &cell_rval,
                      Q_SHARED_LIMIT_CELLf, 2); /* alpha index 2 */
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, Q_MIN_PACKETf,
                      TR_MMU_OUT_PORT_MIN_PKTS);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_LIMIT_ENABLE_PACKETf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_LIMIT_DYNAMIC_PACKETf, 0x1);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval,
                      Q_SHARED_LIMIT_PACKETf, 2); /* alpha index 2 */
    PBMP_ALL_ITER(unit, port) {
        count = IS_CPU_PORT(unit, port) ?
            SOC_INFO(unit).num_cpu_cosq : TR_MMU_NUM_COS;
        for (idx = 0; idx < count; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx,
                                                            cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_PACKETr(unit, port, idx,
                                                              pkt_rval));
        }
    }

    /* Output port per-port per-COS reset offset */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_CELLr, &cell_rval,
                      Q_RESET_OFFSET_CELLf, TR_MMU_OUT_RESET_OFFSET_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_PACKETr, &pkt_rval,
                      Q_RESET_OFFSET_PACKETf, TR_MMU_OUT_RESET_OFFSET_PKTS);
    PBMP_ALL_ITER(unit, port) {
        count = IS_CPU_PORT(unit, port) ?
            SOC_INFO(unit).num_cpu_cosq : TR_MMU_NUM_COS;
        for (idx = 0; idx < count; idx++) {
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_CELLr(unit, port,
                                                                  idx,
                                                                  cell_rval));
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_PACKETr(unit, port,
                                                                    idx,
                                                                    pkt_rval));
        }
    }

    out_shared_cells = total_cells - out_reserved_cells;
    out_shared_pkts = total_pkts - out_reserved_pkts;

    /* Output port per-device shared */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLr, &cell_rval,
                      OP_BUFFER_SHARED_LIMIT_CELLf, out_shared_cells);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_PACKETr, &pkt_rval,
                      OP_BUFFER_SHARED_LIMIT_PACKETf, out_shared_pkts);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_PACKETr(unit, pkt_rval));

    /* Output port per-device shared for YELLOW traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLr, &cell_rval,
                      OP_BUFFER_LIMIT_YELLOW_CELLf, out_shared_cells >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_PACKETr, &pkt_rval,
                      OP_BUFFER_LIMIT_YELLOW_PACKETf, out_shared_pkts >> 2);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_PACKETr(unit, pkt_rval));

    /* Output port per-device shared for RED traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLr, &cell_rval,
                      OP_BUFFER_LIMIT_RED_CELLf, out_shared_cells >> 3);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_PACKETr, &pkt_rval,
                      OP_BUFFER_LIMIT_RED_PACKETf, out_shared_pkts >> 2);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_PACKETr(unit, pkt_rval));

    /*
     * Output port per-port shared
     * Limit check disabled, however still keep code as reference
     * OP_SHARED_LIMIT set to 3/4 of total shared space
     * OP_SHARED_RESET_VALUE set to 1/2 of total shared space
     */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_LIMIT_CELLf, out_shared_cells * 3 / 4);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      OP_SHARED_RESET_VALUE_CELLf, out_shared_cells / 2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &cell_rval,
                      PORT_LIMIT_ENABLE_CELLf, 0);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_LIMIT_PACKETf, out_shared_pkts * 3 / 4);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      OP_SHARED_RESET_VALUE_PACKETf, out_shared_pkts / 2);
    soc_reg_field_set(unit, OP_PORT_CONFIG_PACKETr, &pkt_rval,
                      PORT_LIMIT_ENABLE_PACKETf, 0);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_CELLr(unit, port, cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIG_PACKETr(unit, port,
                                                         pkt_rval));
    }

    /* Output port per-port shared for YELLOW traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_CELLr, &cell_rval,
                      OP_PORT_LIMIT_YELLOW_CELLf,
                      (out_shared_cells * 3 / 4) >> 3);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOW_PACKETr, &pkt_rval,
                      OP_PORT_LIMIT_YELLOW_PACKETf,
                      (out_shared_pkts * 3 / 4) >> 2);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_CELLr(unit, port,
                                                             cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_YELLOW_PACKETr(unit, port,
                                                               pkt_rval));
    }

    /* Output port per-port shared for RED traffic */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_RED_CELLr, &cell_rval,
                      OP_PORT_LIMIT_RED_CELLf,
                      (out_shared_cells * 3 / 4) >> 3);
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_PORT_LIMIT_RED_PACKETr, &pkt_rval,
                      OP_PORT_LIMIT_RED_PACKETf,
                      (out_shared_pkts * 3 / 4) >> 2);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_CELLr(unit, port,
                                                          cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_LIMIT_RED_PACKETr(unit, port,
                                                            pkt_rval));
    }

    /* Output port configuration */
    rval0 = 0;
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval0,
                      MOP_POLICYf, TR_MMU_MOP_POLICY);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval0,
                      SOP_POLICYf, TR_MMU_SOP_POLICY);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval0));

    /* Input port enable */
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE_64r, &rval64,
                          INPUT_PORT_RX_ENABLE_LOf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE_64r, &rval64,
                          INPUT_PORT_RX_ENABLE_HIf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 1));
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE_64r(unit, rval64));
    SOC_IF_ERROR_RETURN(WRITE_OUTPUT_PORT_RX_ENABLE_64r(unit, rval64));

    /* init ESM */
    SOC_IF_ERROR_RETURN(READ_IESMIF_CONTROLr(unit, &rval0));
    soc_reg_field_set(unit, IESMIF_CONTROLr, &rval0, EN_EXT_SEARCH_REQf, 0);
    SOC_IF_ERROR_RETURN(WRITE_IESMIF_CONTROLr(unit, rval0));
    if (soc_feature(unit, soc_feature_esm_support)) {
        /*
         * FP_LOCK is taken to synchronize Counter thread counter collection
         * where counter thread collects counters in external memory and
         * soc_mmu_init() which reintializes counters in external mmeory too.
         */
        if (NULL != SOC_CONTROL(unit)->fp_lock) {
            sal_mutex_take(SOC_CONTROL(unit)->fp_lock, sal_mutex_FOREVER);
        }
        /* coverity[stack_use_callee] */
        /* coverity[stack_use_overflow] */
        rv = soc_triumph_esm_init(unit);

        if (NULL != SOC_CONTROL(unit)->fp_lock) {
            sal_mutex_give(SOC_CONTROL(unit)->fp_lock);
        }

        SOC_IF_ERROR_RETURN(rv);
    }

    /* init dual modid */
    if ((NUM_MODID(unit) == 2) && 
        (SOC_REG_IS_VALID(unit, E2EFC_CONFIGr)) ) {
        SOC_IF_ERROR_RETURN(READ_E2EFC_CONFIGr(unit, &rval0));
        soc_reg_field_set(unit, E2EFC_CONFIGr, &rval0, DUAL_MODID_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_E2EFC_CONFIGr(unit, rval0));
    }

    return SOC_E_NONE;
}

int
soc_triumph_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_triumph_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_triumph_ext_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 rval;
    int num_entries;

    num_entries = soc_mem_index_count(unit, EXT_L2_ENTRYm);
    if (num_entries > 0) {
        /* Disable refresh pulse to ESM */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, AUX_ARB_CONTROL_2r, REG_PORT_ANY,
                                    ESM_AGE_ENABLEf, 0));

        /*
         * ESM background process marches through all entries at
         * rate of ESM_AGE_CNT.COUNT * 8 ns. The background process
         * clears the hit bit of the entry at the first visit and ages
         * out entry on second visit.
         * age time = 2 * count * 8 ns * number of ext_l2_entry entries */
        rval = 0;
        soc_reg_field_set(unit, ESM_AGE_CNTr, &rval, COUNTf,
                          age_seconds * (1000000000 / 8 /
                                         num_entries));
        SOC_IF_ERROR_RETURN(WRITE_ESM_AGE_CNTr(unit, rval));

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESM_L2_AGE_CTLr, REG_PORT_ANY,
                                    HW_AGE_MODEf, enable ? 2 : 0));

        /* Re-enable refresh pulse to ESM */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, AUX_ARB_CONTROL_2r, REG_PORT_ANY,
                                    ESM_AGE_ENABLEf, 1));
    }

    return SOC_E_NONE;
}

int
soc_triumph_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 rval;

    rval = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_ENAf, enable ? 1 : 0);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, rval));

    if (soc_feature(unit, soc_feature_esm_support)) {
        soc_triumph_ext_age_timer_set(unit, age_seconds, enable);
    }

    return SOC_E_NONE;
}

int
soc_triumph_tcam_access(int unit, int op_type, int num_inst, int num_pre_nop,
                        int num_post_nop, uint32 *dbus, int *ibus)
{
    uint32 cfg, rval0, rval1, rval2, status;
    int rv, i, retry, num_pad_entries;
    et_uinst_mem_entry_t uinst_entry;
    uint32 always_drive_dbus;

    SOC_IF_ERROR_RETURN(WRITE_ETU_ET_INST_REQr(unit, 0));

    always_drive_dbus = FALSE;
    num_pad_entries = 0;
    if (soc_feature(unit, soc_feature_always_drive_dbus) &&
        op_type == TCAM_TR_OP_READ) {
        always_drive_dbus = TRUE;

        /* Add 4 extra NOP in front of any read as workaround */
        num_pad_entries = 2;
        sal_memset(&uinst_entry, 0, sizeof(uinst_entry));
        soc_mem_field32_set(unit, ET_UINST_MEMm, &uinst_entry, DOE0f, 1);
        soc_mem_field32_set(unit, ET_UINST_MEMm, &uinst_entry, DOE1f, 1);
        for (i = 0; i < num_pad_entries; i++) {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, ET_UINST_MEMm,
                                              MEM_BLOCK_ANY, i, &uinst_entry));
        }
    }

    for (i = 0; i < num_inst;) {
        sal_memset(&uinst_entry, 0, sizeof(uinst_entry));

        if (i + 1 < num_inst) {
            if (ibus[i] <= 0xff) {
                soc_mem_field_set(unit, ET_UINST_MEMm, (uint32 *)&uinst_entry,
                                  DBUS0f, &dbus[i * TCAM_TR_WORDS_PER_ENTRY]);
                soc_mem_field32_set(unit, ET_UINST_MEMm, &uinst_entry, DOE0f,
                                    1);
                soc_mem_field32_set(unit, ET_UINST_MEMm, &uinst_entry, IBUS0f,
                                    ibus[i]);
                if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_VERBOSE)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "IBUS0: %08x DBUS0: %08x %08x %08x\n"),
                             ibus[i],
                             dbus[i * TCAM_TR_WORDS_PER_ENTRY],
                             dbus[i * TCAM_TR_WORDS_PER_ENTRY + 1],
                             dbus[i * TCAM_TR_WORDS_PER_ENTRY + 2]));
                }
            }
            i++;
        }

        if (ibus[i] <= 0xff) {
            soc_mem_field_set(unit, ET_UINST_MEMm, (uint32 *)&uinst_entry,
                              DBUS1f, &dbus[i * TCAM_TR_WORDS_PER_ENTRY]);
            soc_mem_field32_set(unit, ET_UINST_MEMm, &uinst_entry, DOE1f, 1);
            soc_mem_field32_set(unit, ET_UINST_MEMm, &uinst_entry, IBUS1f,
                                ibus[i]);
            if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_VERBOSE)) {
                LOG_CLI((BSL_META_U(unit,
                                    "IBUS1: %08x DBUS1: %08x %08x %08x\n"),
                         ibus[i],
                         dbus[i * TCAM_TR_WORDS_PER_ENTRY],
                         dbus[i * TCAM_TR_WORDS_PER_ENTRY + 1],
                         dbus[i * TCAM_TR_WORDS_PER_ENTRY + 2]));
            }
        }
        i++;

        if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_VERBOSE)) {
            LOG_CLI((BSL_META_U(unit,
                                "UINST %d: %08x %08x %08x %08x %08x %08x\n"),
                     (i - 1)/ 2,
                     ((uint32 *)&uinst_entry)[0],
                     ((uint32 *)&uinst_entry)[1],
                     ((uint32 *)&uinst_entry)[2],
                     ((uint32 *)&uinst_entry)[3],
                     ((uint32 *)&uinst_entry)[4],
                     ((uint32 *)&uinst_entry)[5]));
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, ET_UINST_MEMm, MEM_BLOCK_ANY,
                                          (i - 1) / 2 + num_pad_entries,
                                          &uinst_entry));
    }

    if (always_drive_dbus) {
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG3_ISr(unit, &cfg));
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &cfg,
                          ALWAYS_DRIVE_DBUSf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG3_ISr(unit, cfg));
    }

    rval0 = 0;
    soc_reg_field_set(unit, ETU_ET_INST_REQr, &rval0, STARTf, 1);
    soc_reg_field_set(unit, ETU_ET_INST_REQr, &rval0, OP_TYPf, op_type);
    soc_reg_field_set(unit, ETU_ET_INST_REQr, &rval0, NUM_PST_DNOPSf,
                      num_post_nop);
    soc_reg_field_set(unit, ETU_ET_INST_REQr, &rval0, NUM_PRE_DNOPSf,
                      num_pre_nop);
    soc_reg_field_set(unit, ETU_ET_INST_REQr, &rval0, NUM_INST_DOPSf,
                      ((num_inst + 1) / 2 + num_pad_entries) & 0x03);

    if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_VERBOSE)) {
        LOG_CLI((BSL_META_U(unit,
                            "INST_REQ: %08x\n"), rval0));
    }

    rv = WRITE_ETU_ET_INST_REQr(unit, rval0);
    if (SOC_SUCCESS(rv)) {
        for (retry = 10; retry; retry--) {
            rv = READ_ETU_ET_INST_STATUSr(unit, &status);
            if (SOC_FAILURE(rv)) {
                break;
            }
            if (soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status, DONEf)) {
                break;
            }

            sal_usleep(100000);
        }
        if (!retry) {
            if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_triumph_tcam_access: Instruction timeout\n")));
            }
            rv = SOC_E_TIMEOUT;
        }
    }

    if (always_drive_dbus) {
        soc_reg_field_set(unit, ETU_DDR72_CONFIG_REG3_ISr, &cfg,
                          ALWAYS_DRIVE_DBUSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ETU_DDR72_CONFIG_REG3_ISr(unit, cfg));
    }

    if (SOC_FAILURE(rv)) {
        return rv;
    }

    if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_VERBOSE)) {
        LOG_CLI((BSL_META_U(unit,
                            "INST_STAT: %08x\n"), status));
    }

    switch (op_type) {
    case TCAM_TR_OP_WRITE:
        break;

    case TCAM_TR_OP_READ:
        if (!SOC_WARM_BOOT(unit) &&
            !soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status, RDACKf)) {
            if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_triumph_tcam_access: No rdack\n")));
            }
            return SOC_E_NOT_FOUND;
        }

        SOC_IF_ERROR_RETURN(READ_ETU_RSLT_DAT0r(unit, &rval0));
        SOC_IF_ERROR_RETURN(READ_ETU_RSLT_DAT1r(unit, &rval1));
        SOC_IF_ERROR_RETURN(READ_ETU_RSLT_DAT2r(unit, &rval2));
        dbus[0] = rval0;
        dbus[1] = rval1;
        dbus[2] = rval2 & 0xff;
        if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_INFO)) {
            LOG_CLI((BSL_META_U(unit,
                                "READ: RSLT_DAT: %08x %08x %08x\n"),
                     rval0, rval1, rval2));
        }
        break;

    case TCAM_TR_OP_SINGLE_SEARCH1:
    case TCAM_TR_OP_SINGLE_SEARCH0:
    case TCAM_TR_OP_PARALLEL_SEARCH:
        rval0 = 0xffffffff;
        rval1 = 0xffffffff;
        if ((op_type == TCAM_TR_OP_SINGLE_SEARCH0 ||
             op_type == TCAM_TR_OP_PARALLEL_SEARCH) &&
            soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                              RBUS0_VALIDf) &&
            soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                              RBUS0_HITf) == 3 &&
            !soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                               RBUS0_PERRf) &&
            !soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                               SEQ_DPEO_ERRf)) {
            SOC_IF_ERROR_RETURN(READ_ETU_SEARCH0_RESULTr(unit, &rval0));
            rval0 = soc_reg_field_get(unit, ETU_SEARCH0_RESULTr, rval0, RBUSf);
        }

        if ((op_type == TCAM_TR_OP_SINGLE_SEARCH1 ||
             op_type == TCAM_TR_OP_PARALLEL_SEARCH) &&
            soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                              RBUS1_VALIDf) &&
            soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                              RBUS1_HITf) == 3 &&
            !soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                               RBUS1_PERRf) &&
            !soc_reg_field_get(unit, ETU_ET_INST_STATUSr, status,
                               SEQ_DPEO_ERRf)) {
            SOC_IF_ERROR_RETURN(READ_ETU_SEARCH1_RESULTr(unit, &rval1));
            rval1 = soc_reg_field_get(unit, ETU_SEARCH1_RESULTr, rval1, RBUSf);
        }

        if (rval0 == 0xffffffff && rval1 == 0xffffffff) {
            return SOC_E_NOT_FOUND;
        }

        dbus[0] = rval0;
        dbus[1] = rval1;
        if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_INFO)) {
            LOG_CLI((BSL_META_U(unit,
                                "SEARCH: RESULT_RESULT: %08x %08x\n"),
                     rval0, rval1));
        }
        break;

    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
soc_triumph_tcam_search_bist(int unit, int part0, int part1, uint32 *data,
                             int index0, int index1, int loop_count)
{
    uint32 rval;
    int rv, result0, result1;

    rval = 0;
    if (part0 != TCAM_PARTITION_RAW) {
        soc_reg_field_set(unit, ETU_LTE_BIST_REF_SEARCH0r, &rval, RBUSf,
                          index0);
        soc_reg_field_set(unit, ETU_LTE_BIST_REF_SEARCH0r, &rval, HITf, 3);
    }
    SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_REF_SEARCH0r(unit, rval));

    rval = 0;
    if (part1 != TCAM_PARTITION_RAW) {
        soc_reg_field_set(unit, ETU_LTE_BIST_REF_SEARCH1r, &rval, RBUSf,
                          index1);
        soc_reg_field_set(unit, ETU_LTE_BIST_REF_SEARCH1r, &rval, HITf, 3);
    }
    SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_REF_SEARCH1r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, RD_EN_BIST_RSLTSf, 1);
    if (loop_count != -1) {
        soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                          loop_count);
    }
    SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(unit, rval));

    rv = soc_tcam_search_entry(unit, part0, part1, data, &result0, &result1);

    if (SOC_SUCCESS(rv)) {
        rv = READ_ETU_LTE_BIST_STATUSr(unit, &rval);
        if (SOC_SUCCESS(rv)) {
            rv = rval ? SOC_E_FAIL : SOC_E_NONE;
        }
    }

    /* disable BIST */
    rval = 0;
    soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, RD_EN_BIST_RSLTSf, 0);
    soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(unit, rval));

    return rv;
}

typedef struct tr_sram_bist_regs_s {
    soc_reg_t mode;
    soc_reg_t lte_d0r_0;
    soc_reg_t lte_d0r_1;
    soc_reg_t lte_d0f_0;
    soc_reg_t lte_d0f_1;
    soc_reg_t lte_d1r_0;
    soc_reg_t lte_d1r_1;
    soc_reg_t lte_d1f_0;
    soc_reg_t lte_d1f_1;
    soc_reg_t lte_adr0 ;
    soc_reg_t lte_adr1 ;
    soc_reg_t lte_tmode0;
    soc_reg_t lte_tmode1;
    soc_reg_t lte_sts_done;
    soc_reg_t lte_sts_err_adr;
    soc_reg_t lte_sts_err_dr_0;
    soc_reg_t lte_sts_err_dr_1;
    soc_reg_t lte_sts_err_df_0;
    soc_reg_t lte_sts_err_df_1;
} tr_sram_bist_regs_t;

STATIC const tr_sram_bist_regs_t tr_sram_bist_regs[2] = {
    {
        ES0_DTU_MODEr,
        ES0_DTU_LTE_D0R_0r,
        ES0_DTU_LTE_D0R_1r,
        ES0_DTU_LTE_D0F_0r,
        ES0_DTU_LTE_D0F_1r,
        ES0_DTU_LTE_D1R_0r,
        ES0_DTU_LTE_D1R_1r,
        ES0_DTU_LTE_D1F_0r,
        ES0_DTU_LTE_D1F_1r,
        ES0_DTU_LTE_ADR0r,
        ES0_DTU_LTE_ADR1r,
        ES0_DTU_LTE_TMODE0r,
        ES0_DTU_LTE_TMODE1r,
        ES0_DTU_LTE_STS_DONEr,
        ES0_DTU_LTE_STS_ERR_ADRr,
        ES0_DTU_LTE_STS_ERR_DR_0r,
        ES0_DTU_LTE_STS_ERR_DR_1r,
        ES0_DTU_LTE_STS_ERR_DF_0r,
        ES0_DTU_LTE_STS_ERR_DF_1r,
    },
    {
        ES1_DTU_MODEr,
        ES1_DTU_LTE_D0R_0r,
        ES1_DTU_LTE_D0R_1r,
        ES1_DTU_LTE_D0F_0r,
        ES1_DTU_LTE_D0F_1r,
        ES1_DTU_LTE_D1R_0r,
        ES1_DTU_LTE_D1R_1r,
        ES1_DTU_LTE_D1F_0r,
        ES1_DTU_LTE_D1F_1r,
        ES1_DTU_LTE_ADR0r,
        ES1_DTU_LTE_ADR1r,
        ES1_DTU_LTE_TMODE0r,
        ES1_DTU_LTE_TMODE1r,
        ES1_DTU_LTE_STS_DONEr,
        ES1_DTU_LTE_STS_ERR_ADRr,
        ES1_DTU_LTE_STS_ERR_DR_0r,
        ES1_DTU_LTE_STS_ERR_DR_1r,
        ES1_DTU_LTE_STS_ERR_DF_0r,
        ES1_DTU_LTE_STS_ERR_DF_1r,
    }
};

int
soc_triumph_ext_sram_enable_set(int unit, int intf_num, int enable,
                                int clear_status)
{
    const tr_sram_bist_regs_t *bist_regs;
    soc_reg_t reg;
    uint32 reg_addr, orval, rval;

    if (intf_num < 0 || intf_num > 1) {
        return SOC_E_PARAM;
    }
    bist_regs = &tr_sram_bist_regs[intf_num];

    /* Enable/disable  LTE - lab test engine  */
    reg = bist_regs->mode;
    reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
    orval = rval;
    soc_reg_field_set(unit, reg, &rval, DTU_ENf, enable ? 1 : 0);
    soc_reg_field_set(unit, reg, &rval, SEL_LTEf, enable ? 1 : 0);
    if (rval != orval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    /* Clear status */
    if (clear_status) {
        reg = bist_regs->lte_tmode0;
        reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
        soc_reg_field_set(unit, reg, &rval, START_LAB_TESTf, 0);
        soc_reg_field_set(unit, reg, &rval, CLR_STATUSf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    return SOC_E_NONE;
}

/* Setup (but not yet start) BIST on specified SRAM module */
int
soc_triumph_ext_sram_bist_setup(int unit, int intf_num,
                                tr_ext_sram_bist_t *sram_bist)
{
    const tr_sram_bist_regs_t *bist_regs;
    soc_reg_t reg;
    uint32 reg_addr, orval, rval;

    if (intf_num < 0 || intf_num > 1) {
        return SOC_E_PARAM;
    }
    bist_regs = &tr_sram_bist_regs[intf_num];

    /* Program optional data registers */
    if (sram_bist->d0r_0 != 0xffffffff) {
        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d0r_0, &rval, DTU_LTE_D0R_0f,
                          sram_bist->d0r_0);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d0r_0, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d0r_1, &rval, DTU_LTE_D0R_1f,
                          sram_bist->d0r_1);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d0r_1, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d0f_0, &rval, DTU_LTE_D0F_0f,
                          sram_bist->d0f_0);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d0f_0, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d0f_1, &rval, DTU_LTE_D0F_1f,
                          sram_bist->d0f_1);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d0f_1, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }
    if (sram_bist->d1r_0 != 0xffffffff) {
        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d1r_0, &rval, DTU_LTE_D1R_0f,
                          sram_bist->d1r_0);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d1r_0, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d1r_1, &rval, DTU_LTE_D1R_1f,
                          sram_bist->d1r_1);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d1r_1, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d1f_0, &rval, DTU_LTE_D1F_0f,
                          sram_bist->d1f_0);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d1f_0, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_d1f_1, &rval, DTU_LTE_D1F_1f,
                          sram_bist->d1f_1);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_d1f_1, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    /* Program optional address registers */
    if (sram_bist->adr0 != -1) {
        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_adr0, &rval, DTU_LTE_ADR0f,
                          sram_bist->adr0);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_adr0, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }
    if (sram_bist->adr1 != -1) {
        rval = 0;
        soc_reg_field_set(unit, bist_regs->lte_adr1, &rval, DTU_LTE_ADR1f,
                          sram_bist->adr1);
        reg_addr = soc_reg_addr(unit, bist_regs->lte_adr1, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    /* Program optional TMODE1 register */
    if (sram_bist->wdoebr != -1) {
        reg = bist_regs->lte_tmode1;
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, WDOEBRf, sram_bist->wdoebr);
        soc_reg_field_set(unit, reg, &rval, WDOEBFf, sram_bist->wdoebf);
        soc_reg_field_set(unit, reg, &rval, WDMRf, sram_bist->wdmr);
        soc_reg_field_set(unit, reg, &rval, WDMFf, sram_bist->wdmf);
        soc_reg_field_set(unit, reg, &rval, RDMRf, sram_bist->rdmr);
        soc_reg_field_set(unit, reg, &rval, RDMFf, sram_bist->rdmf);
        reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    /* Program TMODE0 register */
    reg = bist_regs->lte_tmode0;
    reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
    orval = rval;
    soc_reg_field_set(unit, reg, &rval, CLR_STATUSf, 0);
    soc_reg_field_set(unit, reg, &rval, LOOP_MODEf, sram_bist->loop_mode);
    soc_reg_field_set(unit, reg, &rval, ADR_MODEf, sram_bist->adr_mode);
    soc_reg_field_set(unit, reg, &rval, CMP_EM_RDAT_FRf,
                      sram_bist->cmp_em_rdat_fr);
    if (sram_bist->em_latency != -1) {
        soc_reg_field_set(unit, reg, &rval, EM_LATENCYf,
                          sram_bist->em_latency & 0x7);
        if (soc_reg_field_valid(unit, reg, EM_LATENCY8f)) {
            soc_reg_field_set(unit, reg, &rval, EM_LATENCY8f,
                              sram_bist->em_latency >> 3);
        }
    }
    if (sram_bist->w2r_nops != -1) {
        soc_reg_field_set(unit, reg, &rval, W2R_NOPSf, sram_bist->w2r_nops);
    }
    if (sram_bist->r2w_nops != -1) {
        soc_reg_field_set(unit, reg, &rval, R2W_NOPSf, sram_bist->r2w_nops);
    }
    if (rval != orval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    return SOC_E_NONE;
}

int
soc_triumph_ext_sram_op(int unit, int intf_num, tr_ext_sram_bist_t *sram_bist,
                        tr_ext_sram_bist_t *opt_sram_bist)
{
    soc_tcam_info_t *tcam_info;
    const tr_sram_bist_regs_t *bist_regs[2];
    tr_ext_sram_bist_t *bist_ptr;
    soc_timeout_t to;
    int to_usec;
    soc_reg_t reg;
    uint32 reg_addr, rval;
    int rv, rv1, i;

    tcam_info = SOC_CONTROL(unit)->tcam_info;

    if (intf_num < 0 || intf_num > 1) {
        return SOC_E_PARAM;
    }
    bist_regs[0] = &tr_sram_bist_regs[intf_num];
    if (opt_sram_bist != NULL) {
        bist_regs[1] = &tr_sram_bist_regs[intf_num ^ 1];
    }

    if (SAL_BOOT_QUICKTURN) {
        to_usec = 60000000;
    } else {
        to_usec = 50000;
    }

    if (opt_sram_bist != NULL) {
        /* Kick off the optional BIST on the other SRAM interface */
        reg = bist_regs[1]->lte_tmode0;
        reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
        soc_reg_field_set(unit, reg, &rval, START_LAB_TESTf, 1);
        soc_reg_field_set(unit, reg, &rval, FOREVERf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));
    }

    /* Kick off the BIST on the target SRAM interface */
    reg = bist_regs[0]->lte_tmode0;
    reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
    soc_reg_field_set(unit, reg, &rval, START_LAB_TESTf, 1);
    if (sram_bist->bg_tcam_loop_count > 1) {
        soc_reg_field_set(unit, reg, &rval, FOREVERf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, rval));

    rv = SOC_E_NONE;
    if (sram_bist->bg_tcam_loop_count > 1) {
        /* Kick off the optional BIST on TCAM interface */
        rval = 0;
        soc_reg_field_set(unit, ETU_ET_INST_REQr, &rval, STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ETU_ET_INST_REQr(unit, rval));

        /* BIST should have already finished if loop count is small */
        if (sram_bist->bg_tcam_loop_count > 200) {
            /* loop_count * 8 instrction / freq */
            sal_udelay(sram_bist->bg_tcam_loop_count * 8 /
                       tcam_info->tcam_freq);
        }

        /* Wait until TCAM interface BIST done */
        soc_timeout_init(&to, to_usec, 0);
        while (1) {
            rv = READ_ETU_ET_INST_STATUSr(unit, &rval);
            if (SOC_FAILURE(rv)) {
                break;
            }
            if (soc_reg_field_get(unit, ETU_ET_INST_STATUSr, rval, DONEf)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                rv = SOC_E_TIMEOUT;
                break;
            }
        }

        /* Remove the target SRAM interface from forever mode */
        rv1 = soc_reg32_read(unit, reg_addr, &rval);
        if (SOC_SUCCESS(rv1)) {
            soc_reg_field_set(unit, reg, &rval, FOREVERf, 0);
        } else {
            rval = 0;
        }
        rv1 = soc_reg32_write(unit, reg_addr, rval);
    }

    if (opt_sram_bist != NULL) {
        /* Remove the other SRAM interface from forever mode */
        reg = bist_regs[1]->lte_tmode0;
        reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        rv1 = soc_reg32_read(unit, reg_addr, &rval);
        if (SOC_SUCCESS(rv1)) {
            soc_reg_field_set(unit, reg, &rval, FOREVERf, 0);
        } else {
            rval = 0;
        }
        rv1 = soc_reg32_write(unit, reg_addr, rval);
    }

    if (SOC_FAILURE(rv)) {
        return rv;
    }

    for (i = 0; i < (opt_sram_bist != NULL ? 2 : 1); i++) {
        bist_ptr = i == 0 ? sram_bist : opt_sram_bist;

        /* Wait until BIST done on SRAM interface */
        soc_timeout_init(&to, to_usec, 0);
        reg = bist_regs[i]->lte_sts_done;
        reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        while (1) {
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
            if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                return SOC_E_TIMEOUT;
            }
        }
        bist_ptr->err_cnt = soc_reg_field_get(unit, reg, rval, ERR_CNTf);
        bist_ptr->err_bitmap = soc_reg_field_get(unit, reg, rval, ERR_BITMAPf);

        if (bist_ptr->err_cnt || bist_ptr->err_bitmap) {
            reg = bist_regs[i]->lte_sts_err_adr;
            reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
            bist_ptr->err_adr = soc_reg_field_get(unit, reg, rval, ERR_ADRf);

            reg = bist_regs[i]->lte_sts_err_dr_0;
            reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
            bist_ptr->err_dr_0 = soc_reg_field_get(unit, reg, rval, ERR_DR_0f);

            reg = bist_regs[i]->lte_sts_err_dr_1;
            reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
            bist_ptr->err_dr_1 = soc_reg_field_get(unit, reg, rval, ERR_DR_1f);

            reg = bist_regs[i]->lte_sts_err_df_0;
            reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
            bist_ptr->err_df_0 = soc_reg_field_get(unit, reg, rval, ERR_DF_0f);

            reg = bist_regs[i]->lte_sts_err_df_1;
            reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &rval));
            bist_ptr->err_df_1 = soc_reg_field_get(unit, reg, rval, ERR_DF_1f);
        }
    }

    return SOC_E_NONE;
}

int
soc_triumph_l2x_to_ext_l2(int unit, l2x_entry_t *l2x_entry,
                          ext_l2_entry_entry_t *ext_l2_entry)
{
    sal_mac_addr_t mac;
    uint32 fval;

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, VLAN_IDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf, fval);

    soc_mem_mac_addr_get(unit, L2Xm, l2x_entry, MAC_ADDRf, mac);
    soc_mem_mac_addr_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf, mac);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, KEY_TYPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, KEY_TYPE_VFIf,
                        fval == 3 ? 1 : 0);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, DESTINATIONf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DESTINATIONf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, DEST_TYPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DEST_TYPEf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, CLASS_IDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, CLASS_IDf, fval);


    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, RPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, RPEf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, MIRRORf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MIRRORf, fval);

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, MIRROR1f)) {
        fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, MIRROR1f);
        soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, MIRROR1f, fval);
    }

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, PRIf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, PRIf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, CPUf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, CPUf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, DST_DISCARDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_DISCARDf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, SRC_DISCARDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_DISCARDf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, SCPf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SCPf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, STATIC_BITf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, STATIC_BITf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, VALIDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, VALIDf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, HITDAf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, HITSAf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf, fval);

    fval = soc_mem_field32_get(unit, L2Xm, l2x_entry, LIMIT_COUNTEDf);
    soc_mem_field32_set(unit, EXT_L2_ENTRYm, ext_l2_entry, LIMIT_COUNTEDf, fval);

    return SOC_E_NONE;
}

int
soc_triumph_ext_l2_to_l2x(int unit, ext_l2_entry_entry_t *ext_l2_entry,
                          l2x_entry_t *l2x_entry)
{
    sal_mac_addr_t mac;
    uint32 fval;

    sal_memset(l2x_entry, 0, sizeof(l2x_entry_t));

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, VLAN_IDf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, VLAN_IDf, fval);

    soc_mem_mac_addr_get(unit, EXT_L2_ENTRYm, ext_l2_entry, MAC_ADDRf, mac);
    soc_mem_mac_addr_set(unit, L2Xm, l2x_entry, MAC_ADDRf, mac);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                               KEY_TYPE_VFIf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, KEY_TYPEf, fval ? 3 : 0);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                               DESTINATIONf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, DESTINATIONf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DEST_TYPEf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, DEST_TYPEf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, CLASS_IDf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, CLASS_IDf, fval);


    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, RPEf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, RPEf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, MIRRORf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, MIRRORf, fval);

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, MIRROR1f)) {
        fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, MIRROR1f);
        soc_mem_field32_set(unit, L2Xm, l2x_entry, MIRROR1f, fval);
    }

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, PRIf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, PRIf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, CPUf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, CPUf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                               DST_DISCARDf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, DST_DISCARDf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                               SRC_DISCARDf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, SRC_DISCARDf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SCPf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, SCPf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, STATIC_BITf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, STATIC_BITf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, VALIDf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, VALIDf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, DST_HITf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, HITDAf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry, SRC_HITf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, HITSAf, fval);

    fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                               LIMIT_COUNTEDf);
    soc_mem_field32_set(unit, L2Xm, l2x_entry, LIMIT_COUNTEDf, fval);

    return SOC_E_NONE;
}

/*
 * External L2 Bulk Delete/Replace Acceleration
 *
 * Hardware scanning of the external L2 table for bulk delete and
 * bulk replace can be a little slow with large L2 tables.  This
 * code caches enough information on each entry in order to do a
 * quick software walk of the table looking for matches.
 *
 * There are per-vlan min and max indices that are kept so that for
 * per-vlan or per-vlan/port scans we only need to look at a subset of
 * the entries.
 *
 * Times to scan a 256K L2 table that is empty or near empty drop from
 * 56 msec to 0.8 msec.  If all entries still need to be looked at
 * (because the vlan min/max is the whole table, the scna time is about
 * 6 msec.
 *
 * Memory use is 12 bytes per L2 entry (data word plus mac address)
 * which is 3MBytes for a 256K L2 table.  The per-vlan limits are
 * 32KBytes.
 */
int
_soc_tr_l2e_ppa_init(int unit)
{
    _soc_tr_l2e_ppa_info_t      *einfo;
    _soc_tr_l2e_ppa_vlan_t      *evlan;
    int                         vlan, esize;

    if (SOC_CONTROL(unit)->ext_l2_ppa_info != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_info);
        SOC_CONTROL(unit)->ext_l2_ppa_info = NULL;
    }
    if (SOC_CONTROL(unit)->ext_l2_ppa_vlan != NULL) {
        sal_free(SOC_CONTROL(unit)->ext_l2_ppa_vlan);
        SOC_CONTROL(unit)->ext_l2_ppa_vlan = NULL;
    }

    esize = soc_mem_index_count(unit, EXT_L2_ENTRYm) *
        sizeof(_soc_tr_l2e_ppa_info_t);
    einfo = sal_alloc(esize, "_soc_tr_l2e_ppa_info");
    if (einfo == NULL) {
        return SOC_E_MEMORY;
    }

    evlan = sal_alloc(sizeof(_soc_tr_l2e_ppa_vlan_t),
                      "_soc_tr_l2e_ppa_vlan");
    if (evlan == NULL) {
        sal_free(einfo);
        return SOC_E_MEMORY;
    }
    sal_memset(einfo, 0, esize);
    for (vlan = 0; vlan <= VLAN_ID_MAX; vlan++) {
        evlan->vlan_min[vlan] = -1;
        evlan->vlan_max[vlan] = -1;
    }
    SOC_CONTROL(unit)->ext_l2_ppa_info = einfo;
    SOC_CONTROL(unit)->ext_l2_ppa_vlan = evlan;
    return SOC_E_NONE;
}

STATIC int
_soc_tr_l2e_ppa_update(int unit, int index, ext_l2_entry_entry_t *entry)
{
    _soc_tr_l2e_ppa_info_t *ppa_info;
    _soc_tr_l2e_ppa_vlan_t *ppa_vlan;
    uint32      entvalue;
    uint32      l2data;
    sal_mac_addr_t   l2mac;
    soc_mem_t   mem;
    vlan_id_t   vlan, oldvlan;
    int         vlan_min, vlan_max;
    uint32      vdata, vmask;

    mem = EXT_L2_ENTRYm;
    if (SOC_CONTROL(unit)->ext_l2_ppa_info == NULL) {
        if (!soc_mem_is_valid(unit, mem) ||
            soc_mem_index_count(unit, mem) <= 0) {
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN(_soc_tr_l2e_ppa_init(unit));
    }

    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    ppa_vlan = SOC_CONTROL(unit)->ext_l2_ppa_vlan;

    vlan = 0;
    entvalue = 0;
    if (entry != NULL) {
        l2data = soc_mem_field32_get(unit, mem, entry, LIMIT_COUNTEDf);
        if (l2data != 0) {
            entvalue |= _SOC_TR_L2E_LIMIT_COUNTED;
        }
        l2data = soc_mem_field32_get(unit, mem, entry, VALIDf);
        if (l2data != 0) {
            entvalue |= _SOC_TR_L2E_VALID;
        }
        l2data = soc_mem_field32_get(unit, mem, entry, STATIC_BITf);
        if (l2data != 0) {
            entvalue |= _SOC_TR_L2E_STATIC;
        }
        l2data = soc_mem_field32_get(unit, mem, entry, VLAN_IDf);
        vlan = l2data;
        entvalue |= (l2data & _SOC_TR_L2E_VLAN_MASK) << _SOC_TR_L2E_VLAN_SHIFT;
        l2data = soc_mem_field32_get(unit, mem, entry, Tf);
        if (l2data) {
            l2data = soc_mem_field32_get(unit, mem, entry, TGIDf);
            entvalue |= _SOC_TR_L2E_T |
                ((l2data & _SOC_TR_L2E_TRUNK_MASK) << _SOC_TR_L2E_TRUNK_SHIFT);
        } else {
            l2data = soc_mem_field32_get(unit, mem, entry, MODULE_IDf);
            entvalue |=
                (l2data & _SOC_TR_L2E_MOD_MASK) << _SOC_TR_L2E_MOD_SHIFT;
            l2data = soc_mem_field32_get(unit, mem, entry, PORT_NUMf);
            entvalue |=
                (l2data & _SOC_TR_L2E_PORT_MASK) << _SOC_TR_L2E_PORT_SHIFT;
        }
        soc_mem_mac_addr_get(unit, mem, entry, MAC_ADDRf, l2mac);
    } else {
        sal_memset(l2mac, 0, sizeof(l2mac));
    }
    oldvlan = (ppa_info[index].data >> 16) & 0xfff;
    ppa_info[index].data = entvalue;
    sal_memcpy(ppa_info[index].mac, l2mac, sizeof(sal_mac_addr_t));

    if (vlan != oldvlan) {      /* update vlan_min/max indices */
        if (oldvlan != 0) {
            vlan_min = ppa_vlan->vlan_min[oldvlan];
            vlan_max = ppa_vlan->vlan_max[oldvlan];
            vdata = oldvlan << _SOC_TR_L2E_VLAN_SHIFT;
            vmask = _SOC_TR_L2E_VLAN_MASK << _SOC_TR_L2E_VLAN_SHIFT;

            /* deleted last one? */
            if (index == vlan_min && index == vlan_max) {
                ppa_vlan->vlan_min[oldvlan] = -1;
                ppa_vlan->vlan_max[oldvlan] = -1;
            } else if (index == vlan_min) {
                for (; vlan_min <= vlan_max; vlan_min++) {
                    entvalue = ppa_info[vlan_min].data;
                    if ((entvalue & vmask) == vdata) {
                        break;
                    }
                }
                ppa_vlan->vlan_min[oldvlan] = vlan_min;
            } else if (index == vlan_max) {
                for (; vlan_min <= vlan_max; vlan_max--) {
                    entvalue = ppa_info[vlan_max].data;
                    if ((entvalue & vmask) == vdata) {
                        break;
                    }
                }
                ppa_vlan->vlan_max[oldvlan] = vlan_max;
            }

        }
        if (vlan != 0) {
            vlan_min = ppa_vlan->vlan_min[vlan];
            vlan_max = ppa_vlan->vlan_max[vlan];
            if (vlan_min < 0 || index < vlan_min) {
                ppa_vlan->vlan_min[vlan] = index;
            }
            if (vlan_max < 0 || index > vlan_max) {
                ppa_vlan->vlan_max[vlan] = index;
            }
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "tr_l2e_ppa: index=%d oldvlan=%d min:max %d:%d\n"),
                            index, oldvlan,
                 ppa_vlan->vlan_min[oldvlan],
                 ppa_vlan->vlan_max[oldvlan]));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "tr_l2e_ppa: ent=0x%x mac=%x:%x:%x:%x:%x:%x vlan=%d min:max %d:%d\n"),
                            ppa_info[index].data,
                 l2mac[0], l2mac[1], l2mac[2], l2mac[3], l2mac[4], l2mac[5],
                 vlan,
                 ppa_vlan->vlan_min[vlan],
                 ppa_vlan->vlan_max[vlan]));
    return SOC_E_NONE;
}

int
soc_triumph_ext_l2_entry_update(int unit, int index, void *entry)
{
    ext_l2_entry_tcam_entry_t tcam_entry;
    void *tcam_entry_ptr;
    uint32 fval, *cache;
    sal_mac_addr_t mac;
    int copyno, tcam_entry_dw;
    
    copyno = SOC_MEM_BLOCK_ANY(unit, EXT_L2_ENTRY_TCAMm);
    cache = SOC_MEM_STATE(unit, EXT_L2_ENTRY_TCAMm).cache[copyno];
    if (cache != NULL) {
        if (entry != NULL) {
            sal_memset(&tcam_entry, 0, sizeof(tcam_entry));
            fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, entry, VLAN_IDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &tcam_entry,
                                VLAN_IDf, fval);
            soc_mem_mac_addr_get(unit, EXT_L2_ENTRYm, entry, MAC_ADDRf, mac);
            soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_TCAMm, &tcam_entry,
                                 MAC_ADDRf, mac);
            fval = soc_mem_field32_get(unit, EXT_L2_ENTRYm, entry,
                                       KEY_TYPE_VFIf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &tcam_entry,
                                KEY_TYPE_VFIf, fval);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &tcam_entry, FREEf,
                                0);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_TCAMm, &tcam_entry,
                                MASK_RESERVEDf, 0x3ff);
            tcam_entry_ptr = &tcam_entry;
        } else {
            tcam_entry_ptr = soc_mem_entry_null(unit, EXT_L2_ENTRY_TCAMm);
        }
        tcam_entry_dw = soc_mem_entry_words(unit, EXT_L2_ENTRY_TCAMm);
        sal_memcpy(cache + index * tcam_entry_dw, tcam_entry_ptr,
                   tcam_entry_dw * 4);
    }

    _soc_tr_l2e_ppa_update(unit, index, entry);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_triumph_learn_count_update
 * Description:
 *     Check MAC learn limit and update MAC learn counter if it won't go over
 *     the limit
 * Parameters:
 *     unit            device number
 *     ext_l2_entry    entry to be add or delete
 *     incl_sys_vlan   update system and vlan table or not
 *     diff            number to be add to MAC counter
 *                      use negative to subtract MAC counter
 * Return:
 *     SOC_E_XXX
 */
int
soc_triumph_learn_count_update(int unit, ext_l2_entry_entry_t *ext_l2_entry,
                               int incl_sys_vlan, int diff)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    vlan_or_vfi_mac_count_entry_t v_entry;
    port_or_trunk_mac_count_entry_t p_entry;
    int v_index, p_index, s_limit, v_limit, p_limit, s_count, v_count, p_count;
    int rv;

    p_index = -1;
    v_index = -1;
    switch (soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                DEST_TYPEf)) {
    case 0: /* mod port */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, 0,
                                         &entry));
        if (soc_mem_field32_get(unit, PORT_TABm, &entry, MY_MODIDf) ==
            soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                MODULE_IDf)) {
            p_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                          PORT_NUMf) + 128;
        }
        break;
    case 1: /* trunk */
        p_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                      TGIDf);
        break;
    default:
        return SOC_E_NONE;
    }
    if (incl_sys_vlan) {
        v_index = soc_mem_field32_get(unit, EXT_L2_ENTRYm, ext_l2_entry,
                                      VLAN_IDf);
    }

    /* read limit setting */
    s_limit = v_limit = p_limit = -1;
    if (diff > 0) {
        SOC_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
        if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval, ENABLEf)) {
            if (v_index >= 0) {
                s_limit = soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                                            SYS_LIMITf);
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, VLAN_OR_VFI_MAC_LIMITm,
                                                 MEM_BLOCK_ANY, v_index,
                                                 &entry));
                v_limit = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_LIMITm,
                                              &entry, LIMITf);
            }
            if (p_index >= 0) {
                SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                                 PORT_OR_TRUNK_MAC_LIMITm,
                                                 MEM_BLOCK_ANY, p_index,
                                                 &entry));
                p_limit = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                              &entry, LIMITf);
            }
        } else {
            /* If MAC Learn limit count is not enable -> do nothing */
            return SOC_E_NONE;
        }
    }

    SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_save(unit));

    /* read current count */
    rv = SOC_E_NONE;
    s_count = v_count = 0;
    if (v_index >= 0) {
        rv = READ_SYS_MAC_COUNTr(unit, &rval);
        if (SOC_SUCCESS(rv)) {
            s_count = soc_reg_field_get(unit, SYS_MAC_COUNTr, rval, COUNTf);

            rv = soc_mem_read(unit, VLAN_OR_VFI_MAC_COUNTm, MEM_BLOCK_ANY,
                              v_index, &v_entry);
            if (SOC_SUCCESS(rv)) {
                v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                              &v_entry, COUNTf);
            }
        }
    }

    p_count = 0;
    if (SOC_SUCCESS(rv) && p_index >= 0) {
        rv = soc_mem_read(unit, PORT_OR_TRUNK_MAC_COUNTm, MEM_BLOCK_ANY,
                          p_index, &p_entry);
        if (SOC_SUCCESS(rv)) {
            p_count = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                          &p_entry, COUNTf);
        }
    }

    if (SOC_FAILURE(rv)) {
        SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_restore(unit));
        return rv;
    }

    if (v_index >= 0) {
        s_count += diff;
        v_count += diff;
    }
    if (p_index >= 0) {
        p_count += diff;
    }

    if (diff > 0) {
        if ((s_limit >= 0 && s_count > s_limit) ||
            (v_limit >= 0 && v_count > v_limit) ||
            (p_limit >= 0 && p_count > p_limit)) {
            SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_restore(unit));
            return SOC_E_RESOURCE;
        }
    } else {
        if (s_count < 0) {
            s_count = 0;
        }
        if (v_count < 0) {
            v_count = 0;
        }
        if (p_count < 0) {
            p_count = 0;
        }
    }

    if (v_index >= 0) {
        soc_reg_field_set(unit, SYS_MAC_COUNTr, &rval, COUNTf, s_count);
        rv = WRITE_SYS_MAC_COUNTr(unit, rval);

        if (SOC_SUCCESS(rv)) {
            soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm, &v_entry, COUNTf,
                                v_count);
            rv = WRITE_VLAN_OR_VFI_MAC_COUNTm(unit, MEM_BLOCK_ANY, v_index,
                                              &v_entry);
        }
    }

    if (SOC_SUCCESS(rv) && p_index >= 0) {
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm, &p_entry, COUNTf,
                            p_count);
        rv = WRITE_PORT_OR_TRUNK_MAC_COUNTm(unit, MEM_BLOCK_ANY, p_index,
                                            &p_entry);
    }

    SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_restore(unit));

    return rv;
}

STATIC int
_soc_triumph_tx(int unit, int port, int pkt_len)
{
    static dcb14_t *dcb[SOC_MAX_NUM_DEVICES] = {NULL};
    soc_pbsmh_v3_hdr_t pbsmh;
    int rc, i, done;
    static sdc_t *dch[SOC_MAX_NUM_DEVICES] = {NULL};
    static dma_chan_t tx_ch[SOC_MAX_NUM_DEVICES] = {-1};
    static uint8 tx_flags[SOC_MAX_NUM_DEVICES] = {0};

    if (pkt_len != 192 && pkt_len != 64) {
        return SOC_E_PARAM;
    }

    if (dcb[unit] == NULL) {
        dch[unit] = SOC_CONTROL(unit)->soc_dma_default_tx;
        tx_ch[unit] = dch[unit]->sc_channel;
        tx_flags[unit] = dch[unit]->sc_flags;
        rc = soc_dma_rom_init(unit, 256, TRUE, FALSE);
        if (SOC_FAILURE(rc)) {
            goto _restore;
        }
        dcb[unit] = soc_dma_rom_dcb_alloc(unit, 192);
        if (dcb[unit] == NULL) {
            rc = SOC_E_MEMORY;
            goto _restore;
        }
    } else {
        SOC_IF_ERROR_RETURN
            (soc_dma_chan_config(unit, SOC_DMA_ROM_TX_CHANNEL, DV_TX,
                                 SOC_DMA_F_MBM | SOC_DMA_F_POLL | SOC_DMA_F_DEFAULT));
    }

    PBS_MH_W0_START_SET(&pbsmh);
    PBS_MH_W1_RSVD_SET(&pbsmh);
    PBS_MH_V3_W2_SMOD_DPORT_COS_SET(&pbsmh, 0, port, 0, 0, 0);

    SOC_DCB_STAT_SET(unit, dcb[unit], 1);
    SOC_DCB_HG_SET(unit, dcb[unit], 1);
    SOC_DCB_REQCOUNT_SET(unit, dcb[unit], pkt_len);
    dcb[unit]->mh0 = soc_ntohl(((uint32 *)&pbsmh)[0]);
    dcb[unit]->mh1 = soc_ntohl(((uint32 *)&pbsmh)[1]);
    dcb[unit]->mh2 = soc_ntohl(((uint32 *)&pbsmh)[2]);
    dcb[unit]->mh3 = 0;

    rc = soc_dma_rom_tx_start(unit, dcb[unit]);
    if (SOC_FAILURE(rc)) {
        goto _restore;
    }
    for (i =0; i < 100000; i++) {
        soc_dma_rom_tx_poll(unit, &done);
        if (done) {
            break;
        }
    }

_restore:
    if (dch[unit]) {
        SOC_IF_ERROR_RETURN
            (soc_dma_chan_config(unit, tx_ch[unit], DV_TX, tx_flags[unit]));
    }
    return rc;
}

int
soc_triumph_xgport_mode_change(int unit, soc_port_t port, soc_mac_mode_t mode)
{
    mac_driver_t *macd;
    soc_field_t rst_field = INVALIDf;
    uint32 rval, to_usec, old_bits, bits;
    uint64 val64;
    soc_pbmp_t ctr_pbmp;
    int rv;
    int autoneg, autoneg_done, speed, duplex;
    int pause_tx, pause_rx, loopback, enable;
    soc_port_ability_t ability;

    sal_memset(&ability, 0, sizeof(soc_port_ability_t));

    if (!SOC_PORT_VALID(unit, port)) {
        return SOC_E_PORT;
    }
    switch (port) {
    case 2:
        rst_field = CMIC_XGP0_RST_Lf;
        break;
    case 14:
        rst_field = CMIC_XGP1_RST_Lf;
        break;
    case 26:
        rst_field = CMIC_XGP2_RST_Lf;
        break;
    case 27:
        rst_field = CMIC_XGP3_RST_Lf;
        break;
    default:
        return SOC_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_XGPORT_MODE_REGr(unit, port, &rval));
    old_bits = soc_reg_field_get(unit, XGPORT_MODE_REGr, rval,
                                 XGPORT_MODE_BITSf);
    bits = mode == SOC_MAC_MODE_10000 ? 2 : 1;
    if (bits == old_bits) {
        return SOC_E_NONE;
    }

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* get current PHY setting */
    SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_get(unit, port, &autoneg,
                                                       &autoneg_done));
    SOC_IF_ERROR_RETURN(soc_phyctrl_ability_advert_get(unit, port, &ability));
    SOC_IF_ERROR_RETURN(soc_phyctrl_speed_get(unit, port, &speed));
    SOC_IF_ERROR_RETURN(soc_phyctrl_duplex_get(unit, port, &duplex));

    /* get current MAC setting */
    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));
    SOC_IF_ERROR_RETURN(MAC_ENABLE_GET(macd, unit, port, &enable));
    SOC_IF_ERROR_RETURN(MAC_LOOPBACK_GET(macd, unit, port, &loopback));
    SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &pause_tx, &pause_rx));

    /* 1, Disable MAC TX and RX */
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, FALSE));

    /* 2. Set port mode to disabled */
    SOC_IF_ERROR_RETURN(READ_XGPORT_MODE_REGr(unit, port, &rval));
    soc_reg_field_set(unit, XGPORT_MODE_REGr, &rval, XGPORT_MODE_BITSf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XGPORT_MODE_REGr(unit, port, rval));

    /* 3. Send packet */
    if (old_bits == 2) { /* it was xport-mode */
        SOC_IF_ERROR_RETURN(_soc_triumph_tx(unit, port, 192));
    } else if (old_bits == 1) { /* it was gport-mode */
        SOC_IF_ERROR_RETURN(_soc_triumph_tx(unit, port, 64));
    }

    /* 4. Wait for (more than) 500 core clks */
    sal_usleep(100000);

    /* 5. Block linkscan and sbus access */
    soc_linkscan_pause(unit);
    SCHAN_LOCK(unit);

    /* 6. Assert XGPORT block */
    rv = READ_CMIC_SOFT_RESET_REGr(unit, &rval);
    if (SOC_SUCCESS(rv)) {
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, rst_field, 0);
        rv = WRITE_CMIC_SOFT_RESET_REGr(unit, rval);
    }

    /* 7. De-assert XGPORT block */
    if (SOC_SUCCESS(rv)) {
        sal_usleep(to_usec);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, rst_field, 1);
        rv = WRITE_CMIC_SOFT_RESET_REGr(unit, rval);
    }

    /* 8. Resume sbus access and linkscan */
    SCHAN_UNLOCK(unit);
    soc_linkscan_continue(unit);
    sal_usleep(to_usec);

    /* 9. Change the XGPORT block mode */
    SOC_IF_ERROR_RETURN(READ_XGPORT_MODE_REGr(unit, port, &rval));
    soc_reg_field_set(unit, XGPORT_MODE_REGr, &rval, XGPORT_MODE_BITSf, bits);
    SOC_IF_ERROR_RETURN(WRITE_XGPORT_MODE_REGr(unit, port, rval));

    /* reset xgxs */
    soc_xgxs_reset(unit, port);
    rval = 0;
    soc_reg_field_set(unit, XGPORT_XGXS_NEWCTL_REGr, &rval, TXD1G_FIFO_RSTBf,
                      mode == SOC_MAC_MODE_10000 ? 0xf : 0x1);
    SOC_IF_ERROR_RETURN(WRITE_XGPORT_XGXS_NEWCTL_REGr(unit, port, rval));

    SOC_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port));
    SOC_IF_ERROR_RETURN(soc_phyctrl_probe(unit, port));
    SOC_IF_ERROR_RETURN(soc_phyctrl_init(unit, port));

    /* restore PHY setting */
    SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_set(unit, port, autoneg));
    SOC_IF_ERROR_RETURN(soc_phyctrl_ability_advert_set(unit, port, &ability));
    SOC_IF_ERROR_RETURN(soc_phyctrl_speed_set(unit, port, speed));
    SOC_IF_ERROR_RETURN(soc_phyctrl_duplex_set(unit, port, duplex));
                                             
    SOC_IF_ERROR_RETURN(MAC_INIT(macd, unit, port));

    /* restore MAC setting */
    SOC_IF_ERROR_RETURN(MAC_PAUSE_SET(macd, unit, port, pause_tx, pause_rx));
    SOC_IF_ERROR_RETURN(MAC_LOOPBACK_SET(macd, unit, port, loopback));
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, enable));

    /* Reset and Refresh counters */
    SOC_PBMP_CLEAR(ctr_pbmp);
    SOC_PBMP_PORT_SET(ctr_pbmp, port);
    COMPILER_64_SET(val64, 0, 0);
    SOC_IF_ERROR_RETURN(soc_counter_set_by_port(unit, ctr_pbmp, val64));
    soc_counter_sync(unit); 

    return SOC_E_NONE;
}

/**************************************************
 * @function soc_triumph_l2_entry_limit_count_update
 *
 *
 * @purpose Read L2 table from hardware, calculate
 *          and restore port/trunk and vlan/vfi
 *          mac counts.
 *
 * @param  unit   @b{(input)}  unit number
 *
 * @returns BCM_E_NONE
 * @returns BCM_E_FAIL
 *
 * @comments This rountine gets called for an
 *           L2 table SER event, if any of the
 *           mac limits(system, port, vlan) are
 *           enabled on the device.
 *
 * @end
 */

int
soc_triumph_l2_entry_limit_count_update(int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    uint32 *v_entry;     /* vlan or vfi count entry */
    uint32 *p_entry;     /* port or trunk count entry */
    uint32 *l2;          /* l2 entry */
    int32 v_index = -1;  /* vlan index */
    int32 p_index = -1;  /* port index */
    uint32 s_count = 0;  /* system mac count */
    uint32 p_count = 0;  /* port or trunk mac count */
    uint32 v_count = 0;  /* vlan or vfi mac count */

    /* l2 table */
    int index_min, index_max;
    int entry_dw, entry_sz;
    int chnk_idx, chnk_idx_max, table_chnk_sz, idx;
    int chunk_size = 1024;

    /* port or trunk mac count table */
    int ptm_index_min, ptm_index_max;
    int ptm_table_sz, ptm_entry_dw, ptm_entry_sz;

    /* vlan or vfi mac count table */
    int vvm_index_min, vvm_index_max;
    int vvm_table_sz, vvm_entry_dw, vvm_entry_sz;

    uint32 *buf = NULL;
    uint32 *ptm_buf = NULL;
    uint32 *vvm_buf = NULL;

    uint32 dest_type, key_type;
    uint32 is_valid, is_limit_counted = 0;
    uint32 is_free;
    soc_mem_t mem;
    int rv;

    p_index = -1;
    v_index = -1;


    /* If MAC learn limit not enabled do nothing */
    SOC_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    if (!soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr,
        rval, ENABLEf)) {
        return SOC_E_NONE;
    }


    /*
     * Mac limits are enabled.
     * 1. Read (DMA) L2 table to recalculate mac count
     * 2. Iterate through all the entries.
     * 3. if the entry is limit_counted
     * Check if KEY_TYPE is BRIDGE, VLAN or VFI
     * based on DEST_TYPE determine if Trunk or ModPort
     * Get port index into port_or_trunk_mac_count table
     * Get vlan/vfi index into vlan_or_vfi_mac_count table
     * update the port, vlan/vfi, system mac count.
     * Write(slam) port_or_trunk_mac count and
     * vlan_or_vfi_mac_count tables.
     */

    mem = L2Xm;
    soc_mem_lock(unit, mem);

    /*
     * Allocate memory for port/trunk mac count table to store
     * the updated count.
     */
    ptm_entry_dw = soc_mem_entry_words(unit, PORT_OR_TRUNK_MAC_COUNTm);
    ptm_entry_sz = ptm_entry_dw * sizeof(uint32);
    ptm_index_min = soc_mem_index_min(unit, PORT_OR_TRUNK_MAC_COUNTm);
    ptm_index_max = soc_mem_index_max(unit,PORT_OR_TRUNK_MAC_COUNTm);
    ptm_table_sz = (ptm_index_max - ptm_index_min + 1) * ptm_entry_sz;

    ptm_buf = soc_cm_salloc(unit, ptm_table_sz, "ptm_tmp");

    if (ptm_buf == NULL) {
        soc_mem_unlock(unit, mem);
        return SOC_E_MEMORY;
    }

    sal_memset(ptm_buf, 0, ptm_table_sz);

    /*
     * Allocate memory for vlan/vfi mac count table to store
     * the updated count.
     */
    vvm_entry_dw = soc_mem_entry_words(unit, VLAN_OR_VFI_MAC_COUNTm);
    vvm_entry_sz = vvm_entry_dw * sizeof(uint32);
    vvm_index_min = soc_mem_index_min(unit, VLAN_OR_VFI_MAC_COUNTm);
    vvm_index_max = soc_mem_index_max(unit,VLAN_OR_VFI_MAC_COUNTm);
    vvm_table_sz = (vvm_index_max - vvm_index_min + 1) * vvm_entry_sz;

    vvm_buf = soc_cm_salloc(unit, vvm_table_sz, "vvm_tmp");

    if (vvm_buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        soc_mem_unlock(unit, mem);
        return SOC_E_MEMORY;
    }

    sal_memset(vvm_buf, 0, vvm_table_sz);

    /* Create a copy L2_ENTRY_1m table for calculating mac count */
    entry_dw = soc_mem_entry_words(unit, mem);
    entry_sz = entry_dw * sizeof(uint32);
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    table_chnk_sz = chunk_size * entry_sz;

    buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");

    if (buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        soc_mem_unlock(unit, mem);
        return SOC_E_MEMORY;
    }

    for (chnk_idx = index_min; chnk_idx <= index_max; chnk_idx += chunk_size) {

         sal_memset(buf, 0, table_chnk_sz);

         chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                (chnk_idx + chunk_size - 1) : index_max;

         /* DMA L2 Table */
         rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx,
                                  chnk_idx_max, buf);
         if (rv < 0) {
             soc_cm_sfree(unit, buf);
             soc_cm_sfree(unit, ptm_buf);
             soc_cm_sfree(unit, vvm_buf);
             soc_mem_unlock(unit, mem);
             return SOC_E_FAIL;
         }

         /* Iter through all l2 entries in the chunk */
         for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
              l2 =  (buf + (idx * entry_dw));

              /* check if entry is valid */
              is_valid = soc_mem_field32_get(unit, mem, l2, VALIDf);
              if (!is_valid) {
                  /* not valid entry skip it */
                  continue;
              }

              /* check if entry is limit counted */
              if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
                  is_limit_counted = soc_mem_field32_get(unit,mem, l2,
                                                         LIMIT_COUNTEDf);
                  if (!is_limit_counted) {
                      /* entry not limit counted skip it */
                      continue;
                  }
              }

              /*
               * Get the key type of the entries
               * Process entries with only key_types
               * 1. SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE
               * 2. SOC_MEM_KEY_L2_ENTRY_1_L2_VFI
               */
              key_type = soc_mem_field32_get(unit, mem, l2, KEY_TYPEf);

              if ((key_type != TR_L2_HASH_KEY_TYPE_BRIDGE) &&
                  (key_type != TR_L2_HASH_KEY_TYPE_VFI)) {
                  continue;
              }

              dest_type = soc_mem_field32_get(unit, mem, l2, DEST_TYPEf);

              switch (dest_type) {
                  case 0: /* mod port */
                      rv = soc_mem_read(unit, PORT_TABm,
                                                       MEM_BLOCK_ANY, 0,
                                                       &entry);
                      if (SOC_FAILURE(rv)) {
                          soc_cm_sfree(unit, buf);
                          soc_cm_sfree(unit, ptm_buf);
                          soc_cm_sfree(unit, vvm_buf);
                          soc_mem_unlock(unit, mem);
                          return rv;
                      }
                      if (soc_mem_field32_get(unit, PORT_TABm,
                                              &entry, MY_MODIDf) ==
                          soc_mem_field32_get(unit, mem, l2, L2__MODULE_IDf)) {
                          p_index = soc_mem_field32_get(unit, mem,
                                                        l2, PORT_NUMf) +
                          soc_mem_index_count(unit, TRUNK_GROUPm) ;
                      }
                      break;
                  case 1: /* trunk */
                      p_index = soc_mem_field32_get(unit, mem, l2, L2__TGIDf);
                      break;
                  default:
                      /* if VFI based key then break else continue? */
                      break;
              }

              /*
               * based on key_type get vlan or vfi
               * to index into VLAN_OR_VFI_MAC_COUNTm
               */
              if (key_type == TR_L2_HASH_KEY_TYPE_BRIDGE) {
                  v_index = soc_mem_field32_get(unit, mem, l2, VLAN_IDf);
              } else {
                  v_index = soc_mem_field32_get(unit, mem, l2, VFIf) +
                               soc_mem_index_count(unit, VLAN_TABm);
              }


             /*
              * read and update vlan or vfi mac count
              * in buffer also update the sys mac count.
              */
             v_count = 0;
             if (v_index >= 0) {
                 v_entry =  (vvm_buf + (v_index * vvm_entry_dw));
                 v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                          v_entry, COUNTf);
                 s_count++;
                 v_count++;
                 soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm,
                                     v_entry, COUNTf, v_count);
             }

             /*
              * read and update port or trunk mac count
              * in buffer.
              */
             p_count = 0;
             if (p_index >= 0) {
                 p_entry = ptm_buf + (p_index * ptm_entry_dw);
                 p_count = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                               p_entry, COUNTf);
                 p_count++;
                 soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                     p_entry, COUNTf, p_count);
             }

         } /* End for index */
    } /* End for chnk_idx */

    /* Free memory */
    soc_cm_sfree(unit, buf);

    /* Work on EXT_L2_TABLE */
    if (soc_feature(unit, soc_feature_esm_support)) {
        soc_mem_unlock(unit, mem);
        /* Create a copy EXT_L2_ENTRYm table for calculating mac count */
        mem = EXT_L2_ENTRYm;
        soc_mem_lock(unit, mem);
        entry_dw = soc_mem_entry_words(unit, mem);
        entry_sz = entry_dw * sizeof(uint32);
        index_min = soc_mem_index_min(unit, mem);
        index_max = soc_mem_index_max(unit, mem);
        table_chnk_sz = chunk_size * entry_sz;

        buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");

        if (buf == NULL) {
            soc_cm_sfree(unit, ptm_buf);
            soc_cm_sfree(unit, vvm_buf);
            soc_mem_unlock(unit, mem);
            return SOC_E_MEMORY;
        }

        for (chnk_idx = index_min; chnk_idx <= index_max;
                                   chnk_idx += chunk_size) {

             sal_memset(buf, 0, table_chnk_sz);

             chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                    (chnk_idx + chunk_size - 1) : index_max;

             /* DMA L2 Table */
             rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx,
                                      chnk_idx_max, buf);
             if (rv < 0) {
                 soc_cm_sfree(unit, buf);
                 soc_cm_sfree(unit, ptm_buf);
                 soc_cm_sfree(unit, vvm_buf);
                 soc_mem_unlock(unit, mem);
                 return SOC_E_FAIL;
             }

             /* Iter through all l2 entries in the chunk */
             for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
                  l2 =  (buf + (idx * entry_dw));

                  /* check if entry is valid */
                  is_free = soc_mem_field32_get(unit, mem, l2, FREEf);
                  if (is_free) {
                      /* not valid entry skip it */
                      continue;
                  }

                  /* check if entry is limit counted */
                  if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
                      is_limit_counted = soc_mem_field32_get(unit,mem, l2,
                                                             LIMIT_COUNTEDf);
                      if (!is_limit_counted) {
                          /* entry not limit counted skip it */
                          continue;
                      }
                  }

                  /*
                   * Get the key type(KEY_TYPE_VFIf) of the entries
                   * 0 - VLAN_ID
                   * 0 - VFI
                   */
                  key_type = soc_mem_field32_get(unit, mem, l2, KEY_TYPE_VFIf);

                  /*
                   * based on key_type get vlan or vfi
                   * to index into VLAN_OR_VFI_MAC_COUNTm
                   */
                  if (key_type == 0) {
                      v_index = soc_mem_field32_get(unit, mem, l2, VLAN_IDf);
                  } else {
                      v_index = soc_mem_field32_get(unit, mem, l2, VFIf) +
                                   soc_mem_index_count(unit, VLAN_TABm);
                  }

                  dest_type = soc_mem_field32_get(unit, mem, l2, DEST_TYPEf);

                  switch (dest_type) {
                      case 0: /* mod port */
                          rv = soc_mem_read(unit, PORT_TABm,
                                                           MEM_BLOCK_ANY, 0,
                                                           &entry);
                          if (SOC_FAILURE(rv)) {
                              soc_cm_sfree(unit, buf);
                              soc_cm_sfree(unit, ptm_buf);
                              soc_cm_sfree(unit, vvm_buf);
                              soc_mem_unlock(unit, mem);
                              return rv;
                          }
                          if (soc_mem_field32_get(unit, PORT_TABm,
                                                  &entry, MY_MODIDf) ==
                              soc_mem_field32_get(unit, mem, l2, MODULE_IDf)) {
                              p_index = soc_mem_field32_get(unit, mem,
                                                            l2, PORT_NUMf) +
                                        soc_mem_index_count(unit, TRUNK_GROUPm) ;
                          }
                          break;
                      case 1: /* trunk */
                          p_index = soc_mem_field32_get(unit, mem,
                                                        l2, TGIDf);
                          break;
                      default:
                          break;
                  }


                 /*
                  * read and update vlan or vfi mac count
                  * in buffer also update the sys mac count.
                  */
                 v_count = 0;
                 if (v_index >= 0) {
                     v_entry =  (vvm_buf + (v_index * vvm_entry_dw));
                     v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                              v_entry, COUNTf);
                     s_count++;
                     v_count++;
                     soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm,
                                         v_entry, COUNTf, v_count);
                 }

                 /*
                  * read and update port or trunk mac count
                  * in buffer.
                  */
                 p_count = 0;
                 if (p_index >= 0) {
                     p_entry = ptm_buf + (p_index * ptm_entry_dw);
                     p_count = soc_mem_field32_get(unit,
                                                   PORT_OR_TRUNK_MAC_COUNTm,
                                                   p_entry, COUNTf);
                     p_count++;
                     soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                         p_entry, COUNTf, p_count);
                 }

             } /* End for index */
        } /* End for chnk_idx */

        /* Free memory */
        soc_cm_sfree(unit, buf);

    } /* End if soc_feature_esm_support */

    /* Write the tables to hardware */
     rv = soc_mem_write_range(unit, PORT_OR_TRUNK_MAC_COUNTm, MEM_BLOCK_ANY,
                              ptm_index_min, ptm_index_max, (void *)ptm_buf);

    if (rv < 0) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        soc_mem_unlock(unit, mem);
        return SOC_E_FAIL;
    }

    rv = soc_mem_write_range(unit, VLAN_OR_VFI_MAC_COUNTm, MEM_BLOCK_ANY,
                             vvm_index_min, vvm_index_max, (void *)vvm_buf);

    if (rv < 0) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        soc_mem_unlock(unit, mem);
        return SOC_E_FAIL;
    }

    /* Update system count */
    soc_reg_field_set(unit, SYS_MAC_COUNTr, &rval, COUNTf, s_count);
    rv = WRITE_SYS_MAC_COUNTr(unit, rval);

    /* Free memory */
    soc_cm_sfree(unit, ptm_buf);
    soc_cm_sfree(unit, vvm_buf);

    soc_mem_unlock(unit, mem);
    return rv;
}

#endif /* BCM_TRIUMPH_SUPPORT */
