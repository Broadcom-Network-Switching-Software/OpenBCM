/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        enduro.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/firebolt.h>
#include <soc/bradley.h>
#include <soc/enduro.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/er_tcam.h>
#include <soc/memtune.h>
#include <soc/devids.h>
#include <soc/soc_ser_log.h>

#ifdef BCM_ENDURO_SUPPORT

/*
 * Enduro chip driver functions.  
 */
soc_functions_t soc_enduro_drv_funs = {
    soc_enduro_misc_init,
    soc_enduro_mmu_init,
    soc_enduro_age_timer_get,
    soc_enduro_age_timer_max_get,
    soc_enduro_age_timer_set,
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
    _SOC_PARITY_INFO_TYPE_MMUMTRO,
    _SOC_PARITY_INFO_TYPE_MMUE2EFC,
    _SOC_PARITY_INFO_TYPE_OAM, /* Not parity, but same interrupt */
    _SOC_PARITY_INFO_TYPE_SER, /* Parity error from CMIC SER module */
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

#if defined(SER_TR_TEST_SUPPORT)
soc_ser_test_functions_t ser_enduro_test_fun;
#endif

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

static _soc_parity_info_t _soc_en_ip0_parity_info[] = {
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

static _soc_parity_info_t _soc_en_ip1_parity_info[] = {
    { PARITY_ENf, VFI_PAR_ERRf, NULL,
      VFIm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      VFI_PARITY_CONTROLr,
      VFI_PARITY_STATUS_INTRr, INVALIDr,
      VFI_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, SVP_PAR_ERRf, NULL,
      SOURCE_VPm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      SOURCE_VP_PARITY_CONTROLr,
      SOURCE_VP_PARITY_STATUS_INTRr, INVALIDr,
      SOURCE_VP_PARITY_STATUS_NACKr, INVALIDr },
    { PARITY_ENf, L3_IIF_PAR_ERRf, NULL,
      L3_IIFm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      L3_IIF_PARITY_CONTROLr,
      L3_IIF_PARITY_STATUS_INTRr, INVALIDr,
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
      L3_DEFIP_PARITY_STATUSr, INVALIDr,
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

static _soc_parity_info_t _soc_en_ip2_parity_info[] = {
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

static _soc_parity_info_t _soc_en_ep_parity_info[] = {
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
    { PARITY_ENf, EGR_MPLS_LABEL_PAR_ERRf, NULL,
      EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_CONTROLr,
      EGR_MPLS_VC_AND_SWAP_LABEL_PARITY_STATUSr, INVALIDr,
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
    { PARITY_ENf, EGR_IP_TUNNEL_PAR_ERRf, NULL,
      EGR_IP_TUNNELm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_IP_TUNNEL_PARITY_CONTROLr,
      EGR_IP_TUNNEL_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_PW_COUNT_PAR_ERRf, NULL,
      EGR_PW_INIT_COUNTERSm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_PW_INIT_COUNTERS_PARITY_CONTROLr,
      EGR_PW_INIT_COUNTERS_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
/*    { PARITY_ENf, EGR_IPFIX_SESS_PAR_ERRf, NULL,
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
    { PARITY_ENf, EGR_FP_COUNTERf, NULL,
      EFP_COUNTER_TABLEm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_FP_COUNTER_PARITY_CONTROLr,
      EGR_FP_COUNTER_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_GP0_DBUFf, "EP_GP0_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
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
    { PARITY_ENf, EGR_LP_DBUFf, "EP_LP_DATABUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_LP_DBUF_PARITY_CONTROLr,
      EGR_LP_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { PARITY_ENf, EGR_CM_DBUFf, "EGR_CM_DBUF memory parity error",
      INVALIDm, _SOC_PARITY_INFO_TYPE_SINGLE_PARITY,
      EGR_CM_DBUF_PARITY_CONTROLr,
      EGR_CM_DBUF_PARITY_STATUSr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

/*
 * Is DEQ_CBPERRPTRr for MMU_CBPDATA0 - MMU_CBPDATA31 (15-bit address)?
 * How does MMU_CELLCHKm (15-bit address) report parity error?
 */
static _soc_parity_info_t _soc_en_mmu_parity_info[] = {
    { AGING_CTR_PAR_ERR_ENf, AGING_CTR_PAR_ERRf, NULL,
      MMU_AGING_CTRm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { AGING_EXP_PAR_ERR_ENf, AGING_EXP_PAR_ERRf, NULL,
      MMU_AGING_EXPm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { CCP_PAR_ERR_ENf, CCP_PAR_ERRf, NULL,
      MMU_CCP_MEMm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      CCPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { CFAP_PAR_ERR_ENf, CFAP_PAR_ERRf, NULL,
      MMU_CFAP_MEMm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      CFAPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { CFAP_MEM_FAIL_ENf, CFAP_MEM_FAILf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR_CPU_ERR_ENf, DEQ_PKTHDR_CPU_ERRf, NULL,
      MMU_CBPPKTHEADERCPUm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      DEQ_PKTHDRCPUERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR0_ERR_ENf, DEQ_PKTHDR0_ERRf, NULL,
      MMU_CBPPKTHEADER0_MEM0m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      DEQ_PKTHDR0ERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ0_NOT_IP_ERR_ENf, DEQ0_NOT_IP_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { DEQ0_CELLCRC_ERR_ENf, DEQ0_CELLCRC_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { START_BY_START_ERR_ENf, START_BY_START_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { MEM1_IPMC_TBL_PAR_ERR_ENf, MEM1_IPMC_TBL_PAR_ERRf, "MMU_IPMC_GROUP_TBL",
      INVALIDm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      MEM1_IPMCGRP_TBL_PARITYERRORPTRr, MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr,
      INVALIDr, INVALIDr },
    { MEM1_VLAN_TBL_PAR_ERR_ENf, MEM1_VLAN_TBL_PAR_ERRf, NULL,
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      MEM1_IPMCVLAN_TBL_PARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_CELLHDR_PAR_ERR_ENf, TOQ0_CELLHDR_PAR_ERRf, NULL,
      MMU_CBPCELLHEADERm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_CELLHDRERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_PKTHDR1_PAR_ERR_ENf, TOQ0_PKTHDR1_PAR_ERRf, NULL,
      MMU_CBPPKTHEADER1_MEM0m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_PKTHDR1ERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_PKTLINK_PAR_ERR_ENf, TOQ0_PKTLINK_PAR_ERRf, NULL,
      MMU_PKTLINK0m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_PKTLINKERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_CELLLINK_PAR_ERR_ENf, TOQ0_CELLLINK_PAR_ERRf, NULL,
      MMU_CELLLINKm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_CELLLINKERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { TOQ0_IPMC_TBL_PAR_ERR_ENf, TOQ0_IPMC_TBL_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUIPMC,
      INVALIDr,
      TOQ_IPMCGRPERRPTR0r, TOQ_ERRINTR1r,
      INVALIDr, INVALIDr },
    { TOQ0_VLAN_TBL_PAR_ERR_ENf, TOQ0_VLAN_TBL_PAR_ERRf, NULL,
      MMU_IPMC_VLAN_TBLm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      TOQ_IPMCVLANERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { MTRO_PAR_ERR_ENf, MTRO_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUMTRO,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { PFAP_PAR_ERR_ENf, PFAP_PAR_ERRf, NULL,
      MMU_PFAP_MEMm, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      PFAPPARITYERRORPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { PFAP_MEM_FAIL_ENf, PFAP_MEM_FAILf, NULL,
      MMU_PFAP_MEMm, _SOC_PARITY_INFO_TYPE_GENERIC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { WRED_PAR_ERR_ENf, WRED_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUWRED,
      INVALIDr,
      WRED_PARITY_ERROR_INFOr, WRED_PARITY_ERROR_BITMAPr,
      INVALIDr, INVALIDr },
    { DEQ_PKTHDR2_ERR_ENf, DEQ_PKTHDR2_PAR_ERRf, NULL,
      MMU_CBPPKTHEADER2m, _SOC_PARITY_INFO_TYPE_MMU_PARITY,
      INVALIDr,
      DEQ_PKTHDR2ERRPTRr, INVALIDr,
      INVALIDr, INVALIDr },
    { E2EFC_ERR_ENf, E2EFC_PAR_ERRf, NULL,
      INVALIDm, _SOC_PARITY_INFO_TYPE_MMUE2EFC,
      INVALIDr,
      INVALIDr, INVALIDr,
      INVALIDr, INVALIDr },
    { INVALIDf, INVALIDf }, /* table terminator */
};

static _soc_parity_info_t _soc_en_xqp_parity_info[] = {
    { EHG_TX_DATAf, EHG_TX_DATAf, NULL,
      XQPORT_EHG_TX_TUNNEL_DATAm, _SOC_PARITY_INFO_TYPE_SINGLE_ECC,
      XQPORT_PARITY_CONTROLr,
      XQPORT_EHG_TX_DATA_PARITY_STATUS_INTRr, XQPORT_FORCE_SINGLE_BIT_ERRORr,
      XQPORT_EHG_TX_DATA_PARITY_STATUS_NACKr, XQPORT_FORCE_DOUBLE_BIT_ERRORr },
    
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
} _soc_en_parity_group_info[] = {
    { 0x00001, SOC_BLK_MMU, MEM_FAIL_INT_ENr, MEM_FAIL_INT_STATr, _soc_en_mmu_parity_info },
    { 0x00002, SOC_BLK_EPIPE, EP_INTR_ENABLEr, EP_INTR_STATUSr, _soc_en_ep_parity_info },
    { 0x00004, SOC_BLK_IPIPE, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, _soc_en_ip0_parity_info },
    { 0x00008, SOC_BLK_IPIPE, IP1_INTR_ENABLEr, IP1_INTR_STATUSr, _soc_en_ip1_parity_info },
    { 0x00010, SOC_BLK_IPIPE, IP2_INTR_ENABLEr, IP2_INTR_STATUSr, _soc_en_ip2_parity_info },
 /* { 0x00020, PCIE_REPLAY_PERR, }, */
    { 0x00040, SOC_BLK_XQPORT, XQPORT_INTR_ENABLEr, XQPORT_INTR_STATUSr, _soc_en_xqp_parity_info }, /* x4 */
    { 0 }, /* table terminator */
};

soc_enduro_oam_handler_t en_oam_handler[SOC_MAX_NUM_DEVICES] = {NULL};

static soc_ser_functions_t _enduro_ser_functions;

#define _SOC_ENDURO_SER_REG 1
#define _SOC_ENDURO_SER_MEM 0

typedef union _enduro_ser_nack_reg_mem_u {
    soc_reg_t reg;
    soc_mem_t mem;
} _enduro_ser_nack_reg_mem_t;

STATIC int
_soc_enduro_parity_block_port(int unit, soc_block_t block, soc_port_t *port)
{
    *port = SOC_BLOCK_PORT(unit, block);
    if ((*port != REG_PORT_ANY) && !SOC_PORT_VALID(unit, *port)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_parity_enable(int unit, int enable)
{
    int group, table;
    uint32 addr, group_enable, regval, misc_cfg, cmic_enable = 0, cpi_blk_bit;
    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_field_t enable_field;
    soc_block_t blk;

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].enable_reg;
        group_enable = 0;

        cpi_blk_bit = _soc_en_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
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
                case _SOC_PARITY_INFO_TYPE_MMUMTRO:
                case _SOC_PARITY_INFO_TYPE_MMUE2EFC:
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

    /* MMU enables */
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

STATIC void
_soc_enduro_mem_parity_info(int unit, int block_info_idx,
                            soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}

int
_soc_enduro_mem_parity_control(int unit, soc_mem_t mem,
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
    case VLAN_MACm:
        mem = VLAN_XLATEm;
        break;
    case L2_ENTRY_ONLYm:
        mem = L2Xm;
        break;
    case L2_USER_ENTRY_ONLYm:
    case L2_USER_ENTRY_DATA_ONLYm:
        mem = L2_USER_ENTRYm;
        break;
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        mem = L3_ENTRY_ONLYm;
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
    default:
        /* Do nothing, keep memory as provided */
        break;
    }


    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].enable_reg;
        cpi_blk_bit = _soc_en_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
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
                case _SOC_PARITY_INFO_TYPE_MMUMTRO:
                case _SOC_PARITY_INFO_TYPE_MMUE2EFC:
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
                    /* coverity[assigned_value] */
                    enable_field = INVALIDf;
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
                          PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                          PARITY_STAT_CLEARf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, METERING_CLK_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    } else {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_CHK_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, PARITY_GEN_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg, METERING_CLK_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_single_parity_error(int unit, int group,
                                          soc_port_t block_port, int table,
                                          int schan, char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val, minfo;
    int index, multiple, error, rv;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    info = _soc_en_parity_group_info[group].info;

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
        _soc_enduro_mem_parity_info(unit, block, info[table].error_field, 
                                    &minfo);
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
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = info[table].mem;
            spci.blk_type = block;
            spci.index = soc_enduro_mem_index_remap(unit, spci.mem, index);
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
                              "unit %d %s: parity hardware inconsistency\n"),
                   unit, msg));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_single_ecc_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       int schan, char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val, minfo;
    int rv, index, single_bit = 0, double_bit = 0, multiple = 0, error = 0;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    info = _soc_en_parity_group_info[group].info;

    status_reg = schan ? info[table].nack_status0_reg :
        info[table].intr_status0_reg;
    if (status_reg == INVALIDr) {
        return SOC_E_INTERNAL;
    }
    addr = soc_reg_addr(unit, status_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &reg_val));
    index = soc_reg_field_get(unit, status_reg, reg_val, ENTRY_IDXf);

    if (soc_reg_field_valid(unit, status_reg, SINGLE_BIT_ERRf)) {
        single_bit = soc_reg_field_get(unit, status_reg,
                                       reg_val, SINGLE_BIT_ERRf);
    }
    if (soc_reg_field_valid(unit, status_reg, DOUBLE_BIT_ERRf)) {
        double_bit = soc_reg_field_get(unit, status_reg,
                                       reg_val, DOUBLE_BIT_ERRf);
    }

    if (soc_reg_field_valid(unit, status_reg, ECC_ERRf)) {
        error = soc_reg_field_get(unit, status_reg, reg_val, ECC_ERRf);
    }

    multiple = soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);

    if (single_bit || double_bit || error) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d %s ECC error\n"),
                   unit, msg, index, double_bit ? "double-bit" : "single-bit"));
        _soc_enduro_mem_parity_info(unit, block,
                                    info[table].error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                           SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                           index, minfo);

        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple ECC errors\n"),
                       unit, msg));
        }
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
        spci.reg = INVALIDr;
        spci.mem = info[table].mem;
        spci.blk_type = block;
        spci.index = soc_enduro_mem_index_remap(unit, spci.mem, index);
        spci.double_bit = (double_bit == 0) ? 0 : 1;
        spci.parity_type = SOC_PARITY_TYPE_ECC;
        rv = soc_ser_correction(unit, &spci);
        if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               index, minfo);
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
_soc_enduro_process_dual_parity_error(int unit, int group,
                                        soc_port_t block_port, int table,
                                        int schan, char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg;
    uint32 addr, reg_val, minfo, bitmap = 0;
    int rv, index, bucket_index = 0, multiple = 0, ix, bits, size = 0;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    info = _soc_en_parity_group_info[group].info;

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
            if (MPLS_ENTRYm == info[table].mem) {
                multiple =
                    soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
                bitmap =
                    soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BMf);
                size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BMf);
            } else {
                multiple =
                    soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERR_0f);
                bitmap =
                    soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BM_0f);
                size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BM_0f);
            }
        } else if (soc_reg_field_valid(unit, status_reg, BUCKET_IDX_1f)) {
            bucket_index =
                    soc_reg_field_get(unit, status_reg, reg_val, BUCKET_IDX_1f);
            if (MPLS_ENTRYm == info[table].mem) {
                multiple =
                    soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERRf);
                bitmap =
                    soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BMf);
                size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BMf);
            } else {
                multiple =
                   soc_reg_field_get(unit, status_reg, reg_val, MULTIPLE_ERR_1f);
                bitmap =
                    soc_reg_field_get(unit, status_reg, reg_val, PARITY_ERR_BM_1f);
                size = soc_reg_field_length(unit, status_reg, PARITY_ERR_BM_1f);
            }
        }

        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s has multiple parity errors\n"),
                       unit, msg));
        }

        if (bitmap != 0) {
            for (bits = 0; bits < size; bits++) {
                if (bitmap & (1 << bits)) {
                    index =
                        bucket_index * size * 2 + bits + (ix * size);
                    if (info[table].mem == L2Xm) {
                        /* Check the valid index range before reporting the error.*/
                        if (index > soc_mem_index_max(unit, L2Xm)){
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "L2X entry parity error index"
                                                  " %d out of valid range !\n"),
                                      index));                                                                                                       
                            continue;
                        }
                    }
                    _soc_enduro_mem_parity_info(unit, block,
                                                info[table].error_field, &minfo);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                                       index, minfo);

                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s entry %d parity error\n"),
                               unit, msg, index));
                    
                    if (info[table].mem != INVALIDm) {
                        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                        spci.reg = INVALIDr;
                        spci.mem = info[table].mem;
                        spci.blk_type = block;
                        spci.index = soc_enduro_mem_index_remap(unit, spci.mem, index);
                        rv = soc_ser_correction(unit, &spci);
                        if (SOC_FAILURE(rv)) {
                            soc_event_generate(unit, 
                                SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                index, minfo);
                        }
                    }
                }
            }
        }

        /* Clear parity status */
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_enduro_parity_mmu_clear(int unit, soc_field_t status_field)
{
    uint32 rval;
    SOC_IF_ERROR_RETURN
        (READ_MEM_FAIL_INT_STATr(unit, &rval));
    soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &rval, status_field, 0);
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

STATIC int
_soc_enduro_process_mmu_parity_error(int unit, int group,
                                       soc_port_t block_port, int table,
                                       char *msg, soc_block_t block)
{
    _soc_parity_info_t *info;
    soc_reg_t status_reg, index_reg, group_reg;
    uint32 addr, reg_val, group_status, minfo;
    int index, rv;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

    info = _soc_en_parity_group_info[group].info;
    group_reg = _soc_en_parity_group_info[group].status_reg;
    addr = soc_reg_addr(unit, group_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &group_status));
    
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
    _soc_enduro_mem_parity_info(unit, block,
                                 info[table].error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                       index, minfo); 

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s entry %d parity error\n"),
               unit, msg, index));

    if (info[table].mem != INVALIDm) {
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
        spci.reg = INVALIDr;
        spci.mem = info[table].mem;
        spci.blk_type = block;
        spci.index = soc_enduro_mem_index_remap(unit, spci.mem, index);
        rv = soc_ser_correction(unit, &spci);
        if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               index, minfo);
        }
    }

    /* update soc stats for some cases */
    if (info[table].error_field == CCP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cpcrc++;
    } else if (info[table].error_field == CFAP_PAR_ERRf) {
        SOC_CONTROL(unit)->stat.err_cfap++;
    } 
    SOC_IF_ERROR_RETURN(_soc_enduro_parity_mmu_clear(unit, info[table].error_field));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_mmu_ipmc_parity_error(int unit, _soc_parity_info_t *info,
                                          soc_port_t block_port, soc_block_t block)
{
    static soc_mem_t ipmc_mems[] =
    {
        MMU_IPMC_GROUP_TBL0m,
        MMU_IPMC_GROUP_TBL1m,
        MMU_IPMC_GROUP_TBL2m,
        MMU_IPMC_GROUP_TBL3m
    };
    static soc_field_t ipmc_mem_fields[] =
    {
        IGPERR0f,
        IGPERR1f,
        IGPERR2f,
        IGPERR3f
    };
    int index, rv;
    soc_reg_t mem_id_reg, index_reg;
    uint32 addr, mem_bitmap, entry_index, status, minfo;
    _soc_ser_correct_info_t spci;
    
    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
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
     * MMU_IPMC_GROUP_TBL0m - MMU_IPMC_GROUP_TBL3m
     */
    index_reg = info->intr_status0_reg;
    addr = soc_reg_addr(unit, index_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &entry_index));
    mem_id_reg = info->intr_status1_reg;
    addr = soc_reg_addr(unit, mem_id_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &mem_bitmap));
    for (index = 0; index < COUNTOF(ipmc_mem_fields); index++) {
        if (!soc_reg_field_get(unit, mem_id_reg, mem_bitmap,
                               ipmc_mem_fields[index])) {
            continue;
        }
        _soc_enduro_mem_parity_info(unit, block,
                                    info->error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                           SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           entry_index, minfo);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d IPMC%d entry %d parity error\n"),
                              unit, index, entry_index));
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
        spci.reg = INVALIDr;
        spci.mem = ipmc_mems[index];
        spci.blk_type = SOC_BLK_MMU;
        spci.index = soc_enduro_mem_index_remap(unit, spci.mem, entry_index);
        rv = soc_ser_correction(unit, &spci);
        if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               entry_index, minfo);
        }
        /* Clear memory id register explicitly */
        soc_reg_field_set(unit, mem_id_reg, &mem_bitmap, ipmc_mem_fields[index], 0);
        addr = soc_reg_addr(unit, mem_id_reg, block_port, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, mem_bitmap));
        break;
    }
    SOC_IF_ERROR_RETURN(_soc_enduro_parity_mmu_clear(unit, TOQ0_IPMC_TBL_PAR_ERRf));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_mmu_e2efc_parity_error(int unit, _soc_parity_info_t *info,
                                          soc_port_t block_port, soc_block_t block)
{
    static soc_reg_t e2efc_regs[] = {
          INVALIDr,  
          E2EFC_CNT_SET_LIMITr,
          E2EFC_CNT_RESET_LIMITr,
          E2EFC_CNT_DISC_LIMITr
    };
    int rv;
    soc_reg_t err_reg;
    uint32 reg_val, val, status, entry_index, mem_id, minfo;
    _soc_ser_correct_info_t spci;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &status));
    if (!soc_reg_field_get(unit, MEM_FAIL_INT_STATr, status, E2EFC_PAR_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d E2EFC: parity hardware inconsistency\n"),
                              unit));
        return SOC_E_NONE;
    }

    err_reg = E2EFC_PARITYERRORPTRr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, err_reg, 
                                        block_port, 0, &reg_val));

    /* Disable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));
        
    mem_id = soc_reg_field_get(unit, err_reg, reg_val, INSTf);
    /* CHECK if the error is valid */
    if (mem_id == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d E2EFC: parity hardware inconsistency\n"),
                              unit));
        return SOC_E_NONE;
    }
    entry_index = soc_reg_field_get(unit, err_reg, reg_val, PTRf);

    _soc_enduro_mem_parity_info(unit, block,
                                info->error_field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                       entry_index, minfo);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d E2EFC entry %d parity error\n"),
                          unit, entry_index));
    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
    spci.reg = e2efc_regs[mem_id];
    spci.mem = INVALIDm;
    spci.blk_type = SOC_BLK_MMU;
    spci.index = entry_index;
    rv = soc_ser_correction(unit, &spci);
    if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               entry_index, minfo);
        }
    SOC_IF_ERROR_RETURN(_soc_enduro_parity_mmu_clear(unit, E2EFC_PAR_ERRf));
    /* Enable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_mmu_mtro_parity_error(int unit, _soc_parity_info_t *info,
                                          soc_port_t block_port, soc_block_t block)
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
    uint32 i, addr, status, val, minfo, index = 0;
    _soc_ser_correct_info_t spci;
    int rv, port = 0;
    
    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
    spci.mem = INVALIDm;
    spci.blk_type = SOC_BLK_MMU;
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &status));
    if (!soc_reg_field_get(unit, MEM_FAIL_INT_STATr, status, MTRO_PAR_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d MTRO: parity hardware inconsistency\n"),
                              unit));
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(READ_EGRSHAPEPARITYERRORPTRr(unit, &addr));
    
    /* Disable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));
        
    if (addr < 8) {
        /* port lb0 */
        port = 1;
        index = addr;
    } else if (addr >= 0x40 && addr <= 0x6f) {
        /* port ge0 ~ ge5 */
        port = ((addr - 0x40) / 0x8) + 0x2;
        index = (addr - 0x40) % 0x8;
    } else if (addr >= 0x80 && addr <= 0xaf) {
        /* port ge6 ~ ge11 */
        port = ((addr - 0x80) / 0x8) + 0x8;
        index = (addr - 0x80) % 0x8;
    } else if (addr >= 0xc0 && addr <= 0xef) {
        /* port ge12 ~ ge17 */
        port = ((addr - 0xc0) / 0x8) + 0xe;
        index = (addr - 0xc0) % 0x8;
    } else if (addr >= 0x100 && addr <= 0x12f) {
        /* port ge18 ~ ge23 */
        port = ((addr - 0x100) / 0x8) + 0x14;
        index = (addr - 0x100) % 0x8;
    } else if (addr >= 0x140 && addr <= 0x16f) {
        /* port hg0 ~ hg1 */
        port = ((addr - 0x140) / 0x18) + 0x1a;
        index = (addr - 0x140) % 0x18;
    } else if (addr >= 0x180 && addr <= 0x1af) {
        /* port hg2 ~ hg3 */
        port = ((addr - 0x180) / 0x18) + 0x1c;
        index = (addr - 0x180) % 0x18;
    } else if (addr >= 0x1c0 && addr <= 0x1c7) {
        /* port hg0 ~ hg3 */
        port = ((addr - 0x1c0) / 0x2) + 0x1a;
        index = (addr - 0x1c0) % 0x2 + 0x18;
    } else if (addr >= 0x200 && addr <= 0x22f) {
        port = REG_PORT_ANY;
        index = (addr - 0x200) % 0x30;
    }
    if (port == REG_PORT_ANY) {
        for (i = 0; i < COUNTOF(cpu_mtro_regs); i++) {
            spci.port = port;
            spci.index = index;
            spci.reg = cpu_mtro_regs[i];
            _soc_enduro_mem_parity_info(unit, block,
                                        info->error_field, &minfo);
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
    } else if (port >=1 && port <= 29) {
        for (i = 0; i < COUNTOF(mtro_regs); i++) {
            spci.port = port;
            spci.index = index;
            spci.reg = mtro_regs[i];
            _soc_enduro_mem_parity_info(unit, block,
                                info->error_field, &minfo);
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
    SOC_IF_ERROR_RETURN(_soc_enduro_parity_mmu_clear(unit, MTRO_PAR_ERRf));
    /* Enable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_mmu_wred_parity_error(int unit, _soc_parity_info_t *info,
                                          soc_port_t block_port, soc_block_t block)
{
    int rv;
    soc_mem_t mem;
    soc_reg_t mem_id_reg, index_reg;
    uint32 addr, mem_bitmap, entry_index, status, minfo;
    _soc_ser_correct_info_t spci;
    
    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &status));
    if (!soc_reg_field_get(unit, MEM_FAIL_INT_STATr, status, WRED_PAR_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d WRED: parity hardware inconsistency\n"),
                          unit));
        return SOC_E_NONE;
    }
    /*
     * status0 (WRED_PARITY_ERROR_INFOr) is index
     * status1 (WRED_PARITY_ERROR_BITMAPr) is table id
     */
    index_reg = info->intr_status0_reg;
    addr = soc_reg_addr(unit, index_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &entry_index));
    
    mem_id_reg = info->intr_status1_reg;
    addr = soc_reg_addr(unit, mem_id_reg, block_port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &mem_bitmap));
    
    if (mem_bitmap & 0x000003) {
        mem = MMU_WRED_CFG_CELLm;
    } else if (mem_bitmap & 0x00000c) {
        mem = MMU_WRED_THD_0_CELLm;
    } else if (mem_bitmap & 0x000030) {
        mem = MMU_WRED_THD_1_CELLm;
    } else if (mem_bitmap & 0x0000c0) {
        mem = MMU_WRED_CFG_PACKETm;
    } else if (mem_bitmap & 0x000300) {
        mem = MMU_WRED_THD_0_PACKETm;
    } else if (mem_bitmap & 0x000c00) {
        mem = MMU_WRED_THD_1_PACKETm;
    } else if (mem_bitmap & 0x003000) {
        mem = MMU_WRED_PORT_CFG_CELLm;
    } else if (mem_bitmap & 0x00c000) {
        mem = MMU_WRED_PORT_THD_0_CELLm;
    } else if (mem_bitmap & 0x030000) {
        mem = MMU_WRED_PORT_THD_1_CELLm;
    } else if (mem_bitmap & 0x0c0000) {
        mem = MMU_WRED_PORT_CFG_PACKETm;
    } else if (mem_bitmap & 0x300000) {
        mem = MMU_WRED_PORT_THD_0_PACKETm;
    } else if (mem_bitmap & 0xc00000) {
        mem = MMU_WRED_PORT_THD_1_PACKETm;
    } else {
        mem = INVALIDm;
    }
    if (mem != INVALIDm) {
        _soc_enduro_mem_parity_info(unit, block, info->error_field, &minfo);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                           SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 
                           entry_index, minfo);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d %s entry %d parity error\n"),
                              unit, SOC_MEM_NAME(unit, mem), entry_index));
        spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
        spci.reg = INVALIDr;
        spci.mem = mem;
        spci.blk_type = block;
        spci.index = soc_enduro_mem_index_remap(unit, spci.mem, entry_index);
        rv = soc_ser_correction(unit, &spci);
        if (SOC_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                               entry_index, minfo);
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d WRED: parity hardware inconsistency\n"),
                              unit));
    }
    SOC_IF_ERROR_RETURN(_soc_enduro_parity_mmu_clear(unit, WRED_PAR_ERRf));
    return SOC_E_NONE;
}

STATIC int
_soc_enduro_process_parity_error(int unit)
{
    int group, table;
    uint32 addr, cmic_status, group_status;
    uint32 group_enable, cpi_blk_bit, minfo;

    _soc_parity_info_t *info;
    soc_reg_t group_reg, reg;
    soc_port_t block_port;
    soc_block_t blk;
    char *msg;
    soc_enduro_oam_handler_t oam_handler_snapshot;

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CHIP_PARITY_INTR_STATUSr(unit,
                                                           &cmic_status));

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        group_reg = _soc_en_parity_group_info[group].status_reg;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d %s parity processing\n"),
                                unit, SOC_REG_NAME(unit, group_reg)));
        cpi_blk_bit = _soc_en_parity_group_info[group].cpi_bit;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
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
                                         info[table].error_field) &&
                    (info[table].mem != INVALIDm)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s has bad error field\n"),
                                          unit, SOC_MEM_NAME(unit, info[table].mem)));
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

                /* Handle different parity error reporting style */
                switch (info[table].type) {
                case _SOC_PARITY_INFO_TYPE_GENERIC:
                    _soc_enduro_mem_parity_info(unit, blk,
                                                info[table].error_field,
                                                &minfo);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                       0, minfo);

                    if (info[table].enable_field == START_BY_START_ERR_ENf) {
                        uint64 rr64;
                        uint32 msw;
                        uint32 lsw;
                        uint32 regval;

                        /* Clear the START_BY_START port bitmap */
                        SOC_IF_ERROR_RETURN
                            (READ_START_BY_START_ERRORr(unit, &rr64));
                        COMPILER_64_TO_32_HI(msw, rr64);
                        COMPILER_64_TO_32_LO(lsw, rr64);
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d START_BY_START_ERRROR"
                                              " = 0x%x%08x\n"),
                                   unit, msw, lsw));
                        COMPILER_64_ZERO(rr64);
                        SOC_IF_ERROR_RETURN
                            (WRITE_START_BY_START_ERRORr(unit, rr64));

                        /* Clear the START_BY_START error indication */
                        SOC_IF_ERROR_RETURN
                            (READ_MEM_FAIL_INT_STATr(unit, &regval));
                        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &regval,
                                          START_BY_START_ERRf, 0);
                        SOC_IF_ERROR_RETURN
                            (WRITE_MEM_FAIL_INT_STATr(unit, regval));
                    } else if (info[table].enable_field
                               == DEQ0_CELLCRC_ERR_ENf) {
                        uint32 regval;

                        /* Clear the DEQ0_CELLCRC error indication */
                        SOC_IF_ERROR_RETURN
                            (READ_MEM_FAIL_INT_STATr(unit, &regval));
                        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &regval,
                                          DEQ0_CELLCRC_ERRf, 0);
                        SOC_IF_ERROR_RETURN
                            (WRITE_MEM_FAIL_INT_STATr(unit, regval));
                    }

                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s asserted\n"),
                                          unit, msg));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_parity_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_ecc_error(unit,
                              group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_dual_parity_error(unit,
                                       group, block_port, table, FALSE, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMU_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_mmu_parity_error(unit,
                              group, block_port, table, msg, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUMTRO:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_mmu_mtro_parity_error(unit, 
                            &info[table], block_port, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUE2EFC:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_mmu_e2efc_parity_error(unit, 
                            &info[table], block_port, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUIPMC:
                	 SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_mmu_ipmc_parity_error(unit, 
                            &info[table], block_port, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_MMUWRED:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_mmu_wred_parity_error(unit, 
                            &info[table], block_port, blk));
                    break;
                case _SOC_PARITY_INFO_TYPE_OAM:
                    /* OAM isn't parity-related but shares an interrupt */

                    oam_handler_snapshot = en_oam_handler[unit];

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

            reg = _soc_en_parity_group_info[group].enable_reg;
            addr = soc_reg_addr(unit, reg, block_port, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &group_enable));
            /* Exclude non-parity status bits */
            group_status &= group_enable;
            group_enable &= ~group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
            group_enable |= group_status;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, group_enable));
        } /* loop through each block for the group */
    } /* loop through each group */

    return SOC_E_NONE;
}


void
soc_enduro_parity_error(void *unit_vp, void *d1, void *d2,
                         void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    _soc_enduro_process_parity_error(unit);
    soc_intr_enable(unit, IRQ_MEM_FAIL);
}

STATIC int
_soc_enduro_mem_nack_error_process(int unit, _enduro_ser_nack_reg_mem_t nack_reg_mem, 
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
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (block != SOC_BLOCK_INFO(unit, blk).schan) {
                continue;
            }
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            /* loop through each table in the group */
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (info[table].mem != nack_reg_mem.mem) {
                    continue;
                }
                if (info[table].nack_status0_reg == INVALIDr) {
                    return SOC_E_NONE;
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
                        (_soc_enduro_process_single_parity_error(unit,
                              group, block_port, table, TRUE, msg, block));
                    break;
                case _SOC_PARITY_INFO_TYPE_SINGLE_ECC:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_single_ecc_error(unit,
                              group, block_port, table, TRUE, msg, block));
                    break;
                case _SOC_PARITY_INFO_TYPE_DUAL_PARITY:
                    SOC_IF_ERROR_RETURN
                        (_soc_enduro_process_dual_parity_error(unit,
                              group, block_port, table, TRUE, msg, block));
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
soc_enduro_mem_nack(void *unit_vp, void *addr_vp, void *blk_vp, 
                     void *d3, void *d4)
{
    soc_mem_t mem = INVALIDm;
    int reg_mem = PTR_TO_INT(d3);
    int rv, unit = PTR_TO_INT(unit_vp);
    uint32 address = PTR_TO_INT(addr_vp);
    uint32 block = PTR_TO_INT(blk_vp);
    uint32 offset = 0, min_addr = 0, max_addr = 0;
    soc_regaddrinfo_t ainfo;
    _enduro_ser_nack_reg_mem_t nack_reg_mem;
    nack_reg_mem.mem = INVALIDm;
    nack_reg_mem.reg = INVALIDr;

    if (reg_mem == _SOC_ENDURO_SER_REG) {
        if (address) {
            soc_regaddrinfo_get(unit, &ainfo, address);
            nack_reg_mem.reg = ainfo.reg;
        }
    } else {
        offset = address & ~0xC0f00000; /* Strip block ID */
        mem = soc_addr_to_mem(unit, address, &block);
        if (mem == INVALIDm) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d mem decode failed, "
                                  "SCHAN NACK analysis failure\n"), unit));
            return;
        }
        
        nack_reg_mem.mem = mem;

        min_addr = max_addr = SOC_MEM_INFO(unit, mem).base;
        min_addr += SOC_MEM_INFO(unit, mem).index_min;
        max_addr += SOC_MEM_INFO(unit, mem).index_max;
    }

    if ((rv = _soc_enduro_mem_nack_error_process(unit, nack_reg_mem, (soc_block_t)block,
        reg_mem)) < 0) {
        if (reg_mem == _SOC_ENDURO_SER_REG) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d REG SCHAN NACK analysis failure\n"),
                       unit));
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s entry %d SCHAN NACK analysis failure\n"),
                       unit, SOC_MEM_NAME(unit, mem),
                       min_addr - offset));
        }
    }
    if (reg_mem == _SOC_ENDURO_SER_REG) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d REG SCHAN NACK analysis\n"),
                       unit));
    } else {
        LOG_INFO(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "unit %d %s entry %d SCHAN NACK analysis\n"),
                  unit, SOC_MEM_NAME(unit, mem),
                  min_addr - offset));
    }
}

STATIC int
_soc_enduro_mem_ecc_force(int unit, soc_port_t block_port,
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
_soc_enduro_mem_nack_error_test(int unit)
{
    int group, table, index, rv;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS];
    uint32 parity;
    _soc_parity_info_t *info;
    soc_mem_t mem;
    int	 index_min, index_max, index_range;
    soc_port_t block_port;
    soc_block_t blk;
    soc_field_t par_ecc_field;
    _enduro_ser_nack_reg_mem_t nack_reg_mem;

    /* loop through each group */
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
                       _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
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
                            (_soc_enduro_mem_ecc_force(unit, block_port,
                                                         &(info[table])));
                    }
                    par_ecc_field = ECCf;
                    break;
                default:
                    /* coverity[assigned_value] */
                    par_ecc_field = INVALIDf;
                    continue;
                }

                if (!soc_mem_field_valid(unit, mem, par_ecc_field)) {
#if !defined(SOC_NO_NAMES)
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s doesn't contain %s\n"),
                               unit, SOC_MEM_NAME(unit, mem),
                               soc_fieldnames[par_ecc_field]));
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
                        nack_reg_mem.mem = mem;
                        if ((rv = _soc_enduro_mem_nack_error_process(unit,
                                                                       nack_reg_mem, blk, _SOC_ENDURO_SER_MEM)) < 0) {
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

    return SOC_E_NONE;
}

STATIC int
soc_enduro_pipe_mem_clear(int unit)
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

    SOC_IF_ERROR_RETURN
    (soc_mem_clear(unit, XQPORT_EHG_RX_TUNNEL_DATAm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN
    (soc_mem_clear(unit, XQPORT_EHG_RX_TUNNEL_MASKm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN
    (soc_mem_clear(unit, XQPORT_EHG_TX_TUNNEL_DATAm, COPYNO_ALL, TRUE));

    /* LMEP and LMEP_1 table is not properly handled by reset control */
    SOC_IF_ERROR_RETURN(soc_mem_clear(unit, LMEPm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN(soc_mem_clear(unit, LMEP_1m, COPYNO_ALL, TRUE));

    return SOC_E_NONE;
}

/* SER processing for TCAMs */
STATIC _soc_ser_parity_info_t _soc_enduro_ser_parity_info[] = {
    { EFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_0r, CMIC_SER_END_ADDR_0r,
      CMIC_SER_MEM_ADDR_0r, CMIC_SER_PARITY_MODE_SELr,
      RANGE_0_PARITY_BITSf, -1, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_XY_READ},
    { EGR_ERSPANm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_1r, CMIC_SER_END_ADDR_1r,
      CMIC_SER_MEM_ADDR_1r, CMIC_SER_PARITY_MODE_SELr,
      RANGE_1_PARITY_BITSf, -1, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_XY_READ},
    { FP_SLICE_MAPm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_2r, CMIC_SER_END_ADDR_2r,
      CMIC_SER_MEM_ADDR_2r, CMIC_SER_PARITY_MODE_SELr,
      RANGE_2_PARITY_BITSf, -1, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_XY_READ},
    { FP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_3r, CMIC_SER_END_ADDR_3r,
      CMIC_SER_MEM_ADDR_3r, CMIC_SER_PARITY_MODE_SELr,
      RANGE_3_PARITY_BITSf, -1, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_XY_READ},
    { MPLS_STATION_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_4r, CMIC_SER_END_ADDR_4r,
      CMIC_SER_MEM_ADDR_4r, CMIC_SER_PARITY_MODE_SELr,
      RANGE_4_PARITY_BITSf, -1, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_XY_READ},
    { VFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_5r, CMIC_SER_END_ADDR_5r,
      CMIC_SER_MEM_ADDR_5r, CMIC_SER_PARITY_MODE_SELr,
      RANGE_5_PARITY_BITSf, -1, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_XY_READ},
    { INVALIDm, _SOC_SER_PARITY_MODE_NUM},
};

#define SOC_ENDURO_SER_MEM_AVAILABLE       (2048 * 32)  /* bits */

STATIC int
_soc_enduro_ser_init(int unit)
{
    return soc_ser_init(unit, _soc_enduro_ser_parity_info,
            SOC_ENDURO_SER_MEM_AVAILABLE);
}

void
soc_enduro_ser_fail(int unit)

{
    soc_process_ser_parity_error(unit, _soc_enduro_ser_parity_info,
            SOC_SWITCH_EVENT_DATA_ERROR_PARITY);
    return;
}

uint32 tdm[84] = {2,10,18,26,27,28,29,
                  3,11,19,26,27,28,29,
                  4,12,20,26,27,28,29,
                  5,13,21,26,27,28,29,
                  6,14,22,26,27,28,29,
                  7,15,23,26,27,28,29,
                  8,16,24,26,27,28,29,
                  9,17,25,26,27,28,29,
                  1,30,30,26,27,28,29,
                  1,30,30,26,27,28,29,
                  1,30,30,26,27,28,29,
                  0,30,30,26,27,28,29};

uint32 tdm_56333_lp[56] = {2,10,1,30,30,30,30,
                  3,11,1,30,30,30,30,
                  4,12,30,30,30,30,30,
                  5,13,30,30,30,30,30,
                  6,14,0,30,30,30,30,
                  7,15,30,30,30,30,30,
                  8,16,30,30,30,30,30,
                  9,17,30,30,30,30,30};

uint32 tdm_56333_ge_linerate[84] = {2,10,18,26,27,28,29,
                  3,11,19,26,27,28,29,
                  4,12,20,26,27,28,29,
                  5,13,21,26,27,28,29,
                  6,14,22,26,27,28,29,
                  7,15,23,30,30,0,30,
                  8,16,24,30,30,0,1,
                  9,17,25,30,30,30,1,
                  30,30,30,30,30,30,30,
                  30,30,30,30,30,30,30,
                  30,30,30,30,30,30,30,
                  30,30,30,30,30,0,30};

int
soc_enduro_misc_init(int unit)
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
    arb_tdm_table_entry_t arb_tdm;
    uint64              multipass;
    uint16              dev_id;
    uint8               rev_id;
    uint32              bm;
    soc_field_t         fields[3];
    uint32              values[3];
    soc_pbmp_t          pbmp_ge_linerate;

    /* set the mode for XQPORT blocks */
    PBMP_XQ_ITER(unit, port) {
        mode = 1; /* gport-mode */
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
            soc_port_cmap_set(unit, port, SOC_CTR_TYPE_XE);
            mode = 2;  /* xport-mode */
        }
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XQPORT_MODE_REGr, &rval, 
                                   XQPORT_MODE_BITSf, mode);
        SOC_IF_ERROR_RETURN(WRITE_XQPORT_MODE_REGr(unit, port, rval));
    }

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    /* Program for Different TDM mode */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if((dev_id == BCM56333_DEVICE_ID) || (dev_id == BCM56331_DEVICE_ID)){
        SOC_PBMP_CLEAR(pbmp_ge_linerate);
        pbmp_ge_linerate = soc_property_get_pbmp_default(unit, 
                            spn_BCM56333_PBMP_GE_LINERATE, pbmp_ge_linerate);
        if (SOC_PBMP_NOT_NULL(pbmp_ge_linerate)) {
            /* Use config variable to decide which 12 ports out of 24 
               get guarantee line rate  bandwidth on 56333 */
            i = 0;
            SOC_PBMP_ITER(pbmp_ge_linerate, port) {
                if ((i % 4) == 0) {
                    tdm_56333_ge_linerate[56 + (i / 4)] = port;
                } else if ((i % 4) == 1) {
                    tdm_56333_ge_linerate[63 + (i / 4)] = port;
                } else if ((i % 4) == 2) {
                    tdm_56333_ge_linerate[70 + (i / 4)] = port;
                } else if ((i % 4) == 3) {
                    tdm_56333_ge_linerate[77 + (i / 4)] = port;
                } 
                i++;
                if (i == 12) {
                    break;
                }
            }
            tdm_size = 84;
            arr = tdm_56333_ge_linerate;
        } else {
            if (dev_id == BCM56333_DEVICE_ID) {
                tdm_size = 56;
                arr = tdm_56333_lp;        	
            } else {
                tdm_size = 84;
                arr = tdm;
            }
        }
    } else if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)) { 
        tdm_size = 56;
        arr = tdm_56333_lp;
    } else {
        tdm_size = 84;
        arr = tdm;
    }
    for (i = 0; i < tdm_size; i++) {
        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf, 
                                        arr[i]);
        soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, 
                                       arr[i]); 
        /* Cache the CPU slot entry */
        if (IS_CPU_PORT(unit, arr[i])) {
            sal_memcpy(&(SOC_CONTROL(unit)->iarb_tdm), &iarb_tdm, 
                       sizeof(iarb_tdm));
            SOC_CONTROL(unit)->iarb_tdm_idx = i;
        }
        if (i == tdm_size - 1) {
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                  &iarb_tdm));
        SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                 &arb_tdm));
    }
    rval = 0;
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, 
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_enduro_pipe_mem_clear(unit));
    }

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));

    /* Turn on ingress/egress/mmu parity */
    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        memset(&ser_enduro_test_fun, 0, sizeof(soc_ser_test_functions_t));
        ser_enduro_test_fun.inject_error_f = &soc_enduro_ser_inject_error;
        ser_enduro_test_fun.injection_support = &soc_enduro_ser_error_injection_support;
        soc_ser_test_functions_register(unit, &ser_enduro_test_fun);
#endif /*defined(SER_TR_TEST_SUPPORT*/
        _soc_enduro_parity_enable(unit, TRUE);
        if (soc_feature(unit, soc_feature_ser_parity)) {
            SOC_IF_ERROR_RETURN(_soc_enduro_ser_init(unit));
#ifdef INCLUDE_MEM_SCAN
            soc_mem_scan_ser_list_register(unit, FALSE,
                                           _soc_enduro_ser_parity_info);
#endif /* INCLUDE_MEM_SCAN */
        }

        sal_memset(&_enduro_ser_functions, 0, sizeof(soc_ser_functions_t));
        _enduro_ser_functions._soc_ser_fail_f = &soc_enduro_ser_fail;
        _enduro_ser_functions._soc_ser_parity_error_intr_f = &soc_enduro_parity_error;
        _enduro_ser_functions._soc_ser_mem_nack_f = &soc_enduro_mem_nack;
        soc_ser_function_register(unit, &_enduro_ser_functions);
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
    rval= 0;
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
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

    /* Match the Internal MDC freq with above for External MDC */
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, 40);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    /*
     * Set reference clock (based on 200MHz core clock)
     * to be 200MHz * (1/8) = 25MHz
     */
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVISORf,  8);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVIDENDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_I2Cr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval, DIVISORf,  8);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval, DIVIDENDf, 1);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_STDMAr(unit, rval));

    rval = 0x01; /* 125KHz I2C sampling rate based on 5Mhz reference clock */
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
                              SOC_EN_BROADCAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_EN_L2_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_EN_IP_MULTICAST_RANGE_DEFAULT)));

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

    /* Initialize the multipass loopback bitmap to the loopback port */
    COMPILER_64_ZERO(multipass);
    bm=0x0;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
                bm = bm | (1 << port); 
        }
    }
    soc_reg64_field32_set(unit, MULTIPASS_LOOPBACK_BITMAP_64r, &multipass, 
                        BITMAPf, bm);
    SOC_IF_ERROR_RETURN(WRITE_MULTIPASS_LOOPBACK_BITMAP_64r(unit, multipass));

    /* Apply delay so LEDUP can capture correct status. */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP_CTRLr(unit, &rval));
    soc_reg_field_set(unit,CMIC_LEDUP_CTRLr,
                     &rval, LEDUP_SCAN_START_DELAYf, 0x5);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP_CTRLr(unit, rval));

    /* Enable shaping mode. */
    if (soc_reg_field_valid(unit, EGR_Q_BEGINr, DWRR_OR_SHAPINGf)) {
        SOC_IF_ERROR_RETURN(READ_EGR_Q_BEGINr(unit, &rval));
        soc_reg_field_set(unit,EGR_Q_BEGINr, &rval, DWRR_OR_SHAPINGf, 0x1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_Q_BEGINr(unit, rval));
    }

    /* Cache LMEP/LMEP_1 table */
    SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, LMEPm, MEM_BLOCK_ALL, TRUE));
    SOC_IF_ERROR_RETURN(soc_mem_cache_set(unit, LMEP_1m, MEM_BLOCK_ALL, TRUE));

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
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

#define EN_MMU_IN_PG_MIN_CELLS      12
#define EN_MMU_IN_PG_MIN_PACKETS      12

#define TR_MMU_IN_PORT_MIN_CELLS       72
#define TR_MMU_IN_PORT_MIN_PKTS        1
#define DAGGER_MMU_IN_PORT_MIN_PKTS    72
#define TR_MMU_PG_HDRM_LIMIT_CELLS     36
#define TR_MMU_PG_HDRM_LIMIT_PKTS      36
#define TR_MMU_PG_RESET_OFFSET_CELLS   24
#define TR_MMU_PG_RESET_OFFSET_PKTS    1

#define TR_MMU_OUT_PORT_MIN_CELLS      12
#define TR_MMU_OUT_PORT_MIN_PKTS       1
#define DAGGER_MMU_OUT_PORT_MIN_PKTS   4
#define TR_MMU_OUT_RESET_OFFSET_CELLS  24
#define TR_MMU_OUT_RESET_OFFSET_PKTS   2
#define TR_MMU_SOP_POLICY              0
#define TR_MMU_MOP_POLICY              7
int
soc_enduro_mmu_init(int unit)
{
    uint16              dev_id;
    uint8               rev_id;
    uint32              rval, tm;    
    uint32              rval0, rval1, cell_rval, pkt_rval;
    int                 port;
    int                 total_cells, total_pkts;
    int                 in_reserved_cells, in_reserved_pkts;
    int                 out_reserved_cells, out_reserved_pkts;
    int                 out_shared_cells, out_shared_pkts;
    int                 idx, count;
    soc_pbmp_t          pbmp_8pg;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Total number of cells */
    if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)) {
        total_cells = 8 * 1024; /* 8K cells */
    } else {
        total_cells = 16 * 1024; /* 16K cells */
    }

    /* Total number of packet pointers */
    total_pkts = 6 * 1024; /* 6K packet pointers */

    /* Ports with 8PG (1PG for other ports) */
    SOC_PBMP_CLEAR(pbmp_8pg);
    SOC_PBMP_WORD_SET(pbmp_8pg, 0, 0x3c000000); /* 26-29 */
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
    in_reserved_cells = (NUM_PORT(unit) + 1) * TR_MMU_IN_PORT_MIN_CELLS +
        NUM_PORT(unit) * TR_MMU_PG_HDRM_LIMIT_CELLS +
        (NUM_PORT(unit) + 1) * TR_MMU_ETH_FRAME_CELLS;
    out_reserved_cells =
        (NUM_PORT(unit) * TR_MMU_NUM_COS + SOC_INFO(unit).num_cpu_cosq) *
        TR_MMU_OUT_PORT_MIN_CELLS;
    if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)) {
        in_reserved_pkts = (NUM_PORT(unit) + 1) * DAGGER_MMU_IN_PORT_MIN_PKTS +
            NUM_PORT(unit) * TR_MMU_PG_HDRM_LIMIT_PKTS;
        out_reserved_pkts =
            (NUM_PORT(unit) * TR_MMU_NUM_COS + SOC_INFO(unit).num_cpu_cosq) *
            DAGGER_MMU_OUT_PORT_MIN_PKTS;
    } else {
        in_reserved_pkts = (NUM_PORT(unit) + 1) * TR_MMU_IN_PORT_MIN_PKTS +
            NUM_PORT(unit) * TR_MMU_PG_HDRM_LIMIT_PKTS;
        out_reserved_pkts =
            (NUM_PORT(unit) * TR_MMU_NUM_COS + SOC_INFO(unit).num_cpu_cosq) *
            TR_MMU_OUT_PORT_MIN_PKTS;
    }

    /*
     * Input PGs threshold
     */
    cell_rval = 0;
    soc_reg_field_set(unit, PG_MIN_CELLr, &cell_rval, PG_MINf,
                      EN_MMU_IN_PG_MIN_CELLS);
    pkt_rval = 0;
    soc_reg_field_set(unit, PG_MIN_PACKETr, &pkt_rval, PG_MINf,
                      EN_MMU_IN_PG_MIN_PACKETS);
    PBMP_PORT_ITER(unit, port) {
        idx = SOC_PBMP_MEMBER(pbmp_8pg, port) ? TR_MMU_NUM_PG - 1 : 0;
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit, port, idx,
                                                      cell_rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_PACKETr(unit, port, idx,
                                                        pkt_rval));
    }

    /*
     * Input ports threshold
     */
    /* Reserved space: Input port per-port minimum */
    cell_rval = 0;
    soc_reg_field_set(unit, PORT_MIN_CELLr, &cell_rval, PORT_MINf,
                      TR_MMU_IN_PORT_MIN_CELLS);
    pkt_rval = 0;
    if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)) {
        soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval, PORT_MINf,
                          DAGGER_MMU_IN_PORT_MIN_PKTS);
    } else {
        soc_reg_field_set(unit, PORT_MIN_PACKETr, &pkt_rval, PORT_MINf,
                          TR_MMU_IN_PORT_MIN_PKTS);
    }
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
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &cell_rval, 
                  GLOBAL_HDRM_LIMITf, 
                  (NUM_PORT(unit) + 1) * TR_MMU_ETH_FRAME_CELLS);
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
    if ((dev_id == BCM56230_DEVICE_ID) || (dev_id == BCM56231_DEVICE_ID)) {
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, 
                          Q_MIN_PACKETf, DAGGER_MMU_OUT_PORT_MIN_PKTS);
    } else {
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_PACKETr, &pkt_rval, 
                          Q_MIN_PACKETf, TR_MMU_OUT_PORT_MIN_PKTS);
    }
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

    /* Output port per-device shared  */
    cell_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLr, &cell_rval,
                      OP_BUFFER_SHARED_LIMIT_RESUME_CELLf, out_shared_cells * 85 / 100);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLr(unit, cell_rval));
    pkt_rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_PACKETr, &pkt_rval,
                      OP_BUFFER_SHARED_LIMIT_RESUME_PACKETf, out_shared_pkts * 85 / 100);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_PACKETr(unit, pkt_rval));

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

    /* Apply TM setting on MMU_CELLLINK */
    rval = 0x20;
    SOC_IF_ERROR_RETURN(WRITE_CELLLINKMEMDEBUGr(unit, rval));

    /* Apply TM=0x10 setting for all CAMs */
    rval = 0;
    tm=0x10;
    soc_reg_field_set(unit, SW2_RAM_CONTROL_4r, &rval,
                      CPU_COS_MAP_TCAM_TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_SW2_RAM_CONTROL_4r_REG32(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, EFP_RAM_CONTROLr, &rval,
                      EFP_CAM_TM_7_THRU_0f, tm);
    SOC_IF_ERROR_RETURN(WRITE_EFP_RAM_CONTROLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r, &rval,
                      ALL_TCAMS_TM_7_0f, tm);
    soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r, &rval,
                      ALL_GLOBAL_MASK_TCAMS_TM_7_0f, tm);
    SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_TM_7_THRU_0r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, VLAN_SUBNET_CAM_DBGCTRLr, &rval,
                      TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_VLAN_SUBNET_CAM_DBGCTRLr_REG32(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, L2_USER_ENTRY_CAM_DBGCTRLr, &rval,
                      TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_L2_USER_ENTRY_CAM_DBGCTRLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_128_CAM_DBGCTRLr, &rval,
                      CAM0_TMf, tm);
    soc_reg_field_set(unit, L3_DEFIP_128_CAM_DBGCTRLr, &rval,
                      CAM1_TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_128_CAM_DBGCTRLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval,
                      CAM0_TMf, tm);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval,
                      CAM1_TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL0r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL1r, &rval,
                      CAM2_TMf, tm);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL1r, &rval,
                      CAM3_TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL1r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL2r, &rval,
                      CAM4_TMf, tm);
    soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL2r, &rval,
                      CAM5_TMf, tm);    
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL2r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, L3_TUNNEL_CAM_DBGCTRLr, &rval,
                      TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_L3_TUNNEL_CAM_DBGCTRLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, MPLS_STATION_CAM_DBGCTRLr, &rval,
                      CAM0_TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_MPLS_STATION_CAM_DBGCTRLr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, VFP_CAM_CONTROL_TM_7_THRU_0r, &rval,
                      TMf, tm);
    SOC_IF_ERROR_RETURN(WRITE_VFP_CAM_CONTROL_TM_7_THRU_0r(unit, rval));

    /* Input port enable */
    rval=0;
    soc_reg_field_set(unit, INPUT_PORT_RX_ENABLEr, &rval, 
                          INPUT_PORT_RX_ENABLEf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLEr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_OUTPUT_PORT_RX_ENABLEr(unit, rval));
    return SOC_E_NONE;
}

int
soc_enduro_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_enduro_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_enduro_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

void soc_enduro_oam_handler_register(int unit, soc_enduro_oam_handler_t handler)
{
    en_oam_handler[unit] = handler;
    soc_intr_enable(unit, IRQ_MEM_FAIL);
}

void
soc_enduro_mem_config(int unit)
{
    uint16              dev_id;
    uint8               rev_id;
    soc_persist_t       *sop;

    sop = SOC_PERSIST(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (dev_id) {
    case BCM56320_DEVICE_ID:
        sop->memState[MPLS_ENTRYm].index_max = 0;
        sop->memState[OAM_LM_COUNTERSm].index_max = 0;
        sop->memState[EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm].index_max = 0;
        break;
    case BCM56321_DEVICE_ID:
        sop->memState[MPLS_ENTRYm].index_max = 0;
        sop->memState[OAM_LM_COUNTERSm].index_max = 0;
        sop->memState[EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm].index_max = 0;
        sop->memState[L2Xm].index_max = 16383;
        sop->memState[L2_ENTRY_ONLYm].index_max = 16383;
        sop->memState[L2_HITDA_ONLYm].index_max = 2047;
        sop->memState[L2_HITSA_ONLYm].index_max = 2047;
        sop->memState[L3_DEFIPm].index_max = 511;
        sop->memState[L3_DEFIP_ONLYm].index_max = 511;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 511;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 511;
        sop->memState[L3_TUNNELm].index_max = 127;
        break;
    case BCM56230_DEVICE_ID:
    case BCM56231_DEVICE_ID:
        /* L2 MAC Address : 16K */
        sop->memState[L2Xm].index_max = 16383;
        sop->memState[L2_ENTRY_ONLYm].index_max = 16383;
        sop->memState[L2_HITDA_ONLYm].index_max = 2047;
        sop->memState[L2_HITSA_ONLYm].index_max = 2047;

        /* MPLS Labels : 1K */
        sop->memState[EGR_IP_TUNNELm].index_max = 255;
        sop->memState[EGR_IP_TUNNEL_IPV6m].index_max = 127;
        sop->memState[EGR_IP_TUNNEL_MPLSm].index_max = 255;
        sop->memState[MPLS_ENTRYm].index_max = 1023;
        sop->memState[EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm].index_max = 1023;

        /* VPLS VFI : 1K */
        sop->memState[VFIm].index_max = 1023;
        sop->memState[VFI_1m].index_max = 1023;
        sop->memState[VLAN_OR_VFI_MAC_COUNTm].index_max = 5119;
        sop->memState[VLAN_OR_VFI_MAC_LIMITm].index_max = 5119;

        /* Virtual Ports : 4K */
        sop->memState[SOURCE_VPm].index_max = 4095;
        sop->memState[SVP_DISABLE_VLAN_CHECKS_TABm].index_max = 4095;
        sop->memState[ING_DVP_TABLEm].index_max = 4095;

        /* IPMc Groups : 512 */
        sop->memState[EGR_IPMCm].index_max = 511;
        sop->memState[L3_IPMCm].index_max = 511;
        sop->memState[MMU_IPMC_GROUP_TBL0m].index_max = 511;
        sop->memState[MMU_IPMC_GROUP_TBL1m].index_max = 511;
        sop->memState[MMU_IPMC_GROUP_TBL2m].index_max = 511;
        sop->memState[MMU_IPMC_GROUP_TBL3m].index_max = 511;
        sop->memState[L3_MTU_VALUESm].index_max = 8703;

        /* IPv4 LPM : 4K */
        sop->memState[L3_DEFIPm].index_max = 2047;
        sop->memState[L3_DEFIP_ONLYm].index_max = 2047;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 2047;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 2047;
        SOC_CONTROL(unit)->l3_defip_index_remap = 2048;

        /* L3 Next Hop : 2K */
        sop->memState[EGR_L3_NEXT_HOPm].index_max = 2047;
        sop->memState[ING_L3_NEXT_HOPm].index_max = 2047;
        sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 2047;        
        break;
    default:
        break;
    }
    return;
}

#define _ENDURO_INDEX_1K_ENTRIES  1023
/* Map logical (always starts from 0 and contiguous) index to physical index
   which can have a starting offset and/or holes.
   Input  : logical index
   Returns: physical index */
int
soc_enduro_l3_defip_index_map(int unit, soc_mem_t mem, int index)
{
    if (SOC_CONTROL(unit)->l3_defip_index_remap == 0) {
        return index;
    }

    if (index > _ENDURO_INDEX_1K_ENTRIES) {
        return index + SOC_CONTROL(unit)->l3_defip_index_remap;
    }
 
    return index;
}

/* Map physical (always starts from 0 and contiguous) index to logic index
   which can have a starting offset and/or holes.
   Input  : physical index
   Returns: logical index */
int
soc_enduro_l3_defip_index_remap(int unit, soc_mem_t mem, int index)
{
    if (SOC_CONTROL(unit)->l3_defip_index_remap == 0) {
        return index;
    }

    if (SOC_CONTROL(unit)->l3_defip_urpf) { /* URPF mode */
        int tmp_index;

        tmp_index = index - SOC_CONTROL(unit)->l3_defip_index_remap;
        if (tmp_index > _ENDURO_INDEX_1K_ENTRIES) {
            return tmp_index;
        } 
    }
 
    return index;
}

int
soc_enduro_mem_index_remap(int unit, soc_mem_t mem, int index)
{
	switch (mem) {
	    case L3_DEFIPm:
	    case L3_DEFIP_ONLYm:
	    case L3_DEFIP_DATA_ONLYm:
	    case L3_DEFIP_HIT_ONLYm:
		return soc_enduro_l3_defip_index_remap(unit, mem, index);
	default:
		return index;
	}
}

#if defined(SER_TR_TEST_SUPPORT)
/*
 * Function:
 *      soc_enduro_ser_inject_error
 * Purpose:
 *       Injects an error into a single enduro memory
 * Parameters:
 *      unit               - (IN) Device Number
 *      mem                - (IN) The memory to test
 *      pipeTarget  - (IN) The pipe (x/y) to use when injecting the error
 *      block - (IN) The index into which block will be injected.
 *      index  - (IN) The index into which the error will be injected.
 */
int soc_enduro_ser_inject_error(int unit, uint32 flags, soc_mem_t mem,
                                int pipeTarget, int block, int index)
{
    int group, table;
    _soc_parity_info_t *info;
    soc_mem_t memTable;
    soc_port_t block_port;
    soc_block_t blk;
    ser_test_data_t test_data;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];

    /*H/W memory Test*/
    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;

        SOC_BLOCK_ITER(unit, blk,
            _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }

            for (table = 0; info[table].error_field != INVALIDf; table++) {
                memTable = info[table].mem;
                if (memTable == INVALIDm) {
                    continue;
                }
         
                if (memTable == mem) {
                    if((blk == block) || (block == MEM_BLOCK_ANY)) {
                        /*Inject error*/
                        test_data.mem = mem;
                        test_data.tcam_parity_bit = -1;
                        if (_soc_en_parity_group_info[group].blocktype 
                                    == SOC_BLK_MMU) {
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
                        /*Disable parity*/
                        SOC_IF_ERROR_RETURN(_ser_test_parity_control(
                                unit, &test_data, 0));
                        /*Read the memory for successful injection*/
                        SOC_IF_ERROR_RETURN(ser_test_mem_read(
                                    unit, 0, &test_data));
                        /*Inject error*/
                        SOC_IF_ERROR_RETURN(soc_ser_test_inject_full(
                                unit, flags, &test_data));
                        /*Enable parity*/
                        SOC_IF_ERROR_RETURN(_ser_test_parity_control(
                                unit, &test_data, 1));
                    }
                }
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_enduro_ser_error_injection_support
 * Purpose:
 *      Checks if a memory is supported by error injection interface
 *
 * Parameters:
 *      unit        - (IN) Device Number
 *      memory      - (IN) Test data required for SER test
 *      pipe        - (IN) How many indices to test for each memory
 *
 * Returns:
 *  SOC_E_NONE if memory / reg is supported, SOC_E_UNAVAIL if unsupported
 */
soc_error_t
soc_enduro_ser_error_injection_support (int unit, soc_mem_t mem, int pipe)
{
    int rv = SOC_E_UNAVAIL;
    _soc_parity_info_t *info = NULL;
    int group, table;
    soc_port_t block_port;
    soc_block_t blk;

    for (group = 0; _soc_en_parity_group_info[group].cpi_bit; group++) {
        info = _soc_en_parity_group_info[group].info;
        SOC_BLOCK_ITER(unit, blk,
            _soc_en_parity_group_info[group].blocktype) {
            if (_soc_enduro_parity_block_port(unit, blk, &block_port) < 0) {
                continue;
            }
            for (table = 0; info[table].error_field != INVALIDf; table++) {
                if (info[table].mem != mem) {
                    continue;
                }
                return SOC_E_NONE;
            }
        }
    }
    return rv;
}

#endif /* SER_TR_TEST_SUPPORT */
#endif /* BCM_ENDURO_SUPPORT */
