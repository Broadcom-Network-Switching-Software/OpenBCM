/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane2.c
 * Purpose:
 * Requires:    
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/firebolt.h>
#include <soc/bradley.h>
#include <soc/hurricane2.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>
#include <soc/devids.h>
#include <soc/mspi.h>

#include <shared/util.h>
#include <shared/l3.h>
#include <soc/soc_ser_log.h>


#ifdef BCM_HURRICANE2_SUPPORT

#define SOC_MAX_PHY_PORTS            34

/*
 * Enduro chip driver functions.  
 */
soc_functions_t soc_hurricane2_drv_funs = {
    soc_hurricane2_misc_init,
    soc_hurricane2_mmu_init,
    soc_hurricane2_age_timer_get,
    soc_hurricane2_age_timer_max_get,
    soc_hurricane2_age_timer_set,
};

typedef enum {
    _SOC_PARITY_INFO_TYPE_GENERIC,
    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
    _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
    _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
    _SOC_PARITY_INFO_TYPE_MMU_PARITY,
    _SOC_PARITY_INFO_TYPE_MMU_IPMC,
    _SOC_PARITY_INFO_TYPE_MMU_E2EFC,
    _SOC_PARITY_INFO_TYPE_OAM, /* Not parity, but same interrupt */
    _SOC_PARITY_INFO_TYPE_NUM
} _soc_parity_info_type_t;

typedef struct _soc_parity_info_s {
    soc_field_t             enable_field;
    soc_field_t             error_field;
    char                    *msg;
    soc_mem_t               mem;
    _soc_parity_info_type_t type;
    soc_reg_t               control_reg;
    soc_reg_t               intr_status0_reg;
    soc_reg_t               intr_status1_reg; /* Also SBE force for ECC */
    soc_reg_t               nack_status0_reg;
    soc_reg_t               nack_status1_reg; /* Also DBE force for ECC */
} _soc_parity_info_t;

/*
 *    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY
 *      PARITY_EN
 *      ENTRY_IDX, MULTIPLE_ERR, PARITY_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_SINGLE_ECC
 *      ECC_EN
 *      ENTRY_IDX, DOUBLE_BIT_ERR, MULTIPLE_ERR, ECC_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER
 *      PARITY_EN
 *      PORT_IDX, COUNTER_IDX, MULTIPLE_ERR, PARITY_ERR
 *
 *    _SOC_PARITY_INFO_TYPE_DUAL_PARITY
 *      PARITY_EN
 *      BUCKET_IDX, MULTIPLE_ERR, PARITY_ERR_BM
 */

STATIC _soc_parity_info_t _soc_hu2_ip0_parity_info[] = {
    { PARITY_ENf, VXLT_PAR_ERRf,"VLAN_XLATE table parity error",
      VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      VLAN_XLATE_PARITY_CONTROLr,
      VLAN_XLATE_PARITY_STATUS_0r, VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
 /* { PPA_CMD_COMPLETEf }, */
 /* { MEM_RESET_COMPLETEf }, */
 /* { AGE_CMD_COMPLETEf }, */
    { PARITY_ENf, VLAN_SUBNET_DATA_PARITY_ERRf, "VLAN_SUBNET_DATA table parity error",
      VLAN_SUBNET_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_SUBNET_DATA_PARITY_CONTROLr,
      VLAN_SUBNET_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PROTOCOL_DATA_PARITY_ERRf, "VLAN_PROTOCOL_DATA table parity error",
      VLAN_PROTOCOL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PROTOCOL_DATA_PARITY_CONTROLr,
      VLAN_PROTOCOL_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PARITY_ERRf, "VLAN table parity error",
      VLAN_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PARITY_CONTROLr,
      VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_STG_PARITY_ERRf, "VLAN_STG table parity error",
      STG_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_STG_PARITY_CONTROLr,
      VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { PARITY_ENf, L3_TUNNEL_RAM_PARITY_ERRf, "L3_TUNNEL_RAM table parity error",
      L3_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_TUNNEL_DATA_ONLY_PARITY_CONTROLr,
      L3_TUNNEL_DATA_ONLY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, SOURCE_TRUNK_MAP_PARITY_ERRf, "SOURCE_TRUNK_MAP table parity error",
      SOURCE_TRUNK_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_TRUNK_MAP_PARITY_CONTROLr,
      SOURCE_TRUNK_MAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VFP_POLICY_PAR_ERRf, "VFP_POLICY table parity error",
      VFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFP_POLICY_PARITY_CONTROLr,
      VFP_POLICY_PARITY_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, LMEP_PAR_ERRf, "LMEP table parity error",
      LMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      LMEP_PARITY_CONTROLr,
      LMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, IARB_PKT_ECC_INTRf, "Iarb packet ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_PKT_ECC_CONTROLr,
      IARB_PKT_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    /* IARB_HDR_ECC_INTR, no control/status regiters */
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_wf_ip0_parity_info[] = {
    { PARITY_ENf, VXLT_PAR_ERRf,"VLAN_XLATE table parity error",
      VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      VLAN_XLATE_PARITY_CONTROLr,
      VLAN_XLATE_PARITY_STATUS_0r, VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
 /* { PPA_CMD_COMPLETEf }, */
 /* { MEM_RESET_COMPLETEf }, */
 /* { AGE_CMD_COMPLETEf }, */
    { PARITY_ENf, VLAN_SUBNET_DATA_PARITY_ERRf, "VLAN_SUBNET_DATA table parity error",
      VLAN_SUBNET_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_SUBNET_DATA_PARITY_CONTROLr,
      VLAN_SUBNET_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PROTOCOL_DATA_PARITY_ERRf, "VLAN_PROTOCOL_DATA table parity error",
      VLAN_PROTOCOL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PROTOCOL_DATA_PARITY_CONTROLr,
      VLAN_PROTOCOL_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PARITY_ERRf, "VLAN table parity error",
      VLAN_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PARITY_CONTROLr,
      VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_STG_PARITY_ERRf, "VLAN_STG table parity error",
      STG_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_STG_PARITY_CONTROLr,
      VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { PARITY_ENf, L3_TUNNEL_RAM_PARITY_ERRf, "L3_TUNNEL_RAM table parity error",
      L3_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_TUNNEL_DATA_ONLY_PARITY_CONTROLr,
      L3_TUNNEL_DATA_ONLY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, SOURCE_TRUNK_MAP_PARITY_ERRf, "SOURCE_TRUNK_MAP table parity error",
      SOURCE_TRUNK_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_TRUNK_MAP_PARITY_CONTROLr,
      SOURCE_TRUNK_MAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, LMEP_PAR_ERRf, "LMEP table parity error",
      LMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      LMEP_PARITY_CONTROLr,
      LMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, IARB_PKT_ECC_INTRf, "Iarb packet ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_PKT_ECC_CONTROLr,
      IARB_PKT_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    /* IARB_HDR_ECC_INTR, no control/status regiters */
    { INVALIDf, INVALIDf }, /* table terminator */
};


STATIC _soc_parity_info_t _soc_hu2_ip1_parity_info[] = {
    /* { PARITY_ENf, VFI_PAR_ERRf, "VFI table parity error.",
      VFIm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_PARITY_CONTROLr,
      VFI_PARITY_STATUS_INTRr, INVALIDr,
      VFI_PARITY_STATUS_NACKr, INVALIDr }, */
    /* { PARITY_ENf, SVP_PAR_ERRf, "SOURCE_VP table parity error",
      SOURCE_VPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_VP_PARITY_CONTROLr,
      SOURCE_VP_PARITY_STATUSr, INVALIDr,
      SOURCE_VP_PARITY_STATUS_NACKr, INVALIDr }, */
    /* { PARITY_ENf, L3_IIF_PAR_ERRf, "L3_IIF table parity error",
      L3_IIFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_IIF_PARITY_CONTROLr,
      L3_IIF_PARITY_STATUSr, INVALIDr,
      L3_IIF_PARITY_STATUS_NACKr, INVALIDr }, */
    /* { PARITY_ENf, MPLS_ENTRY_PAR_ERRf, "MPLS_ENTRY table parity error",
      MPLS_ENTRYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      MPLS_ENTRY_PARITY_CONTROLr,
      MPLS_ENTRY_PARITY_STATUS_INTR_0r, MPLS_ENTRY_PARITY_STATUS_INTR_1r,
      MPLS_ENTRY_PARITY_STATUS_NACK_0r, MPLS_ENTRY_PARITY_STATUS_NACK_1r }, */
    { PARITY_ENf, L2_ENTRY_PAR_ERRf, "L2_ENTRY table parity error",
      L2Xm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L2_ENTRY_PARITY_CONTROLr,
      L2_ENTRY_PARITY_STATUS_0r, L2_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L2MC_PAR_ERRf, "L2MC table parity error",
      L2MCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L2MC_PARITY_CONTROLr,
      L2MC_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_ENTRY_PAR_ERRf, "L3_ENTRY table parity error",
      L3_ENTRY_ONLYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L3_ENTRY_PARITY_CONTROLr,
      L3_ENTRY_PARITY_STATUS_0r, L3_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_DEFIP_DATA_PAR_ERRf, "L3_DEFIP_DATA_ONLY table parity error",
      L3_DEFIP_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_DEFIP_PARITY_CONTROLr,
      L3_DEFIP_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3MC_PAR_ERRf, "L3MC table parity error",
      L3_IPMCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3MC_PARITY_CONTROLr,
      L3MC_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MA_STATE_PAR_ERRf, "MA_STATE table parity error",
      MA_STATEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_STATE_PARITY_CONTROLr,
      MA_STATE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, RMEP_PAR_ERRf, "RMEP table parity error",
      RMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      RMEP_PARITY_CONTROLr,
      RMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MAID_REDUCTION_PAR_ERRf, "MAID_REDUCTION table parity error",
      MAID_REDUCTIONm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MAID_REDUCTION_PARITY_CONTROLr,
      MAID_REDUCTION_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MA_INDEX_PAR_ERRf, "MA_INDEX table parity error",
      MA_INDEXm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_INDEX_PARITY_CONTROLr,
      MA_INDEX_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { PARITY_ENf, PORT_CBL_TABLE_PAR_ERRf, "PORT_CBL_TABLE table parity error",
      PORT_CBL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      PORT_CBL_TABLE_PARITY_CONTROLr,
      PORT_CBL_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, unsupported */
    { PARITY_ENf, INITIAL_NHOP_PAR_ERRf, "INITIAL_NHOP table parity error",
      INITIAL_ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      INITIAL_NHOP_PARITY_CONTROLr,
      INITIAL_NHOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* Start of parity-unrelated OAM faults */

    { ANY_RMEP_TLV_PORT_DOWN_INTRf, ANY_RMEP_TLV_PORT_DOWN_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_PORT_UP_INTRf, ANY_RMEP_TLV_PORT_UP_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ANY_RMEP_TLV_INTERFACE_UP_INTRf, ANY_RMEP_TLV_INTERFACE_UP_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { XCON_CCM_DEFECT_INTRf, XCON_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { ERROR_CCM_DEFECT_INTRf, ERROR_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { SOME_RDI_DEFECT_INTRf, SOME_RDI_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    { SOME_RMEP_CCM_DEFECT_INTRf, SOME_RMEP_CCM_DEFECT_INTRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_OAM,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },

    /* End of parity-unrelated OAM faults */
    /* { PARITY_ENf, VFI_1_PAR_INTRf, "VFI_1 parity interrupt",
      VFI_1m, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_1_PARITY_CONTROLr,
      VFI_1_PARITY_STATUS_INTRr, INVALIDr,
      VFI_1_PARITY_STATUS_NACKr, INVALIDr }, */
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu2_ip2_parity_info[] = {
    { PARITY_ENf, ING_NHOP_PAR_ERRf, "ING_L3_NEXT_HOP table parity error",
      ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ING_L3_NEXT_HOP_PARITY_CONTROLr,
      ING_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_METER_PAR_ERRf, "FP_METER_TABLE table parity error",
      FP_METER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_METER_PARITY_CONTROLr,
      IFP_METER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_COUNTER_PAR_ERRf, "FP_COUNTER_TABLE table parity error",
      FP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_COUNTER_PARITY_CONTROLr,
      IFP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_POLICY_PAR_ERRf, "FP_POLICY_TABLE table parity error",
      FP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_POLICY_PARITY_CONTROLr,
      IFP_POLICY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_STORM_CONTROL_PAR_ERRf, "FP_STORM_CONTROL_METERS table parity error",
      FP_STORM_CONTROL_METERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_STORM_CONTROL_PARITY_CONTROLr,
      IFP_STORM_CONTROL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_MTU_VALUES_PAR_ERRf, "L3_MTU_VALUES table parity error",
      L3_MTU_VALUESm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_MTU_VALUES_PARITY_CONTROLr,
      L3_MTU_VALUES_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_MASK_PAR_ERRf, "EGR_MASK table parity error",
      EGR_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MASK_PARITY_CONTROLr,
      EGR_MASK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_SW_PAR_ERRf, "MODPORT_MAP table parity error",
      MODPORT_MAP_SWm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_SW_PARITY_CONTROLr,
      MODPORT_MAP_SW_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },   
    { PARITY_ENf, MODPORT_MAP_IM_PAR_ERRf, "MODPORT_MAP_IM table parity error",
      MODPORT_MAP_IMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_IM_PARITY_CONTROLr,
      MODPORT_MAP_IM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_EM_PAR_ERRf, "MODPORT_MAP_EM table parity error",
      MODPORT_MAP_EMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_EM_PARITY_CONTROLr,
      MODPORT_MAP_EM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, OAM_LM_COUNTERS_PAR_ERRf, "OAM_LM_COUNTERS table parity error.",
      OAM_LM_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      OAM_LM_COUNTERS_PARITY_CONTROLr,
      OAM_LM_COUNTERS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu2_ep_parity_info[] = {
    { PARITY_ENf, EGR_NHOP_PAR_ERRf, NULL,
      EGR_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_L3_NEXT_HOP_PARITY_CONTROLr,
      EGR_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_L3_INTF_PAR_ERRf, NULL,
      EGR_L3_INTFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_L3_INTF_PARITY_CONTROLr,
      EGR_L3_INTF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_VLAN_PAR_ERRf, NULL,
      EGR_VLANm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_VLAN_PARITY_CONTROLr,
      EGR_VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
   { PARITY_ENf, EGR_VLAN_STG_PAR_ERRf, NULL,
      EGR_VLAN_STGm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_VLAN_STG_PARITY_CONTROLr,
      EGR_VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_VXLT_PAR_ERRf, NULL,
      EGR_VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      EGR_VLAN_XLATE_PARITY_CONTROLr,
      EGR_VLAN_XLATE_PARITY_STATUS_0r, EGR_VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { STATS_PAR_ENf, EGR_STATS_COUNTER_TABLE_PAR_ERRf, "Parity enable for egress stats counter memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_STATS_COUNTER_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, EP_INITBUF_DBEf, "EP_INITBUF double-bit ECC error",
      INVALIDm,_SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_DBEr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, EP_INITBUF_SBEf, "EP_INITBUF single-bit ECC error (corrected)",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      EGR_INITBUF_ECC_CONTROLr,
      EGR_INITBUF_ECC_STATUS_SBEr, INVALIDr,
      INVALIDr, INVALIDr },
    { PERQ_PAR_ENf, EGR_PERQ_COUNTERf, "Parity enable for egress perq xmt counter memory",
      EGR_PERQ_XMT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_PERQ_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { EFPCTR_PAR_ENf, EGR_FP_COUNTERf, "Parity enable for efp counter memory",
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_FP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { GP0_ECC_ENf, EGR_GP0_DBUFf, "ECC enable for gp0 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_GP0_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { GP1_ECC_ENf, EGR_GP1_DBUFf, "ECC enable for gp1 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_GP1_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { GP2_ECC_ENf, EGR_GP2_DBUFf, "ECC enable for gp2 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_GP2_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP0_ECC_ENf, EGR_XP0_DBUFf, "ECC enable for xp0 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP0_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP1_ECC_ENf, EGR_XP1_DBUFf, "ECC enable for xp1 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP1_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP2_ECC_ENf, EGR_XP2_DBUFf, "ECC enable for xp2 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP2_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP3_ECC_ENf, EGR_XP3_DBUFf, "ECC enable for xp3 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP3_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP4_ECC_ENf, EGR_XP4_DBUFf, "ECC enable for xp4 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP4_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP5_ECC_ENf, EGR_XP5_DBUFf, "ECC enable for xp5 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP5_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP6_ECC_ENf, EGR_XP6_DBUFf, "ECC enable for xp6 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP6_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { XP7_ECC_ENf, EGR_XP7_DBUFf, "ECC enable for xp7 interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_XP7_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    /* { GP_RESI_ECC_ENf, EGR_GP_RESI_DBUFf, "ECC enable for gp residue buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_GP_RESI_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { CM_ECC_ENf, EGR_CM_DBUFf, "ECC enable for cm interaface buffer",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_EDATABUF_PARITY_CONTROLr,
      EGR_CM_DBITS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu2_xlp_parity_info[] = {
    { TXFIFO_ECC_ENf, TXFIFO0_ERRf, "TXFIFO 0",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_TXFIFO0_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO1_ERRf, "TXFIFO 1",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_TXFIFO1_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO2_ERRf, "TXFIFO 2",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_TXFIFO2_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { TXFIFO_ECC_ENf, TXFIFO3_ERRf, "TXFIFO 3",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_TXFIFO3_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { BOD_ECC_ENABLEf, BOD_XLP0_ERRf, "Cell Assembly FIFO",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_BOD_XLP0_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    {  MIB_RSC_MEM_ENf, MIB_RSC1_MEM_ERRf, 
      "MIB RX Statistic counter memory instance 1",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_MIB_RSC1_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { MIB_RSC_MEM_ENf, MIB_RSC0_MEM_ERRf, 
      "MIB RX Statistic counter memory instance 0",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_MIB_RSC0_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { MIB_TSC_MEM_ENf, MIB_TSC1_MEM_ERRf, 
      "MIB TX Statistic counter memory instance 1",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_MIB_TSC1_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { MIB_TSC_MEM_ENf, MIB_TSC0_MEM_ERRf, 
      "MIB TX Statistic counter memory instance 0",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XLPORT_ECC_CONTROLr,
      XLPORT_MIB_TSC0_ECC_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu2_mmu_parity_info[] = {
    { CELLECCERRORINTMASKf, CELLECCERRORf, "MMU CBP cell data",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr},
    { CBPPKTHDRPARITYERRORINTMASKf, CBPPKTHDRPARITYERRORf, 
      "MMU CBP packet header memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr}, 
    { CBPCELLHDRPARITYERRORINTMASKf, CBPCELLHDRPARITYERRORf, 
      "MMU CBP cell header memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr},
    { XQPARITYERRORINTMASKf, XQPARITYERRORf, 
      "MMU XQ memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { CFAPPARITYERRORINTMASKf, CFAPPARITYERRORf, 
      "MMU CFP memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr},
    { CCPPARITYERRORINTMASKf, CCPPARITYERRORf, 
      "MMU CCP memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr},
    { IPMCREPOVERLIMITERRORINTMASKf, IPMCREPOVERLIMITERRORf, 
      "IPMC replication over limit error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr},
    { XQFLLPARITYERRORINTMASKf, XQFLLPARITYERRORf, 
      "MMU XQ FLL memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr},
    { E2EFCPARITYERRORINTMASKf, E2EFCPARITYERRORf, 
      "MMU E2EFC memory",
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMU_E2EFC,
      INVALIDr,
      E2EFC_PARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr},
    { IPMCVALNXPORTPARITYERRORINTMASKf, IPMCVLANXPORTPARITYERRORf, 
      "MMU IPMC VLAN XPORT memory",
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_MMU_IPMC,
      MEMFAILINTMASKr,
      IPMCPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr},
     /* { IPMCVALNGPORTPARITYERRORINTMASKf, IPMCVLANGPORTPARITYERRORf, 
      "MMU IPMC VLAN GPORT memory",
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_MMU_IPMC,
      MEMFAILINTMASKr,
      IPMCPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr}, */
    { IPMCGROUP0PARITYERRORINTMASKf, IPMCGROUP0PARITYERRORf, 
      "MMU IPMC GORUP0 memory",
      MMU_IPMC_GROUP_TBL0m, _SOC_PARITY_INFO_TYPE_MMU_IPMC,
      INVALIDr,
      IPMCPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr},
    { IPMCGROUP1PARITYERRORINTMASKf, IPMCGROUP1PARITYERRORf, 
      "MMU IPMC GORUP1 memory",
      MMU_IPMC_GROUP_TBL1m, _SOC_PARITY_INFO_TYPE_MMU_IPMC,
      INVALIDr,
      IPMCPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr},
    { INVALIDf, INVALIDf }, /* table terminator */
};


typedef struct {
    uint32             cpi_bit;
    soc_block_t        blocktype;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    _soc_parity_info_t *info;
} _soc_hu2_parity_group_info_t;

static _soc_hu2_parity_group_info_t _soc_hu2_parity_group_info_template[] = {
    { 0x00001, SOC_BLK_MMU, MEMFAILINTMASKr, MEMFAILINTSTATUSr, _soc_hu2_mmu_parity_info },
    { 0x00002, SOC_BLK_EPIPE, EGR_INTR_ENABLEr, EGR_INTR_STATUSr, _soc_hu2_ep_parity_info },
    { 0x00004, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, _soc_hu2_ip0_parity_info },
    { 0x00008, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, IP1_INTR_STATUSr, _soc_hu2_ip1_parity_info },
    { 0x00010, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, IP2_INTR_STATUSr, _soc_hu2_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { 0x00040, SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, _soc_hu2_xlp_parity_info }, /* x4 */
    { 0 }, /* table terminator */
};
static _soc_hu2_parity_group_info_t _soc_wf_parity_group_info_template[] = {
    { 0x00001, SOC_BLK_MMU, MEMFAILINTMASKr, MEMFAILINTSTATUSr, _soc_hu2_mmu_parity_info },
    { 0x00002, SOC_BLK_EPIPE, EGR_INTR_ENABLEr, EGR_INTR_STATUSr, _soc_hu2_ep_parity_info },
    { 0x00004, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, _soc_wf_ip0_parity_info },
    { 0x00008, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, IP1_INTR_STATUSr, _soc_hu2_ip1_parity_info },
    { 0x00010, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, IP2_INTR_STATUSr, _soc_hu2_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { 0x00040, SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, _soc_hu2_xlp_parity_info }, /* x4 */
    { 0 }, /* table terminator */
};


STATIC _soc_generic_ser_info_t *_soc_hu2_tcam_ser_info = NULL;
STATIC _soc_hu2_parity_group_info_t *_soc_hu2_parity_group_info = NULL;
STATIC soc_hurricane2_oam_handler_t hu2_oam_handler[SOC_MAX_NUM_DEVICES] = {NULL};
static soc_hurricane2_oam_ser_handler_t hu2_oam_ser_handler[SOC_MAX_NUM_DEVICES] = {NULL};


static soc_ser_functions_t _hu2_ser_functions;

#define _SOC_HURRICANE2_SER_REG 1
#define _SOC_HURRICANE2_SER_MEM 0

typedef union _hurricane2_ser_nack_reg_mem_u {
    soc_reg_t reg;
    soc_mem_t mem;
} _hurricane2_ser_nack_reg_mem_t;


STATIC int
_soc_hurricane2_parity_block_port(int unit, soc_block_t block, soc_port_t *port)
{
    *port = SOC_BLOCK_PORT(unit, block);
    if ((*port != REG_PORT_ANY) && !SOC_PORT_VALID(unit, *port)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

soc_field_t _soc_hu2_oam_interrupt_fields[] = {
    SOME_RDI_DEFECT_INTRf,
    SOME_RMEP_CCM_DEFECT_INTRf,
    ERROR_CCM_DEFECT_INTRf,
    ANY_RMEP_TLV_PORT_DOWN_INTRf,
    ANY_RMEP_TLV_PORT_UP_INTRf,
    ANY_RMEP_TLV_INTERFACE_DOWN_INTRf,
    ANY_RMEP_TLV_INTERFACE_UP_INTRf,
    XCON_CCM_DEFECT_INTRf,
    INVALIDf
};

STATIC int
_soc_hu2_process_oam_interrupt(int unit)
{
    uint32 rval;
    int found = 0, fidx = 0;
    soc_hurricane2_oam_handler_t oam_handler_snapshot = hu2_oam_handler[unit];


    SOC_IF_ERROR_RETURN(READ_IP1_INTR_STATUSr(unit, &rval));
    
    while (_soc_hu2_oam_interrupt_fields[fidx] != INVALIDf) {
        if (soc_reg_field_get(unit, IP1_INTR_STATUSr, rval,
                          _soc_hu2_oam_interrupt_fields[fidx])) {
        
            if (oam_handler_snapshot != NULL)
            {
                (void)(oam_handler_snapshot(unit,
                           _soc_hu2_oam_interrupt_fields[fidx]));
            }
            found++;
        }
        fidx++;
    }
    if (!found) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Unexpected interrupt received for OAM !!\n")));
    }
    return SOC_E_NONE;
}

void
soc_hurricane2_oam_ser_handler_register(int unit, 
                                soc_hurricane2_oam_ser_handler_t handler) 
{
    hu2_oam_ser_handler[unit] = handler;
}

int
soc_hurricane2_oam_ser_process(int unit, soc_mem_t mem, int index) 
{
    if (hu2_oam_ser_handler[unit]) {
        return hu2_oam_ser_handler[unit](unit, mem, index);
    } else {
        return SOC_E_UNAVAIL;
    }
}


STATIC int
_soc_hurricane2_parity_enable(int unit, int enable)
{
    int group, table;
    uint32 group_enable, regval, cmic_enable = 0, cpi_blk_bit;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    /* loop through each group */
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;
        group_reg = _soc_hu2_parity_group_info[group].enable_reg;
        group_enable = 0;

        cpi_blk_bit = _soc_hu2_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu2_parity_group_info[group].blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                case _SOC_PARITY_INFO_TYPE_MMU_IPMC:
                case _SOC_PARITY_INFO_TYPE_MMU_E2EFC:
                case _SOC_PARITY_INFO_TYPE_OAM:
                    enable_field = info[table].enable_field;
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,  block_port, 0, &regval));
                    soc_reg_field_set(unit, reg, &regval,
                                      info[table].enable_field,
                                      enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,  block_port, 0, regval));
                    enable_field = info[table].error_field;
                    break;
                default:
                    enable_field = INVALIDf;
                    break;
                } /* handle different parity error reporting style */
                if (enable_field != INVALIDf) {
                    soc_reg_field_set(unit, group_reg, &group_enable,
                                      enable_field, enable ? 1 : 0);
                }
            } /* loop through each table in the group */

            if (!SOC_REG_IS_VALID(unit, group_reg)) {
                cpi_blk_bit <<= 1;
                continue;
            }
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, group_reg, block_port, 0, group_enable));
            cmic_enable |= cpi_blk_bit;
            cpi_blk_bit <<= 1;
        }
    } /* loop through each group */

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
    if (enable) {
        /* MMU enables */
        soc_reg_field_set(unit, MISCCONFIGr, &regval, PARITY_CHECK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_GEN_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
        /* Write CMIC enable register */
        (void)soc_cmicm_intr2_enable(unit, cmic_enable);
    } else {
        /* MMU disables */
        soc_reg_field_set(unit, MISCCONFIGr, &regval, PARITY_CHECK_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_GEN_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
        /* Write CMIC disable register */
        (void)soc_cmicm_intr2_disable(unit, cmic_enable);
    }
    return SOC_E_NONE;
}

/* SER processing for TCAMs */
STATIC _soc_generic_ser_info_t _soc_hu2_tcam_ser_info_template[] = {
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2, 
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0, 0 },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0, 0 },
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 37}, {1, 37}, {38, 74}, {39, 74} }, 0, 0, 0, 0, 0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0, 0 },
    { VLAN_SUBNET_ONLYm, VLAN_SUBNETm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0, 0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0, 0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0, 0 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0, 0 },
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_1BIT,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0, 0 },
    { INVALIDm },
};

/* Remove the VFP_TCAM and EFP_TCAM for Wolfhound */
STATIC _soc_generic_ser_info_t _soc_wf_tcam_ser_info_template[] = {
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0, 0 },
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, 
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 37}, {1, 37}, {38, 74}, {39, 74} }, 0, 0, 0, 0, 0 },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0, 0 },
    { VLAN_SUBNET_ONLYm, VLAN_SUBNETm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0, 0 },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS, 
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0, 0 },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0, 0 },
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_1BIT,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0, 0 },
    { INVALIDm },
};



STATIC int
_soc_hurricane2_tcam_ser_init(int unit)
{
    if (soc_feature(unit, soc_feature_field_multi_stage)) {
        _soc_hu2_tcam_ser_info = _soc_hu2_tcam_ser_info_template;
    } else {
        _soc_hu2_tcam_ser_info = _soc_wf_tcam_ser_info_template;
    }
    return soc_generic_ser_init(unit, _soc_hu2_tcam_ser_info);
}

void
soc_hurricane2_ser_fail(int unit)

{
    soc_generic_ser_process_error(unit, _soc_hu2_tcam_ser_info, 0);
}

int
soc_hurricane2_ser_mem_clear(int unit, soc_mem_t mem)
{
    uint32  range_enable;
    int info_ix, i;
    _soc_generic_ser_info_t *cur_spi;
    ser_memory_entry_t ser_mem;
    
    /* Check if enable */
    SOC_IF_ERROR_RETURN
        (READ_SER_RANGE_ENABLEr(unit, &range_enable));

    if (!range_enable) {
        return SOC_E_NONE;
    }

    info_ix = 0;

    while (_soc_hu2_tcam_ser_info[info_ix].mem != INVALIDm) {
        if (_soc_hu2_tcam_ser_info[info_ix].mem == mem) {
            break;
        }
        info_ix++;
    }

    if ((_soc_hu2_tcam_ser_info[info_ix].mem != INVALIDm) &&
        (range_enable & (1 << info_ix))) {
        cur_spi = &(_soc_hu2_tcam_ser_info[info_ix]);

        range_enable &= ~(1 << info_ix);
        /* Disable SER protection on this memory */
        SOC_IF_ERROR_RETURN
            (WRITE_SER_RANGE_ENABLEr(unit, range_enable));
        

        /* Flush SER memory segment for the table */
        sal_memset(&ser_mem, 0, sizeof(ser_mem));
        for (i = cur_spi->ser_section_start;
             i <= cur_spi->ser_section_end;
             i++) {
            SOC_IF_ERROR_RETURN
                    (WRITE_SER_MEMORYm(unit, MEM_BLOCK_ALL, i, &ser_mem));
        }

        range_enable |= (1 << info_ix);
        /* Enable SER protection on this memory */
        SOC_IF_ERROR_RETURN
            (WRITE_SER_RANGE_ENABLEr(unit, range_enable));

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "\t%s: SER[%d-%d]\n"),
                     SOC_MEM_NAME(unit, cur_spi->mem),
                     cur_spi->ser_section_start, cur_spi->ser_section_end));
    }

    return SOC_E_NONE;
    
}


STATIC void
_soc_hu2_mem_parity_info(int unit, int block_info_idx, int pipe,
                         soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | ((pipe & 0xff) << SOC_ERROR_PIPE_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}



int
_soc_hurricane2_mem_parity_control(int unit, soc_mem_t mem,
                                 int copyno, int enable)
{
    int group, table;
    uint32 cpi_blk_bit, regval;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    /* Convert component/aggregate memories to the table for which
     * the parity registers correspond. */
    switch(mem) {
    case VLAN_SUBNETm:
        mem = VLAN_SUBNET_DATA_ONLYm; /* Should be VLAN_SUBNET? */
        break;
    case L2_ENTRY_ONLYm:
        mem = L2Xm;
        break;
    case L2_USER_ENTRY_ONLYm:
    case L2_USER_ENTRY_DATA_ONLYm:
        mem = L2_USER_ENTRYm;
        break;
    case L3_DEFIPm:
        mem = L3_DEFIP_DATA_ONLYm;
        break;
    case L3_DEFIP_128m:
        mem = L3_DEFIP_128_DATA_ONLYm;
        break;
    case EGR_IP_TUNNEL_IPV6m:
    case EGR_IP_TUNNEL_MPLSm:
        mem = EGR_IP_TUNNELm;
        break;
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        mem = L3_ENTRY_ONLYm;
        break;
    case VLAN_MACm:
        mem = VLAN_XLATEm;
        break;
    default:
        /* Do nothing, keep memory as provided */
        break;
    }


    /* loop through each group */
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;
        group_reg = _soc_hu2_parity_group_info[group].enable_reg;
        cpi_blk_bit = _soc_hu2_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu2_parity_group_info[group].blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            if ((copyno != MEM_BLOCK_ANY) && (blk != copyno)) {
                cpi_blk_bit <<= 1;
                continue;
            }

            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                if (mem != info[table].mem) {
                    continue;
                }

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                case _SOC_PARITY_INFO_TYPE_MMU_IPMC:
                case _SOC_PARITY_INFO_TYPE_MMU_E2EFC:
                    enable_field = info[table].enable_field;
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, group_reg, block_port,
                                                enable_field,
                                                enable ? 1 : 0));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        return SOC_E_NONE;
                    }
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, reg, block_port,
                                                info[table].enable_field,
                                                enable ? 1 : 0));
                    break;
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
            cpi_blk_bit <<= 1;
        }
    } /* loop through each group */

    /* MMU controls */
    if (enable) {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));

        soc_reg_field_set(unit, MISCCONFIGr, &regval, PARITY_CHECK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_GEN_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
    } else {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
        soc_reg_field_set(unit, MISCCONFIGr, &regval, PARITY_CHECK_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_GEN_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &regval, E2EFC_PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, regval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane2_process_single_parity_error(int unit, int group,
                                          soc_port_t block_port, int table,
                                     int schan, char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val;
    int index, multiple, error;
    uint32 minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_hu2_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,status_reg,  block_port, 0, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERRf);
 
    if (error) {
        _soc_hu2_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               index, minfo);
        
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, index));
        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple parity errors\n"),
                       unit, msg));
        }

        if (info[table].mem != INVALIDm) {
            sal_memset(&spci, 0, sizeof(spci));
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;         
            spci.reg = INVALIDr;
            spci.mem = info[table].mem;
            spci.blk_type = -1;
            spci.index = index;
            (void)soc_ser_correction(unit, &spci);
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,status_reg,  block_port, 0, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_hurricane2_process_single_ecc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       int schan, char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val;
    int index, double_bit, multiple, error;
    uint32 minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_hu2_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,status_reg,  block_port, 0, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    double_bit = soc_reg_field_get(unit, status_reg,
                                   reg_val, DOUBLE_BIT_ERRf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, ECC_ERRf);
 
    if (error) {
        _soc_hu2_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                               index, minfo);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d %s ECC error\n"),
                   unit, msg, index, double_bit ? "double-bit" : ""));
        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple ECC errors\n"),
                       unit, msg));
        }
        sal_memset(&spci, 0, sizeof(spci));
        if (double_bit) {
            spci.double_bit = 1;
        }
        if (info[table].mem != INVALIDm) {
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info[table].mem;
            spci.blk_type = -1;
            spci.index = index;
            spci.parity_type = SOC_PARITY_TYPE_ECC;
            (void)soc_ser_correction(unit, &spci);
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,status_reg,  block_port, 0, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_hurricane2_process_dual_parity_error(int unit, int group,
                                        soc_port_t block_port, int table,
                                        int schan, char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 reg_val, bitmap = 0;
    int index, bucket_index = 0, multiple = 0, ix, bits, size = 0;
    _soc_ser_correct_info_t spci;
    uint32 minfo;

    info = _soc_hu2_parity_group_info[group].info;

    for (ix = 0; ix < 2; ix ++) {
        if (ix == 1) {
            status_reg = schan ? info[table].nack_status1_reg :
                info[table].intr_status1_reg;
        } else {
            status_reg = schan ? info[table].nack_status0_reg :
                info[table].intr_status0_reg;
        }

        if (status_reg == INVALIDr) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit,status_reg, 
                block_port, 0, &reg_val));
        if (soc_reg_field_valid(unit, status_reg, BUCKET_IDXf)) {
            bucket_index =
                soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDXf);
            multiple =
                soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
            bitmap =
                soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BMf);
            size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BMf);
        } else if (soc_reg_field_valid(unit, status_reg, BUCKET_IDX_0f)) {
            bucket_index =
            soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDX_0f);
            multiple =
                soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERR_0f);
            bitmap =
                soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BM_0f);
            size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BM_0f);         
        } else if (soc_reg_field_valid(unit, status_reg, BUCKET_IDX_1f)) {
            bucket_index =
                soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDX_1f);
            multiple =
               soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERR_1f);
            bitmap =
                soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BM_1f);
            size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BM_1f);
        }

        if (bitmap != 0) {
            for (bits = 0; bits < size; bits++) {
                if (bitmap & (1 << bits)) {
                    index =
                        bucket_index * size * 2 + bits + (ix * size);
                    _soc_hu2_mem_parity_info(unit, block, 0,
                                 info[table].error_field, &minfo);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               index, minfo);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d parity error\n"),
                               unit, msg, index));
                    if (info[table].mem != INVALIDm) {
                        sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
                        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                        spci.reg = INVALIDr;
                        spci.mem = info[table].mem;
                        spci.blk_type = block;
                        spci.index = index;
                        (void)soc_ser_correction(unit, &spci);
                    }
                }
            }
        }

        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple parity errors\n"),
                       unit, msg));
        }

        /* Clear parity status */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit,status_reg,  block_port, 0, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane2_process_mmu_e2efc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
#define _SOC_HU2_MMU_E2EFC_MAX     4
    static soc_reg_t e2efc_regs[_SOC_HU2_MMU_E2EFC_MAX] = {
          INVALIDr,  
          E2EFC_CNT_SET_LIMITr,
          E2EFC_CNT_RESET_LIMITr,
          E2EFC_CNT_DISC_LIMITr
    };


    _soc_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, entry_index, mem_id;
    uint32  minfo;
    _soc_ser_correct_info_t spci;

    info = _soc_hu2_parity_group_info[group].info;

    err_reg = E2EFC_PARITYERRORPTRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    mem_id =
            soc_reg_field_get(unit, err_reg, reg_val, INSTf);
    /* CHECK if the error is valid */
    if (mem_id == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
        return SOC_E_NONE;
    }
    entry_index =
            soc_reg_field_get(unit, err_reg, reg_val, PTRf);

    _soc_hu2_mem_parity_info(unit, block, 0,
                         info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                       entry_index, minfo);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d MMU E2EFC entry %d parity error\n"),
               unit, entry_index));
    sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
    spci.reg = e2efc_regs[mem_id];
    spci.mem = INVALIDm;
    spci.blk_type = SOC_BLK_MMU;
    spci.index = entry_index;
    (void)soc_ser_correction(unit, &spci);

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane2_process_mmu_ipmc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{

    _soc_parity_info_t *info;
    soc_reg_t err_reg;
    uint32 reg_val, entry_index;
    uint32  minfo;
    _soc_ser_correct_info_t spci;
    soc_field_t     index_fld;
    soc_mem_t       target_mem;
    

    info = _soc_hu2_parity_group_info[group].info;

    err_reg = IPMCPARITYERRORPTRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));
    switch (info[table].error_field) {
        case IPMCVLANXPORTPARITYERRORf:
            index_fld = IPMC_VLAN_XPORT_PARITYERRORPTRf;
            target_mem = MMU_IPMC_VLAN_TBLm;
            break;
        case IPMCVLANGPORTPARITYERRORf:
            index_fld = IPMC_VLAN_GPORT_PARITYERRORPTRf;
            target_mem = MMU_IPMC_VLAN_TBLm;
            break;
        case IPMCGROUP0PARITYERRORf:
            index_fld = IPMC_GROUP0_PARITYERRORPTRf;
            target_mem = MMU_IPMC_GROUP_TBL0m;
            break;
        case IPMCGROUP1PARITYERRORf:
            index_fld = IPMC_GROUP1_PARITYERRORPTRf;
            target_mem = MMU_IPMC_GROUP_TBL1m;
            break;
        default:
            return SOC_E_INTERNAL;
    }
    entry_index =
            soc_reg_field_get(unit, err_reg, reg_val, index_fld);

    _soc_hu2_mem_parity_info(unit, block, 0,
                         info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                       entry_index, minfo);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d MMU_%s entry %d parity error\n"),
               unit, SOC_MEM_NAME(unit, target_mem), entry_index));
    sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
    spci.reg = INVALIDr;
    spci.mem = target_mem;
    spci.blk_type = SOC_BLK_MMU;
    spci.index = entry_index;
    (void)soc_ser_correction(unit, &spci);

    return SOC_E_NONE;
}


STATIC int
_soc_hurricane2_process_parity_error(int unit)
{
    int group, table;
    uint32 cmic_rval, group_status,
        group_enable, cpi_blk_bit;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;
    uint32 minfo;

    /* Read CMIC parity status register */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN
        (READ_CMIC_CMC0_IRQ_STAT2r(unit, &cmic_rval));
    if (cmic_rval == 0) {
        return SOC_E_NONE;
    }

    /* loop through each group */
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;
        group_reg = _soc_hu2_parity_group_info[group].status_reg;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d %s parity processing\n"),
                                unit, SOC_REG_NAME(unit, group_reg)));
        cpi_blk_bit = _soc_hu2_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu2_parity_group_info[group].blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            if (!(cmic_rval & cpi_blk_bit)) {
                cpi_blk_bit <<= 1;
                continue;
            }

            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, group_reg, 
                block_port, 0, &group_status));

            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (!soc_reg_field_valid(unit, group_reg,
                                         info[table].error_field)) {
                    if (info[table].mem != INVALIDm) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d %s has bad error field\n"),
                                              unit, SOC_MEM_NAME(unit, info[table].mem)));
                    }
                    continue;
                }

                if (!soc_reg_field_get(unit, group_reg, group_status,
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

                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "unit %d %s analysis\n"),
                                        unit, msg));

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                    _soc_hu2_mem_parity_info(unit, blk, 0,
                                 info[table].error_field, &minfo);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 0,
                               minfo);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s asserted\n"),
                                          unit, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_dual_parity_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;                   
                case _SOC_PARITY_INFO_TYPE_MMU_E2EFC:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_mmu_e2efc_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_IPMC:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_mmu_ipmc_error(unit,
                              group, block_port, table, msg, blk));
                    break;

                case _SOC_PARITY_INFO_TYPE_OAM:
                    SOC_IF_ERROR_RETURN
                         (_soc_hu2_process_oam_interrupt(unit));                    
                    break;

                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */

            /* clear MMU int status */
            if (SOC_BLK_MMU == _soc_hu2_parity_group_info[group].blocktype) {
                SOC_IF_ERROR_RETURN(soc_reg32_set
                            (unit, group_reg,  block_port, 0, 0));
            }
            reg = _soc_hu2_parity_group_info[group].enable_reg;
            SOC_IF_ERROR_RETURN(soc_reg32_get
                            (unit, reg,  block_port, 0, &group_enable));
            group_enable &= ~group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_set
                            (unit, reg,  block_port, 0, group_enable));
            group_enable |= group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_set
                            (unit, reg,  block_port, 0, group_enable));
        } /* loop through each block for the group */
    } /* loop through each group */

    return SOC_E_NONE;
}

void
soc_hurricane2_parity_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    (void)_soc_hurricane2_process_parity_error(unit);
    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d2 != NULL) {
        (void)soc_cmicm_intr2_enable(unit, PTR_TO_INT(d2));
    }
}

STATIC int
_soc_hurricane2_mem_nack_error_process(int unit, 
        _hurricane2_ser_nack_reg_mem_t nack_reg_mem, 
        soc_block_t block, int reg_mem)
{
    int group, table;
    _soc_parity_info_t *info;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;

    if (nack_reg_mem.mem == INVALIDm) {
        return SOC_E_PARAM;
    }

    /* loop through each group */
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu2_parity_group_info[group].blocktype) {
            if (block != SOC_BLOCK_INFO(unit, blk).schan) {
                continue;
            }
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (info[table].mem != nack_reg_mem.mem) {
                    continue;
                }                
                if (info[table].msg) {
                    msg = info[table].msg;
                } else { /* mem must be valid to get her */
                    msg = SOC_MEM_NAME(unit, info[table].mem);
                }

                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg, block));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane2_process_dual_parity_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;                   
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
        } /* loop through each block of the group */
    } /* loop through each group */

    return SOC_E_NONE;
}

void
soc_hurricane2_mem_nack(void *unit_vp, void *addr_vp, void *blk_vp, 
                     void *d3, void *d4)
{
    soc_mem_t mem = INVALIDm;
    int reg_mem = PTR_TO_INT(d3);
    int rv, unit = PTR_TO_INT(unit_vp);
    uint32 address = PTR_TO_INT(addr_vp);
    uint32 block = PTR_TO_INT(blk_vp);
    uint32 offset = 0, min_addr = 0;
    soc_regaddrinfo_t ainfo;
    _hurricane2_ser_nack_reg_mem_t nack_reg_mem;
    soc_field_t     error_fld = INVALIDf;
    uint32 minfo;
    _soc_ser_correct_info_t spci;
    char *msg;

    if (reg_mem == _SOC_HURRICANE2_SER_REG) {
        nack_reg_mem.reg = INVALIDr;
        if (address) {
            soc_regaddrinfo_get(unit, &ainfo, address);
            nack_reg_mem.reg = ainfo.reg;
        }
    } else {
        offset = address;
        mem = soc_addr_to_mem_extended(unit, block, 0, address);
        if (mem == INVALIDm) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d mem decode failed, "
                                  "SCHAN NACK analysis failure\n"), unit));
            return;
        }
        
        nack_reg_mem.mem = mem;

        min_addr = SOC_MEM_INFO(unit, mem).base;
        min_addr += SOC_MEM_INFO(unit, mem).index_min;
    }

    if ((mem == EGR_L3_NEXT_HOPm) || (mem == EGR_L3_INTFm) ||
        (mem == EGR_VLANm) || (mem == EGR_VLAN_STGm) ||
        (mem == EFP_COUNTER_TABLEm) || 
        (mem == EGR_PERQ_XMT_COUNTERSm)) {
        /* 
         * The inetrupt of parity error for these memories will not be triggered.
         * And the parity error status register will noT be updated.
         * Use the offset value of S-channel msg to instead of.
         */
        switch (mem) {
            case EGR_L3_NEXT_HOPm:
                error_fld = EGR_NHOP_PAR_ERRf;
                break;
            case EGR_L3_INTFm:
                error_fld = EGR_L3_INTF_PAR_ERRf;
                break;
            case EGR_VLANm:
                error_fld = EGR_VLAN_PAR_ERRf;
                break;
            case EGR_VLAN_STGm:
                error_fld = EGR_VLAN_STG_PAR_ERRf;
                break;
            case EFP_COUNTER_TABLEm:
                error_fld = EGR_FP_COUNTERf;
                break;
            default:
                /* EGR_PERQ_XMT_COUNTERSm */
                error_fld = EGR_PERQ_COUNTERf;
                break;
        }
        _soc_hu2_mem_parity_info(unit, block, 0,
                                 error_fld, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                               (offset - min_addr), minfo);
        msg = SOC_MEM_NAME(unit, mem);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                   unit, msg, (offset - min_addr)));
        sal_memset(&spci, 0, sizeof(spci));
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;         
        spci.reg = INVALIDr;
        spci.mem = mem;
        spci.blk_type = -1;
        spci.index = (offset - min_addr);
        (void)soc_ser_correction(unit, &spci);
    } else {

        if ((rv = _soc_hurricane2_mem_nack_error_process(unit, nack_reg_mem, 
            (soc_block_t)block, reg_mem)) < 0) {
            if (reg_mem == _SOC_HURRICANE2_SER_REG) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d REG SCHAN NACK analysis failure\n"),
                           unit));
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s entry %d SCHAN NACK analysis failure\n"),
                           unit, SOC_MEM_NAME(unit, mem),
                           offset - min_addr));
            }
        }
    }
    if (reg_mem == _SOC_HURRICANE2_SER_REG) {
        if (nack_reg_mem.reg != INVALIDr) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s REG SCHAN NACK analysis\n"),
                       unit, SOC_REG_NAME(unit, nack_reg_mem.reg)));
            soc_reg32_set(unit, ainfo.reg, ainfo.port, ainfo.idx, 0);
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d invalid register for REG SCHAN NACK analysis\n"),
                       unit));
        }
    } else {
        LOG_INFO(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "unit %d %s entry %d SCHAN NACK analysis\n"),
                  unit, SOC_MEM_NAME(unit, mem),
                  offset - min_addr));
    }
}


static int
soc_hurricane2_pipe_mem_clear(int unit)
{
    uint32              rval, index;
    int                 blk_port;
    soc_block_t         blk;
    int                 pipe_init_usec;
    soc_timeout_t       to;
    int                 tcam_protect_write;
    static const soc_mem_t cam[] = {
        VLAN_SUBNETm,
        VLAN_SUBNET_ONLYm,
        VFP_TCAMm,
        L2_USER_ENTRYm,
        L2_USER_ENTRY_ONLYm,
        L3_DEFIPm,
        L3_DEFIP_ONLYm,
        FP_TCAMm,
        FP_GLOBAL_MASK_TCAMm,
        CPU_COS_MAPm,
        CPU_COS_MAP_ONLYm,
        EFP_TCAMm
    };

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

    if (!SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM) {
        /* TCAM tables are not handled by hardware reset control */
        tcam_protect_write = SOC_CONTROL(unit)->tcam_protect_write;
        SOC_CONTROL(unit)->tcam_protect_write = FALSE;
        for (index = 0; index < sizeof(cam) / sizeof(soc_mem_t); index++) {
            if (SOC_MEM_IS_VALID(unit, cam[index])) {
                if (SAL_BOOT_BCMSIM &&
                    !((cam[index] == VLAN_SUBNET_ONLYm) ||
                     (cam[index] == CPU_COS_MAP_ONLYm) ||
                     (cam[index] == L2_USER_ENTRY_ONLYm) ||
                     (cam[index] == L3_DEFIP_ONLYm))) {
                    continue;
                }
                SOC_IF_ERROR_RETURN(soc_mem_clear(unit, cam[index],
                                                  COPYNO_ALL, TRUE));
            }
        }
        SOC_CONTROL(unit)->tcam_protect_write = tcam_protect_write;

        /* MMU_IPMC_VLAN_TBL */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, TRUE));
        /* MMU_IPMC_GROUP_TBL0 */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL0m, COPYNO_ALL, TRUE));
        /* MMU_IPMC_GROUP_TBL1 */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL1m, COPYNO_ALL, TRUE));
    }

    /* Need to clear ING_PW_TERM_COUNTERS since it's not handled by reset control */
    if (!(SAL_BOOT_QUICKTURN) && SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, ING_PW_TERM_COUNTERSm, COPYNO_ALL, TRUE));
    }

    /* Reset XLPORT MIB counter (registers implemented in memory) */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        blk_port = SOC_BLOCK_PORT(unit, blk);
        if (blk_port < 0) {
            continue;
        }
        rval = 0;
        /* All Ports */
        soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf); 
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, blk_port, 0));
    }
    
    return SOC_E_NONE;
}


#define  HU2_NUM_COS  8
int soc_hu2_xq_mem(int unit, soc_port_t port, soc_mem_t *xq)
{
    soc_mem_t   xq_mems[] = {   MMU_XQ0m,
                                MMU_XQ1m,
                                MMU_XQ2m,
                                MMU_XQ3m,
                                MMU_XQ4m,
                                MMU_XQ5m,
                                MMU_XQ6m,
                                MMU_XQ7m,
                                MMU_XQ8m,
                                MMU_XQ9m,
                                MMU_XQ10m,
                                MMU_XQ11m,
                                MMU_XQ12m,
                                MMU_XQ13m,
                                MMU_XQ14m,
                                MMU_XQ15m,
                                MMU_XQ16m,
                                MMU_XQ17m,
                                MMU_XQ18m,
                                MMU_XQ19m,
                                MMU_XQ20m,
                                MMU_XQ21m,
                                MMU_XQ22m,
                                MMU_XQ23m,
                                MMU_XQ24m,
                                MMU_XQ25m,
                                MMU_XQ26m,
                                MMU_XQ27m,
                                MMU_XQ28m,
                                MMU_XQ29m
                            };
    int max_port;
    max_port = sizeof(xq_mems) / sizeof(xq_mems[0]);
    if (port >= max_port) {
        return(SOC_E_PORT);
    }
    *xq = xq_mems[port];
    return(SOC_E_NONE);
}

/* Hurricane uses 1.5MB of internal Buffers */
#define HU2_MMU_BUFFER_SIZE		(1.5 * 1024 * 1024) 

/* 24GE + 4HG +1CPU ports */ 
#define HU2_MMU_TOTAL_PORTS		29 

#define HU2_MMU_NUM_COS  		8
#define HU2_MMU_CELL_SIZE 		128

/* total cells = 1.5MB/128 = 12288 */
#define HU2_MMU_TOTAL_CELLS 		(HU2_MMU_BUFFER_SIZE/HU2_MMU_CELL_SIZE)

#define HU2_MMU_IN_COS_MIN_CELLS		12
#define HU2_MMU_IN_COS_MIN_XQS		8

/* cos(8) * reserved cells(12) = 96 */
#define HU2_MMU_IN_PORT_STATIC_CELLS	(HU2_MMU_NUM_COS * HU2_MMU_IN_COS_MIN_CELLS)

/* ports(29) * per port cells(96) = 2784 */
#define HU2_MMU_TOTAL_STATIC_CELLS	(HU2_MMU_TOTAL_PORTS * HU2_MMU_IN_PORT_STATIC_CELLS)

/* total cells(12288) - static cells(2784) = 9504 */
#define HU2_MMU_TOTAL_DYNAMIC_CELLS 	(HU2_MMU_TOTAL_CELLS - HU2_MMU_TOTAL_STATIC_CELLS)

/* Every port has 1536 packet pointers(XQs) */
#define HU2_MMU_IN_PORT_TOTAL_XQS	1536
/* number of cos(8) * minimum xqs per cos(8) = 64 */
#define HU2_MMU_IN_PORT_STATIC_XQS	(HU2_MMU_NUM_COS * HU2_MMU_IN_COS_MIN_XQS)
/* total xqs (1536)- static xqs(64) = 1472 */
#define HU2_MMU_IN_PORT_DYNAMIC_XQS	(HU2_MMU_IN_PORT_TOTAL_XQS - HU2_MMU_IN_PORT_STATIC_XQS)

#define HU2_MMU_MAX_PKT_SIZE		9728
/* xoff threshold = 80 */
#define HU2_MMU_XOFF_CELL_LIMIT		((HU2_MMU_MAX_PKT_SIZE/HU2_MMU_CELL_SIZE) + 4)

/* max limit for pri-0 = (xoff threshold(80) + max packet size/128 *2 + 16)*23 */
#define HU2_MMU_MAX_CELL_LIMIT_PRI_0     5704	
/* max limit for pri-1-7 =  (total xqs/128 * 4)= 1536/128*4 */
#define HU2_MMU_MAX_CELL_LIMIT_PRI_1_7   48	

/* cell limit0 + 7 * cell limit for pri1-7 = 6040 */ 
#define HU2_MMU_DYN_CELL_LIMIT		(HU2_MMU_MAX_CELL_LIMIT_PRI_0 +  \
                                         ((HU2_MMU_NUM_COS -1) * HU2_MMU_MAX_CELL_LIMIT_PRI_1_7))

static int port_speed_max[SOC_MAX_PHY_PORTS]; /* max phy port speed */

int
soc_hu2_init_port_mapping(int unit)
{
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_port, num_phy_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);
    sal_memset(&entry, 0, sizeof(entry));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x1f : port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }
    num_port = soc_mem_index_count(unit, PORT_TABm);
    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x3f : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }
    /* MMU logical to physical port mapping */
    /*(Here, Same as Egress logical to physical port mapping) */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_m2p_mapping[port];
        if (phy_port != -1) {
            rval = 0;
            soc_reg_field_set(unit, LOG_TO_PHY_PORT_MAPPINGr, &rval,
                              PHY_PORTf, phy_port);
            SOC_IF_ERROR_RETURN
                (WRITE_LOG_TO_PHY_PORT_MAPPINGr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}


#define CUSTOM_MAPPING_ARRAY_SIZE 34
#define CUSTOM_TSC_PORTMAP_COUNT 8
#define TSCPORT_OFFSET 26
typedef struct _custom_portmap_info_s {
    int phy_port_default;
    int phy_port_custom;
} _custom_portmap_info_t;

static 
_custom_portmap_info_t hu2_custom_tsc_portmap[CUSTOM_TSC_PORTMAP_COUNT] = {
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}
};

/*
 * Hu2 port mapping
 * cpu port number is fixed: physical 0, logical 0, mmu 0
 */
int
soc_hu2_get_port_mapping(int unit, uint16 dev_id)
{

    /* Port Mappings */
    static const int p2l_mapping_24_4_0[] = { /* Non-Cascade, Powersave Plus */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, 27, 28, 29,
         -1, -1, -1, -1
    };
    static const int p2l_mapping_24_0_4[] = { /* Non-Cascade, Powersave Plus */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         -1, -1, -1, -1,
         26, 27, 28, 29
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_4_0[] = { /* Non-Cascade, Powersave Plus */
        0, -1,
        9,  8,  7,  6,  5,  4,  3,  2,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, 27, 28, 29,
        -1, -1, -1, -1
    }; 
    static const int p2l_mapping_18_0_4[] = { /* Non-Cascade, Powersave Plus */
        0, -1,
        9,  8,  7,  6,  5,  4,  3,  2,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        -1, -1, -1, -1,
        26, 27, 28, 29
    }; 
    static const int p2l_mapping_24_4_0_x[] = { /* Non-Cascade (No Phy) */
          0, -1,
          2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, 27, 28, 29,
         -1, -1, -1, -1
    };
    static const int p2l_mapping_24_0_4_x[] = { /* Non-Cascade (No Phy) */
          0, -1,
          2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         -1, -1, -1, -1,
         26, 27, 28, 29
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_4_0_x[] = { /* Non-Cascade (No Phy) */
        0, -1,
        2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, 27, 28, 29,
        -1, -1, -1, -1
    }; 
    static const int p2l_mapping_18_0_4_x[] = { /* Non-Cascade (No Phy) */
        0, -1,
        2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        -1, -1, -1, -1,
        26, 27, 28, 29
    }; 
    static const int p2l_mapping_24_2_2[] =  { /* Cascade */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, -1, 27, -1,
         28, -1, 29, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_2_2[] =  { /* Cascade */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, -1, -1, -1, 22, -1, -1, -1,
         26, -1, 27, -1,
         28, -1, 29, -1
    };
    static const int p2l_mapping_24_2_2_x[] =  { /* Cascade (No Phy) */
          0, -1,
          2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, -1, 27, -1,
         28, -1, 29, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_2_2_x[] =  { /* Cascade (No Phy) */
        0, -1,
        2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, -1, 27, -1,
        28, -1, 29, -1
    }; 
    static const int p2l_mapping_24_3_1[] =  { /* XAUI */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, 27, 28, -1,
         29, -1, -1, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_3_1[] =  { /* XAUI */
        0, -1,
        9,  8,  7,  6,  5,  4,  3,  2,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, 27, 28, -1,
        29, -1, -1, -1
    }; 
    static const int p2l_mapping_24_1_1[] =  { /* 2xXAUI */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, -1, -1, -1,
         27, -1, -1, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_1_1[] =  { /* 2xXAUI */
        0, -1,
        9,  8,  7,  6,  5,  4,  3,  2,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, -1, -1, -1,
        27, -1, -1, -1
    }; 
    static const int p2l_mapping_24_3_1_x[] =  { /* XAUI (No Phy) */
          0, -1,
          2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, 27, 28, -1,
         29, -1, -1, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_3_1_x[] =  { /* XAUI (No Phy) */
        0, -1,
        2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, 27, 28, -1,
        29, -1, -1, -1
    }; 
    static const int p2l_mapping_24_1_1_x[] =  { /* 2xXAUI (No Phy) */
          0, -1,
          2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         26, -1, -1, -1,
         27, -1, -1, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int p2l_mapping_18_1_1_x[] =  { /* 2xXAUI (No Phy) */
        0, -1,
        2,  3,  4,  5,  6,  7,  8,  9,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        26, -1, -1, -1,
        27, -1, -1, -1
    }; 
    static const int p2l_mapping_24_0_0[] = { /* Powersave */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         -1, -1, -1, -1,
         -1, -1, -1, -1
    };
    static const int p2l_mapping_16_0_0[] = { /* Powersave */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1
    };
    static const int p2l_mapping_16_4_0[] = { /* Powersave */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         -1, -1, -1, -1, -1, -1, -1, -1,
         26, 27, 28, 29,
         -1, -1, -1, -1
    };
    static const int p2l_mapping_8_0_0[] = { /* Powersave */
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1
    };
    static const int p2l_mapping_6_4_4[] = { /* Embedded */
          0, -1,
          2, -1, -1, -1,  6, -1, -1, -1,
         10, -1, -1, -1, 14, -1, -1, -1,
         18, -1, -1, -1, 22, -1, -1, -1,
         26, 27, 28, 29,
         21, 23, 24, 25
    };
    static const int p2l_mapping_6_1_4[] = { /* Embedded - XAUI */
          0, -1,
          2, -1, -1, -1,  6, -1, -1, -1,
         10, -1, -1, -1, 14, -1, -1, -1,
         18, -1, -1, -1, 22, -1, -1, -1,
         26, -1, -1, -1,
         21, 23, 24, 25
    };
    static const int p2l_mapping_6_1_3[] = { /* Embedded Plus - XAUI */
          0, -1,
          2, -1, -1, -1,  6, -1, -1, -1,
         10, -1, -1, -1, 14, -1, -1, -1,
         18, -1, -1, -1, 22, -1, -1, -1,
         26, -1, -1, -1,
         27, 28, 29, -1
    };
    /* In Wolfhound ref platform, TSC ports are Even-Odd swapped */
    /* Wolfhound Non-Cascade: only for the BCM953344R reference board */
    /* static const int p2l_mapping_24_4_0_wh[] = {
          0, -1,
          9,  8,  7,  6,  5,  4,  3,  2,
         10, 11, 12, 13, 14, 15, 16, 17,
         18, 19, 20, 21, 22, 23, 24, 25,
         27, 26, 29, 28,
         -1, -1, -1, -1
    }; */
    /* The mapping mode for QSGMII been forced at SGMII mode */
    /* Wolfhound Non-Cascade: only for the BCM953344R reference board */
    /* static const int p2l_mapping_18_4_0_wh[] = {
        0, -1,
        9,  8,  7,  6,  5,  4,  3,  2,
        10, 11, 12, 13, 14, 15, 16, 17,
        18, -1, -1, -1, 22, -1, -1, -1, 
        27, 26, 29, 28,
        -1, -1, -1, -1
    }; */

    /* Port Max Speeds */
    static const int port_speed_max_24x1g_4x10g[] = {
         -1, -1,
         10, 10, 10, 10, 10, 10, 10, 10,
         10, 10, 10, 10, 10, 10, 10, 10,
         10, 10, 10, 10, 10, 10, 10, 10,
        110,110,110,110,
         -1, -1, -1, -1
    };
    static const int port_speed_max_24x1g_4x10g_tsc1[] = {
         -1, -1,
         10, 10, 10, 10, 10, 10, 10, 10,
         10, 10, 10, 10, 10, 10, 10, 10,
         10, 10, 10, 10, 10, 10, 10, 10,
         -1, -1, -1, -1,
        110,110,110,110
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_4x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       110,110,110,110,
        -1, -1, -1, -1
    }; 
    static const int port_speed_max_18x1g_4x10g_tsc1[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
        -1, -1, -1, -1,
       110,110,110,110
    }; 
    static const int port_speed_max_24x1g_3x10g_1x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       110,110,110, -1,
       100, -1, -1, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_3x10g_1x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       110,110,110, -1,
       100, -1, -1, -1
    }; 
    static const int port_speed_max_24x1g_1x10g_1x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       100, -1, -1, -1,
       100, -1, -1, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_1x10g_1x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       100, -1, -1, -1,
       100, -1, -1, -1
    }; 
    static const int port_speed_max_24x1g_2x10g_2x13g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       110, -1,110, -1,
       130, -1,130, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_2x10g_2x13g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       110, -1,110, -1,
       130, -1,130, -1
    }; 
    static const int port_speed_max_24x1g_2x10g_2x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       110, -1,110, -1,
       110, -1,110, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_2x10g_2x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       110, -1,110, -1,
       110, -1,110, -1
    };
    static const int port_speed_max_24x1g_2x13g_2x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       130, -1,130, -1,
       110, -1,110, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_2x13g_2x10g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       130, -1,130, -1,
       110, -1,110, -1
    }; 
    static const int port_speed_max_24x1g_2x1g_2x13g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, 10, -1,
       130, -1,130, -1
    };
    static const int port_speed_max_24x1g_2x2500k_2x13g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        25, -1, 25, -1,
       130, -1,130, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_2x1g_2x13g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
        10, -1, 10, -1,
       130, -1,130, -1
    }; 
    static const int port_speed_max_18x1g_2x2500k_2x13g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
        25, -1, 25, -1,
       130, -1,130, -1
    }; 
    static const int port_speed_max_24x1g_2x13g_2x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       130, -1,130, -1,
        10, -1, 10, -1
    };
    static const int port_speed_max_24x1g_2x13g_2x2500k[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
       130, -1,130, -1,
        25, -1, 25, -1
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_2x13g_2x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       130, -1,130, -1,
        10, -1, 10, -1
    }; 
    static const int port_speed_max_18x1g_2x13g_2x2500k[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
       130, -1,130, -1,
        25, -1, 25, -1
    }; 
    static const int port_speed_max_24x1g_4x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10,
        -1, -1, -1, -1
    };
    static const int port_speed_max_24x1g_4x2500k[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        25, 25, 25, 25,
        -1, -1, -1, -1
    };
    static const int port_speed_max_24x1g_4x2500k_tsc1[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        -1, -1, -1, -1,
        25, 25, 25, 25
    };
    /* The mapping mode for QSGMII been forced at SGMII mode */
    static const int port_speed_max_18x1g_4x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
        10, 10, 10, 10,
        -1, -1, -1, -1
    }; 
    static const int port_speed_max_18x1g_4x2500k[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
        25, 25, 25, 25,
        -1, -1, -1, -1
    }; 
    static const int port_speed_max_18x1g_4x2500k_tsc1[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, -1, -1, -1, 10, -1, -1, -1,
        -1, -1, -1, -1,
        25, 25, 25, 25
    }; 
    static const int port_speed_max_24x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        -1, -1, -1, -1,
        -1, -1, -1, -1
    };
    static const int port_speed_max_16x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1
    };
    static const int port_speed_max_16x1g_4x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        -1, -1, -1, -1, -1, -1, -1, -1,
        10, 10, 10, 10,
        -1, -1, -1, -1
    };
    static const int port_speed_max_8x1g[] = {
        -1, -1,
        10, 10, 10, 10, 10, 10, 10, 10,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1
    };
    static const int port_speed_max_6x1g_4x10g_4x1g[] = {
         -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
        110,110,110,110,
         10, 10, 10, 10
    };
    static const int port_speed_max_6x1g_4x1g_4x1g[] = {
         -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, 10, 10, 10,
         10, 10, 10, 10
    };
    static const int port_speed_max_6x1g_1x10g_4x1g[] = {
         -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
        100, -1, -1, -1,
         10, 10, 10, 10
    };
    static const int port_speed_max_6x1g_1x10g_3x10g[] = {
         -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
         10, -1, -1, -1, 10, -1, -1, -1,
        100, -1, -1, -1,
        110,110,110, -1
    };

    soc_info_t *si;
    int rv = SOC_E_NONE;
    int phy_port;
    const int *p2l_mapping=NULL, *speed_max=NULL;
    int hu2_config_id;
    uint32 qsgmii_override = 0;
    int i;
    int j;
    int index;
    int logical_port;
    int port_bandwidth;
    char *config_str, *sub_str, *sub_str_end;
    int custom_port_mapping = 0;
    int tmp_p2l_mapping[CUSTOM_MAPPING_ARRAY_SIZE];
    int default_p2l_mapping[CUSTOM_MAPPING_ARRAY_SIZE];
    int p2l_mapping_custom[CUSTOM_MAPPING_ARRAY_SIZE] = { /* customized p2l mapping */
        0, -1,
       -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1,
       -1, -1, -1, -1
    };
    int port_speed_max_custom[CUSTOM_MAPPING_ARRAY_SIZE] = {
       -1, -1,
       -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1, -1, -1, -1, -1,
       -1, -1, -1, -1,
       -1, -1, -1, -1
    };

    hu2_config_id = soc_property_get(unit, spn_BCM5615X_CONFIG, 0);

    qsgmii_override = soc_property_get(unit,
            spn_SERDES_QSGMII_SGMII_OVERRIDE, 0); 

    switch (dev_id) {
        case BCM56150_DEVICE_ID:
        case BCM53346_DEVICE_ID:
            if (hu2_config_id == 0) {
                /* Option 1: 24P 1G +4P 1G/10G  (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G +4P 1G/10G  (PHY) */
                    p2l_mapping = p2l_mapping_18_4_0;
                    speed_max = port_speed_max_18x1g_4x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_4_0;
                    speed_max = port_speed_max_24x1g_4x10g;
                }
            } else if ((hu2_config_id == 1) ||
                       (hu2_config_id == 10)) {
                /* Option 2: 24P 1G + 2P 1G/10G + 2P 13G (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 1G/10G + 2P 13G (PHY) */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x10g_2x13g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x10g_2x13g;
                }
            } else if (hu2_config_id == 11) {
                /* Option 2A: 24P 1G + 2P 13G + 2P 1G/10G (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 13G + 2P 1G/10G (PHY) */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x13g_2x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x13g_2x10g;
                }
            } else if (hu2_config_id == 12) {
                /* Option 1A: 24P 1G + 2P 1G/10G + 2P 1G/10G (PHY)  */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 1G/10G + 2P 1G/10G (PHY)  */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x10g_2x10g;
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x10g_2x10g;
                }               
            } else if (hu2_config_id == 13) {
                /* Option 1B: 24P 1G + 4P 1G/10G  (PHY, TSC1) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 4P 1G/10G  (PHY) */
                    p2l_mapping = p2l_mapping_18_0_4;
                    speed_max = port_speed_max_18x1g_4x10g_tsc1; 
                } else {
                    p2l_mapping = p2l_mapping_24_0_4;
                    speed_max = port_speed_max_24x1g_4x10g_tsc1;
                }				
            } else if (hu2_config_id == 2) {
                /* Option 3: 24P 1G + 3P 1G/10G + 1P XAUI (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 3P 1G/10G + 1P XAUI (PHY) */
                    p2l_mapping = p2l_mapping_18_3_1;
                    speed_max = port_speed_max_18x1g_3x10g_1x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_3_1;
                    speed_max = port_speed_max_24x1g_3x10g_1x10g;
                }
            } else if (hu2_config_id == 3) {
                /* Option 4: 24P 1G + 1P XAUI + 1P XAUI (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 1P XAUI + 1P XAUI (PHY) */
                    p2l_mapping = p2l_mapping_18_1_1;
                    speed_max = port_speed_max_18x1g_1x10g_1x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_1_1;
                    speed_max = port_speed_max_24x1g_1x10g_1x10g;
                }
            }
            break;
        case BCM56151_DEVICE_ID:
        case BCM53347_DEVICE_ID:
            if (hu2_config_id == 0) {
                /* Option 1: 24P 1G + 4P 1G/10G (no Phy) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 4P 1G/10G (no Phy) */
                    p2l_mapping = p2l_mapping_18_4_0_x;
                    speed_max = port_speed_max_18x1g_4x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_4_0_x; 
                    speed_max = port_speed_max_24x1g_4x10g; 
                }
            } else if ((hu2_config_id == 1) ||
                       (hu2_config_id == 10)) {
                /* Option 2: 24P 1G + 2P 1G/10G + 2P 13G (no Phy) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 1G/10G + 2P 13G (no Phy) */
                    p2l_mapping = p2l_mapping_18_2_2_x;
                    speed_max = port_speed_max_18x1g_2x10g_2x13g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2_x;
                    speed_max = port_speed_max_24x1g_2x10g_2x13g;
                }
            } else if (hu2_config_id == 11) {
                /* Option 2A: 24P 1G + 2P 13G + 2P 1G/10G (no Phy) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 13G + 2P 1G/10G (no Phy) */
                    p2l_mapping = p2l_mapping_18_2_2_x;
                    speed_max = port_speed_max_18x1g_2x13g_2x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2_x;
                    speed_max = port_speed_max_24x1g_2x13g_2x10g;
                }
            } else if (hu2_config_id == 12) {
                /* Option 1A: 24P 1G + 2P 1G/10G + 2P 1G/10G (no PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 1G/10G + 2P 1G/10G (no PHY) */
                    p2l_mapping = p2l_mapping_18_2_2_x;
                    speed_max = port_speed_max_18x1g_2x10g_2x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2_x;
                    speed_max = port_speed_max_24x1g_2x10g_2x10g;
                }
            } else if (hu2_config_id == 13) {
                /* Option 1B: 24P 1G + 4P 1G/10G  (no PHY, TSC1) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 4P 1G/10G (no Phy) */
                    p2l_mapping = p2l_mapping_18_0_4_x;
                    speed_max = port_speed_max_18x1g_4x10g_tsc1; 
                } else {
                    p2l_mapping = p2l_mapping_24_0_4_x; 
                    speed_max = port_speed_max_24x1g_4x10g_tsc1; 
                }
            } else if (hu2_config_id == 2) {
                /* Option 2A: 24P 1G + 3P 1G/10G + 1P XAUI (no Phy) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 3P 1G/10G + 1P XAUI (no Phy) */
                    p2l_mapping = p2l_mapping_18_3_1_x;
                    speed_max = port_speed_max_18x1g_3x10g_1x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_3_1_x;
                    speed_max = port_speed_max_24x1g_3x10g_1x10g;
                }
            } else if (hu2_config_id == 3) {
                /* Option 2A: 24P 1G + 1P XAUI + 1P XAUI (no Phy) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 1P XAUI + 1P XAUI (no Phy) */
                    p2l_mapping = p2l_mapping_18_1_1_x;
                    speed_max = port_speed_max_18x1g_1x10g_1x10g; 
                } else {
                    p2l_mapping = p2l_mapping_24_1_1_x;
                    speed_max = port_speed_max_24x1g_1x10g_1x10g;
                }
            }
            break;
        case BCM53344_DEVICE_ID:
            if (hu2_config_id == 0) {
                /* Option 1: 24P 1G +4P 1G  (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G +4P 1G  (PHY) */
                    p2l_mapping = p2l_mapping_18_4_0;
                    speed_max = port_speed_max_18x1g_4x1g; 
                } else {
                    p2l_mapping = p2l_mapping_24_4_0;
                    speed_max = port_speed_max_24x1g_4x1g;
                }
            } else if ((hu2_config_id == 1) || 
                       (hu2_config_id == 10)) {
                /* Option 2: 24P 1G + 2P 1G + 2P 13G (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 1G + 2P 13G (PHY) */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x1g_2x13g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x1g_2x13g; 
                }
            } else if (hu2_config_id == 11) {
                /* Option 2A: 24P 1G + 2P 13G + 2P 1G (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 13G + 2P 1G (PHY) */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x13g_2x1g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x13g_2x1g; 
                }
            }
            break;
        case BCM56152_DEVICE_ID:
            if (hu2_config_id == 0) {
                /* Option 1: 24P 1G +4P 1G/2.5G  (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G +4P 1G/2.5G  (PHY) */
                    p2l_mapping = p2l_mapping_18_4_0;
                    speed_max = port_speed_max_18x1g_4x2500k; 
                } else {
                    p2l_mapping = p2l_mapping_24_4_0;
                    speed_max = port_speed_max_24x1g_4x2500k; 
                }
            } else if ((hu2_config_id == 1) || 
                       (hu2_config_id == 10)) {
                /* Option 2: 24P 1G + 2P 1G/2.5G + 2P 13G (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 1G/2.5G + 2P 13G (PHY) */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x2500k_2x13g; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x2500k_2x13g; 
                }
            } else if (hu2_config_id == 11) {
                /* Option 2A: 24P 1G + 2P 13G + 2P 1G/2.5G (PHY) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G + 2P 13G + 2P 1G/2.5G (PHY) */
                    p2l_mapping = p2l_mapping_18_2_2;
                    speed_max = port_speed_max_18x1g_2x13g_2x2500k; 
                } else {
                    p2l_mapping = p2l_mapping_24_2_2;
                    speed_max = port_speed_max_24x1g_2x13g_2x2500k; 
                }
            } else if (hu2_config_id == 13) {
                /* Option 1B: 24P 1G +4P 1G  (PHY, TSC1) */
                if (qsgmii_override == 2) {
                    /* (to-2SGMII): 18P 1G +4P 1G  (PHY) */
                    p2l_mapping = p2l_mapping_18_0_4;
                    speed_max = port_speed_max_18x1g_4x2500k_tsc1; 
                } else {
                    p2l_mapping = p2l_mapping_24_0_4;
                    speed_max = port_speed_max_24x1g_4x2500k_tsc1; 
                }
            }
            break;
        case BCM53334_DEVICE_ID:
            /* 24P  1G  (PHY) */
            p2l_mapping = p2l_mapping_24_0_0;
            speed_max = port_speed_max_24x1g;
            break;
        case BCM53333_DEVICE_ID:
            /* 16P  1G (PHY) */
            p2l_mapping = p2l_mapping_16_0_0;
            speed_max = port_speed_max_16x1g;
            break;
        case BCM53343_DEVICE_ID:
            /* 16P  1G (PHY) + 4P 1G */
            p2l_mapping = p2l_mapping_16_4_0;
            speed_max = port_speed_max_16x1g_4x1g;
            break;
        case BCM53342_DEVICE_ID:
            /* 8P  1G (PHY) */
            p2l_mapping = p2l_mapping_8_0_0;
            speed_max = port_speed_max_8x1g;
            break;
        case BCM53393_DEVICE_ID:
            /* 14P 1G,  (no PHY) */
            p2l_mapping = p2l_mapping_6_4_4;
            speed_max = port_speed_max_6x1g_4x1g_4x1g;
            break;
        case BCM53394_DEVICE_ID:
            if (hu2_config_id == 0) {
                /* 10P 1G + 4x1/2.5/5/10G  (no PHY) */
                p2l_mapping = p2l_mapping_6_4_4;
                speed_max = port_speed_max_6x1g_4x10g_4x1g;
            } else if (hu2_config_id == 1) {
                /* 10P 1G + 1P XAUI (no Phy) */
                p2l_mapping = p2l_mapping_6_1_4;
                speed_max = port_speed_max_6x1g_1x10g_4x1g;
            } else if (hu2_config_id == 2) { 
                /* 6P 1G + 3x1/10G + 1P XAUI (no Phy) */
                p2l_mapping = p2l_mapping_6_1_3;
                speed_max = port_speed_max_6x1g_1x10g_3x10g;
            }
            break;
        default :
            break;
    }

    if ((p2l_mapping == NULL) || (speed_max == NULL)) {
        return SOC_E_CONFIG;
    }

    /* save the default p2l mapping */
    sal_memcpy(default_p2l_mapping, p2l_mapping, sizeof(default_p2l_mapping));
    sal_memcpy(tmp_p2l_mapping, p2l_mapping, sizeof(tmp_p2l_mapping));
    sal_memcpy(port_speed_max_custom, speed_max, sizeof(port_speed_max_custom));

    for (logical_port = 2; logical_port < CUSTOM_MAPPING_ARRAY_SIZE; logical_port++) {
        config_str = soc_property_port_get_str(unit, logical_port, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }
        custom_port_mapping = 1;

        /*
         * portmap.<port>=<physical port number>:<bandwidth in Gb>
         *				  [:<queue config>]
         */
        sub_str = config_str;

        /* Parsing physical port number */
        phy_port = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Port %d: Missing physical port information \"%s\"\n"),
                   logical_port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (phy_port < 2 || phy_port > 33) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Port %d: Invalid physical port number %d\n"),
                   logical_port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }

        if (p2l_mapping_custom[phy_port] != -1) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Port %d: Physical port %d is used by port %d\n"),
                   logical_port, phy_port, p2l_mapping_custom[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Skip ':' */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                              "Port %d: Bad config string \"%s\"\n"),
                       logical_port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        /* Parsing bandwidth */
        port_bandwidth = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Port %d: Missing bandwidth information \"%s\"\n"),
                   logical_port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (port_bandwidth != 1 && port_bandwidth != 10 &&
            port_bandwidth != 13 && port_bandwidth != 11) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Port %d: Invalid bandwidth %d Gb\n"),
                   logical_port, port_bandwidth));
            rv = SOC_E_FAIL;
            continue;
        }

        p2l_mapping_custom[phy_port] = logical_port;
        port_speed_max_custom[phy_port] = port_bandwidth * 10;
        tmp_p2l_mapping[phy_port] = logical_port;
    }

    if (custom_port_mapping) {
        p2l_mapping = tmp_p2l_mapping;
        speed_max = port_speed_max_custom;

        for (i = 0; i < CUSTOM_TSC_PORTMAP_COUNT; i++) {
            for (j = 0; j < CUSTOM_MAPPING_ARRAY_SIZE; j++) {
                if (default_p2l_mapping[j] == i + TSCPORT_OFFSET) {
                    hu2_custom_tsc_portmap[i].phy_port_default = j;
                    /* init customer port mapping at default */
                    hu2_custom_tsc_portmap[i].phy_port_custom = j;
                    break;
                }
    	    }
        for (j = 0; j < CUSTOM_MAPPING_ARRAY_SIZE; j++) {
                if (p2l_mapping_custom[j] == i + TSCPORT_OFFSET) {
                    /* apply real customer port mapping */
                    hu2_custom_tsc_portmap[i].phy_port_custom = j;
                    break;
                }
    	    }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "Customized TSC port mapping[%d] = %d, %d\n"),
                        i, hu2_custom_tsc_portmap[i].phy_port_default,
                        hu2_custom_tsc_portmap[i].phy_port_custom));
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "P2L mapping: \n")));
    for (index = 0;index < CUSTOM_MAPPING_ARRAY_SIZE;index++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "%d "),
                    p2l_mapping[index]));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "\n")));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "MAX SPEED: \n")));
    for (index = 0;index < CUSTOM_MAPPING_ARRAY_SIZE;index++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "%d "),
                    speed_max[index]));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "\n")));

    si = &SOC_INFO(unit);
    si->bandwidth = 64000;
    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        si->port_p2l_mapping[phy_port] = p2l_mapping[phy_port];
        si->port_p2m_mapping[phy_port] = p2l_mapping[phy_port]; /* Same as Egr P2L */
        si->max_port_p2m_mapping[phy_port] = p2l_mapping[phy_port]; /* For COSQ programming */
        port_speed_max[phy_port] = 0;
        if (speed_max[phy_port] != -1) {
            si->port_speed_max[si->port_p2l_mapping[phy_port]] = 
                speed_max[phy_port] * 100;
            port_speed_max[phy_port] = speed_max[phy_port] * 100;
        }
    }
    return rv;
}

/*
 * HU2 port mapping
 */
int
soc_hu2_port_config_init(int unit, uint16 dev_id)
{
    int b, p;
    /* Map of block to first physical port in the block */
    int _hu2_b2pp[SOC_MAX_NUM_BLKS];

    for (b=0; SOC_BLOCK_TYPE(unit, b) != -1; b++) {
        _hu2_b2pp[b] = -1;
        for (p=0; SOC_PORT_BLOCK(unit, p)!= -1; p++) {
            if (b == SOC_PORT_BLOCK(unit, p)) {
                _hu2_b2pp[b] = p;
                break;
            }
        }
    }
    SOC_INFO(unit).blk_fpp = _hu2_b2pp;
    return soc_hu2_get_port_mapping(unit, dev_id);
}

int
soc_hurricane2_tsc_reset(int unit)
{
    int blk, port;
    uint32 rval;

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }

    return SOC_E_NONE;
}

int
_soc_hu2_tdm_init(int unit, uint16 dev_id)
{

    /* Embedded Mode TDM */
    uint32 hu2_tdm_embedded[60] = {26,27,28,29,30,31,
                      26,27,28,29,63,63,
                      26,27,28,29,2,63,
                      26,27,28,29,10,63,
                      26,27,28,29,18,0,
                      26,27,28,29,32,33,
                      26,27,28,29,63,63,
                      26,27,28,29,6,63,
                      26,27,28,29,14,63,
                      26,27,28,29,22,63};

    /* Embedded Plus Mode TDM */
    uint32 hu2_tdm_embedded_plus[60] = {26,30,31,32,63,63,
                      26,30,31,32,63,63,
                      26,30,31,32,2,63,
                      26,30,31,32,10,63,
                      26,30,31,32,18,0,
                      26,30,31,32,63,63,
                      26,30,31,32,63,63,
                      26,30,31,32,6,63,
                      26,30,31,32,14,63,
                      26,30,31,32,22,63};

    /* Powersave Mode TDM */
    uint32 hu2_tdm_powersave[48] = {2,10,18,63,63,0,
                      3,11,19,63,63,63,
                      4,12,20,63,63,63,
                      5,13,21,63,63,63,
                      6,14,22,63,63,63,
                      7,15,23,63,63,63,
                      8,16,24,63,63,63,
                      9,17,25,63,63,63};

    /* Cascade Mode TDM */
    uint32 hu2_tdm_cascade[75] = {2,14,26,28,30,32,
                      3,15,26,28,30,32,
                      4,16,26,28,30,32,
                      5,17,26,28,30,32,
                      0,  
                      6,18,26,28,30,32,
                      7,19,26,28,30,32,
                      8,20,26,28,30,32,
                      9,21,26,28,30,32,
                      63,
                      10,22,26,28,30,32,
                      11,23,26,28,30,32,
                      12,24,26,28,30,32,
                      13,25,26,28,30,32,
                      63};

    /* XAUI Mode TDM */
    uint32 hu2_tdm_xaui[75] = {2,14,26,27,28,30,
                      3,15,26,27,28,30,
                      4,16,26,27,28,30,
                      5,17,26,27,28,30,
                      0,  
                      6,18,26,27,28,30,
                      7,19,26,27,28,30,
                      8,20,26,27,28,30,
                      9,21,26,27,28,30,
                      63,
                      10,22,26,27,28,30,
                      11,23,26,27,28,30,
                      12,24,26,27,28,30,
                      13,25,26,27,28,30,
                      63};

    /* Non-Cascade Mode TDM */
    uint32 hu2_tdm_non_cascade[75] = {2,14,26,27,28,29,
                      3,15,26,27,28,29,
                      4,16,26,27,28,29,
                      5,17,26,27,28,29,
                      0,
                      6,18,26,27,28,29,
                      7,19,26,27,28,29,
                      8,20,26,27,28,29,
                      9,21,26,27,28,29,
                      63,
                      10,22,26,27,28,29,
                      11,23,26,27,28,29,
                      12,24,26,27,28,29,
                      13,25,26,27,28,29,
                      63};

    /* Non-Cascade Mode TDM (option 1b)*/
    uint32 hu2_tdm_non_cascade_1b[75] = {2,14,30,31,32,33,
                      3,15,30,31,32,33,
                      4,16,30,31,32,33,
                      5,17,30,31,32,33,
                      0,
                      6,18,30,31,32,33,
                      7,19,30,31,32,33,
                      8,20,30,31,32,33,
                      9,21,30,31,32,33,
                      63,
                      10,22,30,31,32,33,
                      11,23,30,31,32,33,
                      12,24,30,31,32,33,
                      13,25,30,31,32,33,
                      63};

    /* Powersave Plus Mode TDM */
    uint32 hu2_tdm_powersave_plus[48] = {2,10,18,26,63,0,
                      3,11,19,63,63,63,
                      4,12,20,27,63,63,
                      5,13,21,63,63,63,
                      6,14,22,28,63,63,
                      7,15,23,63,63,63,
                      8,16,24,29,63,63,
                      9,17,25,63,63,63};
    uint32              *arr = NULL;
    int                 tdm_size = 0;
    uint32              rval; 
    iarb_tdm_table_entry_t iarb_tdm;
    mmu_arb_tdm_table_entry_t mmu_arb_tdm;
    int i, port, phy_port;
    int hu2_config_id;
    int j;

    hu2_config_id = soc_property_get(unit, spn_BCM5615X_CONFIG, 0);

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 83);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    switch (dev_id) {
        case BCM56150_DEVICE_ID:
        case BCM56151_DEVICE_ID:
        case BCM53346_DEVICE_ID:
        case BCM53347_DEVICE_ID:
            if (hu2_config_id == 0) {
                tdm_size = 75;
                arr = hu2_tdm_non_cascade;
            } else if (hu2_config_id == 13) {
                tdm_size = 75;
                arr = hu2_tdm_non_cascade_1b;
            } else if ((hu2_config_id == 1) ||
                       (hu2_config_id == 10) ||
                       (hu2_config_id == 11) ||
                       (hu2_config_id == 12)) {
                tdm_size = 75;
                arr = hu2_tdm_cascade;
            } else if ((hu2_config_id == 2) ||
                       (hu2_config_id == 3)) {
                tdm_size = 75;
                arr = hu2_tdm_xaui;
            }
            break;
        case BCM53344_DEVICE_ID:
            if (hu2_config_id == 0) {
                tdm_size = 48;
                arr = hu2_tdm_powersave_plus;
            } else if ((hu2_config_id == 1) ||
                       (hu2_config_id == 10) ||
                       (hu2_config_id == 11)) {
                tdm_size = 75;
                arr = hu2_tdm_cascade;
            }
            break;
        case BCM56152_DEVICE_ID:
            if (hu2_config_id == 0) {
                tdm_size = 75;
                arr = hu2_tdm_non_cascade;
            } else if (hu2_config_id == 13) {
                tdm_size = 75;
                arr = hu2_tdm_non_cascade_1b;
            } else if ((hu2_config_id == 1) ||
                       (hu2_config_id == 10) ||
                       (hu2_config_id == 11)) {
                tdm_size = 75;
                arr = hu2_tdm_cascade;
            }
            break;
        case BCM53333_DEVICE_ID:
        case BCM53334_DEVICE_ID:
        case BCM53342_DEVICE_ID:
            tdm_size = 48;
            arr = hu2_tdm_powersave;
            break;
        case BCM53343_DEVICE_ID:
            tdm_size = 48;
            arr = hu2_tdm_powersave_plus;
            break;
        case BCM53393_DEVICE_ID: 
            tdm_size = 60;
            arr = hu2_tdm_embedded;
            break;
        case BCM53394_DEVICE_ID:
            tdm_size = 60;
            if (hu2_config_id == 2) {
                arr = hu2_tdm_embedded_plus;
            } else {
                arr = hu2_tdm_embedded;
            }
            break;
        default :
            break;
    }

    if ((arr == NULL) || (tdm_size == 0)) {
        return SOC_E_CONFIG;
    }

    for (i = 0; i < tdm_size; i++) {
        for (j = 0; j < CUSTOM_TSC_PORTMAP_COUNT; j++) {
			if (arr[i] == hu2_custom_tsc_portmap[j].phy_port_default) {
                if (hu2_custom_tsc_portmap[j].phy_port_default != hu2_custom_tsc_portmap[j].phy_port_custom) {
                    arr[i] = hu2_custom_tsc_portmap[j].phy_port_custom;
                }
                break;
            }
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "TDM table (size %d):\n"),
                            tdm_size));
    for (i = 0; i < tdm_size; i++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%d "),
                            arr[i]));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "\n")));

    for (i = 0; i < tdm_size; i++) {
        phy_port = arr[i];
        port = (phy_port != 63) ? SOC_INFO(unit).port_p2l_mapping[phy_port] : 63;
        
        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&mmu_arb_tdm, 0, sizeof(mmu_arb_tdm_table_entry_t));

        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 
                                        phy_port);

        soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, PORT_NUMf, 
                                       (port != -1) ? port : 63);
        if (i == tdm_size - 1) {
            soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                  &iarb_tdm));
        SOC_IF_ERROR_RETURN(WRITE_MMU_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                 &mmu_arb_tdm));
    }
    rval = 0;
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));
    return SOC_E_NONE;
}

int
soc_hurricane2_mmu_init(int unit)
{
    int         cos;
    soc_port_t  port;
    uint32      val, oval, cfap_max_idx, tm;
    int         age[HU2_NUM_COS], max_age, min_age;
    int         age_enable, disabled_age;
    int         skid_mark;
    uint32 	dyn_xq_per_port;
    uint32      limit;
    uint32 	xq_limit, pkt_limit, cell_limit;
    uint32      cos_min_xqs, cos_min_cells;
    uint16      dev_id;
    uint8       rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    SOC_IF_ERROR_RETURN(_soc_hu2_tdm_init(unit, dev_id));

    cfap_max_idx = soc_mem_index_max(unit, MMU_CFAPm); 
    SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &val));
    oval = val;
    soc_reg_field_set(unit, CFAPCONFIGr, &val, CFAPPOOLSIZEf, cfap_max_idx);
    if (oval != val) {
        SOC_IF_ERROR_RETURN(WRITE_CFAPCONFIGr(unit, val));
    }

    /* set skidmarker to 3 */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, SKIDMARKERf, 3);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    /* Every inactive COSQ consumes skid mark + 4 XQs; remove them from pool */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    skid_mark = soc_reg_field_get(unit, MISCCONFIGr, val, SKIDMARKERf) + 4;
    
    /* Enable IP to CMICM credit transfer */
    val = 0;
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &val, TRANSFER_ENABLEf, 1);
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &val, NUM_OF_CREDITSf, 32);
    SOC_IF_ERROR_RETURN(WRITE_IP_TO_CMICM_CREDIT_TRANSFERr(unit, val));

    if (dev_id != BCM56147_DEVICE_ID) {
        /* total dynamic xqs - skid mark - 9(reserved xqs) */	
        dyn_xq_per_port = HU2_MMU_IN_PORT_DYNAMIC_XQS -  skid_mark - 9;
        /* limit includes the static minimum cells as well */
        xq_limit = dyn_xq_per_port + 8;
        cos_min_xqs = HU2_MMU_IN_COS_MIN_XQS;
        cos_min_cells = HU2_MMU_IN_COS_MIN_CELLS;
    }
    else {
        /* dynamic xqs = total xqs - static xqs(num of cos * per cos reserve(4)) */
        dyn_xq_per_port = HU2_MMU_IN_PORT_TOTAL_XQS - (HU2_MMU_NUM_COS * 4); 
        /* total dynamic xqs = dynamic xqs - skid mark - 9(reserved xqs) */	
        dyn_xq_per_port = dyn_xq_per_port -  skid_mark - 9;
        /* limit includes the static minimum cells as well */
        xq_limit = dyn_xq_per_port + 4;
        cos_min_xqs = HU2_MMU_IN_COS_MIN_XQS/2;
        cos_min_cells = HU2_MMU_IN_COS_MIN_CELLS + 4;
    }

    PBMP_ALL_ITER(unit, port) {

        for (cos = 0; cos < NUM_COS(unit); cos++) {
            /* 
             * The HOLCOSPKTSETLIMITr register controls BOTH the XQ 
             * size per cosq AND the HOL set limit for that cosq.
             */
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val,
                              PKTSETLIMITf, 
                              xq_limit);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, val));

            /* reset limit is set limit - 4 */
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTRESETLIMITr(unit, port, cos, &val));
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &val,
                              PKTRESETLIMITf, 
                              xq_limit-4);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTRESETLIMITr(unit, port, cos, val));

            val = 0;
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &val,
                              HOLCOSMINXQCNTf, cos_min_xqs);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSMINXQCNTr(unit, port, cos, val)); 

            val = 0;
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLSETLIMITf, cos_min_cells);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &val,
                              CELLRESETLIMITf, cos_min_cells);
            SOC_IF_ERROR_RETURN
                (WRITE_LWMCOSCELLSETLIMITr(unit, port, cos, val)); 
              
            cell_limit = 0;
            if (dev_id != BCM56147_DEVICE_ID) {
                if(!cos) {
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXLIMITf, HU2_MMU_MAX_CELL_LIMIT_PRI_0);
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXRESUMELIMITf, HU2_MMU_MAX_CELL_LIMIT_PRI_0 - 12);
                } else {
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXLIMITf, HU2_MMU_MAX_CELL_LIMIT_PRI_1_7);
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXRESUMELIMITf, HU2_MMU_MAX_CELL_LIMIT_PRI_1_7 - 12);
                }
            } 
            else {
                /* use the settings given by design team for 56147 */
                if(!cos) {
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXLIMITf, 8000);
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXRESUMELIMITf, 7238);
                } else {
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXLIMITf, 7250);
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXRESUMELIMITf, 7238);
                }
           }
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSCELLMAXLIMITr(unit, port, cos, cell_limit)); 
        }

        val = 0;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &val,
                          DYNXQCNTPORTf, dyn_xq_per_port);
        SOC_IF_ERROR_RETURN(WRITE_DYNXQCNTPORTr(unit, port, val)); 

        val = 0;
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &val,
                          DYNRESETLIMPORTf, dyn_xq_per_port - 4);
        SOC_IF_ERROR_RETURN(WRITE_DYNRESETLIMPORTr(unit, port, val)); 

        cell_limit = 0;
        soc_reg_field_set(unit, IBPDISCARDSETLIMITr, &cell_limit,
                          DISCARDSETLIMITf, HU2_MMU_TOTAL_CELLS-1);
        SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, cell_limit));

        val = 0;
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLSETLIMITf, HU2_MMU_DYN_CELL_LIMIT);
        soc_reg_field_set(unit, DYNCELLLIMITr, &val,
                          DYNCELLRESETLIMITf, HU2_MMU_DYN_CELL_LIMIT - 24);
        SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val));

        pkt_limit = 0;
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &pkt_limit,
                          PKTSETLIMITf, HU2_MMU_IN_COS_MIN_CELLS);
        SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, pkt_limit));

        cell_limit = 0;
        soc_reg_field_set(unit, IBPCELLSETLIMITr, &cell_limit,
                          CELLSETLIMITf, HU2_MMU_XOFF_CELL_LIMIT);
        SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, cell_limit));

        /* E2EFC configuration */
        if (IS_HG_PORT(unit, port)){
            SOC_IF_ERROR_RETURN(READ_IE2E_CONTROLr(unit, port, &val));
            soc_reg_field_set(unit, IE2E_CONTROLr, &val, IBP_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_IE2E_CONTROLr(unit, port, val));

            val = 0;
            soc_reg_field_set(unit, E2EFC_HG_MIN_TX_TIMERr, &val,
                              LGf, 0);
            soc_reg_field_set(unit, E2EFC_HG_MIN_TX_TIMERr, &val,
                              TIMERf, 20);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_HG_MIN_TX_TIMERr(unit,
                                                             port - 26, val));

            val = 0;
            soc_reg_field_set(unit, E2EFC_HG_MAX_TX_TIMERr, &val,
                              LGf, 1);
            soc_reg_field_set(unit, E2EFC_HG_MAX_TX_TIMERr, &val,
                              TIMERf, 640);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_HG_MAX_TX_TIMERr(unit,
                                                             port - 26, val));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit,
                              PKT_SET_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit,
                              CELL_SET_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_SET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit,
                              PKT_RESET_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit,
                              CELL_RESET_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_RESET_LIMITr(unit,
                                                             port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit,
                              PKT_DISC_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit,
                              CELL_DISC_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_DISC_LIMITr(unit, port, limit));
        } else {
            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit,
                              PKT_SET_LIMITf, 12);
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit,
                              CELL_SET_LIMITf, 80);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_SET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit,
                              PKT_RESET_LIMITf, 9);
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit,
                              CELL_RESET_LIMITf, 60);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_RESET_LIMITr(unit,
                                                             port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit,
                              PKT_DISC_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit,
                              CELL_DISC_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_DISC_LIMITr(unit, port, limit));
        }
    }

    /* device specific dynamic cell limit */
    if (dev_id != BCM56147_DEVICE_ID) {
        cell_limit = 0;
        soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                          &cell_limit, TOTALDYNCELLSETLIMITf, HU2_MMU_TOTAL_DYNAMIC_CELLS);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, cell_limit));
        cell_limit = 0;
        soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                          &cell_limit, TOTALDYNCELLRESETLIMITf, HU2_MMU_TOTAL_DYNAMIC_CELLS - 24);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, cell_limit));
    }
    else {
        cell_limit = 0;
        soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                          &cell_limit, TOTALDYNCELLSETLIMITf, 8576);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, cell_limit));
        cell_limit = 0;
        soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                      &cell_limit, TOTALDYNCELLRESETLIMITf, 8552);
        SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLRESETLIMITr(unit, cell_limit));

        /* E2EFC configuration */
        SOC_IF_ERROR_RETURN(READ_E2EFC_IBP_ENr(unit, &val));
        soc_reg_field_set(unit, E2EFC_IBP_ENr, &val, ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_E2EFC_IBP_ENr(unit, val));
   }

    /* 
     * Configure per-XQ packet aging for the various COSQs. 
     *
     * The shortest age allowed by H/W is 250 microseconds.
     * The longest age allowed is 7.162 seconds (7162 msec).
     * The maximum ratio between the longest age and the shortest 
     * (nonzero) age is 7:2.
     */
    age_enable = disabled_age = max_age = 0;
    min_age = 7162;
    for (cos = 0; cos < NUM_COS(unit); cos++) {
        if ((age[cos] = 
             soc_property_suffix_num_get(unit, cos, spn_MMU_XQ_AGING,
                                         "cos",  0)) > 0) {
            age_enable = 1;
            if (age[cos] > 7162) {
                age[cos] = 7162;
            }
            if (age[cos] < min_age) {
                min_age = age[cos];
            }
        } else {
            disabled_age = 1;
            age[cos] = 0;
        }
        if (age[cos] > max_age) {
            max_age = age[cos];
        }
    }
    if (!age_enable) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0)); /* Aesthetic */
    } else {
        uint32 regval = 0;
        uint32 timerval;

        /* Enforce the 7:2 ratio between min and max values */
        if ((((max_age * 2)+6) / 7) > min_age) {
            /* Keep requested max age; make min_age comply */ 
            min_age = ((max_age * 2) + 6) / 7; 
        }

        /* 
         * Give up granularity for range, if we need to
         * "disable" (max out) aging for any COSQ(s).
         */
        if (disabled_age) {
            /* Max range */
            max_age = min_age * 7 / 2;
        } 

        /* 
         * Compute shortest duration of one PKTAGINGTIMERr cycle. 
         * This duration is 1/7th of the longest packet age. 
         * This duration is in units of 125 usec (msec * 8).
         */
        timerval = ((8 * max_age) + 6) / 7; 
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, timerval));

        for (cos = 0; cos < NUM_COS(unit); cos++) {
            if (!age[cos]) {
                /* 
                 * Requested to be disabled, but cannot disable individual
                 * COSQs once packet aging is enabled. Therefore, mark
                 * this COSQ's aging duration as maxed out.
                 */
                age[cos] = -1;
            } else if (age[cos] < min_age) {
                age[cos] = min_age;
            }

            /* Normalize each "age" into # of PKTAGINGTIMERr cycles. */
            if (age[cos] > 0) {
                /* coverity[divide_by_zero : FALSE] */
                age[cos] = ((8 * age[cos]) + timerval - 1) / timerval;
            }
            else {
                age[cos] = 7;
            }
            /* Format each "age" for its appropriate field */
            regval |= ((7 - age[cos]) << (cos * 3));
        }
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, regval));
    }

    /* Apply TM=0x10 setting for all CAMs */
    tm=0x10;
    val = 0;
    soc_reg_field_set(unit, SW2_RAM_CONTROL_4r, &val,
                      CPU_COS_MAP_TCAM_TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_SW2_RAM_CONTROL_4r_REG32(unit, val));

    val = 0;
    soc_reg_field_set(unit, L2_USER_ENTRY_CAM_DBGCTRLr, &val,
                      TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_L2_USER_ENTRY_CAM_DBGCTRLr(unit, val));

    
    SOC_IF_ERROR_RETURN(READ_VLAN_SUBNET_CAM_DBGCTRLr_REG32(unit, &val));
    soc_reg_field_set(unit, VLAN_SUBNET_CAM_DBGCTRLr, &val,
                      TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_VLAN_SUBNET_CAM_DBGCTRLr_REG32(unit, val));
   
    /*
     * Port enable
     */
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLEr, &val, MMUPORTENABLEf,
                      SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLEr(unit, val));

    return SOC_E_NONE;
}

STATIC int
_hurricane2_ledup_init(int unit)
{
    int ix, pval1, pval2, pval3, pval4;
    uint32 rval = 0;
    struct led_remap {
       uint32 reg_addr;
       uint32 port0;
       uint32 port1;
       uint32 port2;
       uint32 port3;
    } led0_remap[] = {
        {CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f,REMAP_PORT_1f,REMAP_PORT_2f,REMAP_PORT_3f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f,REMAP_PORT_5f,REMAP_PORT_6f,REMAP_PORT_7f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f,REMAP_PORT_9f,REMAP_PORT_10f,REMAP_PORT_11f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f,REMAP_PORT_13f,REMAP_PORT_14f,REMAP_PORT_15f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f,REMAP_PORT_17f,REMAP_PORT_18f,REMAP_PORT_19f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f,REMAP_PORT_21f,REMAP_PORT_22f,REMAP_PORT_23f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f,REMAP_PORT_25f,REMAP_PORT_26f,REMAP_PORT_27f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f,REMAP_PORT_29f,REMAP_PORT_30f,REMAP_PORT_31f}
    };

    /* initialize the led remap register settings. 
     * port 0 entry will not be used for easy index of the physical port starting 1
     */   
    for (ix = 0; ix < sizeof(led0_remap)/sizeof(led0_remap[0]); ix++) {
        pval1 = (ix * 4) + 2;
        pval2 = pval1 + 1;
        pval3 = pval1 + 2;
        pval4 = pval1 + 3;
        if ((pval1 == 26) || (pval1 == 30)) {
            /* Transpose MXQ Block ports */
            pval4 = pval1;
            pval3 = pval4 + 1;
            pval2 = pval4 + 2;
            pval1 = pval1 + 3;
        }

        rval = 0;
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port0, pval1);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port1, pval2);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port2, pval3);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port3, pval4);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit, 
                soc_reg_addr(unit, led0_remap[ix].reg_addr, REG_PORT_ANY, 0), 
                rval));
    }

    /* Do Raw schan ops because Not all ports may be enabled */
    {
        /* XLPORT_LED_CHAIN_CONFIGr */
        _soc_reg32_set(unit, 5, 0, (0x2022b00), (0x00000006));
        _soc_reg32_set(unit, 6, 0, (0x2022b00), (0x00000006));
    }

    /* initialize the UP0 data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval));
    }

    /* Apply LEDUP delays. */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP0_CTRLr(unit, &rval));
    soc_reg_field_set(unit,CMIC_LEDUP0_CTRLr,
                      &rval, LEDUP_SCAN_START_DELAYf, 0x6);
    soc_reg_field_set(unit,CMIC_LEDUP0_CTRLr,
                      &rval, LEDUP_SCAN_INTRA_PORT_DELAYf, 0x5);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CTRLr(unit, rval));

    return SOC_E_NONE;  
}

STATIC int
_soc_hu2_power_Optimize(int unit) {
    int blk;
    uint32 rval;

    /* Turn on Per Stage Clock Gating */
    SOC_IF_ERROR_RETURN(READ_TOP_CLOCKING_ENFORCE_PSGr(unit,&rval));
    soc_reg_field_set(unit, TOP_CLOCKING_ENFORCE_PSGr, &rval,
                      CLK_ENFORCE_XLPORTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit,rval));

    /* Power Down Unused XLPORT Blocks */
    /* Since these regis are not accessible normally, use direct schan access */
    for (blk=5; blk<=6; blk++) {
        _soc_reg32_get(unit, blk,0, (0x02020b00), &rval); /*XLPORT_ENABLE_REG */
        if (rval == 0) { /* No Ports Enabled */
            rval = 0;
            soc_reg_field_set(unit, XLPORT_POWER_SAVEr, &rval,
                                   XPORT_CORE0f, 1);
            _soc_reg32_set(unit, blk,0, (0x02020d00), rval);

            _soc_reg32_get(unit, blk,0, (0x02021400), &rval);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval,
                                       PWRDWNf, 1);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval,
                                         IDDQf, 1);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval,
                                       TSCCLK_ENf, 0);
            _soc_reg32_set(unit, blk,0, (0x02021400), rval);
        }
    }
    
    return SOC_E_NONE;
}

#if defined(SER_TR_TEST_SUPPORT)
soc_ser_test_functions_t ser_hu2_test_fun;
#endif


int
soc_hurricane2_misc_init(int unit)
{
#define NUM_XLPORT 4
    static const soc_field_t port_field[] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    uint32              rval; 
    uint32              prev_reg_addr;
    uint64              reg64;
    int                 port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t         fields[3];
    uint32              values[3];
    int blk, bindex, mode;
    int phy_port_base;
    soc_info_t *si = &SOC_INFO(unit);
    uint16      dev_id;
    uint8       rev_id;
    int divisor, dividend;
    int hu2_config_id;
    uint32 qsgmii_override = 0;
    int core_clock_freq = 0, parity_enable;
    uint32 eee_duration_time_pulse = 0;
    pbmp_t blk_bitmap;
    int led_intensity = -1;
    uint32 fval;

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    
    soc_cm_get_id(unit, &dev_id, &rev_id);
    hu2_config_id = soc_property_get(unit, spn_BCM5615X_CONFIG, 0);

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_hurricane2_pipe_mem_clear(unit));
    }

    SOC_IF_ERROR_RETURN(soc_hu2_init_port_mapping(unit));

    
    /* Turn on ingress/egress/mmu parity */
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (soc_feature(unit, soc_feature_field_multi_stage)) {
        _soc_hu2_parity_group_info = _soc_hu2_parity_group_info_template;
    } else {
        _soc_hu2_parity_group_info = _soc_wf_parity_group_info_template;
    }
    if (parity_enable) {
        _soc_hurricane2_parity_enable(unit, TRUE);
#if defined(SER_TR_TEST_SUPPORT)
        soc_hu2_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/

        (void)_soc_hurricane2_tcam_ser_init(unit);
        memset(&_hu2_ser_functions, 0, sizeof(soc_ser_functions_t));
#ifdef INCLUDE_MEM_SCAN        
        soc_mem_scan_ser_list_register(unit, TRUE, _soc_hu2_tcam_ser_info); 
#endif /* INCLUDE_MEM_SCAN */
        memset(&_hu2_ser_functions, 0, sizeof(soc_ser_functions_t));
        _hu2_ser_functions._soc_ser_fail_f = &soc_hurricane2_ser_fail;
        _hu2_ser_functions._soc_ser_parity_error_cmicm_intr_f = 
            &soc_hurricane2_parity_error;
        _hu2_ser_functions._soc_ser_mem_nack_f = 
            &soc_hurricane2_mem_nack;
        soc_ser_function_register(unit, &_hu2_ser_functions);
    } else {
        _soc_hurricane2_parity_enable(unit, FALSE);
    }

    core_clock_freq = 0; /* 0 means run in native frequency */
    switch (dev_id) {
        case BCM56151_DEVICE_ID:
        case BCM53347_DEVICE_ID:
        case BCM53346_DEVICE_ID:
            if ((hu2_config_id == 0) ||	(hu2_config_id == 12) || \
                (hu2_config_id == 13)) {
                core_clock_freq = soc_property_get(unit, spn_CORE_CLOCK_SPEED, 0);
            }
            break;
        case BCM56152_DEVICE_ID:
            if ((hu2_config_id == 0) || (hu2_config_id == 13)) {
                core_clock_freq = soc_property_get(unit, spn_CORE_CLOCK_SPEED, 0);
            }
            break;
        case BCM53344_DEVICE_ID:
            if (hu2_config_id == 0) {
                core_clock_freq = soc_property_get(unit, spn_CORE_CLOCK_SPEED, 0);
            }
            break;
        case BCM53343_DEVICE_ID:
        case BCM53342_DEVICE_ID:
            core_clock_freq = soc_property_get(unit, spn_CORE_CLOCK_SPEED, 0);
            break;
        default :
            break;
    }

    eee_duration_time_pulse = 0x53; /* time pulse value for 1 us at 84.375MHz */
    if (core_clock_freq == 25) { /* core clock at 25MHz */
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_CORE_PLL_LOADf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL4r(unit, &rval));
        soc_reg_field_set(unit, TOP_CORE_PLL_CTRL4r, &rval, MSTR_CH0_MDIVf, 135);
        SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL4r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL2r(unit, &rval));
        soc_reg_field_set(unit, TOP_CORE_PLL_CTRL2r, &rval, LOAD_EN_CH0f, 1);
        soc_reg_field_set(unit, TOP_CORE_PLL_CTRL2r, &rval, LOAD_EN_CH1f, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL2r(unit, rval));

        eee_duration_time_pulse = 0x18; /* time pulse value for 1 us at 25MHz */
    } else { /* native 84.375 or 135 MHz */
        core_clock_freq = 84;
        /* Bump up core clock to 135 MHz if any TSC port is > 1G */
        for (port = 26; port <= 33; port++) { /* All TSC ports */
            if (port_speed_max[port] > 1000) {
                SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
                soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_CORE_PLL_LOADf, 1);
                SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(unit, rval));
    
                SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL4r(unit, &rval));
                soc_reg_field_set(unit, TOP_CORE_PLL_CTRL4r, &rval, MSTR_CH0_MDIVf, 25);
                SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL4r(unit, rval));
                
                SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL2r(unit, &rval));
                soc_reg_field_set(unit, TOP_CORE_PLL_CTRL2r, &rval, LOAD_EN_CH0f, 1);
                soc_reg_field_set(unit, TOP_CORE_PLL_CTRL2r, &rval, LOAD_EN_CH1f, 1);
                SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL2r(unit, rval));

                eee_duration_time_pulse = 0x86; /* time pulse value for 1 us at 135MHz */
                core_clock_freq = 135;
                break;
            }
        }
    }

    si->frequency = core_clock_freq;

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_GPORT) {
        blk_bitmap = SOC_BLOCK_BITMAP(unit, blk);
        PBMP_ITER(blk_bitmap, port) {
            SOC_IF_ERROR_RETURN(READ_GE0_EEE_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, GE0_EEE_CONFIGr, &rval, 
                        EEE_DURATION_TIMER_PULSEf, eee_duration_time_pulse);
            SOC_IF_ERROR_RETURN(WRITE_GE0_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE1_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE2_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE3_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE4_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE5_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE6_EEE_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_GE7_EEE_CONFIGr(unit, port, rval));
        }
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /* HR2 WAR for SDK-60041 : for GE port with UNIMAC only
    *  - Wrong UMAC_EEE_REF_COUNT configuration causes packet loss
    * 
    *  1. UMAC_EEE_REF_COUNT set to 0x1f4 for core_clk=500Mhz.
    *      for HR2, core_clk=tsc_pll_clk.
    *  2. Wake/Delay timer for GMII and MII register's reset value 
    *      need be revised.
    *      - WAKE_TIMER set to 0x1e for MII and 0x11 for GMII
    *      - DELAY_ENTRY_TIMER set to 0x3c for MII and 0x22 for GMII
    */
    PBMP_E_ITER(unit, port) {
        if (!IS_GE_PORT(unit, port) || IS_XL_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_UMAC_EEE_REF_COUNTr(unit, port, &rval));
        soc_reg_field_set(unit, 
                UMAC_EEE_REF_COUNTr, &rval, EEE_REF_COUNTf, 0x1f4);
        SOC_IF_ERROR_RETURN(WRITE_UMAC_EEE_REF_COUNTr(unit, port, rval));

        SOC_IF_ERROR_RETURN(WRITE_MII_EEE_WAKE_TIMERr(unit, port, 0x1e));
        SOC_IF_ERROR_RETURN(WRITE_GMII_EEE_WAKE_TIMERr(unit, port, 0x11));
        SOC_IF_ERROR_RETURN(WRITE_MII_EEE_DELAY_ENTRY_TIMERr(unit, port, 0x3c));
        SOC_IF_ERROR_RETURN(WRITE_GMII_EEE_DELAY_ENTRY_TIMERr(unit, port, 0x22));
    }

    /* Enable dual hash on L2 and L3 tables */
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

    /*
     * Egress Enable
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, SOC_INFO(unit).port_l2p_mapping[port], entry));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_64r, &reg64, PORT_BITMAP_LOf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_64r(unit, reg64));

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    prev_reg_addr = 0xffffffff;
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        si->port_num_lanes[port] = 1;
        if (!IS_GE_PORT(unit, port) || IS_XL_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
        switch (dev_id) {
            case BCM53393_DEVICE_ID:
            case BCM53394_DEVICE_ID:
                si->port_num_lanes[port] = 4;
                break;
            case BCM56150_DEVICE_ID:
            case BCM56151_DEVICE_ID:
            case BCM56152_DEVICE_ID:
            case BCM53347_DEVICE_ID:
            case BCM53346_DEVICE_ID:
            case BCM53344_DEVICE_ID:
                qsgmii_override = soc_property_get(unit,
                        spn_SERDES_QSGMII_SGMII_OVERRIDE, 0);
                if (qsgmii_override == 2) {
                    if (port == 18 || port == 22) {
                        si->port_num_lanes[port] = 4; 
                    }
                }
                break;
            default:
                break;
        }
    }
    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        if (!IS_GE_PORT(unit, port) || IS_XL_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }

    PBMP_PORT_ITER(unit, port) {
        if (!IS_HG_PORT(unit, port)) {
            continue;
        }
        si->port_num_lanes[port] = 1;
        SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1) {
            continue;
        }
        phy_port_base = si->port_l2p_mapping[port];

        /* Assert XLPORT soft reset */
        rval = 0;
        for (bindex = 0; bindex < NUM_XLPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

        mode = SOC_HU2_PORT_MODE_QUAD;

        switch (dev_id) {
            case BCM56150_DEVICE_ID:
            case BCM56151_DEVICE_ID:
            case BCM53346_DEVICE_ID:
            case BCM53347_DEVICE_ID:
                if ((hu2_config_id == 3) ||
                    ((hu2_config_id == 2) && (port !=26))) {
                    mode = SOC_HU2_PORT_MODE_SINGLE;
                    si->port_num_lanes[port] = 4;
                    si->port_num_lanes[port+1] = 4;
                    si->port_num_lanes[port+2] = 4;
                    si->port_num_lanes[port+3] = 4;
                }
                /* Fall Thru */
            case BCM56152_DEVICE_ID:
            case BCM53344_DEVICE_ID:
                if ((hu2_config_id == 1) || (hu2_config_id == 10)) {
                    if (port != 26) {
                        mode = SOC_HU2_PORT_MODE_DUAL;
                        si->port_num_lanes[port] = 2;
                        si->port_num_lanes[port+1] = 2;
                    }
                } else if (hu2_config_id == 11) {
                    if (port == 26) {
                        mode = SOC_HU2_PORT_MODE_DUAL;
                        si->port_num_lanes[port] = 2;
                        si->port_num_lanes[port+1] = 2;
                    }
                }
                break;
            case BCM53394_DEVICE_ID:
                if (hu2_config_id == 1) {
                    if (port == 26) {
                        mode = SOC_HU2_PORT_MODE_SINGLE;
                        si->port_num_lanes[port] = 4;
                        si->port_num_lanes[port+1] = 4;
                        si->port_num_lanes[port+2] = 4;
                        si->port_num_lanes[port+3] = 4;
                    }
                } else if (hu2_config_id == 2) {
                    if (port == 26) {
                        mode = SOC_HU2_PORT_MODE_SINGLE;
                        si->port_num_lanes[port] = 4;
                    }
                }
                break;
            default :
                break;
        }

        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                          XPORT0_CORE_PORT_MODEf, mode);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                          XPORT0_PHY_PORT_MODEf, mode);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        for (bindex = 0; bindex < NUM_XLPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
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

    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /*
     * Set reference clock (based on 200MHz core clock)
     * to be 200MHz * (1/40) = 5MHz
     */
    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, 40);
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

    /* Match the Internal MDC freq with above for External MDC */
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 40);
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, divisor);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, dividend);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    PBMP_E_ITER(unit, port) {
        if (!IS_GE_PORT(unit, port) || IS_XL_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        if (!IS_GE_PORT(unit, port) || IS_XL_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }

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

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    fields[1] = LAG_RES_ENf;
    values[0] = values[1] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                 REG_PORT_ANY, 2, fields, values));    

    /* Directed Mirroring ON by default except for CPU port */
    /* The src_port info will be changed as egress port if EM_SRCMOD_CHANGEf = 1 */
    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_EGR_PORTr(unit, port, &rval));
        soc_reg_field_set(unit, EGR_PORTr, &rval, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORTr(unit, port, rval));
        SOC_IF_ERROR_RETURN(READ_IEGR_PORTr(unit, port, &rval));
        soc_reg_field_set(unit, IEGR_PORTr, &rval, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IEGR_PORTr(unit, port, rval));
    }

#ifdef _HURRICANE2_QT_DEBUG
    if (soc_feature(unit, soc_feature_gphy)) {
        rval = 0;
        soc_reg_field_set(unit, GPORT_LINK_STATUS_TO_CMICr, &rval, TRANSPOSEf, 1);
        WRITE_GPORT_LINK_STATUS_TO_CMICr(unit, 0, rval);
    }
#endif

    /* initialize LED UP0 */
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_hurricane2_ledup_init(unit));
    }

    /* Turn on Power Optimization */
    SOC_IF_ERROR_RETURN(_soc_hu2_power_Optimize(unit));

    led_intensity = soc_property_get(unit, spn_LED_INTENSITY, -1);

    if ((led_intensity >= 1) && (led_intensity <= 12)) {
        fval = 0x0;
    } else if ((led_intensity >= 13) && (led_intensity <= 25)) {
        fval = 0x1;
    } else if ((led_intensity >= 26) && (led_intensity <= 37)) {
        fval = 0x2;
    } else if ((led_intensity >= 38) && (led_intensity <= 50)) {
        fval = 0x3;
    } else if ((led_intensity >= 51) && (led_intensity <= 62)) {
        fval = 0x4;
    } else if ((led_intensity >= 63) && (led_intensity <= 75)) {
        fval = 0x5;
    } else if ((led_intensity >= 76) && (led_intensity <= 88)) {
        fval = 0x6;
    } else if ((led_intensity >= 89) && (led_intensity <= 100)) {
        fval = 0x7;
    } else {
        fval = 0xffffffff; /* use chip default */
    }

	if (fval != 0xffffffff) {
        SOC_IF_ERROR_RETURN(READ_TOP_PARALLEL_LED_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_PARALLEL_LED_CTRLr, &rval, LED_CURRENT_SELf, fval);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PARALLEL_LED_CTRLr(unit, rval));
    }


    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit)); 
    }

    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
    }
    return SOC_E_NONE;
}

/* soc_hu2_mem_config:
 * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_hu2_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);

    switch (dev_id) {
        case BCM53342_DEVICE_ID:
        case BCM53343_DEVICE_ID:
        case BCM53344_DEVICE_ID:
        case BCM53346_DEVICE_ID:
        case BCM53347_DEVICE_ID:
        case BCM53393_DEVICE_ID:
        case BCM53394_DEVICE_ID:
            /* -- L2 related -- */
            sop->memState[L2MCm].index_max = 511;
            /* -- L3 related -- */
            sop->memState[L3_IPMCm].index_max = 63;
            sop->memState[L3_DEFIPm].index_max = 63;
            sop->memState[L3_DEFIP_ONLYm].index_max = 63;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 63;
            sop->memState[ING_L3_NEXT_HOPm].index_max = 511;
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 511;
            SOC_CONTROL(unit)->l3_defip_max_tcams = 1;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 64;
            break;
        default:
            SOC_CONTROL(unit)->l3_defip_max_tcams = 8;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 64;
            break;
    }
    return rv;
}

int
soc_hurricane2_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_hurricane2_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_hurricane2_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

void soc_hurricane2_oam_handler_register(int unit, soc_hurricane2_oam_handler_t handler)
{
    uint32 rval;
    int rv, fidx = 0;
    hu2_oam_handler[unit] = handler;
    rv = READ_IP1_INTR_ENABLEr(unit, &rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error reading %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_ENABLEr)));
    }
    while (_soc_hu2_oam_interrupt_fields[fidx] != INVALIDf) {
        soc_reg_field_set(unit, IP1_INTR_ENABLEr, &rval,
                          _soc_hu2_oam_interrupt_fields[fidx], 1);
        fidx++;
    }
    rv = WRITE_IP1_INTR_ENABLEr(unit, rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error writing %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_ENABLEr)));
    }                   
}

 #if defined(INCLUDE_L3)
 
 /* Hurricane2 HU2_LPM TCAM table insert/delete/lookup routines
  * soc_hu2_lpm_init - Called from bcm_l3_init
  * soc_hu2_lpm_insert - Insert/Update an IPv4/IPV6 route entry into HU2_LPM table
  * soc_hu2_lpm_delete - Delete an IPv4/IPV6 route entry from HU2_LPM table
  * soc_hu2_lpm_match  - (Exact match for the key. Will match both IP address
  *                                    and mask)
  * soc_hu2_lpm_lookup (HU2_LPM search) - Not Implemented
  */
 
 
 
 /*
  * TCAM based HU2_LPM implementation. Each table entry can hold one IPV4 entries or
  * one IPV6 entry. 
  *
  *              MAX_PFX_INDEX
  * lpm_prefix_index[98].begin ---> ===============================
  *                                 ==                           ==
  *                                 ==    0                      ==
  * lpm_prefix_index[98].end   ---> ===============================
  *
  * lpm_prefix_index[97].begin ---> ===============================
  *                                 ==                           ==
  *                                 ==    IPV6  Prefix Len = 64  ==
  * lpm_prefix_index[97].end   ---> ===============================
  *
  *
  *
  * lpm_prefix_index[x].begin --->  ===============================
  *                                 ==                           ==
  *                                 ==                           ==
  * lpm_prefix_index[x].end   --->  ===============================
  *
  *
  *              IPV6_PFX_ZERO
  * lpm_prefix_index[33].begin ---> ===============================
  *                                 ==                           ==
  *                                 ==    IPV6  Prefix Len = 0   ==
  * lpm_prefix_index[33].end   ---> ===============================
  *
  *
  * lpm_prefix_index[32].begin ---> ===============================
  *                                 ==                           ==
  *                                 ==    IPV4  Prefix Len = 32  ==
  * lpm_prefix_index[32].end   ---> ===============================
  *
  *
  *
  * lpm_prefix_index[0].begin --->  ===============================
  *                                 ==                           ==
  *                                 ==    IPV4  Prefix Len = 0   ==
  * lpm_prefix_index[0].end   --->  ===============================
 */
 
 #ifndef HU2_LPM_HASH_SUPPORT
 #define HU2_LPM_HASH_SUPPORT 1
 #endif
 
 #define SOC_MEM_COMPARE_RETURN(a, b) {          \
         if ((a) < (b)) { return -1; }           \
         if ((a) > (b)) { return  1; }           \
 }
 
 /* Can move to SOC Control structures */
 soc_hu2_lpm_state_p soc_hu2_lpm_state[SOC_MAX_NUM_DEVICES];
 
 #define SOC_HU2_LPM_LOCK(u)             soc_mem_lock(u, L3_DEFIPm)
 #define SOC_HU2_LPM_UNLOCK(u)           soc_mem_unlock(u, L3_DEFIPm)
 
 #define SOC_HU2_LPM_CACHE_FIELD_CREATE(m, f) soc_field_info_t * m##f
 #define SOC_HU2_LPM_CACHE_FIELD_ASSIGN(m_u, s, m, f) \
                 (s)->m##f = soc_mem_fieldinfo_get(m_u, m, f)
 typedef struct soc_hu2_lpm_field_cache_s {
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, HITf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, GLOBAL_ROUTE0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, CLASS_ID0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DST_DISCARD0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, SRC_DISCARD0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, RPE0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, PRI0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, DEFAULTROUTE0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_UNUSED0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP_PTR0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, UPPER_NEXT_HOP_INDEX0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, NEXT_HOP_INDEX0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, ECMP0f);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MASKf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_U_MASKf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_L_MASKf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODE_MASKf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_Uf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, IP_ADDR_Lf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, MODEf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VALIDf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_IDf);
     SOC_HU2_LPM_CACHE_FIELD_CREATE(L3_DEFIPm, VRF_ID_MASKf);
 } soc_hu2_lpm_field_cache_t, *soc_hu2_lpm_field_cache_p;
 static 
 soc_hu2_lpm_field_cache_p soc_hu2_lpm_field_cache_state[SOC_MAX_NUM_DEVICES];
 
 #define SOC_MEM_OPT_F32_GET(m_unit, m_mem, m_entry_data, m_field) \
         soc_meminfo_fieldinfo_field32_get( \
             (&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), \
             (soc_hu2_lpm_field_cache_state[(m_unit)]->m_mem##m_field))
 
 #define SOC_MEM_OPT_F32_SET(m_unit, m_mem, m_entry_data, m_field, m_val) \
         soc_meminfo_fieldinfo_field32_set( \
             (&SOC_MEM_INFO(m_unit, m_mem)), (m_entry_data), \
             (soc_hu2_lpm_field_cache_state[(m_unit)]->m_mem##m_field), (m_val))
 
 
 #define SOC_MEM_OPT_FIELD_VALID(m_unit, m_mem, m_field) \
             ((soc_hu2_lpm_field_cache_state[(m_unit)]->m_mem##m_field) != NULL)
 
 
 #ifdef HU2_LPM_HASH_SUPPORT
 typedef struct soc_hu2_lpm_hash_s {
     int         unit;
     int         entry_count;    /* Number entries in hash table */
     int         index_count;    /* Hash index max value + 1 */
     uint16      *table;         /* Hash table with 16 bit index */
     uint16      *link_table;    /* To handle collisions */
 } soc_hu2_lpm_hash_t;
 
/*
 * LPM key fields extended from 4 to 6 for VRF supporting 
 *  - for HR2/GH/HR3, the VRF also the LPM key for hash but static at vrf=0
 */
#define NUM_KEY_FIELDS    6 
typedef uint32 soc_hu2_lpm_hash_entry_t[NUM_KEY_FIELDS];
 typedef int (*soc_hu2_lpm_hash_compare_fn)(soc_hu2_lpm_hash_entry_t key1,
                                            soc_hu2_lpm_hash_entry_t key2);
 static soc_hu2_lpm_hash_t *soc_hu2_lpm_hash_tab[SOC_MAX_NUM_DEVICES];
 #define SOC_HU2_LPM_STATE_HASH(u)           (soc_hu2_lpm_hash_tab[(u)])
 
 #define HU2_LPM_HASH_INDEX_NULL  (0xFFFF)
 #define HU2_LPM_HASH_INDEX_MASK  (0x3FFF)
 #define HU2_LPM_HASH_IPV6_MASK   (0x8000)
 
 #define SOC_HU2_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, odata)                 \
     odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_Lf);     \
     odata[1] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_L_MASKf);\
     odata[2] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_Uf);     \
     odata[3] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_U_MASKf);\
     if ((SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_IDf))) {                   \
        odata[4] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_IDf);     \
        soc_hu2_lpm_hash_vrf_get(u, entry_data, &odata[5]);                    \
     } else {                                                                  \
        odata[4] = 0;                                                          \
        odata[5] = 0;                                                          \
     }
 
 #define SOC_HU2_LPM_HASH_ENTRY_IPV4_GET(u, entry_data, odata)                  \
     odata[0] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_Lf);      \
     odata[1] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, IP_ADDR_L_MASKf); \
     odata[2] = 0;                                                              \
     odata[3] = 0x80000001;                                                     \
     if ((SOC_MEM_OPT_FIELD_VALID(u, L3_DEFIPm, VRF_IDf))) {                    \
        odata[4] = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VRF_IDf);      \
        soc_hu2_lpm_hash_vrf_get(u, entry_data, &odata[5]);                     \
     } else {                                                                   \
        odata[4] = 0;                                                           \
        odata[5] = 0;                                                           \
     }
 
 #define SOC_HU2_LPM_HASH_ENTRY_GET soc_hu2_lpm_hash_entry_get
 
 static
 void soc_hu2_lpm_hash_entry_get(int u, void *e,
                                 int index, soc_hu2_lpm_hash_entry_t r_entry);
 static
 uint16 soc_hu2_lpm_hash_compute(uint8 *data, int data_nbits);
 static
 int soc_hu2_lpm_hash_create(int unit,
                             int entry_count,
                             int index_count,
                             soc_hu2_lpm_hash_t **hu2_lpm_hash_ptr);
 static
 int soc_hu2_lpm_hash_destroy(soc_hu2_lpm_hash_t *hu2_lpm_hash);
 static
 int _soc_hu2_lpm_hash_lookup(soc_hu2_lpm_hash_t          *hash,
                             soc_hu2_lpm_hash_compare_fn key_cmp_fn,
                             soc_hu2_lpm_hash_entry_t    entry,
                             int                         pfx,
                             uint16                      *key_index);
 static
 int _soc_hu2_lpm_hash_insert(soc_hu2_lpm_hash_t *hash,
                             soc_hu2_lpm_hash_compare_fn key_cmp_fn,
                             soc_hu2_lpm_hash_entry_t entry,
                             int    pfx,
                             uint16 old_index,
                             uint16 new_index);
 
 static
 int _soc_hu2_lpm_hash_delete(soc_hu2_lpm_hash_t *hash,
                             soc_hu2_lpm_hash_compare_fn key_cmp_fn,
                             soc_hu2_lpm_hash_entry_t entry,
                             int    pfx,
                             uint16 delete_index);

/*
 * Function:
 *      soc_hu2_lpm_hash_vrf_get
 * Purpose:
 *      Service routine used to determine the vrf type 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_entry - (IN)Route info buffer from hw.
 *      vrf_id    - (OUT)Virtual router id.
 * Returns:
 *      BCM_E_XXX
 * Note :
 * - soc_hu2_lpm_hash_xxx() are SW LPM hash routines for HRx/GHx devices.
 * - VRF is once of the key in LPM but only be the valid field after GH2.
 */
STATIC void
soc_hu2_lpm_hash_vrf_get(int unit, void *lpm_entry, uint32 *vrf)
{
    int vrf_id;

    /* Get Virtual Router id if supported. */
    if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, VRF_ID_MASKf)){
        vrf_id = SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_IDf);

        /* Special vrf's handling. */
        if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, VRF_ID_MASKf)) {
            *vrf = _SOC_HASH_L3_VRF_SPECIFIC;    
        } else if (SOC_VRF_MAX(unit) == vrf_id) {
            *vrf = _SOC_HASH_L3_VRF_GLOBAL;
        } else {
            *vrf = _SOC_HASH_L3_VRF_OVERRIDE;    
            if (SOC_MEM_OPT_FIELD_VALID(unit, L3_DEFIPm, GLOBAL_ROUTE0f)) {
                if (SOC_MEM_OPT_F32_GET(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f)) {
                    *vrf = _SOC_HASH_L3_VRF_GLOBAL; 
                }
            }
        }
    } else {
        /* No vrf support on this device. */
        *vrf = _SOC_HASH_L3_VRF_DEFAULT;
    }
}
 
 /*
  *      Extract key data from an entry at the given index.
  */
 static
 void soc_hu2_lpm_hash_entry_get(int u, void *e,
                                 int index, soc_hu2_lpm_hash_entry_t r_entry)
 {
     int                         is_ipv6;

     is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, MODEf);

     if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALIDf)) {
         if (is_ipv6) {
            SOC_HU2_LPM_HASH_ENTRY_IPV6_GET(u, e, r_entry);
         } else {
            SOC_HU2_LPM_HASH_ENTRY_IPV4_GET(u, e, r_entry);
         }
     }
 }
 
 /*
  * Function:
  *      soc_hu2_lpm_hash_compare_key
  * Purpose:
  *      Comparison function for hash table operations.
  */
 static
 int soc_hu2_lpm_hash_compare_key(soc_hu2_lpm_hash_entry_t key1,
                                  soc_hu2_lpm_hash_entry_t key2)
 {
     int idx;
 
     for (idx = 0; idx < NUM_KEY_FIELDS; idx++) { 
         SOC_MEM_COMPARE_RETURN(key1[idx], key2[idx]);
     }
     return (0);
 }
 
 /* #define HU2_LPM_DEBUG*/
 #ifdef HU2_LPM_DEBUG      
 #define H_INDEX_MATCH(str, tab_index, match_index)      \
         LOG_ERROR(BSL_LS_SOC_COMMON, \
                   (BSL_META_U(unit, \
                               "%s index: H %d A %d\n"),   \
                               str, (int)tab_index, match_index)
 #else
 #define H_INDEX_MATCH(str, tab_index, match_index)
 #endif
 
 #define HU2_LPM_NO_MATCH_INDEX 0x4000
 #define HU2_LPM_HASH_INSERT(u, entry_data, tab_index)       \
     soc_hu2_lpm_hash_insert(u, entry_data, tab_index, HU2_LPM_NO_MATCH_INDEX, 0)
 
 #define HU2_LPM_HASH_DELETE(u, key_data, tab_index)         \
     soc_hu2_lpm_hash_delete(u, key_data, tab_index)
 
 #define HU2_LPM_HASH_LOOKUP(u, key_data, pfx, tab_index)    \
     soc_hu2_lpm_hash_lookup(u, key_data, pfx, tab_index)
 
 void soc_hu2_lpm_hash_insert(int u, void *entry_data, uint32 tab_index,
                             uint32 old_index, int pfx)
 {
     soc_hu2_lpm_hash_entry_t    key_hash;
 
     if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, VALIDf)) {
         if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry_data, MODEf)) {
             /* IPV6 entry */
             SOC_HU2_LPM_HASH_ENTRY_IPV6_GET(u, entry_data, key_hash);
         } else {
             /* IPV4 entry */
             SOC_HU2_LPM_HASH_ENTRY_IPV4_GET(u, entry_data, key_hash);
         }
         _soc_hu2_lpm_hash_insert(SOC_HU2_LPM_STATE_HASH(u),
                                 soc_hu2_lpm_hash_compare_key,
                                 key_hash,
                                 pfx,
                                 old_index,
                                 (uint16)tab_index);
     }
 }
 
 void soc_hu2_lpm_hash_delete(int u, void *key_data, uint32 tab_index)
 {
     soc_hu2_lpm_hash_entry_t    key_hash;
     int                         pfx = -1;
     int                         rv;
     uint16                      index;
 
     if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODEf)) {
         SOC_HU2_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
     } else {
         SOC_HU2_LPM_HASH_ENTRY_IPV4_GET(u, key_data, key_hash);
     }
     index = tab_index;
 
     rv = _soc_hu2_lpm_hash_delete(SOC_HU2_LPM_STATE_HASH(u),
                                  soc_hu2_lpm_hash_compare_key,
                                  key_hash, pfx, index);
     if (SOC_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_SOC_COMMON,
                   (BSL_META_U(u,
                               "\ndel  index: H %d error %d\n"), index, rv));
     }
 }
 
 int soc_hu2_lpm_hash_lookup(int u, void *key_data, int pfx, int *key_index)
 {
     soc_hu2_lpm_hash_entry_t    key_hash;
     int                         is_ipv6;
     int                         rv;
     uint16                      index = HU2_LPM_HASH_INDEX_NULL;
 
     is_ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, key_data, MODEf);
     if (is_ipv6) {
         SOC_HU2_LPM_HASH_ENTRY_IPV6_GET(u, key_data, key_hash);
     } else {
         SOC_HU2_LPM_HASH_ENTRY_IPV4_GET(u, key_data, key_hash);
     }
 
     rv = _soc_hu2_lpm_hash_lookup(SOC_HU2_LPM_STATE_HASH(u),
                                  soc_hu2_lpm_hash_compare_key,
                                  key_hash, pfx, &index);
     if (SOC_FAILURE(rv)) {
         *key_index = 0xFFFFFFFF;
         return(rv);
     }
 
     *key_index = index;
 
     return(SOC_E_NONE);
 }
 
 /* 
  * Function:
  *      soc_hu2_lpm_hash_compute
  * Purpose:
  *      Compute CRC hash for key data.
  * Parameters:
  *      data - Key data
  *      data_nbits - Number of data bits
  * Returns:
  *      Computed 16 bit hash
  */
 static
 uint16 soc_hu2_lpm_hash_compute(uint8 *data, int data_nbits)
 {
     return (_shr_crc16b(0, data, data_nbits));
 }
 
 /* 
  * Function:
  *      soc_hu2_lpm_hash_create
  * Purpose:
  *      Create an empty hash table
  * Parameters:
  *      unit  - Device unit
  *      entry_count - Limit for number of entries in table
  *      index_count - Hash index max + 1. (index_count <= count)
  *      hu2_lpm_hash_ptr - Return pointer (handle) to new Hash Table
  * Returns:
  *      SOC_E_NONE       Success
  *      SOC_E_MEMORY     Out of memory (system allocator)
  */
 
 static
 int soc_hu2_lpm_hash_create(int unit,
                             int entry_count,
                             int index_count,
                             soc_hu2_lpm_hash_t **hu2_lpm_hash_ptr)
 {
     soc_hu2_lpm_hash_t  *hash;
     int                 index;
 
     if (index_count > entry_count) {
         return SOC_E_MEMORY;
     }
     hash = sal_alloc(sizeof (soc_hu2_lpm_hash_t), "lpm_hash");
     if (hash == NULL) {
         return SOC_E_MEMORY;
     }
 
     sal_memset(hash, 0, sizeof (*hash));
 
     hash->unit = unit;
     hash->entry_count = entry_count;
     hash->index_count = index_count;
 
     /*
      * Pre-allocate the hash table storage.
      */
     hash->table = sal_alloc(hash->index_count * sizeof(*(hash->table)),
                             "hash_table");
 
     if (hash->table == NULL) {
         sal_free(hash);
         return SOC_E_MEMORY;
     }
     /*
      * In case where all the entries should hash into the same bucket
      * this will prevent the hash table overflow
      */
     hash->link_table = sal_alloc(
                             hash->entry_count * sizeof(*(hash->link_table)),
                             "link_table");
     if (hash->link_table == NULL) {
         sal_free(hash->table);
         sal_free(hash);
         return SOC_E_MEMORY;
     }
 
     /*
      * Set the entries in the hash table to HU2_LPM_HASH_INDEX_NULL
      * Link the entries beyond hash->index_max for handling collisions
      */
     for(index = 0; index < hash->index_count; index++) {
         hash->table[index] = HU2_LPM_HASH_INDEX_NULL;
     }
     for(index = 0; index < hash->entry_count; index++) {
         hash->link_table[index] = HU2_LPM_HASH_INDEX_NULL;
     }
     *hu2_lpm_hash_ptr = hash;
     return SOC_E_NONE;
 }
 
 /* 
  * Function:
  *      soc_hu2_lpm_hash_destroy
  * Purpose:
  *      Destroy the hash table
  * Parameters:
  *      hu2_lpm_hash - Pointer (handle) to Hash Table
  * Returns:
  *      SOC_E_NONE       Success
  */
 static
 int soc_hu2_lpm_hash_destroy(soc_hu2_lpm_hash_t *hu2_lpm_hash)
 {
     if (hu2_lpm_hash != NULL) {
         sal_free(hu2_lpm_hash->table);
         sal_free(hu2_lpm_hash->link_table);
         sal_free(hu2_lpm_hash);
     }
 
     return SOC_E_NONE;
 }
 
 /*
  * Function:
  *      _soc_hu2_lpm_hash_lookup
  * Purpose:
  *      Look up a key in the hash table
  * Parameters:
  *      hash - Pointer (handle) to Hash Table
  *      key_cmp_fn - Compare function which should compare key
  *      entry   - The key to lookup
  *      pfx     - Prefix length for lookup acceleration.
  *      key_index - (OUT)       Index where the key was found.
  * Returns:
  *      SOC_E_NONE      Key found
  *      SOC_E_NOT_FOUND Key not found
  */
 
 static
 int _soc_hu2_lpm_hash_lookup(soc_hu2_lpm_hash_t  *hash,
                             soc_hu2_lpm_hash_compare_fn key_cmp_fn,
                             soc_hu2_lpm_hash_entry_t    entry,
                             int                          pfx,
                             uint16                       *key_index)
 {
     int u = hash->unit;
 
     uint16 hash_val;
     uint16 index;
 
     hash_val = soc_hu2_lpm_hash_compute((uint8 *)entry,
                                     (32 * NUM_KEY_FIELDS)) % hash->index_count;
     index = hash->table[hash_val];
     H_INDEX_MATCH("lhash", entry[0], hash_val);
     H_INDEX_MATCH("lkup ", entry[0], index);
     while(index != HU2_LPM_HASH_INDEX_NULL) {
         uint32  e[SOC_MAX_MEM_FIELD_WORDS];
         soc_hu2_lpm_hash_entry_t  r_entry;
         /*int     rindex;
 
         rindex = (index & HU2_LPM_HASH_INDEX_MASK) >> 1;*/
 
         SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(u, MEM_BLOCK_ANY, index, e));
         SOC_HU2_LPM_HASH_ENTRY_GET(u, e, index, r_entry);
         if ((*key_cmp_fn)(entry, r_entry) == 0) {
             *key_index = (index & HU2_LPM_HASH_INDEX_MASK);
             H_INDEX_MATCH("found", entry[0], index);
             return(SOC_E_NONE);
         }
         
         index = hash->link_table[index & HU2_LPM_HASH_INDEX_MASK];
         H_INDEX_MATCH("lkup1", entry[0], index);
     }
     H_INDEX_MATCH("not_found", entry[0], index);
     return(SOC_E_NOT_FOUND);
 }
 
 /*
  * Function:
  *      _soc_hu2_lpm_hash_insert
  * Purpose:
  *      Insert/Update a key index in the hash table
  * Parameters:
  *      hash - Pointer (handle) to Hash Table
  *      key_cmp_fn - Compare function which should compare key
  *      entry   - The key to lookup
  *      pfx     - Prefix length for lookup acceleration.
  *      old_index - Index where the key was moved from.
  *                  HU2_LPM_HASH_INDEX_NULL if new entry.
  *      new_index - Index where the key was moved to.
  * Returns:
  *      SOC_E_NONE      Key found
  */
 /*
  *      Should be caled before updating the HU2_LPM table so that the
  *      data in the hash table is consistent with the HU2_LPM table
  */
 static
 int _soc_hu2_lpm_hash_insert(soc_hu2_lpm_hash_t *hash,
                             soc_hu2_lpm_hash_compare_fn key_cmp_fn,
                             soc_hu2_lpm_hash_entry_t entry,
                             int    pfx,
                             uint16 old_index,
                             uint16 new_index)
 {
 
 #define INDEX_ADD(hash, hash_idx, new_idx)                      \
     hash->link_table[new_idx & HU2_LPM_HASH_INDEX_MASK] =       \
         hash->table[hash_idx];                                  \
     hash->table[hash_idx] = new_idx
 
 #define INDEX_ADD_LINK(hash, t_index, new_idx)                  \
     hash->link_table[new_idx & HU2_LPM_HASH_INDEX_MASK] =       \
         hash->link_table[t_index & HU2_LPM_HASH_INDEX_MASK];    \
     hash->link_table[t_index & HU2_LPM_HASH_INDEX_MASK] = new_idx
 
 #define INDEX_UPDATE(hash, hash_idx, old_idx, new_idx)          \
     hash->table[hash_idx] = new_idx;                            \
     hash->link_table[new_idx & HU2_LPM_HASH_INDEX_MASK] =       \
         hash->link_table[old_idx & HU2_LPM_HASH_INDEX_MASK];    \
     hash->link_table[old_idx & HU2_LPM_HASH_INDEX_MASK] =       \
         HU2_LPM_HASH_INDEX_NULL
 
 #define INDEX_UPDATE_LINK(hash, prev_idx, old_idx, new_idx)         \
     hash->link_table[prev_idx & HU2_LPM_HASH_INDEX_MASK] = new_idx; \
     hash->link_table[new_idx & HU2_LPM_HASH_INDEX_MASK] =           \
         hash->link_table[old_idx & HU2_LPM_HASH_INDEX_MASK];        \
     hash->link_table[old_idx & HU2_LPM_HASH_INDEX_MASK] =           \
         HU2_LPM_HASH_INDEX_NULL
 
 
     int u = hash->unit;
 
     uint16 hash_val;
     uint16 index;
     uint16 prev_index;
 
     hash_val = soc_hu2_lpm_hash_compute((uint8 *)entry,
                                     (32 * NUM_KEY_FIELDS)) % hash->index_count;
     index = hash->table[hash_val];
     H_INDEX_MATCH("ihash", entry[0], hash_val);
     H_INDEX_MATCH("ins  ", entry[0], new_index);
     H_INDEX_MATCH("ins1 ", index, new_index);
     prev_index = HU2_LPM_HASH_INDEX_NULL;
     if (old_index != HU2_LPM_HASH_INDEX_NULL) {
         while(index != HU2_LPM_HASH_INDEX_NULL) {
             uint32  e[SOC_MAX_MEM_FIELD_WORDS];
             soc_hu2_lpm_hash_entry_t  r_entry;
             /*int     rindex;
             
             rindex = (index & HU2_LPM_HASH_INDEX_MASK) >> 1; */
 
             SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(u, MEM_BLOCK_ANY, index, e));
             SOC_HU2_LPM_HASH_ENTRY_GET(u, e, index, r_entry);
             if ((*key_cmp_fn)(entry, r_entry) == 0) {
                 /* assert(old_index == index);*/
                 if (new_index != index) {
                     H_INDEX_MATCH("imove", prev_index, new_index);
                     if (prev_index == HU2_LPM_HASH_INDEX_NULL) {
                         INDEX_UPDATE(hash, hash_val, index, new_index);
                     } else {
                         INDEX_UPDATE_LINK(hash, prev_index, index, new_index);
                     }
                 }
                 H_INDEX_MATCH("imtch", index, new_index);
                 return(SOC_E_NONE);
             }
             prev_index = index;
             index = hash->link_table[index & HU2_LPM_HASH_INDEX_MASK];
             H_INDEX_MATCH("ins2 ", index, new_index);
         }
     }
     INDEX_ADD(hash, hash_val, new_index);  /* new entry */
     return(SOC_E_NONE);
 }
 
 /*
  * Function:
  *      _soc_hu2_lpm_hash_delete
  * Purpose:
  *      Delete a key index in the hash table
  * Parameters:
  *      hash - Pointer (handle) to Hash Table
  *      key_cmp_fn - Compare function which should compare key
  *      entry   - The key to delete
  *      pfx     - Prefix length for lookup acceleration.
  *      delete_index - Index to delete.
  * Returns:
  *      SOC_E_NONE      Success
  *      SOC_E_NOT_FOUND Key index not found.
  */
 static
 int _soc_hu2_lpm_hash_delete(soc_hu2_lpm_hash_t *hash,
                             soc_hu2_lpm_hash_compare_fn key_cmp_fn,
                             soc_hu2_lpm_hash_entry_t entry,
                             int    pfx,
                             uint16 delete_index)
 {
     uint16 hash_val;
     uint16 index;
     uint16 prev_index;
 
     hash_val = soc_hu2_lpm_hash_compute((uint8 *)entry,
                                     (32 * NUM_KEY_FIELDS)) % hash->index_count;
     index = hash->table[hash_val];
     H_INDEX_MATCH("dhash", entry[0], hash_val);
     H_INDEX_MATCH("del  ", entry[0], index);
     prev_index = HU2_LPM_HASH_INDEX_NULL;
     while(index != HU2_LPM_HASH_INDEX_NULL) {
 #define INDEX_DELETE(hash, hash_idx, del_idx)                   \
     hash->table[hash_idx] =                                     \
         hash->link_table[del_idx & HU2_LPM_HASH_INDEX_MASK];     \
     hash->link_table[del_idx & HU2_LPM_HASH_INDEX_MASK] =        \
         HU2_LPM_HASH_INDEX_NULL
 
 #define INDEX_DELETE_LINK(hash, prev_idx, del_idx)              \
     hash->link_table[prev_idx & HU2_LPM_HASH_INDEX_MASK] =       \
         hash->link_table[del_idx & HU2_LPM_HASH_INDEX_MASK];     \
     hash->link_table[del_idx & HU2_LPM_HASH_INDEX_MASK] =        \
         HU2_LPM_HASH_INDEX_NULL
 
         if (delete_index == index) {
             H_INDEX_MATCH("dfoun", entry[0], index);
             if (prev_index == HU2_LPM_HASH_INDEX_NULL) {
                 INDEX_DELETE(hash, hash_val, delete_index);
             } else {
                 INDEX_DELETE_LINK(hash, prev_index, delete_index);
             }
             return(SOC_E_NONE);
         }
         prev_index = index;
         index = hash->link_table[index & HU2_LPM_HASH_INDEX_MASK];
         H_INDEX_MATCH("del1 ", entry[0], index);
     }
     return(SOC_E_NOT_FOUND);
 }
 #else
 #define HU2_LPM_HASH_INSERT(u, entry_data, tab_index)
 #define HU2_LPM_HASH_DELETE(u, key_data, tab_index)
 #define HU2_LPM_HASH_LOOKUP(u, key_data, pfx, tab_index)
 #endif /* HU2_LPM_HASH_SUPPORT */
 
 static
 int
 _ip_mask2pfx(uint32 ipv4m, int *mask_len)
 {
     *mask_len = 0;
     while (ipv4m & (1 << 31)) {
         *mask_len += 1;
         ipv4m <<= 1;
     }
 
     return ((ipv4m) ? SOC_E_PARAM : SOC_E_NONE);
 }
 
 void
 soc_hu2_lpm_state_dump(int u)
 {
     int i;
     int max_pfx_len;
     max_pfx_len = MAX_PFX_INDEX(u);
 
    if (!bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityVerbose, u)) {
         return;
     }
     for(i = max_pfx_len; i >= 0 ; i--) {
         if ((i != MAX_PFX_INDEX(u)) && (SOC_HU2_LPM_STATE_START(u, i) == -1)) {
             continue;
         }
         LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                     (BSL_META_U(u,
                                 "PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),
                      i,
                      SOC_HU2_LPM_STATE_PREV(u, i),
                      SOC_HU2_LPM_STATE_NEXT(u, i),
                      SOC_HU2_LPM_STATE_START(u, i),
                      SOC_HU2_LPM_STATE_END(u, i),
                      SOC_HU2_LPM_STATE_VENT(u, i),
                      SOC_HU2_LPM_STATE_FENT(u, i)));
     }
 }
 
#ifdef BCM_HURRICANE3_SUPPORT
/*
 *      Replicate entry to the second half of the tcam if URPF check is ON. 
 */
static 
int _lpm_hr3_urpf_entry_replicate(int u, int index, uint32 *e)
{
    int src_tcam_offset;  /* Defip memory size/2 urpf source lookup offset */
    int ipv6;             /* IPv6 entry.                                   */
    uint32 mask0;         /* Mask field value.                             */
    uint32 mask1;         /* Mask field value.                             */
    int def_gw_flag;      /* Entry is default gateway.                     */ 

    if(!SOC_URPF_STATUS_GET(u)) {
        return (SOC_E_NONE);
    }

    src_tcam_offset = (soc_mem_index_count(u, L3_DEFIPm) >> 1);

    ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, MODEf);

    /* Reset destination discard bit. */
    SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DST_DISCARD0f, 0);

    /* Set/Reset default gateway flag based on ip mask value. */
    mask0 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_L_MASKf);
    mask1 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, IP_ADDR_U_MASKf);

    if (!ipv6) {
        if (SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, e, VALIDf)) {
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, (!mask0) ? 1 : 0);
            SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e,
                DEFAULTROUTE0f, (!mask0) ? 1 : 0);
        }
    } else {
        def_gw_flag = ((!mask0) &&  (!mask1)) ? 1 : 0;
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, RPE0f, def_gw_flag);
        SOC_MEM_OPT_F32_SET(u, L3_DEFIPm, e, DEFAULTROUTE0f, def_gw_flag);
    }

    /* Write entry to the second half of the tcam. */
    return WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, (index + src_tcam_offset), e);
}
#endif  /* BCM_HURRICANE3_SUPPORT */

 /*
  *      Create a slot for the new entry rippling the entries if required
  */
 static
 int _lpm_hu2_entry_shift(int u, int from_ent, int to_ent)
 {
     uint32      e[SOC_MAX_MEM_FIELD_WORDS];
 
 #ifdef HU2_LPM_TABLE_CACHED
     SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(u, L3_DEFIPm,
                                        MEM_BLOCK_ANY, from_ent));
 #endif /* HU2_LPM_TABLE_CACHED */
 
     SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, e));
     HU2_LPM_HASH_INSERT(u, e, to_ent);
     SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, e));
#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(u, soc_feature_urpf)) {
        SOC_IF_ERROR_RETURN(_lpm_hr3_urpf_entry_replicate(u, to_ent, e));
    }
#endif  /* BCM_HURRICANE3_SUPPORT */
     return (SOC_E_NONE);
 }
 
 
 /*
  *      Shift prefix entries 1 entry UP, while preserving  
  *      last half empty IPv4 entry if any.
  */
 static
 int _lpm_hu2_shift_pfx_up(int u, int pfx)
 {
     int         from_ent;
     int         to_ent;
 
     to_ent = SOC_HU2_LPM_STATE_END(u, pfx) + 1;
 
     from_ent = SOC_HU2_LPM_STATE_START(u, pfx);
     if(from_ent != to_ent) {
         SOC_IF_ERROR_RETURN(_lpm_hu2_entry_shift(u, from_ent, to_ent));
     } 
     SOC_HU2_LPM_STATE_START(u, pfx) += 1;
     SOC_HU2_LPM_STATE_END(u, pfx) += 1;
     return (SOC_E_NONE);
 }
 
 /*
  *      Shift prefix entries 1 entry DOWN, while preserving  
  *      last half empty IPv4 entry if any.
  */
 static
 int _lpm_hu2_shift_pfx_down(int u, int pfx)
 {
     int         from_ent;
     int         to_ent;
 
 
     to_ent = SOC_HU2_LPM_STATE_START(u, pfx) - 1;
 
     /* Don't move empty prefix . */ 
     if (SOC_HU2_LPM_STATE_VENT(u, pfx) == 0) {
         SOC_HU2_LPM_STATE_START(u, pfx) = to_ent;
         SOC_HU2_LPM_STATE_END(u, pfx) = to_ent - 1;
         return (SOC_E_NONE);
     }
   
     from_ent = SOC_HU2_LPM_STATE_END(u, pfx);
     SOC_IF_ERROR_RETURN(_lpm_hu2_entry_shift(u, from_ent, to_ent));
     SOC_HU2_LPM_STATE_START(u, pfx) -= 1;
     SOC_HU2_LPM_STATE_END(u, pfx) -= 1;
 
     return (SOC_E_NONE);
 }
 
 /*
  *      Create a slot for the new entry rippling the entries if required
  */
 static
 int _lpm_hu2_free_slot_create(int u, int pfx, void *e, int *free_slot)
 {
     int         prev_pfx;
     int         next_pfx;
     int         free_pfx;
     int         curr_pfx; 
 
     if (SOC_HU2_LPM_STATE_VENT(u, pfx) == 0) {
         /*
          * Find the  prefix position. Only prefix with valid
          * entries are in the list.
          * next -> high to low prefix. low to high index
          * prev -> low to high prefix. high to low index
          * Unused prefix length MAX_PFX_INDEX is the head of the
          * list and is node corresponding to this is always
          * present.
          */
         curr_pfx = MAX_PFX_INDEX(u);
         while (SOC_HU2_LPM_STATE_NEXT(u, curr_pfx) > pfx) {
             curr_pfx = SOC_HU2_LPM_STATE_NEXT(u, curr_pfx);
         }
         /* Insert the new prefix */
         next_pfx = SOC_HU2_LPM_STATE_NEXT(u, curr_pfx);
         if (next_pfx != -1) {
             SOC_HU2_LPM_STATE_PREV(u, next_pfx) = pfx;
         }
         SOC_HU2_LPM_STATE_NEXT(u, pfx) = SOC_HU2_LPM_STATE_NEXT(u, curr_pfx);
         SOC_HU2_LPM_STATE_PREV(u, pfx) = curr_pfx;
         SOC_HU2_LPM_STATE_NEXT(u, curr_pfx) = pfx;
 
         SOC_HU2_LPM_STATE_FENT(u, pfx) =  (SOC_HU2_LPM_STATE_FENT(u, curr_pfx) + 1) / 2;
         SOC_HU2_LPM_STATE_FENT(u, curr_pfx) -= SOC_HU2_LPM_STATE_FENT(u, pfx);
         SOC_HU2_LPM_STATE_START(u, pfx) =  SOC_HU2_LPM_STATE_END(u, curr_pfx) +
                                        SOC_HU2_LPM_STATE_FENT(u, curr_pfx) + 1;
         SOC_HU2_LPM_STATE_END(u, pfx) = SOC_HU2_LPM_STATE_START(u, pfx) - 1;
         SOC_HU2_LPM_STATE_VENT(u, pfx) = 0;
     }
 
     free_pfx = pfx;
     while(SOC_HU2_LPM_STATE_FENT(u, free_pfx) == 0) {
         free_pfx = SOC_HU2_LPM_STATE_NEXT(u, free_pfx);
         if (free_pfx == -1) {
             /* No free entries on this side try the other side */
             free_pfx = pfx;
             break;
         }
     }
 
     while(SOC_HU2_LPM_STATE_FENT(u, free_pfx) == 0) {
         free_pfx = SOC_HU2_LPM_STATE_PREV(u, free_pfx);
         if (free_pfx == -1) {
             if (SOC_HU2_LPM_STATE_VENT(u, pfx) == 0) {
                 /* We failed to allocate entries for a newly allocated prefix.*/
                 prev_pfx = SOC_HU2_LPM_STATE_PREV(u, pfx);
                 next_pfx = SOC_HU2_LPM_STATE_NEXT(u, pfx);
                 if (-1 != prev_pfx) {
                     SOC_HU2_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
                 }
                 if (-1 != next_pfx) {
                     SOC_HU2_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
                 }
             }
             return(SOC_E_FULL);
         }
     }
 
     /*
      * Ripple entries to create free space
      */
     while (free_pfx > pfx) {
         next_pfx = SOC_HU2_LPM_STATE_NEXT(u, free_pfx); 
         SOC_IF_ERROR_RETURN(_lpm_hu2_shift_pfx_down(u, next_pfx));
         SOC_HU2_LPM_STATE_FENT(u, free_pfx) -= 1;
         SOC_HU2_LPM_STATE_FENT(u, next_pfx) += 1;
         free_pfx = next_pfx;
     }
 
     while (free_pfx < pfx) {
         SOC_IF_ERROR_RETURN(_lpm_hu2_shift_pfx_up(u, free_pfx));
         SOC_HU2_LPM_STATE_FENT(u, free_pfx) -= 1;
         prev_pfx = SOC_HU2_LPM_STATE_PREV(u, free_pfx); 
         SOC_HU2_LPM_STATE_FENT(u, prev_pfx) += 1;
         free_pfx = prev_pfx;
     }
 
     SOC_HU2_LPM_STATE_VENT(u, pfx) += 1;
     SOC_HU2_LPM_STATE_FENT(u, pfx) -= 1;
     SOC_HU2_LPM_STATE_END(u, pfx) += 1;
     *free_slot = SOC_HU2_LPM_STATE_END(u, pfx);
     sal_memcpy(e, soc_mem_entry_null(u, L3_DEFIPm),
                soc_mem_entry_words(u,L3_DEFIPm) * 4);
 
     return(SOC_E_NONE);
 }
 
 /*
  *      Delete a slot and adjust entry pointers if required.
  *      e - has the contents of entry at slot(useful for IPV4 only)
  */
 static
 int _lpm_hu2_free_slot_delete(int u, int pfx, void *e, int slot)
 {
     int         prev_pfx;
     int         next_pfx;
     int         from_ent;
     int         to_ent;
     uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
     int         rv;
 
     from_ent = SOC_HU2_LPM_STATE_END(u, pfx);
     to_ent = slot;
         SOC_HU2_LPM_STATE_VENT(u, pfx) -= 1;
         SOC_HU2_LPM_STATE_FENT(u, pfx) += 1;
         SOC_HU2_LPM_STATE_END(u, pfx) -= 1;
         if (to_ent != from_ent) {
 #ifdef HU2_LPM_TABLE_CACHED
             if ((rv = soc_mem_cache_invalidate(u, L3_DEFIPm,
                                                MEM_BLOCK_ANY, from_ent)) < 0) {
                 return rv;
             }
 #endif /* HU2_LPM_TABLE_CACHED */
             if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef)) < 0) {
                 return rv;
             }
             HU2_LPM_HASH_INSERT(u, ef, to_ent);
             if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, to_ent, ef)) < 0) {
                 return rv;
             }
#ifdef BCM_HURRICANE3_SUPPORT
            if ((rv = _lpm_hr3_urpf_entry_replicate(u, to_ent, ef)) < 0) {
                return rv;
            }
#endif  /* BCM_HURRICANE3_SUPPORT */
         }
 
         sal_memcpy(ef, soc_mem_entry_null(u, L3_DEFIPm),
                    soc_mem_entry_words(u,L3_DEFIPm) * 4);
         HU2_LPM_HASH_INSERT(u, ef, from_ent);
         if ((rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, from_ent, ef)) < 0) {
             return rv;
         }
#ifdef BCM_HURRICANE3_SUPPORT
        if ((rv = _lpm_hr3_urpf_entry_replicate(u, from_ent, ef)) < 0) {
            return rv;
        }
#endif  /* BCM_HURRICANE3_SUPPORT */
 
     if (SOC_HU2_LPM_STATE_VENT(u, pfx) == 0) {
         /* remove from the list */
         prev_pfx = SOC_HU2_LPM_STATE_PREV(u, pfx); /* Always present */
         assert(prev_pfx != -1);
         next_pfx = SOC_HU2_LPM_STATE_NEXT(u, pfx);
         SOC_HU2_LPM_STATE_NEXT(u, prev_pfx) = next_pfx;
         SOC_HU2_LPM_STATE_FENT(u, prev_pfx) += SOC_HU2_LPM_STATE_FENT(u, pfx);
         SOC_HU2_LPM_STATE_FENT(u, pfx) = 0;
         if (next_pfx != -1) {
             SOC_HU2_LPM_STATE_PREV(u, next_pfx) = prev_pfx;
         }
         SOC_HU2_LPM_STATE_NEXT(u, pfx) = -1;
         SOC_HU2_LPM_STATE_PREV(u, pfx) = -1;
         SOC_HU2_LPM_STATE_START(u, pfx) = -1;
         SOC_HU2_LPM_STATE_END(u, pfx) = -1;
     }
 
     return(rv);
 }
 
 
 
 /*
  * soc_hu2_lpm_prefix_length_get (Extract vrf weighted  prefix lenght from the 
  * hw entry based on ip, mask & vrf)
  */
 static int
 soc_hu2_lpm_prefix_length_get(int u, void *entry, int *pfx_len) 
 {
     int         pfx;
     int         rv;
     int         ipv6;
     uint32      ipv4a;
    int         vrf_id, vrf_type;


 
     ipv6 = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, MODEf);
 
     if (ipv6) {
         ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_L_MASKf);
         if ((rv = _ip_mask2pfx(ipv4a, &pfx)) < 0) {
             return(rv);
         }
         ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_U_MASKf);
         if (pfx) {
             if (ipv4a != 0xffffffff)  {
                 return(SOC_E_PARAM);
             }
             pfx += 32;
         } else {
             if ((rv = _ip_mask2pfx(ipv4a, &pfx)) < 0) {
                 return(rv);
             }
         }
         pfx += IPV6_PFX_ZERO; /* First 33 are for IPV4 addresses */
     } else {
         ipv4a = SOC_MEM_OPT_F32_GET(u, L3_DEFIPm, entry, IP_ADDR_L_MASKf);
         if ((rv = _ip_mask2pfx(ipv4a, &pfx)) < 0) {
             return(rv);
         }
     }
    
    if (soc_mem_field_valid(u,L3_DEFIPm, VRF_IDf)) {
        uint32 vrf_mask_val = 0;
        vrf_id = soc_L3_DEFIPm_field32_get(u, (uint32 *)entry, VRF_IDf);
        vrf_mask_val = soc_L3_DEFIPm_field32_get(u, (uint32 *)entry, VRF_ID_MASKf);
        if (vrf_mask_val) {
            vrf_type = vrf_id;
        } else if (SOC_VRF_MAX(u) == vrf_id) {
            vrf_type = SOC_L3_VRF_GLOBAL;
        } else {
            vrf_type = SOC_L3_VRF_OVERRIDE;    
            if (soc_mem_field_valid(u, L3_DEFIPm, GLOBAL_ROUTE0f)) {
                if (soc_L3_DEFIPm_field32_get(u, (uint32 *)entry, GLOBAL_ROUTE0f)) {
                    vrf_type = SOC_L3_VRF_GLOBAL; 
                }
            }
        }
    } else {
        /* No vrf support on this device. */
        vrf_type = SOC_L3_VRF_DEFAULT;
    }

    switch (vrf_type) { 
    case SOC_L3_VRF_GLOBAL:
        *pfx_len = pfx;
        break;
    case SOC_L3_VRF_OVERRIDE:
        *pfx_len = pfx + 2 * (MAX_PFX_ENTRIES(u) / 6);
        break;
    default:   
        *pfx_len = pfx +  (MAX_PFX_ENTRIES(u) / 6);
        break;
    }

     return (SOC_E_NONE);
 }
 
 /*
  * _soc_hu2_lpm_match (Exact match for the key. Will match both IP address
  * and mask)
  */
  static 
 int
 _soc_hu2_lpm_match(int u,
                void *key_data,
                void *e,         /* return entry data if found */
                int *index_ptr,  /* return key location */
                int *pfx_len)    /* Key prefix length */
 {
     int         pfx = 0;
 
     /* Calculate vrf weighted prefix lengh. */
     soc_hu2_lpm_prefix_length_get(u, key_data, &pfx); 
     *pfx_len = pfx;

#if defined(HU2_LPM_HASH_SUPPORT) 
{
     int         rv;
     int         key_index = 0;
     if (HU2_LPM_HASH_LOOKUP(u, key_data, pfx, &key_index) == SOC_E_NONE) {
         *index_ptr = key_index;
         if ((rv = READ_L3_DEFIPm(u, MEM_BLOCK_ANY,*index_ptr, e)) < 0) {
             return rv;
         }
         return(SOC_E_NONE);
     } else {
         return(SOC_E_NOT_FOUND);
     }
}
 #endif
 }
 
 
 /*
  * Initialize the start/end tracking pointers for each prefix length
  */
 int
 soc_hu2_lpm_init(int u)
 {
     int max_pfx_len;
     int defip_table_size;
     int pfx_state_size;
     int i;
 
     if (! soc_feature(u, soc_feature_lpm_tcam)) {
         return(SOC_E_UNAVAIL);
     }
 
     max_pfx_len = MAX_PFX_ENTRIES(u);
     pfx_state_size = sizeof(soc_hu2_lpm_state_t) * (max_pfx_len);
 
     if (!SOC_HU2_LPM_INIT_CHECK(u)) {
         soc_hu2_lpm_field_cache_state[u] = 
             sal_alloc(sizeof(soc_hu2_lpm_field_cache_t), "lpm_field_state");
         if (NULL == soc_hu2_lpm_field_cache_state[u]) {
             return (SOC_E_MEMORY);
         }
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, HITf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, GLOBAL_ROUTE0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, CLASS_ID0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, DST_DISCARD0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, SRC_DISCARD0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, RPE0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, PRI0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, DEFAULTROUTE0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, ECMP_UNUSED0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, ECMP_PTR0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, UPPER_NEXT_HOP_INDEX0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, NEXT_HOP_INDEX0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, ECMP0f);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, MASKf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, IP_ADDR_U_MASKf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, IP_ADDR_L_MASKf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, MODE_MASKf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, IP_ADDR_Uf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, IP_ADDR_Lf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, MODEf);
         SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u],
            L3_DEFIPm, VALIDf);
        SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u], 
                L3_DEFIPm, VRF_IDf);
        SOC_HU2_LPM_CACHE_FIELD_ASSIGN(u, soc_hu2_lpm_field_cache_state[u], 
                L3_DEFIPm, VRF_ID_MASKf);
         SOC_HU2_LPM_STATE(u) = sal_alloc(pfx_state_size, "HU2_LPM prefix info");
         if (NULL == SOC_HU2_LPM_STATE(u)) {
             sal_free(soc_hu2_lpm_field_cache_state[u]);
             soc_hu2_lpm_field_cache_state[u] = NULL;
             return (SOC_E_MEMORY);
         }
     }
 
 #ifdef HU2_LPM_TABLE_CACHED
     SOC_IF_ERROR_RETURN (soc_mem_cache_set(u, L3_DEFIPm, MEM_BLOCK_ALL, TRUE));
 #endif /* HU2_LPM_TABLE_CACHED */
     SOC_HU2_LPM_LOCK(u);
 
     sal_memset(SOC_HU2_LPM_STATE(u), 0, pfx_state_size);
     for(i = 0; i < max_pfx_len; i++) {
         SOC_HU2_LPM_STATE_START(u, i) = -1;
         SOC_HU2_LPM_STATE_END(u, i) = -1;
         SOC_HU2_LPM_STATE_PREV(u, i) = -1;
         SOC_HU2_LPM_STATE_NEXT(u, i) = -1;
         SOC_HU2_LPM_STATE_VENT(u, i) = 0;
         SOC_HU2_LPM_STATE_FENT(u, i) = 0;
     }
 
     defip_table_size = soc_mem_index_count(u, L3_DEFIPm);
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(u) || SOC_IS_GREYHOUND2(u)) {
        if (SOC_URPF_STATUS_GET(u)) {
            defip_table_size >>= 1;
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

     SOC_HU2_LPM_STATE_FENT(u, (MAX_PFX_INDEX(u))) = defip_table_size;
 
 #ifdef HU2_LPM_HASH_SUPPORT
     if (SOC_HU2_LPM_STATE_HASH(u) != NULL) {
         if (soc_hu2_lpm_hash_destroy(SOC_HU2_LPM_STATE_HASH(u)) < 0) {
             SOC_HU2_LPM_UNLOCK(u);
             return SOC_E_INTERNAL;
         }
         SOC_HU2_LPM_STATE_HASH(u) = NULL;
     }
 
     if (soc_hu2_lpm_hash_create(u, defip_table_size, defip_table_size,
                                 &SOC_HU2_LPM_STATE_HASH(u)) < 0) {
         SOC_HU2_LPM_UNLOCK(u);
         return SOC_E_MEMORY;
     }
 #endif
 
     SOC_HU2_LPM_UNLOCK(u);
 
     return(SOC_E_NONE);
 }
 /*
  * De-initialize the start/end tracking pointers for each prefix length
  */
 int
 soc_hu2_lpm_deinit(int u)
 {
     if (!soc_feature(u, soc_feature_lpm_tcam)) {
         return(SOC_E_UNAVAIL);
     }
 
     SOC_HU2_LPM_LOCK(u);
 
 #ifdef HU2_LPM_HASH_SUPPORT
     if (SOC_HU2_LPM_STATE_HASH(u) != NULL) {
         soc_hu2_lpm_hash_destroy(SOC_HU2_LPM_STATE_HASH(u));
         SOC_HU2_LPM_STATE_HASH(u) = NULL;
     }
 #endif
 
     if (SOC_HU2_LPM_INIT_CHECK(u)) {
         sal_free(soc_hu2_lpm_field_cache_state[u]);
         soc_hu2_lpm_field_cache_state[u] = NULL;
         sal_free(SOC_HU2_LPM_STATE(u));
         SOC_HU2_LPM_STATE(u) = NULL;
     }
 
     SOC_HU2_LPM_UNLOCK(u);
 
     return(SOC_E_NONE);
 }
 
 #ifdef BCM_WARM_BOOT_SUPPORT
 int
 soc_hu2_lpm_reinit_done(int unit)
 {
     int idx;
     int prev_idx = MAX_PFX_INDEX(unit);
 
 
     int defip_table_size = soc_mem_index_count(unit, L3_DEFIPm);
 
     SOC_HU2_LPM_STATE_PREV(unit, MAX_PFX_INDEX(unit)) = -1;
 
     for (idx = MAX_PFX_INDEX(unit); idx > 0 ; idx--) {
         if (-1 == SOC_HU2_LPM_STATE_START(unit, idx)) {
             continue;
         }
 
         SOC_HU2_LPM_STATE_PREV(unit, idx) = prev_idx;
         SOC_HU2_LPM_STATE_NEXT(unit, prev_idx) = idx;
 
         SOC_HU2_LPM_STATE_FENT(unit, prev_idx) =                    \
                           SOC_HU2_LPM_STATE_START(unit, idx) -      \
                           SOC_HU2_LPM_STATE_END(unit, prev_idx) - 1;
         prev_idx = idx;
         
     }
 
     SOC_HU2_LPM_STATE_NEXT(unit, prev_idx) = -1;
     SOC_HU2_LPM_STATE_FENT(unit, prev_idx) =                       \
                           defip_table_size -                       \
                           SOC_HU2_LPM_STATE_END(unit, prev_idx) - 1;
 
     return (SOC_E_NONE);
 }
 
 int
 soc_hu2_lpm_reinit(int unit, int idx, uint32 *lpm_entry)
 {
     int pfx_len;
 
 
     SOC_IF_ERROR_RETURN
         (soc_hu2_lpm_prefix_length_get(unit, lpm_entry, &pfx_len));
 
     if (SOC_HU2_LPM_STATE_VENT(unit, pfx_len) == 0) {
         SOC_HU2_LPM_STATE_START(unit, pfx_len) = idx;
         SOC_HU2_LPM_STATE_END(unit, pfx_len) = idx;
     } else {
         SOC_HU2_LPM_STATE_END(unit, pfx_len) = idx;
     }
 
     SOC_HU2_LPM_STATE_VENT(unit, pfx_len)++;
     HU2_LPM_HASH_INSERT(unit, lpm_entry, idx);
 
     return (SOC_E_NONE);
 }
 #endif /* BCM_WARM_BOOT_SUPPORT */
 
 
 #ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
 /*
  * Function:
  *     soc_hu2_lpm_sw_dump
  * Purpose:
  *     Displays FB HU2_LPM information maintained by software.
  * Parameters:
  *     unit - Device unit number
  * Returns:
  *     None
  */
 void
 soc_hu2_lpm_sw_dump(int unit)
 {
     soc_hu2_lpm_state_p  lpm_state;
     int              i;
 
     LOG_CLI((BSL_META_U(unit,
                         "\n    FB HU2_LPM State -\n")));
     LOG_CLI((BSL_META_U(unit,
                         "        Prefix entries : %d\n"), MAX_PFX_ENTRIES(unit)));
 
     lpm_state = soc_hu2_lpm_state[unit];
     if (lpm_state == NULL) {
         return;
     }
 
     for (i = 0; i < MAX_PFX_ENTRIES(unit); i++) {
         if (lpm_state[i].vent != 0 ) {
             LOG_CLI((BSL_META_U(unit,
                                 "      Prefix %d\n"), i));
             LOG_CLI((BSL_META_U(unit,
                                 "        Start : %d\n"), lpm_state[i].start));
             LOG_CLI((BSL_META_U(unit,
                                 "        End   : %d\n"), lpm_state[i].end));
             LOG_CLI((BSL_META_U(unit,
                                 "        Prev  : %d\n"), lpm_state[i].prev));
             LOG_CLI((BSL_META_U(unit,
                                 "        Next  : %d\n"), lpm_state[i].next));
             LOG_CLI((BSL_META_U(unit,
                                 "        Valid Entries : %d\n"), lpm_state[i].vent));
             LOG_CLI((BSL_META_U(unit,
                                 "        Free  Entries : %d\n"), lpm_state[i].fent));
         }
     }
 
     return;
 }
 #endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
 
 /*
  * Implementation using soc_mem_read/write using entry rippling technique
  * Advantage: A completely sorted table is not required. Lookups can be slow
  * as it will perform a linear search on the entries for a given prefix length.
  * No device access necessary for the search if the table is cached. Auxiliary
  * hash table can be maintained to speed up search(Not implemented) instead of
  * performing a linear search.
  * Small number of entries need to be moved around (97 worst case)
  * for performing insert/update/delete. However CPU needs to do all
  * the work to move the entries.
  */
 
 /*
  * soc_hu2_lpm_insert
  * For IPV4 assume only both IP_ADDR0 is valid
  * Moving multiple entries around in h/w vs  doing a linear search in s/w
  */
 int
 soc_hu2_lpm_insert(int u, void *entry_data)
 {
     int         pfx;
     int         index;
     uint32      e[SOC_MAX_MEM_FIELD_WORDS];
     int         rv = SOC_E_NONE;
     int         found = 0;
 
     sal_memcpy(e, soc_mem_entry_null(u, L3_DEFIPm),
                soc_mem_entry_words(u,L3_DEFIPm) * 4);
 
     SOC_HU2_LPM_LOCK(u);
     rv = _soc_hu2_lpm_match(u, entry_data, e, &index, &pfx);
     if (rv == SOC_E_NOT_FOUND) {
         rv = _lpm_hu2_free_slot_create(u, pfx, e, &index);
         if (rv < 0) {
             SOC_HU2_LPM_UNLOCK(u);
             return(rv);
         }
     } else {
         found = 1;
     }
 
     if (rv == SOC_E_NONE) {
         /* Entry already present. Update the entry */
         soc_hu2_lpm_state_dump(u);
         LOG_INFO(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(u,
                              "\nsoc_hu2_lpm_insert: %d %d\n"),
                              index, pfx));
         if (!found) {
             HU2_LPM_HASH_INSERT(u, entry_data, index);
         }
         rv = WRITE_L3_DEFIPm(u, MEM_BLOCK_ANY, index, entry_data);
#ifdef BCM_HURRICANE3_SUPPORT
         if (rv >= 0 && soc_feature(u, soc_feature_urpf)) {
            rv = _lpm_hr3_urpf_entry_replicate(u, index, entry_data);
         }
#endif  /* BCM_HURRICANE3_SUPPORT */
     }
 
     SOC_HU2_LPM_UNLOCK(u);
 
     return(rv);
 }
 
 /*
  * soc_hu2_lpm_delete
  */
 int
 soc_hu2_lpm_delete(int u, void *key_data)
 {
     int         pfx;
     int         index;
     uint32      e[SOC_MAX_MEM_FIELD_WORDS];
     int         rv = SOC_E_NONE;
 
     SOC_HU2_LPM_LOCK(u);
     rv = _soc_hu2_lpm_match(u, key_data, e, &index, &pfx);
     if (rv == SOC_E_NONE) {
         LOG_INFO(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(u,
                              "\nsoc_hu2_lpm_delete: %d %d\n"),
                              index, pfx));
         HU2_LPM_HASH_DELETE(u, key_data, index);
         rv = _lpm_hu2_free_slot_delete(u, pfx, e, index);
     }
     soc_hu2_lpm_state_dump(u);
     SOC_HU2_LPM_UNLOCK(u);
     return(rv);
 }
 
 /*
  * soc_hu2_lpm_match (Exact match for the key. Will match both IP
  *                   address and mask)
  */
 int
 soc_hu2_lpm_match(int u,
                void *key_data,
                void *e,         /* return entry data if found */
                int *index_ptr)  /* return key location */
 {
     int        pfx;
     int        rv;
 
     SOC_HU2_LPM_LOCK(u);
     rv = _soc_hu2_lpm_match(u, key_data, e, index_ptr, &pfx);
     SOC_HU2_LPM_UNLOCK(u);

#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(u) && soc_feature(u, soc_feature_ser_hw_bg_read) &&
        rv == SOC_E_NOT_FOUND) {
        sal_usleep(1000);
        SOC_HU2_LPM_LOCK(u);
        rv = _soc_hu2_lpm_match(u, key_data, e, index_ptr, &pfx);

        SOC_HU2_LPM_UNLOCK(u);
    }
#endif /* end of BCM_FIRELIGHT_SUPPORT */

    return(rv);
 }
 
#endif

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r
};

int
soc_hu2_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }
        
    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        cur = (4180000 - (5556 * fval)) / 1000;
        fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        peak = (4180000 - (5556 * fval)) / 1000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak	 = peak;
    }
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    *temperature_count=num_entries_out;
    return SOC_E_NONE;
}


int
soc_hu2_show_ring_osc(int unit)
{
    static const struct {
        int osc_sel;
        soc_field_t field0;
        int value0;
        soc_field_t field1;
        int value1;
        char *name;
    } osc_tbl[] = {
        { 0, OSC_0_SELf, 0, INVALIDf, -1, "IO ring 0 HVT min" },
        { 0, OSC_0_SELf, 1, INVALIDf, -1, "IO ring 0 HVT mid" },
        { 0, OSC_0_SELf, 2, INVALIDf, -1, "IO ring 0 HVT max" },
        { 0, OSC_0_SELf, 3, INVALIDf, -1, "IO ring 0 SVT min" },
        { 1, OSC_1_SELf, 0, INVALIDf, -1, "IO ring 1 HVT min" },
        { 1, OSC_1_SELf, 1, INVALIDf, -1, "IO ring 1 HVT mid" },
        { 1, OSC_1_SELf, 2, INVALIDf, -1, "IO ring 1 HVT max" },
        { 1, OSC_1_SELf, 3, INVALIDf, -1, "IO ring 1 SVT min" },
        { 2, IROSC_SELf, 0, INVALIDf, -1, "Core ring 0 five stages" },
        { 2, IROSC_SELf, 1, INVALIDf, -1, "Core ring 0 nine stages" },
        { 3, IROSC_SELf, 0, INVALIDf, -1, "Core ring 1 five stages" },
        { 3, IROSC_SELf, 1, INVALIDf, -1, "Core ring 1 nine stages" },
        { 4, SRAM_OSC_0_PENf, 0, SRAM_OSC_0_NENf, 1, "SRAM ring 0 NMOS" },
        { 5, SRAM_OSC_0_PENf, 1, SRAM_OSC_0_NENf, 0, "SRAM ring 0 PMOS" },
        { 6, SRAM_OSC_1_PENf, 0, SRAM_OSC_1_NENf, 1, "SRAM ring 1 NMOS" },
        { 7, SRAM_OSC_1_PENf, 1, SRAM_OSC_1_NENf, 0, "SRAM ring 1 PMOS" },
        { 8, SRAM_OSC_2_PENf, 0, SRAM_OSC_2_NENf, 1, "SRAM ring 2 NMOS" },
        { 9, SRAM_OSC_2_PENf, 1, SRAM_OSC_2_NENf, 0, "SRAM ring 2 PMOS" },
        { 10, SRAM_OSC_3_PENf, 0, SRAM_OSC_3_NENf, 1, "SRAM ring 3 NMOS" },
        { 11, SRAM_OSC_3_PENf, 1, SRAM_OSC_3_NENf, 0, "SRAM ring 3 PMOS" },
        { 12, SRAM_OSC_4_PENf, 0, SRAM_OSC_4_NENf, 1, "SRAM ring 4 NMOS" },
        { 13, SRAM_OSC_4_PENf, 1, SRAM_OSC_4_NENf, 0, "SRAM ring 4 PMOS" },
        { 14, SRAM_OSC_5_PENf, 0, SRAM_OSC_5_NENf, 1, "SRAM ring 5 NMOS" },
        { 15, SRAM_OSC_5_PENf, 1, SRAM_OSC_5_NENf, 0, "SRAM ring 5 PMOS" },
    };
    uint32 rval, fval;
    int index, core_clk, quo, rem, frac, retry;

    core_clk = 133 * 1024;

    for (index = 0; index < COUNTOF(osc_tbl); index++) {
        rval = 0;
        /*
         * set OSC_CNT_RSTBf to 0 to do softreset
         * set OSC_CNT_START to 0 to hold the counter until it selects
         * the input signal
         */
        SOC_IF_ERROR_RETURN(READ_TOP_RING_OSC_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, OSC_ENABLEf, 1);
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, IROSC_ENf, 1);
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, osc_tbl[index].field0,
                          osc_tbl[index].value0);
        if (osc_tbl[index].field1 != INVALIDf) {
            soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, osc_tbl[index].field1,
                              osc_tbl[index].value1);
        }
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, OSC_SELf,
                          osc_tbl[index].osc_sel);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, OSC_CNT_RSTBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, OSC_CNT_STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));

        for (retry = 0; retry < 10; retry++) {
            sal_usleep(1000);
            SOC_IF_ERROR_RETURN(READ_TOP_OSC_COUNT_STATr(unit, &rval));
            if (!soc_reg_field_get(unit, TOP_OSC_COUNT_STATr, rval, OSC_CNT_DONEf)) {
                continue;
            }

            fval = soc_reg_field_get(unit, TOP_OSC_COUNT_STATr, rval, OSC_CNT_VALUEf);
            quo = core_clk / fval;
            rem = core_clk - quo * fval;
            frac = (rem * 10000) / fval;
            LOG_CLI((BSL_META_U(unit,
                                "%s: %d.%04d Mhz\n"),
                     osc_tbl[index].name, quo, frac));
            break;
        }

        SOC_IF_ERROR_RETURN(READ_TOP_RING_OSC_CTRLr(unit, &rval));
        soc_reg_field_set(unit, TOP_RING_OSC_CTRLr, &rval, OSC_CNT_STARTf, 0);
        /* Clearing theStart bit Clears the Counter */
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));

    }

    return SOC_E_NONE;
}


int
soc_hu2_show_material_process(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, nmos[COUNTOF(pvtmon_result_reg)], n_avg, p_avg;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 0);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    sal_usleep(1000);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 1);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
                                PVTMON_SELECTf, 1));
    sal_usleep(1000);

    p_avg = 0;

    /* Read NMOS information */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_0r, REG_PORT_ANY,
                                MEASUREMENT_CALLIBRATIONf, 5));

    sal_usleep(1000);

    n_avg = 0;
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        nmos[index] = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        n_avg += nmos[index];
    }

    /* Read PMOS information and print both NMOS and PMOS value */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_0r, REG_PORT_ANY,
                                MEASUREMENT_CALLIBRATIONf, 7));

    sal_usleep(1000);

    p_avg = 0;
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        p_avg += fval;

        LOG_CLI((BSL_META_U(unit,
                            "Material process location %d: NMOS = %3d PMOS = %3d\n"),
                 index, nmos[index], fval));
    }

    LOG_CLI((BSL_META_U(unit,
                        "Average:                     NMOS = %3d PMOS = %3d\n"),
             n_avg / COUNTOF(pvtmon_result_reg),
             p_avg / COUNTOF(pvtmon_result_reg)));

    return SOC_E_NONE;
}


#if defined(SER_TR_TEST_SUPPORT)
/*
 * Function:
 *      soc_hu2_ser_mem_test
 * Purpose:
 *      Performs a SER test on a single Hurricane2 memory
 * Parameters:
 *      unit               - (IN) Device Number
 *      mem                - (IN) The memory to test
 *      test_type        - (IN) How many indices in the memory to test
 */
int soc_hu2_ser_mem_test(int unit, soc_mem_t mem, 
                                _soc_ser_test_t test_type, int cmd) {
    int group, table, i, rv;
    _soc_parity_info_t *info;
    soc_mem_t memTable;
    soc_port_t block_port;
    soc_block_t blk;
    int testErrors = 0;
    /*Prevent compiler error for common function signature.*/
    (void) cmd;
    /*TCAM_test*/
    for (i = 0; _soc_hu2_tcam_ser_info[i].mem != INVALIDm; i++) {
        if (_soc_hu2_tcam_ser_info[i].mem == mem) {

            /* coverity[negative_returns : FALSE] */
            rv = ser_test_mem_pipe(unit, SER_RANGE_ENABLEr, 
                            i, -1, _soc_hu2_tcam_ser_info[i].mem, VALIDf, 
                            test_type, MEM_BLOCK_ANY,
                            REG_PORT_ANY, _SOC_ACC_TYPE_PIPE_ANY, &testErrors);
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                    "Error during TCAM test.  Aborting.\n")));
                return rv;
            }
        }
    }
    /*H/W memory Test*/
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                _soc_hu2_parity_group_info[group].blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            for (table = 0; info[table].error_field != INVALIDf; table++) {
                memTable = info[table].mem;
                if (memTable == INVALIDm) {
                    continue;
                }
                if (memTable == mem) {
                    if (_soc_hu2_parity_group_info[group].blocktype 
                                        == SOC_BLK_MMU) {
                        if ((mem == MMU_IPMC_VLAN_TBLm) ||
                           (mem == MMU_IPMC_GROUP_TBL0m) ||
                           (mem == MMU_IPMC_GROUP_TBL1m)) {
                           /* The parity value is generated by software */
                           continue;
                        }
                        rv = ser_test_mem_pipe(unit, MISCCONFIGr, -1, 
                                 PARITY_CHECK_ENf,
                                 mem, INVALIDf, test_type, blk, block_port, 
                                 _SOC_ACC_TYPE_PIPE_ANY, &testErrors);
                    } else {
                        rv = ser_test_mem_pipe(unit,info[table].control_reg, -1, 
                                 info[table].enable_field,
                                 mem, INVALIDf, test_type, blk, block_port, 
                                 _SOC_ACC_TYPE_PIPE_ANY, &testErrors);
                   }
                    if (rv != SOC_E_NONE) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Error during H/W test.  Aborting.\n")));
                        return rv;
                    }
                }
            }

        }
    }
    if (testErrors == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "SER Test passed on unit: %d for memory %s\n"), unit, 
                 SOC_MEM_NAME(unit,mem)));
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                            "SER Test failed on unit: %d for memory %s\n"), unit, 
                 SOC_MEM_NAME(unit,mem)));
        return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hu2_ser_test
 * Purpose:
 *      Performs a SER test on all Hurricane2 Memories
 * Parameters:
 *      unit               - (IN) Device Number
 *      test_type        - (IN) Determines how comprehensive of a test to run
 */
int soc_hu2_ser_test(int unit, _soc_ser_test_t test_type) {
    int i, group, rv, table;
    _soc_parity_info_t *info;
    soc_port_t block_port;
    soc_mem_t memTable;
    soc_block_t blk;
    int numTCAMErr   = 0;
    int numHwMemErr   = 0;
    
    /*Test each TCAM memory*/
    for (i = 0; _soc_hu2_tcam_ser_info[i].mem != INVALIDm; i++) {
        rv = ser_test_mem_pipe(unit, SER_RANGE_ENABLEr, i, -1,
                _soc_hu2_tcam_ser_info[i].mem, VALIDf, test_type, 
                MEM_BLOCK_ANY, REG_PORT_ANY, 
                _SOC_ACC_TYPE_PIPE_ANY, &numTCAMErr);
                if (rv != SOC_E_NONE) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Error during TCAM test.  Aborting.\n")));
                    return rv;
                }
    }
    /*H/W memory Test*/
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                _soc_hu2_parity_group_info[group].blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            for (table = 0; info[table].error_field != INVALIDf; table++) {
                memTable = info[table].mem;
                if (memTable == INVALIDm) {
                    continue;
                }
                if (_soc_hu2_parity_group_info[group].blocktype == 
                             SOC_BLK_MMU) {
                    if ((memTable == MMU_IPMC_VLAN_TBLm) || 
                       (memTable == MMU_IPMC_GROUP_TBL0m) ||
                       (memTable == MMU_IPMC_GROUP_TBL1m)) {
                       /* The parity value is generated by software */
                       continue;
                    }
                    rv = ser_test_mem_pipe(unit,MISCCONFIGr, -1, 
                             PARITY_CHECK_ENf,
                             info[table].mem, INVALIDf, test_type, 
                             blk, block_port, 
                             _SOC_ACC_TYPE_PIPE_ANY, &numHwMemErr);
                } else {
                    rv = ser_test_mem_pipe(unit,info[table].control_reg, -1, 
                             info[table].enable_field,
                             info[table].mem, INVALIDf, test_type, 
                             blk, block_port, 
                             _SOC_ACC_TYPE_PIPE_ANY, &numHwMemErr);
                }
                if (rv != SOC_E_NONE) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Error during H/W test.  Aborting.\n")));
                    return rv;
                }
            }

        }
    }
    
    LOG_CLI((BSL_META_U(unit,
                        "Total TCAM errors on unit %d: %d\n"), unit, numTCAMErr));
    LOG_CLI((BSL_META_U(unit,
                        "Total H/W parity errors on unit %d: %d\n"),unit, numHwMemErr));
    return SOC_E_NONE;
}

STATIC soc_error_t
_ser_hu2_ser_error_injection_support(int unit, soc_mem_t mem,
                                    int pipe_target)
{

    int rv = SOC_E_UNAVAIL;
    int i = 0, hw_enable_ix = 0, group = 0, table = 0;
    uint32 range_enable;
    _soc_generic_ser_info_t *tcam_ser_info;
    _soc_hu2_parity_group_info_t * gp;
    _soc_parity_info_t *info;
    soc_port_t block_port;
    soc_mem_t memTable;
    soc_block_t blk;

    LOG_VERBOSE(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "unit %d, mem %s, pipe_target %d\n"),
                 unit, SOC_MEM_NAME(unit,mem), pipe_target));

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d, mem %d is INVALID or not valid "
                              "for this unit !!\n"),
                   unit, mem));
        return SOC_E_UNAVAIL;
    }

    tcam_ser_info = _soc_hu2_tcam_ser_info;

    /* Search TCAMs */
    if (tcam_ser_info != NULL) {
        /* Check if enable */
        SOC_IF_ERROR_RETURN
            (READ_SER_RANGE_ENABLEr(unit, &range_enable));
        LOG_DEBUG(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                              "Search TCAMs: SER_RANGE_ENABLE 0x%X\n"),
                   range_enable));
        for (i = 0; tcam_ser_info[i].mem != INVALIDm; i++) {
            if (tcam_ser_info[i].mem == mem) {
                hw_enable_ix = tcam_ser_info[i].ser_hw_index;
                LOG_DEBUG(BSL_LS_SOC_SER,
                          (BSL_META_U(unit,
                                      "SER Range HW Enable index %d\n"),
                           hw_enable_ix));
                if (((range_enable >> hw_enable_ix) & 0x1) == 0) {
                    LOG_WARN(BSL_LS_SOC_SER,
                             (BSL_META_U(unit,
                                         "matched mem but SER detection is disabled\n"
                                         "SER_RANGE_ENABLE 0x%X, SER Range HW index %d\n"),
                              range_enable,hw_enable_ix));
                    return rv; /* matched mem but ser_detection is disabled */
                }
                LOG_VERBOSE(BSL_LS_SOC_SER,
                            (BSL_META_U(unit,
                                        "TCAM found\n")));
                return SOC_E_NONE; /* found */
            }
        }
    }

    /*H/W memory Test*/
    for (group = 0; ; group++) {
        gp = &_soc_hu2_parity_group_info[group];
        info = gp->info;
        if (gp->cpi_bit == 0) {
            /* End of table */
            break;
        }
        SOC_BLOCK_ITER(unit, blk, gp->blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                memTable = info[table].mem;
                if (memTable == INVALIDm) {
                    continue;
                }
                if (gp->blocktype == SOC_BLK_MMU) {
                    if ((memTable == MMU_IPMC_VLAN_TBLm) ||
                       (memTable == MMU_IPMC_GROUP_TBL0m) ||
                       (memTable == MMU_IPMC_GROUP_TBL1m)) {
                       /* The parity value is generated by software */
                       continue;
                    }
                }
                if (memTable == mem) {
                    LOG_VERBOSE(BSL_LS_SOC_SER,
                                (BSL_META_U(unit,
                                            "H/W memory found\n")));
                    return SOC_E_NONE; /* found */
                }
            }

        }
    }
    /* The tested memory is not found in SER enabled list, can't inject error of it */
    return rv;
}

/*
 * Function:
 *      soc_hu2_ser_mem_test
 * Purpose:
 *      Performs a SER test on a single Hurricane2 memory
 * Parameters:
 *      unit               - (IN) Device Number
 *      mem                - (IN) The memory to test
 *      test_type        - (IN) How many indices in the memory to test
 */
int soc_hu2_ser_inject_error(int unit, uint32 flags, soc_mem_t mem,
                             int pipeTarget, int block, int index) {
    int group, table, i;
    _soc_parity_info_t *info;
    soc_mem_t memTable;
    soc_port_t block_port;
    soc_block_t blk;
    ser_test_data_t test_data;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];
    test_data.entry_buf = tmp_entry;
    test_data.field_buf = fieldData;
    /*TCAM_test*/
    for (i = 0; _soc_hu2_tcam_ser_info[i].mem != INVALIDm; i++) {
        if (_soc_hu2_tcam_ser_info[i].mem == mem) {          
            soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                     SER_RANGE_ENABLEr, i, INVALIDf, mem,
                                     INVALIDf, MEM_BLOCK_ANY, REG_PORT_ANY,
                                     _SOC_ACC_TYPE_PIPE_ANY, index, &test_data);
            /*Read the memory for successful injection*/
            SOC_IF_ERROR_RETURN(ser_test_mem_read(unit, 0, &test_data));
            /*Disable parity*/
            SOC_IF_ERROR_RETURN(_ser_test_parity_control(unit, &test_data, 0));
            /*Inject error*/            
            SOC_IF_ERROR_RETURN(soc_ser_test_inject_full(unit, flags,
                                                          &test_data));
            /*Enable parity*/
            SOC_IF_ERROR_RETURN(_ser_test_parity_control(unit, &test_data, 1));
            return SOC_E_NONE;

        }
    }
    /*H/W memory Test*/
    for (group = 0; _soc_hu2_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu2_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                _soc_hu2_parity_group_info[group].blocktype) {
            if (_soc_hurricane2_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            for (table = 0; info[table].error_field != INVALIDf; table++) {
                memTable = info[table].mem;
                if (memTable == INVALIDm) {
                    continue;
                }
                if (memTable == mem) {
                    if((blk == block) || (block == MEM_BLOCK_ANY))
                    {
                        /*Inject error*/
                        test_data.mem = mem;
                        test_data.tcam_parity_bit = -1;
                        if (_soc_hu2_parity_group_info[group].blocktype 
                                        == SOC_BLK_MMU) {
                            if ((mem == MMU_IPMC_VLAN_TBLm) || 
                                (mem == MMU_IPMC_GROUP_TBL0m) ||
                                (mem == MMU_IPMC_GROUP_TBL1m)) {
                                /* the parity value is generated by software */
                                continue;
                            }
                            soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                         MISCCONFIGr,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         PARITY_CHECK_ENf,
                                         mem, EVEN_PARITYf, blk,
                                         block_port, _SOC_ACC_TYPE_PIPE_ANY, 
                                         index, &test_data);
                        } else {
                            soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                         info[table].control_reg,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         info[table].enable_field,
                                         mem, EVEN_PARITYf, blk,
                                         block_port, _SOC_ACC_TYPE_PIPE_ANY,
                                         index, &test_data);
                        }
                        SOC_IF_ERROR_RETURN(
                                ser_test_mem_read(unit, 0, &test_data));
                        /*Disable parity*/
                        SOC_IF_ERROR_RETURN(
                                _ser_test_parity_control(unit, &test_data, 0));
                        /*Inject error*/
                        SOC_IF_ERROR_RETURN(
                                soc_ser_test_inject_full(unit, flags,
                                                          &test_data));
                        /*Enable parity*/
                        SOC_IF_ERROR_RETURN(
                                _ser_test_parity_control(unit, &test_data, 1));
                    }
                }
            }

        }
    }
    return SOC_E_NONE;
}

void
soc_hu2_ser_test_register(int unit)
{
    /*Initialize chip-specific functions for SER testing*/
    memset(&ser_hu2_test_fun, 0, sizeof(soc_ser_test_functions_t));
    ser_hu2_test_fun.inject_error_f = &soc_hu2_ser_inject_error;
    ser_hu2_test_fun.test_mem = &soc_hu2_ser_mem_test;
    ser_hu2_test_fun.test     = &soc_hu2_ser_test;
    ser_hu2_test_fun.injection_support = &_ser_hu2_ser_error_injection_support;

    soc_ser_test_functions_register(unit, &ser_hu2_test_fun);
}
#endif /* SER_TR_TEST_SUPPORT */



#endif /* BCM_HURRICANE2_SUPPORT */
