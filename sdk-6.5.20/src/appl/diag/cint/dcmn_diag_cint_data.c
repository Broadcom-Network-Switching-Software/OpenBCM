/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_LIB_CINT) && (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/drv.h>
#include <appl/diag/system.h>

#include <shared/bsl.h>
#include <sdk_config.h>
#include <bcm/port.h>

#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/register.h>
#include <soc/i2c.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#include <soc/sand/sand_mem.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/stat.h>
#endif

#include <appl/diag/dcmn/diag.h>
#include <appl/diag/dcmn/init_deinit.h>
#include <appl/diag/shell.h>
extern int init_deinit_test(int u, args_t *a, void *p);

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/multicast.h>
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/multicast.h>
#endif

#ifdef BCM_88750_A0
#include <soc/dfe/fe1600/fe1600_drv.h>
#endif
#ifdef BCM_88950_A0
#include <soc/dfe/fe3200/fe3200_drv.h>
#endif

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#if defined(INCLUDE_INTR)
#include <appl/dcmn/rx_los/rx_los.h>
#endif /* defined(INCLUDE_INT) */

#if defined(BCM_PETRA_SUPPORT)

int test_bcm_mc_arad_basic(int unit);
int test_bcm_mc_arad_basic_open(int unit);
int test_bcm_mc_arad_mid_size(int unit);
int test_bcm_mc_arad_replications(int unit, unsigned iter_num);
int test_bcm_mc_arad_1(int unit);
int test_bcm_mc_arad_delete_egress_replications(int unit);
void mc_test_set_assert_type(int unit, int atype);
void mc_test_set_debug_level(int unit, int level);

/* perform the multicast test specified by name, supported only for ARAD */
static int diag_test_mc(int unit, char *test_name)
{
    int         rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (!SOC_IS_ARAD(unit)) {
        cli_out("Device not supported\n");
        return SOC_E_UNIT;
    }

    if (test_name == NULL) {
        return SOC_E_PARAM;
    }
    if (!sal_strcmp(test_name, "mc_open")) {
        if ((rv = test_bcm_mc_arad_basic_open(unit))) {
            cli_out("ERROR: test_bcm_mc_arad_basic returned %d\n",rv);
        }
        return rv;
    }
    if (!sal_strcmp(test_name, "mc_basic")) {
        if ((rv = test_bcm_mc_arad_basic(unit))) {
            cli_out("ERROR: test_bcm_mc_arad_basic returned %d\n",rv);
        }
        return rv;
    }
    if (!sal_strcmp(test_name, "mc_mid_size")) {
        if ((rv = test_bcm_mc_arad_mid_size(unit))) {
            cli_out("ERROR: test_bcm_mc_arad_mid_size returned %d\n",rv);
        }
        return rv;
    }
    if (!sal_strcmp(test_name, "mc_1")) {
        if ((rv = test_bcm_mc_arad_1(unit))) {
            cli_out("ERROR: test_bcm_mc_arad_1 returned %d\n",rv);
        }
        return rv;
    }    if (!sal_strcmp(test_name, "mc_egress_delete")) {
        if ((rv = test_bcm_mc_arad_delete_egress_replications(unit))) {
            cli_out("ERROR: test_bcm_mc_arad_delete_egress_replications "
                    "returned %d\n",rv);
        }
        return rv;
    }
    if (!sal_strcmp(test_name, "print_all")) {
        mc_test_set_debug_level(unit, -1);
        return SOC_E_NONE;
    }
    if (!sal_strcmp(test_name, "print_errors_info")) {
        mc_test_set_debug_level(unit, 0);
        return SOC_E_NONE;
    }
    if (!sal_strcmp(test_name, "print_errors")) {
        mc_test_set_debug_level(unit, 1);
        return SOC_E_NONE;
    }
    if (!sal_strcmp(test_name, "test_assert")) { /* set MC test assert type to assert() */
        mc_test_set_assert_type(unit, 1);
        return SOC_E_NONE;
    }
    if (!sal_strcmp(test_name, "test_assert_0")) { /* set MC test assert type to devision by zero */
        mc_test_set_assert_type(unit, 0);
        return SOC_E_NONE;
    }
    if (!sal_strcmp(test_name, "test_assert_none")) { /* set MC test assert type to not assert */
        mc_test_set_assert_type(unit, 2);
        return SOC_E_NONE;
    }
    if (!sal_strcmp(test_name, "get_mc_asserts")) { /* Get the number of MC asserts that occurred */
        uint32 num = dpp_mcds_get_nof_asserts();
        if (num) {
            cli_out("%lu multicast implementation asserts occurred\n",
                    (unsigned long)num);
        }
        return num;

    } else if (!sal_strcmp(test_name, "get_nof_mcids") || !sal_strcmp(test_name, "get_nof_egress_mcids")) { /* get number of egress MCIDs */
        return SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;
    } else if (!sal_strcmp(test_name, "get_nof_ingress_mcids")) { /* get number of egress MCIDs */
        return SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
    } else if (!sal_strcmp(test_name, "get_nof_egress_bitmaps")) { /* get number of egress MCIDs */
        return
#if defined(BCM_88470_A0) 
          SOC_IS_QAX(unit) ? SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmaps :
#endif
          DPP_MC_EGR_NOF_BITMAPS;
    } else if (!sal_strcmp(test_name, "get_nof_ingress_bitmaps")) { /* get number of egress MCIDs */
        return
#if defined(BCM_88470_A0) 
          SOC_IS_QAX(unit) ? SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmaps :
#endif
          0;
    } else if (!sal_strcmp(test_name, "get_nof_egress_bitmap_groups")) { /* get number of egress bitmap groups */
        return SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high + 1;
    } else if (!sal_strcmp(test_name, "perform_mc_assert")) { /* perform a MC assert */
        DPP_MC_ASSERT(0);
        return SOC_E_NONE;
    } else if (!sal_strcmp(test_name, "get_nof_ingress_rep_cuds")) { /* get number of egress MCIDs */
        return SOC_IS_QAX(unit) ? 2 : 1;
    } else if (!sal_strcmp(test_name, "get_nof_egress_rep_cuds")) { /* get number of egress MCIDs */
        return SOC_IS_JERICHO(unit) ? 2 : 1;
    }

    cli_out("ERROR: unknown test name, use one of: "
            "mc_open, mc_basic, mc_mid_size, mc_egress_delete, "
            "print_all, print_errors_info, print_errors, test_assert, "
            "test_assert_0, test_assert_none, perform_mc_assert, get_mc_asserts, "
            "get_nof_egress_mcids, get_nof_ingress_mcids, get_nof_egress_bitmap_groups, "
            "get_nof_egress_bitmaps , get_nof_ingress_bitmaps, get_nof_ingress_rep_cuds, get_nof_egress_rep_cuds\n");
    return SOC_E_PARAM;
}

/* perform the multicast test specified by name, supported only for ARAD */
static int diag_test_mc2(int unit, char *test_name, int param)
{
    int         rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    } else if (!SOC_IS_ARAD(unit)) {
        cli_out("Device not supported\n");
        return SOC_E_UNIT;
    } else if (!test_name) {
        return SOC_E_PARAM;
    } else if (!sal_strcmp(test_name, "mc_replications")) {
        if (param < 0) {
        cli_out("Illegal number of iterations\n");
        return SOC_E_PARAM;
        }
        if ((rv = test_bcm_mc_arad_replications(unit, param))) {
            cli_out("ERROR: test_bcm_mc_arad_replications(%d, %d) "
                    "returned %d\n",unit, param, rv);
        }
        return rv;
    } else if (!sal_strcmp(test_name, "get_irdb")) {
        int out = -1;
        uint32 entry[2];
        const unsigned bit_offset = 2 * (param % 16);
        const int table_index = param / 16;
        if (param < 0 || param > BCM_DPP_MAX_MC_ID(unit)) {
            cli_out("Illegal mcid %d\n", param);
        } else if ((out = READ_IDR_IRDBm(unit, MEM_BLOCK_ANY, table_index, entry)) < 0) {
            cli_out("failed to read IRDB for group %d\n", param);
        } else {
            out = ((*entry) >> bit_offset) & 3;
        }
        return out;
    } else if (!sal_strcmp(test_name, "mc_asserts_enable")) { /* Enable (0/1) MC asserts: in addition to counting them, an error message will be given and possibly assert() will be called */
        dpp_mcds_set_mc_asserts_enabled(param);
        return SOC_E_NONE;
    } else if (!sal_strcmp(test_name, "mc_asserts_real")) { /* Enable (0/1) calling assert() for MC asserts when using diag_test_mc2(unit, "mc_asserts_enable", 1);  */
        dpp_mcds_set_mc_asserts_real(param);
        return SOC_E_NONE;
    }

    cli_out("ERROR: unknown test name, use one of: "
            "mc_replications get_irdb mc_asserts_enable\n");
    return SOC_E_PARAM;
}


static int diag_l3_param(int unit, char *test_name)
{
    if (!SOC_UNIT_VALID(unit)) {
            cli_out("Invalid unit.\n");
            return SOC_E_UNIT;
    } else if (!sal_strcmp(test_name, "max_nof_vrfs")) {
        return SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs;
    }
    return SOC_E_PARAM;
}
/**
 *  structure use for calcultate max entry size and max table
 *  size in bytes for the tested memories and the  memories
 *  filter list
 */
typedef struct {
    uint32 max_entry_size; 
    uint32 max_table_size;
    uint8  *tested_mems;
} ser_mem_init_t;


/**
 * structure use to pass information needed   by write call back 
 * as the pattern to fill and the  memories filter list 
 */
typedef struct {
    int unit;
    uint32 *patterns;
    uint8  *tested_mems;
} ser_mem_write_t;

/**
 * structure use to pass information needed   by read call back 
 * as buffer to read into,expected pattern , summary report 
 * counters as number of num of tested bits  and memories filter list 
 */

typedef struct {
    int unit;
    uint32 pattern;
    uint32 *mem_ptr;
    uint32 num_of_err_entries;
    uint32 num_of_err_bits;
    uint32 num_of_mem_err_bits;
    uint32 num_of_entries;
    uint32 num_of_bits;
    uint32 *patterns;
    uint8  *tested_mems;
    uint32 error_constant;
    int current_read_iteration;
} ser_mem_read_t;

extern int bcm_common_linkscan_enable_set(int,int);
extern int disable_ecc_disable_dynamic_mechanism(int unit);
int rval_test_skip_mem_flipflop(int unit, soc_mem_t mem);

extern _bcm_dpp_counter_state_t *_bcm_dpp_counter_state[SOC_MAX_NUM_DEVICES];

const soc_mem_t  arad_default_tested_mems_list[] = {
      IHP_MEM_500000m,
      IHP_MEM_590000m,
      IRR_MCDBm,
      PPDB_B_LIF_TABLEm,
      IQM_BDBLLm,
      IPS_QDESC_TABLEm,
      EDB_EEDB_BANKm,
      IQM_TAILm,
      EGQ_VLAN_TABLEm,
      IPS_CRBALm,
      IQM_PQDMSm,
      CRPS_CRPS_0_CNTS_MEMm,
      CRPS_CRPS_1_CNTS_MEMm,
      CRPS_CRPS_2_CNTS_MEMm,
      CRPS_CRPS_3_CNTS_MEMm,
      IDR_MCDA_DYNAMICm,
      IDR_MCDB_DYNAMICm,
      IHB_FEC_ENTRYm,
      IHP_MEM_300000m,
      IPS_QDESCm,
      IPS_QSZm,
      IQM_FLUSCNTm,
      EGQ_VSI_MEMBERSHIPm,
      EPNI_TX_TAG_TABLEm,
      IHP_VSI_PORT_MEMBERSHIPm,
      IHB_LPM_2m,
      IHB_LPM_3m,
      IHB_LPM_4m,
      IHB_LPM_5m,
      IHP_MEM_7A0000m,
      IHP_MEM_A10000m,
      OAMP_MEP_DBm,
      IRR_DESTINATION_TABLEm,
      EGQ_MAP_OUTLIF_TO_DSPm,
      IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATIONm,
      IHP_MEM_830000m,
      IHP_MEM_AA0000m,
      PPDB_B_LARGE_EM_FID_COUNTER_DBm,
      OAMP_RMEP_DBm,
      IHB_LPMm,
      IHB_LPM_6m,
      IHB_MEM_F30000m,
      IPT_FDTDATAm,
      PPDB_A_TCAM_ACTIONm,
      IPS_QTYPEm,
      EPNI_MEM_520000m,
      IPS_FLWIDm,
      IHP_VIRTUAL_PORT_TABLEm,
      IDR_MEM_50000m,
      IDR_MEM_180000m,
      IDR_MCDA_PRFSELm,
      IDR_MCDB_PRFSELm,
      IHP_OAMAm,
      OAMP_MEM_100000m,
      IPS_QPM_1_SYS_REDm,
      IQM_CPDMDm,
      IRR_MEM_340000m,
      IRR_LAG_MAPPINGm,
      EPNI_MEM_5B0000m,
      EPNI_DSCP_REMARKm,
      IPT_PCQm,
      IHP_MEM_620000m,
      IHB_FEC_SUPER_ENTRYm,
      OAMP_MEM_190000m,
      IPT_SOP_MMUm,
      IPS_QPM_2_NO_SYS_REDm,
      IQM_CPDMSm,
      OAMP_UMC_TABLEm,
      IHB_VRF_CONFIGm,
      IRR_MEM_300000m,
      IRR_FREE_PCB_MEMORYm,
      IRR_PCB_LINK_TABLEm,
      IHB_MEM_1530000m,
      IQM_MNUSCNTm,
      PPDB_A_TCAM_ENTRY_PARITYm,
      IHB_MEM_E00000m,
      IHB_MEM_E10000m,
      IDR_IRDBm,
      IHB_MEM_EA0000m,
      IPS_MEM_1A0000m,
      IHB_MEM_260000m,
      IHP_VSI_LOW_CFG_2m,
      IPT_BDQm,
      IHB_MEM_14A0000m,
      EPNI_ISID_TABLEm,
      PPDB_B_LARGE_EM_FID_PROFILE_DBm,
      IQM_OCBPRMm,
      IHP_BVD_CFGm,
      IDR_MEM_60000m,
      IHP_OAMBm,
      IHP_VSI_LOW_CFG_1m,
      EPNI_MY_CFM_MAC_TABLEm,
      IHP_OAM_MY_CFM_MACm,
      MMU_IDFm,
      IHB_DESTINATION_STATUSm,
      IHP_DSCP_EXP_REMARKm,
      IDR_MCDA_PRFCFG_0m,
      IDR_MCDB_PRFCFG_0m,
      IRR_MEM_3C0000m,
      RTP_UNICAST_DISTRIBUTION_MEMORYm,
      RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm,
      RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm,
      IRR_MCR_MEMORYm,
      EGQ_AUX_TABLEm,
      IPT_EGQDATAm,
      IHP_LIF_ACCESSEDm,
      IPT_MOP_MMUm,
      IPT_MEM_80000m,
      IDR_MEM_30000m,
      IHP_VSI_HIGH_PROFILEm,
      FDT_IN_BAND_MEMm,
      IRR_FLOW_TABLEm,
      IHB_MEM_270000m,
      IPS_MEM_180000m,
      EPNI_ACE_TABLEm,
      IHB_FEC_ECMPm,
      IHB_MEM_250000m,
      IPT_FDTCTLm,
      IHB_FWD_ACT_PROFILEm,
      EGQ_RCMUCm,
      EGQ_RCMMCm,
      CRPS_CRPS_0_OVTH_MEMm,
      CRPS_CRPS_1_OVTH_MEMm,
      CRPS_CRPS_2_OVTH_MEMm,
      CRPS_CRPS_3_OVTH_MEMm,
      EPNI_ACE_TO_OUT_PP_PORTm,
      EPNI_PRGE_DATAm,
      EPNI_REMARK_IPV4_TO_DSCPm,
      EPNI_REMARK_IPV6_TO_DSCPm,
      IHP_STP_TABLEm,
      IHP_VRID_MY_MAC_MAPm,
      PPDB_B_LARGE_EM_PORT_MINE_TABLE_PHYSICAL_PORTm,
      EGQ_FDMm,
      IDR_MEM_40000m,
      IHB_FEC_ENTRY_ACCESSEDm,
      IHP_MEM_8C0000m,
      IHP_MEM_B30000m,
      IHP_VSI_HIGH_MY_MACm,
      NBI_MLF_TX_MEM_CTRLm,
      IDR_MEM_10000m,
      EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm,
      IHP_DSCP_EXP_MAPm,
      PPDB_A_TCAM_ACTION_HIT_INDICATIONm,
      EGQ_RPDMm,
      IQM_PQREDm,
      EPNI_ACE_TO_FHEIm,
      IHB_TIME_STAMP_FIFOm,
      IHB_OAM_COUNTER_FIFOm,
      IHB_PMF_FES_PROGRAMm,
      IDR_MEM_00000m,
      IHB_PATH_SELECTm,
      IRR_SMOOTH_DIVISIONm,
      IRR_STACK_TRUNK_RESOLVEm,
      IHP_PINFO_LLRm,
      EPNI_ACE_TO_OUT_LIFm,
      EPNI_DSCP_EXP_TO_PCP_DEIm,
      EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm,
      IHB_IN_PORT_KEY_GEN_VARm,
      IHP_BVD_TOPOLOGY_IDm,
      IHP_VTT_IN_PP_PORT_VLAN_CONFIGm,
      IRE_TDM_CONFIGm,
      EPNI_MEM_640000m,
      IHB_MEM_FC0000m,
      OAMP_MEM_220000m,
      IHB_PINFO_LBPm,
      IRR_IS_FREE_PCB_MEMORYm,
      IRR_IS_PCB_LINK_TABLEm,
      IQM_VS_QD_QSZm,
      IQM_VS_QF_QSZm,
      EGQ_HEADER_MAPm,
      EPNI_HEADER_MAPm,
      EPNI_PP_COUNTER_TABLEm,
      EGQ_QDCT_TABLEm,
      EGQ_CH_0_SCMm,
      EGQ_CH_1_SCMm,
      EGQ_QP_SCMm,
      EGQ_TCG_SCMm,
      EGQ_QQST_TABLEm,
      IDR_ETHERNET_METER_CONFIGm,
      EGQ_TTL_SCOPEm,
      EPNI_REMARK_IPV4_TO_EXPm,
      EPNI_REMARK_IPV6_TO_EXPm,
      EGQ_FBMm,
      EGQ_PQSMm,
      EGQ_QQSMm,
      IQM_SRCQRNGm,
      EGQ_RRDMm,
      IHB_PINFO_COUNTERSm,
      EGQ_QM_3m,
      EGQ_PDCMm,
      EGQ_QDCMm,
      IHB_PMF_PASS_1_KEY_GEN_LSBm,
      IHB_PMF_PASS_1_KEY_GEN_MSBm,
      IHB_PMF_PASS_2_KEY_GEN_LSBm,
      IHB_PMF_PASS_2_KEY_GEN_MSBm,
      IHP_LL_MIRROR_PROFILEm,
      IRR_LAG_TO_LAG_RANGEm,
      EGQ_PQSMAXm,
      EGQ_QQSMAXm,
      IHP_VTT_IN_PP_PORT_CONFIGm,
      IRR_ISF_MEMORYm,
      EGQ_DCMm,
      IRR_LAG_NEXT_MEMBERm,
      EGQ_PMCm,
      EGQ_QP_PMCm,
      EGQ_TCG_PMCm,
      IRE_NIF_CTXT_MAPm,
      IQM_MEM_7E00000m,
      EGQ_DSP_PTR_MAPm,
      EGQ_VSI_PROFILEm,
      EPNI_EVEC_TABLEm,
      EPNI_MIRROR_PROFILE_TABLEm,
      IHB_CPU_TRAP_CODE_CTRm,
      OAMP_MEM_50000m,
      IHB_PMF_PROGRAM_SELECTION_CAMm,
      EGQ_PDCMAXm,
      EGQ_QDCMAXm,
      IHB_MEM_15C0000m,
      FDT_MEM_100000m,
      EGQ_QM_0m,
      IHB_PINFO_FLPm,
      IHB_FEM_MAP_INDEX_TABLEm,
      IHP_TOS_2_COSm,
      IRR_STACK_FEC_RESOLVEm,
      EGQ_CH_2_SCMm,
      EGQ_CH_3_SCMm,
      EGQ_CH_4_SCMm,
      EGQ_CH_5_SCMm,
      EGQ_CH_6_SCMm,
      EGQ_CH_7_SCMm,
      EGQ_CH_8_SCMm,
      EGQ_CH_9_SCMm,
      EGQ_NONCH_SCMm,
      CFC_SPI_OOB_TX_0_CALm,
      CFC_SPI_OOB_TX_1_CALm,
      EGQ_QM_2m,
      EGQ_TCG_CBMm,
      EGQ_CBMm,
      EGQ_QP_CBMm,
      IHP_IEEE_1588_ACTIONm,
      IHB_PMF_FEM_PROGRAMm,
      CFC_SPI_OOB_RX_0_CALm,
      CFC_SPI_OOB_RX_1_CALm,
      EGQ_QM_1m,
      IQM_CNREDm,
      IQM_VQPR_MAm,
      IQM_VQPR_MBm,
      IQM_VQPR_MCm,
      IQM_VQPR_MDm,
      IQM_VQPR_MEm,
      IQM_VQPR_MFm,
      IHB_FLP_KEY_CONSTRUCTIONm,
      IPS_MEM_1E0000m,
      IQM_SPRDPRMm,
      IDR_CONTEXT_MRUm,
      IHB_LB_PFC_PROFILEm,
      IHP_BVD_FID_CLASSm,
      IHP_VLAN_EDIT_PCP_DEI_MAPm,
      CRPS_MEM_0080000m,
      CRPS_MEM_0090000m,
      CRPS_MEM_00A0000m,
      CRPS_MEM_00B0000m,
      IPS_QPRISELm,
      EGQ_DWMm,
      IHB_PTC_KEY_GEN_VARm,
      IHP_VTT_PP_PORT_TT_KEY_VARm,
      IHP_VTT_PP_PORT_VT_KEY_VARm,
      IRE_CPU_CTXT_MAPm,
      IRE_RCY_CTXT_MAPm,
      EPNI_EXP_REMARKm,
      EPNI_PCP_DEI_TABLEm,
      OAMP_MEM_40000m,
      OAMP_MEM_60000m,
      OAMP_LOCAL_PORT_2_SYSTEM_PORTm,
      IHB_FLP_PROGRAM_SELECTION_CAMm,
      IQM_VS_QC_QSZm,
      IPS_MEM_220000m,
      CFC_ILKN_TX_0_CALm,
      CFC_ILKN_TX_1_CALm,
      IHP_VRID_TO_VRF_MAPm,
      IHP_VTT_PP_PORT_VSI_PROFILESm,
      IQM_VS_QD_AVGm,
      IQM_VS_QF_AVGm,
      OAMP_MEM_70000m,
      CFC_ILKN_RX_0_CALm,
      CFC_ILKN_RX_1_CALm,
      IHP_ETHERNET_OAM_OPCODE_MAPm,
      IRE_CTXT_MEM_CONTROLm,
      CFC_NIF_PFC_MAPm,
      IHB_TCAM_ACTION_24m,
      IHB_TCAM_ACTION_25m,
      IHB_TCAM_ACTION_26m,
      IHB_TCAM_ACTION_27m,
      EGQ_TC_DP_MAPm,
      EPNI_PRGE_INSTRUCTION_1m,
      EPNI_PRGE_INSTRUCTION_2m,
      EPNI_PRGE_INSTRUCTION_3m,
      EPNI_PRGE_INSTRUCTION_4m,
      EPNI_PRGE_INSTRUCTION_5m,
      EPNI_PRGE_INSTRUCTION_6m,
      EPNI_PRGE_INSTRUCTION_7m,
      EPNI_PRGE_INSTRUCTION_8m,
      EPNI_PRGE_INSTRUCTION_9m,
      EPNI_PRGE_INSTRUCTION_10m,
      EPNI_PRGE_INSTRUCTION_11m,
      IHP_VTT_2ND_KEY_PROG_SEL_TCAMm,
      IHP_MEM_580000m,
      IHB_PINFO_FERm,
      IHP_LLR_LLVPm,
      IHP_PORT_PROTOCOLm,
      IHP_RESERVED_MCm,
      IPT_ITM_TO_OTM_MAPm,
      IPT_SNP_MIR_CMD_MAPm,
      IQM_DBFFMm,
      IHP_FLP_LOOKUPSm,
      EGQ_DWM_8Pm,
      OAMP_MEM_80000m,
      EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm,
      SCH_CIR_SHAPER_CALENDAR__CSCm,
      SCH_DSP_2_PORT_MAP_DSPPm,
      SCH_PIR_SHAPER_CALENDAR__PSCm,
      EPNI_PRGE_PROGRAM_SELECTION_CAMm,
      EPNI_PRGE_INSTRUCTION_0m,
      IHP_VTT_1ST_KEY_PROG_SEL_TCAMm,
      PPDB_A_TCAM_ACCESS_PROFILEm,
      IQM_VS_QB_QSZm,
      IQM_VS_QE_QSZm,
      IRR_SNOOP_MIRROR_TABLE_1m,
      OLP_DSP_EVENT_ROUTEm,
      EDB_ESEM_STEP_TABLEm,
      EGQ_CCMm,
      IHB_L_4_OPSm,
      IHB_PMF_PROGRAM_COUNTERSm,
      IHB_TCAM_ENTRY_PARITY_12m,
      IHB_TCAM_ENTRY_PARITY_13m,
      IHP_ISA_STEP_TABLEm,
      IHP_ISB_STEP_TABLEm,
      IQM_GRSPRMm,
      OAMP_REMOTE_MEP_EXACT_MATCH_STEP_TABLEm,
      PPDB_A_OEMA_STEP_TABLEm,
      PPDB_A_OEMB_STEP_TABLEm,
      PPDB_B_LARGE_EM_STEP_TABLEm,
      IHP_MEM_610000m,
      EGQ_IVEC_TABLEm,
      EPNI_IVEC_TABLEm,
      IHP_MEM_820000m,
      IHP_MEM_A90000m,
      FDT_IRE_TDM_MASKSm,
      IQM_DELFFMm,
      IHB_SNOOP_ACTIONm,
      IHB_FLP_PROCESSm,
      EPNI_MEM_5A0000m,
      IPS_MEM_240000m,
      IQM_ITMPMm,
      IHB_PROGRAM_KEY_GEN_VARm,
      IHP_DEFAULT_COUNTER_SOURCEm,
      PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm,
      EGQ_MAP_PS_TO_IFCm,
      EPNI_COUNTER_SOURCE_MAPm,
      EPNI_ETH_OAM_OPCODE_MAPm,
      EPNI_REMARK_MPLS_TO_DSCPm,
      IHB_MEM_FB0000m,
      IHB_PINFO_PMFm,
      IHB_PTC_INFO_PMFm,
      IQM_VSQDRC_Dm,
      IQM_VSQDRC_Fm,
      SCH_PORT_SCHEDULER_MAP_PSMm,
      OAMP_MEM_180000m,
      EGQ_FQP_NIF_PORT_MUXm,
      IDR_ETHERNET_METER_PROFILESm,
      IPT_EGQCTLm,
      IQM_VS_QC_AVGm,
      IRR_SNOOP_MIRROR_DEST_TABLEm,
      IHB_PMF_PASS_1_LOOKUPm,
      IHB_PMF_PASS_2_LOOKUPm,
      IHP_FLP_PROGRAM_KEY_GEN_VARm,
      EGQ_EPS_PRIO_MAPm,
      EGQ_PQP_NIF_PORT_MUXm,
      IHP_VLAN_RANGE_COMPRESSION_TABLEm,
      IQM_NIFTCMm,
      IHB_MEM_1520000m,
      MMU_FDFm,
      SCH_ONE_PORT_NIF_CONFIGURATION__OPNCm,
      PPDB_A_TCAM_PD_PROFILEm,
      IDR_OCB_BUFFER_TYPEm,
      IHB_MEM_15B0000m,
      IHB_MEM_F20000m,
      IHP_RECYCLE_COMMANDm,
      EPNI_MEM_630000m,
      IHP_FLP_PTC_PROGRAM_SELECTm,
      IHP_MEM_8B0000m,
      IHP_MEM_B20000m,
      IDR_CONTEXT_COLORm,
      IHP_PARSER_CUSTOM_MACRO_WORD_MAPm,
      IQM_PQWQm,
      SCH_PORT_GROUP_PFGMm,
      IHB_PMF_PROGRAM_GENERALm,
      IHP_UNKNOWN_DA_MAPm,
      OAMP_MEM_210000m,
      EGQ_CNM_QUANTA_TO_FC_MAPm,
      IRR_TRAFFIC_CLASS_MAPPINGm,
      CFC_RCL_VSQ_MAPm,
      EPNI_REMARK_MPLS_TO_EXPm,
      IQM_VS_QB_AVGm,
      IQM_VS_QE_AVGm,
      MMU_RAF_WADDRm,
      MMU_RDF_RADDRm,
      IHB_PMF_INITIAL_KEY_2ND_PASSm,
      IRE_NIF_PORT_MAPm,
      MMU_WAF_HALFA_WADDRm,
      MMU_WAF_HALFB_WADDRm,
      IDR_DROP_PRECEDENCE_MAPPINGm,
      IQM_VSQDRC_Cm,
      SCH_PORT_ENABLE__PORTENm,
      EGQ_MC_SP_TC_MAPm,
      IHB_PFC_INFOm,
      PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm,
      SCH_FC_MAP__FCMm,
      IHP_MEM_380000m,
      IQM_VSQDRC_Bm,
      IQM_VSQDRC_Em,
      IHB_PMF_PASS_2_KEY_UPDATEm
};

const soc_mem_t  fe1600_default_tested_mems_list[] = {
    RTP_SCTINCm,
    RTP_SLSCTm, 
    RTP_TOTSFm,
    RTP_MEM_1100000m,
    RTP_FFLBPm,
    RTP_FFLBSm,
    RTP_MULTI_TBm,
    RTP_RCGLBTm,
    RTP_DLLUPm,
    RTP_DLLUSm,
    RTP_MEM_800000m,
    RTP_MEM_900000m,
    RTP_MCLBTPm,
    RTP_MCLBTSm,
    RTP_RMHMTm,
    RTP_CUCTm,
    RTP_DUCTPm,
    RTP_DUCTSm,
    RTP_MCTm
};

/**
 * 
 * 
 * @author elem (09/11/2014)
 *  the function translate emory table name into memory table id
 *  return INVALIDm in case of a failure
 * @param unit 
 * @param name  - name of a memory to translate into mem id as 
 *              apear at allmem.h
 * 
 * @return soc_mem_t 
 */
static soc_mem_t  ser_mem_name_to_id(int unit, char *name)
{
    int         copyno;
    soc_mem_t   mem;
    /* unsigned    array_index; */

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, &mem, name, &copyno, NULL) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",name);
        return INVALIDm;
    }


    if (SOC_MEM_ALIAS_MAP(unit, mem) != INVALIDm && SOC_MEM_ALIAS_MAP(unit, mem) != mem) {
        mem = SOC_MEM_ALIAS_MAP(unit, mem) != INVALIDm ? SOC_MEM_ALIAS_MAP(unit, mem) : mem;
        cli_out("Warning:replacing mem %s with  its parent mem %s\n",name,SOC_MEM_NAME(unit,mem));
    }


    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY)||
        (2+soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit)) 
        )  {

        cli_out("Error: Memory %s not valid for ser test %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        return INVALIDm;
    }

    return mem;
}


/**
 * memories filtering function 
 * that decide if a given memory should include in the test 
 */

static int ser_mem_enable_for_test(int unit, soc_mem_t mem, uint8 *tested_mems)
{
    /* Memory is valid for this test if it is valid, and not ro / wo. */
    return tested_mems[mem];

/* i leave this code inside in case we want to  stop filter acoring to constant list reside in inc_mem*/
    /*
    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY)||
        (2+soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit)) ||
        (end_index - start_index <= 16) ||
         skip_mem(mem) ||
        (SOC_MEM_IS_ALIAS_OR_FORMAT(unit,mem))) {
        return 0;
    } 
    */ 
    return 1;
}

/**
 *  initialize device for ser test
 */

static int init_device_for_mems_ser_test(int unit, ser_mem_read_t *ser_mem_read)
{
    int rv=0;
    uint8 is_allocated = TRUE;
    if (!SOC_IS_ARAD(unit) && !SOC_IS_FE1600_B0(unit) && !SOC_IS_FE3200(unit)){
        return -1;
    }

     if (sand_init_fill_table(unit)) {
         return -1;
     }
     bcm_common_linkscan_enable_set(unit,0);
     rv = soc_counter_stop(unit);
     if (rv < 0) {
         return -1;
     }
     if (SOC_IS_DPP(unit)) {
         if (BCM_E_NONE != sw_state_access[unit].dpp.bcm.counter.is_allocated(unit, &is_allocated)) {
             cli_out("unit %d sw_state_access[unit].dpp.bcm.counter.is_allocated failed\n", unit);

             return -1;
         } 
         if (is_allocated) {
             if (BCM_E_NONE == bcm_dpp_counter_bg_enable_set(unit, FALSE)) {
                 cli_out("unit %d counter processor background accesses suspended\n", unit);
             } else {
                 return -1;
             } 
         }
     }

           
    if (SOC_IS_ARAD(unit)) {
        soc_reg_t reg = ECI_GLOBAL_MEM_OPTIONSr;
        uint32 values[] = {0x1,0x1};
        soc_field_t fields[] ={DIS_ECCf,CPU_BYPASS_ECC_PARf};
        if ((rv = soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_REG_ACCESS,SOC_DPP_RESET_ACTION_INOUT_RESET)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,(BSL_META_U(unit, "ERROR: Unable to reinit unit %d \n"), unit));
            return -1;
        } 
        disable_ecc_disable_dynamic_mechanism(unit);
        if (SOC_E_NONE != soc_reg_fields32_modify(unit, reg, REG_PORT_ANY,2,fields,values))
            return -1;
    }
    else {
        soc_field_t fields[] ={DIS_ECCf,CPU_BYPASS_ECC_PARf,FE_13_MODEf};
        uint32 values[] = {0x1,0x1,0x1};
        soc_reg_t reg = SOC_IS_FE3200(unit) ?  ECI_GLOBAL_MEM_OPTIONSr:ECI_GLOBAL_1r;
        bcm_switch_event_control_t type  = {BCM_SWITCH_EVENT_CONTROL_ALL,0,bcmSwitchEventMask};
        if (SOC_E_NONE != soc_reg_fields32_modify(unit, reg, REG_PORT_ANY,2,fields,values))
            return -1;
        if (BCM_E_NONE != bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type,1))
            return -1;;
    }

    return 0;
}


/* init callback for get  maximum table size*/

static int
ser_mem_init_cb(int unit, soc_mem_t mem, void* data)
{
  soc_mem_info_t *memp= &SOC_MEM_INFO(unit, mem);
  uint32 entry_size = soc_mem_entry_words(unit, mem);
  uint32 table_size =  (memp->index_max + 1 -  memp->index_min) *  entry_size    ;
  ser_mem_init_t *ser_mem_init  = (ser_mem_init_t *) data ;
  if (!ser_mem_enable_for_test(unit, mem,ser_mem_init->tested_mems)) {
      return 0;
  }
  ser_mem_init->max_table_size = ser_mem_init->max_table_size >= table_size ? ser_mem_init->max_table_size : table_size; 
  ser_mem_init->max_entry_size = ser_mem_init->max_entry_size >= entry_size ? ser_mem_init->max_entry_size : entry_size; 

  return 0;
}

/*
* callback for writing table memory   with a given pattern
*/
static int
ser_mem_write_cb(int unit, soc_mem_t mem, void* data)
{
  ser_mem_write_t *ser_mem_write = (ser_mem_write_t *) data;
  uint32  *patterns = ser_mem_write->patterns ;
  int ret = 0;
  int blk;
 
 
  if (!ser_mem_enable_for_test(unit, mem,ser_mem_write->tested_mems)) {
      return 0;
  }

  SOC_MEM_BLOCK_ITER(unit, mem, blk) {   

       ret = sand_fill_table_with_entry(unit, mem, blk, patterns);
  }
  return ret;
}

/*
* callback for read table memory  and 
* compare it with the expected value 
* count an errors 
*/

static int
ser_mem_read_and_compare_cb(int unit, soc_mem_t mem, void* data)
{
    int i,ret;
    ser_mem_read_t *ser_mem_read = (ser_mem_read_t *) data;
    soc_mem_info_t *memp= &SOC_MEM_INFO(unit, mem);
    uint32 mem_row_bit_width = soc_mem_entry_bits(unit, mem);
    uint32 table_num_of_bits = mem_row_bit_width *(memp->index_max + 1 - memp->index_min);
    int remainig_table_bits_len;
    uint32 numels = SOC_MEM_IS_ARRAY(unit,mem) ? SOC_MEM_ARRAY_INFO(unit,mem).numels : 1;
    uint32 expected_value = ser_mem_read->patterns[0];
    uint32 table_nof_error_bits=0;
    char err_print_buffer[2000],helper_print_buffer[2000];
    char err_bits_buffer[2000],helper_print_buffer2[2000];
    uint32 error_constant = ser_mem_read->error_constant;
    int                  block_instance;
    int num_of_insts=0;


    if (!ser_mem_enable_for_test(unit, mem,ser_mem_read->tested_mems)) {
        return 0;
    }

    SOC_MEM_BLOCK_ITER(unit, mem, block_instance) {   
        num_of_insts++;
    /* loop over all array entries*/
        for (i=0;i< numels;i++) {
            uint32 *entry_data;
            int entry_index;

            ret = soc_mem_array_read_range(unit, mem,i, block_instance,memp->index_min,memp->index_max,ser_mem_read->mem_ptr);
            if (ret != SOC_E_NONE) {
                return -1;
            }

            for (entry_data = ser_mem_read->mem_ptr,entry_index = memp->index_min; entry_index <= memp->index_max; ++entry_index) {
                int nof_error_bits = 0, t1, t2;
                uint32 *prev_entry_data = entry_data;
                char buf[250];
                /* coverity[secure_coding] */
                sal_strcpy(err_print_buffer,"");
                sal_strcpy(err_bits_buffer,"");
                for (remainig_table_bits_len=mem_row_bit_width; remainig_table_bits_len >= 32; remainig_table_bits_len -= 32) {
                    if ((t1 = (error_constant | *(entry_data++)) ^ expected_value)) {
                        /* coverity[callee_ptr_arith] */
                        SHR_BITCOUNT_RANGE( (uint32 *)&t1, t2, 0, 32);
                        nof_error_bits += t2;

                    }
                    sal_sprintf(buf,"%08x",*prev_entry_data | error_constant);
                    sal_sprintf(helper_print_buffer,"%s%s",buf,err_print_buffer);
                    /* coverity[secure_coding] */
                    sal_strcpy(err_print_buffer,helper_print_buffer);

                    sal_sprintf(buf,"%08x",(*prev_entry_data  | error_constant)^ expected_value);
                    sal_sprintf(helper_print_buffer2,"%s%s",buf,err_bits_buffer);
                    sal_strcpy(err_bits_buffer,helper_print_buffer2);
                    prev_entry_data = entry_data;
                }
                if (remainig_table_bits_len > 0 ) {
                    sal_sprintf(buf,"%8x",(*prev_entry_data | error_constant)& ((((uint32)1)<<remainig_table_bits_len) - 1));
                    sal_sprintf(helper_print_buffer,"%s%s",buf,err_print_buffer);
                    /* coverity[secure_coding] */
                    sal_strcpy(err_print_buffer,helper_print_buffer);

                    sal_sprintf(buf,"%8x",((*prev_entry_data  | error_constant) ^ expected_value) & ((((uint32)1)<<remainig_table_bits_len) - 1));
                    sal_sprintf(helper_print_buffer2,"%s%s",buf,err_bits_buffer);
                    sal_strcpy(err_bits_buffer,helper_print_buffer2);

                    if ((t1 = ((error_constant | *entry_data++) ^ expected_value) & ((((uint32)1)<<remainig_table_bits_len) - 1))) {
                        /* coverity[callee_ptr_arith] */
                        SHR_BITCOUNT_RANGE((uint32 *)&t1, t2, 0, remainig_table_bits_len);
                        nof_error_bits += t2;
                    }
                }

                if ( nof_error_bits>0 ) {
                    cli_out("SER:  %s,%d, %d,%d,%d,%d,%08x,%s,%s,%d,%d\n",SOC_MEM_NAME(unit,mem),block_instance, soc_mem_index_count(unit, mem),soc_mem_entry_bits(unit, mem),i,entry_index,expected_value, err_print_buffer,err_bits_buffer,nof_error_bits,ser_mem_read->current_read_iteration); 
                    table_nof_error_bits += nof_error_bits;
                    ser_mem_read->num_of_err_bits += nof_error_bits ;
                    ser_mem_read->num_of_err_entries += 1;
                }
            }

            ser_mem_read->num_of_bits +=  table_num_of_bits; 
            ser_mem_read->num_of_entries += (memp->index_max + 1 - memp->index_min);
        }
    }
    if (table_nof_error_bits) {
        cli_out("==> %u/%u bits failed in %s\n",table_nof_error_bits, num_of_insts*table_num_of_bits*numels, SOC_MEM_NAME(unit,mem));
    }
   return 0;
}


/**
 * 
 * 
 * @author elem (09/11/2014)
 *  the function create a bitmap of tested mems
 *  the function get as an input an array of mem names
 *  if the array in not empty(nof_requested_mems!=0) translate
 *  each name in array into its correspnding id(allmem.h), for
 *  each successful name translation set  corresponding entry in
 *  the bit map entry to 1.
 *  if if the array in empty(nof_requested_mems==0)
 *  use a default memory list to run on
 * @param unit 
 * @param nof_requested_mems - num of requested mem for the test
 * @param requested_mems  - list of requested mems names
 * @param tested_mems 
 * 
 * @return STATIC int 
 */
STATIC int mark_tested_mems(int unit,int nof_requested_mems, char **requested_mems,uint8 *tested_mems)
{
    int i;
    soc_mem_t mem;
    const soc_mem_t *mems = SOC_IS_ARAD(unit) ? arad_default_tested_mems_list : fe1600_default_tested_mems_list;

    int len=(SOC_IS_ARAD(unit) ? sizeof(  arad_default_tested_mems_list) : sizeof(fe1600_default_tested_mems_list))/sizeof(soc_mem_t); 
    if (nof_requested_mems>0) {

        for (i=0;i<nof_requested_mems;i++) {
            mem = ser_mem_name_to_id(unit,requested_mems[i]);
            if ((mem == INVALIDm)) {
                return -1;
            }
            tested_mems[mem] = 1;
        }

        return 0;
    }

    for (i=0;i<len;i++) {
        tested_mems[mems[i]] = 1;
    }
    return 0;
}

/**
 * 
 * 
 * @author elem (06/10/2014)
 * 
 * @param unit 
 * @param time_units  -  what is the time units to use: 1 - sec
 *                    0 - usec
 * @param sleep_time  -  sleep time between write and read 
 *                    memories
 * @param num_of_iterations  how many times to run the 
 *                           write-read operation
 * 
 * @param num_of_read_iterations how many times to run the read
 *                           operation after single  write
 *                           operation
 * @param num_of_patterns num of patterns to use 
 * @param patterns  patterns list ,each pattern use for writing 
 *                  all memories than reading back all memories
 *                  and test if
 * @param error_constant use to fake errors  (ored with each
 *                       reading word and test against used
 *                       pattern
 * @return int 
 *  
 * Example 
 *cint_reset(); 
 *int unit=0; 
 *int sleep_time=1; 
 *int num_of_iterations=1; 
 *int num_of_read_iterations=1; 
 *int patterns[] =  {0x5a5a5a5a,0xffffffff,0x0,0x12345678}; 
 *int num_of_patterns = sizeof(patterns)/4; 
 *char *mems[] = {"IQM_VS_QB_QSZ"}; 
 *int num_of_mems = sizeof(mems)/4; 
 *uint8 sleep_unit_in_seconds=1; uint32 error_constant=0x1; 
 *print  diag_mems_ser(unit,  sleep_unit_in_seconds, sleep_time, num_of_iterations, num_of_read_iterations,num_of_patterns, patterns, num_of_mems, mems, error_constant);
 */

static int diag_mems_ser(int unit, 
                         uint8 time_units,               /*units of time to use in sleep 1-seconds 0-usecs*/
                         int sleep_time,                 /* amount of time units to wait between write mems and read mems operations*/
                         int num_of_iterations,          /* nof iteration to perform the write-read loop*/
                         int num_of_write_iterations,    /* nof iteration to perform the write op before reading*/
                         int num_of_read_iterations,     /* nof iteration to perform read after write operation*/
                         int num_of_patterns,            /*nof patterns to use in the test*/
                         int *patterns,                  /* the patterns list  used for the test*/
                         int nof_requested_mems,         /* num of memories use for the test in case we want to check specific mems*/
                         char **requested_mems ,         /*list of memories we want to test in case nof_requested_mems>0*/
                         uint32 error_constant           /* error constant in case we want to simulate errors*/
                         )
{
    int i,j,k;
    ser_mem_write_t ser_mem_write ;
    ser_mem_read_t ser_mem_read ;
    ser_mem_init_t ser_mem_init ={0, 0};
    uint32 *pattern_replication;
    sal_time_t start_time = sal_time();
    unsigned secs, minutes, hours,days;
    uint8 tested_mems[NUM_SOC_MEM] ;

    /*  init and disable dynamic mems in case we start the test. if num_of_write_iterations==0 its mean we continue previous test so we skip it*/
    if (num_of_write_iterations>0) {
        if (init_device_for_mems_ser_test(unit, &ser_mem_read)<0) {
            return -1;
        }
    }


    /*  initiate the requested mem list for test*/
    sal_memset(tested_mems, 0, sizeof (tested_mems));
    if (mark_tested_mems(unit, nof_requested_mems,requested_mems,tested_mems)==-1){
        return -1;

    }
    ser_mem_init.tested_mems = tested_mems;
    ser_mem_read.tested_mems = tested_mems;
    ser_mem_write.tested_mems = tested_mems;


    /* calc max table size in words for dma allocation */
    if ((soc_mem_iterate(unit,
                         ser_mem_init_cb, &ser_mem_init)) < 0) {
        return -1;
    }

    /* Dma mem Allocation */
    if ((ser_mem_read.mem_ptr =soc_cm_salloc(unit, sizeof(uint32) * ser_mem_init.max_table_size, "ser test read buffer")) == NULL) {
        return -1;
    }
    /*
    if ((ser_mem_read.mem_ptr = soc_cm_salloc(unit, sizeof(uint32) * ser_mem_init.max_table_size, "Jericho memory DMA access check")) == NULL) {
        return -1;
    } 
    */
     
    if ((pattern_replication = sal_alloc(sizeof(uint32) * ser_mem_init.max_entry_size, "")) == NULL) {
        return -1;
    }

    ser_mem_read.patterns = pattern_replication;
    ser_mem_write.patterns = pattern_replication;
    ser_mem_read.error_constant = error_constant;

    ser_mem_read.num_of_err_entries = 0;
    ser_mem_read.num_of_err_bits = 0;
    ser_mem_read.num_of_mem_err_bits = 0;
    ser_mem_read.num_of_entries = 0;
    ser_mem_read.num_of_bits = 0;

    for (i=0; i < ((num_of_iterations>1)?num_of_iterations:1); i++) {
        cli_out("start iteration %d\n",i);
        for (j=0; j < num_of_patterns;j++) {


            for (k=0; k < ser_mem_init.max_entry_size;k++) {
                pattern_replication[k] = patterns[j];
            }

            /* perform write memories loop requiered times */
            for (k=0; k< num_of_write_iterations;k++) {
                /* loop for writing all mems*/
                if ((soc_mem_iterate(unit,
                                     ser_mem_write_cb, &ser_mem_write)) < 0) {
                    return -1;
                }
            }

            /* delay before read*/
            time_units==1 ? sal_sleep(sleep_time) : sal_usleep(sleep_time);

            /* perform read memories loop requiered times */
            for (k=0; k< num_of_read_iterations;k++) {
                /* loop for reading all mems*/
                ser_mem_read.current_read_iteration = k+1;
                if ((soc_mem_iterate(unit, 
                                     ser_mem_read_and_compare_cb, &ser_mem_read)) < 0) {
                    return -1;
                }
            }




        }

    }
    /*  deinit and enable dynamic mems */
    secs = sal_time() - start_time;
    minutes = secs / 60;
    hours = minutes / 60;
    days = hours / 24;
    cli_out("Ser Test finished: execution time: days(%u) hours(%u) minutes(%u) seconds(%u)\n",days,hours % 24,minutes % 60, secs % 60);
    cli_out("total bits tested %u\n",ser_mem_read.num_of_bits);
    cli_out("number of failed bits %u\n",ser_mem_read.num_of_err_bits);
    cli_out("number of bits tested in one pattern. %u\n",ser_mem_read.num_of_bits/num_of_patterns);

    sal_free(pattern_replication);
    soc_cm_sfree(unit, ser_mem_read.mem_ptr);

    return 0;
}
#endif /* BCM_PETRA_SUPPORT */


#if defined(INCLUDE_INTR)
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         rx_los_dump,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         rx_los_dump_port,
                         int,int,unit,0,0,
                         bcm_port_t, bcm_port_t, port,0,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         rx_los_handle,
                         int,int,unit,0,0,
                         bcm_port_t ,bcm_port_t ,port,0,0);

CINT_FWRAPPER_CREATE_RP4(int ,int, 0,0,
                         rx_los_port_enable,
                         int, int, unit, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         int, int, enable, 0, 0,
                         int, int, warm_boot, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int ,int, 0,0,
                         rx_los_port_stable,
                         int, int, unit, 0, 0,
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         int, int, timeout, 0, 0,
                         rx_los_state_t *, rx_los_state_t, state, 1, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         rx_los_unit_attach,
                         int,int,unit,0,0,
                         pbmp_t,pbmp_t,pbmp,0,0,
                         int, int, warm_boot, 0, 0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         rx_los_set_config,
                         uint32, uint32, short_sleep_usec, 0, 0,
                         uint32, uint32, long_sleep_usec, 0, 0,
                         uint32, uint32, allowed_retries, 0, 0,
                         uint32, uint32, priority, 0, 0,
                         int, int, link_down_count_max, 0, 0,
                         int, int, link_down_cycle_time, 0, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         rx_los_set_active_sleep_config,
                         uint32,uint32,active_sleep_usec,0,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         rx_los_unit_detach,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         rx_los_sw_unit_detach,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         rx_los_exit_thread);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         rx_los_register,
                         int,int,unit,0,0,
                         rx_los_callback_t ,rx_los_callback_t ,callback,0,0);

#endif /* defined(INCLUDE_INTR) */

#if defined(BCM_PETRA_SUPPORT)
CINT_FWRAPPER_CREATE_RP11(int, int, 0, 0,
                         diag_mems_ser,
                         int,int,unit,0,0,
                         uint8,uint8,sleep_unit_in_seconds,0,0,
                         int,int,sleep_time,0,0,
                         int,int,num_of_iterations,0,0,
                         int,int,num_of_write_iterations,0,0,
                         int,int,num_of_read_iterations,0,0,
                         int,int,num_of_patterns,0,0,
                         int*,int,patterns,1,0,
                         int,int,nof_requested_mems,0,0,
                         char**,char,requested_mems,2,0,
                         uint32,uint32,error_constatnt,0,0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         diag_test_mc,
                         int,int,unit,0,0,
                         char*,char,test_name,1,0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         diag_l3_param,
                         int,int,unit,0,0,
                         char*,char,test_name,1,0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         diag_test_mc2,
                         int,int,unit,0,0,
                         char*,char,test_name,1,0,
                         int,int,param,0,0);
#endif /* BCM_PETRA_SUPPORT */

#if defined(BCM_PETRA_SUPPORT)
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0, 
                         _bcm_petra_stat_counter_lif_counting_get,\
                         int, int, unit, 0,0, CINT_PARAM_IN,
                         uint32,uint32, flags, 0, 0,CINT_PARAM_IN,
                         bcm_stat_counter_source_t*, bcm_stat_counter_source_t, source, 1, 0,CINT_PARAM_IN,
                         bcm_stat_counter_lif_mask_t*, bcm_stat_counter_lif_mask_t, counting_mask, 1, 0,CINT_PARAM_IN,
                         bcm_stat_counter_lif_stack_id_t*, bcm_stat_counter_lif_stack_id_t, lif_stack_id_to_count, 1, 0,CINT_PARAM_OUT);
#endif

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0, 
                         diag_dcmn_phy_measure_port, 
                         int, int, unit, 0,0, 
                         bcm_port_t, bcm_port_t, port, 0, 0,
                         bcm_stat_val_t, bcm_stat_val_t, val, 0 , 0,
                         uint32*, uint32, rate_int,1, 0,
                         uint32*, uint32, rate_remainder,1, 0);



static cint_function_t __cint_diag_functions[] =
{
#if defined(INCLUDE_INTR)
    CINT_FWRAPPER_ENTRY(rx_los_dump),
    CINT_FWRAPPER_ENTRY(rx_los_dump_port),
    CINT_FWRAPPER_ENTRY(rx_los_handle),
    CINT_FWRAPPER_ENTRY(rx_los_port_enable),
    CINT_FWRAPPER_ENTRY(rx_los_port_stable),
    CINT_FWRAPPER_ENTRY(rx_los_unit_attach),
    CINT_FWRAPPER_ENTRY(rx_los_unit_detach),
    CINT_FWRAPPER_ENTRY(rx_los_set_config),
    CINT_FWRAPPER_ENTRY(rx_los_set_active_sleep_config),
    CINT_FWRAPPER_ENTRY(rx_los_sw_unit_detach),
    CINT_FWRAPPER_ENTRY(rx_los_exit_thread),
    CINT_FWRAPPER_ENTRY(rx_los_register),
#endif
#if defined(BCM_PETRA_SUPPORT)
    CINT_FWRAPPER_ENTRY(_bcm_petra_stat_counter_lif_counting_get),
#endif

    CINT_FWRAPPER_ENTRY(diag_dcmn_phy_measure_port),

#if defined(BCM_PETRA_SUPPORT)
    CINT_FWRAPPER_ENTRY(diag_test_mc),
    CINT_FWRAPPER_ENTRY(diag_test_mc2),
    CINT_FWRAPPER_ENTRY(diag_mems_ser),
    CINT_FWRAPPER_ENTRY(diag_l3_param),
#endif /* BCM_PETRA_SUPPORT */

    CINT_ENTRY_LAST
};


#if defined(INCLUDE_INTR)
static cint_enum_map_t __cint_diag_enum_map__rx_los_state_t[] =
{
  { "rx_los_state_ideal_state",     rx_los_state_ideal_state },
  { "rx_los_state_no_signal",       rx_los_state_no_signal },
  { "rx_los_state_no_signal_active",rx_los_state_no_signal_active },
  { "rx_los_state_rx_seq_change",   rx_los_state_rx_seq_change },
  { "rx_los_state_sleep_one",       rx_los_state_sleep_one },
  { "rx_los_state_restart",         rx_los_state_restart },
  { "rx_los_state_sleep_two",       rx_los_state_sleep_two },
  { "rx_los_state_link_up_check",   rx_los_state_link_up_check },
  { "rx_los_state_long_sleep",      rx_los_state_long_sleep },
  { "rx_los_states_count",          rx_los_states_count },
  { NULL }
};
#endif /* defined(INCLUDE_INTR)*/

static cint_enum_type_t __cint_diag_enums[] =
{
#if defined(INCLUDE_INTR)
  { "rx_los_state_t", __cint_diag_enum_map__rx_los_state_t },
#endif /* defined(INCLUDE_INTR) */
  { NULL }
};

static cint_constants_t __cint_diag_constants[] =
{
   { NULL }
};

static cint_function_pointer_t __cint_diag_function_pointers[2];

#if defined(INCLUDE_INTR)
static int
__cint_fpointer__rx_los_callback_t(int unit,
                                   bcm_port_t port,
                                   rx_los_state_t state)
{
    int returnVal;
    cint_interpreter_callback(__cint_diag_function_pointers+0,
                              3,
                              0,
                              &unit,
                              &port,
                              &state,
                              &returnVal);
    return returnVal;
}

static cint_parameter_desc_t __cint_parameters__rx_los_callback_t[] =
{
    {
        "void"
        "",
        0,
        0
    },
    {
        "int",
        "unit",
        0,
        0
    },
    {
        "bcm_port_t",
        "port",
        0,
        0
    },
    {
        "rx_los_state_t",
        "state",
        0,
        0
    },
    CINT_ENTRY_LAST
};
#endif /* defined(INCLUDE_INTR)*/
static cint_function_pointer_t __cint_diag_function_pointers[] =
{
#if defined(INCLUDE_INTR)
    {
        "rx_los_callback_t",
        (cint_fpointer_t) __cint_fpointer__rx_los_callback_t,
        __cint_parameters__rx_los_callback_t
    },
#endif /* && defined(INCLUDE_INTR)*/
    CINT_ENTRY_LAST
};

cint_data_t dcmn_diag_cint_data =
{
    NULL,
    __cint_diag_functions,
    NULL,
    __cint_diag_enums,
    NULL,
    __cint_diag_constants,
    __cint_diag_function_pointers
};

#else
typedef int _dcmn_diag_cint_data_not_empty; /* Make ISO compilers happy. */
#endif /* defined(INCLUDE_LIB_CINT) && (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) */

