/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * Purpose:    Implements application interrupt lists for JERICHO.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>

#include <soc/dpp/QAX/qax_intr.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>

/*************
 * DEFINES   *
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_APPL_INTR

/*************
 * TYPE DEFS *
 */
qax_interrupt_type qax_int_disable_on_init[] = {
    INVALIDr
};



qax_interrupt_type qax_int_active_on_init[] = {
/*
 * List of interrupts that are vectors pointing to other interrupt registers
 */
    QAX_INT_MMU_ERROR_ECC,
    QAX_INT_IRE_ERROR_ECC,
    QAX_INT_IRE_EXTERNAL_IF_ERROR,
    QAX_INT_NBIH_ERROR_ECC,
    QAX_INT_NBIH_NBI_PKT_DROP_COUNTERS_0_75P_INTERRUPT,
    QAX_INT_NBIH_LINK_STATUS_CHANGE_INT,
    QAX_INT_ILKN_PML_ERROR_ECC,
    QAX_INT_OLP_ERROR_ECC,
    QAX_INT_IMP_ERROR_ECC,
    QAX_INT_KAPS_ERROR_ECC,
    QAX_INT_KAPS_KAPS_ERROR,
    QAX_INT_IPS_ERROR_ECC,
    QAX_INT_IHP_ERROR_ECC,
    QAX_INT_FDA_ERROR_ECC,
    QAX_INT_SPB_ERROR_ECC,
    QAX_INT_SPB_ERROR_REASSEMBLY,
    QAX_INT_EDB_ERROR_ECC,
    QAX_INT_EDB_ESEM_INTERRUPT_ONE,
    QAX_INT_EDB_GLEM_INTERRUPT_ONE,
    QAX_INT_EDB_EEDB_POINTER_CONFLICT_INT_VEC,
    QAX_INT_DRCC_ERROR_ECC,
    QAX_INT_ECGM_CGM_REP_AROUND_INT_VEC,
    QAX_INT_EPNI_ERROR_ECC,
    QAX_INT_EPNI_PP_INT_VEC,
    QAX_INT_DDP_ERROR_ECC,
    QAX_INT_DDP_ERROR_EXTERNAL_MEM,
    QAX_INT_IEP_ERROR_ECC,
    QAX_INT_PEM_ERROR_ECC,
    QAX_INT_FDR_ERROR_ECC,
    QAX_INT_FDR_INT_REG_1,
    QAX_INT_FDR_INT_REG_2,
    QAX_INT_FDR_INT_REG_3,
    QAX_INT_FDR_INT_REG_4,
    QAX_INT_FMAC_ERROR_ECC,
    QAX_INT_FMAC_INT_REG_1,
    QAX_INT_FMAC_INT_REG_2,
    QAX_INT_FMAC_INT_REG_3,
    QAX_INT_FMAC_INT_REG_4,
    QAX_INT_FMAC_INT_REG_5,
    QAX_INT_FMAC_INT_REG_6,
    QAX_INT_FMAC_INT_REG_7,
    QAX_INT_FMAC_INT_REG_8,
    QAX_INT_CRPS_ERROR_ECC,
    QAX_INT_CRPS_ENGINE,
    QAX_INT_CRPS_SRC_INVLID_ACCESS,
    QAX_INT_CRPS_OFFSET_WAS_FILTERED,
    QAX_INT_CRPS_ACCESS_COLLISION,
    QAX_INT_TAR_ERROR_ECC,
    QAX_INT_FDT_ERROR_ECC,
    QAX_INT_PPDB_A_ERROR_ECC,
    QAX_INT_PPDB_A_OEMA_INTERRUPT_ONE,
    QAX_INT_PPDB_A_OEMB_INTERRUPT_ONE,
    QAX_INT_PPDB_A_ISEM_INTERRUPT_ONE,
    QAX_INT_CFC_ERROR_ECC,
    QAX_INT_EGQ_ERROR_ECC,
    QAX_INT_EGQ_ERPP_DISCARD_INT_VEC,
    QAX_INT_EGQ_PKT_REAS_INT_VEC,
    QAX_INT_EGQ_ERPP_DISCARD_INT_VEC_2,
    QAX_INT_RTP_ERROR_ECC,
    QAX_INT_IPSEC_SPU_WRAPPER_TOP_ERROR_ECC,
    QAX_INT_ILB_ERROR_ECC,
    QAX_INT_ILB_ERROR_MISSING_FRAGMENT,
    QAX_INT_ILB_ERROR_FLUSH,
    QAX_INT_ILB_ERROR_FIFO_OVERFLOW,
    QAX_INT_ILB_ERROR_GROUP_OUT_OF_BUFFERS,
    QAX_INT_ILB_ERROR_MAX_OUT_OF_ORDER,
    QAX_INT_SQM_ERROR_ECC,
    QAX_INT_SQM_TX_FIFOS_ERR_INT,
    QAX_INT_SQM_QDC_ERROR_INT,
    QAX_INT_ILKN_PMH_ERROR_ECC,
    QAX_INT_IDB_ERROR_ECC,
    QAX_INT_DRCA_ERROR_ECC,
    QAX_INT_CGM_ERROR_ECC,
    QAX_INT_CGM_TOTAL_VOQ_OCCUPANCY_ERR_INT,
    QAX_INT_CGM_TOTAL_PB_VSQ_OCCUPANCY_ERR_INT,
    QAX_INT_CGM_VOQ_OCCUPANCY_ERR_INT,
    QAX_INT_CGM_VSQ_OCCUPANCY_ERR_INT,
    QAX_INT_CGM_STATISTICS_FIFOS_ERR_INT,
    QAX_INT_CGM_QDC_ERR_INT,
    QAX_INT_OAMP_ERROR_ECC,
    QAX_INT_OAMP_RMAPEM_INTERRUPT_ONE,
    QAX_INT_IHB_ERROR_ECC,
    QAX_INT_FCR_ERROR_ECC,
    QAX_INT_SCH_ERROR_ECC,
    QAX_INT_ITE_ERROR_ECC,
    QAX_INT_IPSEC_ERROR_ECC,
    QAX_INT_TXQ_ERROR_ECC,
    QAX_INT_NBIL_ERROR_ECC,
    QAX_INT_NBIL_NBI_PKT_DROP_COUNTERS_0_75P_INTERRUPT,
    QAX_INT_NBIL_LINK_STATUS_CHANGE_INT,
    QAX_INT_DRCB_ERROR_ECC,
    QAX_INT_DQM_ERROR_ECC,
    QAX_INT_DQM_TX_FIFOS_ERR_INT,
    QAX_INT_DQM_QDC_ERROR_INT,
    QAX_INT_PTS_ERROR_ECC,
    QAX_INT_ECI_ERROR_ECC,
    QAX_INT_PPDB_B_ERROR_ECC,
    QAX_INT_PPDB_B_LARGE_EM_INTERRUPT_ONE,
    QAX_INT_PPDB_B_LARGE_EM_INTERRUPT_TWO,
	QAX_INT_PPDB_A_TCAM_PROTECTION_ERROR,
    QAX_INT_PPDB_B_ISEM_INTERRUPT_ONE,
/*
 * Parity & ECC error interrupts
 */
    QAX_INT_MMU_ECC_PARITY_ERR_INT,
    QAX_INT_MMU_ECC_ECC_1B_ERR_INT,
    QAX_INT_MMU_ECC_ECC_2B_ERR_INT,
    QAX_INT_IRE_ECC_PARITY_ERR_INT,
    QAX_INT_IRE_ECC_ECC_1B_ERR_INT,
    QAX_INT_IRE_ECC_ECC_2B_ERR_INT,
    QAX_INT_NBIH_ECC_PARITY_ERR_INT,
    QAX_INT_NBIH_ECC_ECC_1B_ERR_INT,
    QAX_INT_NBIH_ECC_ECC_2B_ERR_INT,
    QAX_INT_ILKN_PML_ECC_PARITY_ERR_INT,
    QAX_INT_ILKN_PML_ECC_ECC_1B_ERR_INT,
    QAX_INT_ILKN_PML_ECC_ECC_2B_ERR_INT,
    QAX_INT_OLP_ECC_PARITY_ERR_INT,
    QAX_INT_OLP_ECC_ECC_1B_ERR_INT,
    QAX_INT_OLP_ECC_ECC_2B_ERR_INT,
    QAX_INT_IMP_ECC_PARITY_ERR_INT,
    QAX_INT_IMP_ECC_ECC_1B_ERR_INT,
    QAX_INT_IMP_ECC_ECC_2B_ERR_INT,
    QAX_INT_KAPS_ECC_PARITY_ERR_INT,
    QAX_INT_KAPS_ECC_ECC_1B_ERR_INT,
    QAX_INT_KAPS_ECC_ECC_2B_ERR_INT,
    QAX_INT_IPS_ECC_PARITY_ERR_INT,
    QAX_INT_IPS_ECC_ECC_1B_ERR_INT,
    QAX_INT_IPS_ECC_ECC_2B_ERR_INT,
    QAX_INT_IHP_ECC_PARITY_ERR_INT,
    QAX_INT_IHP_ECC_ECC_1B_ERR_INT,
    QAX_INT_IHP_ECC_ECC_2B_ERR_INT,
    QAX_INT_FDA_ECC_PARITY_ERR_INT,
    QAX_INT_FDA_ECC_ECC_1B_ERR_INT,
    QAX_INT_FDA_ECC_ECC_2B_ERR_INT,
    QAX_INT_SPB_ECC_PARITY_ERR_INT,
    QAX_INT_SPB_ECC_ECC_1B_ERR_INT,
    QAX_INT_SPB_ECC_ECC_2B_ERR_INT,
    QAX_INT_EDB_ECC_PARITY_ERR_INT,
    QAX_INT_EDB_ECC_ECC_1B_ERR_INT,
    QAX_INT_EDB_ECC_ECC_2B_ERR_INT,
    QAX_INT_DRCC_ECC_PARITY_ERR_INT,
    QAX_INT_DRCC_ECC_ECC_1B_ERR_INT,
    QAX_INT_DRCC_ECC_ECC_2B_ERR_INT,
    QAX_INT_EPNI_ECC_PARITY_ERR_INT,
    QAX_INT_EPNI_ECC_ECC_1B_ERR_INT,
    QAX_INT_EPNI_ECC_ECC_2B_ERR_INT,
    QAX_INT_DDP_ECC_PARITY_ERR_INT,
    QAX_INT_DDP_ECC_ECC_1B_ERR_INT,
    QAX_INT_DDP_ECC_ECC_2B_ERR_INT,
    QAX_INT_IEP_ECC_PARITY_ERR_INT,
    QAX_INT_IEP_ECC_ECC_1B_ERR_INT,
    QAX_INT_IEP_ECC_ECC_2B_ERR_INT,
    QAX_INT_PEM_ECC_PARITY_ERR_INT,
    QAX_INT_PEM_ECC_ECC_1B_ERR_INT,
    QAX_INT_PEM_ECC_ECC_2B_ERR_INT,
    QAX_INT_FDR_ECC_PARITY_ERR_INT,
    QAX_INT_FDR_ECC_ECC_1B_ERR_INT,
    QAX_INT_FDR_ECC_ECC_2B_ERR_INT,
    QAX_INT_FMAC_ECC_PARITY_ERR_INT,
    QAX_INT_FMAC_ECC_ECC_1B_ERR_INT,
    QAX_INT_FMAC_ECC_ECC_2B_ERR_INT,
    QAX_INT_CRPS_ECC_PARITY_ERR_INT,
    QAX_INT_CRPS_ECC_ECC_1B_ERR_INT,
    QAX_INT_CRPS_ECC_ECC_2B_ERR_INT,
    QAX_INT_TAR_ECC_PARITY_ERR_INT,
    QAX_INT_TAR_ECC_ECC_1B_ERR_INT,
    QAX_INT_TAR_ECC_ECC_2B_ERR_INT,
    QAX_INT_FDT_ECC_PARITY_ERR_INT,
    QAX_INT_FDT_ECC_ECC_1B_ERR_INT,
    QAX_INT_FDT_ECC_ECC_2B_ERR_INT,
    QAX_INT_PPDB_A_ECC_PARITY_ERR_INT,
    QAX_INT_PPDB_A_ECC_ECC_1B_ERR_INT,
    QAX_INT_PPDB_A_ECC_ECC_2B_ERR_INT,
    QAX_INT_CFC_ECC_PARITY_ERR_INT,
    QAX_INT_CFC_ECC_ECC_1B_ERR_INT,
    QAX_INT_CFC_ECC_ECC_2B_ERR_INT,
    QAX_INT_EGQ_ECC_PARITY_ERR_INT,
    QAX_INT_EGQ_ECC_ECC_1B_ERR_INT,
    QAX_INT_EGQ_ECC_ECC_2B_ERR_INT,
    QAX_INT_RTP_ECC_PARITY_ERR_INT,
    QAX_INT_RTP_ECC_ECC_1B_ERR_INT,
    QAX_INT_RTP_ECC_ECC_2B_ERR_INT,
    QAX_INT_IPSEC_SPU_WRAPPER_TOP_ECC_PARITY_ERR_INT,
    QAX_INT_IPSEC_SPU_WRAPPER_TOP_ECC_ECC_1B_ERR_INT,
    QAX_INT_IPSEC_SPU_WRAPPER_TOP_ECC_ECC_2B_ERR_INT,
    QAX_INT_ILB_ECC_PARITY_ERR_INT,
    QAX_INT_ILB_ECC_ECC_1B_ERR_INT,
    QAX_INT_ILB_ECC_ECC_2B_ERR_INT,
    QAX_INT_SQM_ECC_PARITY_ERR_INT,
    QAX_INT_SQM_ECC_ECC_1B_ERR_INT,
    QAX_INT_SQM_ECC_ECC_2B_ERR_INT,
    QAX_INT_ILKN_PMH_ECC_PARITY_ERR_INT,
    QAX_INT_ILKN_PMH_ECC_ECC_1B_ERR_INT,
    QAX_INT_ILKN_PMH_ECC_ECC_2B_ERR_INT,
    QAX_INT_IDB_ECC_PARITY_ERR_INT,
    QAX_INT_IDB_ECC_ECC_1B_ERR_INT,
    QAX_INT_IDB_ECC_ECC_2B_ERR_INT,
    QAX_INT_DRCA_ECC_PARITY_ERR_INT,
    QAX_INT_DRCA_ECC_ECC_1B_ERR_INT,
    QAX_INT_DRCA_ECC_ECC_2B_ERR_INT,
    QAX_INT_CGM_ECC_PARITY_ERR_INT,
    QAX_INT_CGM_ECC_ECC_1B_ERR_INT,
    QAX_INT_CGM_ECC_ECC_2B_ERR_INT,
    QAX_INT_OAMP_ECC_PARITY_ERR_INT,
    QAX_INT_OAMP_ECC_ECC_1B_ERR_INT,
    QAX_INT_OAMP_ECC_ECC_2B_ERR_INT,
    QAX_INT_IHB_ECC_PARITY_ERR_INT,
    QAX_INT_IHB_ECC_ECC_1B_ERR_INT,
    QAX_INT_IHB_ECC_ECC_2B_ERR_INT,
    QAX_INT_FCR_ECC_PARITY_ERR_INT,
    QAX_INT_FCR_ECC_ECC_1B_ERR_INT,
    QAX_INT_FCR_ECC_ECC_2B_ERR_INT,
    QAX_INT_SCH_ECC_PARITY_ERR_INT,
    QAX_INT_SCH_ECC_ECC_1B_ERR_INT,
    QAX_INT_SCH_ECC_ECC_2B_ERR_INT,
    QAX_INT_ITE_ECC_PARITY_ERR_INT,
    QAX_INT_ITE_ECC_ECC_1B_ERR_INT,
    QAX_INT_ITE_ECC_ECC_2B_ERR_INT,
    QAX_INT_IPSEC_ECC_PARITY_ERR_INT,
    QAX_INT_IPSEC_ECC_ECC_1B_ERR_INT,
    QAX_INT_IPSEC_ECC_ECC_2B_ERR_INT,
    QAX_INT_TXQ_ECC_PARITY_ERR_INT,
    QAX_INT_TXQ_ECC_ECC_1B_ERR_INT,
    QAX_INT_TXQ_ECC_ECC_2B_ERR_INT,
    QAX_INT_NBIL_ECC_PARITY_ERR_INT,
    QAX_INT_NBIL_ECC_ECC_1B_ERR_INT,
    QAX_INT_NBIL_ECC_ECC_2B_ERR_INT,
    QAX_INT_DRCB_ECC_PARITY_ERR_INT,
    QAX_INT_DRCB_ECC_ECC_1B_ERR_INT,
    QAX_INT_DRCB_ECC_ECC_2B_ERR_INT,
    QAX_INT_DQM_ECC_PARITY_ERR_INT,
    QAX_INT_DQM_ECC_ECC_1B_ERR_INT,
    QAX_INT_DQM_ECC_ECC_2B_ERR_INT,
    QAX_INT_PTS_ECC_PARITY_ERR_INT,
    QAX_INT_PTS_ECC_ECC_1B_ERR_INT,
    QAX_INT_PTS_ECC_ECC_2B_ERR_INT,
    QAX_INT_ECI_ECC_PARITY_ERR_INT,
    QAX_INT_ECI_ECC_ECC_1B_ERR_INT,
    QAX_INT_ECI_ECC_ECC_2B_ERR_INT,
    QAX_INT_PPDB_B_ECC_PARITY_ERR_INT,
    QAX_INT_PPDB_B_ECC_ECC_1B_ERR_INT,
    QAX_INT_PPDB_B_ECC_ECC_2B_ERR_INT,

    QAX_INT_DRCA_PHY_CDR_ABOVE_TH,
    QAX_INT_DRCB_PHY_CDR_ABOVE_TH,
    QAX_INT_DRCC_PHY_CDR_ABOVE_TH,

    QAX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0,
    QAX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_1,

    INVALIDr
};

qax_interrupt_type qax_int_disable_print_on_init[] = {
    INVALIDr
};

/*************
 * FUNCTIONS *
 */
int
qax_interrupt_cmn_param_init(int unit, intr_common_params_t* common_params)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(common_params);

    common_params->int_disable_on_init = qax_int_disable_on_init;
    common_params->int_disable_print_on_init = qax_int_disable_print_on_init;
    common_params->int_active_on_init = qax_int_active_on_init;

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

