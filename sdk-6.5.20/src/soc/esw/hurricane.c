/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/firebolt.h>
#include <soc/bradley.h>
#include <soc/hurricane.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>
#include <soc/devids.h>

#ifdef BCM_HURRICANE_SUPPORT

/*
 * Enduro chip driver functions.  
 */
soc_functions_t soc_hurricane_drv_funs = {
    soc_hurricane_misc_init,
    soc_hurricane_mmu_init,
    soc_hurricane_age_timer_get,
    soc_hurricane_age_timer_max_get,
    soc_hurricane_age_timer_set,
};

typedef enum {
    _SOC_PARITY_INFO_TYPE_GENERIC,
    _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
    _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
    _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER,
    _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
    _SOC_PARITY_INFO_TYPE_MMU_PARITY,
    _SOC_PARITY_INFO_TYPE_MMUIPMC,
    _SOC_PARITY_INFO_TYPE_MMUWRED,
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

STATIC _soc_parity_info_t _soc_hu_ip0_parity_info[] = {
    { PARITY_ENf, VXLT_PAR_ERRf, NULL,
      VLAN_XLATEm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      VLAN_XLATE_PARITY_CONTROLr,
      VLAN_XLATE_PARITY_STATUS_0r, VLAN_XLATE_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
 /* { PPA_CMD_COMPLETEf }, */
 /* { MEM_RESET_COMPLETEf }, */
 /* { AGE_CMD_COMPLETEf }, */
    { PARITY_ENf, VLAN_SUBNET_DATA_PARITY_ERRf, NULL,
      VLAN_SUBNET_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_SUBNET_DATA_PARITY_CONTROLr,
      VLAN_SUBNET_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PROTOCOL_DATA_PARITY_ERRf, NULL,
      VLAN_PROTOCOL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PROTOCOL_DATA_PARITY_CONTROLr,
      VLAN_PROTOCOL_DATA_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_PARITY_ERRf, NULL,
      VLAN_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_PARITY_CONTROLr,
      VLAN_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VLAN_STG_PARITY_ERRf, NULL,
      STG_TABm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VLAN_STG_PARITY_CONTROLr,
      VLAN_STG_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_TUNNEL_RAM_PARITY_ERRf, NULL,
      L3_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_TUNNEL_DATA_ONLY_PARITY_CONTROLr,
      L3_TUNNEL_DATA_ONLY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, SOURCE_TRUNK_MAP_PARITY_ERRf, NULL,
      SOURCE_TRUNK_MAP_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_TRUNK_MAP_PARITY_CONTROLr,
      SOURCE_TRUNK_MAP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, VFP_POLICY_PAR_ERRf, NULL,
      VFP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFP_POLICY_PARITY_CONTROLr,
      VFP_POLICY_PARITY_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, LMEP_PAR_ERRf, NULL,
      LMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      LMEP_PARITY_CONTROLr,
      LMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, IARB_PKT_ECC_INTRf, "Iarb packet ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_PKT_ECC_CONTROLr,
      IARB_PKT_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { ECC_ENf, IARB_HDR_ECC_INTRf, "Iarb header ecc error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      IARB_HDR_ECC_CONTROLr,
      IARB_HDR_ECC_STATUS_INTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu_ip1_parity_info[] = {
    { PARITY_ENf, VFI_PAR_ERRf, NULL,
      VFIm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_PARITY_CONTROLr,
      VFI_PARITY_STATUS_INTRr, INVALIDr,
      VFI_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, SVP_PAR_ERRf, NULL,
      SOURCE_VPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_VP_PARITY_CONTROLr,
      SOURCE_VP_PARITY_STATUSr, INVALIDr,
      SOURCE_VP_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, L3_IIF_PAR_ERRf, NULL,
      L3_IIFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_IIF_PARITY_CONTROLr,
      L3_IIF_PARITY_STATUSr, INVALIDr,
      L3_IIF_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, MPLS_ENTRY_PAR_ERRf, NULL,
      MPLS_ENTRYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      MPLS_ENTRY_PARITY_CONTROLr,
      MPLS_ENTRY_PARITY_STATUS_INTR_0r, MPLS_ENTRY_PARITY_STATUS_INTR_1r,
      MPLS_ENTRY_PARITY_STATUS_NACK_0r, MPLS_ENTRY_PARITY_STATUS_NACK_1r },
    { PARITY_ENf, L2_ENTRY_PAR_ERRf, NULL,
      L2Xm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L2_ENTRY_PARITY_CONTROLr,
      L2_ENTRY_PARITY_STATUS_0r, L2_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L2MC_PAR_ERRf, NULL,
      L2MCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L2MC_PARITY_CONTROLr,
      L2MC_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_ENTRY_PAR_ERRf, NULL,
      L3_ENTRY_ONLYm, _SOC_PARITY_INFO_TYPE_DUAL_PARITY,
      L3_ENTRY_PARITY_CONTROLr,
      L3_ENTRY_PARITY_STATUS_0r, L3_ENTRY_PARITY_STATUS_1r,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_DEFIP_DATA_PAR_ERRf, NULL,
      L3_DEFIP_DATA_ONLYm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_DEFIP_PARITY_CONTROLr,
      L3_DEFIP_PARITY_STATUSr, INVALIDr
      INVALIDr, INVALIDr },
	{ PARITY_ENf, L3MC_PAR_ERRf, NULL,
	  L3_IPMCm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
	  L3MC_PARITY_CONTROLr,
	  L3MC_PARITY_STATUSr, INVALIDr,
	  INVALIDr, INVALIDr },
    { PARITY_ENf, MA_STATE_PAR_ERRf, NULL,
      MA_STATEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_STATE_PARITY_CONTROLr,
      MA_STATE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, RMEP_PAR_ERRf, NULL,
      RMEPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      RMEP_PARITY_CONTROLr,
      RMEP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MAID_REDUCTION_PAR_ERRf, NULL,
      MAID_REDUCTIONm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MAID_REDUCTION_PARITY_CONTROLr,
      MAID_REDUCTION_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MA_INDEX_PAR_ERRf, NULL,
      MA_INDEXm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MA_INDEX_PARITY_CONTROLr,
      MA_INDEX_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, PORT_CBL_TABLE_PAR_ERRf, NULL,
      PORT_CBL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      PORT_CBL_TABLE_PARITY_CONTROLr,
      PORT_CBL_TABLE_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, INITIAL_NHOP_PAR_ERRf, NULL,
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
    { PARITY_ENf, VFI_1_PAR_INTRf, NULL,
      VFI_1m, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_1_PARITY_CONTROLr,
      VFI_1_PARITY_STATUS_INTRr, INVALIDr,
      VFI_1_PARITY_STATUS_NACKr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu_ip2_parity_info[] = {
    { PARITY_ENf, ING_NHOP_PAR_ERRf, NULL,
      ING_L3_NEXT_HOPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      ING_L3_NEXT_HOP_PARITY_CONTROLr,
      ING_L3_NEXT_HOP_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_METER_PAR_ERRf, NULL,
      FP_METER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_METER_PARITY_CONTROLr,
      IFP_METER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_COUNTER_PAR_ERRf, NULL,
      FP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_COUNTER_PARITY_CONTROLr,
      IFP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_POLICY_PAR_ERRf, NULL,
      FP_POLICY_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_POLICY_PARITY_CONTROLr,
      IFP_POLICY_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, IFP_STORM_CONTROL_PAR_ERRf, NULL,
      FP_STORM_CONTROL_METERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      IFP_STORM_CONTROL_PARITY_CONTROLr,
      IFP_STORM_CONTROL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, L3_MTU_VALUES_PAR_ERRf, NULL,
      L3_MTU_VALUESm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_MTU_VALUES_PARITY_CONTROLr,
      L3_MTU_VALUES_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_MASK_PAR_ERRf, NULL,
      EGR_MASKm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MASK_PARITY_CONTROLr,
      EGR_MASK_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_SW_PAR_ERRf, NULL,
      MODPORT_MAP_SWm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_SW_PARITY_CONTROLr,
      MODPORT_MAP_SW_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
	{ PARITY_ENf, MODPORT_MAP_SW_PAR_ERRf, NULL,
	  MODPORT_MAPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
	  MODPORT_MAP_SW_PARITY_CONTROLr,
	  MODPORT_MAP_SW_PARITY_STATUSr, INVALIDr,
	  INVALIDr, INVALIDr },   
    { PARITY_ENf, MODPORT_MAP_IM_PAR_ERRf, NULL,
      MODPORT_MAP_IMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_IM_PARITY_CONTROLr,
      MODPORT_MAP_IM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, MODPORT_MAP_EM_PAR_ERRf, NULL,
      MODPORT_MAP_EMm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      MODPORT_MAP_EM_PARITY_CONTROLr,
      MODPORT_MAP_EM_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu_ep_parity_info[] = {
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
/*    { PARITY_ENf, EGR_MPLS_LABEL_PAR_ERRf, NULL,
      EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_CONTROLr,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
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
/*    { PARITY_ENf, EGR_IP_TUNNEL_PAR_ERRf, NULL,
      EGR_IP_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IP_TUNNEL_PARITY_CONTROLr,
      EGR_IP_TUNNEL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, 
    { PARITY_ENf, EGR_PW_COUNT_PAR_ERRf, NULL,
      EGR_PW_INIT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_PW_INIT_COUNTERS_PARITY_CONTROLr,
      EGR_PW_INIT_COUNTERS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_IPFIX_SESS_PAR_ERRf, NULL,
      EGR_IPFIX_SESSION_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IPFIX_SESSION_PARITY_CONTROLr,
      EGR_IPFIX_SESSION_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_IPFIX_EXPORT_PAR_ERRf, NULL,
      EGR_IPFIX_EXPORT_FIFOm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IPFIX_EXPORT_FIFO_PARITY_CONTROLr,
      EGR_IPFIX_EXPORT_FIFO_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
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
    { PARITY_ENf, EGR_PERQ_COUNTERf, "EP_PERQ_COUNTER table parity error",
      EGR_PERQ_XMT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_PERQ_COUNTER_PARITY_CONTROLr,
      EGR_PERQ_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
/*    { PARITY_ENf, EGR_FP_COUNTERf, NULL,
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_FP_COUNTER_PARITY_CONTROLr,
      EGR_FP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, EGR_GP0_DBUFf, "EP_GP0_DATABUF memory parity error",
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GP0_DBUF_PARITY_CONTROLr,
      EGR_GP0_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GP1_DBUFf, "EP_GP1_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GP1_DBUF_PARITY_CONTROLr,
      EGR_GP1_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GP2_DBUFf, "EP_GP2_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_GP2_DBUF_PARITY_CONTROLr,
      EGR_GP2_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ0_DBUFf, "EP_XQ0_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ0_DBUF_PARITY_CONTROLr,
      EGR_XQ0_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ1_DBUFf, "EP_XQ1_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ1_DBUF_PARITY_CONTROLr,
      EGR_XQ1_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ2_DBUFf, "EP_XQ2_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ2_DBUF_PARITY_CONTROLr,
      EGR_XQ2_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_XQ3_DBUFf, "EP_XQ3_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_XQ3_DBUF_PARITY_CONTROLr,
      EGR_XQ3_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
/*    { PARITY_ENf, EGR_LP_DBUFf, "EP_LP_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_LP_DBUF_PARITY_CONTROLr,
      EGR_LP_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr }, */
    { PARITY_ENf, EGR_CM_DBUFf, "EGR_CM_DBUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_CM_DBUF_PARITY_CONTROLr,
      EGR_CM_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

STATIC _soc_parity_info_t _soc_hu_xqp_parity_info[] = {
    { XQBOD_RXFIFOf, XQBOD_RXFIFOf, "XQBOD_RXFIFO",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XQPORT_PARITY_CONTROLr,
      XQPORT_XQBOD_RXFIFO_PARITY_STATUS_INTRr, XQPORT_FORCE_SINGLE_BIT_ERRORr,
      INVALIDr, XQPORT_FORCE_DOUBLE_BIT_ERRORr },
    { XQBODE_TXFIFOf, XQBODE_TXFIFOf, "XQBODE_TXFIFO",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XQPORT_PARITY_CONTROLr,
      XQPORT_XQBODE_TXFIFO_PARITY_STATUS_INTRr, XQPORT_FORCE_SINGLE_BIT_ERRORr,
      INVALIDr, XQPORT_FORCE_DOUBLE_BIT_ERRORr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

static struct {
    uint32             cpi_bit;
    soc_block_t        blocktype;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    _soc_parity_info_t *info;
} _soc_hu_parity_group_info[] = {
    { 0x00002, SOC_BLK_EPIPE, EP_INTR_ENABLEr, EP_INTR_STATUSr, _soc_hu_ep_parity_info },
    { 0x00004, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, _soc_hu_ip0_parity_info },
    { 0x00008, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, IP1_INTR_STATUSr, _soc_hu_ip1_parity_info },
    { 0x00010, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, IP2_INTR_STATUSr, _soc_hu_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { 0x00040, SOC_BLK_XQPORT, XQPORT_INTR_ENABLEr, XQPORT_INTR_STATUSr, _soc_hu_xqp_parity_info }, /* x4 */
    { 0 }, /* table terminator */
};

STATIC soc_hurricane_oam_handler_t hu_oam_handler = NULL;

static soc_ser_functions_t _hurricane_ser_functions;

STATIC int
_soc_hurricane_parity_block_port(int unit, soc_block_t block, soc_port_t *port)
{
    *port = SOC_BLOCK_PORT(unit, block);
    if ((*port != REG_PORT_ANY) && !SOC_PORT_VALID(unit, *port)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane_parity_enable(int unit, int enable)
{
    int group, table;
    uint32 addr, group_enable, regval, cmic_enable = 0, cpi_blk_bit;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    /* loop through each group */
    for (group = 0; _soc_hu_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu_parity_group_info[group].info;
        group_reg = _soc_hu_parity_group_info[group].enable_reg;
        group_enable = 0;

        cpi_blk_bit = _soc_hu_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu_parity_group_info[group].blocktype) {
            if (_soc_hurricane_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].enable_field != INVALIDf; table++) {
                /* handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                case _SOC_PARITY_INFO_TYPE_OAM:
                    enable_field = info[table].enable_field;
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    reg = info[table].control_reg;
                    if (!SOC_REG_IS_VALID(unit, reg)) {
                        continue;
                    }
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &regval));
                    soc_reg_field_set(unit, reg, &regval,
                                      info[table].enable_field,
                                      enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, regval));
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
            addr = soc_reg_addr(unit, group_reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
            cmic_enable |= cpi_blk_bit;
            cpi_blk_bit <<= 1;
        }
    } /* loop through each group */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_CHIP_PARITY_INTR_ENABLEr(unit,
                                                            cmic_enable));
    return SOC_E_NONE;
}

int
_soc_hurricane_mem_parity_control(int unit, soc_mem_t mem,
                                 int copyno, int enable)
{
    int group, table;
    uint32 cpi_blk_bit, misc_cfg;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    if (!soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        /* Parity checking is not enabled, nothing to do */
        return SOC_E_NONE;
    }

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
    for (group = 0; _soc_hu_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu_parity_group_info[group].info;
        group_reg = _soc_hu_parity_group_info[group].enable_reg;
        cpi_blk_bit = _soc_hu_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu_parity_group_info[group].blocktype) {
            if (_soc_hurricane_parity_block_port(unit, blk, &block_port) < 0) {
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
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                    enable_field = info[table].enable_field;
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, group_reg, block_port,
                                                enable_field,
                                                enable ? 1 : 0));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                case _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER:
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
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                          E2EFC_PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHECK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, E2EFC_PARITY_GEN_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, E2EFC_PARITY_STAT_CLEARf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, METERING_CLK_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    } else {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHECK_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, E2EFC_PARITY_GEN_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, METERING_CLK_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane_process_single_parity_error(int unit, int group,
                                          soc_port_t block_port, int table,
                                          int schan, char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val;
    int index, multiple, error;

    info = _soc_hu_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    addr = soc_reg_addr(unit, status_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERRf);
 
    if (error) {
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
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_hurricane_process_single_ecc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       int schan, char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val;
    int index, double_bit, multiple, error;

    info = _soc_hu_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    addr = soc_reg_addr(unit, status_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);
    double_bit = soc_reg_field_get(unit, status_reg,
                                   reg_val, DOUBLE_BIT_ERRf);
    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
    error = soc_reg_field_get(unit, status_reg, reg_val, ECC_ERRf);
 
    if (error) {
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
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_hurricane_process_dual_parity_error(int unit, int group,
                                        soc_port_t block_port, int table,
                                        int schan, char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val, bitmap;
    int index, bucket_index, multiple, ix, bits, size;

    info = _soc_hu_parity_group_info[group].info;

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

        addr = soc_reg_addr(unit, status_reg, block_port, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
        bucket_index =
            soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDXf);
        multiple =
            soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
        bitmap =
            soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BMf);
        size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BMf);

        if (bitmap != 0) {
            for (bits = 0; bits < size; bits++) {
                if (bitmap & (1 << bits)) {
                    index =
                        bucket_index * size * 2 + bits + (ix * size);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d parity error\n"),
                               unit, msg, index));
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
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane_process_mmu_parity_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg, index_reg;
    uint32 addr, reg_val;
    int index;

    info = _soc_hu_parity_group_info[group].info;

    status_reg = info[table].intr_status1_reg;
    index_reg = info[table].intr_status0_reg;
    if (index_reg == INVALIDr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s parity error\n"),
                              unit, msg));
        return SOC_E_NONE;
    }
    if (status_reg != INVALIDr) {
        addr = soc_reg_addr(unit, status_reg, block_port, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
        if (reg_val == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s: parity hardware inconsistency\n"),
                       unit, msg));
            return SOC_E_NONE;
        }
    }
    addr = soc_reg_addr(unit, index_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = reg_val;
 
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s entry %d parity error\n"),
               unit, msg, index));
    /* update soc stats for some cases */
    if (info[table].error_field == CCP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cpcrc++;
    } else if (info[table].error_field == CFAP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cfap++;
    } 

    return SOC_E_NONE;
}


STATIC int
_soc_hurricane_process_parity_error(int unit)
{
    int group, table, index, size;
    uint32 addr, cmic_status, group_status, status,
        group_enable, cpi_blk_bit;
    uint32 entry_idx;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;
    soc_hurricane_oam_handler_t oam_handler_snapshot;

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CHIP_PARITY_INTR_STATUSr(unit,
                                                           &cmic_status));

    /* loop through each group */
    for (group = 0; _soc_hu_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu_parity_group_info[group].info;
        group_reg = _soc_hu_parity_group_info[group].status_reg;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d %s parity processing\n"),
                                unit, SOC_REG_NAME(unit, group_reg)));
        cpi_blk_bit = _soc_hu_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu_parity_group_info[group].blocktype) {
            if (_soc_hurricane_parity_block_port(unit, blk, &block_port) < 0) {
                cpi_blk_bit <<= 1;
                continue;
            }
            if (!(cmic_status & cpi_blk_bit)) {
                cpi_blk_bit <<= 1;
                continue;
            }

            addr = soc_reg_addr(unit, group_reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &group_status));

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
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s asserted\n"),
                                          unit, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane_process_dual_parity_error(unit,
                                       group, block_port, table, FALSE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane_process_mmu_parity_error(unit,
                              group, block_port, table, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                    /*
                     * status0 (MEM1_IPMCGRP_TBL_PARITYERRORPTRr) is index
                     * status1 (MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr) is table id
                     *         MMU_IPMC_GROUP_TBL0m - MMU_IPMC_GROUP_TBL8m
                     */
                    reg = info[table].intr_status1_reg;
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
                    size = soc_reg_field_length(unit, reg, STATUSf);
                    reg = info[table].intr_status0_reg;
                    for (index = 0; index < size; index++, status >>= 1) {
                        if (!(status & 1)) {
                            continue;
                        }
                        addr = soc_reg_addr(unit, reg, block_port, index);
                        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr,
                                                           &entry_idx));
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d %s%d entry %d parity error\n"),
                                   unit, msg, index, entry_idx));
                    }
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                    /*
                     * status0 (WRED_PARITY_ERROR_INFOr) is index
                     * status1 (WRED_PARITY_ERROR_BITMAPr) is table id
                     */
                    reg = info[table].intr_status0_reg;
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr,
                                                       &entry_idx));
                    reg = info[table].intr_status1_reg;
                    addr = soc_reg_addr(unit, reg, block_port, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &status));
                    if (status & 0x000003) {
                        msg = "WRED_CFG_CELL";
                    } else if (status & 0x00000c) {
                        msg = "WRED_THD_0_CELL";
                    } else if (status & 0x000030) {
                        msg = "WRED_THD_1_CELL";
                    } else if (status & 0x0000c0) {
                        msg = "WRED_CFG_PACKET";
                    } else if (status & 0x000300) {
                        msg = "WRED_THD_0_PACKET";
                    } else if (status & 0x000c00) {
                        msg = "WRED_THD_1_PACKET";
                    } else if (status & 0x003000) {
                        msg = "WRED_PORT_CFG_CELL";
                    } else if (status & 0x00c000) {
                        msg = "WRED_PORT_THD_0_CELL";
                    } else if (status & 0x030000) {
                        msg = "WRED_PORT_THD_1_CELL";
                    } else if (status & 0x0c0000) {
                        msg = "WRED_PORT_CFG_PACKET";
                    } else if (status & 0x300000) {
                        msg = "WRED_PORT_THD_0_PACKET";
                    } else if (status & 0xc00000) {
                        msg = "WRED_PORT_THD_1_PACKET";
                    } else {
                        break;
                    }
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d parity error\n"),
                               unit, msg, entry_idx));
                    break;
                case _SOC_PARITY_INFO_TYPE_OAM:
                    /* OAM isn't parity-related but shares an interrupt */

                    oam_handler_snapshot = hu_oam_handler;

                    if (oam_handler_snapshot != NULL)
                    {
                        SOC_IF_ERROR_RETURN(oam_handler_snapshot(unit,
                                            info[table].error_field));
                    }

                    break;
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */

            reg = _soc_hu_parity_group_info[group].enable_reg;
            addr = soc_reg_addr(unit, reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &group_enable));
            group_enable &= ~group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
            group_enable |= group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
        } /* loop through each block for the group */
    } /* loop through each group */

    return SOC_E_NONE;
}

void
soc_hurricane_parity_error(void *unit_vp, void *d1, void *d2,
                         void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    _soc_hurricane_process_parity_error(unit);
    soc_intr_enable(unit, IRQ_MEM_FAIL);
}

STATIC int
_soc_hurricane_mem_nack_error_process(int unit, soc_mem_t mem, int copyno)
{
    int group, table;
    _soc_parity_info_t *info;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;

    if (mem == INVALIDm) {
        return SOC_E_PARAM;
    }

    /* loop through each group */
    for (group = 0; _soc_hu_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu_parity_group_info[group].blocktype) {
            if (copyno != blk) {
                continue;
            }
            if (_soc_hurricane_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (info[table].mem != mem) {
                    continue;
                }
                if (info[table].nack_status0_reg == INVALIDr) {
                    return SOC_E_NOT_FOUND;
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
                        (_soc_hurricane_process_single_parity_error(unit,
                              group, block_port, table, TRUE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane_process_single_ecc_error(unit,
                              group, block_port, table, TRUE, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_hurricane_process_dual_parity_error(unit,
                              group, block_port, table, TRUE, msg));
                    break;
                default:
                    break;
                } /* handle different parity error reporting style */
            } /* loop through each table in the group */
        } /* loop through each block of the group */
    } /* loop through each group */

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane_mem_ecc_force(int unit, soc_port_t block_port,
                            _soc_parity_info_t *info)
{
    soc_reg_t force_sbe_reg, force_dbe_reg;
    soc_field_t force_field;

    force_field = info->error_field;
    if (force_field == RX_FIFO_MEM_ERRf) {
        force_field = RX_FIFO_MEMf;
    } else if (force_field == TX_FIFO_MEM_ERRf) {
        force_field = TX_FIFO_MEMf;
    }

    force_sbe_reg = info->intr_status1_reg;
    force_dbe_reg = info->nack_status1_reg;

    /* Single-bit error force */
    if (SOC_REG_IS_VALID(unit, force_sbe_reg) &&
        soc_reg_field_valid(unit, force_sbe_reg, force_field)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, force_sbe_reg,
                                    block_port, force_field, 1));        
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s.%s not valid\n"),
                   unit, SOC_REG_NAME(unit, force_sbe_reg),
                   SOC_FIELD_NAME(unit, force_field)));
    }

    sal_usleep(1000); /* Wait for interrupt-mode logic to kick in */

    /* Double-bit error force */
    if (SOC_REG_IS_VALID(unit, force_dbe_reg) &&
        soc_reg_field_valid(unit, force_dbe_reg, force_field)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, force_dbe_reg,
                                    block_port, force_field, 1));        
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s.%s not valid\n"),
                   unit, SOC_REG_NAME(unit, force_dbe_reg),
                   SOC_FIELD_NAME(unit, force_field)));
    }

    return SOC_E_NONE;
}

int
_soc_hurricane_mem_nack_error_test(int unit)
{
    int group, table, index, rv;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS];
    uint32 parity, rval;
    _soc_parity_info_t *info;
    soc_mem_t mem;
    int	 index_min, index_max, index_range;
    soc_port_t block_port;
    soc_block_t blk;
    soc_field_t par_ecc_field;

    /* loop through each group */
    for (group = 0; _soc_hu_parity_group_info[group].cpi_bit; group++) {
        info = _soc_hu_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_hu_parity_group_info[group].blocktype) {
            if (_soc_hurricane_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            for (table = 0; info[table].error_field != INVALIDf; table++) {
                mem = info[table].mem;
                if (mem == INVALIDm) {
                    continue;
                }
                if ((info[table].control_reg == INVALIDr) ||
                    !soc_reg_field_valid(unit, info[table].control_reg,
                                         info[table].enable_field)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s has no parity toggle\n"),
                               unit, SOC_MEM_NAME(unit, mem)));
                    continue;
                }

                index_min = soc_mem_index_min(unit, mem);
                index_max = soc_mem_index_max(unit, mem);
                index_range = index_max - index_min + 1;
                index = index_min + (index_range / 2);

                if ((rv = soc_mem_write(unit, mem, blk, index,
                                        soc_mem_entry_null(unit, mem))) < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d mem write error\n"),
                               unit, SOC_MEM_NAME(unit, mem), index));
                    continue;
                }
            
                /* Disable parity */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, info[table].control_reg,
                                            block_port, info[table].enable_field, 0));

                if ((rv = soc_mem_read(unit, mem, blk, index, tmp_entry)) < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d mem read error\n"),
                               unit, SOC_MEM_NAME(unit, mem), index));
                    continue;
                }

                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                case _SOC_PARITY_INFO_TYPE_SINGLE_COUNTER:
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    par_ecc_field = EVEN_PARITYf;
                    if (mem == L3_ECMP_COUNTm) {
                        par_ecc_field = EVEN_PARITY_0f;
                    }
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    if (block_port != REG_PORT_ANY) {
                        /* Force registers exist */
                        SOC_IF_ERROR_RETURN
                            (_soc_hurricane_mem_ecc_force(unit, block_port,
                                                         &(info[table])));
                    }
                    par_ecc_field = ECCf;
                    break;
                default:
                    par_ecc_field = INVALIDf;
                }

                if (!soc_mem_field_valid(unit, mem, par_ecc_field)) {
#if !defined(SOC_NO_NAMES)
                    if (par_ecc_field == INVALIDf) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d %s doesn't contain INVALIDf\n"),
                                   unit, SOC_MEM_NAME(unit, mem)));
                    } else {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d %s doesn't contain %s\n"),
                                   unit, SOC_MEM_NAME(unit, mem),
                                   soc_fieldnames[par_ecc_field]));
                    }
#endif
                    continue;
                }

                parity =
                    soc_mem_field32_get(unit, mem, tmp_entry, par_ecc_field);
                parity ^= 0x1; /* Bad parity */
                soc_mem_field32_set(unit, mem, tmp_entry, par_ecc_field, parity);
            

                if ((rv = soc_mem_write(unit, mem, blk, index, tmp_entry)) < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d mem write error\n"),
                               unit, SOC_MEM_NAME(unit, mem), index));
                    continue;
                }
            
                /* Enable parity */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, info[table].control_reg,
                                            block_port, info[table].enable_field, 1));

                if ((rv = soc_mem_read(unit, mem, blk, index, tmp_entry)) < 0) {
                    if (rv == SOC_E_FAIL) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d NACK received for %s entry %d:\n\t"),
                                   unit, SOC_MEM_NAME(unit, mem), index));
                        if ((rv = _soc_hurricane_mem_nack_error_process(unit,
                                                                       mem, blk)) < 0) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "unit %d %s entry %d SCHAN NACK analysis failure\n"),
                                       unit, SOC_MEM_NAME(unit, mem), index));
                        }
                    } else {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d %s entry %d mem read error\n"),
                                   unit, SOC_MEM_NAME(unit, mem), index));
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d mem parity induction failed\n"),
                               unit, SOC_MEM_NAME(unit, mem), index));
                }
                sal_usleep(1000);
            }
        }
    }

    /* Apply delay so LEDUP can capture correct status. */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP_CTRLr(unit, &rval));
    soc_reg_field_set(unit,CMIC_LEDUP_CTRLr,
                      &rval, LEDUP_SCAN_START_DELAYf, 0x5);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP_CTRLr(unit, rval));

    return SOC_E_NONE;
}

static int
soc_hurricane_pipe_mem_clear(int unit)
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

    /* Need to clear ING_PW_TERM_COUNTERS since it's not handled by reset control */
    if (!(SAL_BOOT_QUICKTURN) && SOC_MEM_IS_VALID(unit, ING_PW_TERM_COUNTERSm)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, ING_PW_TERM_COUNTERSm, COPYNO_ALL, TRUE));
    }
    return SOC_E_NONE;
}


#define  HU_NUM_COS  8
int soc_hu_xq_mem(int unit, soc_port_t port, soc_mem_t *xq)
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
#define HU_MMU_BUFFER_SIZE		(1.5 * 1024 * 1024) 

/* 24GE + 4HG +1CPU ports */ 
#define HU_MMU_TOTAL_PORTS		29 

#define HU_MMU_NUM_COS  		8
#define HU_MMU_CELL_SIZE 		128

/* total cells = 1.5MB/128 = 12288 */
#define HU_MMU_TOTAL_CELLS 		(HU_MMU_BUFFER_SIZE/HU_MMU_CELL_SIZE)

#define HU_MMU_IN_COS_MIN_CELLS		12
#define HU_MMU_IN_COS_MIN_XQS		8

/* cos(8) * reserved cells(12) = 96 */
#define HU_MMU_IN_PORT_STATIC_CELLS	(HU_MMU_IN_COS_MIN_XQS * HU_MMU_IN_COS_MIN_CELLS)

/* ports(29) * per port cells(96) = 2784 */
#define HU_MMU_TOTAL_STATIC_CELLS	(HU_MMU_TOTAL_PORTS * HU_MMU_IN_PORT_STATIC_CELLS)

/* total cells(12288) - static cells(2784) = 9504 */
#define HU_MMU_TOTAL_DYNAMIC_CELLS 	(HU_MMU_TOTAL_CELLS - HU_MMU_TOTAL_STATIC_CELLS)

/* Every port has 1536 packet pointers(XQs) */
#define HU_MMU_IN_PORT_TOTAL_XQS	1536
/* number of cos(8) * minimum xqs per cos(8) = 64 */
#define HU_MMU_IN_PORT_STATIC_XQS	(HU_MMU_NUM_COS * HU_MMU_IN_COS_MIN_XQS)
/* total xqs (1536)- static xqs(64) = 1472 */
#define HU_MMU_IN_PORT_DYNAMIC_XQS	(HU_MMU_IN_PORT_TOTAL_XQS - HU_MMU_IN_PORT_STATIC_XQS)

#define HU_MMU_MAX_PKT_SIZE		9728
/* xoff threshold = 80 */
#define HU_MMU_XOFF_CELL_LIMIT		((HU_MMU_MAX_PKT_SIZE/HU_MMU_CELL_SIZE) + 4)

/* max limit for pri-0 = (xoff threshold(80) + max packet size/128 *2 + 16)*23 */
#define HU_MMU_MAX_CELL_LIMIT_PRI_0     5704	
/* max limit for pri-1-7 =  (total xqs/128 * 4)= 1536/128*4 */
#define HU_MMU_MAX_CELL_LIMIT_PRI_1_7   48	

/* cell limit0 + 7 * cell limit for pri1-7 = 6040 */ 
#define HU_MMU_DYN_CELL_LIMIT		(HU_MMU_MAX_CELL_LIMIT_PRI_0 +  \
                                         ((HU_MMU_NUM_COS -1) * HU_MMU_MAX_CELL_LIMIT_PRI_1_7))

int
soc_hurricane_mmu_init(int unit)
{
    int         cos;
    soc_port_t  port;
    uint32      val, oval, cfap_max_idx, tm;
    int         age[HU_NUM_COS], max_age, min_age;
    int         age_enable, disabled_age;
    int         skid_mark;
    uint32 	dyn_xq_per_port;
    uint32      limit;
    uint32 	xq_limit, pkt_limit, cell_limit;
    uint32      cos_min_xqs, cos_min_cells;
    uint16      dev_id;
    uint8       rev_id;

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
    
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id != BCM56147_DEVICE_ID) {
        /* total dynamic xqs - skid mark - 9(reserved xqs) */	
        dyn_xq_per_port = HU_MMU_IN_PORT_DYNAMIC_XQS -  skid_mark - 9;
        /* limit includes the static minimum cells as well */
        xq_limit = dyn_xq_per_port + 8;
        cos_min_xqs = HU_MMU_IN_COS_MIN_XQS;
        cos_min_cells = HU_MMU_IN_COS_MIN_CELLS;
    }
    else {
        /* dynamic xqs = total xqs - static xqs(num of cos * per cos reserve(4)) */
        dyn_xq_per_port = HU_MMU_IN_PORT_TOTAL_XQS - (HU_MMU_NUM_COS * 4); 
        /* total dynamic xqs = dynamic xqs - skid mark - 9(reserved xqs) */	
        dyn_xq_per_port = dyn_xq_per_port -  skid_mark - 9;
        /* limit includes the static minimum cells as well */
        xq_limit = dyn_xq_per_port + 4;
        cos_min_xqs = HU_MMU_IN_COS_MIN_XQS/2;
        cos_min_cells = HU_MMU_IN_COS_MIN_CELLS + 4;
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
                                      CELLMAXLIMITf, HU_MMU_MAX_CELL_LIMIT_PRI_0);
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXRESUMELIMITf, HU_MMU_MAX_CELL_LIMIT_PRI_0 - 12);
                } else {
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXLIMITf, HU_MMU_MAX_CELL_LIMIT_PRI_1_7);
                    soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &cell_limit,
                                      CELLMAXRESUMELIMITf, HU_MMU_MAX_CELL_LIMIT_PRI_1_7 - 12);
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
                      DISCARDSETLIMITf, HU_MMU_TOTAL_CELLS-1);
    SOC_IF_ERROR_RETURN(WRITE_IBPDISCARDSETLIMITr(unit, port, cell_limit));

    if (dev_id != BCM56147_DEVICE_ID) {
    	val = 0;
    	soc_reg_field_set(unit, DYNCELLLIMITr, &val,
        	              DYNCELLSETLIMITf, HU_MMU_DYN_CELL_LIMIT);
    	soc_reg_field_set(unit, DYNCELLLIMITr, &val,
        	              DYNCELLRESETLIMITf, HU_MMU_DYN_CELL_LIMIT - 24);
    	SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val)); 

    	pkt_limit = 0;
    	soc_reg_field_set(unit, IBPPKTSETLIMITr, &pkt_limit,
        	              PKTSETLIMITf, HU_MMU_IN_COS_MIN_CELLS);
    	SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, pkt_limit));    

    	cell_limit = 0;
    	soc_reg_field_set(unit, IBPCELLSETLIMITr, &cell_limit, 
        	              CELLSETLIMITf, HU_MMU_XOFF_CELL_LIMIT);   
    	SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, cell_limit));
	}
    else {
        /* use the settings given by design team for 56147 */
    	val = 0;
        /* dyn cell limit = 8000 + (total cos -1)*4*2048/128 */
    	soc_reg_field_set(unit, DYNCELLLIMITr, &val,
        	              DYNCELLSETLIMITf, 8448);
    	soc_reg_field_set(unit, DYNCELLLIMITr, &val,
        	              DYNCELLRESETLIMITf, 8424);
    	SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, port, val)); 

    	pkt_limit = 0;
    	soc_reg_field_set(unit, IBPPKTSETLIMITr, &pkt_limit,
        	              PKTSETLIMITf, 33);
    	SOC_IF_ERROR_RETURN(WRITE_IBPPKTSETLIMITr(unit, port, pkt_limit));    

    	cell_limit = 0;
    	soc_reg_field_set(unit, IBPCELLSETLIMITr, &cell_limit, 
        	              CELLSETLIMITf, 36);   
    	SOC_IF_ERROR_RETURN(WRITE_IBPCELLSETLIMITr(unit, port, cell_limit));
   
        /* E2EFC configuration */
        if (IS_HG_PORT(unit, port)){
            SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &val));
            soc_reg_field_set(unit, XPORT_CONFIGr, &val, E2E_IBP_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, val));

            SOC_IF_ERROR_RETURN(READ_IE2E_CONTROLr(unit, port, &val));
            soc_reg_field_set(unit, IE2E_CONTROLr, &val, IBP_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_IE2E_CONTROLr(unit, port, val));

            val = 0;
            soc_reg_field_set(unit, E2EFC_HG_MIN_TX_TIMERr, &val, 
                              LGf, 0);
            soc_reg_field_set(unit, E2EFC_HG_MIN_TX_TIMERr, &val, 
                              TIMERf, 1);                
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_HG_MIN_TX_TIMERr(unit, port-26, val));

            val = 0;
            soc_reg_field_set(unit, E2EFC_HG_MAX_TX_TIMERr, &val, 
                              LGf, 1);
            soc_reg_field_set(unit, E2EFC_HG_MAX_TX_TIMERr, &val, 
                              TIMERf, 640);                
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_HG_MAX_TX_TIMERr(unit, port-26, val));

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
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_RESET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                              PKT_DISC_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                              CELL_DISC_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_DISC_LIMITr(unit, port, limit));
        }
        else {
            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit, 
                              PKT_SET_LIMITf, 33);
            soc_reg_field_set(unit, E2EFC_CNT_SET_LIMITr, &limit, 
                              CELL_SET_LIMITf, 36);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_SET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit, 
                              PKT_RESET_LIMITf, 24);
            soc_reg_field_set(unit, E2EFC_CNT_RESET_LIMITr, &limit, 
                              CELL_RESET_LIMITf, 26);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_RESET_LIMITr(unit, port, limit));

            limit = 0;
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                              PKT_DISC_LIMITf, 1456);
            soc_reg_field_set(unit, E2EFC_CNT_DISC_LIMITr, &limit, 
                              CELL_DISC_LIMITf, 9504);
            SOC_IF_ERROR_RETURN(WRITE_E2EFC_CNT_DISC_LIMITr(unit, port, limit));

        }

	}

    }

    /* device specific dynamic cell limit */
    if (dev_id != BCM56147_DEVICE_ID) {
   	cell_limit = 0;
    	soc_reg_field_set(unit, TOTALDYNCELLSETLIMITr,
                          &cell_limit, TOTALDYNCELLSETLIMITf, HU_MMU_TOTAL_DYNAMIC_CELLS);
    	SOC_IF_ERROR_RETURN(WRITE_TOTALDYNCELLSETLIMITr(unit, cell_limit));
    	cell_limit = 0;
    	soc_reg_field_set(unit, TOTALDYNCELLRESETLIMITr,
                          &cell_limit, TOTALDYNCELLRESETLIMITf, HU_MMU_TOTAL_DYNAMIC_CELLS - 24);
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
    
    val = 0;
    soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r, &val,
                      ALL_TCAMS_TM_7_0f, tm);
    soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r, &val,
                      ALL_GLOBAL_MASK_TCAMS_TM_7_0f, tm);
    SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_TM_7_THRU_0r(unit, val));

    /*
     * Port enable
     */
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLEr, &val, MMUPORTENABLEf,
                      SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLEr(unit, val));

    return SOC_E_NONE;
}



uint32 hu_tdm[75] = {2,14,26,27,28,29,
                  3,15,26,27,28,29,
                  4,16,26,27,28,29,
                  5,17,26,27,28,29,
                  0,                  
                  6,18,26,27,28,29,
                  7,19,26,27,28,29,
                  8,20,26,27,28,29,
                  9,21,26,27,28,29,
                  0,
                  10,22,26,27,28,29,
                  11,23,26,27,28,29,
                  12,24,26,27,28,29,
                  13,25,26,27,28,29,
                  30};

int
soc_hurricane_misc_init(int unit)
{
#define NUM_XQPORT 4
    uint32              rval; 
    uint32              prev_reg_addr;
    uint64              reg64;
    int                 port, i;
    uint32              mode;
    uint32              *arr;
    int                 tdm_size;
    iarb_tdm_table_entry_t iarb_tdm;
    mmu_arb_tdm_table_entry_t mmu_arb_tdm;
    uint32              imask;
    soc_field_t         fields[3];
    uint32              values[3];

    for (port = 26; port <= 29; port++) {
        if (SOC_PORT_VALID(unit, port)) {
            mode = 1; /* gport mode */
            if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
                soc_port_cmap_set(unit, port, SOC_CTR_TYPE_XE);
                mode = 2; /* xport mode */
            } 
            SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
            soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, XQPORT_MODE_BITSf, mode);
            SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, port, rval));
        } /* if SOC_PORT_VALID */
    } /* for port= */

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 83);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    /* Program for Different TDM mode */
    tdm_size = 75;
    arr = hu_tdm;
    for (i = 0; i < tdm_size; i++) {
        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&mmu_arb_tdm, 0, sizeof(mmu_arb_tdm_table_entry_t));
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 
                                        arr[i]);
        soc_MMU_ARB_TDM_TABLEm_field32_set(unit, &mmu_arb_tdm, PORT_NUMf, 
                                       arr[i]); 
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

    if (!SOC_IS_RELOADING(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_hurricane_pipe_mem_clear(unit));
    }

    /* Turn on ingress/egress/mmu parity */
    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        _soc_hurricane_parity_enable(unit, TRUE);
        if (soc_property_get(unit, "parity_test", FALSE)) {
            _soc_hurricane_mem_nack_error_test(unit);
        }
        memset(&_hurricane_ser_functions, 0, sizeof(soc_ser_functions_t));
        _hurricane_ser_functions._soc_ser_parity_error_intr_f = 
            &soc_hurricane_parity_error;
        soc_ser_function_register(unit, &_hurricane_ser_functions);
    }
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

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
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
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
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
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

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    PBMP_E_ITER(unit, port) {
        if (IS_XQ_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        if (IS_XQ_PORT(unit, port)) {
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

    /* Enable MMU parity error interrupt */
    SOC_IF_ERROR_RETURN(WRITE_MEMFAILINTSTATUSr(unit, 0));
    SOC_IF_ERROR_RETURN(READ_MEMFAILINTMASKr(unit, &imask));
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CFAPPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CCPPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CBPPKTHDRPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CBPCELLHDRPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      XQPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CELLECCERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CFAPFAILINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      SOFTRESETINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CELLNOTIPINTMASKf, 1);
    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                          IPMCREPOVERLIMITERRORINTMASKf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_MEMFAILINTMASKr(unit, imask));

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
    /* Apply delay so LEDUP can capture correct status. */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP_CTRLr(unit, &rval));
    soc_reg_field_set(unit,CMIC_LEDUP_CTRLr,
                     &rval, LEDUP_SCAN_START_DELAYf, 0x5);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP_CTRLr(unit, rval));

    return SOC_E_NONE;
}

int
soc_hurricane_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_hurricane_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_hurricane_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

void soc_hurricane_oam_handler_register(soc_hurricane_oam_handler_t handler)
{
    hu_oam_handler = handler;
}

int
soc_hu_xqport_mode_change(int unit, soc_port_t port, soc_mac_mode_t mode)
{
    int rv = SOC_E_NONE;
    uint32 old_bits, bits, rval, to_usec, max_sz;
    uint64 rval64, max_sz64;
    soc_field_t xq_rst2 = INVALIDf;
    soc_field_t ep_port_rst = INVALIDf;
    soc_field_t ep_mmu_rst = INVALIDf;
    mac_driver_t *macd;
    int autoneg, autoneg_done, speed, duplex, enable, master;
    int pause_rx, pause_tx, dual_modid;
    soc_port_ability_t ability;
    soc_reg_t           reg;
    int reset_sleep_usec;
    int hypercore_mixed_mode = 0;

    if (!SOC_PORT_VALID(unit, port)) {
        return SOC_E_PORT;
    }

    hypercore_mixed_mode =
        soc_property_get(unit, spn_BCM5614X_HYPERCORE_MIXED_MODE, 0);

    sal_memset(&ability, 0, sizeof(soc_port_ability_t));

    SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
    old_bits = soc_reg_field_get(unit, XQPORT_MODE_REGr, rval,
                                 XQPORT_MODE_BITSf);
    bits = mode == SOC_MAC_MODE_10000 ? 2 : 1;
    if (bits == old_bits) {
        return SOC_E_NONE;
    }

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* get current MAC and PHY settings */
    SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_get(unit, port, &autoneg,
                                                       &autoneg_done));
    SOC_IF_ERROR_RETURN(soc_phyctrl_ability_advert_get(unit, port, &ability));
    SOC_IF_ERROR_RETURN(soc_phyctrl_speed_get(unit, port, &speed));
    SOC_IF_ERROR_RETURN(soc_phyctrl_duplex_get(unit, port, &duplex));
    SOC_IF_ERROR_RETURN(soc_phyctrl_enable_get(unit, port, &enable));
    SOC_IF_ERROR_RETURN(soc_phyctrl_master_get(unit, port, &master));

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));
    SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN(READ_MAC_CNTMAXSZr(unit, port, &max_sz64));
    SOC_IF_ERROR_RETURN(READ_QPORT_CNTMAXSIZEr(unit, port, &max_sz));

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));

    /* Disable MAC TX and RX */
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, FALSE));

    /* Change the XQPORT block mode */
    SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
    soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, 
                      XQPORT_MODE_BITSf, bits);
    SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, port, rval));
    
    /* Disable MAC TX and RX */
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, FALSE));

    /* Block linkscan and sbus access */
    soc_linkscan_pause(unit);
    COUNTER_LOCK(unit);
    /* Egress disable */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
    sal_usleep(to_usec);

    if (hypercore_mixed_mode == 0) {
        /* Disable mixed mode speed for XQPorts belongig to same hypercore.
        * In this mode changing speed on XQPort connected to lane0 impacts the setting on
        * XQPort connected to lane2.*/
        switch (port) {
            case 26:
                ep_port_rst = EGR_XQP0_PORT_INT_RESETf;
                ep_mmu_rst = EGR_XQP0_MMU_INT_RESETf;
                xq_rst2 = XQ0_HOTSWAP_RST_Lf;
                break;
            case 27:
                ep_port_rst = EGR_XQP1_PORT_INT_RESETf;
                ep_mmu_rst = EGR_XQP1_MMU_INT_RESETf;
                xq_rst2 = XQ1_HOTSWAP_RST_Lf;
                break;
            case 28:
                ep_port_rst = EGR_XQP2_PORT_INT_RESETf;
                ep_mmu_rst = EGR_XQP2_MMU_INT_RESETf;
                xq_rst2 = XQ2_HOTSWAP_RST_Lf;
                break;
            case 29:
                ep_port_rst = EGR_XQP3_PORT_INT_RESETf;
                ep_mmu_rst = EGR_XQP3_MMU_INT_RESETf;
                xq_rst2 = XQ3_HOTSWAP_RST_Lf;
                break;
            default:
                return SOC_E_PORT;
        }

        /* Hold the XQPORT hotswap reset */
        rval = 0;
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, xq_rst2, 0);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));
        sal_usleep(to_usec);

        /* Clear the ECRC register */
        SOC_IF_ERROR_RETURN(WRITE_EGRESSCELLREQUESTCOUNTr(unit, port, 0));

        /* Reset the EP */
        SOC_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &rval,
                ep_port_rst, 1);
        soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &rval,
                ep_mmu_rst, 1);
        SOC_IF_ERROR_RETURN(WRITE_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, rval));
        sal_usleep(to_usec);

        SOC_IF_ERROR_RETURN(READ_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &rval,
                ep_port_rst, 0);
        soc_reg_field_set(unit, EDATABUF_XQP_FLEXPORT_CONFIGr, &rval,
                ep_mmu_rst, 0);
        SOC_IF_ERROR_RETURN(WRITE_EDATABUF_XQP_FLEXPORT_CONFIGr(unit, rval));
        sal_usleep(to_usec);

        /* Bring the XQPORT block out of reset */
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, xq_rst2, 1);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));
        sal_usleep(to_usec);

        /* Bring the hyperlite out of reset */
        soc_xgxs_reset(unit, port);
        SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, XQPORT_XGXS_NEWCTL_REGr,
            port, TXD1G_FIFO_RSTBf, 0xf));

        /* Port probe and initialization */
        PHY_FLAGS_CLR(unit,port,~PHY_FLAGS_INIT_DONE);
        SOC_COUNTER_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port));
        SOC_COUNTER_IF_ERROR_RETURN(soc_phyctrl_probe(unit, port));
        SOC_COUNTER_IF_ERROR_RETURN(soc_phyctrl_init(unit, port));
        /* end of disable mixed mode in hypercore */
    } else {
        /* Enable mixed mode speed for XQPorts belongig to same hypercore.
        * In this mode, the XQPorts support mixed mode speed
        * without switch reset. Changing speed on a XQPort does not impact the
        * settings on the other XQPort in the same hypercore */

        /* Bring the hyperlite out of reset */
        reset_sleep_usec = 1100;
        reg = MAC_XGXS_CTRLr;

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, LCREFENf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        sal_usleep(reset_sleep_usec);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, AFIFO_RSTf, 1);
        soc_reg64_field32_set(unit, reg, &rval64, TXFIFO_RSTLf, 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        sal_usleep(reset_sleep_usec);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
        soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        sal_usleep(reset_sleep_usec);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        sal_usleep(reset_sleep_usec);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_MDIOREGSf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        sal_usleep(reset_sleep_usec);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_PLLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        sal_usleep(reset_sleep_usec);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, BIGMACRSTLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

        /* Bring Tx FIFO out of reset */
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, TXFIFO_RSTLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

        SOC_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, XQPORT_XGXS_NEWCTL_REGr,
            port, TXD1G_FIFO_RSTBf, 0xf));
        /* end of enable mixed mode in hypercore */
    }

    /* Change the XQPORT block mode */
    SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
    soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, 
                      XQPORT_MODE_BITSf, bits);
    SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, port, rval));

    /* Need to reset the Bigmac to make registers accessible */
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &rval64));
    soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, TXRESETf, 0);
    soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, RXRESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, rval64));

    /* restore PHY setting */
    SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_set(unit, port, autoneg));
    SOC_IF_ERROR_RETURN(soc_phyctrl_ability_advert_set(unit, port, &ability));
    if (!autoneg) {
        SOC_IF_ERROR_RETURN(soc_phyctrl_speed_set(unit, port, speed));
        SOC_IF_ERROR_RETURN(soc_phyctrl_duplex_set(unit, port, duplex));
    }
    SOC_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, port, enable));
    SOC_IF_ERROR_RETURN(soc_phyctrl_master_set(unit, port, master));

    if (mode == SOC_MAC_MODE_10000) {
        soc_port_cmap_set(unit, port, SOC_CTR_TYPE_XE);
    } else {
        soc_port_cmap_set(unit, port, SOC_CTR_TYPE_GE);
    }

    /* Port probe and initialization */
    SOC_IF_ERROR_RETURN(MAC_INIT(macd, unit, port));

    /* Restore previous MAC settings */
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, enable));
    SOC_IF_ERROR_RETURN(MAC_PAUSE_SET(macd, unit, port, pause_tx, pause_rx));

    /* Restore XQPORT registers that may have been reset */
    dual_modid = (NUM_MODID(unit) == 2) ? 1 : 0;
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XMODID_ENr, port,
                                               DUAL_MODID_ENf, dual_modid));
    SOC_IF_ERROR_RETURN(WRITE_MAC_CNTMAXSZr(unit, port, max_sz64));
    SOC_IF_ERROR_RETURN(WRITE_QPORT_CNTMAXSIZEr(unit, port, max_sz));

    /* Enable MAC TX and RX */
    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, port, TRUE));

    /* Egress Enable */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));

    /* Resume sbus access and linkscan */
    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit);
    return rv;
}
#endif /* BCM_HURRICANE_SUPPORT */
